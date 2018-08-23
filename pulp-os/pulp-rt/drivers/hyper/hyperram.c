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
 *          Jie Chen, GreenWaves Technologies (jie.chen@greenwaves-technologies.com)
 */

#include "rt/rt_api.h"

#define __RT_HYPER_TEMP_BUFFER_SIZE 128

// Temporary buffer of size __RT_HYPER_TEMP_BUFFER_SIZE used for misaligned
// transfers between hyperram and L2
static char *__rt_hyper_temp_buffer;

// Set to 1 if there is already a pending misaligned transfer, as they are
// sharing __rt_hyper_temp_buffer
static int __rt_hyper_pending_misaligned;

// First misaligned pending copy waiting until __rt_hyper_temp_buffer is
// available
static rt_event_t *__rt_hyper_first_waiting_misaligned;



int __rt_hyperram_init(rt_hyperram_t *hyper, int ramsize)
{
  rt_extern_alloc_t *alloc = (rt_extern_alloc_t *)rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_extern_alloc_t));
  if (alloc == NULL) return -1;

  hyper->alloc = alloc;
  if (rt_extern_alloc_init(alloc, 0, ramsize)) return -1;

  return 0;
}



void rt_hyperram_conf_init(rt_hyperram_conf_t *conf)
{
  conf->id = -1;
  conf->ram_size = 0;
}



static void __rt_hyperram_free(rt_hyperram_t *hyper)
{
  if (hyper != NULL) {
    if (hyper->alloc != NULL) rt_free(RT_ALLOC_FC_DATA, (void *)hyper, sizeof(rt_extern_alloc_t));
    rt_free(RT_ALLOC_FC_DATA, (void *)hyper, sizeof(rt_hyperram_t));
  }
}



rt_hyperram_t *rt_hyperram_open(char *dev_name, rt_hyperram_conf_t *conf, rt_event_t *event)
{
  rt_hyperram_t *hyper = NULL;
  int channel;
  int ramsize;

  if (dev_name)
  {
    rt_dev_t *dev = rt_dev_get(dev_name);
    if (dev == NULL) goto error;
    channel = dev->channel;
    ramsize = dev->u.hyperram.size;
  }
  else
  {
    channel = ARCHI_UDMA_HYPER_ID(conf->id);
    ramsize = conf->ram_size;
  }


  if (__rt_hyper_temp_buffer == NULL)
  {
    __rt_hyper_temp_buffer = rt_alloc(RT_ALLOC_PERIPH, __RT_HYPER_TEMP_BUFFER_SIZE);
    if (__rt_hyper_temp_buffer == NULL) goto error;
  }

  hyper = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_hyperram_t));
  if (hyper == NULL) goto error;

  hyper->alloc = NULL;
  hyper->channel = channel;

  if (__rt_hyperram_init(hyper, ramsize)) goto error;

  return hyper;

error:
  __rt_hyperram_free(hyper);
  return NULL;
}



void rt_hyperram_close(rt_hyperram_t *handle, rt_event_t *event)
{
  __rt_hyperram_free(handle);
}



#if defined(ARCHI_HAS_CLUSTER)

void __rt_hyperram_cluster_req_done(void *_req)
{
  rt_hyperram_req_t *req = (rt_hyperram_req_t *)_req;
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}



void __rt_hyperram_cluster_req(void *_req)
{
  rt_hyperram_req_t *req = (rt_hyperram_req_t *)_req;
  rt_event_t *event = &req->event;
  __rt_init_event(event, event->sched, __rt_hyperram_cluster_req_done, (void *)req);
  __rt_event_set_pending(event);
  __rt_hyper_copy(UDMA_CHANNEL_ID(req->dev->channel) + req->is_write, req->addr, req->hyper_addr, req->size, event, REG_MBR0);
}



void __rt_hyperram_cluster_copy(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_hyperram_req_t *req, int is_write)
{
  req->dev = dev;
  req->addr = addr;
  req->hyper_addr = hyper_addr;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->is_write = is_write;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_hyperram_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}



void __rt_hyperram_alloc_cluster_req(void *_req)
{
  rt_hyperram_alloc_req_t *req = (rt_hyperram_alloc_req_t *)_req;
  req->result = rt_hyperram_alloc(req->dev, req->size);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}



void __rt_hyperram_free_cluster_req(void *_req)
{
  rt_hyperram_free_req_t *req = (rt_hyperram_free_req_t *)_req;
  rt_hyperram_free(req->dev, req->chunk, req->size);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}



void rt_hyperram_alloc_cluster(rt_hyperram_t *dev, int size, rt_hyperram_alloc_req_t *req)
{
  req->dev = dev;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_hyperram_alloc_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}



void rt_hyperram_free_cluster(rt_hyperram_t *dev, void *chunk, int size, rt_hyperram_free_req_t *req)
{
  req->dev = dev;
  req->size = size;
  req->chunk = chunk;
  req->cid = rt_cluster_id();
  req->done = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_hyperram_free_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

#endif


// Performs a direct aligned copy:
//  - hyper addr is multiple of 2
//  - l2 addr is multiple of 4
//  - size is multiple of 4
void __attribute__((noinline)) __rt_hyper_copy_aligned(int channel,
  void *addr, void *hyper_addr, int size, rt_event_t *event, int mbr)
{
  rt_periph_copy_t *copy = &event->copy;

  copy->ctrl = RT_PERIPH_COPY_HYPER << RT_PERIPH_COPY_CTRL_TYPE_BIT;
  copy->u.hyper.hyper_addr = mbr | (unsigned int)hyper_addr;
  // In case the size is bigger than the maximum burst size
  // split the transfer into smaller transfers using the repeat count
  if (size > 512) {
    copy->addr = (unsigned int)addr;
    copy->repeat = 512;
    copy->repeat_size = size;
    size = 512;
  } else {
    copy->repeat = 0;
  }

  rt_periph_copy(copy, channel, (unsigned int)addr, size, UDMA_CHANNEL_CFG_SIZE_16, event);
}




// Performs a misaligned 2d read without any constraint.
// This function can be either called directly or as an event callback
// This function is like a state machine, 
// it checks the state of the pending copy and does one more step
// so that the whole transfer can be done asynchronously without blocking
// the core.
static void __rt_hyper_resume_misaligned_read(void *arg)
{
  int irq = rt_irq_disable();

start:
  {
    rt_event_t *event = (rt_event_t *)arg;
    rt_periph_copy_t *copy = &event->copy;

    // Extract the current state of the transfer to see how to continue it
    int addr = copy->u.hyper.pending_addr;
    int hyper_addr = copy->u.hyper.pending_hyper_addr;
    int channel = copy->u.hyper.channel;
    int mbr = copy->u.hyper.mbr;
    int size = copy->u.hyper.pending_size;
    int async = copy->u.hyper.async;
    int do_memcpy = copy->u.hyper.memcpy;
    copy->u.hyper.memcpy = 0;

    // Compute information to see how to do one more step
    int addr_aligned = (addr + 3) & ~0x3;
    int prologue_size = addr_aligned - (int)addr;
    int hyper_addr_aligned = hyper_addr + prologue_size;

    if (size < 4)
      prologue_size = size;

    // In case the transfer is asynchronous, prepare the callback so that
    // we are called again when the next step is over
    if (async)
    {
      __rt_init_event(event, event->sched, __rt_hyper_resume_misaligned_read, event);
      // Be careful to not free the event, it is reused to finish the transfer
      __rt_event_set_pending(event);
    }

    if (prologue_size)
    {
      // Case where we have a partial copy to do
      if (!do_memcpy)
      {
        // A partial transfer must first transfer to the temporary area
        // and finish the transfer by hands using a memcpy.
        // This part is first called to trigger the transfer while the part after
        // is called to do the memcpy as a second step.
        __rt_hyper_copy_aligned(channel, (void *)__rt_hyper_temp_buffer, (void *)(hyper_addr & ~1), 4, event, mbr);

        // In case it is asynchronous, just remember we have to do
        // a memcpy when the transfer is done and leave
        if (async) {
          copy->u.hyper.memcpy = 1;
          goto end;
        }

        // Otherwise finish the transfer now
        __rt_wait_event(event);
      }

      memcpy((void *)addr, &__rt_hyper_temp_buffer[hyper_addr & 1], prologue_size);

      copy->u.hyper.pending_hyper_addr += prologue_size;
      copy->u.hyper.pending_addr += prologue_size;
      copy->u.hyper.pending_size -= prologue_size;

      // In case the transfer is asynchronous, we get there to do the memcpy
      // without triggering any transfer, so we can start again to trigger one.
      if (async && copy->u.hyper.pending_size) goto start;

    }
    else if (size > 0)
    {
      // Case where we have the body to transfer
      int size_aligned = size & ~0x3;

      if ((hyper_addr_aligned & 0x1) == 0)
      {
        // Good case where the body is aligned on both sides and we can do
        // a direct copy.
        __rt_hyper_copy_aligned(channel, (void *)addr, (void *)hyper_addr, size_aligned, event, mbr);

        copy->u.hyper.pending_hyper_addr += size_aligned;
        copy->u.hyper.pending_addr += size_aligned;
        copy->u.hyper.pending_size -= size_aligned;

        // In case it is asynchronous, just leave, we'll continue the transfer
        // when this one is over
        if (async) goto end;

        __rt_wait_event(event);

      }
      else
      {
        // Bad case where we have to transfer the body using a temporary
        // buffer as the aligments on both sides are not compatible.
        // This part is very similar to the prologue.
        // Just be careful to split into small transfers to fit the temporary buffer.

        if (size_aligned > __RT_HYPER_TEMP_BUFFER_SIZE - 4)
          size_aligned = __RT_HYPER_TEMP_BUFFER_SIZE - 4;

        if (!do_memcpy)
        {
          __rt_hyper_copy_aligned(channel, (void *)__rt_hyper_temp_buffer, (void *)(hyper_addr & ~1), size_aligned+4, event, mbr);

          if (async) {
            copy->u.hyper.memcpy = 1;
            goto end;
          }

          __rt_wait_event(event);
        }

        memcpy((void *)addr, &__rt_hyper_temp_buffer[1], size_aligned);
    
        copy->u.hyper.pending_hyper_addr += size_aligned;
        copy->u.hyper.pending_addr += size_aligned;
        copy->u.hyper.pending_size -= size_aligned;

        if (async && copy->u.hyper.pending_size) goto start;
      }
    }

    // Now check if we are done
    if (copy->u.hyper.pending_size == 0)
    {
      // Check if we are doing a 2D transfer
      if (copy->u.hyper.size_2d > 0)
      {
        // In this case, update the global size
        copy->u.hyper.size_2d -= copy->u.hyper.length;

        // And check if we must reenqueue a line.
        if (copy->u.hyper.size_2d > 0)
        {
          copy->u.hyper.pending_hyper_addr = copy->u.hyper.pending_hyper_addr - copy->u.hyper.length + copy->u.hyper.stride;
          copy->u.hyper.pending_size = copy->u.hyper.size_2d > copy->u.hyper.length ? copy->u.hyper.length : copy->u.hyper.size_2d;
          if (!async) event = __rt_wait_event_prepare_blocking();
          goto start;
        }
      }

      // Special case where it is asynchronous
      if (async)
      {
        // First enqueue the user event.
        // We need to restore it as we reused the user event for the internal copies.
        __rt_event_restore(event);
        __rt_event_enqueue(event);

        // In case there are other misaligned copies waiting for the temporary
        // buffer, wakeup one of them.
        if (__rt_hyper_first_waiting_misaligned)
        {
          event = __rt_hyper_first_waiting_misaligned;
          if (event)
          {
            __rt_hyper_first_waiting_misaligned = event->next;
            __rt_event_enqueue(event);
          }
        }
       else
        {
          __rt_hyper_pending_misaligned = 0;
        }
      }
    }
  }

end:
  rt_irq_restore(irq);
}



// Performs a misaligned 2d write without any constraint.
// This function can be either called directly or as an event callback
// This function is like a state machine, 
// it checks the state of the pending copy and does one more step
// so that the whole transfer can be done asynchronously without blocking
// the core.
static void __rt_hyper_resume_misaligned_write(void *arg)
{
  int irq = rt_irq_disable();

start:
  {
    rt_event_t *event = (rt_event_t *)arg;
    rt_periph_copy_t *copy = &event->copy;

    // Extract the current state of the transfer to see how to continue it
    int addr = copy->u.hyper.pending_addr;
    int hyper_addr = copy->u.hyper.pending_hyper_addr;
    int channel = copy->u.hyper.channel;
    int mbr = copy->u.hyper.mbr;
    int size = copy->u.hyper.pending_size;
    int async = copy->u.hyper.async;
    int do_memcpy = copy->u.hyper.memcpy;
    copy->u.hyper.memcpy = 0;

    // Compute information to see how to do one more step
    int addr_aligned = (addr + 3) & ~0x3;
    int prologue_size = addr_aligned - (int)addr;
    int hyper_addr_aligned = hyper_addr + prologue_size;

    if (size < 4)
      prologue_size = size;

    // In case the transfer is asynchronous, prepare the callback so that
    // we are called again when the next step is over
    if (async)
    {
      __rt_init_event(event, event->sched, __rt_hyper_resume_misaligned_write, event);
      // Be careful to not free the event, it is reused to finish the transfer
      __rt_event_set_pending(event);
    }

    if (prologue_size)
    {
      // Case where we have a partial copy to do
      if (!do_memcpy)
      {
        // A partial transfer must first transfer the content of the hyperram
        // to the temporary area and partially overwrite it with a memcpy.
        // This part is first called to trigger the transfer while the part after
        // is called to do the memcpy and the final transfer as a second step.
        __rt_hyper_copy_aligned(channel-1, (void *)__rt_hyper_temp_buffer, (void *)(hyper_addr & ~1), 4, event, mbr);

        // In case it is asynchronous, just remember we have to do
        // a memcpy when the transfer is done and leave
        if (async) {
          copy->u.hyper.memcpy = 1;
          goto end;
        }

        // Otherwise finish the transfer now
        __rt_wait_event(event);
        event = __rt_wait_event_prepare_blocking();
      }

      memcpy(&__rt_hyper_temp_buffer[hyper_addr & 1], (void *)addr, prologue_size);

      __rt_hyper_copy_aligned(channel, (void *)__rt_hyper_temp_buffer, (void *)(hyper_addr & ~1), 4, event, mbr);

      copy->u.hyper.pending_hyper_addr += prologue_size;
      copy->u.hyper.pending_addr += prologue_size;
      copy->u.hyper.pending_size -= prologue_size;

      if (async) goto end;

      // Otherwise finish the transfer now
      __rt_wait_event(event);
    }
    else if (size > 0)
    {
      // Case where we have the body to transfer
      int size_aligned = size & ~0x3;

      if ((hyper_addr_aligned & 0x1) == 0)
      {
        // Good case where the body is aligned on both sides and we can do
        // a direct copy.
        __rt_hyper_copy_aligned(channel, (void *)addr, (void *)hyper_addr, size_aligned, event, mbr);

        copy->u.hyper.pending_hyper_addr += size_aligned;
        copy->u.hyper.pending_addr += size_aligned;
        copy->u.hyper.pending_size -= size_aligned;

        // In case it is asynchronous, just leave, we'll continue the transfer
        // when this one is over
        if (async) goto end;

        __rt_wait_event(event);
      }
      else
      {
        // Bad case where we have to transfer the body using a temporary
        // buffer as the aligments on both sides are not compatible.
        // This part is very similar to the prologue.
        // Just be careful to split into small transfers to fit the temporary buffer.

        if ((size_aligned & 1) == 0)
          size_aligned--;

        if (size_aligned > __RT_HYPER_TEMP_BUFFER_SIZE - 2)
          size_aligned = __RT_HYPER_TEMP_BUFFER_SIZE - 2;

        if (!do_memcpy)
        {
          __rt_hyper_copy_aligned(channel-1, (void *)__rt_hyper_temp_buffer, (void *)(hyper_addr & ~1), 4, event, mbr);

          if (async) {
            copy->u.hyper.memcpy = 1;
            goto end;
          }

          __rt_wait_event(event);
          event = __rt_wait_event_prepare_blocking();
        }

        memcpy(&__rt_hyper_temp_buffer[1], (void *)addr, size_aligned);
    
        __rt_hyper_copy_aligned(channel, (void *)__rt_hyper_temp_buffer, (void *)(hyper_addr & ~1), size_aligned+2, event, mbr);

        copy->u.hyper.pending_hyper_addr += size_aligned;
        copy->u.hyper.pending_addr += size_aligned;
        copy->u.hyper.pending_size -= size_aligned;

        if (async) goto end;

        __rt_wait_event(event);
      }
    }

    // Now check if we are done
    if (copy->u.hyper.pending_size == 0)
    {
      // Check if we are doing a 2D transfer
      if (copy->u.hyper.size_2d > 0)
      {
        // In this case, update the global size
        copy->u.hyper.size_2d -= copy->u.hyper.length;

        // And check if we must reenqueue a line.
        if (copy->u.hyper.size_2d > 0)
        {
          copy->u.hyper.pending_hyper_addr = copy->u.hyper.pending_hyper_addr - copy->u.hyper.length + copy->u.hyper.stride;
          copy->u.hyper.pending_size = copy->u.hyper.size_2d > copy->u.hyper.length ? copy->u.hyper.length : copy->u.hyper.size_2d;
          if (!async) event = __rt_wait_event_prepare_blocking();
          goto start;
        }
      }

      // Special case where it is asynchronous
      if (async)
      {
        // First enqueue the user event.
        // We need to restore it as we reused the user event for the internal copies.
        __rt_event_restore(event);
        __rt_event_enqueue(event);

        // In case there are other misaligned copies waiting for the temporary
        // buffer, wakeup one of them.
        if (__rt_hyper_first_waiting_misaligned)
        {
          event = __rt_hyper_first_waiting_misaligned;
          if (event)
          {
            __rt_hyper_first_waiting_misaligned = event->next;
            __rt_event_enqueue(event);
          }
        }
       else
        {
          __rt_hyper_pending_misaligned = 0;
        }
      }
    }
  }

end:
  rt_irq_restore(irq);
}



static void __attribute__((noinline)) __rt_hyper_copy_misaligned(int channel,
  void *addr, void *hyper_addr, int size, rt_event_t *event, int mbr, int async)
{
  // Misaligned copies are always handled as asynchronous copies to simplify.
  // Pack all information into the copy structure so that we can follow
  // the progress of the transfer
  rt_periph_copy_t *copy = &event->copy;
  copy->u.hyper.pending_addr = (int)addr;
  copy->u.hyper.pending_size = size;
  copy->u.hyper.pending_hyper_addr = (int)hyper_addr;
  copy->u.hyper.mbr = mbr;
  copy->u.hyper.channel = channel;
  copy->u.hyper.async = async;
  copy->u.hyper.memcpy = 0;

  if (async)
  {
    // If the call is asynchronous we will need to use the event to handle
    // the various steps of the transfer and so we need to save the user information
    __rt_event_save(event);

    // Be careful that only one misaligned copy can be done at the same time
    // as they share the temporary buffer.
    // If another is pushed, just enqueue it to a SW queue.
    if (!__rt_hyper_pending_misaligned)
    {
      __rt_hyper_pending_misaligned = 1;
      if (channel & 1)
        __rt_hyper_resume_misaligned_write((void *)event);
      else
        __rt_hyper_resume_misaligned_read((void *)event);
    }
    else
    {
      event->next = __rt_hyper_first_waiting_misaligned;
      __rt_hyper_first_waiting_misaligned = event;
      if (channel & 1)
        __rt_init_event(event, event->sched, __rt_hyper_resume_misaligned_write, event);
      else
        __rt_init_event(event, event->sched, __rt_hyper_resume_misaligned_read, event);
      __rt_event_set_pending(event);
    }
  }
  else
  {
    // In case it is synchronous, just loop until the transfer is finished
    while (1)
    {
      if (channel & 1)
        __rt_hyper_resume_misaligned_write((void *)event);
      else
        __rt_hyper_resume_misaligned_read((void *)event);
      if (copy->u.hyper.pending_size <= 0) break;

      event = __rt_wait_event_prepare_blocking();
    }
  }

}



void __rt_hyper_copy(int channel,
  void *addr, void *hyper_addr, int size, rt_event_t *event, int mbr)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  // Check if we are in the fast case where everything is correctly aligned.
  if (likely((((int)addr & 0x3) == 0) && (((int)hyper_addr) & 0x1) == 0 && (((int)size & 0x3) == 0)))
  {
    __rt_hyper_copy_aligned(channel, addr, hyper_addr, size, call_event, mbr);
    __rt_wait_event_check(event, call_event);
  }
  else
  {
    // Otherwise go through the slow misaligned case.
    rt_periph_copy_t *copy = &call_event->copy;
    copy->u.hyper.size_2d = 0;
    __rt_hyper_copy_misaligned(channel, addr, hyper_addr, size, call_event, mbr, event != NULL);
  }

  rt_irq_restore(irq);
}


void __rt_hyper_copy_2d(int channel,
  void *addr, void *hyper_addr, int size, short stride, short length, rt_event_t *event, int mbr)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_periph_copy_t *copy = &call_event->copy;
  copy->u.hyper.stride = stride;
  copy->u.hyper.length = length;
  copy->u.hyper.size_2d = size;

  __rt_hyper_copy_misaligned(channel, addr, hyper_addr, length, call_event, mbr, event != NULL);

  rt_irq_restore(irq);
}



void rt_hyperram_read(rt_hyperram_t *dev,
  void *addr, void *hyper_addr, int size, rt_event_t *event);

RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_hyper_init()
{
  __rt_hyper_temp_buffer = NULL;
  __rt_hyper_pending_misaligned = 0;
  __rt_hyper_first_waiting_misaligned = NULL;
}