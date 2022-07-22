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

#include "bsp/wifi.h"


int32_t pi_wifi_open(struct pi_device *device)
{
    return device->api->open(device);
}

void pi_wifi_close(struct pi_device *device)
{
    device->api->close(device);
}

int32_t pi_wifi_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    return device->api->ioctl(device, (uint32_t) cmd, arg);
}

int32_t pi_wifi_at_cmd(struct pi_device *device, char *cmd, char *resp)
{
    pi_wifi_api_t *wifi_api = (pi_wifi_api_t *) device->api->specific_api;
    return wifi_api->at_cmd(device, cmd, resp);
}

int8_t pi_wifi_connect_to_ap(struct pi_device *device, const char *ssid, const char *pwd, pi_task_t *callback)
{
    pi_wifi_api_t *wifi_api = (pi_wifi_api_t *) device->api->specific_api;
    return wifi_api->connect_to_ap(device, ssid, pwd, callback);
}

int8_t pi_wifi_disconnect_from_ap(struct pi_device *device)
{
    pi_wifi_api_t *wifi_api = (pi_wifi_api_t *) device->api->specific_api;
    return wifi_api->disconnect_from_ap(device);
}

void pi_wifi_data_send(struct pi_device *device, char *buffer, uint32_t size)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_wifi_data_send_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_wifi_data_send_async(struct pi_device *device, char *buffer,
                            uint32_t size, pi_task_t *task)
{
    device->api->write(device, 0, (const void *) buffer, size, task);
}

void pi_wifi_data_get(struct pi_device *device, char *buffer, uint32_t size)
{
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    pi_wifi_data_get_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_wifi_data_get_async(struct pi_device *device, char *buffer,
                           uint32_t size, pi_task_t *task)
{
    device->api->read(device, 0, (void *) buffer, size, task);
}

int8_t pi_wifi_create_tcp_server(struct pi_device *device, uint16_t port)
{
    pi_wifi_api_t *wifi_api = (pi_wifi_api_t *) device->api->specific_api;
    return wifi_api->create_tcp_server(device, port);
}

int8_t pi_wifi_wait_client_connection(struct pi_device *device, char *resp, pi_task_t *callback)
{
    pi_wifi_api_t *wifi_api = (pi_wifi_api_t *) device->api->specific_api;
    return wifi_api->wait_client_connection(device, resp, callback);
}

int8_t pi_wifi_wait_client_event(struct pi_device *device, char *resp, pi_task_t *callback)
{
    pi_wifi_api_t *wifi_api = (pi_wifi_api_t *) device->api->specific_api;
    return wifi_api->wait_client_event(device, resp, callback);
}
