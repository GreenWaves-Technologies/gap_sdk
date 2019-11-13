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

#include "bsp/transport.h"


int pi_transport_open(struct pi_device *device)
{
  struct pi_transport_conf *conf = (struct pi_transport_conf *)device->config;
  pi_transport_api_t *api = (pi_transport_api_t *)conf->api;
  device->api = (struct pi_device_api *)api;
  return api->open(device);
}



int pi_transport_connect(struct pi_device *device, void (*rcv_callback(void *arg, struct pi_transport_header)), void *arg)
{
  // TODO
  static int connection = PI_TRANSPORT_USER_FIRST_CHANNEL;

  return connection++;
}



int pi_transport_send_header_async(struct pi_device *device, struct pi_transport_header *header, int channel, size_t size, pi_task_t *task)
{
  header->channel = channel;
  header->packet_size = size;
  return pi_transport_send_async(device, header, sizeof(*header), task);
}



int pi_transport_send_header(struct pi_device *device, struct pi_transport_header *header, int channel, size_t size)
{
  pi_task_t task;
  if (pi_transport_send_header_async(device, header, channel, size, pi_task_block(&task)))
  	return -1;
  pi_task_wait_on(&task);
  return 0;
}



int pi_transport_send(struct pi_device *device, void *buffer, size_t size)
{
  pi_task_t task;
  if (pi_transport_send_async(device, buffer, size, pi_task_block(&task)))
  	return -1;
  pi_task_wait_on(&task);
  return 0;
}



int pi_transport_receive(struct pi_device *device, void *buffer, size_t size)
{
  pi_task_t task;
  if (pi_transport_receive_async(device, buffer, size, pi_task_block(&task)))
  	return -1;
  pi_task_wait_on(&task);
  return 0;
}
