/*
 * Copyright (C) 2019 GreenWaves Technologies
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
#include "bsp/display.h"


int pi_display_open(struct pi_device *device)
{
  struct pi_display_conf *conf = (struct pi_display_conf *)device->config;
  pi_display_api_t *api = (pi_display_api_t *)conf->api;
  device->api = (struct pi_device_api *)api;
  return api->open(device);
}



void pi_display_write(struct pi_device *device, pi_buffer_t *buffer, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  pi_task_t task;
  pi_task_block(&task);
  pi_display_write_async(device, buffer, x, y, w, h, &task);
  pi_task_wait_on(&task);
  pi_task_destroy(&task);
}



void __display_conf_init(struct pi_display_conf *conf)
{
}
