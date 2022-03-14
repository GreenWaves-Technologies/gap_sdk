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

#include "pmsis.h"

#if !defined(__FREERTOS__)
#include <archi/chips/gap9_v2/udma_ffc/udma_ffc.h>
#else
#include "chips/gap9/drivers/udma/udma_core.h"
#endif  /*  */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

typedef struct pi_ffc_data_s
{
    struct pi_ffc_itf_data_s *itf_data;
    pi_ffc_fixed_type_e fixed_type;
    uint32_t fixed_scale;
    uint32_t fixed_precision;
    pi_ffc_float_type_e float_type;
    pi_ffc_mode_e mode;
    pi_ffc_io_mode_e io_mode;
    uint8_t continuous_mode;
} pi_ffc_data_t;

typedef struct pi_ffc_itf_data_s
{
    pi_task_t *fifo_head; /*!< head of the tasks FIFO */
    pi_task_t *fifo_tail; /*!< tail of the tasks FIFO */
    pi_task_t *end_task;  /*!< current callback task */
    void* latest_conf;    /*!< last used FFC configuration */
    int32_t rx_chan_id;   /*!< RX udma channel */
    int32_t tx_chan_id;   /*!< TX udma channel */
    uint8_t device_id;    /*!< Device ID */
    int32_t nb_open;      /*!< number of devices opened */
} pi_ffc_itf_data_t;

static pi_ffc_itf_data_t* g_ffc_itf_data[ARCHI_UDMA_NB_FFC];

/********************
 * Static functions
 *******************/

static inline uint32_t __ffc_compute_udma_lin_shift(uint8_t is_rx,
                                                    pi_ffc_mode_e mode,
                                                    pi_ffc_float_type_e fl_type,
                                                    pi_ffc_fixed_type_e fp_type)
{
    uint32_t fp_shift = 0;
    uint32_t fl_shift = 0;
    switch (fl_type)
    {
    case PI_FFC_FLOAT_FP16:
    case PI_FFC_FLOAT_BFP16:
        fl_shift = 1;
        break;

    default:
        fl_shift = 2;
        break;
    }

    switch (fp_type)
    {
    case PI_FFC_FIXED_8:
        fp_shift = 0;
        break;

    case PI_FFC_FIXED_16:
        fp_shift = 1;
        break;

    default:
        fp_shift = 2;
        break;
    }

    if (0 != is_rx)
    {
        return (PI_FFC_FIXED_TO_FLOAT == mode) ? fp_shift : fl_shift;
    }
    else
    {
        return (PI_FFC_FLOAT_TO_FIXED == mode) ? fp_shift : fl_shift;
    }
}

// Has to be synchronized with irq_disabled(done in convert_async)
// since irq handler might pop at the same time
static inline void __ffc_drv_fifo_enqueue(pi_ffc_itf_data_t *itf_data,
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

static inline pi_task_t *__ffc_drv_fifo_pop(pi_ffc_itf_data_t *itf_data)
{
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
    return ret_task;
}

static void __pi_ffc_change_continuous_mode(pi_ffc_data_t* dev_data,
                                            uint8_t continuous_mode)
{
    uint32_t base = (uint32_t) UDMA_FFC_ADDR(dev_data->itf_data->device_id);

    if (continuous_mode != dev_data->continuous_mode)
    {
        if (continuous_mode != 0)
        {
            udma_ffc_start_set(base, 1);
        }
        else
        {
            udma_ffc_start_set(base, 0);
        }
        dev_data->continuous_mode = continuous_mode;
    }
}

static void __pi_ffc_change_event_source(pi_ffc_itf_data_t* itf_data)
{
    /* change event source according to io mode */
    uint32_t base = (uint32_t) UDMA_FFC_ADDR(itf_data->device_id);

    pi_ffc_data_t* dev_data = (pi_ffc_data_t*) itf_data->end_task->data[3];

    switch(dev_data->io_mode)
    {
    case PI_FFC_MEMORY_IN_MEMORY_OUT:
        //fallthrough
    case PI_FFC_STREAM_IN_MEMORY_OUT:
    {
        /* use output channel as event source */
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id));
        pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id));
        break;
    }

    case PI_FFC_MEMORY_IN_STREAM_OUT:
    {
        /* use input channel as event source */
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id));
        pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id));
        break;
    }

    case PI_FFC_STREAM_IN_STREAM_OUT:
    {
        /* FFC has no control over data flow, no event source */
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id));
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id));
        break;
    }

    default:
        //invalid choice
        break;
    }
}

static void __pi_ffc_conf_apply(pi_ffc_itf_data_t* itf_data)
{
    uint32_t base = (uint32_t) UDMA_FFC_ADDR(itf_data->device_id);

    pi_ffc_data_t* dev_data = (pi_ffc_data_t*) itf_data->end_task->data[3];

    /* retrieve current task */
    if (itf_data->latest_conf != dev_data)
    {
        itf_data->latest_conf = dev_data;

        udma_ffc_fp_format_set(base, dev_data->fixed_type);
        udma_ffc_fl_format_set(base, dev_data->float_type);
        udma_ffc_fp_prec_set(base,  dev_data->fixed_precision);
        udma_ffc_fp_scale_set(base, dev_data->fixed_scale);
        udma_ffc_mode_direction_set(base, dev_data->mode);
        udma_ffc_mode_io_mode_set(base, dev_data->io_mode);
        __pi_ffc_change_event_source(itf_data);
    }
    udma_ffc_rx_dest_set(base, itf_data->rx_chan_id);
    udma_ffc_tx_dest_set(base, itf_data->tx_chan_id);

    /* set stream as blocking */
    udma_ctrl_stream_cfg_set((uint32_t) ARCHI_UDMA_ADDR, 1 << (18 + itf_data->device_id));
}

static void __pi_ffc_conversion_start(pi_ffc_itf_data_t* itf_data)
{
    uint32_t chan_id;

    if (NULL == itf_data->end_task)
    {
        return;
    }

    uint32_t src = itf_data->end_task->data[0];
    uint32_t dst = itf_data->end_task->data[1];
    uint32_t len = itf_data->end_task->data[2];
    pi_ffc_data_t* dev_data = (pi_ffc_data_t*) itf_data->end_task->data[3];

    __pi_ffc_conf_apply(itf_data);

    uint32_t base = (uint32_t) UDMA_FFC_ADDR(itf_data->device_id);

    /* launch the conversion */
    if (dev_data->continuous_mode != 0)
    {
        udma_ffc_conv_num_set(base, 0);
    }
    else
    {
        udma_ffc_conv_num_set(base, len);
        udma_ffc_start_set(base, 1);
    }

    /* setup & launching channels */

    if (0 == (dev_data->io_mode & 2))
    {
        chan_id = udma_ffc_rx_dest_get(base);
        uint32_t udma_core = pi_udma_core_lin_addr_get(chan_id);
        uint32_t rx_shift = __ffc_compute_udma_lin_shift(0, dev_data->mode, dev_data->float_type, dev_data->fixed_type);
        pi_udma_core_lin_enqueue(udma_core, (uint32_t) dst, len << rx_shift, 0);
    }

    if (0 == (dev_data->io_mode & 1))
    {
        chan_id = udma_ffc_tx_dest_get(base);
        uint32_t udma_core = pi_udma_core_lin_addr_get(chan_id);
        uint32_t tx_shift = __ffc_compute_udma_lin_shift(1, dev_data->mode, dev_data->float_type, dev_data->fixed_type);
        pi_udma_core_lin_enqueue(udma_core, (uint32_t) src, len << tx_shift, 0);
    }
}

/********************
 * Callback
 ********************/

static void __pi_ffc_event_handler(uint32_t event, void *arg)
{

    pi_ffc_itf_data_t* itf_data = (pi_ffc_itf_data_t*) arg;
    pi_task_t* task = itf_data->end_task;

    /* handle current task end */
    if (task != NULL)
    {
        pi_task_push_irq_safe(task);
    }

    itf_data->end_task = NULL;

    /* start new task if needed */
    pi_task_t *next_task = __ffc_drv_fifo_pop(itf_data);
    if (next_task)
    {
        itf_data->end_task = next_task;
        __pi_ffc_conversion_start(itf_data);
    }
}


/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_ffc_conf_init(pi_ffc_conf_t *conf)
{
    conf->itf = 0;
    conf->mode = PI_FFC_FLOAT_TO_FIXED;
    conf->io_mode = PI_FFC_MEMORY_IN_MEMORY_OUT;
    conf->fixed_type = PI_FFC_FIXED_32;
    conf->fixed_scale = 0;
    conf->fixed_precision = 0;
    conf->float_type = PI_FFC_FLOAT_FP32;
}

int pi_ffc_open(pi_device_t *device)
{
    uint32_t irq = pi_irq_disable();
    pi_ffc_conf_t *conf = (pi_ffc_conf_t*) device->config;
    pi_ffc_itf_data_t *itf_data = g_ffc_itf_data[conf->itf];

    if (NULL == itf_data)
    {
        /* allocate itf data */
        itf_data = pi_fc_l1_malloc(sizeof(pi_ffc_itf_data_t));
        if (NULL == itf_data)
        {
            pi_irq_restore(irq);
            return PI_ERR_NO_MEM;
        }
        g_ffc_itf_data[conf->itf] = itf_data;

        /* allocate 2 udma lin channels */
        /* set both of them to trigger event handler, io_mode will decide which
         * one to use */
        int32_t tx_chan_id = pi_udma_core_lin_alloc();
        int32_t rx_chan_id = pi_udma_core_lin_alloc();
        if (rx_chan_id < 0 || tx_chan_id < 0)
        {
            pi_fc_l1_free(itf_data, sizeof(pi_ffc_itf_data_t));
            pi_irq_restore(irq);
            return PI_FAIL;
        }
        itf_data->rx_chan_id = rx_chan_id;
        itf_data->tx_chan_id = tx_chan_id;

        pi_fc_event_handler_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id),
                                __pi_ffc_event_handler, itf_data);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id),
                                __pi_ffc_event_handler, itf_data);
        /* use rx as default */
        pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id));

        itf_data->nb_open = 1;
        itf_data->device_id = conf->itf;
        itf_data->fifo_head = NULL;
        itf_data->fifo_tail = NULL;
        itf_data->end_task = NULL;
        itf_data->latest_conf = NULL;

        /* disable udma reset before setting regs */
        uint32_t periph_id = ARCHI_UDMA_FFC_ID(itf_data->device_id);
        udma_ctrl_cfg_rstn_set_set(ARCHI_UDMA_ADDR, (1 << periph_id));
        udma_ctrl_cfg_cg_set_set(ARCHI_UDMA_ADDR, (1 << periph_id));
    }
    else
    {
        itf_data->nb_open++;
    }

    /* allocate device data */
    device->data = pi_fc_l1_malloc(sizeof(pi_ffc_data_t));
    if (NULL == device->data)
    {
        pi_irq_restore(irq);
        return PI_ERR_L2_NO_MEM;
    }

    /* initialize device data */
    pi_ffc_data_t* dev_data = (pi_ffc_data_t*) device->data;
    dev_data->itf_data = itf_data;
    dev_data->fixed_type = conf->fixed_type;
    dev_data->fixed_scale = conf->fixed_scale;
    dev_data->fixed_precision = conf->fixed_precision;
    dev_data->float_type = conf->float_type;
    dev_data->mode = conf->mode;
    dev_data->io_mode = conf->io_mode;
    dev_data->continuous_mode = 0; /* continuous mode disabled by default */

    pi_irq_restore(irq);
    return PI_OK;
}

void pi_ffc_close(pi_device_t *device)
{
    uint32_t irq = pi_irq_disable();
    pi_ffc_data_t *dev_data = (pi_ffc_data_t*) device->data;
    pi_ffc_itf_data_t* itf_data = dev_data->itf_data;

    /* decrement number of devices opened */
    itf_data->nb_open--;

    if (itf_data->nb_open == 0)
    {
        /* clear events, disable IRQs & free allocated udma channels */
        pi_udma_core_lin_reset(pi_udma_core_lin_addr_get(itf_data->rx_chan_id));
        pi_udma_core_lin_reset(pi_udma_core_lin_addr_get(itf_data->tx_chan_id));

        pi_udma_core_lin_free(itf_data->rx_chan_id);
        pi_udma_core_lin_free(itf_data->tx_chan_id);

        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->rx_chan_id));
        pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_CHAN_LIN(itf_data->tx_chan_id));

        uint32_t periph_id = ARCHI_UDMA_FFC_ID(itf_data->device_id);
        udma_ctrl_cfg_rstn_clr_set(ARCHI_UDMA_ADDR, (1 << periph_id));
        udma_ctrl_cfg_cg_clr_set(ARCHI_UDMA_ADDR, (1 << periph_id));

        /* free itf data */
        g_ffc_itf_data[itf_data->device_id] = NULL;
        pi_fc_l1_free(itf_data, sizeof(pi_ffc_itf_data_t));
    }
    /* free device data */
    pi_fc_l1_free(device->data, sizeof(pi_ffc_data_t));
    pi_irq_restore(irq);
}

int32_t pi_ffc_ioctl(pi_device_t *device, uint32_t cmd, void *arg)
{
    uint32_t irq = pi_irq_disable();
    switch (cmd)
    {
    case PI_FFC_IOCTL_SET_IO_MODE:
    {
        pi_ffc_data_t *dev_data = (pi_ffc_data_t*) device->data;
        dev_data->io_mode = (pi_ffc_io_mode_e)((uintptr_t) arg);
        /* make last config invalid */
        dev_data->itf_data->latest_conf = NULL;
        break;
    }

    case PI_FFC_IOCTL_CONTINUOUS_ENABLE:
    {
        pi_ffc_data_t *dev_data = (pi_ffc_data_t*) device->data;

        uint8_t continuous_mode = (uint8_t)((uintptr_t) arg);
        __pi_ffc_change_continuous_mode(dev_data, continuous_mode);
        /* make last config invalid */
        dev_data->itf_data->latest_conf = NULL;
        break;
    }

    default:
        pi_irq_restore(irq);
        return PI_FAIL;
    }
    pi_irq_restore(irq);
    return PI_OK;
}

void pi_ffc_convert(pi_device_t *device, void* src, void* dst, uint16_t size)
{
    pi_task_t block;
    pi_task_block(&block);
    pi_ffc_convert_async(device, src, dst, size, &block);
    pi_task_wait_on(&block);
    pi_task_destroy(&block);
}

void pi_ffc_convert_async(pi_device_t* device, void* src, void* dst,
                          uint16_t size, pi_task_t* task)
{
    uint32_t irq = pi_irq_disable();

    pi_ffc_data_t *dev_data = (pi_ffc_data_t*) device->data;
    task->data[0] = (uint32_t) src;
    task->data[1] = (uint32_t) dst;
    task->data[2] = (uint32_t) size;
    task->data[3] = (uint32_t) dev_data;

    pi_ffc_itf_data_t *itf_data = dev_data->itf_data;

    /* if a request is in progress, enqueue this one */
    /* else, execute it */
    if (NULL == itf_data->end_task)
    {
        itf_data->end_task = task;
        __pi_ffc_conversion_start(itf_data);
    }
    else
    {
        __ffc_drv_fifo_enqueue(itf_data, task);
    }

    pi_irq_restore(irq);
}
