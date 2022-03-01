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

