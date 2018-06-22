/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Copyright (C) 2018 GreenWaves Technologies
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

int __rt_hyperram_init(rt_hyperram_t *hyper)
{
  rt_extern_alloc_t *alloc = (rt_extern_alloc_t *)rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_extern_alloc_t));
  if (alloc == NULL) return -1;

  hyper->alloc = alloc;
  if (rt_extern_alloc_init(alloc, 0, hyper->dev->u.hyperram.size)) return -1;

  return 0;
}

void rt_hyperram_conf_init(rt_hyperram_conf_t *conf)
{

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

  rt_dev_t *dev = rt_dev_get(dev_name);
  if (dev == NULL) goto error;

  hyper = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_hyperram_t));
  if (hyper == NULL) goto error;

  hyper->dev = dev;
  hyper->alloc = NULL;
  hyper->channel = dev->channel;

  if (__rt_hyperram_init(hyper)) goto error;

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

void __rt_hyper_copy(int channel,
  void *addr, void *hyper_addr, int size, rt_event_t *event, int mbr)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;

  copy->ctrl = RT_PERIPH_COPY_HYPER << RT_PERIPH_COPY_CTRL_TYPE_BIT;
  copy->u.hyper.hyper_addr = mbr | (unsigned int)hyper_addr;
  if (size > 512) {
    copy->addr = (unsigned int)addr;
    copy->u.hyper.repeat = 512;
    copy->u.hyper.repeat_size = size;
    size = 512;
  } else {
    copy->u.hyper.repeat = 0;
  }

  rt_periph_copy(copy, channel, (unsigned int)addr, size, UDMA_CHANNEL_CFG_SIZE_16, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}
