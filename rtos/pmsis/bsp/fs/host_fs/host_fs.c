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
#include "bsp/fs/hostfs.h"
#include <string.h>
#include "semihost.h"

static pi_fs_data_t bsp_fs_data;

typedef struct {
  pi_fs_file_t header;
  int fd;
} pi_host_fs_file_t;

static int32_t __pi_host_fs_mount(struct pi_device *device)
{
  bsp_fs_data.cluster_reqs_first = NULL;
  return 0;
}

static void __pi_host_fs_unmount(struct pi_device *device)
{
}

static pi_fs_file_t *__pi_host_fs_open(struct pi_device *device, const char *file_name, int flags)
{
  pi_host_fs_file_t *file = pmsis_l2_malloc(sizeof(pi_host_fs_file_t));
  if (file == NULL) goto error;

  file->header.fs = device;

  // The possible values are specified in openocd in src/target/semihosting_common.c
  file->fd = semihost_open(file_name, flags == PI_FS_FLAGS_WRITE ? 6 : flags == PI_FS_FLAGS_APPEND ? 8 : 0);
  if (file->fd == -1)
    return NULL;

  file->header.api = (pi_fs_api_t *)device->api;
  file->header.data = file;
  file->header.fs = device;
  file->header.fs_data = &bsp_fs_data;

  return (pi_fs_file_t *)file;

error:
  return NULL;
}

static void __pi_host_fs_close(pi_fs_file_t *arg)
{
  pi_host_fs_file_t *file = (pi_host_fs_file_t *)arg;
  semihost_close(file->fd);
  pmsis_l2_malloc_free(file, sizeof(pi_host_fs_file_t));
}

static int32_t __pi_host_fs_read_async(pi_fs_file_t *arg, void *buffer, uint32_t size, pi_task_t *task)
{
  pi_host_fs_file_t *file = (pi_host_fs_file_t *)arg;
  int result = size - semihost_read(file->fd, buffer, size);
  #if defined(__PULP_OS__)
  task->implem.data[0] = result;
  #else
  task->data[0] = result;
  #endif  /* __PULP_OS__ */
  pi_task_push(task);
  return result;
}

static int32_t __pi_host_fs_direct_read_async(pi_fs_file_t *arg, void *buffer, uint32_t size, pi_task_t *task)
{
  pi_host_fs_file_t *file = (pi_host_fs_file_t *)arg;
  int result = size - semihost_read(file->fd, buffer, size);
  #if defined(__PULP_OS__)
  task->implem.data[0] = result;
  #else
  task->data[0] = result;
  #endif  /* __PULP_OS__ */
  pi_task_push(task);
  return result;
}

static int32_t __pi_host_fs_write_async(pi_fs_file_t *arg, void *buffer, uint32_t size, pi_task_t *task)
{
  pi_host_fs_file_t *file = (pi_host_fs_file_t *)arg;
  int result = size - semihost_write(file->fd, buffer, size);

  pi_task_push(task);
  return result;
}

static int32_t __pi_host_fs_seek(pi_fs_file_t *arg, unsigned int offset)
{
  pi_host_fs_file_t *file = (pi_host_fs_file_t *)arg;
  return semihost_seek(file->fd, offset);
}

static int32_t __pi_host_fs_copy_async(pi_fs_file_t *arg, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_task_t *task)
{
  if (__pi_host_fs_seek(arg, index))
    return -1;

  if (ext2loc)
    return __pi_host_fs_read_async(arg, buffer, size, task);
  else
    return __pi_host_fs_write_async(arg, buffer, size, task);
}

static int32_t __pi_host_fs_copy_2d_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc, pi_task_t *task)
{
  unsigned int chunk;
  for (chunk=0; chunk<size; chunk+=length)
  {
    if (length > size)
      length = size;

    if (__pi_host_fs_seek(file, index))
      goto error;

    if (ext2loc)
    {
      if (pi_fs_read(file, buffer, length) != (int)length)
        goto error;
    }
    else
    {
      if (pi_fs_write(file, buffer, length) != (int)length)
        goto error;
    }

    buffer = ((char *)buffer) + length;
    index += stride;
  }
  pi_task_push(task);
  return 0;

error:
  pi_task_push(task);
  return -1;
}

pi_fs_api_t __pi_host_fs_api = {
  .mount = __pi_host_fs_mount,
  .unmount = __pi_host_fs_unmount,
  .open = __pi_host_fs_open,
  .close = __pi_host_fs_close,
  .read = __pi_host_fs_read_async,
  .direct_read = __pi_host_fs_direct_read_async,
  .write = __pi_host_fs_write_async,
  .seek = __pi_host_fs_seek,
  .copy = __pi_host_fs_copy_async,
  .copy_2d = __pi_host_fs_copy_2d_async
};

void pi_hostfs_conf_init(struct pi_hostfs_conf *conf)
{
    pi_fs_conf_init(&conf->fs);
    conf->fs.type = PI_FS_HOST;
    conf->fs.api = &__pi_host_fs_api;
}
