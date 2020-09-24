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


void __pi_cl_fs_req_exec(void *req);

void __pi_cl_fs_req_done(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_file_t *file = req->file;
    pi_fs_data_t *fs = req->file->fs_data;
    pi_task_t *task = &fs->cl_req_task;

    #if defined(__PULP_OS__)
    req->rw.result = task->implem.data[0];
    #else
    req->rw.result = task->data[0];
    #endif  /* __PULP_OS__ */
    cl_notify_task_done(&(req->rw.done), req->rw.cid);

    uint32_t irq = disable_irq();
    fs->cluster_reqs_first = (void *) req->callback.next;
    req = fs->cluster_reqs_first;
    restore_irq(irq);
    if (req)
    {
        __pi_cl_fs_req_exec(req);
    }
}

void __pi_cl_fs_req_exec(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_file_t *file = req->file;
    pi_fs_data_t *fs = req->file->fs_data;

    pi_task_callback(&(fs->cl_req_task), __pi_cl_fs_req_done, (void *)req);
    if (req->rw.write)
    {
        req->rw.result = pi_fs_write_async(file, req->rw.buffer, req->rw.size,
                                           &(fs->cl_req_task));
    }
    else
    {
        if (req->rw.direct)
        {
            req->rw.result = pi_fs_direct_read_async(file, req->rw.buffer,
                                                     req->rw.size, &(fs->cl_req_task));
        }
        else
        {
            req->rw.result = pi_fs_read_async(file, req->rw.buffer, req->rw.size,
                                              &(fs->cl_req_task));
        }
    }
}

void __pi_cl_fs_req(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_data_t *fs = req->file->fs_data;

    int is_first = fs->cluster_reqs_first == NULL;

    if (is_first)
    {
        fs->cluster_reqs_first = req;
    }
    else
    {
        fs->cluster_reqs_last->callback.next = (void *)req;
    }
    fs->cluster_reqs_last = req;
    req->callback.next = NULL;
    if (is_first)
    {
        pi_task_callback(&(fs->cl_req_task), __pi_cl_fs_req_exec, _req);
        pi_task_push(&(fs->cl_req_task));
    }
}

void pi_cl_fs_read(pi_fs_file_t *file, void *buffer, uint32_t size, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->rw.buffer = buffer;
    req->rw.size = size;
    req->rw.cid = pi_cluster_id();
    req->rw.done = 0;
    req->rw.write = 0;
    req->rw.direct = 0;
    req->rw.result = -1;

    pi_callback_init(&(req->callback), __pi_cl_fs_req, (void *) req);
    pi_cl_send_callback_to_fc(&(req->callback));
}

void pi_cl_fs_write(pi_fs_file_t *file, void *buffer, uint32_t size,
                    pi_cl_fs_req_t *req)
{
    req->file = file;
    req->rw.buffer = buffer;
    req->rw.size = size;
    req->rw.cid = pi_cluster_id();
    req->rw.done = 0;
    req->rw.write = 1;
    req->rw.direct = 0;
    req->rw.result = -1;

    pi_callback_init(&(req->callback), __pi_cl_fs_req, (void *) req);
    pi_cl_send_callback_to_fc(&(req->callback));
}

void pi_cl_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size,
                          pi_cl_fs_req_t *req)
{
    req->file = file;
    req->rw.buffer = buffer;
    req->rw.size = size;
    req->rw.cid = pi_cluster_id();
    req->rw.done = 0;
    req->rw.write = 0;
    req->rw.direct = 1;
    req->rw.result = -1;

    pi_callback_init(&(req->callback), __pi_cl_fs_req, (void *) req);
    pi_cl_send_callback_to_fc(&(req->callback));
}


void __pi_cl_fs_seek_req_exec(void *_req)
{
    pi_cl_fs_req_t *req = _req;
    pi_fs_file_t *file = req->file;
    pi_fs_data_t *fs = req->file->fs_data;

    req->rw.result = pi_fs_seek(req->file, req->rw.offset);
    cl_notify_task_done(&(req->rw.done), req->rw.cid);

    uint32_t irq = disable_irq();
    fs->cluster_reqs_first = (void *) req->callback.next;
    req = fs->cluster_reqs_first;
    restore_irq(irq);
    if (req)
    {
        __pi_cl_fs_seek_req_exec(_req);
    }
}

void __pi_cl_fs_seek_req(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_data_t *fs = req->file->fs_data;

    int is_first = fs->cluster_reqs_first == NULL;

    if (is_first)
    {
        fs->cluster_reqs_first = req;
    }
    else
    {
        fs->cluster_reqs_last->callback.next = (void *) req;
    }
    fs->cluster_reqs_last = req;
    req->callback.next = NULL;

    if (is_first)
    {
        pi_task_callback(&(fs->cl_req_task), __pi_cl_fs_seek_req_exec, _req);
        pi_task_push(&(fs->cl_req_task));
    }
}

void pi_cl_fs_seek(pi_fs_file_t *file, uint32_t offset, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->rw.offset = offset;
    req->rw.cid = pi_cluster_id();
    req->rw.done = 0;
    req->rw.result = -1;

    pi_callback_init(&(req->callback), __pi_cl_fs_seek_req, (void *) req);
    pi_cl_send_callback_to_fc(&(req->callback));
}


void __pi_cl_fs_copy_req_exec(void *_req);

void __pi_cl_fs_copy_req_done(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_file_t *file = req->file;
    pi_fs_data_t *fs = req->file->fs_data;
    pi_task_t *task = &fs->cl_req_task;

    cl_notify_task_done(&(req->copy.done), req->copy.cid);

    uint32_t irq = disable_irq();
    fs->cluster_reqs_first = (void *)req->callback.next;
    req = fs->cluster_reqs_first;
    restore_irq(irq);
    if (req)
    {
        __pi_cl_fs_copy_req_exec(req);
    }
}

void __pi_cl_fs_copy_req_exec(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_file_t *file = req->file;
    pi_fs_data_t *fs = req->file->fs_data;

    pi_task_callback(&(fs->cl_req_task), __pi_cl_fs_copy_req_done, (void *) req);
    if (req->copy.length)
    {
        req->copy.result = pi_fs_copy_2d_async(req->file, req->copy.index,
                                               req->copy.buffer, req->copy.size,
                                               req->copy.stride, req->copy.length,
                                               req->copy.ext2loc, &(fs->cl_req_task));
    }
    else
    {
        req->copy.result = pi_fs_copy_async(req->file, req->copy.index,
                                            req->copy.buffer, req->copy.size,
                                            req->copy.ext2loc, &(fs->cl_req_task));
    }
}

void __pi_cl_fs_copy_req(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    pi_fs_data_t *fs = req->file->fs_data;

    int is_first = fs->cluster_reqs_first == NULL;

    if (is_first)
    {
        fs->cluster_reqs_first = req;
    }
    else
    {
        fs->cluster_reqs_last->callback.next = (void *)req;
    }
    fs->cluster_reqs_last = req;
    req->callback.next = NULL;

    if (is_first)
    {
        pi_task_callback(&(fs->cl_req_task), __pi_cl_fs_copy_req_exec, _req);
        pi_task_push(&(fs->cl_req_task));
    }
}

void pi_cl_fs_copy(pi_fs_file_t *file, uint32_t index, void *buffer,
                   uint32_t size, int32_t ext2loc, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->copy.index = index;
    req->copy.buffer = buffer;
    req->copy.size = size;
    req->copy.ext2loc = ext2loc;
    req->copy.length = 0;
    req->copy.done = 0;
    req->copy.result = -1;
    req->copy.cid = pi_cluster_id();

    pi_callback_init(&(req->callback), __pi_cl_fs_copy_req, (void *) req);
    pi_cl_send_callback_to_fc(&(req->callback));
}

void pi_cl_fs_copy_2d(pi_fs_file_t *file, uint32_t index, void *buffer,
                      uint32_t size, uint32_t stride, uint32_t length,
                      int32_t ext2loc, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->copy.index = index;
    req->copy.buffer = buffer;
    req->copy.size = size;
    req->copy.stride = stride;
    req->copy.length = length;
    req->copy.ext2loc = ext2loc;
    req->copy.done = 0;
    req->copy.result = -1;
    req->copy.cid = pi_cluster_id();

    pi_callback_init(&(req->callback), __pi_cl_fs_copy_req, (void *) req);
    pi_cl_send_callback_to_fc(&(req->callback));
}
