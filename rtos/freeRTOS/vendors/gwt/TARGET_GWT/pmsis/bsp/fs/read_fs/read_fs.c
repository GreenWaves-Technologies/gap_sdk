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
#include "bsp/flash.h"
#include <string.h>

#define READ_FS_THRESHOLD            16
#define READ_FS_THRESHOLD_BLOCK      128
#define READ_FS_THRESHOLD_BLOCK_FULL (READ_FS_THRESHOLD_BLOCK + 8)


typedef struct {
  unsigned int addr;
  unsigned int size;
  unsigned int path_size;
  char name[];
} pi_fs_desc_t;

#if 0
// Called by global pi_error_str to display fs errors
//
char *__pi_fs_error_str(int error)
{
#ifdef __pi_USE_IO
  switch (pi_error_code(error))
  {
    case FS_MOUNT_FLASH_ERROR: return "FS mount error: failed to open flash";
    case FS_MOUNT_MEM_ERROR:   return "FS mount error: failed to allocate memory";
  }
#endif

  return NULL;
}



// Can be called from anywhere to report an error
// This will either:
//  - register the error in the scheduler if no event is given, as it is then a synchronous call
//  - or enqueue the error in the scheduler for delayed notification as it is an asynchronous call
//
static inline void __pi_fs_abort(pi_task *event, int error, void *object)
{
  //int irq = pi_irq_disable();
  __pi_error_report(event, __pi_error(pi_error_FS, error), object);
  //pi_irq_restore(irq);
}

// Can be called to register an error for a synchronous call
static inline void __pi_fs_error(int error)
{
  __pi_error_register(__pi_error(pi_error_FS, error));
}

#endif



static void __pi_fs_free(pi_fs_t *fs)
{
  if (fs != NULL)
  {
    if (fs->pi_fs_info) pmsis_l2_malloc_free(fs->pi_fs_info, fs->pi_fs_l2->pi_fs_size);
    if (fs->pi_fs_l2) pmsis_l2_malloc_free(fs->pi_fs_l2, sizeof(pi_fs_l2_t));
  }
}


// This function can be called to do all the required asynchronous steps to mount a FS.
// This can execute in 2 ways:
//   - No event is given in which case each call is synchronous and the call
//     to this function will just do all steps in one shot
//   - An event is given in which case, the function will just execute one asynchronous
//     step and will continue with the next step once it is called again by the event
//     execution
static void __pi_fs_mount_step(void *arg)
{
  pi_fs_t *fs = (pi_fs_t *)arg;

  // Each asynchronous step is given fs->step_event which is an intermediate event
  // to reenqueue this function and do everything asynchronously.
  // It can also be NULL to do everything synchronously
  switch (fs->mount_step)
  {
  case 1:
      // Read the offset telling where is the file-system header
      pi_flash_read_async(fs->flash, 0, &fs->pi_fs_l2->pi_fs_offset, 8, pi_task_callback(&fs->step_event, __pi_fs_mount_step, (void *)arg));
      break;

  case 2:
      // Read the header size at the first header word
      pi_flash_read_async(fs->flash, (int)fs->pi_fs_l2->pi_fs_offset, &fs->pi_fs_l2->pi_fs_size, 8, pi_task_callback(&fs->step_event, __pi_fs_mount_step, (void *)arg));
      break;

  case 3:
  {
      // Allocate roon for the file-system header and read it
      int pi_fs_size = ((fs->pi_fs_l2->pi_fs_size + 7) & ~7);
      int pi_fs_offset = fs->pi_fs_l2->pi_fs_offset;
      fs->pi_fs_info = pmsis_l2_malloc(pi_fs_size);
      if (fs->pi_fs_info == NULL)
      {
          //__pi_fs_abort(fs->pending_event, FS_MOUNT_MEM_ERROR, (void *)fs);
          goto error;
      }
      pi_flash_read_async(fs->flash, pi_fs_offset + 8, (void *)fs->pi_fs_info, pi_fs_size, pi_task_callback(&fs->step_event, __pi_fs_mount_step, (void *)arg));
  }
      break;

  case 4:
      fs->error = 0;
      pi_task_push(fs->pending_event);
  }

  fs->mount_step++;

  return;

error:
  fs->error = -1;
  pi_task_push(fs->pending_event);
}

static void __pi_read_fs_unmount(struct pi_device *device)
{
  //int irq = pi_irq_disable();

  pi_fs_t *fs = (pi_fs_t *)device->data;

  __pi_fs_free(fs);

  //pi_irq_restore(irq);
}


static int32_t __pi_read_fs_mount(struct pi_device *device)
{

  struct pi_fs_conf *conf = (struct pi_fs_conf *)device->config;

  // Note that this function does not need to disable interrupts early and does not
  // need to lock a mutex as this is a FS creation so no one can access this object
  // at the same time and we can also safely mount several time the same device
  // at the same time as this is protected by the functions called from here.
  // Moreover it is important to not block imterrupts as this function can last
  // a long time due to flash access in case of synchronous mode.

  //pi_trace(pi_trace_DEV_CTRL, "[FS] Mounting file-system (device: %s)\n", dev_name);

  pi_fs_t *fs = pmsis_l2_malloc(sizeof(pi_fs_t));
  if (fs == NULL) goto error;

  pi_task_t task;

  // Initialize all fields where something needs to be closed in case of error
  fs->pi_fs_l2 = NULL;
  fs->pi_fs_info = NULL;
  fs->flash = conf->flash;

  fs->pi_fs_l2 = pmsis_l2_malloc(sizeof(pi_fs_l2_t));
  if (fs->pi_fs_l2 == NULL) goto error;

  fs->mount_step = 1;
  fs->pi_fs_info = NULL;
  fs->pending_event = pi_task_block(&task);

  device->data = (void *)fs;

  // This function will take care of either blocking the thread if we are in blocking mode
  // or will just execute it asynchronously
  __pi_fs_mount_step((void *)fs);

  pi_task_wait_on(&task);

  if (fs->error)
    goto error;

  return 0;

error:
    //__pi_fs_error(FS_MOUNT_MEM_ERROR);
  __pi_fs_free(fs);
  return -1;
}



static pi_fs_file_t *__pi_read_fs_open(struct pi_device *device, const char *file_name, int flags)
{
  pi_fs_t *fs = (pi_fs_t *)device->data;

  // No need to mask interrupts, as the file-system is read-only
  // its structure cannot change

  //pi_trace(pi_trace_FS, "[FS] Opening file (name: %s)\n", file_name);

  // Get information about the file system from the header
  unsigned int *pi_fs_info = fs->pi_fs_info;
  int nb_comps = *pi_fs_info++;

  // Find the file in the file-system
  pi_fs_desc_t *desc = NULL;
  int i;
  for (i=0; i<nb_comps; i++) {
    desc = (pi_fs_desc_t *)pi_fs_info;
    if (strcmp(desc->name, file_name) == 0) break;
    pi_fs_info = (unsigned int *)((unsigned int)pi_fs_info + sizeof(pi_fs_desc_t) + desc->path_size);
  }

  // Leave if the file is not found
  if (i == nb_comps) goto error;

  // Now allocate the file descriptor and fills it
  pi_fs_file_t *file = pmsis_l2_malloc(sizeof(pi_fs_file_t));
  if (file == NULL) goto error;

  file->cache = pmsis_l2_malloc(READ_FS_THRESHOLD_BLOCK_FULL);
  if (file->cache == NULL) goto error1;

  file->offset = 0;
  file->size = desc->size;
  file->addr = desc->addr;
  file->fs = device;
  file->cache_addr = -1;

  return file;

error1:
  pmsis_l2_malloc_free(file, sizeof(pi_fs_file_t));
error:
  return NULL;
}

static void __pi_read_fs_close(pi_fs_file_t *file)
{
  //printf("[FS] Closing file (file: %p)\n", file);
  pmsis_l2_malloc_free(file->cache, READ_FS_THRESHOLD_BLOCK_FULL);
  pmsis_l2_malloc_free((void *)file, sizeof(pi_fs_file_t));
}



// Reads a block from device, which must be 4-bytes aligned on both the address and the size
static int __pi_fs_read_block(pi_fs_t *fs, unsigned int addr, unsigned int buffer, int size, pi_task_t *event)
{
  //printf("[FS] Read block (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, size);


  pi_flash_read_async(fs->flash, addr, (void *)buffer, size, event);
  return size;
}

// Reads a block from cache, whose size is inferior to READ_FS_THRESHOLD
static int __pi_fs_read_from_cache(pi_fs_file_t *file, unsigned int buffer, unsigned int addr, int size)
{
  //printf("[FS] Read from cache (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, size);

  pi_fs_t *fs = (pi_fs_t *)file->fs->data;

  memcpy((void *)buffer, &file->cache[addr - file->cache_addr], size);

  return size;

}

// Reads a block from cache, whose size is inferior to READ_FS_THRESHOLD,
// with no alignment constraint.
// If the data is not in the cache, it is loaded fron FS
// and then it copied from the cache to the buffer
static int __pi_fs_read_cached(pi_fs_file_t *file, unsigned int buffer, unsigned int addr, unsigned int size, int *pending, pi_task_t *event)
{
  //printf("[FS] Read cached (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, size);

  if (size > READ_FS_THRESHOLD_BLOCK_FULL - (addr & 0x7)) size = READ_FS_THRESHOLD_BLOCK_FULL - (addr & 0x7);

  pi_fs_t *fs = (pi_fs_t *)file->fs->data;

  if (addr < file->cache_addr || addr + size > file->cache_addr + READ_FS_THRESHOLD_BLOCK_FULL) {
    file->cache_addr = addr & ~0x7;
    __pi_fs_read_block(fs, file->cache_addr, (int)file->cache, READ_FS_THRESHOLD_BLOCK_FULL, event);
    *pending = 1;
    return 0;
  }

  return __pi_fs_read_from_cache(file, buffer, addr, size);
}

int __pi_fs_read(pi_fs_file_t *file, unsigned int buffer, unsigned int addr, int size, int *pending, pi_task_t *event)
{
  pi_fs_t *fs = (pi_fs_t *)file->fs->data;

  //printf("[FS] Read through cache (addr: 0x%x, buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", addr, buffer, addr, size);

  // 2 case where we go through the cache:
  //   - Small accesses
  //   - FS address alignment is different from L2 alignment,
  //     there is now way to transfer it directly, it must go through the cache
  int use_cache = size <= READ_FS_THRESHOLD || (addr & 0x7) != (buffer & 0x7);
  if (use_cache) return __pi_fs_read_cached(file, buffer, addr, size, pending, event);

  // Cache hit
  if (size <= READ_FS_THRESHOLD_BLOCK_FULL &&
    addr >= file->cache_addr &&
    addr + size < file->cache_addr + READ_FS_THRESHOLD_BLOCK_FULL) {
    return __pi_fs_read_from_cache(file, buffer, addr, size);
  }

  // Now this is the case where we can transfer part of the buffer directly from the FS to the L2
  // First handle beginning of buffer in case it is not aligned
  int prefix_size = addr & 0x7;
  if (prefix_size) {
    prefix_size = 4 - prefix_size;
    //printf("[FS] Reading block prefix (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, prefix_size);
    int read_size = __pi_fs_read_cached(file, buffer, addr, prefix_size, pending, event);
    if (*pending) return read_size;
    addr += prefix_size;
    buffer += prefix_size;
    size -= prefix_size;
  }

  // Then the block in the middle, drop the end to get an aligned size, the end will be
  // retrieved through the cache during the next call
  int block_size = size & ~0x7;
  __pi_fs_read_block(fs, addr, buffer, block_size, event);
  *pending = 1;

  return block_size;
}

static int32_t __pi_read_fs_write(pi_fs_file_t *arg, void *buffer, uint32_t size, pi_task_t *task)
{
  return 0;
}

static int32_t __pi_read_fs_seek(pi_fs_file_t *file, unsigned int offset)
{
  //printf("[FS] File seek (file: %p, offset: 0x%x)\n", file, offset);

  if (offset < file->size) {
    file->offset = offset;
    return 0;
  }
  return -1;
}

// This function can be called to do all the required asynchronous steps to mount a FS.
// This can execute in 2 ways:
//   - No event is given in which case each call is synchronous and the call
//     to this function will just do all steps in one shot
//   - An event is given in which case, the function will just execute one asynchronous
//     step and will continue with the next step once it is called again by the event
//     execution
static void __pi_fs_try_read(void *arg)
{
  pi_fs_file_t *file = (pi_fs_file_t *)arg;

  int pending = 0;

  if (file->pending_size == 0)
  {
    // In case there was a user event specified, enqueue it now that all
    // steps are done to notify the user
    pi_task_push(file->pending_event);
    return;
    //__pi_mutex_unlock(&file->fs->mutex);
  }

  int size = __pi_fs_read(
    file, file->pending_buffer, file->pending_addr, file->pending_size, &pending, pi_task_callback(&file->step_event, __pi_fs_try_read, (void *)file)
  );

  file->pending_addr += size;
  file->pending_buffer += size;
  file->pending_size -= size;

  if (!pending)
  {
    if (file->pending_size == 0)
    {
      // In case there was a user event specified, enqueue it now that all
      // steps are done to notify the user
      pi_task_push(file->pending_event);
    }
    else
    {
      pi_task_push(pi_task_callback(&file->step_event, __pi_fs_try_read, (void *)file));
    }
  }
}

static int32_t __pi_read_fs_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *event)
{
  // Lock the file-system instead of masking interrupts as we can return
  // from this function with a pending operation on the cache
  // which must prevent anyone from accessing the cache
  // This also indirectly lock the file, which is good as we keep some pending state
  // also in the file
  //__pi_mutex_lock(&file->fs->mutex);

  int real_size = size;
  if (file->offset + size > file->size) {
    real_size = file->size - file->offset;
  }

  //printf("[FS] File read (file: %p, buffer: %p, size: 0x%xx, real_size: 0x%x, offset: 0x%x, addr: 0x%x)\n", file, buffer, (int)size, real_size, file->offset, file->addr + file->offset);

  // Store the read information into the file in case the read is kept pending
  // when we return

  #if defined(__PULP_OS__)
  __rt_task_init(event);
  #endif
  file->pending_event = event;
  file->pending_buffer = (unsigned int)buffer;
  file->pending_size = real_size;
  file->pending_addr = file->addr + file->offset;

  file->offset += real_size;

  __pi_fs_try_read((void *)file);

  return real_size;
}

static int32_t __pi_read_fs_direct_read_async(pi_fs_file_t *file, void *buffer, uint32_t size, pi_task_t *event)
{
  pi_fs_t *fs = (pi_fs_t *)file->fs->data;
  // Mask interrupt to update file current position and get information
  //int irq = pi_irq_disable();

  int real_size = size;
  unsigned int addr = file->addr + file->offset;
  if (file->offset + size > file->size) {
    real_size = file->size - file->offset;
  }
  file->offset += real_size;

  //pi_irq_restore(irq);

  pi_flash_read_async(fs->flash, addr, (void *)buffer, real_size, event);

  return real_size;
}

static int32_t __pi_read_fs_copy_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, int32_t ext2loc, pi_task_t *task)
{
  pi_fs_t *fs = (pi_fs_t *)file->fs->data;
  return pi_flash_copy_async(fs->flash, file->addr + index, buffer, size, ext2loc, task);
}

static int32_t __pi_read_fs_copy_2d_async(pi_fs_file_t *file, uint32_t index, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int32_t ext2loc, pi_task_t *task)
{
  pi_fs_t *fs = (pi_fs_t *)file->fs->data;
  return pi_flash_copy_2d_async(fs->flash, file->addr + index, buffer, size, stride, length, ext2loc, task);
}

void __pi_cl_fs_req_done(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    #if defined(PMSIS_DRIVERS)
    cl_notify_task_done(&(req->done), req->cid);
    #else
    req->done = 1;
    __rt_cluster_notif_req_done(req->cid);
    #endif  /* PMSIS_DRIVERS */
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

    #if defined(__PULP_OS__)
    __rt_task_init_from_cluster(&req->task);
    #endif  /* __PULP_OS__ */
    pi_task_callback(&req->task, __pi_cl_fs_req, (void *) req);
    #if defined(PMSIS_DRIVERS)
    pi_cl_send_task_to_fc(&(req->task));
    #else
    __rt_cluster_push_fc_event(&req->task);
    #endif  /* PMSIS_DRIVERS */
}

void pi_cl_fs_direct_read(pi_fs_file_t *file, void *buffer, uint32_t size, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->buffer = buffer;
    req->size = size;
    req->cid = pi_cluster_id();
    req->done = 0;
    req->direct = 1;

    #if defined(__PULP_OS__)
    __rt_task_init_from_cluster(&req->task);
    #endif  /* __PULP_OS__ */
    pi_task_callback(&req->task, __pi_cl_fs_req, (void *) req);
    #if defined(PMSIS_DRIVERS)
    pi_cl_send_task_to_fc(&(req->task));
    #else
    __rt_cluster_push_fc_event(&req->task);
    #endif  /* PMSIS_DRIVERS */
}

void __pi_cl_fs_seek_req(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    req->result = pi_fs_seek(req->file, req->offset);
    #if defined(PMSIS_DRIVERS)
    cl_notify_task_done(&(req->done), req->cid);
    #else
    req->done = 1;
    __rt_cluster_notif_req_done(req->cid);
    #endif  /* PMSIS_DRIVERS */
}

void pi_cl_fs_seek(pi_fs_file_t *file, uint32_t offset, pi_cl_fs_req_t *req)
{
    req->file = file;
    req->offset = offset;
    req->cid = pi_cluster_id();
    req->done = 0;

    #if defined(__PULP_OS__)
    __rt_task_init_from_cluster(&req->task);
    #endif  /* __PULP_OS__ */
    pi_task_callback(&req->task, __pi_cl_fs_seek_req, (void *) req);
    #if defined(PMSIS_DRIVERS)
    pi_cl_send_task_to_fc(&(req->task));
    #else
    __rt_cluster_push_fc_event(&req->task);
    #endif  /* PMSIS_DRIVERS */
}

void __pi_cl_fs_copy_req_done(void *_req)
{
    pi_cl_fs_req_t *req = (pi_cl_fs_req_t *)_req;
    #if defined(PMSIS_DRIVERS)
    cl_notify_task_done(&(req->done), req->cid);
    #else
    req->done = 1;
    __rt_cluster_notif_req_done(req->cid);
    #endif  /* PMSIS_DRIVERS */
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
  #if defined(__PULP_OS__)
  __rt_task_init_from_cluster(&req->task);
  #endif  /* __PULP_OS__ */
  pi_task_callback(&req->task, __pi_cl_fs_copy_req, (void *) req);
  #if defined(PMSIS_DRIVERS)
  pi_cl_send_task_to_fc(&(req->task));
  #else
  __rt_cluster_push_fc_event(&req->task);
  #endif  /* PMSIS_DRIVERS */
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
  #if defined(__PULP_OS__)
  __rt_task_init_from_cluster(&req->task);
  #endif  /* __PULP_OS__ */
  pi_task_callback(&req->task, __pi_cl_fs_copy_req, (void *) req);
  #if defined(PMSIS_DRIVERS)
  pi_cl_send_task_to_fc(&(req->task));
  #else
  __rt_cluster_push_fc_event(&req->task);
  #endif  /* PMSIS_DRIVERS */
}

pi_fs_api_t __pi_read_fs_api = {
  .mount = __pi_read_fs_mount,
  .unmount = __pi_read_fs_unmount,
  .open = __pi_read_fs_open,
  .close = __pi_read_fs_close,
  .read = __pi_read_fs_read_async,
  .direct_read = __pi_read_fs_direct_read_async,
  .write = __pi_read_fs_write,
  .seek = __pi_read_fs_seek,
  .copy = __pi_read_fs_copy_async,
  .copy_2d = __pi_read_fs_copy_2d_async
};