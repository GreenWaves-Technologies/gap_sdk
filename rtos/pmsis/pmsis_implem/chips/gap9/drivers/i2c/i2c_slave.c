/*
 * Copyright (C) 2022 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"
#include "i2c_internal.h"
#include "pmsis/drivers/i2c_slave.h"
#include "i2c_slave_internal.h"


/** Internal defines **/
#define I2C_SLAVE_ADDR0 0
#define I2C_SLAVE_ADDR1 1
#define I2C_ADDR_PUSH_DISABLE 0
#define I2C_ADDR_PUSH_ENABLE  1

struct i2c_slave_itf_data *__global_i2c_slave_itf_data[UDMA_NB_I2C];


static void __pi_i2c_slave_handle_error(struct i2c_slave_itf_data *itf_data)
{
    I2C_TRACE("I2C_slave(%d)->slave_error_handler\n", device_id);
    // I2C_STATUS_FOLL_ERROR_ARLO_EVENT & I2C_STATUS_FOLL_ERROR_FRAMING_EVENT
    // => clear status, unlock and purge
    int device_id = itf_data->id;
    uint32_t arlo = __pi_i2c_get_event_status(itf_data->base, I2C_STATUS_FOLL_ERROR_ARLO_EVENT);
    uint32_t framing = __pi_i2c_get_event_status(itf_data->base, I2C_STATUS_FOLL_ERROR_FRAMING_EVENT);
    if (arlo || framing)
    {
        I2C_TRACE("I2C_slave(%d)->slave_error_handler - arbitration loss or framing error\n", device_id);
        udma_i2c_status_reg_idx_set(itf_data->base,
                (arlo << I2C_STATUS_FOLL_ERROR_ARLO_EVENT) |
                (framing << I2C_STATUS_FOLL_ERROR_FRAMING_EVENT) |
                (1 << I2C_FLAG_FOLL_UNLOCK_EVENT_O) |
                (1 << I2C_FLAG_FOLL_PURGE_EVENT_O));
    }
}

__attribute__((section(".text"))) __noinline
void __pi_i2c_slave_event_handler(uint32_t event, void *arg)
{
    struct i2c_slave_itf_data *itf_data = arg;
    int device_id = itf_data->id;
    I2C_TRACE("I2C_slave(%d)->slave_event_handler\n", device_id);

    // even for tx, there is a pseudo rx for addr matcher byte
    uint8_t *l2_buffer = itf_data->rx_buffer;

    uint8_t addr_byte = *l2_buffer;
    uint8_t match = addr_byte >> 6;
    l2_buffer++;

    struct pi_i2c_slave_args slave_args = {
        .slave_addr = itf_data->addr[match],
        .handle = arg,
        .ret = PI_FAIL, // if callee does not fill it, consider it a failure
        .itf_id = device_id,
    };

    // First check whether we're here because of read or send
    // then let the user supplied callback execute
    // User will have the responsibility of unlocking the udma once callback
    // is done
    if(__pi_i2c_get_event_status(itf_data->base, I2C_STATUS_FOLL_ERROR_ARLO_EVENT)
            ||__pi_i2c_get_event_status(itf_data->base, I2C_STATUS_FOLL_ERROR_FRAMING_EVENT))
    {
        // TODO should we execute callback if there is an error ?
        __pi_i2c_slave_handle_error(itf_data);
    }
    if(__pi_i2c_get_event_status(itf_data->base, I2C_STATUS_FOLL_EOF_RCV_EVENT))
    {
        uint32_t bytes_left = pi_udma_core_lin_bytes_left_get(itf_data->rx_chan_addr);
        uint32_t size = itf_data->rx_buffer_size - bytes_left;
        slave_args.nb_bytes = size - 1; // remove push byte
        slave_args.l2_buffer = l2_buffer; // take buffer minus first byte
        if(itf_data->rx_callback)
        {
            itf_data->rx_callback(&slave_args);
        }
    }
    if(__pi_i2c_get_event_status(itf_data->base,
            I2C_STATUS_FOLL_EOF_SND_EVENT))
    {
        uint32_t bytes_left = pi_udma_core_lin_bytes_left_get(itf_data->tx_chan_addr);
        uint32_t size = itf_data->tx_buffer_size - bytes_left;
        slave_args.nb_bytes = size;
        slave_args.l2_buffer = itf_data->tx_buffer;
        if(itf_data->tx_callback)
        {
            itf_data->tx_callback(&slave_args);
        }
    }
    return;
}

/*
 * @brief Internal open of i2c interface
 * Caller must provide synchronization
 */
int pi_i2c_slave_open(struct pi_device *device)
{
    pi_assert((NULL != device) && (NULL != device->config));
    struct pi_i2c_slave_conf *conf = device->config;

    struct i2c_slave_itf_data *itf_data = NULL;
    // check interface first
    I2C_TRACE("I2C_slave(%d)->open_slave\n", conf->itf);
    int irq = disable_irq();
    if(!(itf_data = __global_i2c_slave_itf_data[conf->itf]))
    {
        uint32_t i2c_base = UDMA_I2C_ADDR(conf->itf);

        // prepare itf struct
        itf_data = pi_fc_l1_malloc(sizeof(struct i2c_slave_itf_data));
        I2C_TRACE("I2C_slave(%d)->itf_data=%x\n", conf->itf, itf_data);
        if(itf_data == NULL)
        {
            restore_irq(irq);
            return -1;
        }
        __global_i2c_slave_itf_data[conf->itf] = itf_data;
        memset(itf_data,0,sizeof(struct i2c_slave_itf_data));
        itf_data->id = conf->itf;
        itf_data->base = i2c_base;

        // disable udma reset before setting regs
        udma_ctrl_cfg_rstn_set_set(UDMA_CTRL_ADDR, 1 << UDMA_I2C_ID(conf->itf));
        udma_ctrl_cfg_cg_set_set(UDMA_CTRL_ADDR, 1 << UDMA_I2C_ID(conf->itf));

        itf_data->rx_chan_id = pi_udma_core_lin_alloc();
        I2C_TRACE("I2C(%d)->rx chan id = %x\n", conf->itf, itf_data->rx_chan_id);
        itf_data->tx_chan_id = pi_udma_core_lin_alloc();
        itf_data->cmd_chan_id = pi_udma_core_lin_alloc();
        udma_i2c_udma_cmd_dest_reg_idx_set(i2c_base, itf_data->cmd_chan_id);

        itf_data->rx_chan_addr = pi_udma_core_lin_addr_get(itf_data->rx_chan_id);
        itf_data->tx_chan_addr = pi_udma_core_lin_addr_get(itf_data->tx_chan_id);
        itf_data->cmd_chan_addr = pi_udma_core_lin_addr_get(itf_data->cmd_chan_id);

        // Master init procedure
        int cmd_buf_id = 0;
        uint32_t cmd_buf[8];

        udma_i2c_status_reg_idx_set(i2c_base, 1<<I2C_FLAG_PRESC_DIV10_EVENT_O);
        cmd_buf[cmd_buf_id++] = __i2c_prepare_timing(conf->max_baudrate,
                pi_freq_get(PI_FREQ_DOMAIN_PERIPH));
        if(conf->addr0 != 0)
        {
            itf_data->addr[0] = conf->addr0;
            cmd_buf[cmd_buf_id++] = CMD_FOLL_ADDR(I2C_SLAVE_ADDR0,
                    I2C_ADDR_PUSH_ENABLE, conf->addr0_10_bit, conf->addr0,
                    conf->mask0, conf->sof0, conf->eof0);
        }
        if(conf->addr1 != 0)
        {
            itf_data->addr[1] = conf->addr1;
            cmd_buf[cmd_buf_id++] = CMD_FOLL_ADDR(I2C_SLAVE_ADDR1,
                    I2C_ADDR_PUSH_ENABLE, conf->addr1_10_bit, conf->addr1,
                    conf->mask1, conf->sof1, conf->eof1);
        }
        cmd_buf[cmd_buf_id++] = I2C_CMD_EVENT(1);
        pi_udma_core_lin_enqueue(itf_data->cmd_chan_addr, (uint32_t)cmd_buf,
                cmd_buf_id*sizeof(uint32_t), 0);

        while(!__pi_i2c_get_event_status(i2c_base,I2C_FLAG_CMD_EVENT_I))
        {
            pi_time_wait_us(1);
        }
        udma_i2c_status_reg_idx_set(i2c_base, 1<<I2C_FLAG_CMD_EVENT_I);

        udma_i2c_foll_udma_rx_dest_reg_idx_set(i2c_base, itf_data->rx_chan_id);
        udma_i2c_foll_udma_tx_dest_reg_idx_set(i2c_base, itf_data->tx_chan_id);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_SLAVE_EVT(conf->itf),
                __pi_i2c_slave_event_handler, itf_data);
        pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_I2C_SLAVE_EVT(conf->itf));

        itf_data->rx_callback = conf->rx_callback;
        itf_data->tx_callback = conf->tx_callback;

    }
    device->data = itf_data;
    itf_data->open_nb++;
    restore_irq(irq);
    return 0;
}

void pi_i2c_slave_close(struct pi_device *device)
{
    pi_assert(NULL != device);

    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)device->data;

    int irq = disable_irq();
    itf_data->open_nb--;
    if(itf_data->open_nb == 0)
    {
        // flush channels
        pi_udma_core_lin_free(itf_data->rx_chan_id);
        pi_udma_core_lin_free(itf_data->tx_chan_id);
        pi_udma_core_lin_free(itf_data->cmd_chan_id);

        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id));
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id));
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->cmd_chan_id));

        pi_udma_core_lin_reset(itf_data->rx_chan_addr);
        pi_udma_core_lin_reset(itf_data->tx_chan_addr);
        pi_udma_core_lin_reset(itf_data->cmd_chan_addr);

        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_I2C_SLAVE_EVT(itf_data->id));

        // disable udma reset before setting regs
        udma_ctrl_cfg_cg_clr_set(ARCHI_UDMA_ADDR, 1 << UDMA_I2C_ID(itf_data->id));
        udma_ctrl_cfg_rstn_clr_set(ARCHI_UDMA_ADDR, 1 << UDMA_I2C_ID(itf_data->id));
        __global_i2c_slave_itf_data[itf_data->id] = NULL;
        pi_fc_l1_free(itf_data,sizeof(itf_data));
    }
    restore_irq(irq);
}


void pi_i2c_slave_conf_init(pi_i2c_slave_conf_t *conf)
{
    pi_assert(NULL != conf);

    conf->max_baudrate = 400000;
    conf->itf = 0;
    conf->addr0 = 0;
    conf->addr1 = 0;
    conf->sof0 = 0;
    conf->eof0 = 0;
    conf->sof1 = 0;
    conf->eof1 = 0;
    conf->addr0_10_bit = 0;
    conf->addr1_10_bit = 0;
    conf->mask0 = 0x1F;
    conf->mask1 = 0x1F;
    conf->addr0 = 0;
    conf->addr1 = 0;
    conf->rx_callback = NULL;
    conf->tx_callback = NULL;
}


/** accessors **/

void pi_i2c_slave_conf_set_addr0(struct pi_i2c_slave_conf *conf, uint16_t addr,
                                 uint8_t mask, uint8_t is_10_bit, uint8_t eof, uint8_t sof)
{
    pi_assert(NULL != conf);

    conf->addr0 = addr;
    conf->mask0 = mask;
    conf->addr0_10_bit = is_10_bit;
    conf->eof0 = eof;
    conf->sof0 = sof;
}

void pi_i2c_slave_conf_set_addr1(struct pi_i2c_slave_conf *conf, uint16_t addr,
                                 uint8_t mask, uint8_t is_10_bit, uint8_t eof, uint8_t sof)
{
    pi_assert(NULL != conf);

    conf->addr1 = addr;
    conf->mask1 = mask;
    conf->addr1_10_bit = is_10_bit;
    conf->eof1 = eof;
    conf->sof1 = sof;
}

void pi_i2c_slave_set_rx_channel(void *handle,
                                 void *l2_addr, uint32_t size)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != itf_data);
    pi_assert((NULL != l2_addr) && (IS_BUFF_IN_L2(l2_addr)));
    pi_assert(0 != size);

    int irq = disable_irq();
    pi_udma_core_lin_enqueue(itf_data->rx_chan_addr, (uint32_t)l2_addr, size, 0);
    itf_data->rx_buffer = l2_addr;
    itf_data->rx_buffer_size = size;
    restore_irq(irq);
}

void pi_i2c_slave_set_tx_channel(void *handle,
                                 void *l2_addr, uint32_t size)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != itf_data);
    pi_assert((NULL != l2_addr) && (IS_BUFF_IN_L2(l2_addr)));
    pi_assert(0 != size);

    int irq = disable_irq();
    pi_udma_core_lin_enqueue(itf_data->tx_chan_addr, (uint32_t)l2_addr, size, 0);
    itf_data->tx_buffer = l2_addr;
    itf_data->tx_buffer_size = size;
    restore_irq(irq);
}

void pi_i2c_slave_unlock(void *handle, int is_rd)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != handle);

    int device_id = itf_data->id;
    if(is_rd)
    {
        udma_i2c_status_reg_idx_set(itf_data->base,(1<<I2C_FLAG_FOLL_UNLOCK_EVENT_O)
                | (1 << I2C_FLAG_FOLL_PURGE_EVENT_O)
                |(1 << I2C_STATUS_FOLL_EOF_RCV_EVENT));
    }
    else
    {
        udma_i2c_status_reg_idx_set(itf_data->base,(1<<I2C_FLAG_FOLL_UNLOCK_EVENT_O) |
            (1 << I2C_FLAG_FOLL_PURGE_EVENT_O) | (1<<I2C_STATUS_FOLL_EOF_SND_EVENT));
    }
}

void pi_i2c_slave_set_rx(void *handle, void *l2_addr,
                         uint32_t size)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != itf_data);
    pi_assert((NULL != l2_addr) && (IS_BUFF_IN_L2(l2_addr)));
    pi_assert(0 != size);

    int irq = disable_irq();
    pi_udma_core_lin_enqueue(itf_data->rx_chan_addr, (uint32_t)l2_addr, size, 0);
    itf_data->rx_buffer = l2_addr;
    itf_data->rx_buffer_size = size;
    restore_irq(irq);
}

void pi_i2c_slave_set_tx(void *handle, void *l2_addr,
                         uint32_t size)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != itf_data);
    pi_assert((NULL != l2_addr) && (IS_BUFF_IN_L2(l2_addr)));
    pi_assert(0 != size);

    int irq = disable_irq();
    pi_udma_core_lin_enqueue(itf_data->tx_chan_addr, (uint32_t)l2_addr, size, 0);
    itf_data->tx_buffer = l2_addr;
    itf_data->tx_buffer_size = size;
    restore_irq(irq);
}

void pi_i2c_slave_stop_rx(void *handle)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != itf_data);

    int irq = disable_irq();
    pi_udma_core_lin_stop(itf_data->rx_chan_addr);
    restore_irq(irq);
}

void pi_i2c_slave_stop_tx(void *handle)
{
    struct i2c_slave_itf_data *itf_data = (struct i2c_slave_itf_data *)handle;

    pi_assert(NULL != itf_data);

    int irq = disable_irq();
    pi_udma_core_lin_stop(itf_data->tx_chan_addr);
    restore_irq(irq);
}
