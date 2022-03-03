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
#include <string.h>


void testlib_i2s_trafficgen_conf_init(testlib_i2s_trafficgen_config_t *config)
{
    config->transfer_size = 128;
    config->itf = 0;
    config->sample_rate = 48000;
    config->nb_slots = 16;
    config->word_size = 32;
}


int testlib_i2s_trafficgen_init(testlib_i2s_trafficgen_t *data, testlib_i2s_trafficgen_config_t *config)
{
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = config->sample_rate;
    i2s_conf.itf = config->itf;
    i2s_conf.word_size = config->word_size;
    i2s_conf.channels = config->nb_slots;
    i2s_conf.options = PI_I2S_OPT_TDM | PI_I2S_OPT_FULL_DUPLEX;

    pi_open_from_conf(&data->dev, &i2s_conf);
    if (pi_i2s_open(&data->dev))
    {
        printf("Error opening i2s\n");
        return -3;
    }

    struct pi_i2s_channel_conf i2s_slot_conf;
    pi_i2s_channel_conf_init(&i2s_slot_conf);
    uint16_t frame = (1 << config->nb_slots) - 1;
    data->frame = frame;
    for (int i=0; i<config->nb_slots; i++)
    {
        i2s_slot_conf.options = PI_I2S_OPT_IS_RX | PI_I2S_OPT_PINGPONG | PI_I2S_OPT_ENABLED;
        i2s_slot_conf.word_size = config->word_size;
        i2s_slot_conf.block_size = config->transfer_size;

        if (i == 0)
        {
            data->transfer_size = config->transfer_size * config->nb_slots;
            i2s_slot_conf.pingpong_buffers[0] = pi_l2_malloc(data->transfer_size);
            i2s_slot_conf.pingpong_buffers[1] = pi_l2_malloc(data->transfer_size);
            data->rx_buffers[0] = i2s_slot_conf.pingpong_buffers[0];
            data->rx_buffers[1]= i2s_slot_conf.pingpong_buffers[1];

            if (i2s_slot_conf.pingpong_buffers[0] == NULL || i2s_slot_conf.pingpong_buffers[1] == NULL)
            {
                printf("Error allocating memory\n");
                return -1;
            }
        }

        if (pi_i2s_frame_channel_conf_set(&data->dev, frame, i, &i2s_slot_conf))
        {
            printf("Error setting conf channel\n");
            return -4;
        }
    }

    pi_i2s_channel_conf_init(&i2s_slot_conf);
    for (int i=0; i<config->nb_slots; i++)
    {
        i2s_slot_conf.options = PI_I2S_OPT_IS_TX | PI_I2S_OPT_PINGPONG | PI_I2S_OPT_ENABLED;
        i2s_slot_conf.word_size = config->word_size;
        i2s_slot_conf.block_size = config->transfer_size;

        if (i == 0)
        {
            i2s_slot_conf.pingpong_buffers[0] = pi_l2_malloc(config->transfer_size * config->nb_slots);
            i2s_slot_conf.pingpong_buffers[1] = pi_l2_malloc(config->transfer_size * config->nb_slots);
            data->tx_buffers[0] = i2s_slot_conf.pingpong_buffers[0];
            data->tx_buffers[1]= i2s_slot_conf.pingpong_buffers[1];

            if (i2s_slot_conf.pingpong_buffers[0] == NULL || i2s_slot_conf.pingpong_buffers[1] == NULL)
            {
                printf("Error allocating memory\n");
                return -1;
            }
            for (int i=0; i<config->transfer_size * config->nb_slots; i++)
            {
                ((uint8_t *)i2s_slot_conf.pingpong_buffers[0])[i] = i;
                ((uint8_t *)i2s_slot_conf.pingpong_buffers[1])[i] = i;
            }

        }

        if (pi_i2s_frame_channel_conf_set(&data->dev, frame, i, &i2s_slot_conf))
        {
            printf("Error setting conf channel\n");
            return -4;
        }
    }

    return 0;
}


static void testlib_i2s_rx_callback(void *arg)
{
    testlib_i2s_trafficgen_t *data = (testlib_i2s_trafficgen_t *)arg;

    if (data->end)
    {
        data->rx_pending--;
        if (data->rx_pending == 0)
        {
            pi_task_push(&data->rx_end_task);
        }
        return;
    }

    pi_i2s_frame_read_async(&data->dev, data->frame, pi_task_irq_callback(&data->rx_tasks[data->rx_current_task], testlib_i2s_rx_callback, (void *)data));
    data->rx_current_task ^= 1;
}

static void testlib_i2s_tx_callback(void *arg)
{
    testlib_i2s_trafficgen_t *data = (testlib_i2s_trafficgen_t *)arg;

    if (data->end)
    {
        data->tx_pending--;
        if (data->tx_pending == 0)
        {
            pi_task_push(&data->tx_end_task);
        }
        return;
    }

    pi_i2s_frame_write_async(&data->dev, data->frame, NULL, 0, pi_task_irq_callback(&data->tx_tasks[data->tx_current_task], testlib_i2s_tx_callback, (void *)data));
    data->tx_current_task ^= 1;
}


int testlib_i2s_trafficgen_start(testlib_i2s_trafficgen_t *data)
{
    data->end = 0;

    data->tx_pending = 2;
    data->tx_current_task = 0;
    pi_task_block(&data->tx_end_task);
    pi_i2s_frame_write_async(&data->dev, data->frame, NULL, 0, pi_task_irq_callback(&data->tx_tasks[0], testlib_i2s_tx_callback, (void *)data));
    pi_i2s_frame_write_async(&data->dev, data->frame, NULL, 0, pi_task_irq_callback(&data->tx_tasks[1], testlib_i2s_tx_callback, (void *)data));

    data->rx_pending = 2;
    data->rx_current_task = 0;
    pi_task_block(&data->rx_end_task);
    pi_i2s_frame_read_async(&data->dev, data->frame, pi_task_irq_callback(&data->rx_tasks[0], testlib_i2s_rx_callback, (void *)data));
    pi_i2s_frame_read_async(&data->dev, data->frame, pi_task_irq_callback(&data->rx_tasks[1], testlib_i2s_rx_callback, (void *)data));

    if (pi_i2s_ioctl(&data->dev, PI_I2S_IOCTL_START, NULL))
    {
        return -4;
    }
    return 0;
}


int testlib_i2s_trafficgen_stop(testlib_i2s_trafficgen_t *data)
{
    int errors = 0;

    data->end = 1;

    pi_task_wait_on(&data->tx_end_task);
    pi_task_wait_on(&data->rx_end_task);

    if (pi_i2s_ioctl(&data->dev, PI_I2S_IOCTL_STOP, NULL))
    {
        return -4;
    }

    return errors;
}


int testlib_i2s_trafficgen_deinit(testlib_i2s_trafficgen_t *data)
{
    pi_l2_free(data->tx_buffers[0], data->transfer_size);
    pi_l2_free(data->tx_buffers[1], data->transfer_size);
    pi_l2_free(data->rx_buffers[0], data->transfer_size);
    pi_l2_free(data->rx_buffers[1], data->transfer_size);
    pi_i2s_close(&data->dev);
    return 0;
}

