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
}


int32_t pi_fs_mount(struct pi_device *device)
{
  struct pi_fs_conf *conf = (struct pi_fs_conf *)device->config;
  pi_fs_api_t *api;
  if (conf->type == PI_FS_READ_ONLY)
  {
  	api = &__pi_read_fs_api;
  }
  else
  {
  	api = &__pi_host_fs_api;
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
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->close(file);
}

int32_t pi_fs_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->read(file, buffer, size, task);
}

int32_t pi_fs_read(pi_fs_file_t *file, void *buffer, uint32_t size)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  pi_task_t task;
  int result = pi_fs_read_async(file, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_write_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->write(file, buffer, size, task);
}

int32_t pi_fs_write(pi_fs_file_t *file, void *buffer, uint32_t size)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  pi_task_t task;
  int result = pi_fs_write_async(file, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_direct_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *task)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->direct_read(file, buffer, size, task);
}

int32_t pi_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  pi_task_t task;
  int result = pi_fs_direct_read_async(file, buffer, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_copy_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_task_t *task)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->copy(file, index, buffer, size, ext2loc, task);
}

int32_t pi_fs_copy(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  pi_task_t task;
  int result = pi_fs_copy_async(file, index, buffer, size, ext2loc, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_copy_2d_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc, pi_task_t *task)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->copy_2d(file, index, buffer, size, stride, length, ext2loc, task);
}

int32_t pi_fs_copy_2d(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  pi_task_t task;
  int result = pi_fs_copy_2d_async(file, index, buffer, size, stride, length, ext2loc, pi_task_block(&task));
  pi_task_wait_on(&task);
  return result;
}

int32_t pi_fs_seek(pi_fs_file_t *file, unsigned int offset)
{
  pi_fs_api_t *api = (pi_fs_api_t *)(file->fs->api);
  return api->seek(file, offset);
}