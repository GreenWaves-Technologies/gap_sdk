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


void testlib_uart_trafficgen_conf_init(testlib_uart_trafficgen_config_t *config)
{
    config->transfer_size = 128;
    config->itf = 0;
    config->baudrate = 1000000;
    config->control_flow = 1;
}


int testlib_uart_trafficgen_init(testlib_uart_trafficgen_t *data, testlib_uart_trafficgen_config_t *config)
{
    struct pi_uart_conf conf;

    data->transfer_size = config->transfer_size;
    data->rx_buffers[0] = pi_l2_malloc(config->transfer_size);
    data->rx_buffers[1] = pi_l2_malloc(config->transfer_size);
    data->tx_buffers[0] = pi_l2_malloc(config->transfer_size);
    data->tx_buffers[1] = pi_l2_malloc(config->transfer_size);

    if (data->rx_buffers[0] == NULL || data->rx_buffers[1] == NULL || data->tx_buffers[0] == NULL || data->tx_buffers[1] == NULL)
    {
        return -1;
    }

    for (int i=0; i<config->transfer_size; i++)
    {
        ((uint8_t *)data->tx_buffers[0])[i] = i;
        ((uint8_t *)data->tx_buffers[1])[i] = i;
    }

    pi_uart_conf_init(&conf);

    conf.use_ctrl_flow = config->control_flow;
    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.uart_id = config->itf;
    conf.baudrate_bps = config->baudrate;

    pi_open_from_conf(&data->dev, &conf);

    if (pi_uart_open(&data->dev))
    {
        return -1;
    }

    return 0;
}


static void testlib_uart_rx_callback(void *arg)
{
    testlib_uart_trafficgen_t *data = (testlib_uart_trafficgen_t *)arg;

    if (data->end)
    {
        data->rx_pending--;
        if (data->rx_pending == 0)
        {
            pi_task_push(&data->rx_end_task);
        }
        return;
    }

    pi_uart_read_async(&data->dev, data->rx_buffers[data->rx_current_task], data->transfer_size, pi_task_irq_callback(&data->rx_tasks[data->rx_current_task], testlib_uart_rx_callback, (void *)data));
    data->rx_current_task ^= 1;
}

static void testlib_uart_tx_callback(void *arg)
{
    testlib_uart_trafficgen_t *data = (testlib_uart_trafficgen_t *)arg;

    if (data->end)
    {
        data->tx_pending--;
        if (data->tx_pending == 0)
        {
            pi_task_push(&data->tx_end_task);
        }
        return;
    }

    pi_uart_write_async(&data->dev, data->tx_buffers[data->tx_current_task], data->transfer_size, pi_task_irq_callback(&data->tx_tasks[data->tx_current_task], testlib_uart_tx_callback, (void *)data));
    data->tx_current_task ^= 1;
}


int testlib_uart_trafficgen_start(testlib_uart_trafficgen_t *data)
{
    data->end = 0;

    data->tx_pending = 2;
    data->tx_current_task = 0;
    pi_task_block(&data->tx_end_task);
    pi_uart_write_async(&data->dev, data->tx_buffers[0], data->transfer_size, pi_task_irq_callback(&data->tx_tasks[0], testlib_uart_tx_callback, (void *)data));
    pi_uart_write_async(&data->dev, data->tx_buffers[1], data->transfer_size, pi_task_irq_callback(&data->tx_tasks[1], testlib_uart_tx_callback, (void *)data));

    data->rx_pending = 2;
    data->rx_current_task = 0;
    pi_task_block(&data->rx_end_task);
    pi_uart_read_async(&data->dev, data->rx_buffers[0], data->transfer_size, pi_task_irq_callback(&data->rx_tasks[0], testlib_uart_rx_callback, (void *)data));
    pi_uart_read_async(&data->dev, data->rx_buffers[1], data->transfer_size, pi_task_irq_callback(&data->rx_tasks[1], testlib_uart_rx_callback, (void *)data));

    return 0;
}


int testlib_uart_trafficgen_stop(testlib_uart_trafficgen_t *data)
{
    int errors = 0;

    data->end = 1;

    pi_task_wait_on(&data->tx_end_task);
    pi_task_wait_on(&data->rx_end_task);

    return errors;
}


int testlib_uart_trafficgen_deinit(testlib_uart_trafficgen_t *data)
{
    pi_l2_free(data->tx_buffers[0], data->transfer_size);
    pi_l2_free(data->tx_buffers[1], data->transfer_size);
    pi_l2_free(data->rx_buffers[0], data->transfer_size);
    pi_l2_free(data->rx_buffers[1], data->transfer_size);
    pi_uart_close(&data->dev);
    return 0;
}

