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

#ifndef __BSP_DISPLAY_H__
#define __BSP_DISPLAY_H__

#include "pmsis.h"
#include "bsp/buffer.h"

/**
 * @defgroup Display Display
 *
 * The display driver provides supprt to write and print on LCD screens.
 */

/**
 * @addtogroup Display
 * @{
 */


typedef enum
{
  PI_DISPLAY_IOCTL_CUSTOM = 0
} pi_display_ioctl_cmd_e;


/**
 * @brief Open a display device.
 *
 * This function must be called before the display device can be used.
 * It does all the needed configuration to enable displaye device.
 *
 * @param device         A pointer to the device structure of the device to open.
 *
 * @retval 0 Device opened successfully. error code otherwise.
 */
int pi_display_open(struct pi_device *device);

/**
 * @brief Write on a display device. Blocking API.
 *
 * This function is called to print/write a buffer on a display device.
 * It displays a whole frame of (width * height) size on display at position (x, y).
 *
 * @param device         A pointer to the device structure of the device to open.
 * @param buffer         Data buffer to write on diplay.
 * @param x              X position on LCD.
 * @param y              Y position on LCD.
 * @param w              Width of buffer.
 * @param h              Height of buffer.
 *
 * @note This function is synchronous, caller can not do anything else until transfer
 * is finished. The pending asynchronous function is below : pi_display_write_async().
 */
void pi_display_write(struct pi_device *device, pi_buffer_t *buffer,
                      uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * @brief Send display specific command.
 *
 * This function is called to send a display device specific command.
 * Arguments for commands can be given using @param arg parameter.
 *
 * @param device         A pointer to the device structure of the device to open.
 * @param cmd            Command to send.
 * @param arg            Parameters/arguments to send to device for the command.
 *
 * @return Value Depend on type of commands.
 */
static inline int32_t pi_display_ioctl(struct pi_device *device, uint32_t cmd,
                                       void *arg);

/**
 * @brief Write on a display device. Non blocking API.
 *
 * This function is called to print/write a buffer on a display device.
 * It displays a whole frame of (width * height) size on display at position (x, y).
 *
 * @param device         A pointer to the device structure of the device to open.
 * @param buffer         Data buffer to write on diplay.
 * @param x              X position on LCD.
 * @param y              Y position on LCD.
 * @param w              Width of buffer.
 * @param h              Height of buffer.
 * @param task           Task to use to check end of transfer.
 *
 * @note This function is asynchronous, caller can do something else while transfer is
 * enqueued The pending synchronous function is below : pi_display_write().
 */
static inline void pi_display_write_async(struct pi_device *device, pi_buffer_t *buffer,
                                          uint16_t x, uint16_t y,uint16_t w, uint16_t h,
                                          pi_task_t *task);

/**
 * @}
 */


/// @cond IMPLEM

typedef struct {
  int (*open)(struct pi_device *device);
  void (*write_async)(struct pi_device *device, pi_buffer_t *buffer, uint16_t x, uint16_t y,uint16_t w, uint16_t h, pi_task_t *task);
  int32_t (*ioctl)(struct pi_device *device, uint32_t cmd, void *arg);
} pi_display_api_t;

struct pi_display_conf {
  pi_display_api_t *api;
};

static inline void pi_display_write_async(struct pi_device *device, pi_buffer_t *buffer, uint16_t x, uint16_t y,uint16_t w, uint16_t h, pi_task_t *task)
{
  pi_display_api_t *api = (pi_display_api_t *)device->api;
  api->write_async(device, buffer, x, y, w, h, task);
}

void __display_conf_init(struct pi_display_conf *conf);

static inline int32_t pi_display_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
  pi_display_api_t *api = (pi_display_api_t *)device->api;
  return api->ioctl(device, cmd, arg);
}


/// @endcond

#endif  /* __BSP_DISPLAY_H__ */
