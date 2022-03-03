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


void testlib_i2c_trafficgen_conf_init(testlib_i2c_trafficgen_config_t *config)
{
    config->transfer_size = 32;
    config->itf = 0;
    config->baudrate = 400000;
}


int testlib_i2c_trafficgen_init(testlib_i2c_trafficgen_t *data, testlib_i2c_trafficgen_config_t *config)
{
    struct pi_i2c_conf conf;

    data->transfer_size = config->transfer_size;
    data->tx_buffers[0] = pi_l2_malloc(config->transfer_size);
    data->tx_buffers[1] = pi_l2_malloc(config->transfer_size);

    if (data->tx_buffers[0] == NULL || data->tx_buffers[1] == NULL)
    {
        return -1;
    }

    for (int i=0; i<config->transfer_size; i++)
    {
        ((uint8_t *)data->tx_buffers[0])[i] = i;
        ((uint8_t *)data->tx_buffers[1])[i] = i;
    }

    pi_i2c_conf_init(&conf);

    conf.itf = config->itf;
    //conf.max_baudrate = config->baudrate;
    conf.cs = 0xA0;

    pi_open_from_conf(&data->dev, &conf);

    if (pi_i2c_open(&data->dev))
    {
        return -1;
    }

    return 0;
}


static void testlib_i2c_tx_callback(void *arg)
{
    testlib_i2c_trafficgen_t *data = (testlib_i2c_trafficgen_t *)arg;

    if (data->end)
    {
        data->tx_pending--;
        if (data->tx_pending == 0)
        {
            pi_task_push(&data->tx_end_task);
        }
        return;
    }

    pi_i2c_write_async(&data->dev, data->tx_buffers[data->tx_current_task], data->transfer_size, 0, pi_task_irq_callback(&data->tx_tasks[data->tx_current_task], testlib_i2c_tx_callback, (void *)data));
    data->tx_current_task ^= 1;
}


int testlib_i2c_trafficgen_start(testlib_i2c_trafficgen_t *data)
{
    data->end = 0;

    data->tx_pending = 2;
    data->tx_current_task = 0;
    pi_task_block(&data->tx_end_task);
    pi_i2c_write_async(&data->dev, data->tx_buffers[0], data->transfer_size, 0, pi_task_irq_callback(&data->tx_tasks[0], testlib_i2c_tx_callback, (void *)data));
    pi_i2c_write_async(&data->dev, data->tx_buffers[1], data->transfer_size, 0, pi_task_irq_callback(&data->tx_tasks[1], testlib_i2c_tx_callback, (void *)data));

    return 0;
}
int testlib_i2c_trafficgen_stop(testlib_i2c_trafficgen_t *data)
{
    int errors = 0;

    data->end = 1;

    pi_task_wait_on(&data->tx_end_task);

    return errors;
}


int testlib_i2c_trafficgen_deinit(testlib_i2c_trafficgen_t *data)
{
    pi_l2_free(data->tx_buffers[0], data->transfer_size);
    pi_l2_free(data->tx_buffers[1], data->transfer_size);
    pi_i2c_close(&data->dev);
    return 0;
}

