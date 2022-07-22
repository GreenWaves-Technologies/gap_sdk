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

#include "pmsis.h"
#include "bsp/ble.h"

int32_t pi_ble_open(struct pi_device *device)
{
    return device->api->open(device);
}

void pi_ble_close(struct pi_device *device)
{
    device->api->close(device);
}

int32_t pi_ble_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    return device->api->ioctl(device, (uint32_t) cmd, arg);
}

int32_t pi_ble_at_cmd(struct pi_device *device, const char *cmd, char *resp, uint32_t size)
{
    pi_ble_api_t *ble_api = (pi_ble_api_t *) device->api->specific_api;
    return ble_api->at_cmd(device, cmd, resp, size);
}

int32_t pi_ble_peer_connect(struct pi_device *device, const char *addr)
{
    pi_ble_api_t *ble_api = (pi_ble_api_t *) device->api->specific_api;
    return ble_api->peer_connect(device, addr);
}

int32_t pi_ble_peer_disconnect(struct pi_device *device, const char *addr)
{
    pi_ble_api_t *ble_api = (pi_ble_api_t *) device->api->specific_api;
    return ble_api->peer_disconnect(device, addr);
}

void pi_ble_data_send(struct pi_device *device, uint8_t *buffer, uint32_t size)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_ble_data_send_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_ble_data_send_async(struct pi_device *device, uint8_t *buffer,
                            uint32_t size, pi_task_t *task)
{
    device->api->write(device, 0, (const void *) buffer, size, task);
}

void pi_ble_data_get(struct pi_device *device, uint8_t *buffer, uint32_t size)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_ble_data_get_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_ble_data_get_async(struct pi_device *device, uint8_t *buffer,
                           uint32_t size, pi_task_t *task)
{
    device->api->read(device, 0, (void *) buffer, size, task);
}
