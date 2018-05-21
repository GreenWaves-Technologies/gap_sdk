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

static void __rt_hyperflash_free(rt_hyperflash_t *hyper)
{
  if (hyper != NULL) {
    rt_free(RT_ALLOC_FC_DATA, (void *)hyper, sizeof(rt_hyperflash_t));
  }
}

static rt_flash_t *__rt_hyperflash_open(rt_dev_t *dev, rt_flash_conf_t *conf, rt_event_t *event)
{
  rt_hyperflash_t *hyper = NULL;

  hyper = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_hyperflash_t));
  if (hyper == NULL) goto error;

  hyper->header.dev = dev;
  hyper->channel = dev->channel;

  // HyperFlash
  hal_hyper_udma_dt1_set(0);

  if (event) __rt_event_enqueue(event);

  return (rt_flash_t *)hyper;

error:
  __rt_hyperflash_free(hyper);
  return NULL;
}

static void __rt_hyperflash_close(rt_flash_t *flash, rt_event_t *event)
{
  rt_hyperflash_t *hyper = (rt_hyperflash_t *)flash;
  __rt_hyperflash_free(hyper);
}

static void __rt_hyperflash_read(rt_flash_t *_dev, void *data, void *addr, size_t size, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_hyperflash_t *dev = (rt_hyperflash_t *)_dev;

  rt_hyperflash_copy(dev, 0, data, addr, size, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

rt_flash_dev_t hyperflash_desc = {
  .open      = &__rt_hyperflash_open,
  .close     = &__rt_hyperflash_close,
  .read      = &__rt_hyperflash_read
};
