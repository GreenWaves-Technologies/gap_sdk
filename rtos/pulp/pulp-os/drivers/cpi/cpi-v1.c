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

static rt_cpi_t __rt_cpi[ARCHI_UDMA_NB_CAM];





void pi_cpi_conf_init(struct pi_cpi_conf *conf)
{
  conf->itf = 0;
}



int pi_cpi_open(struct pi_device *device)
{
  int periph_id;
  int channel;
  struct pi_cpi_conf *conf = (struct pi_cpi_conf *)device->config;
  rt_cpi_t *cpi = &__rt_cpi[conf->itf];

  device->data = (void *)cpi;

  periph_id = ARCHI_UDMA_CAM_ID(conf->itf);
  channel = UDMA_EVENT_ID(periph_id);

  cpi->channel_id = periph_id;
  cpi->open_count++;
  cpi->base = ARCHI_UDMA_ADDR + UDMA_PERIPH_OFFSET(periph_id);

  if (cpi->open_count == 1)
  {
    // Activate routing of UDMA cpi soc events to FC to trigger interrupts
    soc_eu_fcEventMask_setEvent(channel);

    // Deactivate cpi clock-gating
    plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));

    // Redirect all UDMA cpi events to the standard callback
    __rt_udma_register_channel_callback(channel, __rt_udma_handle_copy, (void *)cpi);
  }



  return 0;
}



void pi_cpi_close(struct pi_device *device)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;

  int periph_id = cpi->channel_id;

  cpi->open_count--;

  if (cpi->open_count == 0)
  {
    // Deactivate event routing
    soc_eu_fcEventMask_clearEvent(UDMA_EVENT_ID(periph_id));

    // Reactivate clock-gating
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<periph_id));
  }
}



void pi_cpi_capture(struct pi_device *device, void *buffer, int32_t size)
{
  struct pi_task task;
  pi_cpi_capture_async(device, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
}



void pi_cpi_capture_async(struct pi_device *device, void *buffer, int32_t size, pi_task_t *task)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;
  int irq = rt_irq_disable();
  __rt_task_init(task);

  __rt_udma_copy_enqueue(task, UDMA_CHANNEL_ID(cpi->channel_id), &cpi->channel, (uint32_t)buffer, size, UDMA_CHANNEL_CFG_SIZE_16);
  rt_irq_restore(irq);
}



static void __attribute__((constructor)) __rt_cpi_init()
{
  for (int i=0; i<ARCHI_UDMA_NB_CAM; i++)
  {
    __rt_cpi[i].open_count = 0;
    __rt_udma_channel_init(UDMA_EVENT_ID(ARCHI_UDMA_CAM_ID(0) + i), &__rt_cpi[i].channel);
  }
}



#ifdef __ZEPHYR__

#include <zephyr.h>
#include <device.h>
#include <init.h>

static int cpi_init(struct device *device)
{
  ARG_UNUSED(device);

  __rt_cpi_init();

  return 0;
}

struct cpi_config {
};

struct cpi_data {
};

static const struct cpi_config cpi_cfg = {
};

static struct cpi_data cpi_data = {
};

DEVICE_INIT(cpi, "cpi", &cpi_init,
    &cpi_data, &cpi_cfg,
    PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif