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

RT_L2_DATA static unsigned short __rt_hyperflash_set_reg_buffer[2];

static void __rt_hyperflash_read(rt_flash_t *_dev, void *data, void *addr, size_t size, rt_event_t *event);

void __rt_hyperflash_set_reg(rt_hyperflash_t *dev, unsigned int addr, unsigned short value)
{
  rt_event_t *event = rt_event_get_blocking(NULL);
  __rt_hyperflash_set_reg_buffer[0] = value;
  rt_hyperflash_copy(dev, UDMA_CHANNEL_ID(dev->channel) + 1, __rt_hyperflash_set_reg_buffer, (void *)(addr<<1), 2, event);
  rt_event_wait(event);
}

static void __rt_hyperflash_set_reg_noshift(rt_hyperflash_t *dev, unsigned int addr, unsigned short value)
{
  rt_event_t *event = rt_event_get_blocking(NULL);
  __rt_hyperflash_set_reg_buffer[0] = value;
  rt_hyperflash_copy(dev, UDMA_CHANNEL_ID(dev->channel) + 1, __rt_hyperflash_set_reg_buffer, (void *)(addr), 2, event);
  rt_event_wait(event);
}

static unsigned int __rt_hyperflash_get_status_reg(rt_hyperflash_t *dev)
{
  __rt_hyperflash_set_reg(dev, 0x555, 0x70);
  __rt_hyperflash_read((rt_flash_t *)dev, __rt_hyperflash_set_reg_buffer, 0, 4, NULL);
  return __rt_hyperflash_set_reg_buffer[0];
}

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

  if (dev)
  {
    hyper->channel = dev->channel;
  }
  else
  {
    hyper->channel = ARCHI_UDMA_HYPER_ID(conf->id);
  }

  struct pi_hyper_conf hyper_conf;

  pi_hyper_conf_init(&hyper_conf);

  hyper_conf.id = hyper->channel - ARCHI_UDMA_HYPER_ID(0);

  pi_open_from_conf(&hyper->device, &hyper_conf);

  if (pi_hyper_open(&hyper->device))
    return NULL;

  // HyperFlash
#if !defined(ARCHI_UDMA_HYPER_VERSION) || ARCHI_UDMA_HYPER_VERSION == 1
  hal_hyper_udma_dt1_set(0);
#endif

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

// TODO this only works if event is NULL. This should be implemented with a state
// machine as for hyperram accesses
static void __rt_hyperflash_program(rt_flash_t *_dev, void *data, void *addr, size_t size, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)_dev;

  unsigned int hyper_addr = (unsigned int)addr;
  unsigned int buffer = (unsigned int)data;

  while (size > 0)
  {
    // Hyperflash burst can go up to 512 bytes and should not cross a 512 bytes
    // boundary
    unsigned int iter_size = 512 - (hyper_addr & 0x1ff);
    if (iter_size > size)
      iter_size = size;

    __rt_hyperflash_set_reg(dev, 0x555, 0xAA);
    __rt_hyperflash_set_reg(dev, 0x2AA, 0x55);
    __rt_hyperflash_set_reg(dev, 0x555, 0xA0);

    int irq = rt_irq_disable();

    rt_event_t *call_event = __rt_wait_event_prepare(event);

    rt_hyperflash_copy(dev, 1, (void *)buffer, (void *)hyper_addr, iter_size, call_event);

  __rt_wait_event_check(event, call_event);

    rt_irq_restore(irq);

    while(((__rt_hyperflash_get_status_reg(dev) >> 7) & 1) == 0)
    {
      rt_time_wait_us(100);
    }

    size -= iter_size;
    hyper_addr += iter_size;
    buffer += iter_size;
  }
}

static void __rt_hyperflash_erase_chip(rt_flash_t *_dev, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)_dev;

  __rt_hyperflash_set_reg(dev, 0x555, 0xAA);
  __rt_hyperflash_set_reg(dev, 0x2AA, 0x55);
  __rt_hyperflash_set_reg(dev, 0x555, 0x80);
  __rt_hyperflash_set_reg(dev, 0x555, 0xAA);
  __rt_hyperflash_set_reg(dev, 0x2AA, 0x55);
  __rt_hyperflash_set_reg(dev, 0x555, 0x10);

  while(((__rt_hyperflash_get_status_reg(dev) >> 7) & 1) == 0)
  {
    rt_time_wait_us(1000);
  }
}

static void __rt_hyperflash_erase_sector(rt_flash_t *_dev, void *data, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)_dev;

  __rt_hyperflash_set_reg(dev, 0x555, 0xAA);
  __rt_hyperflash_set_reg(dev, 0x2AA, 0x55);
  __rt_hyperflash_set_reg(dev, 0x555, 0x80);
  __rt_hyperflash_set_reg(dev, 0x555, 0xAA);
  __rt_hyperflash_set_reg(dev, 0x2AA, 0x55);
  __rt_hyperflash_set_reg_noshift(dev, (int)data, 0x30);

  while(((__rt_hyperflash_get_status_reg(dev) >> 7) & 1) == 0)
  {
    rt_time_wait_us(1000);
  }
}

static void __rt_hyperflash_erase(rt_flash_t *_dev, void *data, int size, rt_event_t *event)
{
  __rt_hyperflash_erase_sector(_dev, data, event);
}

rt_flash_dev_t hyperflash_desc = {
  .open         = &__rt_hyperflash_open,
  .close        = &__rt_hyperflash_close,
  .read         = &__rt_hyperflash_read,
  .program      = &__rt_hyperflash_program,
  .erase_chip   = &__rt_hyperflash_erase_chip,
  .erase_sector = &__rt_hyperflash_erase_sector,
  .erase        = &__rt_hyperflash_erase
};
