/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define PI_INLINE_HYPER_LVL_0 static inline
#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static uint8_t __hyper_periph_open = 0;

extern struct hyper_driver_fifo *__global_hyper_driver_fifo[];

extern pi_task_t *__global_hyper_fifo_head;
extern pi_task_t *__global_hyper_fifo_tail;

static uint32_t read_val = 0, write_val = 0;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Inner functions.
 ******************************************************************************/

/*******************************************************************************
 * PMSIS FC functions implem
 ******************************************************************************/

void pi_hyper_conf_init(struct pi_hyper_conf *conf)
{
    memset(conf, 0, sizeof(struct pi_hyper_conf));
    conf->device = PI_DEVICE_HYPERBUS_TYPE;
    conf->cs = 0;
    conf->type = PI_HYPER_TYPE_RAM;
    conf->id = 0;
}

int32_t pi_hyper_open(struct pi_device *device)
{
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *) device->config;
    /* Make a copy of config just in case. */
    struct pi_hyper_conf *config = (struct pi_hyper_conf *) pi_malloc(sizeof(struct pi_hyper_conf));
    if (config == NULL)
    {
        return -1;
    }
    config->device = conf->device;
    config->cs = conf->cs;
    config->type = conf->type;
    config->id = conf->id;
    device->config = config;
    /* Init Hyper periph. */
    if (!__hyper_periph_open)
    {
        /* Init event list. */
        struct hyper_driver_fifo *fifo = (struct hyper_driver_fifo *) pi_malloc(sizeof(struct hyper_driver_fifo));
        if (fifo == NULL)
        {
            return -1;
        }
        __global_hyper_fifo_head = NULL;
        __global_hyper_fifo_tail = NULL;
        fifo->device_id = config->id;
        fifo->pending_emu_size_2d = 0;
        __global_hyper_driver_fifo[conf->id] = fifo;

        /* Set handlers. */
#if CHIP_VERSION == 3
        NVIC_EnableIRQ(29);
        NVIC_EnableIRQ(30);
#else
        pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_RX(conf->id), hyper_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_TX(conf->id), hyper_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_RX(conf->id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_TX(conf->id));
#endif
        /* Disable UDMA CG. */
        udma_init_device(UDMA_HYPER_ID(conf->id));

        /* Pin settings. */
        __pi_hyper_pin_settings();

        /* Hyper config. */
        __pi_hyper_settings(0);
    }
    /* Signal devices used. */
    __hyper_periph_open++;
    return 0;
}

void pi_hyper_close(struct pi_device *device)
{
    struct pi_hyper_conf *conf = (struct pi_hyper_conf *) device->config;
    /* Devices used. */
    __hyper_periph_open--;
    /* Deinit Hyper periph. */
    if (!__hyper_periph_open)
    {
        /* Clear handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_HYPER_RX(conf->id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_HYPER_TX(conf->id));
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_HYPER_RX(conf->id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_HYPER_TX(conf->id));

        /* Enable UDMA CG. */
        udma_deinit_device(UDMA_HYPER_ID(conf->id));

        /* Free allocated fifo. */
        pi_free(__global_hyper_driver_fifo[conf->id]);

        /* Pin settings. */
        __pi_hyper_pin_reset_settings();
    }
    /* Free allocated config. */
    pi_free(conf);
}
