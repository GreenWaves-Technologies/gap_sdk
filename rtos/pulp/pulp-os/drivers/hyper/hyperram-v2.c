/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"


// If not NULL, this task is enqueued when the current transfer is finished.
RT_FC_TINY_DATA struct pi_task *__rt_hyper_end_task;
// If not NULL, a task is on-going, all other should be put on-hold
RT_FC_TINY_DATA struct pi_task *__rt_hyper_current_task;

// Following variables are used to reenqueue transfers to overcome burst limit.
// This is used directly by assebly to quickly reenqueue the transfer.
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_base;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_hyper_addr;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_addr;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_repeat;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_repeat_size;

// Head and tail of the queue of pending transfers which were put on hold
// as a transfer was already on-going.
RT_FC_TINY_DATA struct pi_task *__rt_hyper_pending_tasks;
RT_FC_TINY_DATA struct pi_task *__rt_hyper_pending_tasks_last;

// All the following are used to keep track of the current transfer when it is
// emulated due to aligment constraints.
// The interrupt handler executed at end of transfer will execute the FSM to reenqueue
// a partial transfer.
RT_FC_TINY_DATA int __rt_hyper_pending_emu_channel;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_emu_hyper_addr;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_emu_addr;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_emu_size;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_emu_size_2d;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_emu_length;
RT_FC_TINY_DATA unsigned int __rt_hyper_pending_emu_stride;
RT_FC_TINY_DATA unsigned char __rt_hyper_pending_emu_do_memcpy;
RT_FC_TINY_DATA struct pi_task *__rt_hyper_pending_emu_task;

// Local task used to enqueue cluster requests.
// We cannot reuse the task coming from cluster side as it is used by the emulation
// state machine so we copy the request here to improve performance.
static struct pi_task __pi_hyper_cluster_task;
static pi_cl_hyper_req_t *__pi_hyper_cluster_reqs_first;
static pi_cl_hyper_req_t *__pi_hyper_cluster_reqs_last;

static int __rt_hyper_open_count;


// Hyper structure allocated when opening the driver
typedef struct {
  rt_extern_alloc_t alloc;
  int channel;
  int cs;
  int type;
} pi_hyper_t;



// Allocate all resources for hyper driver, especially takes care of the hyper allocator
static int __pi_hyper_init(pi_hyper_t *hyper, int ramsize);

// Free all resources allocated for the driver
static void __pi_hyper_free(pi_hyper_t *hyper);

// Performs a direct aligned copy:
//  - hyper addr is multiple of 2
//  - l2 addr is multiple of 4
//  - size is multiple of 4
void __attribute__((noinline)) __pi_hyper_copy_aligned(int channel,
  uint32_t addr, uint32_t _hyper_addr, uint32_t size, pi_task_t *event);

// Performs a misaligned 2d read without any constraint.
// This function can be either called directly or as an event callback
// This function is like a state machine,
// it checks the state of the pending copy and does one more step
// so that the whole transfer can be done asynchronously without blocking
// the core.
static int __pi_hyper_resume_misaligned_read(struct pi_task *task);

// Performs a misaligned 2d write without any constraint.
// This function can be either called directly or as an event callback
// This function is like a state machine,
// it checks the state of the pending copy and does one more step
// so that the whole transfer can be done asynchronously without blocking
// the core.
static int __pi_hyper_resume_misaligned_write(struct pi_task *task);

// Continue the pending misaligned transfer until nothing else can be done.
// This can also switch to the next pending task if the pending one is done.
static void __pi_hyper_copy_misaligned(struct pi_task *task);

// Execute a 1D copy.
// Figure out if the copy can be pushed directly (if it has good alignments)
// or if it should be handled with partial copies
static void __pi_hyper_copy_exec(int channel, uint32_t addr, uint32_t hyper_addr, uint32_t size, pi_task_t *event);

// Execute a 2D copy.
// Contrary to 1D copies, 2D copies are always handled with partial copies
static void __pi_hyper_2d_copy_exec(int channel, uint32_t addr, uint32_t hyper_addr, uint32_t size, int stride, uint32_t length, pi_task_t *event);

// CHeck if there is a task waiting for execute it and if so, remove it from the queue
// and execute it
static void exec_pending_task();

// Try to trigger a copy. If there is already one pending, the copy is put on hold,
// otherwise it is execute.
static void __pi_hyper_copy(int channel,
  uint32_t addr, uint32_t hyper_addr, uint32_t size, pi_task_t *event, int cs);

// Try to trigger a 2d copy. If there is already one pending, the copy is put on hold,
// otherwise it is execute.
static void __pi_hyper_copy_2d(int channel, uint32_t addr, uint32_t hyper_addr, uint32_t size, int stride, int length, pi_task_t *event, int cs);

// This is called by the interrupt handler when a transfer is finished and a pending
// misaligned transfer is detected, to continue it.
void __rt_hyper_resume_emu_task();

// This is called by the interrupt handler when a transfer is finished and a waiting
// transfer is detected, to execute it.
void __rt_hyper_resume_copy(struct pi_task *task);

// Execute a transfer request from cluster side
static void __pi_hyper_cluster_req_exec(pi_cl_hyper_req_t *req);

// Handle end of cluster request, by sending the reply to the cluster
static void __pi_hyper_cluster_req_done(void *_req);

// Handle a transfer request from cluster side.
// This will either execute it if none is pending or put it on hold
static void __pi_hyper_cluster_req(void *_req);

static inline void *l2_memcpy(void *dst0, const void *src0, size_t len0);




#define __PI_HYPER_TEMP_BUFFER_SIZE 128

// Temporary buffer of size __PI_HYPER_TEMP_BUFFER_SIZE used for misaligned
// transfers between hyper and L2
static char __pi_hyper_temp_buffer[__PI_HYPER_TEMP_BUFFER_SIZE];




void pi_hyper_conf_init(struct pi_hyper_conf *conf)
{
  conf->id = -1;
  conf->baudrate = 0;
  conf->type = PI_HYPER_TYPE_RAM;
  conf->cs = 0;
}


#ifdef __RT_HYPER_COPY_ASM

extern void __rt_hyper_handle_copy();

#else

static void __rt_hyper_handle_copy()
{
  if (__rt_hyper_pending_repeat != 0)
  {
    __rt_hyper_pending_addr += __rt_hyper_pending_repeat;
    __rt_hyper_pending_hyper_addr += __rt_hyper_pending_repeat;
    __rt_hyper_pending_repeat_size -= __rt_hyper_pending_repeat;

    uint32_t iter_size = __rt_hyper_pending_repeat;

    if (__rt_hyper_pending_repeat_size <= __rt_hyper_pending_repeat)
    {
      __rt_hyper_pending_repeat = 0;
      iter_size = __rt_hyper_pending_repeat_size;
    }

    hal_hyper_enqueue(__rt_hyper_pending_base, (unsigned int)__rt_hyper_pending_addr, __rt_hyper_pending_hyper_addr, iter_size, UDMA_CHANNEL_CFG_EN);
  }
  else
  {
    pi_task_t *task = __rt_hyper_end_task;
    if (task != NULL)
    {
      __rt_hyper_end_task = NULL;
      __rt_hyper_current_task = NULL;
      __rt_event_handle_end_of_task(task);
    }

    if (__rt_hyper_pending_emu_task != NULL)
    {
      __rt_hyper_resume_emu_task();
      return;
    }

    if (__rt_hyper_pending_tasks != NULL)
    {
      __rt_hyper_resume_copy(__rt_hyper_pending_tasks);
    }

  }
}

#endif



static int __rt_get_div(int freq)
{
  int periph_freq = __rt_freq_periph_get();

  if (freq == 0 || freq >= periph_freq)
  {
    return 0;
  }
  else
  {
    // Round-up the divider to obtain a frequency which is below the maximum
    int div = (periph_freq + freq - 1) / freq;

    // The divider always divide by 2 once we activate the divider, thus increase by 1
    // in case it is even to not go above the max frequency.
    if (div & 1) div += 1;
    div >>= 1;

    return div;
  }
}



int32_t pi_hyper_open(struct pi_device *device)
{
  struct pi_hyper_conf *conf = (struct pi_hyper_conf *)device->config;
  int periph_id;
  int channel;

  periph_id = ARCHI_UDMA_HYPER_ID(conf->id);
  channel = UDMA_EVENT_ID(periph_id);

  pi_hyper_t *hyper = pmsis_l2_malloc(sizeof(pi_hyper_t));
  if (hyper == NULL) return -1;

  if (conf->type == PI_HYPER_TYPE_RAM)
  {
    if (__pi_hyper_init(hyper, 1000000))
      goto error;
  }

  hyper->channel = periph_id;
  hyper->cs = conf->cs;
  hyper->type = conf->type;

  __rt_hyper_open_count++;
  if (__rt_hyper_open_count == 1)
  {
    // Activate routing of UDMA hyper soc events to FC to trigger interrupts
    soc_eu_fcEventMask_setEvent(channel+3);

    // Deactivate Hyper clock-gating
    plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));

    // Redirect all UDMA hyper events to our callback
    __rt_udma_register_channel_callback(channel, __rt_hyper_handle_copy, NULL);
    __rt_udma_register_channel_callback(channel+1, __rt_hyper_handle_copy, NULL);
  }

  hyper_clk_div_set(UDMA_HYPER_ADDR(0), __rt_get_div(conf->baudrate));

  if ((conf->type == PI_HYPER_TYPE_RAM && conf->cs == 0) || (conf->type == PI_HYPER_TYPE_FLASH && conf->cs == 1))
  {
    hyper_device_set(UDMA_HYPER_ADDR(0),
      HYPER_DEVICE_DT1(1) |
      HYPER_DEVICE_DT0(0) |
      HYPER_DEVICE_TYPE(1)
    );
  }
  else
  {
    hyper_device_set(UDMA_HYPER_ADDR(0),
      HYPER_DEVICE_DT1(0) |
      HYPER_DEVICE_DT0(1) |
      HYPER_DEVICE_TYPE(1)
    );
  }

  hyper_mba0_set(UDMA_HYPER_ADDR(0), REG_MBR0);
  hyper_mba1_set(UDMA_HYPER_ADDR(0), REG_MBR1);

  hyper_timing_cfg_set(UDMA_HYPER_ADDR(0),
    HYPER_TIMING_CFG_CS_MAX(665) |
    HYPER_TIMING_CFG_RWDS_DELAY(1) |
    HYPER_TIMING_CFG_RW_RECOVERY(6) |
    HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(1) |
    HYPER_TIMING_CFG_LATENCY0(6) |
    HYPER_TIMING_CFG_LATENCY1(6)
  );

  hyper_irq_en_set(UDMA_HYPER_ADDR(0), 1);

  device->data = (void *)hyper;

  return 0;

error:
  pmsis_l2_malloc_free(hyper, sizeof(pi_hyper_t));
  return -1;
}



void pi_hyper_close(struct pi_device *device)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;

  __rt_hyper_open_count--;
  if (__rt_hyper_open_count == 0)
  {
    int periph_id = hyper->channel;
    int channel = UDMA_EVENT_ID(periph_id);

    // Deactivate event routing
    soc_eu_fcEventMask_clearEvent(channel + 3);

    // Reactivate clock-gating
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<periph_id));
  }

  __pi_hyper_free(hyper);
  pmsis_l2_malloc_free(hyper, sizeof(pi_hyper_t));
}



uint32_t pi_hyperram_alloc(struct pi_device *device, uint32_t size)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;
  return (uint32_t)rt_extern_alloc(&hyper->alloc, size);
}



int32_t pi_hyperram_free(struct pi_device *device, uint32_t chunk, uint32_t size)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;
  return rt_extern_free(&hyper->alloc, (void *)chunk, size);
}



void pi_hyper_read_async(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, struct pi_task *task)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;
  __rt_task_init(task);
  __pi_hyper_copy(UDMA_CHANNEL_ID(hyper->channel) + 0, (uint32_t)addr, hyper_addr, size, task, hyper->cs);
}



void pi_hyper_read(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size)
{
  struct pi_task task;
  pi_hyper_read_async(device, hyper_addr, addr, size, pi_task_block(&task));
  pi_task_wait_on(&task);
}



void pi_hyper_write_async(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, struct pi_task *task)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;
  __rt_task_init(task);
  task->done = 0;
  __pi_hyper_copy(UDMA_CHANNEL_ID(hyper->channel) + 1, (uint32_t)addr, hyper_addr, size, task, hyper->cs);
}



void pi_hyper_write(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size)
{
  struct pi_task task;
  pi_hyper_write_async(device, hyper_addr, addr, size, pi_task_block(&task));
  pi_task_wait_on(&task);
}


void pi_hyper_read_2d_async(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, struct pi_task *task)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;
  __rt_task_init(task);
  __pi_hyper_copy_2d(UDMA_CHANNEL_ID(hyper->channel) + 0, (uint32_t)addr, hyper_addr, size, stride, length, task, hyper->cs);
}



void pi_hyper_read_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length)
{
  struct pi_task task;
  pi_hyper_read_2d_async(device, hyper_addr, addr, size, stride, length, pi_task_block(&task));
  pi_task_wait_on(&task);
}



void pi_hyper_write_2d_async(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, struct pi_task *task)
{
  pi_hyper_t *hyper = (pi_hyper_t *)device->data;
  __rt_task_init(task);
  __pi_hyper_copy_2d(UDMA_CHANNEL_ID(hyper->channel) + 1, (uint32_t)addr, hyper_addr, size, stride, length, task, hyper->cs);
}



void pi_hyper_write_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length)
{
  struct pi_task task;
  pi_hyper_write_2d_async(device, hyper_addr, addr, size, stride, length, pi_task_block(&task));
  pi_task_wait_on(&task);
}



static int __pi_hyper_init(pi_hyper_t *hyper, int ramsize)
{
  if (rt_extern_alloc_init(&hyper->alloc, 0, ramsize))
    return -1;

  return 0;
}



static void __pi_hyper_free(pi_hyper_t *hyper)
{
  if (hyper->type == PI_HYPER_TYPE_RAM)
  {
    rt_extern_alloc_deinit(&hyper->alloc);
  }
}



void __attribute__((noinline)) __pi_hyper_copy_aligned(int channel,
  uint32_t addr, uint32_t hyper_addr, uint32_t size, pi_task_t *event)
{
  unsigned int base = hal_udma_channel_base(channel);

  // In case the size is bigger than the maximum burst size
  // split the transfer into smaller transfers using the repeat count
  if (size > 512) {
    __rt_hyper_pending_base = base;
    __rt_hyper_pending_hyper_addr = hyper_addr;
    __rt_hyper_pending_addr = (unsigned int)addr;
    __rt_hyper_pending_repeat = 512;
    __rt_hyper_pending_repeat_size = size;
    size = 512;
  } else {
    __rt_hyper_pending_repeat = 0;
  }

  __rt_hyper_end_task = event;

  hal_hyper_enqueue(base, (unsigned int)addr, hyper_addr, size, UDMA_CHANNEL_CFG_EN);
}



static int __pi_hyper_resume_misaligned_read(struct pi_task *task)
{
  while (1)
  {
    // Compute information to see how to do one more step
    int addr_aligned = (__rt_hyper_pending_emu_addr + 3) & ~0x3;
    int prologue_size = addr_aligned - (int)__rt_hyper_pending_emu_addr;
    int hyper_addr_aligned = __rt_hyper_pending_emu_hyper_addr + prologue_size;

    if (__rt_hyper_pending_emu_size < 4)
      prologue_size = __rt_hyper_pending_emu_size;

    if (prologue_size)
    {
      // Case where we have a partial copy to do
      if (!__rt_hyper_pending_emu_do_memcpy)
      {
        // A partial transfer must first transfer to the temporary area
        // and finish the transfer by hands using a memcpy.
        // This part is first called to trigger the transfer while the part after
        // is called to do the memcpy as a second step.
        __pi_hyper_copy_aligned(
          __rt_hyper_pending_emu_channel,
          (uint32_t)__pi_hyper_temp_buffer,
          (__rt_hyper_pending_emu_hyper_addr & ~1),
          4,
          NULL
        );

        // It is asynchronous, just remember we have to do
        // a memcpy when the transfer is done and leave
        __rt_hyper_pending_emu_do_memcpy = 1;
        return 0;
      }

      __rt_hyper_pending_emu_do_memcpy = 0;
      l2_memcpy((void *)__rt_hyper_pending_emu_addr, &__pi_hyper_temp_buffer[__rt_hyper_pending_emu_hyper_addr & 1], prologue_size);

      __rt_hyper_pending_emu_hyper_addr += prologue_size;
      __rt_hyper_pending_emu_addr += prologue_size;
      __rt_hyper_pending_emu_size -= prologue_size;

      // The transfer is asynchronous, we get there to do the memcpy
      // without triggering any transfer, so we can start again to trigger one.
      if (__rt_hyper_pending_emu_size)
        continue;

    }
    else if (__rt_hyper_pending_emu_size > 0)
    {
      // Case where we have the body to transfer
      uint32_t size_aligned = __rt_hyper_pending_emu_size & ~0x3;

      if ((hyper_addr_aligned & 0x1) == 0)
      {
        // Good case where the body is aligned on both sides and we can do
        // a direct copy.
        __pi_hyper_copy_aligned(
          __rt_hyper_pending_emu_channel,
          __rt_hyper_pending_emu_addr,
          __rt_hyper_pending_emu_hyper_addr,
          size_aligned,
          NULL
        );

        __rt_hyper_pending_emu_hyper_addr += size_aligned;
        __rt_hyper_pending_emu_addr += size_aligned;
        __rt_hyper_pending_emu_size -= size_aligned;

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
          size_aligned = __PI_HYPER_TEMP_BUFFER_SIZE - 4;

        if (!__rt_hyper_pending_emu_do_memcpy)
        {
          __pi_hyper_copy_aligned(
            __rt_hyper_pending_emu_channel,
            (uint32_t)__pi_hyper_temp_buffer,
            (__rt_hyper_pending_emu_hyper_addr & ~1),
            size_aligned+4,
            NULL
          );

          __rt_hyper_pending_emu_do_memcpy = 1;
          return 0;
        }

        __rt_hyper_pending_emu_do_memcpy = 0;
        l2_memcpy((void *)__rt_hyper_pending_emu_addr, &__pi_hyper_temp_buffer[1], size_aligned);

        __rt_hyper_pending_emu_hyper_addr += size_aligned;
        __rt_hyper_pending_emu_addr += size_aligned;
        __rt_hyper_pending_emu_size -= size_aligned;

        if (__rt_hyper_pending_emu_size)
          continue;
      }
    }

    // Now check if we are done
    if (__rt_hyper_pending_emu_size == 0)
    {
      // Check if we are doing a 2D transfer
      if (__rt_hyper_pending_emu_size_2d > 0)
      {
        // In this case, update the global size
        if (__rt_hyper_pending_emu_size_2d > __rt_hyper_pending_emu_length)
          __rt_hyper_pending_emu_size_2d -= __rt_hyper_pending_emu_length;
        else
          __rt_hyper_pending_emu_size_2d = 0;

        // And check if we must reenqueue a line.
        if (__rt_hyper_pending_emu_size_2d > 0)
        {
          __rt_hyper_pending_emu_hyper_addr = __rt_hyper_pending_emu_hyper_addr - __rt_hyper_pending_emu_length + __rt_hyper_pending_emu_stride;
          __rt_hyper_pending_emu_size = __rt_hyper_pending_emu_size_2d > __rt_hyper_pending_emu_length ? __rt_hyper_pending_emu_length : __rt_hyper_pending_emu_size_2d;
          continue;
        }
      }

      __rt_hyper_pending_emu_task = NULL;
      __rt_hyper_current_task = NULL;
      __rt_event_handle_end_of_task(task);

      return 1;
    }
    break;
  }

  return 0;
}



static int __pi_hyper_resume_misaligned_write(struct pi_task *task)
{

  while(1)
  {
    // Compute information to see how to do one more step
    int addr_aligned = (__rt_hyper_pending_emu_addr + 3) & ~0x3;
    int prologue_size = addr_aligned - __rt_hyper_pending_emu_addr;
    int hyper_addr_aligned = __rt_hyper_pending_emu_hyper_addr + prologue_size;

    if (__rt_hyper_pending_emu_size < 4)
      prologue_size = __rt_hyper_pending_emu_size;

    if (prologue_size)
    {
      // Case where we have a partial copy to do
      if (!__rt_hyper_pending_emu_do_memcpy)
      {
        // A partial transfer must first transfer the content of the hyper
        // to the temporary area and partially overwrite it with a memcpy.
        // This part is first called to trigger the transfer while the part after
        // is called to do the memcpy and the final transfer as a second step.
        __pi_hyper_copy_aligned(
          __rt_hyper_pending_emu_channel-1,
          (uint32_t)__pi_hyper_temp_buffer,
          (__rt_hyper_pending_emu_hyper_addr & ~1),
          4,
          NULL
        );

        // It is asynchronous, just remember we have to do
        // a memcpy when the transfer is done and leave
        __rt_hyper_pending_emu_do_memcpy = 1;
        return 0;
      }

      __rt_hyper_pending_emu_do_memcpy = 0;
      l2_memcpy(&__pi_hyper_temp_buffer[__rt_hyper_pending_emu_hyper_addr & 1], (void *)__rt_hyper_pending_emu_addr, prologue_size);

      __pi_hyper_copy_aligned(
        __rt_hyper_pending_emu_channel,
        (uint32_t)__pi_hyper_temp_buffer,
        (__rt_hyper_pending_emu_hyper_addr & ~1),
        4,
        NULL
      );

      __rt_hyper_pending_emu_hyper_addr += prologue_size;
      __rt_hyper_pending_emu_addr += prologue_size;
      __rt_hyper_pending_emu_size -= prologue_size;

      return 0;
    }
    else if (__rt_hyper_pending_emu_size > 0)
    {
      // Case where we have the body to transfer
      uint32_t size_aligned = __rt_hyper_pending_emu_size & ~0x3;

      if ((hyper_addr_aligned & 0x1) == 0)
      {
        // Good case where the body is aligned on both sides and we can do
        // a direct copy.
        __pi_hyper_copy_aligned(
          __rt_hyper_pending_emu_channel,
          __rt_hyper_pending_emu_addr,
          __rt_hyper_pending_emu_hyper_addr,
          size_aligned,
          NULL
        );

        __rt_hyper_pending_emu_hyper_addr += size_aligned;
        __rt_hyper_pending_emu_addr += size_aligned;
        __rt_hyper_pending_emu_size -= size_aligned;

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
          size_aligned = __PI_HYPER_TEMP_BUFFER_SIZE - 4;

        if (!__rt_hyper_pending_emu_do_memcpy)
        {
          __pi_hyper_copy_aligned(
            __rt_hyper_pending_emu_channel-1,
            (uint32_t)__pi_hyper_temp_buffer,
            (__rt_hyper_pending_emu_hyper_addr & ~1),
            4,
            NULL
          );

          __rt_hyper_pending_emu_do_memcpy = 1;
          return 0;
        }

        __rt_hyper_pending_emu_do_memcpy = 0;
        l2_memcpy(&__pi_hyper_temp_buffer[1], (void *)__rt_hyper_pending_emu_addr, size_aligned-1);

        __pi_hyper_copy_aligned(
          __rt_hyper_pending_emu_channel,
          (uint32_t)__pi_hyper_temp_buffer,
          (__rt_hyper_pending_emu_hyper_addr & ~1),
          size_aligned,
          NULL
        );

        __rt_hyper_pending_emu_hyper_addr += size_aligned-1;
        __rt_hyper_pending_emu_addr += size_aligned-1;
        __rt_hyper_pending_emu_size -= size_aligned-1;

        return 0;
      }
    }

    // Now check if we are done
    if (__rt_hyper_pending_emu_size == 0)
    {
      // Check if we are doing a 2D transfer
      if (__rt_hyper_pending_emu_size_2d > 0)
      {
        // In this case, update the global size
        if (__rt_hyper_pending_emu_size_2d > __rt_hyper_pending_emu_length)
          __rt_hyper_pending_emu_size_2d -= __rt_hyper_pending_emu_length;
        else
          __rt_hyper_pending_emu_size_2d = 0;

        // And check if we must reenqueue a line.
        if (__rt_hyper_pending_emu_size_2d > 0)
        {
          __rt_hyper_pending_emu_hyper_addr = __rt_hyper_pending_emu_hyper_addr - __rt_hyper_pending_emu_length + __rt_hyper_pending_emu_stride;
          __rt_hyper_pending_emu_size = __rt_hyper_pending_emu_size_2d > __rt_hyper_pending_emu_length ? __rt_hyper_pending_emu_length : __rt_hyper_pending_emu_size_2d;
          continue;
        }
      }

      __rt_hyper_pending_emu_task = NULL;
      __rt_hyper_current_task = NULL;
      __rt_event_handle_end_of_task(task);

      return 1;
    }
    break;
  }

  return 0;
}



static void __pi_hyper_copy_misaligned(struct pi_task *task)
{
    int end;
    if (__rt_hyper_pending_emu_channel & 1)
      end = __pi_hyper_resume_misaligned_write(task);
    else
      end = __pi_hyper_resume_misaligned_read(task);

    if (!end)
      return;

    exec_pending_task();
}



static void __pi_hyper_copy_exec(int channel, uint32_t addr, uint32_t hyper_addr, uint32_t size, pi_task_t *event)
{
  __rt_hyper_current_task = event;

  // Check if we are in the fast case where everything is correctly aligned.
  if (likely((((int)addr & 0x3) == 0) && (((int)hyper_addr) & 0x1) == 0 && (((int)size & 0x3) == 0 || ((channel & 1) && ((int)size & 0x1) == 0))))
  {
    __pi_hyper_copy_aligned(channel, addr, hyper_addr, size, event);
  }
  else
  {
    // Otherwise go through the slow misaligned case.
    __rt_hyper_pending_emu_channel = channel;
    __rt_hyper_pending_emu_hyper_addr = (unsigned int)hyper_addr;
    __rt_hyper_pending_emu_addr = (unsigned int)addr;
    __rt_hyper_pending_emu_size = size;
    __rt_hyper_pending_emu_do_memcpy = 0;
    __rt_hyper_pending_emu_task = event;

    __pi_hyper_copy_misaligned(event);
  }
}



static void __pi_hyper_2d_copy_exec(int channel, uint32_t addr, uint32_t hyper_addr, uint32_t size, int stride, uint32_t length, pi_task_t *event)
{
  __rt_hyper_current_task = event;

  // Otherwise go through the slow misaligned case.
  __rt_hyper_pending_emu_channel = channel;
  __rt_hyper_pending_emu_hyper_addr = hyper_addr;
  __rt_hyper_pending_emu_addr = (unsigned int)addr;
  __rt_hyper_pending_emu_size = size > length ? length : size;
  __rt_hyper_pending_emu_do_memcpy = 0;
  __rt_hyper_pending_emu_task = event;
  __rt_hyper_pending_emu_size_2d = size;
  __rt_hyper_pending_emu_length = length;
  __rt_hyper_pending_emu_stride = stride;

  __pi_hyper_copy_misaligned(event);
}



static void exec_pending_task()
{
  struct pi_task *task = __rt_hyper_pending_tasks;

  if (task)
  {
    __rt_hyper_pending_tasks = task->implem.next;

    int is_2d = (task->implem.data[0] >> 8) & 0xff;
    unsigned int channel = task->implem.data[0] & 0xff;
    uint32_t addr = task->implem.data[1];
    uint32_t hyper_addr = task->implem.data[2];
    uint32_t size = task->implem.data[3];

    if (!is_2d)
    {
      __pi_hyper_copy_exec(channel, addr, hyper_addr, size, task);
    }
    else
    {
      uint32_t stride = task->implem.data[4];
      uint32_t length = task->implem.data[5];
      __pi_hyper_2d_copy_exec(channel, addr, hyper_addr, size, stride, length, task);
    }
  }
}



// TODO this should be done using UDMA
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



void __pi_hyper_copy(int channel,
  uint32_t addr, uint32_t hyper_addr, uint32_t size, pi_task_t *event, int cs)
{
  int irq = rt_irq_disable();

  if (cs)
    hyper_addr += REG_MBR1;

  if (__rt_hyper_current_task != NULL)
  {
    if (__rt_hyper_pending_tasks != NULL)
    __rt_hyper_pending_tasks_last->implem.next = event;
    else
      __rt_hyper_pending_tasks = event;
    __rt_hyper_pending_tasks_last = event;
    event->implem.next = NULL;

    event->implem.data[0] = channel;
    event->implem.data[1] = (unsigned int)addr;
    event->implem.data[2] = (unsigned int)hyper_addr;
    event->implem.data[3] = size;
  }
  else
  {
    __pi_hyper_copy_exec(channel, addr, hyper_addr, size, event);
  }

  rt_irq_restore(irq);
}



void __pi_hyper_copy_2d(int channel,
  uint32_t addr, uint32_t hyper_addr, uint32_t size, int stride, int length, pi_task_t *event, int cs)
{
  int irq = rt_irq_disable();

  if (cs)
    hyper_addr += REG_MBR1;

  if (__rt_hyper_current_task != NULL)
  {
    if (__rt_hyper_pending_tasks != NULL)
    __rt_hyper_pending_tasks_last->implem.next = event;
    else
      __rt_hyper_pending_tasks = event;
    __rt_hyper_pending_tasks_last = event;
    event->implem.next = NULL;

    event->implem.data[0] = channel | (1<<8);
    event->implem.data[1] = (unsigned int)addr;
    event->implem.data[2] = (unsigned int)hyper_addr;
    event->implem.data[3] = size;
    event->implem.data[4] = stride;
    event->implem.data[5] = length;
  }
  else
  {
    __pi_hyper_2d_copy_exec(channel, addr, hyper_addr, size, stride, length, event);
  }

  rt_irq_restore(irq);
}



void __rt_hyper_resume_emu_task()
{
  // The pending copy is an emulated misaligned request, just continue it
  __pi_hyper_copy_misaligned(__rt_hyper_pending_emu_task);
}



void __rt_hyper_resume_copy(struct pi_task *task)
{
  exec_pending_task();
}



static void __attribute__((constructor)) __rt_hyper_init()
{
  __rt_hyper_end_task = NULL;
  __rt_hyper_current_task = NULL;
  __rt_hyper_pending_tasks = NULL;
  __pi_hyper_cluster_reqs_first = NULL;
  __rt_hyper_pending_emu_channel = -1;
  __rt_hyper_open_count = 0;
  __rt_hyper_pending_emu_size = 0;
  __rt_hyper_pending_emu_size_2d = 0;
}



#if defined(ARCHI_HAS_CLUSTER)

static void __pi_hyper_cluster_req_exec(pi_cl_hyper_req_t *req)
{
  pi_hyper_t *hyper = (pi_hyper_t *)req->device->data;
  pi_task_t *event = &__pi_hyper_cluster_task;
  pi_task_callback(event, __pi_hyper_cluster_req_done, (void* )req);

  if(req->is_2d)
    __pi_hyper_copy_2d(UDMA_CHANNEL_ID(hyper->channel) + req->is_write, (uint32_t)req->addr, req->hyper_addr, req->size, req->stride, req->length, event, hyper->cs);
  else
    __pi_hyper_copy(UDMA_CHANNEL_ID(hyper->channel) + req->is_write, (uint32_t)req->addr, req->hyper_addr, req->size, event, hyper->cs);
}

static void __pi_hyper_cluster_req_done(void *_req)
{
  pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *)_req;
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
    __pi_hyper_cluster_reqs_first = req->next;

  req = __pi_hyper_cluster_reqs_first;
  if (req)
  {
    __pi_hyper_cluster_req_exec(req);
  }
}

static void __pi_hyper_cluster_req(void *_req)
{
  pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t* )_req;

  int is_first = __pi_hyper_cluster_reqs_first == NULL;

  if (is_first)
    __pi_hyper_cluster_reqs_first = req;
  else
    __pi_hyper_cluster_reqs_last->next = req;

  __pi_hyper_cluster_reqs_last = req;
  req->next = NULL;

  if (is_first)
  {
    __pi_hyper_cluster_req_exec(req);
  }
}

void __cl_hyper_cluster_copy(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, pi_cl_hyper_req_t *req, int ext2loc)
{
  req->device = device;
  req->addr = addr;
  req->hyper_addr = hyper_addr;
  req->size = size;
  req->cid = pi_cluster_id();
  req->done = 0;
  req->is_write = (ext2loc)? 0:1;
  req->is_2d = 0;
  __rt_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __pi_hyper_cluster_req, (void* )req);
  __rt_cluster_push_fc_event(&req->event);
}


void __cl_hyper_cluster_copy_2d(struct pi_device *device,
  uint32_t hyper_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, pi_cl_hyper_req_t *req, int ext2loc)
{
  req->device = device;
  req->addr = addr;
  req->hyper_addr = hyper_addr;
  req->size = size;
  req->stride = stride;
  req->length = length;
  req->cid = pi_cluster_id();
  req->done = 0;
  req->is_write = (ext2loc)? 0:1;
  req->is_2d = 1;
  __rt_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __pi_hyper_cluster_req, (void* )req);
  __rt_cluster_push_fc_event(&req->event);
}


void __pi_hyperram_alloc_cluster_req(void *_req)
{
  pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *)_req;
  req->result = pi_hyperram_alloc(req->device, req->size);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}



void __pi_hyperram_free_cluster_req(void *_req)
{
  pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *)_req;
  pi_hyperram_free(req->device, req->chunk, req->size);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}



void pi_cl_hyperram_alloc(struct pi_device *device, uint32_t size, pi_cl_hyperram_alloc_req_t *req)
{
  req->device = device;
  req->size = size;
  req->cid = pi_cluster_id();
  req->done = 0;
  __rt_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __pi_hyperram_alloc_cluster_req, (void* )req);
  __rt_cluster_push_fc_event(&req->event);
}



void pi_cl_hyperram_free(struct pi_device *device, uint32_t chunk, uint32_t size, pi_cl_hyperram_free_req_t *req)
{
  req->device = device;
  req->size = size;
  req->chunk = chunk;
  req->cid = pi_cluster_id();
  req->done = 0;
  __rt_task_init_from_cluster(&req->event);
  pi_task_callback(&req->event, __pi_hyperram_free_cluster_req, (void* )req);
  __rt_cluster_push_fc_event(&req->event);
}

#endif
