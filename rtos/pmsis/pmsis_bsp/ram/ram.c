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

#include "bsp/ram.h"

int32_t pi_ram_open(struct pi_device *device)
{
    struct pi_ram_conf *conf = (struct pi_ram_conf *)device->config;
    pi_ram_api_t *api = (pi_ram_api_t *)conf->api;
    device->api = (struct pi_device_api *)api;
    return api->open(device);
}


void __pi_ram_conf_init(struct pi_ram_conf *conf)
{
}


static void __pi_ram_cluster_req_done(void *_req)
{
    pi_cl_ram_req_t *req = (pi_cl_ram_req_t *)_req;
    cl_notify_task_done(&(req->done), req->cid);
}

static void __pi_ram_cluster_req(void *_req)
{
    pi_cl_ram_req_t *req = (pi_cl_ram_req_t* )_req;

    if (req->is_2d)
  	pi_ram_copy_2d_async(req->device, req->pi_ram_addr, req->addr, req->size, req->stride, req->length, req->ext2loc, pi_task_callback(&req->event, __pi_ram_cluster_req_done, (void *)req));
    else
  	pi_ram_copy_async(req->device, req->pi_ram_addr, req->addr, req->size, req->ext2loc, pi_task_callback(&req->event, __pi_ram_cluster_req_done, (void *)req));
}


void pi_cl_ram_copy(struct pi_device *device,
                 uint32_t pi_ram_addr, void *addr, uint32_t size, int ext2loc, pi_cl_ram_req_t *req)
{
    req->device = device;
    req->addr = addr;
    req->pi_ram_addr = pi_ram_addr;
    req->size = size;
    req->cid = pi_cluster_id();
    req->done = 0;
    req->ext2loc = ext2loc;
    req->is_2d = 0;
    pi_task_callback(&req->event, __pi_ram_cluster_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->event));
}



void pi_cl_ram_copy_2d(struct pi_device *device,
                    uint32_t pi_ram_addr, void *addr, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_cl_ram_req_t *req)
{
    req->device = device;
    req->addr = addr;
    req->pi_ram_addr = pi_ram_addr;
    req->size = size;
    req->stride = stride;
    req->length = length;
    req->cid = pi_cluster_id();
    req->done = 0;
    req->ext2loc = ext2loc;
    req->is_2d = 1;
    pi_task_callback(&req->event, __pi_ram_cluster_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->event));
}


void __pi_ram_alloc_cluster_req(void *_req)
{
    pi_cl_ram_alloc_req_t *req = (pi_cl_ram_alloc_req_t *)_req;
    req->error = pi_ram_alloc(req->device, &req->result, req->size);
    cl_notify_task_done(&(req->done), req->cid);
}



void __pi_ram_free_cluster_req(void *_req)
{
    pi_cl_ram_free_req_t *req = (pi_cl_ram_free_req_t *)_req;
    req->error = pi_ram_free(req->device, req->chunk, req->size);
    cl_notify_task_done(&(req->done), req->cid);
}


void pi_cl_ram_alloc(struct pi_device *device, uint32_t size, pi_cl_ram_alloc_req_t *req)
{
    req->device = device;
    req->size = size;
    req->cid = pi_cluster_id();
    req->done = 0;
    pi_task_callback(&req->event, __pi_ram_alloc_cluster_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->event));
}

void pi_cl_ram_free(struct pi_device *device, uint32_t chunk, uint32_t size, pi_cl_ram_free_req_t *req)
{
    req->device = device;
    req->size = size;
    req->chunk = chunk;
    req->cid = pi_cluster_id();
    req->done = 0;
    pi_task_callback(&req->event, __pi_ram_free_cluster_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->event));
}
