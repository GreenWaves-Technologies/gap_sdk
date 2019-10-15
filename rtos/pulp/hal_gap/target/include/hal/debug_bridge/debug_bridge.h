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

#ifndef __HAL_DEBUG_BRIDGE_DEBUG_BRIDGE_H__
#define __HAL_DEBUG_BRIDGE_DEBUG_BRIDGE_H__

#include <stdint.h>
#include "debug_bridge/debug_bridge.h"

#define HAL_DEBUG_STRUCT_NAME __hal_debug_struct
#define HAL_DEBUG_STRUCT_NAME_STR "__hal_debug_struct"

typedef hal_debug_struct_t hal_bridge_t;

extern hal_debug_struct_t HAL_DEBUG_STRUCT_NAME;

static inline hal_debug_struct_t *hal_debug_struct_get()
{
  return &HAL_DEBUG_STRUCT_NAME;
}


static inline hal_bridge_t *hal_bridge_get()
{
  return hal_debug_struct_get();
}


#define HAL_DEBUG_STRUCT_INIT { PROTOCOL_VERSION_4, {0}, {0}, 0, 1, 0 ,0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0}


static inline int hal_bridge_is_connected(hal_bridge_t *bridge) {
  return *(volatile uint32_t *)&bridge->bridge.connected;
}

static inline void hal_bridge_connect(hal_bridge_req_t *req)
{
  req->type = HAL_BRIDGE_REQ_CONNECT;
}

static inline void hal_bridge_reply(hal_bridge_req_t *req)
{
  req->type = HAL_BRIDGE_REQ_REPLY;
}

static inline void hal_bridge_disconnect(hal_bridge_req_t *req)
{
  req->type = HAL_BRIDGE_REQ_DISCONNECT;
}

static inline void hal_bridge_open(hal_bridge_req_t *req, int name_len, const char* name, int flags, int mode)
{
  req->type = HAL_BRIDGE_REQ_OPEN;
  req->open.name_len = name_len;
  req->open.name = (uint32_t)(long)name;
  req->open.flags = flags;
  req->open.mode = mode;
}

static inline void hal_bridge_close(hal_bridge_req_t *req, int file)
{
  req->type = HAL_BRIDGE_REQ_CLOSE;
  req->close.file = file;
}

static inline void hal_bridge_read(hal_bridge_req_t *req, int file, void* ptr, int len)
{
  req->type = HAL_BRIDGE_REQ_READ;
  req->read.file = file;
  req->read.ptr = (uint32_t)(long)ptr;
  req->read.len = len;
}

static inline void hal_bridge_write(hal_bridge_req_t *req, int file, void* ptr, int len)
{
  req->type = HAL_BRIDGE_REQ_WRITE;
  req->write.file = file;
  req->write.ptr = (uint32_t)(long)ptr;
  req->write.len = len;
}

static inline void hal_bridge_fb_open(hal_bridge_req_t *req, int name_len, const char* name, int width, int height, int format)
{
  req->type = HAL_BRIDGE_REQ_FB_OPEN;
  req->fb_open.name_len = name_len;
  req->fb_open.name = (uint32_t)(long)name;
  req->fb_open.width = width;
  req->fb_open.height = height;
  req->fb_open.format = format;
}

static inline void hal_bridge_fb_update(hal_bridge_req_t *req, uint64_t fb, unsigned int addr, int posx, int posy, int width, int height)
{
  req->type = HAL_BRIDGE_REQ_FB_UPDATE;
  req->fb_update.screen = fb;
  req->fb_update.addr = addr;
  req->fb_update.posx = posx;
  req->fb_update.posy = posy;
  req->fb_update.width = width;
  req->fb_update.height = height;
}

static inline void hal_bridge_target_status_sync(hal_bridge_req_t *req)
{
  req->type = HAL_BRIDGE_REQ_TARGET_STATUS_SYNC;
}


static inline void hal_debug_flush_printf(hal_debug_struct_t *debug_struct) {
  while(*(volatile uint32_t *)&debug_struct->pending_putchar);
}

static inline void hal_debug_exit(hal_debug_struct_t *debug_struct, int status) {
  *(volatile uint32_t *)&debug_struct->exit_status = 0x80000000 | status;
}

static inline int hal_debug_is_empty(hal_debug_struct_t *debug_struct)
{
  return *(volatile uint32_t *)&debug_struct->putc_current == 0;
}

static inline int hal_debug_is_busy(hal_debug_struct_t *debug_struct)
{
  return *(volatile uint32_t *)&debug_struct->pending_putchar;
}

static inline void hal_debug_send_printf(hal_debug_struct_t *debug_struct) {
  if (debug_struct->putc_current)
  {
    *(volatile uint32_t *)&debug_struct->pending_putchar = debug_struct->putc_current;
    *(volatile uint32_t *)&debug_struct->putc_current = 0;
  }
}

static inline void hal_debug_putchar(hal_debug_struct_t *debug_struct, char c) {
  hal_debug_flush_printf(debug_struct);
  *(volatile uint8_t *)&(debug_struct->putc_buffer[debug_struct->putc_current++]) = c;
  if (*(volatile uint32_t *)&debug_struct->putc_current == HAL_PRINTF_BUF_SIZE || c == '\n') {
    hal_debug_send_printf(debug_struct);
  }
}

static inline int hal_debug_putchar_nopoll(hal_debug_struct_t *debug_struct, char c) {
  if (*(volatile uint32_t *)&debug_struct->pending_putchar)
    return -1;
  *(volatile uint8_t *)&(debug_struct->putc_buffer[debug_struct->putc_current++]) = c;
  if (*(volatile uint32_t *)&debug_struct->putc_current == HAL_PRINTF_BUF_SIZE || c == '\n') {
    hal_debug_send_printf(debug_struct);
  }
  return 0;
}

static inline void hal_debug_step(hal_debug_struct_t *debug_struct, unsigned int value) {
  *(volatile uint32_t *)&debug_struct->debug_step = value;
  *(volatile uint32_t *)&debug_struct->debug_step_pending = 1;
  while (*(volatile uint32_t *)&debug_struct->debug_step_pending);
}

static inline void hal_debug_reset(hal_debug_struct_t *debug_struct) {
  *(volatile uint32_t *)&debug_struct->exit_status = 0x80000000 | 0x40000000;    // 0xC0000000: reset
}

#endif
