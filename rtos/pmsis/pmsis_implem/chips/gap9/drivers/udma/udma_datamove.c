/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if !defined(__FREERTOS__)
#define SOC_EVENT_UDMA_CHAN_LIN(id) ( (id) )
#define SOC_EVENT_UDMA_CHAN_2D(id)  ( ARCHI_UDMA_NB_2D_ADDRGEN + (id) )
#define UDMA_NB_CHAN_LIN            ( ARCHI_UDMA_NB_LIN_ADDRGEN )
#endif  /* __FREERTOS__ */

#define __UDMA_NB_DATAMOVE          ( 2 )

typedef struct pi_udma_datamove_data_s
{
    struct pi_udma_datamove_itf_data_s *itf_data;
    pi_udma_datamove_transf_cfg_t src_trf_cfg; /*!< Source data transfer configuration */
    pi_udma_datamove_transf_cfg_t dst_trf_cfg; /*!< Destination data transfer configuration */
} pi_udma_datamove_data_t;

typedef struct pi_udma_datamove_itf_data_s
{
    uint8_t device_id;
    pi_task_t* fifo_head;
    pi_task_t* fifo_tail;
    pi_task_t* end_task;
    int32_t nb_open; /*!< number of devices opened */
    int32_t rx_lin_chan_id;
    int32_t tx_lin_chan_id;
    int32_t rx_2d_chan_id;
    int32_t tx_2d_chan_id;
} pi_udma_datamove_itf_data_t;

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static pi_udma_datamove_itf_data_t* g_udma_datamove_itf_data[__UDMA_NB_DATAMOVE];

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static inline void __pi_udma_datamove_task_fifo_enqueue(pi_udma_datamove_itf_data_t *itf_data,
                                                        pi_task_t *pi_task)
{
    //uint32_t irq = pi_irq_disable();
    if (itf_data->fifo_tail)
    {
        // tail insert
        itf_data->fifo_tail->next      = pi_task;
        itf_data->fifo_tail            = itf_data->fifo_tail->next;
        itf_data->fifo_tail->next      = NULL;
    }
    else
    {
        // Initialize the list
        itf_data->fifo_head          = pi_task;
        itf_data->fifo_head->next    = NULL;
        // set the base tail
        itf_data->fifo_tail          = itf_data->fifo_head;
    }
    //pi_irq_restore(irq);
}

static inline pi_task_t* __pi_udma_datamove_task_fifo_pop(pi_udma_datamove_itf_data_t *itf_data)
{
    //uint32_t irq = pi_irq_disable();
    pi_task_t *ret_task = NULL;
    if (itf_data->fifo_head != NULL)
    {
        ret_task = itf_data->fifo_head;
        hal_compiler_barrier();
        itf_data->fifo_head = itf_data->fifo_head->next;
        if (itf_data->fifo_head == NULL)
        {
            itf_data->fifo_tail = NULL;
        }
    }
    //pi_irq_restore(irq);
    return ret_task;
}

static inline void __pi_udma_datamove_copy_start(pi_udma_datamove_itf_data_t* itf_data)
{
    if (NULL == itf_data->end_task)
    {
        return;
    }

    uint32_t src = itf_data->end_task->data[0];
    uint32_t dst = itf_data->end_task->data[1];
    uint32_t len = itf_data->end_task->data[2];

    pi_udma_datamove_data_t* dev_data = (pi_udma_datamove_data_t*) itf_data->end_task->data[3];
    uint32_t udma_ctrl_base = (uint32_t) ARCHI_UDMA_ADDR;

    /* select the rx_channel according to the device configuration */
    int32_t rx_chan = -1;
    if (dev_data->dst_trf_cfg.type == PI_UDMA_DATAMOVE_TRF_LINEAR)
    {
        rx_chan = itf_data->rx_lin_chan_id;
    }
    else
    {
        rx_chan = itf_data->rx_2d_chan_id;
    }
    /* select the tx_channel according to the device configuration */
    int32_t tx_chan = -1;
    if (dev_data->src_trf_cfg.type == PI_UDMA_DATAMOVE_TRF_LINEAR)
    {
        tx_chan = itf_data->tx_lin_chan_id;
    }
    else
    {
        tx_chan = itf_data->tx_2d_chan_id;
    }

    if (0 == itf_data->device_id)
    {
        /* set the channels to use */
        //hal_udma_ctrl_datamove0_cfg_set_ids(tx_chan, rx_chan);
        udma_ctrl_datamove_cfg_source_id_0_set(udma_ctrl_base, tx_chan);
        udma_ctrl_datamove_cfg_dest_id_0_set(udma_ctrl_base, rx_chan);

        /* launch the copy, also activate clock for udma channels */
        //hal_udma_ctrl_datamove0_enable();
        udma_ctrl_datamove0_size_en_set(udma_ctrl_base, 1);
    }
    else
    {
        /* set the channels to use */
        //hal_udma_ctrl_datamove1_cfg_set_ids(tx_chan, rx_chan);
        udma_ctrl_datamove_cfg_source_id_1_set(udma_ctrl_base, tx_chan);
        udma_ctrl_datamove_cfg_dest_id_1_set(udma_ctrl_base, rx_chan);

        /* launch the copy, also activate clock for udma channels */
        //hal_udma_ctrl_datamove1_enable();
        udma_ctrl_datamove1_size_en_set(udma_ctrl_base, 1);
    }

    /* setup and launch channels */
    if (dev_data->dst_trf_cfg.type == PI_UDMA_DATAMOVE_TRF_LINEAR)
    {
        uint32_t udma_core = pi_udma_core_lin_addr_get(rx_chan);
        uint32_t config = UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1);
        pi_udma_core_lin_enqueue(udma_core, dst, len, config);
    }
    else
    {
        uint32_t udma_core = pi_udma_core_2d_addr_get(rx_chan - UDMA_NB_CHAN_LIN);
        uint32_t config = UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN(1);
        pi_udma_core_2d_enqueue(udma_core, dst, 0, len, dev_data->dst_trf_cfg.stride,
                                dev_data->dst_trf_cfg.row_len, config);
    }

    if (dev_data->src_trf_cfg.type == PI_UDMA_DATAMOVE_TRF_LINEAR)
    {
        uint32_t udma_core = pi_udma_core_lin_addr_get(tx_chan);
        uint32_t config = UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1);
        pi_udma_core_lin_enqueue(udma_core, src, len, config);
    }
    else
    {
        uint32_t udma_core = pi_udma_core_2d_addr_get(tx_chan - UDMA_NB_CHAN_LIN);
        uint32_t config = UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN(1);
        pi_udma_core_2d_enqueue(udma_core, src, 0, len, dev_data->src_trf_cfg.stride,
                                dev_data->src_trf_cfg.row_len, config);
    }
}


static void __pi_udma_datamove_event_handler(uint32_t event, void* arg)
{
    pi_udma_datamove_itf_data_t* itf_data = (pi_udma_datamove_itf_data_t*) arg;
    pi_task_t* task = itf_data->end_task;
    uint32_t udma_ctrl_base = (uint32_t) ARCHI_UDMA_ADDR;

    /* stop the DATAMOVE */
    if (0 == itf_data->device_id)
    {
        //hal_udma_ctrl_datamove0_stop();
        udma_ctrl_datamove0_size_stop_set(udma_ctrl_base, 1);
    }
    else
    {
        //hal_udma_ctrl_datamove1_stop();
        udma_ctrl_datamove1_size_stop_set(udma_ctrl_base, 1);
    }

    /* handle current task end */
    if (task != NULL)
    {
       pi_task_push_irq_safe(task);
    }

    itf_data->end_task = NULL;

    /* start new task if needed */
    pi_task_t *next_task = __pi_udma_datamove_task_fifo_pop(itf_data);
    if (next_task)
    {
        itf_data->end_task = next_task;
        __pi_udma_datamove_copy_start(itf_data);
    }
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_udma_datamove_conf_init(pi_udma_datamove_conf_t *conf)
{
    conf->device_id = 0;
    conf->src_trf_cfg.type = PI_UDMA_DATAMOVE_TRF_LINEAR;
    conf->src_trf_cfg.row_len = 0;
    conf->src_trf_cfg.stride = 0;
    conf->dst_trf_cfg.type = PI_UDMA_DATAMOVE_TRF_LINEAR;
    conf->dst_trf_cfg.row_len = 0;
    conf->dst_trf_cfg.stride = 0;
}

int pi_udma_datamove_open(pi_device_t *device)
{
    int status = PI_OK;
    uint32_t irq = pi_irq_disable();
    pi_udma_datamove_conf_t *conf = (pi_udma_datamove_conf_t*) device->config;

    if (conf->device_id >= __UDMA_NB_DATAMOVE)
    {
        pi_irq_restore(irq);
        return PI_FAIL;
    }

    pi_udma_datamove_itf_data_t* itf_data = g_udma_datamove_itf_data[conf->device_id];

    if (NULL == itf_data)
    {
        /* allocate itf data */
        itf_data = pi_fc_l1_malloc(sizeof(pi_udma_datamove_itf_data_t));
        if (NULL == itf_data)
        {
            pi_irq_restore(irq);
            return PI_ERR_NO_MEM;
        }
        g_udma_datamove_itf_data[conf->device_id] = itf_data;

        /* allocate lin channels */
        itf_data->rx_lin_chan_id = pi_udma_core_lin_alloc();
        itf_data->tx_lin_chan_id = pi_udma_core_lin_alloc();
        if ((0 > itf_data->rx_lin_chan_id) || (0 > itf_data->tx_lin_chan_id))
        {
            pi_udma_core_lin_free(itf_data->rx_lin_chan_id);
            pi_udma_core_lin_free(itf_data->tx_lin_chan_id);
            pi_fc_l1_free(itf_data, sizeof(pi_udma_datamove_itf_data_t));
            g_udma_datamove_itf_data[conf->device_id] = NULL;
            pi_irq_restore(irq);
            return PI_FAIL;
        }

        /* enable lin events */
        pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_lin_chan_id));
        pi_fc_event_handler_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_lin_chan_id),
                                __pi_udma_datamove_event_handler, itf_data);
        /* allocate 2d channels */
        itf_data->rx_2d_chan_id = pi_udma_core_2d_alloc();
        itf_data->tx_2d_chan_id = pi_udma_core_2d_alloc();
        if ((itf_data->rx_2d_chan_id < 0) || (itf_data->tx_2d_chan_id < 0))
        {
            pi_udma_core_2d_free(itf_data->rx_2d_chan_id);
            pi_udma_core_2d_free(itf_data->tx_2d_chan_id);
            pi_udma_core_lin_free(itf_data->rx_lin_chan_id);
            pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_lin_chan_id));
            pi_udma_core_lin_free(itf_data->tx_lin_chan_id);
            pi_fc_l1_free(itf_data, sizeof(pi_udma_datamove_itf_data_t));
            g_udma_datamove_itf_data[conf->device_id] = NULL;
            pi_irq_restore(irq);
            return PI_FAIL;
        }
        /* enable 2D events */
        pi_soc_eu_fc_mask_set(itf_data->rx_2d_chan_id);
        pi_fc_event_handler_set(itf_data->rx_2d_chan_id,
                                __pi_udma_datamove_event_handler, itf_data);

        /* Initialize itf data */
        itf_data->nb_open = 1;
        itf_data->end_task = NULL;
        itf_data->fifo_head = NULL;
        itf_data->fifo_tail = NULL;
        itf_data->device_id = conf->device_id;
    }
    else
    {
        itf_data->nb_open++;
    }

    /* allocate device data */
    device->data = pi_fc_l1_malloc(sizeof(pi_udma_datamove_data_t));
    if (NULL == device->data)
    {
        /* the device we are opening initialized the interface */
        /* we need to close it */
        if (itf_data->nb_open == 1)
        {
            /* clear events, disable IRQs && free allocated udma channels */
            pi_udma_core_2d_free(itf_data->rx_2d_chan_id);
            pi_udma_core_2d_free(itf_data->tx_2d_chan_id);
            pi_udma_core_lin_free(itf_data->rx_lin_chan_id);
            pi_udma_core_lin_free(itf_data->tx_lin_chan_id);
            pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_lin_chan_id));
            pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_2D((itf_data->rx_2d_chan_id - UDMA_NB_CHAN_LIN)));
            /* free itf data */
            g_udma_datamove_itf_data[itf_data->device_id] = NULL;
            pi_fc_l1_free(itf_data, sizeof(pi_udma_datamove_itf_data_t));
        }
        pi_irq_restore(irq);
        return PI_ERR_NO_MEM;
    }

    /* initialize device data */
    pi_udma_datamove_data_t* dev_data = (pi_udma_datamove_data_t*) device->data;
    dev_data->itf_data = itf_data;
    dev_data->src_trf_cfg.type = conf->src_trf_cfg.type;
    dev_data->src_trf_cfg.row_len = conf->src_trf_cfg.row_len;
    dev_data->src_trf_cfg.stride = conf->src_trf_cfg.stride;
    dev_data->dst_trf_cfg.type = conf->dst_trf_cfg.type;
    dev_data->dst_trf_cfg.row_len = conf->dst_trf_cfg.row_len;
    dev_data->dst_trf_cfg.stride = conf->dst_trf_cfg.stride;

    pi_irq_restore(irq);
    return status;
}

void pi_udma_datamove_close(pi_device_t *device)
{
    uint32_t irq = pi_irq_disable();
    pi_udma_datamove_data_t* dev_data = (pi_udma_datamove_data_t*) device->data;
    pi_udma_datamove_itf_data_t* itf_data = dev_data->itf_data;

    itf_data->nb_open--;

    if (0 == itf_data->nb_open)
    {
        /* clear events, disable IRQs && free allocated udma channels */
        pi_udma_core_2d_free(itf_data->rx_2d_chan_id);
        pi_udma_core_2d_free(itf_data->tx_2d_chan_id);
        pi_udma_core_lin_free(itf_data->rx_lin_chan_id);
        pi_udma_core_lin_free(itf_data->tx_lin_chan_id);
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_lin_chan_id));
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_2D((itf_data->rx_2d_chan_id - UDMA_NB_CHAN_LIN)));
        /* free itf data */
        g_udma_datamove_itf_data[itf_data->device_id] = NULL;
        pi_fc_l1_free(itf_data, sizeof(pi_udma_datamove_itf_data_t));
    }

    /* free device data */
    pi_fc_l1_free(dev_data, sizeof(pi_udma_datamove_data_t));

    pi_irq_restore(irq);
}

int32_t pi_udma_datamove_copy_async(pi_device_t *device, void* src, void* dst,
                                    uint32_t len, pi_task_t* task)
{
    uint32_t irq = pi_irq_disable();

    pi_udma_datamove_data_t *dev_data = (pi_udma_datamove_data_t*) device->data;
    task->data[0] = (uint32_t) src;
    task->data[1] = (uint32_t) dst;
    task->data[2] = (uint32_t) len;
    task->data[3] = (uint32_t) dev_data;

    pi_udma_datamove_itf_data_t *itf_data = dev_data->itf_data;

    /* if a request is in progress, enqueue this one */
    /* else, execute it */
    if (NULL == itf_data->end_task)
    {
        itf_data->end_task = task;
        __pi_udma_datamove_copy_start(itf_data);
    }
    else
    {
        __pi_udma_datamove_task_fifo_enqueue(itf_data, task);
    }

    pi_irq_restore(irq);
    return PI_OK;
}

int32_t pi_udma_datamove_copy(pi_device_t *device, void* src, void* dst, uint32_t len)
{
    pi_task_t task;
    pi_task_block(&task);
    int32_t status = pi_udma_datamove_copy_async(device, src, dst, len, &task);
    if (PI_OK == status)
    {
        pi_task_wait_on(&task);
    }
    pi_task_destroy(&task);
    return status;
}
