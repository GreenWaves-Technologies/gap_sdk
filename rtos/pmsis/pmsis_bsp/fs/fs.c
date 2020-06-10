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
#include "bsp/fs.h"

// Default FS config init
//
// Default FS config init
//
void pi_fs_conf_init(struct pi_fs_conf *conf)
{
  conf->type = PI_FS_READ_ONLY;
  
  // By default, mount operation uses the first partition compatible with the FS.
  conf->partition_name = NULL;
  
  // By default, an error is returned to the user if the file system is not found in the partition.
  conf->auto_format = false;

  conf->api = NULL;
}


int32_t pi_fs_mount(struct pi_device *device)
{
  struct pi_fs_conf *conf = (struct pi_fs_conf *)device->config;
  pi_fs_api_t *api = conf->api;

  if (api == NULL)
  {
    switch (conf->type)
    {
      case PI_FS_READ_ONLY:
        api = &__pi_read_fs_api;
        break;

      case PI_FS_HOST:
        api = &__pi_host_fs_api;
        break;

      default:
        return -1;
    }
  }

  device->api = (struct pi_device_api *)api;

  return api->mount(device);
}


void pi_fs_unmount(struct pi_device *device)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(device->api);
  api->unmount(device);
}

pi_fs_file_t *pi_fs_open(struct pi_device *device, const char *file_name, int flags)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(device->api);
  return api->open(device, file_name, flags);
}

void pi_fs_close(pi_fs_file_t *file)
{
  return file->api->close(file);
}

int32_t pi_fs_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
  return file->api->read(file, buffer, size, task);
}

int32_t pi_fs_read(pi_fs_file_t *file, void *buffer, uint32_t size)
{
  pi_task_t task;
  int result = pi_fs_read_async(file, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_write_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
  return file->api->write(file, buffer, size, task);
}

int32_t pi_fs_write(pi_fs_file_t *file, void *buffer, uint32_t size)
{
  pi_task_t task;
  int result = pi_fs_write_async(file, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_direct_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
  return file->api->direct_read(file, buffer, size, task);
}

int32_t pi_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size)
{
  pi_task_t task;
  int result = pi_fs_direct_read_async(file, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_copy_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_task_t *task)
{
  return file->api->copy(file, index, buffer, size, ext2loc, task);
}

int32_t pi_fs_copy(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc)
{
  pi_task_t task;
  int result = pi_fs_copy_async(file, index, buffer, size, ext2loc, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_copy_2d_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc, pi_task_t *task)
{
  return file->api->copy_2d(file, index, buffer, size, stride, length, ext2loc, task);
}

int32_t pi_fs_copy_2d(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc)
{
  pi_task_t task;
  int result = pi_fs_copy_2d_async(file, index, buffer, size, stride, length, ext2loc, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_seek(pi_fs_file_t *file, unsigned int offset)
{
  return file->api->seek(file, offset);
}


void __pi_cl_fs_req_done(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    #if defined(__PULP_OS__)
    req->result = req->task.implem.data[0];
    #else
    req->result = req->task.data[0];
    #endif  /* __PULP_OS__ */
    cl_notify_task_done(&(req->done), req->cid);
}


void __pi_cl_fs_req(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_file_t *file = req->file;
    if (req->direct)
    {
        req->result = pi_fs_direct_read_async(file, req->buffer, req->size, pi_task_callback(&req->task, __pi_cl_fs_req_done, (void *)req));
    }
    else
    {
        req->result = pi_fs_read_async(req->file, req->buffer, req->size, pi_task_callback(&req->task, __pi_cl_fs_req_done, (void *)req));
    }
}


void pi_cl_fs_read(pi_fs_file_t *file, void *buffer, uint32_t size, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->buffer = buffer;
    req->size = size;
    req->cid = pi_cluster_id();
    req->done = 0;
    req->direct = 0;
    req->result = -1;

    pi_task_callback(&req->task, __pi_cl_fs_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->task));
}


void __pi_cl_fs_req_write(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_file_t *file = req->file;

    req->result = pi_fs_write_async(file, req->buffer, req->size, pi_task_callback(&req->task, __pi_cl_fs_req_done, (void *)req));
}


void pi_cl_fs_write(pi_fs_file_t *file, void *buffer, uint32_t size,
  pi_cl_fs_req_t *req)
{
    req->file = file;
    req->buffer = buffer;
    req->size = size;
    req->cid = pi_cluster_id();
    req->done = 0;
    req->direct = 0;

    pi_task_callback(&req->task, __pi_cl_fs_req_write, (void *) req);
    pi_cl_send_task_to_fc(&(req->task));
}


void pi_cl_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->buffer = buffer;
    req->size = size;
    req->cid = pi_cluster_id();
    req->done = 0;
    req->direct = 1;
    req->result = -1;

    pi_task_callback(&req->task, __pi_cl_fs_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->task));
}

void __pi_cl_fs_seek_req(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    req->result = pi_fs_seek(req->file, req->offset);
    cl_notify_task_done(&(req->done), req->cid);
}

void pi_cl_fs_seek(pi_fs_file_t *file, uint32_t offset, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->offset = offset;
    req->cid = pi_cluster_id();
    req->done = 0;

    pi_task_callback(&req->task, __pi_cl_fs_seek_req, (void *) req);
    pi_cl_send_task_to_fc(&(req->task));
}

void __pi_cl_fs_copy_req_done(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_fs_copy_req(void *_req)
{
  pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
  if (req->length)
    req->result = pi_fs_copy_2d_async(req->file, req->index, req->buffer, req->size, req->stride, req->length, req->ext2loc, pi_task_callback(&req->task, __pi_cl_fs_copy_req_done, (void *)req));
  else
    req->result = pi_fs_copy_async(req->file, req->index, req->buffer, req->size, req->ext2loc, pi_task_callback(&req->task, __pi_cl_fs_copy_req_done, (void *)req));

  if (req->result)
    __pi_cl_fs_copy_req_done(_req);
}


void pi_cl_fs_copy(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_cl_fs_req_t *req)
{
  req->file = file;
  req->index = index;
  req->buffer = buffer;
  req->size = size;
  req->ext2loc = ext2loc;
  req->length = 0;
  req->done = 0;
  req->cid = pi_cluster_id();
  pi_task_callback(&req->task, __pi_cl_fs_copy_req, (void *) req);
  pi_cl_send_task_to_fc(&(req->task));
}

void pi_cl_fs_copy_2d(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc, pi_cl_fs_req_t *req)
{
  req->file = file;
  req->index = index;
  req->buffer = buffer;
  req->size = size;
  req->stride = stride;
  req->length = length;
  req->ext2loc = ext2loc;
  req->done = 0;
  req->cid = pi_cluster_id();
  pi_task_callback(&req->task, __pi_cl_fs_copy_req, (void *) req);
  pi_cl_send_task_to_fc(&(req->task));
}
