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
 */

#include "rt/rt_api.h"
#include <string.h>

void rt_flash_conf_init(rt_flash_conf_t *conf)
{
  conf->id = -1;
}

rt_flash_t *rt_flash_open(char *dev_name, rt_flash_conf_t *conf, rt_event_t *event)
{
  rt_flash_dev_t *desc;
  rt_dev_t *dev = NULL;

  if (dev_name)
  {
    dev = rt_dev_get(dev_name);
    if (dev == NULL) return NULL;

    desc = (rt_flash_dev_t *)dev->desc;
  }
  else
  {
    if (0)
    {
    }
#ifdef ARCHI_UDMA_HAS_SPIM
    else if (conf->type == RT_FLASH_TYPE_SPI)
    {
      desc = &spiflash_desc;
    }
#endif
#ifdef ARCHI_UDMA_HAS_HYPER
    else if (conf->type == RT_FLASH_TYPE_HYPER)
    {
      desc = &hyperflash_desc;
    }
#endif
#ifdef ARCHI_UDMA_HAS_MRAM
    else if (conf->type == RT_FLASH_TYPE_MRAM)
    {
      desc = &mram_desc;
    }
#endif
    else
      return NULL;
  }

  rt_flash_t *flash = desc->open(dev, conf, event);
  if (flash == NULL) return NULL;

  memcpy((void *)&flash->desc, (void *)desc, sizeof(rt_flash_dev_t));

  return flash;
}


void rt_flash_close(rt_flash_t *handle, rt_event_t *event)
{
  handle->desc.close(handle, event);
}

void __rt_flash_program(rt_flash_t *handle, void *data, void *addr, size_t size, rt_event_t *event)
{
  handle->desc.program(handle, data, addr, size, event);
}

void __rt_flash_erase(rt_flash_t *handle, void *addr, int size, rt_event_t *event)
{
  handle->desc.erase(handle, addr, size, event);
}

void __rt_flash_erase_chip(rt_flash_t *handle, rt_event_t *event)
{
  handle->desc.erase_chip(handle, event);
}

void __rt_flash_erase_sector(rt_flash_t *handle, void *data, rt_event_t *event)
{
  handle->desc.erase_sector(handle, data, event);
}

#if defined(ARCHI_HAS_CLUSTER)

void __rt_flash_cluster_req_done(void *_req)
{
  rt_flash_req_t *req = (rt_flash_req_t *)_req;
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}

void __rt_flash_cluster_req(void *_req)
{
  rt_flash_req_t *req = (rt_flash_req_t *)_req;
  rt_event_t *event = &req->event;
  __rt_init_event(event, rt_event_internal_sched(), __rt_flash_cluster_req_done, (void *)req);
  __rt_event_set_pending(event);
  rt_flash_read(req->dev, req->addr, req->flash_addr, req->size, event);
}

void rt_flash_cluster_read(rt_flash_t *dev, void *addr, void *flash_addr, size_t size, rt_flash_req_t *req)
{
  req->dev = dev;
  req->addr = addr;
  req->flash_addr = flash_addr;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->is_write = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_flash_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);   
}

#endif
