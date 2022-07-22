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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define __PI_HYPER_TEMP_BUFFER_SIZE ( 128 )

#define BURST_SIZE                  ( 512 )

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static struct hyper_itf_data_s *g_hyper_itf_data[UDMA_NB_HYPER] = {NULL};

PI_L2 static uint8_t __pi_hyper_temp_buffer[__PI_HYPER_TEMP_BUFFER_SIZE];

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Hyper event handler. */
static void __pi_hyper_handler(void *arg);

/* Clock divider. */
static uint8_t __pi_hyper_clk_div_get(uint32_t freq);

/* Hyperbus settings. */
static void __pi_hyper_settings(struct hyper_itf_conf_s *conf);

/* Copy exec. */
static void __pi_hyper_copy_exec(struct hyper_itf_data_s *itf_data, struct pi_task *task);
static void __pi_hyper_copy_2d_exec(struct hyper_itf_data_s *itf_data, struct pi_task *task);

static void __attribute__((noinline)) __pi_hyper_copy_aligned(struct hyper_itf_data_s *itf_data,
                                                              struct pi_task *task);
static int32_t __pi_hyper_copy_misaligned(struct hyper_itf_data_s *itf_data,
                                          struct pi_task *task);
static int32_t __pi_hyper_resume_misaligned_write(struct hyper_itf_data_s *itf_data,
                                                  struct pi_task *task);
static int32_t __pi_hyper_resume_misaligned_read(struct hyper_itf_data_s *itf_data,
                                                 struct pi_task *task);
static int32_t __pi_hyper_resume_emu_task(struct hyper_itf_data_s *itf_data);

/*******************************************************************************
 * Internal function implementation
 ******************************************************************************/

static void __pi_hyper_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_HYPER_ID(0);
    //HYPER_TRACE("Hyper IRQ %ld %ld\n", event, periph_id);

    struct hyper_itf_data_s *itf_data = g_hyper_itf_data[periph_id];
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    struct pi_task *task = pi_udma_fifo_hw_fifo_current_task_get(udma_chan, 0);

    /* Pending data on current transfer ? */
    if (task->data[1] != 0)
    {
        //HYPER_TRACE("Reenqueue current task\n");
        uint32_t buff = task->data[0];
        uint32_t size = task->data[7];
        udma_channel_e channel = task->data[2];
        uint32_t hyper_addr = task->data[3];
        if (task->data[1] <= task->data[7])
        {
            size = task->data[1];
        }
        /* TODO : set burst size in copy(), update burst size in handler and call copy_aligned() */
        /* Update next buffer, size and hyper address. */
        task->data[0] += size;
        task->data[1] -= size;
        task->data[3] += size;
        /* HYPER_TRACE("HYPER(%d) : HW enqueue task=%lx, ext=%lx buf=%lx size=%ld, " */
        /*             "channel=%d, burst=%ld, size_remain=%ld.\n", */
        /*             periph_id, task, hyper_addr, buff, size, channel, */
        /*             burst_size, task->data[1]); */
        hal_udma_hyper_enqueue(hyper_addr, buff, size, 0, channel);
    }
    else
    {
        //HYPER_TRACE("No reenqueue\n");
        int32_t emu_task = 0;
        /* Is current transfer emulated ? */
        if (task->data[6])
        {
            //HYPER_TRACE("Resume emu\n");
            emu_task = __pi_hyper_resume_emu_task(itf_data);
            if (emu_task == 0)
            {
                /* Emulated transfer not finished yet. */
                return;
            }
        }

        /* Transfer not emulated, handle current task and pop new one. */
        if ((task->data[6] == 0) || emu_task)
        {
            //HYPER_TRACE("Handle current task\n");
            /* Handle current task. */
            task = pi_udma_fifo_hw_fifo_task_pop(udma_chan);
            __pi_irq_handle_end_of_task(task);

            //HYPER_TRACE("Enqueue new task\n");
            task = pi_udma_fifo_sw_fifo_task_pop(udma_chan);
            if (task != NULL)
            {
                if (task->data[4])
                {
                    __pi_hyper_copy_2d_exec(itf_data, task);
                }
                else
                {
                    __pi_hyper_copy_exec(itf_data, task);
                }
            }
        }
    }
}

static uint8_t __pi_hyper_clk_div_get(uint32_t freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    uint32_t div = 0;
    if ((freq == 0) || (freq >= periph_freq))
    {
        div = 0;
    }
    else
    {
        // Round-up the divider to obtain a frequency which is below the maximum.
        div = (periph_freq + freq - 1) / (freq);
        // The divider always divide by 2 once we activate the divider, thus increase by 1
        // in case it is even to not go above the max frequency.
        if (div & 1)
        {
            div += 1;
        }
        div >>= 1;
    }
    HYPER_TRACE("Periph_freq=%ld, hyper_freq=%ld, div=%ld\n", periph_freq, freq, div);
    return div;
}

static void __pi_hyper_settings(struct hyper_itf_conf_s *conf)
{
    uint8_t device_id = conf->device_id;
    uint8_t cs = conf->cs;
    uint8_t clk_div = conf->clk_div;
    uint8_t hyper_type = conf->hyper_type;
    uint8_t dev_type = (uint8_t) TYPE_HYPERBUS;
    uint8_t dev_0 = 0;
    uint8_t dev_1 = 0;
    uint8_t latency_0 = 6;
    uint8_t latency_1 = 6;
    uint8_t rw_recov = 6;
    uint8_t rwds_delay = 1;
    uint8_t autocheck = 1;
    uint32_t cs_max = 665;

    if (((cs == 0) && (hyper_type == PI_HYPER_TYPE_RAM)) ||
        ((cs == 1) && (hyper_type == PI_HYPER_TYPE_FLASH)))
    {
        dev_0 = (uint8_t) TYPE_RAM;
        dev_1 = (uint8_t) TYPE_FLASH;
    }
    else
    {
        dev_0 = (uint8_t) TYPE_FLASH;
        dev_1 = (uint8_t) TYPE_RAM;
    }

    HYPER_TRACE("HYPER(%d) Timing settings : latency_0=%d latency_1=%d"
                "rw_recov=%d, rwds_delay=%d, autocheck %d, cs_max=%ld\n",
                device_id, latency_0, latency_1, rw_recov, rwds_delay, autocheck, cs_max);

    hal_udma_hyper_device_set(dev_type, dev_0, dev_1);
    hal_udma_hyper_timing_cfg_set(latency_0, latency_1, rw_recov, rwds_delay, autocheck, cs_max);
    hal_udma_hyper_clk_div_set(clk_div);
    hal_udma_hyper_mba0_set(REG_MBR0);
    hal_udma_hyper_mba1_set(REG_MBR1);

    hal_udma_hyper_irq_enable();

    HYPER_TRACE("HYPER(%d) : TIMING=%lx, MBA0=%lx, MBA1=%lx, DEVICE=%lx\n",
                device_id, hyper_timing_cfg_get(), hyper_mba0_get(),
                hyper_mba1_get(), hyper_device_get());
}

static void __attribute__((noinline)) __pi_hyper_copy_aligned(struct hyper_itf_data_s *itf_data,
                                                              struct pi_task *task)
{
    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    uint32_t device_id = itf_data->device_id;
    uint32_t buff = 0, size = 0;
    udma_channel_e channel = task->data[2];
    uint32_t hyper_addr = task->data[3];
    uint32_t burst_size = (uint32_t) BURST_SIZE;
    pi_udma_fifo_buffer_set(task, &buff, &size, burst_size);
    /* Update next hyper address, buffer and size updated by above call. */
    task->data[3] += size;
    task->data[7] = burst_size;
    pi_udma_fifo_hw_fifo_task_enqueue(udma_chan, task, 0);
    HYPER_TRACE("HYPER(%d) : HW enqueue task=%lx, ext=%lx buf=%lx size=%ld, "
                "channel=%d, burst=%ld, size_remain=%ld.\n",
                device_id, task, hyper_addr, buff, size, channel,
                burst_size, task->data[1]);
    hal_udma_hyper_enqueue(hyper_addr, buff, size, 0, channel);
}

static int32_t __pi_hyper_copy_misaligned(struct hyper_itf_data_s *itf_data,
                                          struct pi_task *task)
{
    int32_t end = 0;
    /* HYPER_TRACE("Misaligned transfer: %lx %lx %ld %ld\n", */
    /*             itf_data->pending_emu_hyper_addr, itf_data->pending_emu_buffer, */
    /*             itf_data->pending_emu_size, itf_data->pending_emu_channel); */
   if (itf_data->pending_emu_channel == TX_CHANNEL)
    {
        end = __pi_hyper_resume_misaligned_write(itf_data, task);
    }
    else
    {
        end = __pi_hyper_resume_misaligned_read(itf_data, task);
    }
    return end;
}

static void __pi_hyper_copy_exec(struct hyper_itf_data_s *itf_data,
                                 struct pi_task *task)
{
    if (((task->data[0] & 0x3) == 0) &&
        ((task->data[3] & 0x1) == 0) &&
        (((task->data[1] & 0x3) == 0) ||
         ((task->data[2] == TX_CHANNEL) && ((task->data[1] & 0x1) == 0))))
    {
        /* Aligned copy. */
        itf_data->pending_emu_size = 0;
        __pi_hyper_copy_aligned(itf_data, task);
    }
    else
    {
        /* Misaligned copy. */
        itf_data->pending_emu_buffer = task->data[0];
        itf_data->pending_emu_hyper_addr = task->data[3];
        itf_data->pending_emu_size = task->data[1];
        itf_data->pending_emu_size_2d = 0;
        itf_data->pending_emu_channel = task->data[2];
        itf_data->pending_emu_do_memcpy = 0;
        task->data[6] = 1;
        __pi_hyper_copy_misaligned(itf_data, task);
    }
}

static void __pi_hyper_copy_2d_exec(struct hyper_itf_data_s *itf_data,
                                    struct pi_task *task)
{
    /* Misaligned copy. */
    itf_data->pending_emu_buffer = task->data[0];
    itf_data->pending_emu_hyper_addr = task->data[3];
    itf_data->pending_emu_size = (task->data[1] > task->data[5]) ? task->data[5] : task->data[1];
    itf_data->pending_emu_size_2d = task->data[1];
    itf_data->pending_emu_channel = task->data[2];
    itf_data->pending_emu_do_memcpy = 0;
    //task->data[6] = 1;
    HYPER_TRACE("Misaligned task=%lx, transfer: %lx %lx %ld %ld\n",
                task, itf_data->pending_emu_hyper_addr, itf_data->pending_emu_buffer,
                itf_data->pending_emu_size, itf_data->pending_emu_channel);
    __pi_hyper_copy_misaligned(itf_data, task);
}

static int32_t __pi_hyper_resume_emu_task(struct hyper_itf_data_s *itf_data)
{
    pi_task_t *task = pi_udma_fifo_hw_fifo_current_task_get(&(itf_data->udma_chan), 0);
    HYPER_TRACE("Resume misaligned task=%lx\n", task);
    return __pi_hyper_copy_misaligned(itf_data, task);
}

static int32_t __pi_hyper_resume_misaligned_write(struct hyper_itf_data_s *itf_data,
                                                  struct pi_task *task)
{
    while (1)
    {
        // Compute information to see how to do one more step
        int32_t addr_aligned = (itf_data->pending_emu_buffer + 3) & ~0x3;
        int32_t prologue_size = addr_aligned - itf_data->pending_emu_buffer;
        int32_t hyper_addr_aligned = itf_data->pending_emu_hyper_addr + prologue_size;

        if (itf_data->pending_emu_size < 4)
        {
            prologue_size = itf_data->pending_emu_size;
        }

        HYPER_TRACE("Misaligned write: %lx %lx %ld\n",
                    hyper_addr_aligned, addr_aligned, prologue_size);
        if (prologue_size)
        {
            HYPER_TRACE("Prologue\n");
            // Case where we have a partial copy to do
            if (itf_data->pending_emu_do_memcpy == 0)
            {
                /*
                 * A partial transfer must first transfer the content of the hyper
                 * to the temporary area and partially overwrite it with a memcpy.
                 * This part is first called to trigger the transfer while the part after
                 * is called to do the memcpy and the final transfer as a second step.
                 */
                task->data[0] = (uint32_t) __pi_hyper_temp_buffer;
                task->data[1] = 4;
                task->data[2] = RX_CHANNEL;
                task->data[3] = itf_data->pending_emu_hyper_addr & ~0x1;
                HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                            task->data[3], task->data[0], task->data[1], task->data[2]);
                __pi_hyper_copy_aligned(itf_data, task);
                //HYPER_TRACE("Copy sent\n");

                // It is asynchronous, just remember we have to do
                // a memcpy when the transfer is done and leave
                itf_data->pending_emu_do_memcpy = 1;
                return 0;
            }

            itf_data->pending_emu_do_memcpy = 0;
            memcpy(&__pi_hyper_temp_buffer[itf_data->pending_emu_hyper_addr & 0x1],
                   (void *) itf_data->pending_emu_buffer,
                   prologue_size);

            task->data[0] = (uint32_t) __pi_hyper_temp_buffer;
            task->data[1] = 4;
            task->data[2] = TX_CHANNEL;
            task->data[3] = itf_data->pending_emu_hyper_addr & ~0x1;
            HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                            task->data[3], task->data[0], task->data[1], task->data[2]);
            __pi_hyper_copy_aligned(itf_data, task);
            //HYPER_TRACE("Copy sent\n");

            itf_data->pending_emu_hyper_addr += prologue_size;
            itf_data->pending_emu_buffer += prologue_size;
            itf_data->pending_emu_size -= prologue_size;
            HYPER_TRACE("After memcpy & copy: %lx %lx %ld\n",
                        itf_data->pending_emu_hyper_addr,
                        itf_data->pending_emu_buffer, itf_data->pending_emu_size);
            return 0;
        }
        else if (itf_data->pending_emu_size > 0)
        {
            HYPER_TRACE("Pending emu size > 0\n");
            // Case where we have the body to transfer
            uint32_t size_aligned = itf_data->pending_emu_size & ~0x3;

            if ((hyper_addr_aligned & 0x1) == 0)
            {
                HYPER_TRACE("Pending emu size > 0, if hyper_addr_aligned\n");
                // Good case where the body is aligned on both sides and we can do
                // a direct copy.
                task->data[0] = itf_data->pending_emu_buffer;
                task->data[1] = size_aligned;
                task->data[2] = itf_data->pending_emu_channel;
                task->data[3] = itf_data->pending_emu_hyper_addr;
                HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                            task->data[3], task->data[0], task->data[1], task->data[2]);
                __pi_hyper_copy_aligned(itf_data, task);
                //HYPER_TRACE("Copy sent\n");

                itf_data->pending_emu_hyper_addr += size_aligned;
                itf_data->pending_emu_buffer += size_aligned;
                itf_data->pending_emu_size -= size_aligned;
                HYPER_TRACE("After copy: %lx %lx %ld\n", itf_data->pending_emu_hyper_addr,
                            itf_data->pending_emu_buffer, itf_data->pending_emu_size);

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

                if (itf_data->pending_emu_do_memcpy == 0)
                {
                    task->data[0] = (uint32_t) __pi_hyper_temp_buffer;
                    task->data[1] = 4;
                    task->data[2] = RX_CHANNEL;
                    task->data[3] = itf_data->pending_emu_hyper_addr & ~0x1;
                    HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                                task->data[3], task->data[0], task->data[1], task->data[2]);
                    __pi_hyper_copy_aligned(itf_data, task);
                    //HYPER_TRACE("Copy sent\n");

                    itf_data->pending_emu_do_memcpy = 1;
                    return 0;
                }

                itf_data->pending_emu_do_memcpy = 0;
                memcpy(&__pi_hyper_temp_buffer[1],
                       (void *) itf_data->pending_emu_buffer,
                       (size_aligned - 1));

                task->data[0] = (uint32_t) __pi_hyper_temp_buffer;
                task->data[1] = size_aligned;
                task->data[2] = itf_data->pending_emu_channel;
                task->data[3] = itf_data->pending_emu_hyper_addr & ~0x1;
                HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                                task->data[3], task->data[0], task->data[1], task->data[2]);
                __pi_hyper_copy_aligned(itf_data, task);
                //HYPER_TRACE("Copy sent\n");

                itf_data->pending_emu_hyper_addr += size_aligned-1;
                itf_data->pending_emu_buffer += size_aligned-1;
                itf_data->pending_emu_size -= size_aligned-1;
                HYPER_TRACE("After memcpy & copy: %lx %lx %ld\n",
                            itf_data->pending_emu_hyper_addr,
                            itf_data->pending_emu_buffer, itf_data->pending_emu_size);
                return 0;
            }
        }
        // Now check if we are done
        if (itf_data->pending_emu_size == 0)
        {
            HYPER_TRACE("Pending emu_size is NULL.\n");
            // Check if we are doing a 2D transfer
            if (itf_data->pending_emu_size_2d > 0)
            {
                HYPER_TRACE("Pending emu size 2d: %ld %ld.\n",
                            itf_data->pending_emu_size_2d, task->data[5]);
                // In this case, update the global size
                if (itf_data->pending_emu_size_2d > task->data[5])
                {
                    itf_data->pending_emu_size_2d -= task->data[5];
                }
                else
                {
                    itf_data->pending_emu_size_2d = 0;
                }

                // And check if we must reenqueue a line.
                if (itf_data->pending_emu_size_2d > 0)
                {
                    itf_data->pending_emu_hyper_addr = itf_data->pending_emu_hyper_addr - task->data[5] + task->data[4];
                    itf_data->pending_emu_size = (itf_data->pending_emu_size_2d > task->data[5]) ? task->data[5] : itf_data->pending_emu_size_2d;
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

static int32_t __pi_hyper_resume_misaligned_read(struct hyper_itf_data_s *itf_data,
                                                 struct pi_task *task)
{
    while (1)
    {
        // Compute information to see how to do one more step
        int32_t addr_aligned = (itf_data->pending_emu_buffer + 3) & ~0x3;
        int32_t prologue_size = addr_aligned - itf_data->pending_emu_buffer;
        int32_t hyper_addr_aligned = itf_data->pending_emu_hyper_addr + prologue_size;

        if (itf_data->pending_emu_size < 4)
        {
            prologue_size = itf_data->pending_emu_size;
        }

        HYPER_TRACE("Misaligned read: %lx %lx %ld\n",
                    hyper_addr_aligned, addr_aligned, prologue_size);
        if (prologue_size)
        {
            HYPER_TRACE("prologue\n");
            // Case where we have a partial copy to do
            if (itf_data->pending_emu_do_memcpy == 0)
            {
                /*
                 * A partial transfer must first transfer to the temporary area
                 * and finish the transfer by hands using a memcpy.
                 * This part is first called to trigger the transfer while the part after
                 * is called to do the memcpy as a second step.
                 */
                task->data[0] = (uint32_t) __pi_hyper_temp_buffer;
                task->data[1] = 4;
                task->data[2] = RX_CHANNEL;
                task->data[3] = itf_data->pending_emu_hyper_addr & ~0x1;
                HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                            task->data[3], task->data[0], task->data[1], task->data[2]);
                __pi_hyper_copy_aligned(itf_data, task);
                //HYPER_TRACE("Copy sent\n");

                // It is asynchronous, just remember we have to do
                // a memcpy when the transfer is done and leave
                itf_data->pending_emu_do_memcpy = 1;
                return 0;
            }

            itf_data->pending_emu_do_memcpy = 0;
            memcpy((void *)itf_data->pending_emu_buffer,
                   &__pi_hyper_temp_buffer[itf_data->pending_emu_hyper_addr & 0x1],
                   prologue_size);

            itf_data->pending_emu_hyper_addr += prologue_size;
            itf_data->pending_emu_buffer += prologue_size;
            itf_data->pending_emu_size -= prologue_size;
            HYPER_TRACE("After memcpy: %lx %lx %ld\n",
                        itf_data->pending_emu_hyper_addr,
                        itf_data->pending_emu_buffer, itf_data->pending_emu_size);

            // The transfer is asynchronous, we get there to do the memcpy
            // without triggering any transfer, so we can start again to trigger one.
            if (itf_data->pending_emu_size)
            {
                continue;
            }
        }
        else if (itf_data->pending_emu_size > 0)
        {
            // Case where we have the body to transfer
            uint32_t size_aligned = itf_data->pending_emu_size & ~0x3;

            if ((hyper_addr_aligned & 0x1) == 0)
            {
                // Good case where the body is aligned on both sides and we can do
                // a direct copy.
                task->data[0] = itf_data->pending_emu_buffer;
                task->data[1] = size_aligned;
                task->data[2] = itf_data->pending_emu_channel;
                task->data[3] = itf_data->pending_emu_hyper_addr;
                HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                            task->data[3], task->data[0], task->data[1], task->data[2]);
                __pi_hyper_copy_aligned(itf_data, task);
                //HYPER_TRACE("Copy sent\n");

                itf_data->pending_emu_hyper_addr += size_aligned;
                itf_data->pending_emu_buffer += size_aligned;
                itf_data->pending_emu_size -= size_aligned;
                HYPER_TRACE("After copy: %lx %lx %ld\n",
                            itf_data->pending_emu_hyper_addr,
                            itf_data->pending_emu_buffer, itf_data->pending_emu_size);

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

                if (itf_data->pending_emu_do_memcpy == 0)
                {
                    task->data[0] = (uint32_t) __pi_hyper_temp_buffer;
                    task->data[1] = size_aligned + 4;
                    task->data[2] = itf_data->pending_emu_channel;
                    task->data[3] = itf_data->pending_emu_hyper_addr & ~0x1;
                    HYPER_TRACE("Copy: %lx %lx %ld %ld\n",
                                task->data[3], task->data[0], task->data[1], task->data[2]);
                    __pi_hyper_copy_aligned(itf_data, task);
                    //HYPER_TRACE("Copy sent\n");

                    itf_data->pending_emu_do_memcpy = 1;
                    return 0;
                }

                itf_data->pending_emu_do_memcpy = 0;
                memcpy((void *) itf_data->pending_emu_buffer,
                       &__pi_hyper_temp_buffer[1],
                       size_aligned);

                itf_data->pending_emu_hyper_addr += size_aligned;
                itf_data->pending_emu_buffer += size_aligned;
                itf_data->pending_emu_size -= size_aligned;
                HYPER_TRACE("After memcpy: %lx %lx %ld\n",
                            itf_data->pending_emu_hyper_addr,
                            itf_data->pending_emu_buffer, itf_data->pending_emu_size);

                if (itf_data->pending_emu_size)
                {
                    continue;
                }
            }
        }
        // Now check if we are done
        if (itf_data->pending_emu_size == 0)
        {
            HYPER_TRACE("Pending emu_size is NULL.\n");
            // Check if we are doing a 2D transfer
            if (itf_data->pending_emu_size_2d > 0)
            {
                HYPER_TRACE("Pending emu size 2d: %ld %ld.\n",
                            itf_data->pending_emu_size_2d, task->data[5]);
                // In this case, update the global size
                if (itf_data->pending_emu_size_2d > task->data[5])
                {
                    itf_data->pending_emu_size_2d -= task->data[5];
                }
                else
                {
                    itf_data->pending_emu_size_2d = 0;
                }

                // And check if we must reenqueue a line.
                if (itf_data->pending_emu_size_2d > 0)
                {
                    itf_data->pending_emu_hyper_addr = itf_data->pending_emu_hyper_addr - task->data[5] + task->data[4];
                    itf_data->pending_emu_size = (itf_data->pending_emu_size_2d > task->data[5]) ? task->data[5] : itf_data->pending_emu_size_2d;
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

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_hyper_conf_init(struct pi_hyper_conf *conf)
{
    conf->device = PI_DEVICE_HYPERBUS_TYPE;
    conf->cs = 0;
    conf->type = PI_HYPER_TYPE_RAM;
    conf->id = 0;
    conf->baudrate = 0;
    conf->burst_length = -1;
    conf->latency = 6;
}

int32_t __pi_hyper_open(struct pi_hyper_conf *conf,
                        struct hyper_itf_conf_s **driver_conf)
{
    uint32_t irq = __disable_irq();

    /* Allocate driver conf. */
    *driver_conf = pi_fc_l1_malloc(sizeof(struct hyper_itf_conf_s));
    if (*driver_conf == NULL)
    {
        HYPER_TRACE_ERR("Driver conf alloc failed !\n");
        __restore_irq(irq);
        return -11;
    }
    else
    {
        HYPER_TRACE("Driver conf allocated: %p.\n", *driver_conf);
    }
    /* Set driver conf. */
    struct hyper_itf_conf_s *config = (struct hyper_itf_conf_s *) *driver_conf;
    config->device_id = conf->id;
    config->hyper_type = conf->type;
    config->cs = conf->cs;

    /* Set clock_div. */
    config->clk_div = __pi_hyper_clk_div_get(conf->baudrate);

    uint8_t hyper_type = config->hyper_type;

    struct hyper_itf_data_s *itf_data = g_hyper_itf_data[conf->id];
    if (itf_data == NULL)
    {
        itf_data = pi_fc_l1_malloc(sizeof(struct hyper_itf_data_s));
        if (itf_data == NULL)
        {
            HYPER_TRACE_ERR("Driver data alloc failed !\n");
            pi_fc_l1_free(*driver_conf, sizeof(struct hyper_itf_conf_s));
            *driver_conf = NULL;
            __restore_irq(irq);
            return -12;
        }
        g_hyper_itf_data[conf->id] = itf_data;
        pi_udma_fifo_init(&(itf_data->udma_chan));
        itf_data->nb_open = 0;
        itf_data->device_id = conf->id;
        itf_data->pending_emu_size = 0;

        HYPER_TRACE("Disable CG for HYPER(%d), addr=%lx\n", conf->id, hyper(0));
        HYPER_TRACE("CG=%lx, val=%lx\n", udma_ctrl, udma_ctrl_cg_get());
        /* Disable UDMA CG. */
        hal_udma_ctrl_cg_disable(UDMA_HYPER_ID(itf_data->device_id));
        HYPER_TRACE("CG=%lx, val=%lx\n", udma_ctrl, udma_ctrl_cg_get());

        /* Set handlers. */
        pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_RX(itf_data->device_id),
                                __pi_hyper_handler);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_TX(itf_data->device_id),
                                __pi_hyper_handler);
        //pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_EOT(itf_data->device_id), );

        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_RX(itf_data->device_id));
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_TX(itf_data->device_id));
        //hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_EOT(itf_data->device_id));

        /* Hyperbus settings(MBA, timing,...) */
        __pi_hyper_settings(config);
    }
    itf_data->nb_open++;
    HYPER_TRACE("HYPER(%d) : open=%ld\n", itf_data->device_id, itf_data->nb_open);

    __restore_irq(irq);
    return 0;
}

void __pi_hyper_close(struct hyper_itf_conf_s *conf)
{
    uint32_t irq = __disable_irq();

    struct hyper_itf_data_s *itf_data = g_hyper_itf_data[conf->device_id];
    itf_data->nb_open--;
    /* Close itf if no devices opened. */
    if (itf_data->nb_open == 0)
    {
        HYPER_TRACE("HYPER(%d) : closing\n", conf->device_id);

        /* Enable UDMA CG. */
        hal_udma_ctrl_cg_enable(UDMA_HYPER_ID(itf_data->device_id));

        /* Set handlers. */
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_HYPER_RX(itf_data->device_id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_HYPER_TX(itf_data->device_id));
        //pi_fc_event_handler_set(SOC_EVENT_UDMA_HYPER_EOT(itf_data->device_id), );

        /* Enable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_HYPER_RX(itf_data->device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_HYPER_TX(itf_data->device_id));
        //hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_HYPER_EOT(itf_data->device_id));

        /* Free allocated itf struct. */
        pi_fc_l1_free(itf_data, sizeof(struct hyper_itf_data_s));
        g_hyper_itf_data[conf->device_id] = NULL;
    }
    /* Free allocated conf struct. */
    pi_fc_l1_free(conf, sizeof(struct hyper_itf_conf_s));

    __restore_irq(irq);
}

void __pi_hyper_copy(struct hyper_itf_conf_s *conf,
                     struct hyper_transfer_s *transfer, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct hyper_itf_data_s *itf_data = g_hyper_itf_data[conf->device_id];
    task->data[0] = transfer->buffer;
    task->data[1] = transfer->size;
    task->data[2] = transfer->channel;
    task->data[3] = transfer->hyper_addr;
    task->data[4] = transfer->stride;
    task->data[5] = transfer->length;
    task->data[6] = 0;          /* is_emulated. */
    task->data[7] = 0;          /* repeat size. */

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Only one transfer at a time. */
    if (hw_buff_idx == 0)
    {
        HYPER_TRACE("HYPER(%d) : enqueue transfer in HW buffer. "
                    "Task=%lx, hyper_addr=%lx, l2_buf=%lx, size=%ld, channel=%d\n",
                    itf_data->device_id, task, task->data[3], task->data[0],
                    task->data[1], task->data[2]);
        __pi_hyper_copy_exec(itf_data, task);
    }
    else
    {
        HYPER_TRACE("HYPER(%d) : in use, enqueue transfer in SW buffer. "
                    "Task=%lx, hyper_addr=%lx, l2_buf=%lx, size=%ld, channel=%d\n",
                    conf->device_id, task, task->data[3], task->data[0],
                    task->data[1], task->data[2]);
        pi_udma_fifo_sw_fifo_task_enqueue(udma_chan, task);
    }
    __restore_irq(irq);
}

void __pi_hyper_copy_2d(struct hyper_itf_conf_s *conf,
                        struct hyper_transfer_s *transfer, struct pi_task *task)
{
    uint32_t irq = __disable_irq();
    struct hyper_itf_data_s *itf_data = g_hyper_itf_data[conf->device_id];
    task->data[0] = transfer->buffer;
    task->data[1] = transfer->size;
    task->data[2] = transfer->channel;
    task->data[3] = transfer->hyper_addr;
    task->data[4] = transfer->stride;
    task->data[5] = transfer->length;
    task->data[6] = 1;          /* is_emulated. */
    task->data[7] = 0;          /* repeat size. */

    pi_udma_fifo_t *udma_chan = &(itf_data->udma_chan);
    int32_t hw_buff_idx = pi_udma_fifo_hw_buffer_empty(udma_chan);
    /* Only one transfer at a time. */
    if (hw_buff_idx == 0)
    {
        HYPER_TRACE("HYPER(%d) : enqueue transfer in HW buffer. "
                    "Task=%lx, hyper_addr=%lx, l2_buf=%lx, size=%ld, channel=%d\n",
                    itf_data->device_id, task, task->data[3], task->data[0],
                    task->data[1], task->data[2]);
        __pi_hyper_copy_2d_exec(itf_data, task);
    }
    else
    {
        HYPER_TRACE("HYPER(%d) : in use, enqueue transfer in SW buffer. "
                    "Task=%lx, hyper_addr=%lx, l2_buf=%lx, size=%ld, channel=%d\n",
                    conf->device_id, task, task->data[3], task->data[0],
                    task->data[1], task->data[2]);
        pi_udma_fifo_sw_fifo_task_enqueue(udma_chan, task);
    }
    __restore_irq(irq);
}

int32_t __pi_hyper_ioctl(struct hyper_itf_conf_s *conf, uint32_t cmd, void *arg)
{
    uint32_t irq = __disable_irq();

    switch (cmd)
    {
    default :
        break;
    }
    __restore_irq(irq);
    return 0;
}
