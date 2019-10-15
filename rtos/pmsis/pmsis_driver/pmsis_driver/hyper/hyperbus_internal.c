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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define __PI_HYPER_TEMP_BUFFER_SIZE (128)
#define __PI_HYPER_NB_PADS          (13)

/*******************************************************************************
 * Driver data
 *****************************************************************************/
/* For burst. */
static udma_channel_e pending_channel;
static uint32_t pending_device_id;
static uint32_t pending_hyper_addr;
static uint32_t pending_buffer;
static uint32_t pending_repeat;
static uint32_t pending_repeat_size;

/* Emulated transfers. */
static udma_channel_e pending_emu_channel;
static uint32_t pending_emu_device_id;
static uint32_t pending_emu_hyper_addr;
static uint32_t pending_emu_buffer;
static uint32_t pending_emu_size;
static uint32_t pending_emu_size_2d;
static uint32_t pending_emu_length;
static uint32_t pending_emu_stride;
static uint32_t pending_emu_do_memcpy;
static struct pi_task *pending_emu_task;

struct hyper_driver_fifo *__global_hyper_driver_fifo[UDMA_NB_HYPER];

static struct pi_task *__pi_hyper_end_task;

static uint8_t __pi_hyper_temp_buffer[__PI_HYPER_TEMP_BUFFER_SIZE];

static pi_pad_e __hyper_pad[__PI_HYPER_NB_PADS] = {
    PI_PAD_36_A15_I2S1_WS,      /* HYPER_CKN. */
    PI_PAD_37_B14_I2S1_SDI,     /* HYPER_CK. */
    PI_PAD_40_D2_SPIM0_SDIO0,   /* HYPER_DQ[0]. */
    PI_PAD_41_A11_SPIM0_SDIO1,  /* HYPER_DQ[1]. */
    PI_PAD_42_B10_SPIM0_SDIO2,  /* HYPER_DQ[2]. */
    PI_PAD_43_A10_SPIM0_SDIO3,  /* HYPER_DQ[3]. */
    PI_PAD_44_B8_SPIM0_CSN0,    /* HYEPR_DQ[4]. */
    PI_PAD_45_A8_SPIM0_CSN1,    /* HYPER_DQ[5]. */
    PI_PAD_46_B7_SPIM0_SCK,     /* HYPER_DQ[6]. */
    PI_PAD_47_A9_SPIS0_CSN,     /* HYPER_DQ[7]. */
    PI_PAD_48_B15_SPIS0_MISO,   /* HYPER_CSN0. */
    PI_PAD_49_A16_SPIS0_MOSI,   /* HYPER_CSN1. */
    PI_PAD_50_B9_SPIS0_SCK      /* HYPER_RWDS. */
};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_hyper_handle_end_of_task(struct pi_task *task);
static inline void *l2_memcpy(void *dst0, const void *src0, size_t len0);
/* Enqueue a task in fifo. */
static void __pi_hyper_task_fifo_enqueue(struct hyper_driver_fifo *fifo, struct pi_task *task);
/* Pop a task from fifo. */
static struct pi_task *__pi_hyper_task_fifo_pop(struct hyper_driver_fifo *fifo);
static void exec_pending_task();
static void __pi_hyper_callback(void *arg);
static struct pi_task *__pi_hyper_prepare_callback(struct hyper_transfer_s *transfer,
                                                   struct pi_task *task);
static void __pi_hyper_copy_aligned(struct hyper_transfer_s *transfer, struct pi_task *task);
static void __pi_hyper_copy_exec(struct hyper_transfer_s *transfer, struct pi_task *task);
static void __pi_hyper_resume_emu_task();
static void __pi_hyper_copy_misaligned(struct pi_task *task);
static int32_t __pi_hyper_resume_misaligned_write(struct pi_task *task);
static int32_t __pi_hyper_resume_misaligned_read(struct pi_task *task);

/*******************************************************************************
 * Inner functions
 ******************************************************************************/

/* Hyper settings. */
void __pi_hyper_settings(uint8_t chip_select)
{
    /* MBR0 is Ram, MBR1 is Flash. */
    if (!chip_select)
    {
        hyper_mbr0_set(REG_MBR0);
        hyper_mbr1_set(REG_MBR1 >> 24);
        hyper_dt0_set(PI_HYPER_TYPE_RAM);
        hyper_dt1_set(PI_HYPER_TYPE_FLASH);
        hyper_crt0_set(MEM_ACCESS);
        hyper_crt1_set(MEM_ACCESS); /* Flash is always in mem access. */
    }
    else
    {
        hyper_mbr0_set(REG_MBR1 >> 24);
        hyper_mbr1_set(REG_MBR0);
        hyper_dt0_set(PI_HYPER_TYPE_FLASH);
        hyper_dt1_set(PI_HYPER_TYPE_RAM);
        hyper_crt0_set(MEM_ACCESS); /* Flash is always in mem access. */
    }
    /* Max length setting. */
    hyper_max_length_set(0x1ff, 1, 0, chip_select);
    hyper_max_length_set(0x1ff, 1, 1, chip_select);
    /* Timing setting. */
    hyper_timing_set(4, 4, 4, 1, 0, chip_select);
    hyper_timing_set(4, 4, 4, 1, 1, chip_select);
}

/* Pin settings. */
void __pi_hyper_pin_settings()
{
    for (uint32_t i=0; i<(uint32_t) __PI_HYPER_NB_PADS; i++)
    {
        pi_pad_set_function(__hyper_pad[i], PI_PAD_FUNC3);
    }
}

void __pi_hyper_pin_reset_settings()
{
    for (uint32_t i=0; i<(uint32_t) __PI_HYPER_NB_PADS; i++)
    {
        pi_pad_set_function(__hyper_pad[i], PI_PAD_FUNC0);
    }
}

/* L2 memcopy. */
static inline void *l2_memcpy(void *dst0, const void *src0, size_t len0)
{
  char *dst = (char *) dst0;
  char *src = (char *) src0;

  void *save = dst0;

  while (len0--)
  {
      *dst++ = *src++;
  }

  return save;
}

/* Handler. */
/* TODO    : In ASM, maybe ?
 * TODO bis: enqueue in handler  */
static void __pi_hyper_handle_end_of_task(struct pi_task *task)
{
    if(task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        if(task->id == PI_TASK_CALLBACK_ID)
        {
            pmsis_event_push(pmsis_event_get_default_scheduler(), task);
        }
    }
}

void hyper_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_HYPER_ID(0);

    if (pending_repeat != 0)
    {
        pending_buffer += pending_repeat;
        pending_hyper_addr += pending_repeat;
        pending_repeat_size -= pending_repeat;

        uint32_t iter_size = pending_repeat;
        if (pending_repeat_size <= pending_repeat)
        {
            pending_repeat = 0;
            iter_size = pending_repeat_size;
        }
        hal_hyper_enqueue(pending_device_id, pending_channel, pending_hyper_addr,
                          pending_buffer, iter_size, UDMA_CFG_EN(1));
    }
    else
    {
        /* Case end of current transfer. */
        struct pi_task *task = __pi_hyper_end_task;
        if (task != NULL)
        {
            __pi_hyper_end_task = NULL;
            __pi_hyper_handle_end_of_task(task);
        }
        /* Case pending misaligned transfer. */
        if (pending_emu_task != NULL)
        {
            __pi_hyper_resume_emu_task();
            return;
        }
        /* Case new transfer in fifo. */
        /* Maybe use a struct to get fifo of tasks. */
        task = __pi_hyper_task_fifo_pop(__global_hyper_driver_fifo[0]);
        if (task != NULL)
        {
            __pi_hyper_callback((void*)task->arg[1]);
        }
    }
}

/* TODO : Change prot to take a fifo in arg. */
/* Pass struct device for device_id. */
static void exec_pending_task()
{
    struct pi_task *task = __pi_hyper_task_fifo_pop(__global_hyper_driver_fifo[0]);

    if (task != NULL)
    {
        __pi_hyper_handle_end_of_task(task);
    }
}

/* Enqueue a task in fifo. */
static void __pi_hyper_task_fifo_enqueue(struct hyper_driver_fifo *fifo, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct hyper_driver_task *new_task = (struct hyper_driver_task *) pi_default_malloc(sizeof(struct hyper_driver_task));
    if (new_task == NULL)
    {
        __restore_irq(irq);
        return;
    }
    new_task->task = task;
    new_task->next = NULL;
    if (fifo->fifo_head == NULL)
    {
        /* Empty fifo. */
        fifo->fifo_head = new_task;
        fifo->fifo_tail = fifo->fifo_head;
    }
    else
    {
        fifo->fifo_tail->next = new_task;
        fifo->fifo_tail = fifo->fifo_tail->next;
    }
    __restore_irq(irq);
}

/* Pop a task from fifo. */
static struct pi_task *__pi_hyper_task_fifo_pop(struct hyper_driver_fifo *fifo)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    if (fifo->fifo_head != NULL)
    {
        task_to_return = fifo->fifo_head->task;
        struct hyper_driver_task *task_to_free = fifo->fifo_head;
        fifo->fifo_head = fifo->fifo_head->next;
        pi_default_free(task_to_free, sizeof(struct hyper_driver_task));
        if (fifo->fifo_head == NULL)
        {
            fifo->fifo_tail = NULL;
        }
    }
    __restore_irq(irq);
    return task_to_return;
}

/* Callback init. */
static void __pi_hyper_callback(void *arg)
{
    struct hyper_cb_args_s *cb_args = (struct hyper_cb_args_s *) arg;

    if (cb_args->transfer.stride)
    {
        __pi_hyper_copy_2d(cb_args->transfer.device_id, &cb_args->transfer, cb_args->cb);
    }
    else
    {
        __pi_hyper_copy(cb_args->transfer.device_id, &cb_args->transfer, cb_args->cb);
    }

    /* Free used structures. */
    pi_default_free(cb_args, sizeof(struct hyper_cb_args_s));
}

static struct pi_task *__pi_hyper_prepare_callback(struct hyper_transfer_s *transfer,
                                                   struct pi_task *task)
{
    struct pi_task *cb_task = (struct pi_task *) pi_default_malloc(sizeof(struct pi_task));
    struct hyper_cb_args_s *cb_args = (struct hyper_cb_args_s *) pi_default_malloc(sizeof(struct hyper_cb_args_s));
    if ((cb_task == NULL) || (cb_args == NULL))
    {
        return NULL;
    }
    /* Callback args. */
    cb_args->transfer.hyper_addr = transfer->hyper_addr;
    cb_args->transfer.buffer = transfer->buffer;
    cb_args->transfer.size = transfer->size;
    cb_args->transfer.channel = transfer->channel;
    cb_args->transfer.device_id = transfer->device_id;
    cb_args->transfer.stride = transfer->stride;
    cb_args->transfer.length = transfer->length;
    cb_args->cb = task;
    /* Callback task. */
    pi_task_callback_no_mutex(cb_task, __pi_hyper_callback, cb_args);
    cb_task->destroy = 1;

    return cb_task;
}

/* Copy functions. */
static void __pi_hyper_resume_emu_task()
{
    __pi_hyper_copy_misaligned(pending_emu_task);
}

static void __pi_hyper_copy_misaligned(struct pi_task *task)
{
    uint32_t end = 0;
    if (pending_emu_channel == TX_CHANNEL)
    {
        end = __pi_hyper_resume_misaligned_write(task);
    }
    else
    {
        end = __pi_hyper_resume_misaligned_read(task);
    }
    if (!end)
    {
        return;
    }
    exec_pending_task();
}

static void __pi_hyper_copy_aligned(struct hyper_transfer_s *transfer, struct pi_task *task)
{
    if (transfer->size > (uint32_t) BURST_SIZE)
    {
        pending_channel = transfer->channel;
        pending_device_id = transfer->device_id;
        pending_hyper_addr = transfer->hyper_addr;
        pending_buffer = (uint32_t) transfer->buffer;
        pending_repeat = (uint32_t) BURST_SIZE;
        pending_repeat_size = transfer->size;
        transfer->size = (uint32_t) BURST_SIZE;
    }
    else
    {
        pending_repeat = 0;
    }
    __pi_hyper_end_task = task;
    hal_hyper_enqueue(transfer->device_id, transfer->channel, transfer->hyper_addr,
                      (uint32_t) transfer->buffer, transfer->size, UDMA_CFG_EN(1));
}

static void __pi_hyper_copy_exec(struct hyper_transfer_s *transfer, struct pi_task *task)
{
    if ((((uint32_t) transfer->buffer & 0x3) == 0) &&
        (((uint32_t) transfer->hyper_addr & 0x1) == 0) &&
        ((((uint32_t) transfer->size & 0x3) == 0) ||
         ((transfer->channel == TX_CHANNEL) && (((uint32_t) transfer->size & 0x1) == 0))))
    {
        /* Aligned copy. */
        __pi_hyper_copy_aligned(transfer, task);
    }
    else
    {
        /* Misaligned copy. */
        pending_emu_channel = transfer->channel;
        pending_emu_device_id = transfer->device_id;
        pending_emu_hyper_addr = transfer->hyper_addr;
        pending_emu_buffer = (uint32_t) transfer->buffer;
        pending_emu_size = transfer->size;
        pending_emu_do_memcpy = 0;
        pending_emu_task = task;
        __pi_hyper_copy_misaligned(task);
    }
}

void __pi_hyper_copy(int device, struct hyper_transfer_s *transfer,
                     struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    if ((__pi_hyper_end_task != NULL) || (pending_emu_size != 0))
    {
        /* Transfer on going, enqueue this one to the list. */
        struct pi_task *task_enqueue = __pi_hyper_prepare_callback(transfer, task);
        __pi_hyper_task_fifo_enqueue(__global_hyper_driver_fifo[device], task_enqueue);
    }
    else
    {
        /* Execute the transfer. */
        __pi_hyper_copy_exec(transfer, task);
    }
    __restore_irq(irq);
}

static void __pi_hyper_copy_2d_exec(struct hyper_transfer_s *transfer, struct pi_task *task)
{
    /* Misaligned copy. */
    pending_emu_channel = transfer->channel;
    pending_emu_device_id = transfer->device_id;
    pending_emu_hyper_addr = transfer->hyper_addr;
    pending_emu_buffer = (uint32_t) transfer->buffer;
    pending_emu_size = (transfer->size > transfer->length) ? transfer->length : transfer->size;
    pending_emu_do_memcpy = 0;
    pending_emu_size_2d = transfer->size;
    pending_emu_length = transfer->length;
    pending_emu_stride = transfer->stride;
    pending_emu_task = task;
    __pi_hyper_copy_misaligned(task);
}

void __pi_hyper_copy_2d(int device, struct hyper_transfer_s *transfer,
                        struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    if ((__pi_hyper_end_task != NULL) || (pending_emu_size_2d != 0))
    {
        /* Transfer on going, enqueue this one to the list. */
        struct pi_task *task_enqueue = __pi_hyper_prepare_callback(transfer, task);
        __pi_hyper_task_fifo_enqueue(__global_hyper_driver_fifo[device], task_enqueue);
    }
    else
    {
        /* Execute the transfer. */
        __pi_hyper_copy_2d_exec(transfer, task);
    }
    __restore_irq(irq);
}

/* Init L3 memory alloc. */
uint32_t __pi_hyper_alloc_init(malloc_t *alloc, uint32_t heapstart, int32_t size)
{
    return __malloc_extern_init(alloc, 0, size);
}

void __pi_hyper_alloc_deinit(malloc_t *alloc)
{
    __malloc_extern_deinit(alloc);
    pi_default_free(alloc, sizeof(malloc_t));
}

/* Misaligned transfer functions. */
static int32_t __pi_hyper_resume_misaligned_write(struct pi_task *task)
{
    while(1)
    {
        // Compute information to see how to do one more step
        int32_t addr_aligned = (pending_emu_buffer + 3) & ~0x3;
        int32_t prologue_size = addr_aligned - pending_emu_buffer;
        int32_t hyper_addr_aligned = pending_emu_hyper_addr + prologue_size;

        if (pending_emu_size < 4)
        {
            prologue_size = pending_emu_size;
        }

        if (prologue_size)
        {
            // Case where we have a partial copy to do
            if (!pending_emu_do_memcpy)
            {
                /*
                 * A partial transfer must first transfer the content of the hyper
                 * to the temporary area and partially overwrite it with a memcpy.
                 * This part is first called to trigger the transfer while the part after
                 * is called to do the memcpy and the final transfer as a second step.
                 */
                struct hyper_transfer_s transfer = {0};
                transfer.channel = RX_CHANNEL;
                transfer.device_id = pending_emu_device_id;
                transfer.hyper_addr = pending_emu_hyper_addr & ~0x1;
                transfer.buffer = __pi_hyper_temp_buffer;
                transfer.size = 4;
                __pi_hyper_copy_aligned( &transfer, NULL);

                // It is asynchronous, just remember we have to do
                // a memcpy when the transfer is done and leave
                pending_emu_do_memcpy = 1;
                return 0;
            }

            pending_emu_do_memcpy = 0;
            l2_memcpy(&__pi_hyper_temp_buffer[pending_emu_hyper_addr & 0x1],
                      (void *) pending_emu_buffer,
                      prologue_size);

            struct hyper_transfer_s transfer = {0};
            transfer.channel = TX_CHANNEL;
            transfer.device_id = pending_emu_device_id;
            transfer.hyper_addr = pending_emu_hyper_addr & ~0x1;
            transfer.buffer = __pi_hyper_temp_buffer;
            transfer.size = 4;
            __pi_hyper_copy_aligned( &transfer, NULL);

            pending_emu_hyper_addr += prologue_size;
            pending_emu_buffer += prologue_size;
            pending_emu_size -= prologue_size;

            return 0;
        }
        else if (pending_emu_size > 0)
        {
            // Case where we have the body to transfer
            uint32_t size_aligned = pending_emu_size & ~0x3;

            if ((hyper_addr_aligned & 0x1) == 0)
            {
                // Good case where the body is aligned on both sides and we can do
                // a direct copy.
                struct hyper_transfer_s transfer = {0};
                transfer.channel = pending_emu_channel;
                transfer.device_id = pending_emu_device_id;
                transfer.hyper_addr = pending_emu_hyper_addr;
                transfer.buffer = (void *) pending_emu_buffer;
                transfer.size = size_aligned;
                __pi_hyper_copy_aligned( &transfer, NULL);

                pending_emu_hyper_addr += size_aligned;
                pending_emu_buffer += size_aligned;
                pending_emu_size -= size_aligned;

                // It is asynchronous, just leave, we'll continue the transfer
                // when this one is over
                return 0;
            }
            else
            {
                // Bad case where we have to transfer the body using a temporary
                // buffer as the aligments on both sides are not compatible.
                // This part is very similar to the prologue.
                // Just be careful to split into small transfers to fit the temporary buffer.
                if (size_aligned > __PI_HYPER_TEMP_BUFFER_SIZE - 4)
                {
                    size_aligned = __PI_HYPER_TEMP_BUFFER_SIZE - 4;
                }

                if (!pending_emu_do_memcpy)
                {
                    struct hyper_transfer_s transfer = {0};
                    transfer.channel = RX_CHANNEL;
                    transfer.device_id = pending_emu_device_id;
                    transfer.hyper_addr = pending_emu_hyper_addr & ~0x1;
                    transfer.buffer = __pi_hyper_temp_buffer;
                    transfer.size = 4;
                    __pi_hyper_copy_aligned( &transfer, NULL);

                    pending_emu_do_memcpy = 1;
                    return 0;
                }

                pending_emu_do_memcpy = 0;
                l2_memcpy(&__pi_hyper_temp_buffer[1],
                          (void *) pending_emu_buffer,
                          (size_aligned - 1));

                struct hyper_transfer_s transfer = {0};
                transfer.channel = pending_emu_channel;
                transfer.device_id = pending_emu_device_id;
                transfer.hyper_addr = pending_emu_hyper_addr & ~0x1;
                transfer.buffer = __pi_hyper_temp_buffer;
                transfer.size = size_aligned;
                __pi_hyper_copy_aligned( &transfer, NULL);

                pending_emu_hyper_addr += size_aligned-1;
                pending_emu_buffer += size_aligned-1;
                pending_emu_size -= size_aligned-1;

                return 0;
            }
        }
        // Now check if we are done
        if (pending_emu_size == 0)
        {
            // Check if we are doing a 2D transfer
            if (pending_emu_size_2d > 0)
            {
                // In this case, update the global size
                if (pending_emu_size_2d > pending_emu_length)
                {
                    pending_emu_size_2d -= pending_emu_length;
                }
                else
                {
                    pending_emu_size_2d = 0;
                }

                // And check if we must reenqueue a line.
                if (pending_emu_size_2d > 0)
                {
                    pending_emu_hyper_addr = pending_emu_hyper_addr - pending_emu_length + pending_emu_stride;
                    pending_emu_size = (pending_emu_size_2d > pending_emu_length) ? pending_emu_length : pending_emu_size_2d;
                    continue;
                }
            }

            pending_emu_task = NULL;
            __pi_hyper_handle_end_of_task(task);

            return 1;
        }
        break;
    }
    return 0;
}

static int32_t __pi_hyper_resume_misaligned_read(struct pi_task *task)
{
    while (1)
    {
        // Compute information to see how to do one more step
        int32_t addr_aligned = (pending_emu_buffer + 3) & ~0x3;
        int32_t prologue_size = addr_aligned - pending_emu_buffer;
        int32_t hyper_addr_aligned = pending_emu_hyper_addr + prologue_size;

        if (pending_emu_size < 4)
        {
            prologue_size = pending_emu_size;
        }

        if (prologue_size)
        {
            // Case where we have a partial copy to do
            if (!pending_emu_do_memcpy)
            {
                /*
                 * A partial transfer must first transfer to the temporary area
                 * and finish the transfer by hands using a memcpy.
                 * This part is first called to trigger the transfer while the part after
                 * is called to do the memcpy as a second step.
                 */
                struct hyper_transfer_s transfer = {0};
                transfer.channel = RX_CHANNEL;
                transfer.device_id = pending_emu_device_id;
                transfer.hyper_addr = pending_emu_hyper_addr & ~0x1;
                transfer.buffer = __pi_hyper_temp_buffer;
                transfer.size = 4;
                __pi_hyper_copy_aligned( &transfer, NULL);

                // It is asynchronous, just remember we have to do
                // a memcpy when the transfer is done and leave
                pending_emu_do_memcpy = 1;
                return 0;
            }

            pending_emu_do_memcpy = 0;
            l2_memcpy((void *)pending_emu_buffer,
                      &__pi_hyper_temp_buffer[pending_emu_hyper_addr & 0x1],
                      prologue_size);

            pending_emu_hyper_addr += prologue_size;
            pending_emu_buffer += prologue_size;
            pending_emu_size -= prologue_size;

            // The transfer is asynchronous, we get there to do the memcpy
            // without triggering any transfer, so we can start again to trigger one.
            if (pending_emu_size)
            {
                continue;
            }
        }
        else if (pending_emu_size > 0)
        {
            // Case where we have the body to transfer
            uint32_t size_aligned = pending_emu_size & ~0x3;

            if ((hyper_addr_aligned & 0x1) == 0)
            {
                // Good case where the body is aligned on both sides and we can do
                // a direct copy.
                struct hyper_transfer_s transfer = {0};
                transfer.channel = pending_emu_channel;
                transfer.device_id = pending_emu_device_id;
                transfer.hyper_addr = pending_emu_hyper_addr;
                transfer.buffer = (void *) pending_emu_buffer;
                transfer.size = size_aligned;
                __pi_hyper_copy_aligned( &transfer, NULL);

                pending_emu_hyper_addr += size_aligned;
                pending_emu_buffer += size_aligned;
                pending_emu_size -= size_aligned;

                // It is asynchronous, just leave, we'll continue the transfer
                // when this one is over
                return 0;
            }
            else
            {
                // Bad case where we have to transfer the body using a temporary
                // buffer as the aligments on both sides are not compatible.
                // This part is very similar to the prologue.
                // Just be careful to split into small transfers to fit the temporary buffer.
                if (size_aligned > __PI_HYPER_TEMP_BUFFER_SIZE - 4)
                {
                    size_aligned = __PI_HYPER_TEMP_BUFFER_SIZE - 4;
                }

                if (!pending_emu_do_memcpy)
                {
                    struct hyper_transfer_s transfer = {0};
                    transfer.channel = pending_emu_channel;
                    transfer.device_id = pending_emu_device_id;
                    transfer.hyper_addr = pending_emu_hyper_addr & ~0x1;
                    transfer.buffer = __pi_hyper_temp_buffer;
                    transfer.size = size_aligned + 4;
                    __pi_hyper_copy_aligned( &transfer, NULL);

                    pending_emu_do_memcpy = 1;
                    return 0;
                }

                pending_emu_do_memcpy = 0;
                l2_memcpy((void *) pending_emu_buffer,
                          &__pi_hyper_temp_buffer[1],
                          size_aligned);

                pending_emu_hyper_addr += size_aligned;
                pending_emu_buffer += size_aligned;
                pending_emu_size -= size_aligned;

                if (pending_emu_size)
                {
                    continue;
                }
            }
        }
        // Now check if we are done
        if (pending_emu_size == 0)
        {
            // Check if we are doing a 2D transfer
            if (pending_emu_size_2d > 0)
            {
                // In this case, update the global size
                if (pending_emu_size_2d > pending_emu_length)
                {
                    pending_emu_size_2d -= pending_emu_length;
                }
                else
                {
                    pending_emu_size_2d = 0;
                }

                // And check if we must reenqueue a line.
                if (pending_emu_size_2d > 0)
                {
                    pending_emu_hyper_addr = pending_emu_hyper_addr - pending_emu_length + pending_emu_stride;
                    pending_emu_size = (pending_emu_size_2d > pending_emu_length) ? pending_emu_length : pending_emu_size_2d;
                    continue;
                }
            }

            pending_emu_task = NULL;
            __pi_hyper_handle_end_of_task(task);

            return 1;
        }
        break;
    }
    return 0;
}

/* Hyperflash settings. */
void __pi_hyper_flash_config(struct pi_device *device)
{
    //pi_hyper_type_e type = ((struct pi_hyper_conf*)device->config)->type;
    //((struct pi_hyper_conf*)device->config)->type = PI_HYPER_TYPE_FLASH;

    for (uint32_t i=0; i<4; i++)
    {
        pi_hyper_write(device, (VCR_Seq[i].addr << 1), &VCR_Seq[i].data, 2);
    }
    //((struct pi_hyper_conf*)device->config)->type = type;
}
