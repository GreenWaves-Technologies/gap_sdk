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

#define BURST_SIZE 512

/*******************************************************************************
 * Driver data
 *****************************************************************************/

struct hyper_driver_fifo *__global_hyper_driver_fifo[UDMA_NB_HYPER];

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
static uint8_t __pi_hyper_task_fifo_enqueue(struct hyper_driver_fifo *fifo,
                                            struct pi_task *task);

/* Pop a task from fifo. */
static struct pi_task *__pi_hyper_task_fifo_pop(struct hyper_driver_fifo *fifo);

static int32_t __pi_hyper_resume_emu_task(struct hyper_driver_fifo *fifo);

static int32_t __pi_hyper_resume_misaligned_write(struct hyper_driver_fifo *fifo,
                                                  struct pi_task *task);

static int32_t __pi_hyper_resume_misaligned_read(struct hyper_driver_fifo *fifo,
                                                 struct pi_task *task);

static int32_t __pi_hyper_copy_misaligned(struct hyper_driver_fifo *fifo,
                                          struct pi_task *task);

static void __attribute__((noinline)) __pi_hyper_copy_aligned(struct hyper_driver_fifo *fifo,
                                                              struct pi_task *task);

static void __pi_hyper_copy_exec(struct hyper_driver_fifo *fifo,
                                 struct pi_task *task);

static void __pi_hyper_copy_2d_exec(struct hyper_driver_fifo *fifo,
                                    struct pi_task *task);

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
    return memcpy(dst0, src0, len0);
}

/* Handler. */
/* TODO    : In ASM, maybe ?
 * TODO bis: enqueue in handler  */
static void __pi_hyper_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
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
    HYPER_TRACE("Hyper IRQ %d %d\n", periph_id, event);

    struct hyper_driver_fifo *fifo = __global_hyper_driver_fifo[periph_id];
    struct pi_task *task = fifo->fifo_head;
    if (task->data[7] != 0)
    {
        task->data[0] += task->data[2]; /* Hyper_addr + repeat_size */
        task->data[1] += task->data[2]; /* buffer + repeat_size */
        task->data[7] -= task->data[2]; /* size - repeat_size */
        uint32_t iter_size = task->data[2];
        if (task->data[7] <= task->data[2])
        {
            iter_size = task->data[7];
            task->data[7] = 0;
        }
        HYPER_TRACE("Reenqueue %d %x %x, repeat_size=%d, full_size=%d\n",
                    iter_size, task->data[0], task->data[1], task->data[2], task->data[7]);
        hal_hyper_enqueue(fifo->device_id, task->data[5], task->data[0],
                          task->data[1], iter_size, UDMA_CFG_EN(1));
    }
    else
    {
        HYPER_TRACE("No reenqueue\n");
        /* Case end of current transfer. */
        struct pi_task *task = NULL;
        uint32_t emu_task = 0;
        /* Case pending misaligned transfer. */
        task = fifo->fifo_head;
        if (task->data[6])
        {
            HYPER_TRACE("Emul? Yes\n");
            emu_task = __pi_hyper_resume_emu_task(fifo);
            if (emu_task == 0)
            {
                return;
            }
        }

        /* Case new transfer in fifo. */
        if ((task->data[6] == 0) || emu_task)
        {
            task = NULL;
            task = __pi_hyper_task_fifo_pop(fifo);
            __pi_hyper_handle_end_of_task(task);
            task = fifo->fifo_head;
            /* Maybe use a struct to get fifo of tasks. */
            if (task != NULL)
            {
                HYPER_TRACE("New transfer\n");
                if (task->data[3])
                {
                    __pi_hyper_copy_2d_exec(fifo, task);
                }
                else
                {
                    __pi_hyper_copy_exec(fifo, task);
                }
            }
        }
    }
}

/* Enqueue a task in fifo. */
static uint8_t __pi_hyper_task_fifo_enqueue(struct hyper_driver_fifo *fifo, struct pi_task *task)
{
    uint8_t head = 0;
    uint32_t irq = __disable_irq();
    if (fifo->fifo_head == NULL)
    {
        /* Transfer on going, enqueue this one to the list. */
        /* Empty fifo. */
        fifo->fifo_head = task;
        fifo->fifo_tail = fifo->fifo_head;
        head = 0;
    }
    else
    {
        /* Execute the transfer. */
        fifo->fifo_tail->next = task;
        fifo->fifo_tail = fifo->fifo_tail->next;
        head = 1;
    }
    __restore_irq(irq);
    return head;
}

/* Pop a task from fifo. */
static struct pi_task *__pi_hyper_task_fifo_pop(struct hyper_driver_fifo *fifo)
{
    uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    if (fifo->fifo_head != NULL)
    {
        task_to_return = fifo->fifo_head;
        fifo->fifo_head = fifo->fifo_head->next;
        if (fifo->fifo_head == NULL)
        {
            fifo->fifo_tail = NULL;
        }
    }
    __restore_irq(irq);
    return task_to_return;
}

/* Copy functions. */
static int32_t __pi_hyper_resume_emu_task(struct hyper_driver_fifo *fifo)
{
    return __pi_hyper_copy_misaligned(fifo, fifo->fifo_head);
}

static int32_t __pi_hyper_copy_misaligned(struct hyper_driver_fifo *fifo, struct pi_task *task)
{
    uint32_t end = 0;
    if (fifo->pending_emu_channel == TX_CHANNEL)
    {
        end = __pi_hyper_resume_misaligned_write(fifo, task);
    }
    else
    {
        end = __pi_hyper_resume_misaligned_read(fifo, task);
    }
    return end;
}

static void __attribute__((noinline)) __pi_hyper_copy_aligned(struct hyper_driver_fifo *fifo, struct pi_task *task)
{
    if (task->data[2] > (uint32_t) BURST_SIZE)
    {
        task->data[7] = task->data[2];
        task->data[2] = (uint32_t) BURST_SIZE;
    }
    HYPER_TRACE("Enqueue %d %x %x, burst=%d, full_size=%d\n",
                task->data[2], task->data[0], task->data[1], task->data[2], task->data[7]);
    hal_hyper_enqueue(fifo->device_id, task->data[5], task->data[0],
                      task->data[1], task->data[2], UDMA_CFG_EN(1));
}

static void __pi_hyper_copy_exec(struct hyper_driver_fifo *fifo, struct pi_task *task)
{
    if ((((uint32_t) task->data[1] & 0x3) == 0) &&
        (((uint32_t) task->data[0] & 0x1) == 0) &&
        ((((uint32_t) task->data[2] & 0x3) == 0) ||
         ((task->data[5] == TX_CHANNEL) && (((uint32_t) task->data[2] & 0x1) == 0))))
    {
        /* Aligned copy. */
        //fifo->pending_emu_hyper_addr = 0;
        fifo->pending_emu_size = 0;
        //fifo->pending_emu_size_2d = 0;
        //fifo->pending_emu_channel = 0;
        __pi_hyper_copy_aligned(fifo, task);
    }
    else
    {
        /* Misaligned copy. */
        fifo->pending_emu_hyper_addr = task->data[0];
        fifo->pending_emu_buffer = task->data[1];
        fifo->pending_emu_size = task->data[2];
        fifo->pending_emu_size_2d = 0;
        fifo->pending_emu_channel = task->data[5];
        fifo->pending_emu_do_memcpy = 0;
        task->data[6] = 1;
        HYPER_TRACE("Misaligned: %x %x %d %d\n", fifo->pending_emu_hyper_addr,
                    fifo->pending_emu_buffer, fifo->pending_emu_size, fifo->pending_emu_channel);
        __pi_hyper_copy_misaligned(fifo, task);
    }
}

void __pi_hyper_copy(int device, struct hyper_transfer_s *transfer,
                     struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct hyper_driver_fifo *fifo = __global_hyper_driver_fifo[device];
    task->data[0] = transfer->hyper_addr;
    task->data[1] = (uint32_t) transfer->buffer;
    task->data[2] = transfer->size;
    task->data[3] = transfer->stride;
    task->data[4] = transfer->length;
    task->data[5] = transfer->channel;
    task->data[6] = 0;          /* is_emulated. */
    task->data[7] = 0;          /* repeat_size. */
    task->next = NULL;
    uint8_t head = __pi_hyper_task_fifo_enqueue(fifo, task);
    if (head == 0)
    {
        /* Execute the transfer. */
        __pi_hyper_copy_exec(fifo, task);
    }
    __restore_irq(irq);
}

static void __pi_hyper_copy_2d_exec(struct hyper_driver_fifo *fifo, struct pi_task *task)
{
    /* Misaligned copy. */
    fifo->pending_emu_hyper_addr = task->data[0];
    fifo->pending_emu_buffer = task->data[1];
    fifo->pending_emu_size = (task->data[2] > task->data[4]) ? task->data[4] : task->data[2];
    fifo->pending_emu_size_2d = task->data[2];
    fifo->pending_emu_channel = task->data[5];
    fifo->pending_emu_do_memcpy = 0;
    //task->data[2] = (task->data[2] > task->data[4]) ? task->data[4] : task->data[2];
    __pi_hyper_copy_misaligned(fifo, task);
}

void __pi_hyper_copy_2d(int device, struct hyper_transfer_s *transfer,
                        struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct hyper_driver_fifo *fifo = __global_hyper_driver_fifo[device];
    task->data[0] = transfer->hyper_addr;
    task->data[1] = (uint32_t) transfer->buffer;
    task->data[2] = transfer->size;
    task->data[3] = transfer->stride;
    task->data[4] = transfer->length;
    task->data[5] = transfer->channel;
    task->data[6] = 1;          /* is_emulated. */
    task->data[7] = 0;          /* repeat_size. */
    //task->data[2] = (transfer->size > transfer->length) ? transfer->length : transfer->size;
    //task->data[6] = transfer->size; /* size_2d. */
    task->next = NULL;
    uint8_t head = __pi_hyper_task_fifo_enqueue(fifo, task);
    if (head == 0)
    {
        /* Execute the transfer. */
        __pi_hyper_copy_2d_exec(fifo, task);
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
static int32_t __pi_hyper_resume_misaligned_write(struct hyper_driver_fifo *fifo,
                                                  struct pi_task *task)
{
    while (1)
    {
        // Compute information to see how to do one more step
        int32_t addr_aligned = (fifo->pending_emu_buffer + 3) & ~0x3;
        int32_t prologue_size = addr_aligned - fifo->pending_emu_buffer;
        int32_t hyper_addr_aligned = fifo->pending_emu_hyper_addr + prologue_size;

        if (fifo->pending_emu_size < 4)
        {
            prologue_size = fifo->pending_emu_size;
        }

        HYPER_TRACE("Misaligned write: %x %d %x\n",
                    addr_aligned, prologue_size, hyper_addr_aligned);
        if (prologue_size)
        {
            HYPER_TRACE("Prologue\n");
            // Case where we have a partial copy to do
            if (fifo->pending_emu_do_memcpy == 0)
            {
                /*
                 * A partial transfer must first transfer the content of the hyper
                 * to the temporary area and partially overwrite it with a memcpy.
                 * This part is first called to trigger the transfer while the part after
                 * is called to do the memcpy and the final transfer as a second step.
                 */
                task->data[0] = fifo->pending_emu_hyper_addr & ~0x1;
                task->data[1] = (uint32_t) __pi_hyper_temp_buffer;
                task->data[2] = 4;
                task->data[5] = RX_CHANNEL;
                HYPER_TRACE("Copy: %x %x %d %d\n",
                            task->data[0], task->data[1], task->data[2], task->data[5]);
                __pi_hyper_copy_aligned(fifo, task);
                HYPER_TRACE("Copy sent\n");

                // It is asynchronous, just remember we have to do
                // a memcpy when the transfer is done and leave
                fifo->pending_emu_do_memcpy = 1;
                return 0;
            }

            fifo->pending_emu_do_memcpy = 0;
            l2_memcpy(&__pi_hyper_temp_buffer[fifo->pending_emu_hyper_addr & 0x1],
                      (void *) fifo->pending_emu_buffer,
                      prologue_size);

            task->data[0] = fifo->pending_emu_hyper_addr & ~0x1;
            task->data[1] = (uint32_t) __pi_hyper_temp_buffer;
            task->data[2] = 4;
            task->data[5] = TX_CHANNEL;
            HYPER_TRACE("Copy: %x %x %d %d\n",
                        task->data[0], task->data[1], task->data[2], task->data[5]);
            __pi_hyper_copy_aligned(fifo, task);
            HYPER_TRACE("Copy sent\n");

            fifo->pending_emu_hyper_addr += prologue_size;
            fifo->pending_emu_buffer += prologue_size;
            fifo->pending_emu_size -= prologue_size;
            HYPER_TRACE("After memcpy & copy: %x %x %d\n", fifo->pending_emu_hyper_addr,
                        fifo->pending_emu_buffer, fifo->pending_emu_size);

            return 0;
        }
        else if (fifo->pending_emu_size > 0)
        {
            HYPER_TRACE("Pending emu size > 0\n");
            // Case where we have the body to transfer
            uint32_t size_aligned = fifo->pending_emu_size & ~0x3;

            if ((hyper_addr_aligned & 0x1) == 0)
            {
                HYPER_TRACE("Pending emu size > 0, if hyper_addr_aligned\n");
                // Good case where the body is aligned on both sides and we can do
                // a direct copy.
                task->data[0] = fifo->pending_emu_hyper_addr;
                task->data[1] = fifo->pending_emu_buffer;
                task->data[2] = size_aligned;
                task->data[5] = fifo->pending_emu_channel;
                HYPER_TRACE("Copy: %x %x %d %d\n",
                            task->data[0], task->data[1], task->data[2], task->data[5]);
                __pi_hyper_copy_aligned(fifo, task);
                HYPER_TRACE("Copy sent\n");

                fifo->pending_emu_hyper_addr += size_aligned;
                fifo->pending_emu_buffer += size_aligned;
                fifo->pending_emu_size -= size_aligned;
                HYPER_TRACE("After copy: %x %x %d\n", fifo->pending_emu_hyper_addr,
                            fifo->pending_emu_buffer, fifo->pending_emu_size);

                // It is asynchronous, just leave, we'll continue the transfer
                // when this one is over
                return 0;
            }
            else
            {
                HYPER_TRACE("Pending emu size > 0, else hyper_addr_aligned\n");
                // Bad case where we have to transfer the body using a temporary
                // buffer as the aligments on both sides are not compatible.
                // This part is very similar to the prologue.
                // Just be careful to split into small transfers to fit the temporary buffer.
                if (size_aligned > __PI_HYPER_TEMP_BUFFER_SIZE - 4)
                {
                    size_aligned = __PI_HYPER_TEMP_BUFFER_SIZE - 4;
                }

                if (fifo->pending_emu_do_memcpy == 0)
                {
                    task->data[0] = fifo->pending_emu_hyper_addr & ~0x1;
                    task->data[1] = (uint32_t) __pi_hyper_temp_buffer;
                    task->data[2] = 4;
                    task->data[5] = RX_CHANNEL;
                    HYPER_TRACE("Copy: %x %x %d %d\n",
                                task->data[0], task->data[1], task->data[2], task->data[5]);
                    __pi_hyper_copy_aligned(fifo, task);
                    HYPER_TRACE("Copy sent\n");

                    fifo->pending_emu_do_memcpy = 1;
                    return 0;
                }

                fifo->pending_emu_do_memcpy = 0;
                l2_memcpy(&__pi_hyper_temp_buffer[1],
                          (void *) fifo->pending_emu_buffer,
                          (size_aligned - 1));

                task->data[0] = fifo->pending_emu_hyper_addr & ~0x1;
                task->data[1] = (uint32_t) __pi_hyper_temp_buffer;
                task->data[2] = size_aligned;
                task->data[5] = fifo->pending_emu_channel;
                HYPER_TRACE("Copy: %x %x %d %d\n",
                            task->data[0], task->data[1], task->data[2], task->data[5]);
                __pi_hyper_copy_aligned(fifo, task);
                HYPER_TRACE("Copy sent\n");

                fifo->pending_emu_hyper_addr += size_aligned-1;
                fifo->pending_emu_buffer += size_aligned-1;
                fifo->pending_emu_size -= size_aligned-1;
                HYPER_TRACE("After memcpy & copy: %x %x %d\n", fifo->pending_emu_hyper_addr,
                            fifo->pending_emu_buffer, fifo->pending_emu_size);

                return 0;
            }
        }
        // Now check if we are done
        if (fifo->pending_emu_size == 0)
        {
            HYPER_TRACE("Pending emu_size is NULL.\n");
            // Check if we are doing a 2D transfer
            if (fifo->pending_emu_size_2d > 0)
            {
                HYPER_TRACE("Pending emu size 2d %d %d.\n",
                            fifo->pending_emu_size_2d, task->data[4]);
                // In this case, update the global size
                if (fifo->pending_emu_size_2d > task->data[4])
                {
                    fifo->pending_emu_size_2d -= task->data[4];
                }
                else
                {
                    fifo->pending_emu_size_2d = 0;
                }

                // And check if we must reenqueue a line.
                if (fifo->pending_emu_size_2d > 0)
                {
                    fifo->pending_emu_hyper_addr = fifo->pending_emu_hyper_addr - task->data[4] + task->data[3];
                    fifo->pending_emu_size = (fifo->pending_emu_size_2d > task->data[4]) ? task->data[4] : fifo->pending_emu_size_2d;
                    HYPER_TRACE("Continue\n");
                    continue;
                }
            }
            HYPER_TRACE("Return\n");
            task->data[6] = 0;
            return 1;
        }
        break;
    }
    return 0;
}

static int32_t __pi_hyper_resume_misaligned_read(struct hyper_driver_fifo *fifo,
                                                 struct pi_task *task)
{
    while (1)
    {
        // Compute information to see how to do one more step
        int32_t addr_aligned = (fifo->pending_emu_buffer + 3) & ~0x3;
        int32_t prologue_size = addr_aligned - fifo->pending_emu_buffer;
        int32_t hyper_addr_aligned = fifo->pending_emu_hyper_addr + prologue_size;

        if (fifo->pending_emu_size < 4)
        {
            prologue_size = fifo->pending_emu_size;
        }

        HYPER_TRACE("Misaligned read: %x %d %x\n",
                    addr_aligned, prologue_size, hyper_addr_aligned);
        if (prologue_size)
        {
            HYPER_TRACE("prologue\n");
            // Case where we have a partial copy to do
            if (fifo->pending_emu_do_memcpy == 0)
            {
                /*
                 * A partial transfer must first transfer to the temporary area
                 * and finish the transfer by hands using a memcpy.
                 * This part is first called to trigger the transfer while the part after
                 * is called to do the memcpy as a second step.
                 */
                task->data[0] = fifo->pending_emu_hyper_addr & ~0x1;
                task->data[1] = (uint32_t) __pi_hyper_temp_buffer;
                task->data[2] = 4;
                task->data[5] = RX_CHANNEL;
                HYPER_TRACE("Copy: %x %x %d %d\n",
                            task->data[0], task->data[1], task->data[2], task->data[5]);
                __pi_hyper_copy_aligned(fifo, task);
                HYPER_TRACE("Copy sent\n");

                // It is asynchronous, just remember we have to do
                // a memcpy when the transfer is done and leave
                fifo->pending_emu_do_memcpy = 1;
                return 0;
            }

            fifo->pending_emu_do_memcpy = 0;
            l2_memcpy((void *)fifo->pending_emu_buffer,
                      &__pi_hyper_temp_buffer[fifo->pending_emu_hyper_addr & 0x1],
                      prologue_size);

            fifo->pending_emu_hyper_addr += prologue_size;
            fifo->pending_emu_buffer += prologue_size;
            fifo->pending_emu_size -= prologue_size;
            HYPER_TRACE("After memcpy: %x %x %d\n", fifo->pending_emu_hyper_addr,
                        fifo->pending_emu_buffer, fifo->pending_emu_size);

            // The transfer is asynchronous, we get there to do the memcpy
            // without triggering any transfer, so we can start again to trigger one.
            if (fifo->pending_emu_size)
            {
                continue;
            }
        }
        else if (fifo->pending_emu_size > 0)
        {
            // Case where we have the body to transfer
            uint32_t size_aligned = fifo->pending_emu_size & ~0x3;

            if ((hyper_addr_aligned & 0x1) == 0)
            {
                // Good case where the body is aligned on both sides and we can do
                // a direct copy.
                task->data[0] = fifo->pending_emu_hyper_addr;
                task->data[1] = fifo->pending_emu_buffer;
                task->data[2] = size_aligned;
                task->data[5] = fifo->pending_emu_channel;
                HYPER_TRACE("Copy: %x %x %d %d\n",
                            task->data[0], task->data[1], task->data[2], task->data[5]);
                __pi_hyper_copy_aligned(fifo, task);
                HYPER_TRACE("Copy sent\n");

                fifo->pending_emu_hyper_addr += size_aligned;
                fifo->pending_emu_buffer += size_aligned;
                fifo->pending_emu_size -= size_aligned;
                HYPER_TRACE("After copy: %x %x %d\n", fifo->pending_emu_hyper_addr,
                            fifo->pending_emu_buffer, fifo->pending_emu_size);

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

                if (fifo->pending_emu_do_memcpy == 0)
                {
                    task->data[0] = fifo->pending_emu_hyper_addr & ~0x1;
                    task->data[1] = (uint32_t) __pi_hyper_temp_buffer;
                    task->data[2] = size_aligned + 4;
                    task->data[5] = fifo->pending_emu_channel;
                    HYPER_TRACE("Copy: %x %x %d %d\n",
                                task->data[0], task->data[1], task->data[2], task->data[5]);
                    __pi_hyper_copy_aligned(fifo, task);
                    HYPER_TRACE("Copy sent\n");

                    fifo->pending_emu_do_memcpy = 1;
                    return 0;
                }

                fifo->pending_emu_do_memcpy = 0;
                l2_memcpy((void *) fifo->pending_emu_buffer,
                          &__pi_hyper_temp_buffer[1],
                          size_aligned);

                fifo->pending_emu_hyper_addr += size_aligned;
                fifo->pending_emu_buffer += size_aligned;
                fifo->pending_emu_size -= size_aligned;
                HYPER_TRACE("After memcpy: %x %x %d\n", fifo->pending_emu_hyper_addr,
                            fifo->pending_emu_buffer, fifo->pending_emu_size);

                if (fifo->pending_emu_size)
                {
                    continue;
                }
            }
        }
        // Now check if we are done
        if (fifo->pending_emu_size == 0)
        {
            HYPER_TRACE("Pending emu_size is NULL.\n");
            // Check if we are doing a 2D transfer
            if (fifo->pending_emu_size_2d > 0)
            {
                HYPER_TRACE("Pending emu size 2d %d %d.\n",
                            fifo->pending_emu_size_2d, task->data[4]);
                // In this case, update the global size
                if (fifo->pending_emu_size_2d > task->data[4])
                {
                    fifo->pending_emu_size_2d -= task->data[4];
                }
                else
                {
                    fifo->pending_emu_size_2d = 0;
                }

                // And check if we must reenqueue a line.
                if (fifo->pending_emu_size_2d > 0)
                {
                    fifo->pending_emu_hyper_addr = fifo->pending_emu_hyper_addr - task->data[4] + task->data[3];
                    fifo->pending_emu_size = (fifo->pending_emu_size_2d > task->data[4]) ? task->data[4] : fifo->pending_emu_size_2d;
                    HYPER_TRACE("Continue\n");
                    continue;
                }
            }
            HYPER_TRACE("Return\n");
            task->data[6] = 0;
            return 1;
        }
        break;
    }
    return 0;
}
