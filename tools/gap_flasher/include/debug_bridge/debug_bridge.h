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
  HAL_BRIDGE_REQ_FIRST_USER = 8
} hal_bridge_req_e;

typedef enum {
  HAL_BRIDGE_REQ_FB_FORMAT_GRAY = 1
} hal_bridge_fb_format_e;

typedef struct hal_bridge_req_s {
  uint32_t next;
  uint32_t size;
  uint32_t type;
  uint32_t done;
  uint32_t popped;
  uint32_t padding;
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
  };
} hal_bridge_req_t;

// This structure can be used to interact with the host loader
typedef struct {

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

  uint32_t notif_req_addr;
  uint32_t notif_req_value;

  uint32_t bridge_connected;

} hal_debug_struct_t;

#endif