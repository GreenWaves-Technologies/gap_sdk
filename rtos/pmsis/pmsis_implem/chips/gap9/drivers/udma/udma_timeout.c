/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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
#include "chips/gap9/drivers/udma/udma_timeout.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if !defined(__FREERTOS__)
#define UDMA_NB_TIMEOUT       ( 8 )
#endif  /* __FREERTOS__ */

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static struct pi_udma_timeout_s *g_udma_timeout[UDMA_NB_TIMEOUT];

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Event handler. */
static void __pi_udma_timeout_event_handler(uint32_t event, void *arg);

/* Enqueue task in SW fifo. */
static inline void __pi_udma_timeout_task_push(struct pi_udma_timeout_s *driver_data,
                                               pi_task_t *task);

/* Pop task from SW fifo. */
static inline pi_task_t *__pi_udma_timeout_task_pop(struct pi_udma_timeout_s *driver_data);

/* Start a UDMA timer, when in SW trigger mode. */
static void __pi_udma_timeout_start(uint8_t timeout_id);

/* Stop a UDMA timer. */
static void __pi_udma_timeout_stop(uint8_t timeout_id);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

/** TIMEOUT_PRE Register. */
static inline uint32_t __pi_udma_ctrl_timeout_prescaler_conf_get(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_PRE0_OFFSET + (timeout_id << 3));
    uint32_t prescaler = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    return prescaler;
}

static inline uint32_t __pi_udma_ctrl_timeout_prescaler_enabled(uint8_t timeout_id)
{
    uint32_t prescaler = __pi_udma_ctrl_timeout_prescaler_conf_get(timeout_id);
    prescaler &= UDMA_CTRL_TIMEOUT_PRE0_EN_MASK;
    prescaler >>= UDMA_CTRL_TIMEOUT_PRE0_EN_BIT;
    return prescaler;
}

static inline void __pi_udma_ctrl_timeout_prescaler_set(uint8_t timeout_id,
                                                       uint16_t presc_value,
                                                       uint8_t enable)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_PRE0_OFFSET + (timeout_id << 3));
    uint32_t prescaler = (UDMA_CTRL_TIMEOUT_PRE0_CNT(presc_value) |
                          UDMA_CTRL_TIMEOUT_PRE0_EN(enable));
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, prescaler);
}

static inline void __pi_udma_ctrl_timeout_prescaler_reset(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_PRE0_OFFSET + (timeout_id << 3));
    udma_ctrl_timeout_pre0_t prescaler = {0};
    prescaler.raw = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    prescaler.clr = 1;
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, prescaler.raw);
}

static inline void __pi_udma_ctrl_timeout_prescaler_start(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_PRE0_OFFSET + (timeout_id << 3));
    udma_ctrl_timeout_pre0_t prescaler = {0};
    prescaler.raw = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    prescaler.en = 1;
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, prescaler.raw);
}

static inline void __pi_udma_ctrl_timeout_prescaler_stop(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_PRE0_OFFSET + (timeout_id << 3));
    udma_ctrl_timeout_pre0_t prescaler = {0};
    prescaler.raw = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    prescaler.en = 0;
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, prescaler.raw);
}


/** TIMEOUT_CHX Register. */
static inline uint32_t __pi_udma_ctrl_timeout_timeout_get(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_CH0_OFFSET + (timeout_id << 3));
    uint32_t timeout = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    return timeout;
}

static inline void __pi_udma_ctrl_timeout_timeout_set(uint8_t timeout_id,
                                                     uint8_t udma_chan_id,
                                                     uint8_t mode,
                                                     uint16_t timeout_val,
                                                     uint8_t enable)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_CH0_OFFSET + (timeout_id << 3));
    uint32_t timeout = (UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID(udma_chan_id) |
                        UDMA_CTRL_TIMEOUT_CH0_MODE(mode) |
                        UDMA_CTRL_TIMEOUT_CH0_EN(enable) |
                        UDMA_CTRL_TIMEOUT_CH0_CNT(timeout_val));
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, timeout);

}
static inline void __pi_udma_ctrl_timeout_mode_set(uint8_t timeout_id, uint8_t mode)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_CH0_OFFSET + (timeout_id << 3));
    udma_ctrl_timeout_ch0_t timeout = {0};
    timeout.raw = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    timeout.mode = mode;
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, timeout.raw);
}

static inline void __pi_udma_ctrl_timeout_timeout_start(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_CH0_OFFSET + (timeout_id << 3));
    udma_ctrl_timeout_ch0_t timeout = {0};
    timeout.raw = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    timeout.en = 1;
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, timeout.raw);
}

static inline void __pi_udma_ctrl_timeout_timeout_stop(uint8_t timeout_id)
{
    uint32_t reg_offset = (UDMA_CTRL_TIMEOUT_CH0_OFFSET + (timeout_id << 3));
    udma_ctrl_timeout_ch0_t timeout = {0};
    timeout.raw = GAP_READ((uint32_t) ARCHI_UDMA_ADDR, reg_offset);
    timeout.en = 0;
    GAP_WRITE((uint32_t) ARCHI_UDMA_ADDR, reg_offset, timeout.raw);
}


static void __pi_udma_timeout_event_handler(uint32_t event, void *arg)
{
    uint32_t timeout_id = (uint32_t) arg;
    //TIMEOUT_TRACE("Timeout id=%ld event\n", timeout_id);
    struct pi_udma_timeout_s *driver_data = g_udma_timeout[timeout_id];
    struct pi_task *task = __pi_udma_timeout_task_pop(driver_data);
    __pi_udma_timeout_stop(timeout_id);
    if (task != NULL)
    {
        //TIMEOUT_TRACE("Handle task=%lx\n", task);
        //printf("Handle task=%lx, arg=%lx\n", task, task->arg[3]);
        /* Timeout reached, abort transfer. */
        pi_callback_func_t func = (pi_callback_func_t) task->arg[2];
        func((void *) task->arg[3]);

        /* Set transfer end result. */
        task->arg[2] = -1;

        /* Release event task. */
        pi_task_push(task);
    }
}

static inline void __pi_udma_timeout_task_push(struct pi_udma_timeout_s *driver_data,
                                               pi_task_t *task)
{
    uint32_t irq = disable_irq();
    if ((driver_data->fifo_head == NULL) || (driver_data->fifo_head == (void *) 0xFFFFFFFF))
    {
        driver_data->fifo_head = task;
    }
    else
    {
        driver_data->fifo_tail->next = task;
    }
    driver_data->fifo_tail = task;
    restore_irq(irq);
}

static inline pi_task_t *__pi_udma_timeout_task_pop(struct pi_udma_timeout_s *driver_data)
{
    pi_task_t *task_return = NULL;
    if (driver_data->fifo_head != NULL)
    {
        task_return = driver_data->fifo_head;
        driver_data->fifo_head = driver_data->fifo_head->next;
    }
    return task_return;
}

static void __pi_udma_timeout_start(uint8_t timeout_id)
{
    __pi_udma_ctrl_timeout_prescaler_stop(timeout_id);
    __pi_udma_ctrl_timeout_prescaler_reset(timeout_id);
    __pi_udma_ctrl_timeout_timeout_start(timeout_id);
    __pi_udma_ctrl_timeout_prescaler_start(timeout_id);
}

static void __pi_udma_timeout_stop(uint8_t timeout_id)
{
    __pi_udma_ctrl_timeout_prescaler_stop(timeout_id);
    __pi_udma_ctrl_timeout_timeout_stop(timeout_id);
}

int32_t pi_udma_timeout_config_set(pi_task_t *task, uint8_t timeout_id,
                                   uint8_t udma_chan_id, uint32_t timeout_us)
{
    int32_t status = 0;
    /* Fast clock used by timeout. */
    float periph_freq = (float) ARCHI_FLL_REF_CLOCK; /* 24MHz. */
    float nb_tick_us = periph_freq / 1000000.0;
    float timeout_val = ((float) timeout_us) * nb_tick_us;
    uint16_t prescaler = 0;
    TIMEOUT_TRACE("Timeout_%d : configure udma_chan=%d, timeout_us=%ld, task=%lx\n",
                  timeout_id, udma_chan_id, timeout_us, task);
    TIMEOUT_TRACE("Periph_freq=%f, timeout_us=%ld, nb_tick_us=%f, timeout_val=%f\n",
                  periph_freq, timeout_us, nb_tick_us, timeout_val);
    prescaler = (timeout_val > 0xFFFF);
    while (timeout_val > 0xFFFF)
    {
        prescaler <<= 1;
        timeout_val /= 2;
    }
    if (__pi_udma_ctrl_timeout_prescaler_enabled(timeout_id))
    {
        /* Timeout already in use. */
        TIMEOUT_TRACE_ERR("Timeout id=%ld already in use\n", timeout_id);
        return -11;
    }

    __pi_udma_timeout_task_push(g_udma_timeout[timeout_id], task);

    uint32_t mode = g_udma_timeout[timeout_id]->mode;
    uint8_t enable = (mode != PI_UDMA_TIMEOUT_MODE_SW_TRIGGER);
    TIMEOUT_TRACE("Timeout_%d : mode=%d, prescaler=%d, timeout=%f, enable=%d\n",
                  timeout_id, mode, prescaler, timeout_val, enable);
    __pi_udma_ctrl_timeout_prescaler_set(timeout_id, prescaler, enable);
    __pi_udma_ctrl_timeout_timeout_set(timeout_id, udma_chan_id, mode,
                                      (uint16_t) timeout_val, enable);
    __pi_udma_ctrl_timeout_prescaler_reset(timeout_id);
    return status;
}

pi_task_t *__pi_udma_timeout_task_remove(uint8_t timeout_id)
{
    return __pi_udma_timeout_task_pop(g_udma_timeout[timeout_id]);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

int32_t pi_udma_timeout_alloc(pi_udma_timeout_mode_e mode)
{
    int32_t timeout_id = -1;
    for (uint32_t tid = 0; tid < (uint32_t) UDMA_NB_TIMEOUT; tid++)
    {
        if ((g_udma_timeout[tid] == NULL) || (g_udma_timeout[tid]->fifo_head == NULL))
        {
            /* Alloc UDMA timeout struct. */
            g_udma_timeout[tid] = pi_fc_l1_malloc(sizeof(struct pi_udma_timeout_s));
            if (g_udma_timeout[tid] == NULL)
            {
                TIMEOUT_TRACE_ERR("Timeout struct alloc failed !\n");
                timeout_id = -11;
                break;
            }
            g_udma_timeout[tid]->fifo_head = (void *) 0xFFFFFFFF;
            g_udma_timeout[tid]->mode = mode;
            /* Set FC event handler. */
            pi_fc_event_handler_set(SOC_EVENT_UDMA_TIMEOUT(tid),
                                    __pi_udma_timeout_event_handler,
                                    (void *) tid);
            /* Enable SoC events propagation to FC. */
            pi_soc_eu_fc_mask_set(SOC_EVENT_UDMA_TIMEOUT(tid));
            timeout_id = tid;
            TIMEOUT_TRACE("Timeout id=%ld allocated\n", timeout_id);
            break;
        }
    }
    return timeout_id;
}

void pi_udma_timeout_free(int32_t timeout_id)
{
    TIMEOUT_TRACE("Timeout_%ld : free timeout.\n", timeout_id);
    g_udma_timeout[timeout_id]->fifo_head = NULL;
    /* Free UDMA timeout struct. */
    pi_fc_l1_free(g_udma_timeout[timeout_id], sizeof(struct pi_udma_timeout_s));
    /* Clear FC event handler. */
    pi_fc_event_handler_clear(SOC_EVENT_UDMA_TIMEOUT(timeout_id));
    /* Disable SoC events propagation. */
    pi_soc_eu_fc_mask_clear(SOC_EVENT_UDMA_TIMEOUT(timeout_id));
}

int32_t pi_udma_timeout_ioctl(int32_t timeout_id, uint32_t cmd, void *arg)
{
    TIMEOUT_TRACE("Timeout_%ld : ioctl cmd=%ld, arg=%lx\n", timeout_id, cmd, arg);
    int32_t status = 0;
    switch (cmd)
    {
    case PI_UDMA_TIMEOUT_IOCTL_START :
        __pi_udma_timeout_start(timeout_id);
        break;

    case PI_UDMA_TIMEOUT_IOCTL_STOP :
        __pi_udma_timeout_stop(timeout_id);
        break;

    default :
        status = -1;
    }
    return status;
}
