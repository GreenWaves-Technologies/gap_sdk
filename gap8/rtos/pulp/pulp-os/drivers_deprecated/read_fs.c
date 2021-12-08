/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Copyright (C) 2018 GreenWaves Technologies
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"



// Called by global rt_error_str to display fs errors
//
char *__rt_fs_error_str(int error)
{
#ifdef __RT_USE_IO
  switch (rt_error_code(error))
  {
    case RT_FS_MOUNT_FLASH_ERROR: return "FS mount error: failed to open flash";
    case RT_FS_MOUNT_MEM_ERROR:   return "FS mount error: failed to allocate memory";
  }
#endif

  return NULL;
}



// Can be called from anywhere to report an error
// This will either:
//  - register the error in the scheduler if no event is given, as it is then a synchronous call
//  - or enqueue the error in the scheduler for delayed notification as it is an asynchronous call
//
static inline void __rt_fs_abort(rt_event_t *event, int error, void *object)
{
  int irq = rt_irq_disable();
  __rt_error_report(event, __rt_error(RT_ERROR_FS, error), object);
  rt_irq_restore(irq);
}

// Can be called to register an error for a synchronous call
static inline void __rt_fs_error(int error)
{
  __rt_error_register(__rt_error(RT_ERROR_FS, error));
}



// Default FS config init
//
void rt_fs_conf_init(rt_fs_conf_t *conf)
{
  conf->type = RT_FS_READ_ONLY;
  rt_flash_conf_init(&conf->flash_conf);
}


static void __rt_fs_free(rt_fs_t *fs)
{
  if (fs != NULL)
  {
    if (fs->fs_info) rt_free(RT_ALLOC_PERIPH, fs->fs_info, fs->fs_l2->fs_size);
    if (fs->flash) rt_flash_close(fs->flash, NULL);
    if (fs->fs_l2) rt_free(RT_ALLOC_PERIPH, fs->fs_l2, sizeof(rt_fs_l2_t));
    if (fs->cache) rt_free(RT_ALLOC_PERIPH, fs->cache, FS_READ_THRESHOLD_BLOCK_FULL);
  }
}


// This function can be called to do all the required asynchronous steps to mount a FS.
// This can execute in 2 ways:
//   - No event is given in which case each call is synchronous and the call
//     to this function will just do all steps in one shot
//   - An event is given in which case, the function will just execute one asynchronous
//     step and will continue with the next step once it is called again by the event
//     execution
static int __rt_fs_mount_step(void *arg)
{
  rt_fs_t *fs = (rt_fs_t *)arg;

  // Loop on all steps in case there synchronous steps
  while (fs->mount_step < 4)
  {
    // Each asynchronous step is given fs->step_event which is an intermediate event
    // to reenqueue this function and do everything asynchronously.
    // It can also be NULL to do everything synchronously
    switch (fs->mount_step)
    {
      case 0:
        // Open the flash
        fs->flash = rt_flash_open((char *)fs->dev_name, &fs->flash_conf, fs->step_event);
        if (fs->flash == NULL) {
          __rt_fs_abort(
            fs->pending_event, RT_FS_MOUNT_FLASH_ERROR, (void *)fs
          );
          goto error;
        }
        break;

      case 1:
        // Read the offset telling where is the file-system header
        rt_flash_read(fs->flash, &fs->fs_l2->fs_offset, 0, 8, fs->step_event);
        break;

      case 2:
        // Read the header size at the first header word
        rt_flash_read(fs->flash, &fs->fs_l2->fs_size, (void *)(int)fs->fs_l2->fs_offset, 8, fs->step_event);
        break;

      case 3: {
        // Allocate roon for the file-system header and read it
        int fs_size = ((fs->fs_l2->fs_size + 7) & ~7);
        int fs_offset = fs->fs_l2->fs_offset;
        fs->fs_info = rt_alloc(RT_ALLOC_PERIPH, fs_size);
        if (fs->fs_info == NULL) {
          __rt_fs_abort(fs->pending_event, RT_FS_MOUNT_MEM_ERROR, (void *)fs);
          goto error;
        }
        rt_flash_read(fs->flash, (void *)fs->fs_info, (void *)(fs_offset + 8), fs_size, fs->step_event);
        break;
      }
    }

    fs->mount_step++;

    // Only continue to execute other steps if no event is provided as it
    // is then synchonous
    if (fs->step_event != NULL) goto end;
  }

  // In case there was a user event specified, enqueue it now that all
  // steps are done to notify the user
  if (fs->step_event) rt_event_enqueue(fs->pending_event);

end:
  return 0;

error:
  return -1;
}

static void __rt_fs_mount_step_noe(void *arg)
{
    (void)__rt_fs_mount_step(arg);
}

void rt_fs_unmount(rt_fs_t *fs, rt_event_t *event)
{
  int irq = rt_irq_disable();

  __rt_fs_free(fs);
  if (event) __rt_event_enqueue(event);

  rt_irq_restore(irq);
}

rt_fs_t *rt_fs_mount(const char *dev_name, rt_fs_conf_t *conf, rt_event_t *event)
{

  // Note that this function does not need to disable interrupts early and does not
  // need to lock a mutex as this is a FS creation so no one can access this object
  // at the same time and we can also safely mount several time the same device
  // at the same time as this is protected by the functions called from here.
  // Moreover it is important to not block imterrupts as this function can last
  // a long time due to flash access in case of synchronous mode.

  rt_trace(RT_TRACE_DEV_CTRL, "[FS] Mounting file-system (device: %s)\n", dev_name);

  rt_fs_t *fs = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_fs_t));
  if (fs == NULL) goto error;

  // Initialize all fields where something needs to be closed in case of error
  fs->fs_l2 = NULL;
  fs->cache = NULL;
  fs->flash = NULL;
  fs->fs_info = NULL;

  if (conf)
    memcpy((void *)&fs->flash_conf, (void *)&conf->flash_conf, sizeof(conf->flash_conf));
  else
    rt_flash_conf_init(&fs->flash_conf);

  fs->fs_l2 = rt_alloc(RT_ALLOC_PERIPH, sizeof(rt_fs_l2_t));
  if (fs->fs_l2 == NULL) goto error;

  fs->cache = rt_alloc(RT_ALLOC_PERIPH, FS_READ_THRESHOLD_BLOCK_FULL);
  if (fs->cache == NULL) goto error;

  fs->mount_step = 0;
  fs->dev_name = dev_name;
  fs->fs_info = NULL;
  fs->pending_event = event;

  // If the user specified an event, we must do all intermediate steps
  // asynchronously with another event, otherwise just do everything
  // synchronously with no event
  if (event) {
    fs->step_event = __rt_init_event(&fs->event, rt_event_internal_sched(), __rt_fs_mount_step_noe, (void *)fs);
  } else {
    fs->step_event = NULL;
  }

  // This function will take care of either blocking the thread if we are in blocking mode
  // or will just execute it asynchronously
  if (__rt_fs_mount_step((void *)fs)) goto error;

  return fs;

error:
    __rt_fs_error(RT_FS_MOUNT_MEM_ERROR);
  __rt_fs_free(fs);
  return NULL;
}



rt_file_t *rt_fs_open(rt_fs_t *fs, const char *file_name, int flags, rt_event_t *event)
{
  // No need to mask interrupts, as the file-system is read-only
  // its structure cannot change

  rt_trace(RT_TRACE_FS, "[FS] Opening file (name: %s)\n", file_name);

  // Get information about the file system from the header
  unsigned int *fs_info = fs->fs_info;
  int nb_comps = *fs_info++;

  // Find the file in the file-system
  rt_fs_desc_t *desc = NULL;
  int i;
  for (i=0; i<nb_comps; i++) {
    desc = (rt_fs_desc_t *)fs_info;
    if (strcmp(desc->name, file_name) == 0) break;
    fs_info = (unsigned int *)((unsigned int)fs_info + sizeof(rt_fs_desc_t) + desc->path_size);
  }

  // Leave if the file is not found
  if (i == nb_comps) goto error;

  // Now allocate the file descriptor and fills it
  rt_file_t *file = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_file_t));
  if (file == NULL) goto error;

  file->offset = 0;
  file->size = desc->size;
  file->addr = desc->addr;
  file->fs = fs;

  return file;

error:
  return NULL;
}

void rt_fs_close(rt_file_t *file, rt_event_t *event)
{
  rt_trace(RT_TRACE_FS, "[FS] Closing file (file: %p)\n", file);
  rt_free(RT_ALLOC_FC_DATA, (void *)file, sizeof(rt_file_t));
}



// Reads a block from device, which must be 4-bytes aligned on both the address and the size
static int __rt_fs_read_block(rt_fs_t *fs, unsigned int addr, unsigned int buffer, int size, rt_event_t *event)
{
  rt_trace(RT_TRACE_FS, "[FS] Read block (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, size);

  rt_flash_read(fs->flash, (void *)buffer, (void *)addr, size, event);
  return size;
}

// Reads a block from cache, whose size is inferior to FS_READ_THRESHOLD
static int __rt_fs_read_from_cache(rt_file_t *file, unsigned int buffer, unsigned int addr, int size)
{
  rt_trace(RT_TRACE_FS, "[FS] Read from cache (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, size);

  memcpy((void *)buffer, &file->fs->cache[addr - file->fs->cache_addr], size);

  return size;
}

// Reads a block from cache, whose size is inferior to FS_READ_THRESHOLD,
// with no alignment constraint.
// If the data is not in the cache, it is loaded fron FS
// and then it copied from the cache to the buffer
static int __rt_fs_read_cached(rt_file_t *file, unsigned int buffer, unsigned int addr, unsigned int size, int *pending, rt_event_t *event)
{
  rt_trace(RT_TRACE_FS, "[FS] Read cached (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, size);

  if (size > FS_READ_THRESHOLD_BLOCK_FULL - (addr & 0x7)) size = FS_READ_THRESHOLD_BLOCK_FULL - (addr & 0x7);

  rt_fs_t *fs = file->fs;

  if (addr < fs->cache_addr || addr + size > fs->cache_addr + FS_READ_THRESHOLD_BLOCK_FULL) {
    fs->cache_addr = addr & ~0x7;
    __rt_fs_read_block(fs, fs->cache_addr, (int)fs->cache, FS_READ_THRESHOLD_BLOCK_FULL, event);
    *pending = 1;
    return 0;
  }

  return __rt_fs_read_from_cache(file, buffer, addr, size);
}

int __rt_fs_read(rt_file_t *file, unsigned int buffer, unsigned int addr, int size, int *pending, rt_event_t *event)
{
  rt_fs_t *fs = file->fs;

  rt_trace(RT_TRACE_FS, "[FS] Read through cache (addr: 0x%x, buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", addr, buffer, addr, size);

  // 2 case where we go through the cache:
  //   - Small accesses
  //   - FS address alignment is different from L2 alignment,
  //     there is now way to transfer it directly, it must go through the cache
  int use_cache = size <= FS_READ_THRESHOLD || (addr & 0x7) != (buffer & 0x7);
  if (use_cache) return __rt_fs_read_cached(file, buffer, addr, size, pending, event);

  // Cache hit
  if (size <= FS_READ_THRESHOLD_BLOCK_FULL &&
    addr >= fs->cache_addr &&
    addr + size < fs->cache_addr + FS_READ_THRESHOLD_BLOCK_FULL) {
    return __rt_fs_read_from_cache(file, buffer, addr, size);
  }

  // Now this is the case where we can transfer part of the buffer directly from the FS to the L2
  // First handle beginning of buffer in case it is not aligned
  int prefix_size = addr & 0x7;
  if (prefix_size) {
    prefix_size = 8 - prefix_size;
    rt_trace(RT_TRACE_FS, "[FS] Reading block prefix (buffer: 0x%x, addr: 0x%x, size: 0x%x)\n", buffer, addr, prefix_size);
    int read_size = __rt_fs_read_cached(file, buffer, addr, prefix_size, pending, event);
    if (*pending) return read_size;
    addr += prefix_size;
    buffer += prefix_size;
    size -= prefix_size;
  }

  // Then the block in the middle, drop the end to get an aligned size, the end will be
  // retrieved through the cache during the next call
  int block_size = size & ~0x7;
  __rt_fs_read_block(file->fs, addr, buffer, block_size, event);
  *pending = 1;

  return block_size;
}

int rt_fs_seek(rt_file_t *file, unsigned int offset)
{
  rt_trace(RT_TRACE_FS, "[FS] File seek (file: %p, offset: 0x%x)\n", file, offset);

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
static void __rt_fs_try_read(void *arg)
{
  rt_file_t *file = (rt_file_t *)arg;

  int pending = 0;
  rt_event_t *event = file->step_event;

  while (file->pending_size) {

    int size = __rt_fs_read(
      file, file->pending_buffer, file->pending_addr, file->pending_size, &pending,
      event
    );

    file->pending_addr += size;
    file->pending_buffer += size;
    file->pending_size -= size;

    if (pending && event) return;
  }

  // In case there was a user event specified, enqueue it now that all
  // steps are done to notify the user
  if (file->step_event) {
    __rt_event_restore(file->step_event);
    rt_event_enqueue(file->pending_event);
    __rt_mutex_unlock(&file->fs->mutex);
  }
}

int rt_fs_read(rt_file_t *file, void *buffer, size_t size, rt_event_t *event)
{
  // Lock the file-system instead of masking interrupts as we can return
  // from this function with a pending operation on the cache
  // which must prevent anyone from accessing the cache
  // This also indirectly lock the file, which is good as we keep some pending state
  // also in the file
  __rt_mutex_lock(&file->fs->mutex);

  int real_size = size;
  if (file->offset + size > file->size) {
    real_size = file->size - file->offset;
  }

  rt_trace(RT_TRACE_FS, "[FS] File read (file: %p, buffer: %p, size: 0x%xx, real_size: 0x%x, offset: 0x%x, addr: 0x%x)\n", file, buffer, (int)size, real_size, file->offset, file->addr + file->offset);

  // If the user specified an event, we must do all intermediate steps
  // asynchronously with another event, otherwise just do everything
  // synchronously with no event
  if (event) {
    __rt_event_save(event);
    file->step_event = __rt_init_event(event, rt_event_internal_sched(), __rt_fs_try_read, (void *)file);
    __rt_event_set_pending(event);
  } else {
    file->step_event = NULL;
  }

  // Store the read information into the file in case the read is kept pending
  // when we return
  file->pending_event = event;
  file->pending_buffer = (unsigned int)buffer;
  file->pending_size = real_size;
  file->pending_addr = file->addr + file->offset;

  file->offset += real_size;

  __rt_fs_try_read((void *)file);

  return real_size;
}

int rt_fs_direct_read(rt_file_t *file, void *buffer, size_t size, rt_event_t *event)
{
  // Mask interrupt to update file current position and get information
    int irq = rt_irq_disable();

  int real_size = size;
  unsigned int addr = file->addr + file->offset;
  if (file->offset + size > file->size) {
    real_size = file->size - file->offset;
  }
  file->offset += real_size;

  rt_irq_restore(irq);

  rt_flash_read(file->fs->flash, (void *)buffer, (void *)addr, real_size, event);

  return real_size;
}


#if defined(ARCHI_HAS_CLUSTER)

void __rt_fs_cluster_req_done(void *_req)
{
  rt_fs_req_t *req = (rt_fs_req_t *)_req;
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}

void __rt_fs_cluster_req(void *_req)
{
  rt_fs_req_t *req = (rt_fs_req_t *)_req;
  rt_event_t *event = &req->event;
  rt_file_t *file = req->file;
  __rt_init_event(event, rt_event_internal_sched(), __rt_fs_cluster_req_done, (void *)req);
  if (req->direct) {
    req->result = rt_fs_direct_read(file, req->buffer, req->size, event);
  } else {
    req->result = rt_fs_read(req->file, req->buffer, req->size, event);
  }
}

void __rt_fs_cluster_read(rt_file_t *file, void *buffer, size_t size, rt_fs_req_t *req, int direct)
{
  req->file = file;
  req->buffer = buffer;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->direct = direct;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_fs_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

void __rt_fs_cluster_seek_req(void *_req)
{
  rt_fs_req_t *req = (rt_fs_req_t *)_req;
  req->result = rt_fs_seek(req->file, req->offset);
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}

void __rt_fs_cluster_seek(rt_file_t *file, unsigned int offset, rt_fs_req_t *req)
{
  req->file = file;
  req->offset = offset;
  req->cid = rt_cluster_id();
  req->done = 0;

  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_fs_cluster_seek_req, (void *)req);
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

#endif
