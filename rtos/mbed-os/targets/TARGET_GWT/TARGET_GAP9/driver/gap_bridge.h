/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GAP_BRIDGE_H_
#define _GAP_BRIDGE_H_

#include "gap_util.h"
#include "gap_debug.h"

/*!
 * @addtogroup bridge
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @name Driver version */
/*@{*/

#define PROTOCOL_VERSION  4    // Added field "bridge_to_target" in requests as they are now released by the target

/*! @brief BRIDGE driver version 1.0.0. */
#define GAP_BRIDGE_DRIVER_VERSION (MAKE_VERSION(PROTOCOL_VERSION, 0, 0))
/*@}*/

typedef enum {
  BRIDGE_REQ_CONNECT = 0,
  BRIDGE_REQ_DISCONNECT = 1,
  BRIDGE_REQ_OPEN = 2,
  BRIDGE_REQ_READ = 3,
  BRIDGE_REQ_WRITE = 4,
  BRIDGE_REQ_CLOSE = 5,
  BRIDGE_REQ_FB_OPEN = 6,
  BRIDGE_REQ_FB_UPDATE = 7,
  BRIDGE_REQ_TARGET_STATUS_SYNC = 8,
  BRIDGE_REQ_REPLY = 9,
  BRIDGE_REQ_EFUSE_ACCESS = 10,
  BRIDGE_REQ_EEPROM_ACCESS = 11,
  BRIDGE_REQ_BUFFER_ALLOC = 12,
  BRIDGE_REQ_BUFFER_FREE = 13,
  BRIDGE_REQ_FLASH_ACCESS = 14,
  BRIDGE_REQ_FLASH_ERASE_CHIP = 15,
  BRIDGE_REQ_FLASH_ERASE_SECTOR = 16,
  BRIDGE_REQ_FIRST_USER = 17
} bridge_req_e;

typedef enum {
  HAL_BRIDGE_REQ_FB_FORMAT_GRAY = 1,
  HAL_BRIDGE_REQ_FB_FORMAT_RGB = 2,
  HAL_BRIDGE_REQ_FB_FORMAT_RAW = 3
} bridge_fb_format_e;

typedef struct _bridge_req_s {
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
} bridge_req_t;

typedef debug_struct_t bridge_t;

/*! @} */

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @addtogroup bridge_driver
 * @{
 */

/*!
 * @brief Initilize binding software event BRIDGE, binding to notifyReqAddr, notifyReqValue
 *
 */
void BRIDGE_Init();

/*!
 * @brief Connect to bridge.
 *
 * @param wait_bridge  BRIDGE waiting flag.
 * @param event        BRIDGE Interrupt handler pointer.
 */
int BRIDGE_Connect(int wait_bridge, void *event);

/*!
 * @brief Disconnect to bridge.
 *
 * @param event  BRIDGE Interrupt handler pointer.
 */
void BRIDGE_Disconnect(void *event);

/*!
 * @brief Bridge open a file according to flags and mode.
 *
 * @param name         File name.
 * @param flags        File flags.
 * @param mode         File mode.
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             File open status
 */
int BRIDGE_Open(const char* name, int flags, int mode, void* event);

/*!
 * @brief Bridge waiting openning a file.
 *
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             File open status
 */
int BRIDGE_OpenWait(void *event);

/*!
 * @brief Bridge close a file.
 *
 * @param file         File number.
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             File close status
 */
int BRIDGE_Close(int file, void *event);

/*!
 * @brief Bridge waiting closing a file.
 *
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             File close status
 */
int BRIDGE_CloseWait(void *event);

/*!
 * @brief Bridge read a file.
 *
 * @param file        File number.
 * @param ptr         Read buffer pointer.
 * @param len         Read length.
 * @param event       BRIDGE Interrupt handler pointer.
 * @retval            File read status
 */
int BRIDGE_Read(int file, void* ptr, int len, void *event);

/*!
 * @brief Bridge waiting Reading a file.
 *
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             File read status
 */
int BRIDGE_ReadWait(void *event);

/*!
 * @brief Bridge Write a file.
 *
 * @param file        File number.
 * @param ptr         Write buffer pointer.
 * @param len         Write length.
 * @param event       BRIDGE Interrupt handler pointer.
 * @retval            File write status
 */
int BRIDGE_Write(int file, void* ptr, int len, void *event);

/*!
 * @brief Bridge waiting Writing a file.
 *
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             File write status
 */
int BRIDGE_WriteWait(void *event);

/*!
 * @brief Bridge delay for an reference clock rising edge.
 *
 */
void BRIDGE_Delay();

/*!
 * @brief Bridge open image frame buffer.
 *
 * @param name         Image name.
 * @param width        Image width.
 * @param height       Image height.
 * @param format       Image format.
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             Image frame open status
 */
uint64_t BRIDGE_FBOpen(const char* name, int width, int height, bridge_fb_format_e format, void *event);

/*!
 * @brief Bridge Waiting openning image frame buffer.
 *
 * @param event        BRIDGE Interrupt handler pointer.
 * @retval             Image frame open status
 */
uint64_t BRIDGE_FbBOpenWait(void *event);

/*!
 * @brief Bridge open image frame buffer.
 *
 * @param fb           Frame buffer number.
 * @param addr         Frame buffer address.
 * @param posx         Frame buffer position X.
 * @param posy         Frame buffer position Y.
 * @param width        Image width.
 * @param height       Image height.
 * @param event        BRIDGE Interrupt handler pointer.
 */
void BRIDGE_FBUpdate(uint64_t fb, int addr, int posx, int posy, int width, int height, void *event);

/*!
 * @brief Bridge taget status synchronozation.
 *
 * @param event        BRIDGE Interrupt handler pointer.
 */
void BRIDGE_TargetStatusSync(void *event);

/*!
 * @brief Bridge pirntf flush.
 *
 */
void BRIDGE_PrintfFlush();

/*!
 * @brief Bridge Send notification.
 *
 */
void BRIDGE_SendNotif();

/*!
 * @brief Bridge clear notification.
 *
 */
void BRIDGE_ClearNotif();

/*!
 * @brief BRIDGE interrupt handler
 */
void BRIDGE_IRQHandler(void);

/*@}*/


#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_BRIDGE_H_*/
