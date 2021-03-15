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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __DEBUG_BRIDGE_DEBUG_BRIDGE_H__
#define __DEBUG_BRIDGE_DEBUG_BRIDGE_H__

#define PROTOCOL_VERSION_0 0    // Initial version
#define PROTOCOL_VERSION_1 1    // Added runtime / bridge state synchronization
#define PROTOCOL_VERSION_2 2    // Added bridge to runtime requests
#define PROTOCOL_VERSION_3 3    // Added field "connected" in target state to allow bridge to reconnect several times
#define PROTOCOL_VERSION_4 4    // Added field "bridge_to_target" in requests as they are now released by the target

#define HAL_PRINTF_BUF_SIZE 128

typedef enum {
  HAL_BRIDGE_REQ_CONNECT = 0,
  HAL_BRIDGE_REQ_DISCONNECT = 1,
  HAL_BRIDGE_REQ_OPEN = 2,
  HAL_BRIDGE_REQ_READ = 3,
  HAL_BRIDGE_REQ_WRITE = 4,
  HAL_BRIDGE_REQ_CLOSE = 5,
  HAL_BRIDGE_REQ_FB_OPEN = 6,
  HAL_BRIDGE_REQ_FB_UPDATE = 7,
  HAL_BRIDGE_REQ_TARGET_STATUS_SYNC = 8,
  HAL_BRIDGE_REQ_REPLY = 9,
  HAL_BRIDGE_TARGET_REQ_EFUSE_ACCESS = 10,
  HAL_BRIDGE_TARGET_REQ_EEPROM_ACCESS = 11,
  HAL_BRIDGE_TARGET_REQ_BUFFER_ALLOC = 12,
  HAL_BRIDGE_TARGET_REQ_BUFFER_FREE = 13,
  HAL_BRIDGE_TARGET_REQ_FLASH_ACCESS = 14,
  HAL_BRIDGE_TARGET_REQ_FLASH_ERASE_CHIP = 15,
  HAL_BRIDGE_TARGET_REQ_FLASH_ERASE_SECTOR = 16,
  HAL_BRIDGE_TARGET_REQ_FLASH_ERASE = 17,
  HAL_BRIDGE_REQ_FIRST_USER = 18
} hal_bridge_req_e;

typedef enum {
  HAL_BRIDGE_REQ_FB_FORMAT_GRAY = 1,
  HAL_BRIDGE_REQ_FB_FORMAT_RGB = 2,
  HAL_BRIDGE_REQ_FB_FORMAT_RAW = 3
} hal_bridge_fb_format_e;

typedef struct hal_bridge_req_s {
  uint64_t bridge_data;
  uint32_t next;
  uint32_t size;
  uint32_t type;
  uint32_t done;
  uint32_t popped;
  uint32_t bridge_to_target;
  union {
    struct {
      uint32_t name_len;
      uint32_t name;
      uint32_t flags;
      uint32_t mode;
      uint32_t retval;
    } open;
    struct {
      uint32_t file;
      uint32_t retval;
    } close;
    struct {
      uint32_t file;
      uint32_t ptr;
      uint32_t len;
      uint32_t retval;
    } read;
    struct {
      uint32_t file;
      uint32_t ptr;
      uint32_t len;
      uint32_t retval;
    } write;
    struct {
      uint64_t screen;
      uint32_t name_len;
      uint32_t name;
      uint32_t width;
      uint32_t height;
      uint32_t format;
    } fb_open;
    struct {
      uint64_t screen;
      uint32_t addr;
      uint32_t posx;
      uint32_t posy;
      uint32_t width;
      uint32_t height;
    } fb_update;
    struct {
      uint32_t is_write;
      uint32_t index;
      uint32_t value;
      uint32_t mask;
    } efuse_access;
    struct {
      uint8_t itf;
      uint8_t cs;
      uint8_t is_write;
      uint8_t padding;
      uint32_t addr;
      uint32_t buffer;
      uint32_t size;
      uint32_t retval;
    } eeprom_access;
    struct {
      uint8_t type;
      uint8_t itf;
      uint8_t cs;
      uint8_t is_write;
      uint32_t addr;
      uint32_t buffer;
      uint32_t size;
      uint32_t retval;
    } flash_access;
    struct {
      uint8_t type;
      uint8_t itf;
      uint8_t cs;
      uint8_t padding;
      uint32_t retval;
    } flash_erase_chip;
    struct {
      uint8_t type;
      uint8_t itf;
      uint8_t cs;
      uint8_t padding;
      uint32_t addr;
      uint32_t retval;
    } flash_erase_sector;
    struct {
      uint8_t type;
      uint8_t itf;
      uint8_t cs;
      uint8_t padding;
      uint32_t addr;
      uint32_t size;
      uint32_t retval;
    } flash_erase;
    struct {
      uint32_t size;
      uint32_t buffer;
    } buffer_alloc;
    struct {
      uint32_t size;
      uint32_t buffer;
    } buffer_free;
    struct {
    } target_status_sync;
  };
} hal_bridge_req_t;

typedef struct {
  volatile int32_t connected;
} __attribute__((packed)) hal_target_state_t;

typedef struct {
  // The bridge will set it to one when it finds the debug struct to notify
  // the target that the bridge is present.
  volatile int32_t connected;
} __attribute__((packed)) hal_bridge_state_t;

// This structure can be used to interact with the host loader
typedef struct {

  uint32_t protocol_version;

  hal_target_state_t target;

  hal_bridge_state_t bridge;

  // Used by external debug bridge to get exit status when using the board
  uint32_t exit_status;

  // Printf
  uint32_t use_internal_printf;
  uint32_t pending_putchar;
  uint32_t putc_current;
  uint8_t putc_buffer[HAL_PRINTF_BUF_SIZE];

  // Debug step, used for showing progress to host loader
  uint32_t debug_step;
  uint32_t debug_step_pending;

  // Requests
  uint32_t first_req;
  uint32_t last_req;
  uint32_t first_bridge_req;
  uint32_t first_bridge_free_req;
  uint32_t target_req;

  uint32_t notif_req_addr;
  uint32_t notif_req_value;

} __attribute__((packed)) hal_debug_struct_t;

#endif