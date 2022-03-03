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
#include "pmsis/drivers/i2c.h"
#include "i2c_internal.h"


//#define USE_TIMEOUT 1


i2c_itf_data_t *__pi_i2c_itf_data[UDMA_NB_I2C];


static int __pi_i2c_prepare_write_cmd_buf(i2c_slave_data_t *slave_data, uint32_t *write_buffer,
    int size, pi_i2c_xfer_flags_e flags);

static int __pi_i2c_prepare_read_cmd_buf(i2c_slave_data_t *slave_data, uint32_t *write_buffer,
    int size, pi_i2c_xfer_flags_e flags);

static int __pi_i2c_prepare_dual_cmd_buf(i2c_slave_data_t *slave_data, uint32_t *write_buffer,
    int size, pi_i2c_xfer_flags_e flags);

static int __pi_i2c_prepare_write_read_buf(i2c_slave_data_t *slave_data,
    uint32_t *buffer, int size0, int size1);

static int __pi_i2c_prepare_write_dual_buf(i2c_slave_data_t *slave_data,
    uint32_t *buffer, int size0, int size1);


static void __pi_i2c_write_exec(i2c_slave_data_t *slave_data, uint8_t *buffer, int size,
    pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    uint16_t slave_addr = slave_data->slave_addr;
    i2c_itf_data_t *itf_data = slave_data->itf_data;

#if defined(USE_TIMEOUT)
    // TODO this does not take into account errors. Timeout should be restarted with proper time
    uint32_t timeout_us = task->timeout;
    if (timeout_us)
    {
        __pi_i2c_timeout_config_set(task, slave_data->itf_data->tx_timeout_id,
                                    slave_data->itf_data->tx_chan_id, timeout_us,
                                    __pi_i2c_timeout_abort, slave_data->itf_data);
    }
#endif
    int cmd_buf_size = __pi_i2c_prepare_write_cmd_buf(slave_data, itf_data->cmd_buf,
        size, flags);
    pi_udma_core_lin_enqueue(itf_data->tx_chan_addr, (uint32_t)buffer, size, 0);
    pi_udma_core_lin_enqueue(itf_data->cmd_chan_addr, (uint32_t)itf_data->cmd_buf,
            cmd_buf_size*sizeof(uint32_t), 0);
}


static void __pi_i2c_read_exec(i2c_slave_data_t *slave_data, uint8_t *buffer, int size,
    pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    uint16_t slave_addr = slave_data->slave_addr;
    i2c_itf_data_t *itf_data = slave_data->itf_data;

#if defined(USE_TIMEOUT)
    uint32_t timeout_us = task->timeout;
    if (timeout_us)
    {
        __pi_i2c_timeout_config_set(task, slave_data->itf_data->rx_timeout_id,
                                    slave_data->itf_data->rx_chan_id, timeout_us,
                                    __pi_i2c_timeout_abort, slave_data->itf_data);
    }
#endif
    int cmd_buf_size = __pi_i2c_prepare_read_cmd_buf(slave_data, itf_data->cmd_buf, size,
        flags);
    pi_udma_core_lin_enqueue(itf_data->rx_chan_addr, (uint32_t)buffer, size, 0);
    pi_udma_core_lin_enqueue(itf_data->cmd_chan_addr, (uint32_t)itf_data->cmd_buf,
            cmd_buf_size*sizeof(uint32_t), 0);
}


static void __pi_i2c_write_read_exec(i2c_slave_data_t *slave_data, void *tx_buffer,
    void *rx_buffer, uint32_t tx_size, uint32_t rx_size, pi_task_t *task)
{
    uint16_t slave_addr = slave_data->slave_addr;
    i2c_itf_data_t *itf_data = slave_data->itf_data;
    int8_t bits = slave_data->is_10_bits;

    pi_udma_core_lin_enqueue(itf_data->tx_chan_addr, (uint32_t)tx_buffer, tx_size, 0);
    pi_udma_core_lin_enqueue(itf_data->rx_chan_addr, (uint32_t)rx_buffer, rx_size, 0);

    int cmd_buf_size = __pi_i2c_prepare_write_read_buf(slave_data, itf_data->cmd_buf,
        tx_size, rx_size);
    pi_udma_core_lin_enqueue(itf_data->cmd_chan_addr, (uint32_t)itf_data->cmd_buf,
        sizeof(i2c_cmd_t)*cmd_buf_size, 0);
}

static void __pi_i2c_write_dual_exec(i2c_slave_data_t *slave_data, void *tx_buffer0,
    void *tx_buffer1, uint32_t tx_size0, uint32_t tx_size1, pi_task_t *task)
{
    uint16_t slave_addr = slave_data->slave_addr;
    i2c_itf_data_t *itf_data = slave_data->itf_data;

    pi_udma_core_lin_enqueue(itf_data->tx_chan_addr, (uint32_t)tx_buffer0, tx_size0, 0);
    pi_udma_core_lin_enqueue(itf_data->tx_chan_addr, (uint32_t)tx_buffer1, tx_size1, 0);
    int cmd_buf_size = __pi_i2c_prepare_write_dual_buf(slave_data, itf_data->cmd_buf,
        tx_size0, tx_size1);
    pi_udma_core_lin_enqueue(itf_data->cmd_chan_addr, (uint32_t)itf_data->cmd_buf,
            sizeof(i2c_cmd_t)*cmd_buf_size, 0);
}


static inline void __pi_i2c_send_request_from_irq(i2c_itf_data_t* itf_data, pi_task_t* task)
{
    int cmd_buf_size;

    pi_device_t *device = (pi_device_t *)task->data[3];
    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    if(task->data[0] == I2C_WRITE)
    {
        __pi_i2c_write_exec(slave_data, (void*) task->data[1], task->data[2], task->data[4], task);
    }
    else if(task->data[0] == I2C_READ)
    {
        __pi_i2c_read_exec(slave_data, (void*) task->data[1], task->data[2], task->data[4], task);
    }
    else if(task->data[0] == I2C_WRITE_READ)
    {
        __pi_i2c_write_read_exec(slave_data, (void*)task->data[1], (void *)task->data[2],
            task->data[4], task->data[5], task);
    }
    else if(task->data[0] == I2C_WRITE_DUAL)
    {
        __pi_i2c_write_dual_exec(slave_data, (void*)task->data[1], (void *)task->data[2],
            task->data[4], task->data[5], task);
    }
}

static inline void __pi_i2c_handle_error(int device_id, i2c_itf_data_t* itf_data)
{
    I2C_TRACE("I2C(%d)->lead_error_handler\n", device_id);
    uint32_t nack = __pi_i2c_get_event_status(itf_data->base, I2C_STATUS_ERROR_NACK_EVENT);
    uint32_t arlo = __pi_i2c_get_event_status(itf_data->base, I2C_STATUS_ERROR_ARLO_EVENT);
    uint32_t framing = __pi_i2c_get_event_status(itf_data->base, I2C_STATUS_ERROR_FRAMING_EVENT);
    if (nack || arlo || framing)
    {
        // 1) stop cmd, rx and tx leader udma addr gen
        pi_udma_core_lin_stop(itf_data->rx_chan_addr);
        pi_udma_core_lin_stop(itf_data->tx_chan_addr);
        pi_udma_core_lin_stop(itf_data->cmd_chan_addr);

        // 2) clear event, unlock and purge
        udma_i2c_status_reg_idx_set(itf_data->base,
                (nack << I2C_STATUS_ERROR_NACK_EVENT) |
                (arlo << I2C_STATUS_ERROR_ARLO_EVENT) |
                (framing << I2C_STATUS_ERROR_FRAMING_EVENT) |
                (1 << I2C_FLAG_UNLOCK_EVENT_O) |
                (1 << I2C_FLAG_PURGE_EVENT_O));
    }

    // 3) depends on error:
    if (nack)
    {
        I2C_TRACE_ERR("I2C(%d)->lead_error_handler - nack error\n", device_id);
        // NACK => report error
        itf_data->end_task->data[0] = PI_ERR_I2C_NACK;
        itf_data->end_task->arg[3] = 0;
        pi_task_push_irq_safe(itf_data->end_task);
        itf_data->end_task = NULL;
    }
    else if (arlo || framing)
    {
        I2C_TRACE_ERR("I2C(%d)->lead_error_handler - arbitration loss or framing error\n", device_id);
        // ARLO and FRAMING error => restart current CMD buffer
        __pi_i2c_send_request_from_irq(itf_data, itf_data->end_task);
    }
}

__attribute__((section(".text"))) __noinline
void __pi_i2c_lead_event_handler(uint32_t event, void *arg)
{
    i2c_itf_data_t *itf_data = arg;
    int device_id = itf_data->id;
    I2C_TRACE("I2C(%d)->lead_event_handler\n", device_id);

    // I2C_FLAG_CMD_EVENT is basically our EOT?
    if(__pi_i2c_get_event_status(itf_data->base,I2C_FLAG_CMD_EVENT_I))
    {
        // set the return status to OK
        itf_data->end_task->data[0] = PI_OK;
        itf_data->end_task->arg[3] = 0;
        // if it's a mutex, release on the spot
        pi_task_push_irq_safe(itf_data->end_task);
        itf_data->end_task = NULL;
        udma_i2c_status_reg_idx_set(itf_data->base, 1<<I2C_FLAG_CMD_EVENT_I);
    }
    else
    {
        // There was an error
        __pi_i2c_handle_error(device_id, itf_data);
        return;
    }
    pi_task_t *next_task = __pi_i2c_drv_fifo_pop(itf_data);
    if(next_task)
    {
        __pi_i2c_send_request_from_irq(itf_data, next_task);
        itf_data->end_task = next_task;
    }
    return;
}

/**
 * \brief internal helper function for preparing write command buffer
 */
static int __pi_i2c_prepare_write_cmd_buf(i2c_slave_data_t *slave_data, uint32_t *buffer,
    int size, pi_i2c_xfer_flags_e flags)
{
    uint16_t slave_addr = slave_data->slave_addr;
    int index = 0;

    if(!(flags & PI_I2C_XFER_NO_START) || (flags & PI_I2C_XFER_RESTART))
    {   // generate a start condition
        buffer[index++]  = slave_data->cfg;
        buffer[index++]  = I2C_CMD_LEAD_START(1);
    }
    // slave addr, no rnw bit since it's a write
    if(!slave_data->is_10_bits)
    {
        buffer[index++] = I2C_CMD_LEAD_SEND_IMM_ADDR(0, slave_addr & 0xFE);
    }
    else
    {
        slave_addr = slave_addr & 0x3FF;
        uint16_t slave_addrh = (((slave_addr>>7)|0)&0x7) | 0xF0;
        uint16_t slave_addrl = (slave_addr&0xFF);
        buffer[index++] = I2C_CMD_LEAD_SEND_IMM_ADDR(1, (slave_addrh << 8)
                | slave_addrl);
    }

    buffer[index++]  = I2C_CMD_RPT(size);
    buffer[index++]  = I2C_CMD_MISC_SEND(1);
    if(!(flags & PI_I2C_XFER_NO_STOP))
    {
        buffer[index++] = I2C_CMD_STOP(1);
    }

    if(slave_data->wait_cycles)
    {
        buffer[index++] = I2C_CMD_RPT(slave_data->wait_cycles);
        buffer[index++] = I2C_CMD_MISC_WAIT(1);
    }

    buffer[index++] = I2C_CMD_EVENT(1);

    return index;
}

/**
 * \brief internal helper function for preparing read command buffer
 */
static inline int __pi_i2c_prepare_read_cmd_buf(i2c_slave_data_t *slave_data, uint32_t *buffer,
    int size, pi_i2c_xfer_flags_e flags)
{
    uint16_t slave_addr = slave_data->slave_addr;
    int index = 0;

    if(!(flags & PI_I2C_XFER_NO_START) || (flags & PI_I2C_XFER_RESTART))
    {
        buffer[index++]  = slave_data->cfg;
        buffer[index++]  = I2C_CMD_LEAD_START(1);
    }

    // slave addr + rnw bit
    if(!slave_data->is_10_bits)
    {
        buffer[index++]  = I2C_CMD_LEAD_SEND_IMM_ADDR(0, (slave_addr &0xFE)|1);
    }
    else
    {   // for 10 bits, need to use write mode first
        slave_addr = slave_addr & 0x3FF;
        uint16_t slave_addrh = (((slave_addr>>7)|0)&0x7) | 0xF0;
        uint16_t slave_addrl = (slave_addr&0xFF);
        buffer[index++] = I2C_CMD_LEAD_SEND_IMM_ADDR(1, (slave_addrh << 8)
                | slave_addrl);
        buffer[index++] = I2C_CMD_LEAD_START(1);
        buffer[index++] = I2C_CMD_LEAD_SEND_IMM(slave_addrh|1);
    }
    buffer[index++] = I2C_CMD_RPT(size - 1);
    // receive -1 byte because there is a "last"
    buffer[index++] = I2C_CMD_MISC_RECEIVE(1);
    buffer[index++] = I2C_CMD_MISC_RECEIVE_LAST(1);

    if(!(flags & PI_I2C_XFER_NO_STOP))
    {
        buffer[index++] = I2C_CMD_STOP(1);
    }

    if(slave_data->wait_cycles)
    {
        buffer[index++] = I2C_CMD_RPT(slave_data->wait_cycles);
        buffer[index++] = I2C_CMD_MISC_WAIT(1);
    }
    buffer[index++] = I2C_CMD_EVENT(1);

    return index;
}


/**
 * \brief internal helper function for preparing write&read command buffer
 */
static int __pi_i2c_prepare_write_read_buf(i2c_slave_data_t *slave_data,
    uint32_t *buffer, int size0, int size1)
{
    int index = 0;

    buffer[index++]  = slave_data->cfg;
    buffer[index++]  = I2C_CMD_LEAD_START(1);
    // slave addr, no rnw bit since it's a write
    buffer[index++] = I2C_CMD_LEAD_SEND_IMM_ADDR(0, slave_data->slave_addr);
    buffer[index++] = I2C_CMD_RPT(size0);
    buffer[index++] = I2C_CMD_MISC_SEND(1);
    buffer[index++] = I2C_CMD_LEAD_START(1);
    // slave addr + rnw bit
    if(!slave_data->is_10_bits)
    {
        buffer[index++]  = I2C_CMD_LEAD_SEND_IMM_ADDR(0, slave_data->slave_addr|1);
    }
    else
    {// for 10 bits, need to use write mode first
        buffer[index++] = I2C_CMD_LEAD_SEND_IMM_ADDR(1, slave_data->slave_addr);
        buffer[index++] = I2C_CMD_LEAD_START(1);
        buffer[index++] = I2C_CMD_LEAD_SEND_IMM(slave_data->slave_addrh|1);
    }
    buffer[index++] = I2C_CMD_RPT(size1-1);
    // receive -1 byte because there is a "last"
    buffer[index++] = I2C_CMD_MISC_RECEIVE(1);
    buffer[index++] = I2C_CMD_MISC_RECEIVE_LAST(1);
    buffer[index++] = I2C_CMD_STOP(1);
    buffer[index++] = I2C_CMD_EVENT(1);

    return index;
}


/**
 * \brief internal helper function for preparing write&read command buffer
 */
static int __pi_i2c_prepare_write_dual_buf(i2c_slave_data_t *slave_data,
    uint32_t *buffer, int size0, int size1)
{
    int index = 0;

    buffer[index++]  = slave_data->cfg;
    buffer[index++]  = I2C_CMD_LEAD_START(1);
    // slave addr, no rnw bit since it's a write
    buffer[index++] = I2C_CMD_LEAD_SEND_IMM_ADDR(0, slave_data->slave_addr);
    buffer[index++] = I2C_CMD_RPT(size0);
    buffer[index++] = I2C_CMD_MISC_SEND(1);
    buffer[index++] = I2C_CMD_RPT(size1);
    buffer[index++] = I2C_CMD_MISC_SEND(1);
    buffer[index++] = I2C_CMD_STOP(1);
    buffer[index++] = I2C_CMD_EVENT(1);

    return index;
}


static void __pi_i2c_timestamp_enable(i2c_itf_data_t *itf_data, struct pi_i2c_conf *conf)
{
#if defined(__FREERTOS__)
    uint8_t is_rx = conf->ts_ch;

    uint32_t base = UDMA_CTRL_ADDR;
    uint8_t evt_id = conf->ts_evt_id; 
    uint8_t soc_evt = is_rx ? SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id) : SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id); 

    uint32_t cfg_evt_val = (udma_ctrl_cfg_event_get(base) & ~(0xFF<<evt_id*8))| (soc_evt << (evt_id*8));

    pi_soc_eu_pr_mask_set(soc_evt);
    udma_ctrl_cfg_event_set(base, cfg_evt_val);
#endif
}

/*
 * @brief Internal open of i2c interface
 * Caller must provide synchronization
 */
int pi_i2c_open(pi_device_t *device)
{
    pi_assert((NULL != device) && (NULL != device->config));

    i2c_slave_data_t *slave_data;
    struct pi_i2c_conf *conf = device->config;
    i2c_itf_data_t *itf_data = NULL;

    // check interface first
    I2C_TRACE("I2C(%d)->lead_open\n", conf->itf);
    int irq = disable_irq();

    slave_data = pi_fc_l1_malloc(sizeof(i2c_slave_data_t));
    if (slave_data == NULL) goto error0;

    if(!(itf_data = __pi_i2c_itf_data[conf->itf]))
    {
        uint32_t i2c_base = UDMA_I2C_ADDR(conf->itf);

        // prepare itf struct
        itf_data = pi_fc_l1_malloc(sizeof(i2c_itf_data_t));
        I2C_TRACE("I2C(%d)->itf_data=%x\n", conf->itf, itf_data);
        if(itf_data == NULL) goto error1;

        itf_data->rx_chan_id = pi_udma_core_lin_alloc();
        if (itf_data->rx_chan_id == -1) goto error2;
        itf_data->tx_chan_id = pi_udma_core_lin_alloc();
        if (itf_data->tx_chan_id == -1) goto error3;
        itf_data->cmd_chan_id = pi_udma_core_lin_alloc();
        if (itf_data->cmd_chan_id == -1) goto error4;

        __pi_i2c_itf_data[conf->itf] = itf_data;
        itf_data->id = conf->itf;
        itf_data->fifo_head = NULL;
        itf_data->end_task = NULL;
        itf_data->base = i2c_base;
        itf_data->open_nb = 0;

        // disable udma reset before setting regs
        udma_ctrl_cfg_rstn_set_set(UDMA_CTRL_ADDR, 1 << UDMA_I2C_ID(conf->itf));
        udma_ctrl_cfg_cg_set_set(UDMA_CTRL_ADDR, 1 << UDMA_I2C_ID(conf->itf));

        udma_i2c_lead_udma_rx_dest_reg_idx_set(i2c_base, itf_data->rx_chan_id);
        udma_i2c_lead_udma_tx_dest_reg_idx_set(i2c_base, itf_data->tx_chan_id);
        udma_i2c_udma_cmd_dest_reg_idx_set(i2c_base, itf_data->cmd_chan_id);

        itf_data->rx_chan_addr = pi_udma_core_lin_addr_get(itf_data->rx_chan_id);
        itf_data->tx_chan_addr = pi_udma_core_lin_addr_get(itf_data->tx_chan_id);
        itf_data->cmd_chan_addr = pi_udma_core_lin_addr_get(itf_data->cmd_chan_id);

#if defined(USE_TIMEOUT)
        itf_data->rx_timeout_id = 0xFF;
        itf_data->tx_timeout_id = 0xFF;
#endif

        udma_i2c_status_reg_idx_set(i2c_base, 1<<I2C_FLAG_PRESC_DIV10_EVENT_O);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_LEAD_EVT(conf->itf),
                __pi_i2c_lead_event_handler, itf_data);
        pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_I2C_LEAD_EVT(conf->itf));
    }

    slave_data->itf_data = itf_data;
    slave_data->is_10_bits = conf->is_10_bits;
    slave_data->wait_cycles = conf->wait_cycles;
    slave_data->cfg = __i2c_prepare_timing(conf->max_baudrate, pi_freq_get(PI_FREQ_DOMAIN_PERIPH));

    if (conf->is_10_bits)
    {
        uint16_t slave_addr = conf->cs & 0x3FF;
        uint16_t slave_addrh = (((slave_addr>>7)|0)&0x7) | 0xF0;
        uint16_t slave_addrl = (slave_addr&0xFF);
        slave_data->slave_addr = (slave_addrh << 8) | slave_addrl;
        slave_data->slave_addrh = slave_addrh;
    }
    else
    {
        slave_data->slave_addr = conf->cs & 0xFE;
    }

    device->data = (void *)slave_data;

    itf_data->open_nb++;
    restore_irq(irq);
    return 0;

error4:
    pi_udma_core_lin_free(itf_data->tx_chan_id);
error3:
    pi_udma_core_lin_free(itf_data->rx_chan_id);
error2:
    pi_fc_l1_free(itf_data, sizeof(i2c_itf_data_t));
error1:
    pi_fc_l1_free(slave_data, sizeof(i2c_slave_data_t));
error0:
    restore_irq(irq);
    return -1;
}


void pi_i2c_close(pi_device_t *device)
{
    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    I2C_TRACE("I2C->i2c_close\n");
    int irq = disable_irq();
    i2c_itf_data_t *itf_data = slave_data->itf_data;
    pi_fc_l1_free(slave_data,sizeof(*slave_data));

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

        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_I2C_LEAD_EVT(itf_data->id));

        udma_ctrl_cfg_cg_clr_set(UDMA_CTRL_ADDR, 1 << UDMA_I2C_ID(itf_data->id));
        udma_ctrl_cfg_rstn_clr_set(UDMA_CTRL_ADDR, 1 << UDMA_I2C_ID(itf_data->id));
        __pi_i2c_itf_data[itf_data->id] = NULL;
        pi_fc_l1_free(itf_data,sizeof(*itf_data));
    }
    restore_irq(irq);
}


static void __pi_i2c_baudrate_set(i2c_itf_data_t *driver_data)
{
}


#if defined(USE_TIMEOUT)
void __pi_i2c_timeout_abort(void* arg)
{
    i2c_itf_data_t *driver_data = (i2c_itf_data_t*) arg;
    uint32_t device_id = driver_data->id;
    int irq = pi_irq_disable();
    /* Stop UDMA channels. */
    udma_core_lin_t *udma_core = NULL;
    if (driver_data->cmd_chan_id != 0xFF)
    {
        pi_udma_core_lin_reset(driver_data->rx_chan_addr);
    }
    if (driver_data->tx_chan_id != 0xFF)
    {
        if (driver_data->end_task->data[0] == I2C_WRITE)
        {
            driver_data->end_task->arg[3] = pi_udma_core_lin_bytes_left_get(driver_data->base);
        }
        pi_udma_core_lin_reset(driver_data->tx_chan_addr);
    }
    if (driver_data->rx_chan_id != 0xFF)
    {
        if (driver_data->end_task->data[0] == I2C_READ)
        {
            driver_data->end_task->arg[3] = pi_udma_core_lin_bytes_left_get(driver_data->base);
        }
        pi_udma_core_lin_reset(driver_data->rx_chan_addr);
    }

    /* Status events clear. */
    uint32_t status_mask = (1 << UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_BIT |
                            1 << UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_BIT |
                            1 << UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_BIT);
    udma_i2c_status_reg_idx_set(driver_data->base, status_mask);

    /* Pop current aborted task. */
    driver_data->end_task = NULL;
    pi_task_t *next_task = __pi_i2c_drv_fifo_pop(driver_data);
    if (next_task)
    {
        driver_data->end_task = next_task;
        __pi_i2c_send_request_from_irq(driver_data, next_task);
    }
    pi_irq_restore(irq);
}

static void __pi_i2c_udma_timeout_rx_set(i2c_itf_data_t *driver_data,
                                         uint8_t timeout_id)
{
    driver_data->rx_timeout_id = timeout_id;
}

static void __pi_i2c_udma_timeout_tx_set(i2c_itf_data_t *driver_data,
                                         uint8_t timeout_id)
{
    driver_data->tx_timeout_id = timeout_id;
}
#endif

void pi_i2c_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    I2C_TRACE("I2C(%d) : ioctl cmd=%lx, arg=%lx\n", slave_data->itf_data->id, cmd, arg);

    uint32_t irq = disable_irq();
    uint8_t udma_timeout_id = 0xFF;
    switch (cmd)
    {
    case PI_I2C_CTRL_SET_MAX_BAUDRATE :
        __pi_i2c_baudrate_set(slave_data->itf_data);
        break;

    case PI_I2C_IOCTL_ABORT_RX :
        __pi_i2c_timeout_abort(slave_data->itf_data);
        break;

    case PI_I2C_IOCTL_ABORT_TX :
        __pi_i2c_timeout_abort(slave_data->itf_data);
        break;

    case PI_I2C_IOCTL_ATTACH_TIMEOUT_RX :
#if defined(USE_TIMEOUT)
        udma_timeout_id = (uint32_t) arg;
        __pi_i2c_udma_timeout_rx_set(slave_data->itf_data, udma_timeout_id);
#endif
        break;

    case PI_I2C_IOCTL_DETACH_TIMEOUT_RX :
#if defined(USE_TIMEOUT)
        __pi_i2c_udma_timeout_rx_set(slave_data->itf_data, udma_timeout_id);
#endif
        break;

    case PI_I2C_IOCTL_ATTACH_TIMEOUT_TX :
#if defined(USE_TIMEOUT)
        udma_timeout_id = (uint32_t) arg;
        __pi_i2c_udma_timeout_tx_set(slave_data->itf_data, udma_timeout_id);
#endif
        break;

    case PI_I2C_IOCTL_DETACH_TIMEOUT_TX :
#if defined(USE_TIMEOUT)
        __pi_i2c_udma_timeout_tx_set(slave_data->itf_data, udma_timeout_id);
#endif
        break;

    case PI_I2C_IOCTL_EN_TIMESTAMP : 
        __pi_i2c_timestamp_enable(slave_data->itf_data, (struct pi_i2c_conf *) arg);
        break;

    default :
        break;
    }
    restore_irq(irq);
}


void pi_i2c_conf_init(pi_i2c_conf_t *conf)
{
    pi_assert(NULL != conf);

    conf->max_baudrate = 400000;
    conf->itf = 0;
    conf->cs = 0;
    conf->wait_cycles = 0;
    conf->is_10_bits = 0;
}

void pi_i2c_conf_set_wait_cycles(struct pi_i2c_conf *conf, uint16_t wait_cycles)
{
    pi_assert(NULL != conf);

    conf->wait_cycles = wait_cycles;
}

/** accessors **/
void pi_i2c_conf_set_slave_addr(struct pi_i2c_conf *conf, uint16_t slave_addr,
                                int8_t is_10_bits)
{
    pi_assert(NULL != conf);

    conf->cs = slave_addr;
    conf->is_10_bits = is_10_bits;
}

int pi_i2c_write(struct pi_device *device, uint8_t *tx_data, int length, pi_i2c_xfer_flags_e flags)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    pi_i2c_write_async(device, (void*)tx_data, (uint32_t)length, flags, &task_block);
    pi_task_wait_on(&task_block);
    return pi_i2c_get_request_status(&task_block);
}

int pi_i2c_read (struct pi_device *device, uint8_t *rx_buff, int length,
                 pi_i2c_xfer_flags_e flags)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    pi_i2c_read_async(device, (void*)rx_buff, (uint32_t)length, flags, &task_block);
    pi_task_wait_on(&task_block);
    return pi_i2c_get_request_status(&task_block);
}

void pi_i2c_read_async(struct pi_device *device, uint8_t *buffer, int size,
                       pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    pi_assert(NULL != device);
    pi_assert((NULL != rx_buff) && (IS_BUFF_IN_L2(rx_buff)));
    pi_assert(0 != length);
    pi_assert(NULL != task);

    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    i2c_itf_data_t *itf_data = slave_data->itf_data;
    uint16_t slave_addr = slave_data->slave_addr;

    task->data[0] = I2C_READ;
    task->data[1] = (uintptr_t)buffer;
    task->data[2] = (uintptr_t)size;
    task->data[3] = (uintptr_t)device;
    task->data[4] = (uintptr_t)flags;

    int irq = disable_irq();
    if (!itf_data->end_task)
    {  // exec transfer
        __pi_i2c_read_exec(slave_data, buffer, size, flags, task);
        itf_data->end_task = task;
    }
    else
    {
        __pi_i2c_drv_fifo_enqueue(itf_data, task);
    }
    restore_irq(irq);
}

void pi_i2c_write_async(struct pi_device *device, uint8_t *buffer, int size,
                        pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
    pi_assert(NULL != device);
    pi_assert((NULL != tx_data) && (IS_BUFF_IN_L2(tx_data)));
    pi_assert(0 != length);
    pi_assert(NULL != task);

    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    int irq = disable_irq();
    i2c_itf_data_t *itf_data = slave_data->itf_data;

    task->data[0] = I2C_WRITE;
    task->data[1] = (uintptr_t)buffer;
    task->data[2] = (uintptr_t)size;
    task->data[3] = (uintptr_t)device;
    task->data[4] = (uintptr_t)flags;

    if(!itf_data->end_task)
    {   // exec transfer
        __pi_i2c_write_exec(slave_data, buffer, size, flags, task);
        itf_data->end_task = task;
    }
    else
    {
        __pi_i2c_drv_fifo_enqueue(itf_data, task);
    }
    restore_irq(irq);
}

int pi_i2c_read_timeout(struct pi_device *device, uint8_t *rx_buff, int length,
                        pi_i2c_xfer_flags_e flags, uint32_t timeout_us)
{
    pi_assert(NULL != device);
    pi_assert((NULL != rx_buff) && (IS_BUFF_IN_L2(rx_buff)));
    pi_assert(0 != length);

    pi_task_t task_block = {0};
    pi_task_block(&task_block);
#if defined(USE_TIMEOUT)
    pi_task_timeout_set(&task_block, timeout_us);
#endif
    pi_i2c_read_async(device, rx_buff, length, flags, &task_block);
    pi_task_wait_on(&task_block);
    int status = pi_task_status_get(&task_block);
    //return ((status == -1) ? -1 : 0);
    return status;
}


int pi_i2c_write_timeout(struct pi_device *device, uint8_t *tx_data, int length,
                         pi_i2c_xfer_flags_e flags, uint32_t timeout_us)
{
    pi_assert(NULL != device);
    pi_assert((NULL != tx_data) && (IS_BUFF_IN_L2(tx_data)));
    pi_assert(0 != length);

    pi_task_t task_block = {0};
    pi_task_block(&task_block);
#if defined(USE_TIMEOUT)
    pi_task_timeout_set(&task_block, timeout_us);
#endif
    pi_i2c_write_async(device, tx_data, length, flags, &task_block);
    pi_task_wait_on(&task_block);
    int status = pi_task_status_get(&task_block);
    //return ((status == -1) ? -1 : 0);
    return status;
}

int pi_i2c_get_request_status(pi_task_t* task)
{
    pi_assert(NULL != task);

    return (int) (task->data[0]);
}


void pi_i2c_write_read(struct pi_device *device, void *tx_buffer,
                       void *rx_buffer, uint32_t tx_size, uint32_t rx_size)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    pi_i2c_write_read_async(device, tx_buffer, rx_buffer, tx_size, rx_size, &task_block);
    pi_task_wait_on(&task_block);
}


void pi_i2c_write_read_async(struct pi_device *device, void *tx_buffer,
                             void *rx_buffer, uint32_t tx_size, uint32_t rx_size,
                             pi_task_t *task)
{
    pi_assert(NULL != device);
    pi_assert((NULL != tx_buffer) && (IS_BUFF_IN_L2(tx_buffer)));
    pi_assert((NULL != rx_buffer) && (IS_BUFF_IN_L2(rx_buffer)));
    pi_assert(0 != tx_size);
    pi_assert(0 != rx_size);
    pi_assert(NULL != task);

    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    int irq = disable_irq();
    i2c_itf_data_t *itf_data = slave_data->itf_data;

    task->data[0] = I2C_WRITE_READ;
    task->data[1] = (uintptr_t)tx_buffer;
    task->data[2] = (uintptr_t)rx_buffer;
    task->data[3] = (uintptr_t)device;
    task->data[4] = (uintptr_t)tx_size;
    task->data[5] = (uintptr_t)rx_size;

    if(!itf_data->end_task)
    {
        __pi_i2c_write_read_exec(slave_data, tx_buffer, rx_buffer, tx_size, rx_size, task);
        itf_data->end_task = task;
    }
    else
    {
        __pi_i2c_drv_fifo_enqueue(itf_data, task);
    }
    restore_irq(irq);
}

void pi_i2c_write_dual_async(struct pi_device *device, void *tx_buffer0,
                             void *tx_buffer1, uint32_t tx_size0, uint32_t tx_size1,
                             pi_task_t *task)
{
    pi_assert(NULL != device);
    pi_assert((NULL != tx_buffer0) && (IS_BUFF_IN_L2(tx_buffer0)));
    pi_assert((NULL != tx_buffer1) && (IS_BUFF_IN_L2(tx_buffer1)));
    pi_assert(0 != tx_size0);
    pi_assert(0 != tx_size1);
    pi_assert(NULL != task);

    i2c_slave_data_t *slave_data = (i2c_slave_data_t *)device->data;

    int irq = disable_irq();
    i2c_itf_data_t *itf_data = slave_data->itf_data;

    task->data[0] = I2C_WRITE_DUAL;
    task->data[1] = (uintptr_t)tx_buffer0;
    task->data[2] = (uintptr_t)tx_buffer1;
    task->data[3] = (uintptr_t)device;
    task->data[4] = (uintptr_t)tx_size0;
    task->data[5] = (uintptr_t)tx_size1;

    if(!itf_data->end_task)
    {
        __pi_i2c_write_dual_exec(slave_data, tx_buffer0, tx_buffer1, tx_size0, tx_size1, task);
        itf_data->end_task = task;
    }
    else
    {
        __pi_i2c_drv_fifo_enqueue(itf_data, task);
    }
    restore_irq(irq);
}

