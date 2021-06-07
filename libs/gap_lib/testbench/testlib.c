/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "testbench.h"
#include "testlib.h"
#include <bsp/ram/hyperram.h>
#include <string.h>


static pi_device_t * timestamp;
static uint32_t fifo_id;

int i2s_deinit(struct pi_device *i2s, int itf)
{
    pi_i2s_close(i2s);

    if (pi_testbench_i2s_verif_close(pi_testbench_get(), itf))
        return 0;

    return 0;
}


struct pi_device *i2s_init(struct pi_device *i2s, i2s_config_t *config)
{
    pi_testbench_i2s_verif_config_t i2s_config = {
        .sampling_freq=config->sampling_freq, .word_size=config->word_size, .nb_slots=config->nb_slots,
        .is_full_duplex=config->full_duplex, .is_ext_clk=config->is_ext_clk, .is_ext_ws=config->is_ext_ws,
        .is_sai0_clk=config->is_sai0_clk, .is_sai0_ws=config->is_sai0_ws, .clk_polarity=config->clk_polarity,
        .ws_polarity=config->ws_polarity, .ws_delay=config->ws_delay
    };

    if (pi_testbench_i2s_verif_open(pi_testbench_get(), config->itf, &i2s_config))
        return NULL;

    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = config->sampling_freq;
    i2s_conf.itf = config->itf;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S;
    i2s_conf.word_size = config->word_size;
    i2s_conf.channels = config->nb_slots;
    i2s_conf.options = PI_I2S_OPT_TDM | config->options;
    i2s_conf.ws_delay = config->ws_delay;

    if(config->use_fast_clk)
    {
        i2s_conf.options |= PI_I2S_OPT_REF_CLK_FAST;
        i2s_conf.ref_clk_freq = config->fast_clk_freq;
    }

    if (config->is_ext_clk || config->is_sai0_clk)
    {
        i2s_conf.options |= PI_I2S_OPT_EXT_CLK;
    }

    if (config->is_ext_ws || config->is_sai0_ws )
    {
        i2s_conf.options |= PI_I2S_OPT_EXT_WS;
    }

    if (config->full_duplex)
    {
        i2s_conf.options |= PI_I2S_OPT_FULL_DUPLEX;
    }

    if (config->is_sai0_clk)
    {
        i2s_conf.options |= PI_I2S_OPT_EXT_CLK_INT_ROUTED;
    }

    if (config->is_sai0_ws)
    {
        i2s_conf.options |= PI_I2S_OPT_EXT_WS_INT_ROUTED;
    }

    if (config->clk_polarity && !config->is_sai0_clk)
    {
        i2s_conf.options |= PI_I2S_OPT_CLK_POLARITY_FALLING_EDGE;
    }
    else
    {
        i2s_conf.options |= PI_I2S_OPT_CLK_POLARITY_RISING_EDGE;
    }

    if (config->ws_polarity && !config->is_sai0_ws)
    {
        i2s_conf.options |= PI_I2S_OPT_WS_POLARITY_FALLING_EDGE;
    }
    else
    {
        i2s_conf.options |= PI_I2S_OPT_WS_POLARITY_RISING_EDGE;
    }

    pi_open_from_conf(i2s, &i2s_conf);

    if (pi_i2s_open(i2s))
        return NULL;

    return i2s;
}

static uint32_t slot_iter_next(i2s_slot_test_t *i2s_slot, int is_tx)
{
    uint32_t current_value = i2s_slot->current_value & (1ULL << i2s_slot->word_size) - 1;

    // Convert the reduced element to the memory size, according to format (sign extension and shift)
    if (!is_tx && ((i2s_slot->format >> 2) & 1))
    {
        uint32_t sign_value = 0;
        if ((current_value >> (i2s_slot->word_size - 1)) & 1)
        {
            sign_value = (~((1ULL << i2s_slot->word_size) - 1)) & ((1ULL<<(i2s_slot->elem_size*8)) - 1);
        }
        current_value |= sign_value;
    }

    // Compute next value

    if (i2s_slot->incr_end - i2s_slot->current_value <= i2s_slot->incr_value)
    {
        i2s_slot->current_value = i2s_slot->incr_start;
    }
    else
    {
        i2s_slot->current_value += i2s_slot->incr_value;
    }

    return current_value;
}


static int slot_iter_find_first(i2s_slot_test_t *i2s_slot, uint32_t value)
{
    value &= (1ULL << i2s_slot->word_size) - 1;
    int fact = (value - i2s_slot->incr_start) / i2s_slot->incr_value;
    int result;
    uint32_t new_current_value = i2s_slot->incr_value * fact + i2s_slot->incr_start;
    result = new_current_value != i2s_slot->current_value;
    i2s_slot->current_value = new_current_value;
    return result;
}


static void set_buffer_elem(void *buffer, int index, uint32_t value, int word_size)
{
    if (word_size == 1)
    {
        ((uint8_t *)buffer)[index] = value;
    }
    else if (word_size == 2)
    {
        ((uint16_t *)buffer)[index] = value;
    }
    else
    {
        ((uint32_t *)buffer)[index] = value;
    }
}


static void set_buffer_elem_iter(i2s_slot_test_t *i2s_slot, void *buffer, int index)
{
    uint32_t current_value = slot_iter_next(i2s_slot, 1);

    // Fill element with radnom values on upper part to better check that the HW cutting the useless part
    current_value = (0xAAAAAAAA & ~((1ULL<<i2s_slot->word_size) - 1)) | current_value;

    set_buffer_elem(buffer, index, current_value, i2s_slot->elem_size);
}


void i2s_slot_deinit(i2s_slot_test_t *i2s_slot)
{
    if (!i2s_slot->bypass || !i2s_slot->is_rx)
    {
        struct pi_i2s_channel_conf i2s_conf;
        pi_i2s_channel_conf_init(&i2s_conf);
        i2s_conf.options = PI_I2S_OPT_DISABLED | (i2s_slot->is_rx ? PI_I2S_OPT_IS_RX: PI_I2S_OPT_IS_TX);
        pi_i2s_channel_conf_set(i2s_slot->i2s, i2s_slot->slot, &i2s_conf);

        pi_l2_free(i2s_slot->buffers[0], i2s_slot->buffer_size);
        pi_l2_free(i2s_slot->buffers[1], i2s_slot->buffer_size);
    }
}


int i2s_slot_init(i2s_slot_test_t *i2s_slot, struct pi_device *i2s, i2s_slot_config_t *slot_config)
{
    i2s_slot->i2s = i2s;
    i2s_slot->itf = slot_config->itf;
    i2s_slot->slot = slot_config->slot;
    i2s_slot->retval = 0;
    i2s_slot->word_size = slot_config->word_size;
    i2s_slot->elem_size = slot_config->elem_size;
    i2s_slot->is_rx = slot_config->is_rx;
    i2s_slot->is_first_rx = 1;
    i2s_slot->mute_delay_start = slot_config->mute_delay_start;
    i2s_slot->mute_delay_incr = slot_config->mute_delay_incr;
    i2s_slot->mute_delay_end = slot_config->mute_delay_end;
    i2s_slot->random_mute = slot_config->random_mute;
    i2s_slot->format = slot_config->format;
    i2s_slot->bypass = slot_config->bypass;

    if (!i2s_slot->bypass || !i2s_slot->is_rx)
    {
        pi_task_block(&i2s_slot->end_task);

        struct pi_i2s_channel_conf i2s_conf;
        pi_i2s_channel_conf_init(&i2s_conf);

        int buffer_size = slot_config->nb_elem * slot_config->elem_size;
        i2s_slot->buffer_size= buffer_size;
        i2s_slot->nb_elem = slot_config->nb_elem;

        if (slot_config->is_rx)
        {
            i2s_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_RX | PI_I2S_OPT_ENABLED;
        }
        else
        {
            i2s_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_TX | PI_I2S_OPT_ENABLED;
        }
        i2s_slot->buffers[0] = pi_l2_malloc(buffer_size);
        if (i2s_slot->buffers[0] == NULL)
        {
            printf("Failed to allocate\n");
            return -1;
        }
        i2s_slot->buffers[1] = pi_l2_malloc(buffer_size);
        if (i2s_slot->buffers[1] == NULL)
        {
            printf("Failed to allocate\n");
            return -1;
        }

        i2s_conf.pingpong_buffers[0] = i2s_slot->buffers[0];
        i2s_conf.pingpong_buffers[1] = i2s_slot->buffers[1];

        i2s_conf.block_size = buffer_size;
        i2s_conf.word_size = slot_config->word_size;
        i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S;
        i2s_conf.mem_word_size = slot_config->elem_size * 8;
        
        if (slot_config->bypass)
        {
            i2s_conf.options |= PI_I2S_OPT_LOOPBACK;
        }

        if ((slot_config->format >> 0) & 1)
        {
            i2s_conf.format |= PI_I2S_CH_FMT_DATA_ORDER_MSB;
        }
        else
        {
            i2s_conf.format |= PI_I2S_CH_FMT_DATA_ORDER_LSB;
        }

        if ((slot_config->format >> 1) & 1)
        {
            i2s_conf.format |= PI_I2S_CH_FMT_DATA_ALIGN_LEFT;
        }
        else
        {
            i2s_conf.format |= PI_I2S_CH_FMT_DATA_ALIGN_RIGHT;
        }

        if ((slot_config->format >> 2) & 1)
        {
            i2s_conf.format |= PI_I2S_CH_FMT_DATA_SIGN_EXTEND;
        }
        else
        {
            i2s_conf.format |= PI_I2S_CH_FMT_DATA_SIGN_NO_EXTEND;
        }

        if (slot_config->slot_disable)
        {
            i2s_conf.slot_enable = 0;
        }

        if (pi_i2s_channel_conf_set(i2s, slot_config->slot, &i2s_conf))
            return -1;

        if (slot_config -> ts_evt_en)
        {
            pi_timestamp_event_t evt;
            evt.ts_evt_id = 0xFF;
            pi_udma_timestamp_ioctl(timestamp, PI_UDMA_TIMESTAMP_IOCTL_EVT_ALLOC, &evt);
            if (evt.ts_evt_id == 0xFF)
            {
                return -2;
            }

            i2s_conf.ts_evt_id = evt.ts_evt_id;

            pi_i2s_channel_timestamp_set(i2s, slot_config->slot, &i2s_conf);
            evt.dest_id = fifo_id;
            pi_udma_timestamp_ioctl(timestamp, PI_UDMA_TIMESTAMP_IOCTL_SET_EVT, &evt);
        }

        pi_testbench_i2s_verif_slot_config_t config = { .is_rx=slot_config->is_rx, .word_size=slot_config->word_size, .format=slot_config->format };
        if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), slot_config->itf, slot_config->slot, &config))
            return -1;

        if (!slot_config->is_rx)
        {
            i2s_slot->tx_buffers[0] = i2s_conf.pingpong_buffers[0];
            i2s_slot->tx_buffers[1] = i2s_conf.pingpong_buffers[1];
            i2s_slot->tx_buffer = 0;
        }
    }

    return 0;
}


void i2s_slot_callback_tx_file_dumper(void *arg)
{
    i2s_slot_test_t *i2s_slot = (i2s_slot_test_t *)arg;

    int error = pi_i2s_write_status(&i2s_slot->task);

    if (error)
    {
        i2s_slot->retval++;
        pi_task_push(&i2s_slot->end_task);
        return;
    }

    if (i2s_slot->nb_sample > 0)
    {
        i2s_slot->nb_sample -= i2s_slot->nb_elem;
    }

    if (i2s_slot->nb_sample > 0 || i2s_slot->nb_sample == -1)
    {
        void *buffer = i2s_slot->tx_buffers[i2s_slot->tx_buffer];
        i2s_slot->tx_buffer ^= 1;
        for (int i=0; i<i2s_slot->nb_elem; i++)
        {
            set_buffer_elem_iter(i2s_slot, buffer, i);
        }

        int err = pi_i2s_channel_write_async(i2s_slot->i2s, i2s_slot->slot, NULL, i2s_slot->buffer_size, pi_task_callback(&i2s_slot->task, i2s_slot_callback_tx_file_dumper, (void *)i2s_slot));
    }
    else
    {
        pi_task_push(&i2s_slot->end_task);
    }
}


static uint32_t buffer_get_elem(void *buffer, int index, int word_size, void **address)
{
    void *result;
    if (word_size == 1)
    {
        uint8_t *addr = &((uint8_t *)buffer)[index];
        *address = addr;
        return *addr;
    }
    else if (word_size == 2)
    {
        uint16_t *addr = &((uint16_t *)buffer)[index];
        *address = addr;
        return *addr;
    }
    else if (word_size == 3)
    {
        uint32_t result = 0;
        uint8_t *src = &((uint8_t *)buffer)[index*3];
        memcpy((void *)&result, (void *)src, 3);
        return result;
    }
    else
    {
        uint32_t *addr = &((uint32_t *)buffer)[index];
        *address = addr;
        return *addr;
    }
}

void i2s_slot_callback_rx_iter(void *arg)
{
    i2s_slot_test_t *i2s_slot = (i2s_slot_test_t *)arg;
    void *chunk;
    int size;

    if (pi_i2s_read_status(&i2s_slot->task, &chunk, (size_t *)&size))
    {
        i2s_slot->retval++;
        goto end;
    }

    int nb_elem = size / i2s_slot->elem_size;

    for (int i=0; i<nb_elem && i2s_slot->nb_sample > 0; i++)
    {
        void *address;
        uint32_t value = buffer_get_elem(chunk, i, i2s_slot->elem_size, &address);

        if (i2s_slot->is_first_rx)
        {
            i2s_slot->is_first_rx = 0;
            slot_iter_find_first(i2s_slot, value);
        }

        if (i2s_slot->mute_ack)
        {
            if (slot_iter_find_first(i2s_slot, value))
            {
                i2s_slot->mute_ack--;
            }
        }

        uint32_t current_value = slot_iter_next(i2s_slot, 0);

        if (value != current_value)
        {
            printf("Detected error (itf: %d, slot: %d, index: %d, nb_elem: %d, expected: 0x%x, got: 0x%x, address: %p)\n", i2s_slot->itf, i2s_slot->slot, i, nb_elem, current_value, value, address);
            i2s_slot->retval++;
            goto end;
        }

        i2s_slot->nb_sample--;
        if (i2s_slot->nb_sample == 0)
            goto end;
    }

    pi_i2s_channel_read_async(i2s_slot->i2s, i2s_slot->slot, pi_task_callback(&i2s_slot->task, i2s_slot_callback_rx_iter, (void *)i2s_slot));
    return;

end:
    pi_task_push(&i2s_slot->end_task);
}


int i2s_slot_stop(i2s_slot_test_t *i2s_slot)
{
    return pi_testbench_i2s_verif_slot_stop(pi_testbench_get(), i2s_slot->itf, i2s_slot->slot);
}


void i2s_slot_end(i2s_slot_test_t *i2s_slot)
{
    pi_task_push(&i2s_slot->end_task);
}


int i2s_slot_start(i2s_slot_test_t *i2s_slot, i2s_slot_start_config_t *config)
{
    if (!i2s_slot->bypass)
    {
        if (config->type == I2S_VERIF_RX_ITER)
        {
            i2s_slot->incr_start = config->testbench_config.rx_iter.incr_start & ((1ULL<<i2s_slot->word_size) - 1);
            i2s_slot->incr_end = config->testbench_config.rx_iter.incr_end & ((1ULL<<i2s_slot->word_size) - 1);
            i2s_slot->incr_value = config->testbench_config.rx_iter.incr_value;
            i2s_slot->current_value = config->testbench_config.rx_iter.incr_start;
            i2s_slot->nb_sample = config->nb_samples;

            if (i2s_slot->incr_value >= i2s_slot->incr_end)
                i2s_slot->incr_value = 0;

            pi_i2s_channel_read_async(i2s_slot->i2s, i2s_slot->slot, pi_task_callback(&i2s_slot->task, i2s_slot_callback_rx_iter, (void *)i2s_slot));
        }
        else if (config->type == I2S_VERIF_TX_ITER)
        {
            i2s_slot->incr_start = config->tx_iter.incr_start & ((1ULL<<i2s_slot->word_size) - 1);
            i2s_slot->incr_end = config->tx_iter.incr_end & ((1ULL<<i2s_slot->word_size) - 1);
            i2s_slot->incr_value = config->tx_iter.incr_value;
            i2s_slot->current_value = config->tx_iter.incr_start;
            i2s_slot->nb_sample = config->tx_iter.nb_samples;

            if (i2s_slot->incr_value >= i2s_slot->incr_end)
                i2s_slot->incr_value = 0;

            for (int i=0; i<i2s_slot->nb_elem; i++)
            {
                set_buffer_elem_iter(i2s_slot, i2s_slot->tx_buffers[0], i);
            }
            for (int i=0; i<i2s_slot->nb_elem; i++)
            {
                set_buffer_elem_iter(i2s_slot, i2s_slot->tx_buffers[1], i);
            }

            pi_i2s_channel_write_async(i2s_slot->i2s, i2s_slot->slot, NULL, i2s_slot->buffer_size, pi_task_callback(&i2s_slot->task, i2s_slot_callback_tx_file_dumper, (void *)i2s_slot));
        }
    }

    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), i2s_slot->itf, i2s_slot->slot, &config->testbench_config))
        return -1;

    return 0;
}


int i2s_slot_wait(i2s_slot_test_t *i2s_slot)
{
    if (i2s_slot->bypass)
    {
        return 0;
    }
    else
    {
        pi_task_wait_on(&i2s_slot->end_task);
        return i2s_slot->retval;
    }
}


int i2s_test_deinit(i2s_test_t *test)
{
    for (int i=0; i<16; i++)
    {
        if (test->slot_test_rx[i].i2s)
        {
            i2s_slot_deinit(&test->slot_test_rx[i]);
        }
        if (test->slot_test_tx[i].i2s)
        {
            i2s_slot_deinit(&test->slot_test_tx[i]);
        }
    }

    return i2s_deinit(&test->i2s, test->itf);
}


int i2s_test_init(i2s_test_t *test, i2s_test_config_t *config)
{
    i2s_config_t i2s_config = {
        .itf=config->itf, .sampling_freq=config->sampling_freq, .word_size=config->word_size, .nb_slots=config->nb_slots,
        .full_duplex=config->full_duplex, .is_ext_clk=config->is_ext_clk, .is_ext_ws=config->is_ext_ws,
        .is_sai0_clk=config->is_sai0_clk, .is_sai0_ws=config->is_sai0_ws, .clk_polarity=config->clk_polarity,
        .ws_polarity=config->ws_polarity, .use_fast_clk=config->use_fast_clk, .fast_clk_freq=config->fast_clk_freq,
        .ws_delay=config->ws_delay, .elem_size=config->elem_size
    };
    if (i2s_init(&test->i2s, &i2s_config) == NULL)
        return -1;

    timestamp = config->timestamp;
    test->rx_slots = config->rx_slots;
    test->tx_slots = config->tx_slots;
    test->is_ext_clk = config->is_ext_clk;
    test->itf = config->itf;
    test->random_mute = config->random_mute;

    fifo_id = config->fifo_id;

    for (int i=0; i<16; i++)
    {
        test->slot_test_rx[i].i2s = NULL;

        if ((config->rx_slots >> i) & 1)
        {
            int slot_width = (((i < 8 ? config->rx_slots_width_0 : config->rx_slots_width_1) >> (i*8)) & 0x1F) + 1;
            int slot_format = (config->rx_slots_format >> (i*4)) & 0xF;
            int random_mute = (config->random_mute >> i) & 1;
            int ts_evt_en = (config->ts_evt >> i) & 1;

            i2s_slot_config_t i2s_slot_config = {
                .itf=config->itf, .slot=i, .is_rx=1, .word_size=slot_width, .nb_elem=config->buffer_nb_elem, .elem_size=config->elem_size, .format=slot_format,
                .mute_delay_start=30, .mute_delay_incr=20, .mute_delay_end=150, .random_mute=random_mute, .ts_evt_en=ts_evt_en

            };

            if (i2s_slot_init(&test->slot_test_rx[i], &test->i2s, &i2s_slot_config))
                return -1;
    
            int iter = config->nb_slots;
            if (config->full_duplex)
            {
                iter *= 2;
            }

            i2s_slot_start_config_t start_config = {
                .type=I2S_VERIF_RX_ITER,
                .nb_samples=config->nb_samples,
                .testbench_config={
                    .type=PI_TESTBENCH_I2S_VERIF_RX_ITER,
                    .rx_iter= { .nb_samples=-1, .incr_start=i*2+config->itf*32, .incr_end=(1ULL << config->word_size) - 1, .incr_value=iter*3 + config->incr }
                }
            };

            if (i2s_slot_start(&test->slot_test_rx[i], &start_config))
                return -1;
        } 
    }


    for (int i=0; i<16; i++)
    {
        test->slot_test_tx[i].i2s = NULL;

        if ((config->tx_slots >> i) & 1)
        {
            int slot_width = (((i < 8 ? config->tx_slots_width_0 : config->tx_slots_width_1) >> (i*8)) & 0x1F) + 1;
            int slot_format = ((config->tx_slots_format >> (i*4)) & 0xF);
            int random_mute = (config->random_mute >> i) & 1;
            int ts_evt_en = (config->ts_evt >> (16+i)) & 1;
            int bypass = (config->tx_slots_bypass >> i) & 1;

            i2s_slot_config_t i2s_slot_config = {
                .itf=config->itf, .slot=i, .is_rx=0, .word_size=slot_width, .nb_elem=config->buffer_nb_elem, .elem_size=config->elem_size, .format=slot_format,
                .mute_delay_start=30, .mute_delay_incr=20, .mute_delay_end=150, .random_mute=random_mute, .ts_evt_en=ts_evt_en, .bypass=bypass
            };

            if (i2s_slot_init(&test->slot_test_tx[i], &test->i2s, &i2s_slot_config))
                return -1;
    
            int iter = config->nb_slots;
            if (config->full_duplex)
            {
                iter *= 2;
            }

            char filename[32];
            sprintf(filename, "outfile_test%d_itf%d_slot%d.txt", config->test_id, config->itf, i);
            i2s_slot_start_config_t start_config = {
                .type=I2S_VERIF_TX_ITER,
                .testbench_config={
                    .type=PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER,
                    .tx_file_dumper= { .nb_samples=-1, .filepath=(uint32_t)filename }
                },
                .tx_iter={ .nb_samples=config->nb_samples, .incr_start=i*2+1 + config->itf*32, .incr_end=(1ULL << config->word_size) - 1, .incr_value=iter*3 }
            };

            if (i2s_slot_start(&test->slot_test_tx[i], &start_config))
                return -1;
        }
    }

    return 0;
}


static void mute_task_trigger(i2s_slot_test_t *slot);

static void mute_task_handler(void *arg)
{
    i2s_slot_test_t *slot = (i2s_slot_test_t *)arg;

    if (!slot->mute)
    {
        slot->mute_ack++;
    }

    pi_i2s_slots_enable(slot->i2s, 1<<slot->slot, slot->mute);

    slot->mute ^= 1;


    slot->mute_delay_current += slot->mute_delay_incr;
    if (slot->mute_delay_current >= slot->mute_delay_end)
    {
        slot->mute_delay_current = slot->mute_delay_start;
    }

    pi_task_push_delayed_us(pi_task_callback(&slot->mute_task, mute_task_handler, (void *)slot), slot->mute_delay_current);
}


static void mute_task_trigger(i2s_slot_test_t *slot)
{
    pi_task_push_delayed_us(pi_task_callback(&slot->mute_task, mute_task_handler, (void *)slot), slot->mute_delay_current);
}


int i2s_test_start(i2s_test_t *test)
{
    if (test->is_ext_clk)
    {
        pi_testbench_i2s_verif_start(pi_testbench_get(), test->itf);
    }

    if (pi_i2s_ioctl(&test->i2s, PI_I2S_IOCTL_START, NULL))
        return -1;

    if (test->random_mute)
    {
        for (int i=0; i<16; i++)
        {
            if ((test->rx_slots >> i) & 1)
            {
                i2s_slot_test_t *slot = &test->slot_test_rx[i];
                if (slot->random_mute)
                {
                    slot->mute = 0;
                    slot->mute_ack = 0;
                    slot->mute_delay_current = slot->mute_delay_start + (slot->mute_delay_end - slot->mute_delay_start) / 16 * i;
                    mute_task_trigger(slot);
                    continue;
                }
            }
            if ((test->tx_slots >> i) & 1)
            {
                i2s_slot_test_t *slot = &test->slot_test_tx[i];
                if (slot->random_mute)
                {
                    slot->mute = 0;
                    slot->mute_ack = 0;
                    slot->mute_delay_current = slot->mute_delay_start + (slot->mute_delay_end - slot->mute_delay_start) / 16 * i;
                    mute_task_trigger(slot);
                }
            }
        }
    }

    return 0;
}


int i2s_test_wait(i2s_test_t *test)
{
    int errors = 0;

    for (int i=0; i<16; i++)
    {
        if ((test->rx_slots >> i) & 1)
        {
            errors += i2s_slot_wait(&test->slot_test_rx[i]);
        }
    }

    for (int i=0; i<16; i++)
    {
        if ((test->tx_slots >> i) & 1)
        {
            errors += i2s_slot_wait(&test->slot_test_tx[i]);
        }
    }

    return errors;
}


int i2s_test_stop(i2s_test_t *test)
{

    if (test->random_mute)
    {
        for (int i=0; i<16; i++)
        {
            if ((test->rx_slots >> i) & 1)
            {
                i2s_slot_test_t *slot = &test->slot_test_rx[i];
                pi_task_abort(&slot->mute_task);
            }
            if ((test->tx_slots >> i) & 1)
            {
                i2s_slot_test_t *slot = &test->slot_test_tx[i];
                pi_task_abort(&slot->mute_task);
            }
        }
    }

    if (pi_i2s_ioctl(&test->i2s, PI_I2S_IOCTL_STOP, NULL))
        return -1;

    return 0;
}


void testlib_hyperram_trafficgen_conf_init(testlib_hyperram_trafficgen_config_t *config)
{
    config->transfer_size = 8192;
    config->itf = -1;
    config->cs = -1;
    config->frequency = -1;
}


int testlib_hyperram_trafficgen_init(testlib_hyperram_trafficgen_t *data, testlib_hyperram_trafficgen_config_t *config)
{
    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);

    if (config->itf != -1)
    {
        conf.hyper_itf = config->itf;
    }
    if (config->cs != -1)
    {
        conf.hyper_cs = config->cs;
    }
    if (config->frequency != -1)
    {
        conf.baudrate = config->frequency;
    }

    pi_open_from_conf(&data->dev, &conf);

    if (pi_ram_open(&data->dev))
        goto error0;

    if (pi_ram_alloc(&data->dev, &data->hyper_addr, config->transfer_size))
        goto error1;

    data->transfer_size = config->transfer_size;

    data->buffer = pi_l2_malloc(config->transfer_size);
    if (data->buffer == NULL) goto error2;

    for (int i=0; i<config->transfer_size/4; i++)
    {
        ((uint32_t *)data->buffer)[i] = i;
    }

    return 0;

error2:
    pi_ram_free(&data->dev, data->hyper_addr, config->transfer_size);
error1:
    pi_ram_close(&data->dev);
error0:
    return -1;
}


static void testlib_hyperram_callback(void *arg)
{
    testlib_hyperram_trafficgen_t *data = (testlib_hyperram_trafficgen_t *)arg;

    if (data->end)
    {
        data->pending--;
        if (data->pending == 0)
        {
            pi_task_push(&data->end_task);
        }
        return;
    }

    if (data->is_read)
    {
        data->is_read = 0;
        pi_ram_read_async(&data->dev, data->hyper_addr, data->buffer, data->transfer_size, pi_task_callback(&data->read_task, testlib_hyperram_callback, (void *)data));
    }
    else
    {
        data->is_read = 1;
        pi_ram_write_async(&data->dev, data->hyper_addr, data->buffer, data->transfer_size, pi_task_callback(&data->write_task, testlib_hyperram_callback, (void *)data));
    }
}


int testlib_hyperram_trafficgen_start(testlib_hyperram_trafficgen_t *data)
{
    data->is_read = 0;
    data->end = 0;
    data->pending = 2;
    pi_task_block(&data->end_task);
    pi_ram_write_async(&data->dev, data->hyper_addr, data->buffer, data->transfer_size, pi_task_callback(&data->read_task, testlib_hyperram_callback, (void *)data));
    pi_ram_read_async(&data->dev, data->hyper_addr, data->buffer, data->transfer_size, pi_task_callback(&data->write_task, testlib_hyperram_callback, (void *)data));
    return 0;
}


int testlib_hyperram_trafficgen_stop(testlib_hyperram_trafficgen_t *data)
{
    int errors = 0;

    data->end = 1;

    pi_task_wait_on(&data->end_task);

    for (int i=0; i<data->transfer_size/4; i++)
    {
        uint32_t expected = i;
        if (expected != ((uint32_t *)data->buffer)[i])
        {
            errors++;
        }
    }

    return errors;
}


int testlib_hyperram_trafficgen_deinit(testlib_hyperram_trafficgen_t *data)
{
    pi_ram_free(&data->dev, data->hyper_addr, data->transfer_size);
    pi_ram_close(&data->dev);
    return 0;
}

