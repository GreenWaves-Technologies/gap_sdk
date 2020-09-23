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

#ifndef __PMSIS_DRIVER_UDMA_UDMA_CORE_UDMA_UTIL_H__
#define __PMSIS_DRIVER_UDMA_UDMA_CORE_UDMA_UTIL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct
{
    pi_task_t *hw_buffer[2];    /*!< UDMA hw buffers. */
    pi_task_t *fifo_head;       /*!< SW fifo head. */
    pi_task_t *fifo_tail;       /*!< SW fifo tail. */
} pi_udma_fifo_t;

/**
 * struct pi_task data field array desc :
 * data[0] = l2_buffer
 * data[1] = size
 * data[2] = channel
 */

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Initialize UDMA fifo.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 */
static inline void pi_udma_fifo_init(pi_udma_fifo_t *fifo)
{
    fifo->hw_buffer[0] = NULL;
    fifo->hw_buffer[1] = NULL;
    fifo->fifo_head = NULL;
    fifo->fifo_tail = NULL;
}

/**
 * \brief Get free HW slot ID.
 *
 * This function returns ID of first free UDMA HW slot.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 *
 * \retval -1            No free slot.
 * \retval slot_id       ID of free slot.
 */
static inline int32_t pi_udma_fifo_hw_buffer_empty(pi_udma_fifo_t *fifo)
{
    if (fifo->hw_buffer[0] == NULL)
    {
        return 0;
    }
    else if (fifo->hw_buffer[1] == NULL)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/**
 * \brief Enqueue a task in HW buffer.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 * \param task           Pointer to task to enqueue.
 * \param hw_buff_idx    HW slot ID.
 */
static inline void pi_udma_fifo_hw_fifo_task_enqueue(pi_udma_fifo_t *fifo,
                                                     pi_task_t *task, uint8_t hw_buff_idx)
{
    uint32_t irq = disable_irq();
    fifo->hw_buffer[hw_buff_idx] = task;
    restore_irq(irq);
}

/**
 * \brief Pop a task from HW buffer.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 *
 * \retval Task          Pointer to current struct pi_task.
 * \retval NULL          If HW buffers are empty.
 */
static inline pi_task_t *pi_udma_fifo_hw_fifo_task_pop(pi_udma_fifo_t *fifo)
{
    pi_task_t *task_to_return = NULL;
    //uint32_t irq = disable_irq();
    task_to_return = fifo->hw_buffer[0];
    fifo->hw_buffer[0] = fifo->hw_buffer[1];
    fifo->hw_buffer[1] = NULL;
    //restore_irq(irq);
    return task_to_return;
}

/**
 * \brief Get current task in HW buffer.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 * \param hw_buff_idx    HW slot ID.
 *
 * \retval Task          Pointer to current struct pi_task.
 * \retval NULL          If HW buffer is empty.
 */
static inline pi_task_t *pi_udma_fifo_hw_fifo_current_task_get(pi_udma_fifo_t *fifo,
                                                               uint8_t hw_buff_idx)
{
    pi_task_t *task_to_return = NULL;
    task_to_return = fifo->hw_buffer[hw_buff_idx];
    return task_to_return;
}

/**
 * \brief Enqueue a task in SW fifo.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 * \param task           Pointer to task to enqueue.
 */
static inline void pi_udma_fifo_sw_fifo_task_enqueue(pi_udma_fifo_t *fifo,
                                                     pi_task_t *task)
{
    uint32_t irq = disable_irq();
    if (fifo->fifo_head == NULL)
    {
        fifo->fifo_head = task;
    }
    else
    {
        fifo->fifo_tail->next = task;
    }
    fifo->fifo_tail = task;
    restore_irq(irq);
}

/**
 * \brief Pop a task from SW fifo.
 *
 * \param fifo           Pointer to UDMA fifo struct.
 *
 * \retval Task          Pointer to first struct pi_task from SW fifo.
 * \retval NULL          If SW fifo is empty.
 */
static inline pi_task_t *pi_udma_fifo_sw_fifo_task_pop(pi_udma_fifo_t *fifo)
{
    pi_task_t *task_to_return = NULL;
    //uint32_t irq = disable_irq();
    if (fifo->fifo_head != NULL)
    {
        task_to_return = fifo->fifo_head;
        fifo->fifo_head = fifo->fifo_head->next;
    }
    //restore_irq(irq);
    return task_to_return;
}

/**
 * \brief Compute UDMA register values.
 *
 * \param task           Current transfer(struct pi_task).
 * \param buff           Pointer to L2 buffer address.
 * \param size           Pointer to size of buffer.
 * \param max_size       Max size supported by UDMA.
 */
static inline void pi_udma_fifo_buffer_set(struct pi_task *task, uint32_t *buff,
                                           uint32_t *size, uint32_t max_size)
{
    *buff = task->data[0];
    *size = task->data[1];
    if (*size > max_size)
    {
        *size = max_size;
        task->data[0] += *size;
    }
    task->data[1] -= *size;
}

#endif  /* __PMSIS_DRIVER_UDMA_UDMA_CORE_UDMA_UTIL_H__ */
