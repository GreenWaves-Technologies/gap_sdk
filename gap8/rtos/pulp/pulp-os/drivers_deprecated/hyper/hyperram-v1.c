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


void rt_hyperram_conf_init(rt_hyperram_conf_t *conf)
{
  pi_hyper_conf_init((struct pi_hyper_conf *)conf); 
}


rt_hyperram_t *rt_hyperram_open(char *dev_name, rt_hyperram_conf_t *conf, rt_event_t *event)
{
  rt_hyperram_conf_t default_conf;
  struct pi_device *device = NULL;
  struct pi_hyper_conf pi_conf;

  if (conf == NULL)
    conf = &default_conf;

  if (dev_name)
  {
    rt_dev_t *dev = rt_dev_get(dev_name);
    if (dev == NULL) goto error;
    conf->id = dev->channel;
    conf->ram_size = dev->u.hyperram.size;
  }

  device = rt_alloc(RT_ALLOC_FC_DATA, sizeof(struct pi_device));
  if (device == NULL)
    return NULL;

  pi_hyper_conf_init(&pi_conf);
  pi_conf.id = conf->id;

  pi_open_from_conf(device, &pi_conf);

  if (pi_hyper_open(device))
    goto error;

  return (rt_hyperram_t *)device;

error:
if (device != NULL)
    rt_free(RT_ALLOC_FC_DATA, device, sizeof(struct pi_device));
  return NULL;
}



void rt_hyperram_close(rt_hyperram_t *handle, rt_event_t *event)
{
  pi_hyper_close((struct pi_device *)handle);
  rt_free(RT_ALLOC_FC_DATA, handle, sizeof(struct pi_device));
}
