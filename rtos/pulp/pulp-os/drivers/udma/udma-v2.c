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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"

RT_FC_TINY_DATA rt_udma_channel_t *__rt_udma_channels[ARCHI_NB_PERIPH*2];


#ifndef __RT_UDMA_COPY_ASM

void __rt_udma_handle_copy(int event, void *arg)
{
  rt_udma_channel_t *channel = __rt_udma_channels[event];
  pi_task_t *pending_1 = channel->pendings[1];
  pi_task_t *pending_0 = channel->pendings[0];
  pi_task_t *pending_first = channel->waitings_first;
  channel->pendings[0] = pending_1;

  if (pending_first)
  {
    // TODO
  }
  else
  {
    channel->pendings[1] = NULL;
  }

  __rt_event_handle_end_of_task(pending_0);
}

#else

extern void __rt_udma_handle_copy();

#endif


void __rt_udma_copy_enqueue(pi_task_t *task, int channel_id, rt_udma_channel_t *channel, uint32_t buffer, uint32_t size, uint32_t cfg)
{
  unsigned int base = hal_udma_channel_base(channel_id);

  // A UDMA channel has 2 slots, enqueue the copy to the UDMA if one of them is available, otherwise
  // put the transfer on hold.
  if (channel->pendings[0] == NULL)
  {
    channel->pendings[0] = task;
    plp_udma_enqueue(base, buffer, size, UDMA_CHANNEL_CFG_EN | cfg);
  }
  else if (channel->pendings[1] == NULL)
  {
    channel->pendings[1] = task;
    plp_udma_enqueue(base, buffer, size, UDMA_CHANNEL_CFG_EN | cfg);
  }
  else
  {
    task->implem.data[0] = buffer;
    task->implem.data[1] = size;
    task->implem.data[2] = cfg;

    if (channel->waitings_first == NULL)
      channel->waitings_first = task;
    else
      channel->waitings_last->implem.next = task;

    channel->waitings_last = task;
    task->implem.next = NULL;
  }
}



void __rt_udma_channel_init(int channel_id, rt_udma_channel_t *channel)
{
  channel->pendings[0] = NULL;
  channel->pendings[1] = NULL;
  channel->waitings_first = NULL;
  __rt_udma_channels[channel_id] = channel;
}


#ifndef __ZEPHYR__

void __rt_udma_channel_reg_data(int channel_id, void *data)
{
  __rt_udma_channels[channel_id] = data;
}

#endif