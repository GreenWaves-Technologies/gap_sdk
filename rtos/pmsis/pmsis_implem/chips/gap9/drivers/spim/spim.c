/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#include <stdint.h>
#include <string.h>
#include "pmsis.h"

#include "spim_v4.h"
#include "chips/gap9/drivers/udma/udma_core.h"
#include "pmsis/task.h"

#if !defined(__FREERTOS__)
#define SOC_EVENT_UDMA_SPIM_EOT(id)        (80 + id + 0)
#else
#define likely(x)                          (__builtin_expect(x, 1))
#endif  /* __FREERTOS__ */

#ifndef SPIM_TRACE
#if !defined(__TRACE_ALL__) && !defined(__TRACE_SPIM__)
#define SPIM_TRACE(x...)
#else
#define SPIM_TRACE(level, x...) POS_TRACE(level, "[SPIM] " x)
#endif
#endif

#define PI_SPIM_UDMA_CMD_SIZE 4

typedef struct
{
    uint32_t *temp_buff;
    uint32_t addr;
    uint32_t size;
    uint32_t end;
} pi_spim_pending_transfert_t;


typedef struct
{
    uint32_t udma_cmd[PI_SPIM_UDMA_CMD_SIZE];
    uint32_t temp_buff[2];
} pi_spim_l2_t;

typedef struct
{
    pi_task_t *pending_copy;
    pi_task_t *waiting_first;
    pi_task_t *waiting_last;
    unsigned int pending_repeat_base;
    unsigned int pending_repeat_callback;
    unsigned int pending_repeat_asm_callback;
    unsigned int pending_repeat_misaligned_size;
    unsigned int pending_repeat_misaligned_addr;
    unsigned int pending_repeat_misaligned_ram_addr;
    unsigned int pending_repeat_misaligned_end;
    unsigned int pending_repeat_misaligned_length;
    unsigned int pending_repeat_misaligned_stride;
    unsigned int pending_repeat_misaligned_2d_size;
    unsigned int pending_chunk_size;
    unsigned int pending_send_cmd;
    unsigned int pending_eot;
    void (*pending_callback)(uint32_t event, void *arg);
    uint32_t udma_cmd[PI_SPIM_UDMA_CMD_SIZE];
    int open_count;
    int id;
    int is_slave;
    pi_task_t task;
    unsigned int pending_repeat_addr;
    unsigned int pending_repeat_dup_addr;
    unsigned int pending_repeat_send;
    unsigned int pending_repeat_flags;
    pi_spim_pending_transfert_t pending_transfers[2];
    int pending_transfer_index;
    int pending_transfer_read_index;
    struct pi_device *pending_repeat_device;
    uint32_t periph_base;
    uint32_t rx_cmd;
    uint32_t tx_cmd;
    int channel;
    int pending_is_auto;
    uint32_t cmd_channel_base;
    uint32_t tx_channel_base;
    uint32_t rx_channel_base;
    int cmd_channel;
    int tx_channel;
    int rx_channel;
} pi_spim_t;

#define PI_SPIM_T_PENDING_COPY          0
#define PI_SPIM_T_WAITING_FIRST         4
#define PI_SPIM_T_WAITING_LAST          8
#define PI_SPIM_T_REPEAT_BASE           12
//#define PI_SPIM_T_REPEAT_LEN            16
#define PI_SPIM_T_REPEAT_CALLBACK       16
#define PI_SPIM_T_REPEAT_ASM_CALLBACK   20

typedef struct
{
    pi_spim_t *spim;
    uint32_t rx_cmd;
    uint32_t tx_cmd;
    uint8_t *receive_addr_ucode;
    uint32_t receive_addr_ucode_size;
    uint8_t *send_addr_ucode;
    uint32_t send_addr_ucode_size;
    uint32_t *udma_receive_cmd;
    uint32_t *udma_send_cmd;
    uint32_t udma_receive_cmd_size;
    uint32_t udma_send_cmd_size;
    int max_baudrate;
    unsigned int cfg;
    unsigned int periph_base;
    char cs;
    char wordsize;
    char big_endian;
    char channel;
    char byte_align;
    unsigned char div;
    char polarity;
    char phase;
    uint32_t max_rcv_size;
    uint32_t max_snd_size;
} pi_spim_cs_t;

typedef struct {
    unsigned int cmd[4];
} pi_spim_cmd_t;


static PI_L2 pi_spim_t g_spim_data[ARCHI_UDMA_NB_SPIM];

void pi_spim_handle_waiting_copy(pi_task_t *task);

void pi_spim_handle_eot(uint32_t event, void *arg)
{
    pi_spim_t *spim = (pi_spim_t *) arg;

    pi_task_t *task = spim->pending_copy;
    spim->pending_copy = NULL;

    /* handle current task end */
    pi_task_push_irq_safe(task);

    task = spim->waiting_first;
    if (task)
    {
        spim->waiting_first = task->next;
        pi_spim_handle_waiting_copy(task);
    }
}

void pi_spim_handle_rx_copy(uint32_t event, void *arg)
{
    pi_soc_eu_fc_mask_clear(event);
    pi_spim_handle_eot(event, arg);
}

void pi_spim_handle_tx_copy(uint32_t event, void *arg)
{
    pi_soc_eu_fc_mask_clear(event);
    pi_spim_handle_eot(event, arg);
}


static int pi_spim_get_div(uint32_t spi_freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);

    if (spi_freq >= periph_freq)
    {
        return 0;
    }
    else
    {
        // Round-up the divider to obtain an SPI frequency which is below the maximum
        int div = (periph_freq + spi_freq - 1)/ spi_freq;
        // The SPIM always divide by 2 once we activate the divider, thus increase by 1
        // in case it is even to not go above the max frequency.
        if (div & 1) div += 1;
        return div;
    }
}



static inline int pi_spim_get_byte_align(int wordsize, int big_endian)
{
    return wordsize == PI_SPI_WORDSIZE_32 && big_endian;
}

static void pi_spim_apply_conf(pi_spim_cs_t *spim_cs)
{
    if (spim_cs->udma_receive_cmd)
    {
        spim_cs->udma_receive_cmd[0] = spim_cs->cfg;
        spim_cs->udma_receive_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
    }

    if (spim_cs->udma_send_cmd)
    {
        spim_cs->udma_send_cmd[0] = spim_cs->cfg;
        spim_cs->udma_send_cmd[1] = SPI_CMD_SOT(spim_cs->cs);
    }

    spim_cs->rx_cmd = SPI_CMD_RX_DATA(1, SPI_CMD_4_WORD_PER_TRANSF, 8, 0, 0);
    spim_cs->tx_cmd = SPI_CMD_TX_DATA(1, SPI_CMD_4_WORD_PER_TRANSF, 8, 0, 0);
}

int pi_spi_open(struct pi_device *device)
{
    uint32_t irq = pi_irq_disable();

    struct pi_spi_conf *conf = (struct pi_spi_conf *) device->config;

    int periph_id = ARCHI_UDMA_SPIM_ID(conf->itf);

    SPIM_TRACE(POS_LOG_INFO, "Opening SPIM device (device: %p, id: %d, cs: %d, max_baudrate: %d, wordsize: %d, big_endian: %d, polarity: %d, phase: %d)\n",
               device, conf->itf, conf->cs, conf->max_baudrate, conf->wordsize, conf->big_endian, conf->polarity, conf->phase);

    pi_spim_t *spim = &g_spim_data[conf->itf];

    pi_spim_cs_t *spim_cs = pmsis_l2_malloc(sizeof(pi_spim_cs_t));
    if (spim_cs == NULL)
    {
        SPIM_TRACE(POS_LOG_WARNING, "Failed to allocate spim structure\n");
        goto error;
    }

    device->data = (void *) spim_cs;

    spim_cs->channel = periph_id;
    spim_cs->periph_base = (uint32_t) UDMA_SPIM_ADDR(conf->itf);
    spim_cs->spim = spim;
    spim_cs->wordsize = conf->wordsize;
    spim_cs->big_endian = conf->big_endian;
    spim_cs->polarity = conf->polarity;
    spim_cs->phase = conf->phase;
    spim_cs->max_baudrate = conf->max_baudrate;
    spim_cs->cs = conf->cs;
    spim_cs->byte_align = pi_spim_get_byte_align(conf->wordsize, conf->big_endian);
    spim_cs->max_rcv_size = conf->max_rcv_chunk_size;
    spim_cs->max_snd_size = conf->max_snd_chunk_size;
    spim_cs->udma_send_cmd = NULL;
    spim_cs->udma_receive_cmd = NULL;

    int div = pi_spim_get_div(spim_cs->max_baudrate);
    spim_cs->div = div;

    spim_cs->cfg = SPI_CMD_CFG(div, conf->polarity, conf->phase);

    spim->open_count++;
    if (spim->open_count == 1)
    {
        spim->is_slave = conf->is_slave;

        spim->rx_channel  = pi_udma_core_lin_alloc();
        spim->tx_channel  = pi_udma_core_lin_alloc();
        spim->cmd_channel = pi_udma_core_lin_alloc();

        if (spim->rx_channel < 0 || spim->tx_channel < 0 || spim->cmd_channel < 0)
        {
            pi_udma_core_lin_free(spim->rx_channel);
            pi_udma_core_lin_free(spim->tx_channel);
            pi_udma_core_lin_free(spim->cmd_channel);

            SPIM_TRACE(POS_LOG_WARNING, "Failed to allocate channels\n");
            goto error;
        }

        spim->rx_channel_base  = pi_udma_core_lin_addr_get(spim->rx_channel);
        spim->tx_channel_base  = pi_udma_core_lin_addr_get(spim->tx_channel);
        spim->cmd_channel_base = pi_udma_core_lin_addr_get(spim->cmd_channel);


        udma_ctrl_cfg_rstn_set_set(ARCHI_UDMA_ADDR, (1 << periph_id));
        udma_ctrl_cfg_cg_set_set(ARCHI_UDMA_ADDR, (1 << periph_id));

        uint32_t event = SOC_EVENT_UDMA_SPIM_EOT(conf->itf);

        pi_fc_event_handler_set(event, pi_spim_handle_eot, (void *) spim);
        pi_soc_eu_fc_mask_set(event);

        udma_spim_rx_dest_set(spim_cs->periph_base, spim->rx_channel);
        udma_spim_tx_dest_set(spim_cs->periph_base, spim->tx_channel);
        udma_spim_cmd_dest_set(spim_cs->periph_base, spim->cmd_channel);

        udma_spim_config_set(spim_cs->periph_base, UDMA_SPIM_CONFIG_SPI_SLAVE_MODE(conf->is_slave));
    }

    pi_irq_restore(irq);

    return 0;

error:
    pi_irq_restore(irq);
    return -1;
}

static void __pi_spi_timestamp_enable(pi_spim_cs_t *spim_cs, struct pi_spi_conf *conf)
{
    pi_spim_t *spim = spim_cs->spim;
    uint8_t is_rx = conf->ts_ch;

    uint32_t base = ARCHI_UDMA_ADDR;
    uint8_t evt_id = conf->ts_evt_id;
    uint8_t soc_evt = is_rx ? spim->rx_channel : spim->tx_channel; 

    uint32_t cfg_evt_val = (udma_ctrl_cfg_event_get(base) & ~(0xFF<<evt_id*8))| (soc_evt << (evt_id*8));

    pi_soc_eu_pr_mask_set(soc_evt);
    udma_ctrl_cfg_event_set(base, cfg_evt_val);
}

void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *_arg)
{
    uint32_t irq = pi_irq_disable();
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;
    uint32_t arg = (uint32_t) _arg;

    int polarity = (cmd >> __PI_SPI_CTRL_CPOL_BIT) & 3;
    int phase = (cmd >> __PI_SPI_CTRL_CPHA_BIT) & 3;
    int set_freq = (cmd >> __PI_SPI_CTRL_SET_MAX_BAUDRATE_BIT) & 1;
    int wordsize = (cmd >> __PI_SPI_CTRL_WORDSIZE_BIT) & 3;
    int big_endian = (cmd >> __PI_SPI_CTRL_ENDIANNESS_BIT) & 3;
    int ts_spi = (cmd >> __PI_SPI_CTRL_SET_TIMESTAMP) & 1;

    if (ts_spi)
    {
        __pi_spi_timestamp_enable(spim_cs, (struct pi_spi_conf *) _arg);
    }
    else
    {
        if (set_freq)
        {
            spim_cs->max_baudrate = arg;
            spim_cs->div = pi_spim_get_div(arg);
        }

        if (polarity)
        {
            spim_cs->polarity = polarity >> 1;
        }
        if (phase)
        {
            spim_cs->phase = phase >> 1;
        }
        if (wordsize)
        {
            spim_cs->wordsize = wordsize >> 1;
        }
        if (big_endian)
        {
            spim_cs->big_endian = big_endian >> 1;
        }

        spim_cs->cfg = SPI_CMD_CFG(spim_cs->div, spim_cs->polarity, spim_cs->phase);
        spim_cs->byte_align = pi_spim_get_byte_align(spim_cs->wordsize, spim_cs->big_endian);
    }

    pi_irq_restore(irq);
}

void pi_spi_close(struct pi_device *device)
{
    int irq = pi_irq_disable();
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;
    pi_spim_t *spim = spim_cs->spim;

    SPIM_TRACE(POS_LOG_INFO, "Closing SPIM device (device: %p)\n", device);

    spim->open_count--;

    if (spim->open_count == 0)
    {
        // Deactivate SPIM channels
        udma_spim_rx_dest_set(spim_cs->periph_base, 0xFF);
        udma_spim_tx_dest_set(spim_cs->periph_base, 0xFF);
        udma_spim_cmd_dest_set(spim_cs->periph_base, 0xFF);

        // And free them
        pi_udma_core_lin_free(spim->rx_channel);
        pi_udma_core_lin_free(spim->tx_channel);
        pi_udma_core_lin_free(spim->cmd_channel);

        // Deactivate event routing
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_SPIM_EOT(spim->id));

        // Reactivate clock-gating and reset
        // Reactivate clock-gating and reset
        udma_ctrl_cfg_cg_clr_set(ARCHI_UDMA_ADDR, (1 << spim_cs->channel));
        udma_ctrl_cfg_rstn_clr_set(ARCHI_UDMA_ADDR, (1 << spim_cs->channel));
    }

    pi_l2_free(spim_cs, sizeof(pi_spim_cs_t));

    pi_irq_restore(irq);
}



static void __attribute__((noinline)) pi_spim_enqueue_to_pending(pi_spim_t *spim, pi_task_t *task, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4)
{
    task->data[0] = data0;
    task->data[1] = data1;
    task->data[2] = data2;
    task->data[3] = data3;
    task->data[4] = data4;

    if (spim->waiting_first)
    {
        spim->waiting_last->next = task;
    }
    else
    {
        spim->waiting_first = task;
    }
    spim->waiting_last = task;
    task->next = NULL;
}



static void __attribute__((noinline)) pi_spim_enqueue_to_pending_7(pi_spim_t *spim, pi_task_t *task, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4, uint32_t data5, uint32_t data6, uint32_t data7)
{
    task->data[0] = data0;
    task->data[1] = data1;
    task->data[2] = data2;
    task->data[3] = data3;
    task->data[4] = data4;
    task->data[5] = data5;
    task->data[6] = data6;
    task->data[7] = data7;

    if (spim->waiting_first)
    {
        spim->waiting_last->next = task;
    }
    else
    {
        spim->waiting_first = task;
    }
    spim->waiting_last = task;
    task->next = NULL;
}



void pi_spi_send_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
    int irq = pi_irq_disable();

    SPIM_TRACE(POS_LOG_TRACE, "Sending bitstream (device: %p, buffer: %p, len: 0x%x, flags: 0x%x, task: %p)\n", device, data, len, flags, task);

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;
    pi_spim_t *spim = spim_cs->spim;
    int qspi = ((flags >> 2) & 0x3) == 1;
    int cs_mode = (flags >> 0) & 0x3;

    if (spim->pending_copy)
    {
        task->data[0] = 0;
        task->data[1] = (int)device;
        task->data[2] = (int)data;
        task->data[3] = len;
        task->data[4] = flags;

        if (spim->waiting_first)
        {
            spim->waiting_last->next = task;
        }
        else
        {
            spim->waiting_first = task;
        }
        spim->waiting_last = task;
        task->next = NULL;

        goto end;
    }

    int buffer_size = (len+7)/8;

    spim->pending_copy = task;

    int size = (len + 7) >> 3;
    int endianness = spim_cs->big_endian ? SPI_CMD_MSB_FIRST : SPI_CMD_LSB_FIRST;

    // First enqueue the header with SPI config, cs, and send command.
    // The rest will be sent by the assembly code.
    // First the user data and finally an epilogue with the EOT command.

    pi_udma_core_lin_enqueue(spim->tx_channel_base, (uint32_t) data, buffer_size, 0);

    if (!spim->is_slave)
    {
        spim->udma_cmd[0] = spim_cs->cfg;
        spim->udma_cmd[1] = SPI_CMD_SOT(spim_cs->cs);

        if (spim_cs->wordsize == PI_SPI_WORDSIZE_8)
        {
            spim->udma_cmd[2] = SPI_CMD_TX_DATA(len/8, SPI_CMD_4_WORD_PER_TRANSF, 8, qspi, endianness);
        }
        else if (spim_cs->wordsize == PI_SPI_WORDSIZE_16)
        {
            spim->udma_cmd[2] = SPI_CMD_TX_DATA(len/16, SPI_CMD_2_WORD_PER_TRANSF, 16, qspi, endianness);
        }
        else
        {
            spim->udma_cmd[2] = SPI_CMD_TX_DATA(len/32, SPI_CMD_1_WORD_PER_TRANSF, 32, qspi, endianness);
        }
        spim->udma_cmd[3] = SPI_CMD_EOT(1, cs_mode == PI_SPI_CS_KEEP);

        pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) spim->udma_cmd, 4*4, 0);
    }
    else
    {
        spim->udma_cmd[0] = SPI_CMD_SLAVE_TX_DATA(len>>3, 0);
        spim->udma_cmd[1] = SPI_CMD_SLAVE_EOT();

        pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) spim->udma_cmd, 8, 0);
    }

end:
    pi_irq_restore(irq);
}


void pi_spi_send(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_send_async(device, data, len, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}



void pi_spi_receive_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
    SPIM_TRACE(POS_LOG_TRACE, "Receive bitstream (device: %p, buffer: %p, len: 0x%x, flags: 0x%x, task: %p)\n", device, data, len, flags, task);

    int irq = pi_irq_disable();

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
    pi_spim_t *spim = spim_cs->spim;
    int qspi = ((flags >> 2) & 0x3) == 1;
    int cs_mode = (flags >> 0) & 0x3;

    if (spim->pending_copy)
    {
        task->data[0] = 1;
        task->data[1] = (int)device;
        task->data[2] = (int)data;
        task->data[3] = len;
        task->data[4] = flags;

        if (spim->waiting_first)
        {
            spim->waiting_last->next = task;
        }
        else
        {
            spim->waiting_first = task;
        }

        spim->waiting_last = task;
        task->next = NULL;

        goto end;
    }

    spim->pending_copy = task;

    int size = (len + 7) >> 3;
    int endianness = spim_cs->big_endian ? SPI_CMD_MSB_FIRST : SPI_CMD_LSB_FIRST;

    pi_udma_core_lin_enqueue(spim->rx_channel_base, (uint32_t) data, size, 0);

    if (!spim->is_slave)
    {
        spim->udma_cmd[0] = spim_cs->cfg;
        spim->udma_cmd[1] = SPI_CMD_SOT(spim_cs->cs);

        if (spim_cs->wordsize == PI_SPI_WORDSIZE_8)
        {
            spim->udma_cmd[2] = SPI_CMD_RX_DATA(len/8, SPI_CMD_4_WORD_PER_TRANSF, 8, qspi, endianness);
        }
        else if (spim_cs->wordsize == PI_SPI_WORDSIZE_16)
        {
            spim->udma_cmd[2] = SPI_CMD_RX_DATA(len/16, SPI_CMD_2_WORD_PER_TRANSF, 16, qspi, endianness);
        }
        else
        {
            spim->udma_cmd[2] = SPI_CMD_RX_DATA(len/32, SPI_CMD_1_WORD_PER_TRANSF, 32, qspi, endianness);
        }

        spim->udma_cmd[3] = SPI_CMD_EOT(1, cs_mode == PI_SPI_CS_KEEP);

        pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) spim->udma_cmd, 4*4, 0);
    }
    else
    {
        spim->udma_cmd[0] = SPI_CMD_SLAVE_RX_DATA(len>>3, 0);
        spim->udma_cmd[1] = SPI_CMD_SLAVE_EOT();

        pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) spim->udma_cmd, 8, 0);
    }

end:
    pi_irq_restore(irq);
}

void pi_spi_receive(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_receive_async(device, data, len, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}


void pi_spi_transfer_async(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
    SPIM_TRACE(POS_LOG_TRACE, "Transfering bitstream (device: %p, tx_buffer: %p, rx_buffer: %p, len: 0x%x, flags: 0x%x, task: %p)\n", device, tx_data, rx_data, len, flags, task);

    int irq = pi_irq_disable();

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;
    pi_spim_t *spim = spim_cs->spim;
    int cs_mode = (flags >> 0) & 0x3;

    if (spim->pending_copy)
    {
        task->data[0] = 2;
        task->data[1] = (int)device;
        task->data[2] = (int)tx_data;
        task->data[3] = (int)rx_data;
        task->data[4] = len;
        task->data[5] = cs_mode;

        if (spim->waiting_first)
            spim->waiting_last->next = task;
        else
            spim->waiting_first = task;

        spim->waiting_last = task;
        task->next = NULL;

        goto end;
    }

    //int channel_id = UDMA_CHANNEL_ID(spim_cs->channel);

    int endianness = spim_cs->big_endian ? SPI_CMD_MSB_FIRST : SPI_CMD_LSB_FIRST;

    spim->pending_copy = task;

    int size = (len + 7) >> 3;

    pi_udma_core_lin_enqueue(spim->rx_channel_base, (uint32_t) rx_data, size, 0);
    pi_udma_core_lin_enqueue(spim->tx_channel_base, (uint32_t) tx_data, size, 0);

    if (!spim->is_slave)
    {
        // First enqueue the header with SPI config, cs, and send command.
        // The rest will be sent by the assembly code.
        // First the user data and finally an epilogue with the EOT command.
        spim->udma_cmd[0] = spim_cs->cfg;
        spim->udma_cmd[1] = SPI_CMD_SOT(spim_cs->cs);

        if (spim_cs->wordsize == PI_SPI_WORDSIZE_8)
        {
            spim->udma_cmd[2] = SPI_CMD_FUL(len/8, SPI_CMD_4_WORD_PER_TRANSF, 8, endianness);
        }
        else if (spim_cs->wordsize == PI_SPI_WORDSIZE_16)
        {
            spim->udma_cmd[2] = SPI_CMD_FUL(len/16, SPI_CMD_2_WORD_PER_TRANSF, 16, endianness);
        }
        else
        {
            spim->udma_cmd[2] = SPI_CMD_FUL(len/32, SPI_CMD_1_WORD_PER_TRANSF, 32, endianness);
        }

        spim->udma_cmd[3] = SPI_CMD_EOT(1, cs_mode == PI_SPI_CS_KEEP);

        pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) spim->udma_cmd, 4*4, 0);
    }
    else
    {
        spim->udma_cmd[0] = SPI_CMD_SLAVE_FUL(len>>3, 0);
        spim->udma_cmd[1] = SPI_CMD_SLAVE_EOT();

        pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) spim->udma_cmd, 8, 0);
    }

end:
    pi_irq_restore(irq);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data,
                     size_t len, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_transfer_async(device, tx_data, rx_data, len, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}

void pi_spi_copy_2d(struct pi_device *device, uint32_t addr, void *data,
                    uint32_t size, uint32_t stride, uint32_t length, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_copy_2d_async(device, addr, data, size, stride, length, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}


static __attribute__((noinline)) void pi_spim_send_enqueue_transfer(uint32_t event, void *arg)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) arg;
    pi_spim_t *spim = (pi_spim_t *) spim_cs->spim;

    uint32_t addr = spim->pending_repeat_misaligned_addr;
    uint32_t size = spim->pending_repeat_misaligned_size;
    uint32_t chunk_size = spim_cs->max_snd_size;

    uint32_t cmd_size;
    uint32_t *cmd;

    SPIM_TRACE(POS_LOG_TRACE, "Enqueueing send transfer (addr: 0x%lx, ram_addr: 0x%x, pending size: 0x%lx)\n", addr, spim->pending_repeat_misaligned_ram_addr, size);

    memcpy(spim_cs->send_addr_ucode, &spim->pending_repeat_misaligned_ram_addr, spim_cs->send_addr_ucode_size);



    cmd_size = spim_cs->udma_send_cmd_size;
    cmd = spim_cs->udma_send_cmd;

    void *callback = pi_spim_send_enqueue_transfer;

    if (size > chunk_size)
    {
        size = chunk_size;
    }

    SPIM_TRACE(POS_LOG_TRACE, "Enqueueing aligned send chunk (addr: 0x%x, size: 0x%lx)\n", spim->pending_repeat_misaligned_addr, size);

    spim->pending_repeat_misaligned_ram_addr += size;
    spim->pending_repeat_misaligned_addr += size;
    spim->pending_repeat_misaligned_size -= size;

    cmd[cmd_size++] = __BITINSERT(spim->tx_cmd, size-1, SPI_CMD_TX_DATA_SIZE_WIDTH, SPI_CMD_TX_DATA_SIZE_OFFSET);
    cmd[cmd_size++] = SPI_CMD_EOT(1, 0);

    if (spim->pending_repeat_misaligned_size == 0)
    {
        if (spim->pending_repeat_misaligned_2d_size > 0)
        {
            uint32_t size = spim->pending_repeat_misaligned_length;

            if (size > spim->pending_repeat_misaligned_2d_size)
            {
                size = spim->pending_repeat_misaligned_2d_size;
            }

            spim->pending_repeat_misaligned_2d_size -= size;

            spim->pending_repeat_misaligned_size = size;
            spim->pending_repeat_misaligned_ram_addr = spim->pending_repeat_misaligned_ram_addr - spim->pending_repeat_misaligned_length + spim->pending_repeat_misaligned_stride;
        }
        else
        {
            callback = pi_spim_handle_eot;
        }
    }

    pi_fc_event_handler_set_func(SOC_EVENT_UDMA_SPIM_EOT(spim->id), callback);

    pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) cmd, cmd_size*4, 0);
    pi_udma_core_lin_enqueue(spim->tx_channel_base, (uint32_t) addr, size, 0);
}



static void __attribute__((noinline)) pi_spim_send_handle_misaligned(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, pi_spim_t *spim)
{
    SPIM_TRACE(POS_LOG_TRACE, "Handling SPIM chunk (addr: 0x%lx, size: 0x%lx)\n", addr, size);

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = size;
    spim->pending_repeat_misaligned_2d_size = 0;

    pi_fc_event_handler_set_args(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim_cs);

    pi_spim_send_enqueue_transfer(0, spim_cs);
}


static void __attribute__((noinline)) pi_spim_send_handle_misaligned_2d(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, uint32_t stride, uint32_t length, pi_spim_t *spim)
{
    SPIM_TRACE(POS_LOG_TRACE, "Sending SPIM 2D chunk (addr: 0x%lx, data: 0x%lx, size: 0x%lx, stride: 0x%lx, length: 0x%lx)\n", addr, data, size, stride, length);

    int transfer_size = size > length ? length : size;

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = transfer_size;
    spim->pending_repeat_misaligned_length = length;
    spim->pending_repeat_misaligned_stride = stride;
    spim->pending_repeat_misaligned_2d_size = size - transfer_size;

    pi_fc_event_handler_set_args(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim_cs);

    pi_spim_send_enqueue_transfer(0, spim_cs);
}

static __attribute__((noinline)) void pi_spim_receive_enqueue_transfer(uint32_t event, void *arg)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) arg;
    pi_spim_t *spim = (pi_spim_t *) spim_cs->spim;

    pi_fc_event_handler_set_func(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim->pending_callback);

    uint32_t addr = spim->pending_repeat_misaligned_addr;
    uint32_t size = spim->pending_repeat_misaligned_size;
    uint32_t chunk_size = spim_cs->max_rcv_size;

    if (size == 0)
    {
        return;
    }

    uint32_t cmd_size;
    uint32_t *cmd;

    SPIM_TRACE(POS_LOG_TRACE, "Enqueueing receive transfer (addr: 0x%lx, ram_addr: 0x%x, pending size: 0x%lx)\n", addr, spim->pending_repeat_misaligned_ram_addr, size);

    memcpy(spim_cs->receive_addr_ucode, &spim->pending_repeat_misaligned_ram_addr, spim_cs->receive_addr_ucode_size);

    cmd_size = spim_cs->udma_receive_cmd_size;
    cmd = spim_cs->udma_receive_cmd;

    spim->pending_callback = pi_spim_receive_enqueue_transfer;

    if (size > chunk_size)
    {
        size = chunk_size;
    }
    else
    {
        size &= ~0x3;
    }

    SPIM_TRACE(POS_LOG_TRACE, "Enqueueing aligned receive chunk (addr: 0x%x, size: 0x%lx)\n", spim->pending_repeat_misaligned_addr, size);

    spim->pending_repeat_misaligned_ram_addr += size;
    spim->pending_repeat_misaligned_addr += size;
    spim->pending_repeat_misaligned_size -= size;

    cmd[cmd_size++] = __BITINSERT(spim->rx_cmd, size*8-1, SPI_CMD_RX_DATA_SIZE_WIDTH, SPI_CMD_RX_DATA_SIZE_OFFSET);
    cmd[cmd_size++] = SPI_CMD_EOT(1, 0);

    if (spim->pending_repeat_misaligned_size == 0)
    {
        if (spim->pending_repeat_misaligned_2d_size > 0)
        {
            uint32_t size = spim->pending_repeat_misaligned_length;

            if (size > spim->pending_repeat_misaligned_2d_size)
            {
                size = spim->pending_repeat_misaligned_2d_size;
            }

            spim->pending_repeat_misaligned_2d_size -= size;

            spim->pending_repeat_misaligned_size = size;
            spim->pending_repeat_misaligned_ram_addr = spim->pending_repeat_misaligned_ram_addr - spim->pending_repeat_misaligned_length + spim->pending_repeat_misaligned_stride;
        }
        else
        {
            spim->pending_callback = pi_spim_handle_eot;
        }
    }


    pi_udma_core_lin_enqueue(spim->rx_channel_base, (uint32_t) addr, size, 0);
    pi_udma_core_lin_enqueue(spim->cmd_channel_base, (uint32_t) cmd, cmd_size*4, 0);
}



static void __attribute__((noinline)) pi_spim_receive_handle_misaligned(pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size, pi_spim_t *spim)
{
    SPIM_TRACE(POS_LOG_TRACE, "Receiving SPIM chunk (addr: 0x%lx, size: 0x%lx)\n", addr, size);

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = size;
    spim->pending_repeat_misaligned_2d_size = 0;

    pi_fc_event_handler_set_args(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim_cs);

    pi_spim_receive_enqueue_transfer(0, spim_cs);

    if (spim->pending_repeat_misaligned_size)
    {
        pi_spim_receive_enqueue_transfer(0, spim_cs);
    }
    else
    {
        pi_fc_event_handler_set_func(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim->pending_callback);
    }

}



static void __attribute__((noinline)) pi_spim_receive_handle_misaligned_2d(
    pi_spim_cs_t *spim_cs, uint32_t addr, uint32_t data, uint32_t size,
    uint32_t stride, uint32_t length, pi_spim_t *spim)
{
    SPIM_TRACE(POS_LOG_TRACE, "Receiving SPIM 2D chunk (addr: 0x%lx, data: 0x%lx, size: 0x%lx, stride: 0x%lx, length: 0x%lx)\n", addr, data, size, stride, length);

    int transfer_size = size > length ? length : size;

    spim->pending_repeat_misaligned_ram_addr = addr;
    spim->pending_repeat_misaligned_addr = data;
    spim->pending_repeat_misaligned_size = transfer_size;
    spim->pending_repeat_misaligned_length = length;
    spim->pending_repeat_misaligned_stride = stride;
    spim->pending_repeat_misaligned_2d_size = size - transfer_size;

    pi_fc_event_handler_set_args(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim_cs);

    pi_spim_receive_enqueue_transfer(0, spim_cs);

    if (spim->pending_repeat_misaligned_size)
    {
        pi_spim_receive_enqueue_transfer(0, spim_cs);
    }
    else
    {
        pi_fc_event_handler_set_func(SOC_EVENT_UDMA_SPIM_EOT(spim->id), spim->pending_callback);
    }
}


void pi_spi_copy_2d_async(struct pi_device *device, uint32_t addr, void *data,
                          uint32_t size, uint32_t stride, uint32_t length,
                          pi_spi_flags_e flags, pi_task_t *task)
{
    SPIM_TRACE(POS_LOG_DEBUG, "Copy 2D bitstream (device: %p, ext2loc: %d, addr: 0x%lx, buffer: %p, size: 0x%lx, stride: 0x%lx, length: 0x%lx, flags: 0x%x, task: %p)\n", device, __BITEXTRACT(flags, 1, 5), addr, data, size, stride, length, flags, task);

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;
    pi_spim_t *spim = spim_cs->spim;

    uint32_t irq = pi_irq_disable();

    if (likely(!spim->pending_copy))
    {
        int qspi = __BITEXTRACT(flags, 2, 2) == 1;
        int cs_mode = __BITEXTRACT(flags, 2, 0);
        int ext2loc = __BITEXTRACT(flags, 1, 4);

        spim->pending_copy = task;
        spim->pending_is_auto = cs_mode == PI_SPI_CS_AUTO;

        if (ext2loc)
        {
            spim->rx_cmd = __BITINSERT(spim_cs->rx_cmd, qspi, SPI_CMD_RX_DATA_QPI_WIDTH, SPI_CMD_RX_DATA_QPI_OFFSET);
            pi_spim_receive_handle_misaligned_2d(spim_cs, addr, (uint32_t) data, size, stride, length, spim);
        }
        else
        {
            spim->tx_cmd = __BITINSERT(spim_cs->tx_cmd, qspi, SPI_CMD_TX_DATA_QPI_WIDTH, SPI_CMD_TX_DATA_QPI_OFFSET);
            pi_spim_send_handle_misaligned_2d(spim_cs, addr, (uint32_t) data, size, stride, length, spim);
        }
    }
    else
    {
        pi_spim_enqueue_to_pending_7(spim, task, 4, (int) device, addr, (int) data, size, stride, length, flags);
    }

    pi_irq_restore(irq);
}



void pi_spi_copy(struct pi_device *device, uint32_t addr, void *data,
                 uint32_t size, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_spi_copy_async(device, addr, data, size, flags, pi_task_block(&task));
    pi_task_wait_on(&task);
}



void pi_spi_copy_async(struct pi_device *device, uint32_t addr, void *data,
                       uint32_t size, pi_spi_flags_e flags, pi_task_t *task)
{
    SPIM_TRACE(POS_LOG_DEBUG, "Copy bitstream (device: %p, ext2loc: %d, addr: 0x%lx, buffer: %p, size: 0x%lx, flags: 0x%x, task: %p)\n", device, __BITEXTRACT(flags, 1, 4), addr, data, size, flags, task);

    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;
    pi_spim_t *spim = spim_cs->spim;

    uint32_t irq = pi_irq_disable();

    if (likely(!spim->pending_copy))
    {
        int qspi = __BITEXTRACT(flags, 2, 2) == 1;
        int cs_mode = __BITEXTRACT(flags, 2, 0);
        int ext2loc = __BITEXTRACT(flags, 1, 4);

        spim->pending_copy = task;
        spim->pending_is_auto = cs_mode == PI_SPI_CS_AUTO;

        if (ext2loc)
        {
            spim->rx_cmd = __BITINSERT(spim_cs->rx_cmd, qspi, SPI_CMD_RX_DATA_QPI_WIDTH, SPI_CMD_RX_DATA_QPI_OFFSET);
            pi_spim_receive_handle_misaligned(spim_cs, addr, (uint32_t) data, size, spim);
        }
        else
        {
            spim->tx_cmd = __BITINSERT(spim_cs->tx_cmd, qspi, SPI_CMD_TX_DATA_QPI_WIDTH, SPI_CMD_TX_DATA_QPI_OFFSET);
            pi_spim_send_handle_misaligned(spim_cs, addr, (uint32_t) data, size, spim);
        }
    }
    else
    {
        pi_spim_enqueue_to_pending_7(spim, task, 3, (int) device, addr, (int) data, size, flags, 0, 0);
    }

    pi_irq_restore(irq);
}



void *pi_spi_receive_ucode_set(struct pi_device *device, uint8_t *ucode,
                               uint32_t ucode_size)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;

    if (spim_cs->udma_receive_cmd)
    {
        pi_l2_free(spim_cs->udma_receive_cmd, (spim_cs->udma_receive_cmd_size + 2)*4);
    }

    spim_cs->udma_receive_cmd = pi_l2_malloc(ucode_size + 4*4);
    if (spim_cs->udma_receive_cmd == NULL)
    {
        return NULL;
    }

    pi_spim_apply_conf(spim_cs);

    memcpy(&spim_cs->udma_receive_cmd[2], ucode, ucode_size);
    spim_cs->udma_receive_cmd_size = 2 + (ucode_size >> 2);

    return (void *)&spim_cs->udma_receive_cmd[2];
}



void pi_spi_receive_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode,
                                        uint32_t ucode_size)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *) device->data;

    spim_cs->receive_addr_ucode = ucode;
    spim_cs->receive_addr_ucode_size = ucode_size;
}



void *pi_spi_send_ucode_set(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;

    if (spim_cs->udma_send_cmd)
    {
        pi_l2_free(spim_cs->udma_send_cmd, (spim_cs->udma_send_cmd_size + 2)*4);
    }

    spim_cs->udma_send_cmd = pi_l2_malloc(ucode_size + 4*4);
    if (spim_cs->udma_send_cmd == NULL)
    {
        return NULL;
    }

    pi_spim_apply_conf(spim_cs);

    memcpy(&spim_cs->udma_send_cmd[2], ucode, ucode_size);
    spim_cs->udma_send_cmd_size = 2 + (ucode_size >> 2);

    return (void *)&spim_cs->udma_send_cmd[2];
}



void pi_spi_send_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode,
                                     uint32_t ucode_size)
{
    pi_spim_cs_t *spim_cs = (pi_spim_cs_t *)device->data;

    spim_cs->send_addr_ucode = ucode;
    spim_cs->send_addr_ucode_size = ucode_size;
}



void pi_spim_handle_waiting_copy(pi_task_t *task)
{
    if (task->data[0] == 0)
    {
        pi_spi_send_async((struct pi_device *)task->data[1], (void *)task->data[2],
                          task->data[3], task->data[4], task);
    }
    else if (task->data[0] == 1)
    {
        pi_spi_receive_async((struct pi_device *)task->data[1], (void *)task->data[2],
                             task->data[3], task->data[4], task);
    }
    else
    {
        pi_spi_transfer_async((struct pi_device *)task->data[1], (void *)task->data[2],
                              (void *)task->data[3], task->data[4], task->data[5], task);
    }
}

void pi_spi_conf_init(struct pi_spi_conf *conf)
{
    conf->wordsize = PI_SPI_WORDSIZE_8;
    conf->big_endian = 0;
    conf->max_baudrate = 10000000;
    conf->cs = -1;
    conf->itf = 0;
    conf->polarity = 0;
    conf->phase = 0;
    conf->max_rcv_chunk_size = -1;
    conf->max_snd_chunk_size = -1;
    conf->is_slave = 0;
}

#if !defined(__FREERTOS__)
static void __attribute__((constructor)) pi_spim_init()
{
    for (int i=0; i<ARCHI_UDMA_NB_SPIM; i++)
    {
        g_spim_data[i].open_count = 0;
        g_spim_data[i].pending_copy = NULL;
        g_spim_data[i].waiting_first = NULL;
        g_spim_data[i].id = i;
    }
}
#endif  /* __FREERTOS__ */

#ifdef __ZEPHYR__

#include <zephyr.h>
#include <device.h>
#include <init.h>

static int spi_init(struct device *device)
{
    ARG_UNUSED(device);

    pi_spim_init();

    return 0;
}

struct spi_config {
};

struct spi_data {
};

static const struct spi_config spi_cfg = {
};

static struct spi_data spi_data = {
};

DEVICE_INIT(spi, "spi", &spi_init,
        &spi_data, &spi_cfg,
        PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif
