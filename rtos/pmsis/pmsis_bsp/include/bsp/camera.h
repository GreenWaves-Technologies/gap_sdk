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

#ifndef __BSP_CAMERA_H__
#define __BSP_CAMERA_H__

#include "pmsis/drivers/cpi.h"
#include "pmsis/drivers/i2c.h"

/**
 * @defgroup Camera Camera
 *
 * The camera driver provides support for capture data from an external
 * camera and getting the data into the chip running
 * this driver.
 *
 */

/**
 * @addtogroup Camera
 * @{
 */

/**@{*/

/** \enum pi_camera_cmd_e
 * \brief Command ID for pi_camera_control.
 *
 */
typedef enum {
  PI_CAMERA_CMD_ON,    /*!< Power-up the camera. */
  PI_CAMERA_CMD_OFF,   /*!< Power-down the camera. */
  PI_CAMERA_CMD_START, /*!< Start the camera, i.e. it will start sending data on the interface. */
  PI_CAMERA_CMD_STOP   /*!< Stop the camera, i.e. it will stop sending data on the interface. */
} pi_camera_cmd_e;     /*!< */


/** \enum pi_camera_format_e
 * \brief Camera format.
 *
 * This can be given within the camera configuration when it is opened.
 */
typedef enum {
  PI_CAMERA_VGA,  /*!< VGA format (640 x 480). */
  PI_CAMERA_QVGA, /*!< QVGA format (320 x 240). */
  PI_CAMERA_QQVGA /*!< QQVGA format (160 x 120). */
} pi_camera_format_e;

/** \enum pi_camera_color_mode_e
 * \brief Camera color mode.
 *
 * This can be given within the camera configuration when it is opened.
 */
typedef enum {
  PI_CAMERA_GRAY8,      /*!< 8 bit grayscale. */
  PI_CAMERA_RGB565,     /*!< 16 bit RGB . */
  PI_CAMERA_RGB888,     /*!< 24 bit RGB . */
  PI_CAMERA_YUV         /*!< 24 bit YUV . */
} pi_camera_color_mode_e;


/** \brief Open an image sensor device.
 *
 * This function must be called before the Camera device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int32_t pi_camera_open(struct pi_device *device);


/** \brief Control the Camera device.
 *
 * This function is used to control and configure the Camera device. For each
 * command, the arguments necessary are listed below:
 *       CMD         |     Type of argument
 *    --|--
 *    CMD_ON         |     NULL
 *    CMD_OFF        |     NULL
 *    CMD_START      |     NULL
 *    CMD_STOP       |     NULL
 *
 * \param device    The device structure of the device to control.
 * \param cmd       The command for controlling or configuring the camera.
 *   Check the description of pi_camera_cmd_e for further information.
 * \param *arg      A pointer to the arguments of the command.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
static inline int32_t pi_camera_control(struct pi_device *device,
  pi_camera_cmd_e cmd, void *arg);

/** \brief Capture a sequence of samples.
 *
 * Queue a buffer that will receive Camera samples.
 * The samples will start being stored in the provided buffer as soon as the
 * camera is started. If it is already started, it starts storing them
 * immediately. On some chips, the start of the sampling may be differed to the
 * next start of frame, see chip-specific section for more details.
 * The caller is blocked until the transfer is finished.
 *
 * \param device    The device structure of the device where to capture
 *   samples.
 * \param buffer    The memory buffer where the captured samples will be
 *   transfered.
 * \param size      The size in bytes of the memory buffer.
 */
void pi_camera_capture(struct pi_device *device, void *buffer, uint32_t size);

/** \brief Capture a sequence of samples.
 *
 * Queue a buffer that will receive Camera samples.
 * The samples will start being stored in the provided buffer as soon as the
 * camera is started. If it is already started, it starts storing them
 * immediately. On some chips, the start of the sampling may be differed to the
 * next start of frame, see chip-specific section for more details.
 * It is possible to call this function asynchronously and several times in
 * order to queue several buffers.
 * At a minimum 2 buffers should be queued to ensure that no data sampled is
 * lost. This is also the most efficient way to retrieve data from the Camera
 * device. You should always make sure that at least 2 buffers are always
 * queued, by queuing a new one as soon as the current one is full.
 * Can only be called from fabric-controller side.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * \param device    The device structure of the device where to capture
 *   samples.
 * \param buffer    The memory buffer where the captured samples will be
 *   transfered.
 * \param size      The size in bytes of the memory buffer.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
static inline void pi_camera_capture_async(struct pi_device *device, void *buffer,
  uint32_t size, pi_task_t *task);

/** \brief Close an opened Camera device.
 *
 * This function can be called to close an opened Camera device once it is not
 * needed anymore in order to free all allocated resources. Once this function
 * is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
static inline void pi_camera_close(struct pi_device *device);

/** \brief Set camera register.
 *
 * This can be called to set a camera register. This must be used carefully
 * as this can disturb the behavior of the other calls. The list of registers
 * is specific to each camera, see the documentation of the camera for more
 * information.
 *
 * \param device    The device structure of the camera.
 * \param reg_addr  The register address.
 * \param value     A pointer to the value to be set. The size of this variable
 *   depends on the register being accessed.
 */
static inline int32_t pi_camera_reg_set(struct pi_device *device,
  uint32_t reg_addr, uint8_t *value);

/** \brief Get camera register.
 *
 * This can be called to get a camera register. The list of registers
 * is specific to each camera, see the documentation of the camera for more
 * information.
 *
 * \param device    The device structure of the camera.
 * \param reg_addr  The register address.
 * \param value     A pointer to the value where the read value will be stored.
 *   The size of this variable depends on the register being accessed.
 */
static inline int32_t pi_camera_reg_get(struct pi_device *device,
  uint32_t reg_addr, uint8_t *value);





//!@}

/**
 * @}
 */


/// @cond IMPLEM

typedef struct {
  int32_t (*open)(struct pi_device *device);
  void (*close)(struct pi_device *device);
  int32_t (*control)(struct pi_device *device, pi_camera_cmd_e cmd, void *arg);
  void (*capture_async)(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task);
  int32_t (*reg_get)(struct pi_device *device, uint32_t addr, uint8_t *value);
  int32_t (*reg_set)(struct pi_device *device, uint32_t addr, uint8_t *value);
  void (*set_crop)(struct pi_device *device, uint8_t offset_x, uint8_t offset_y,uint16_t width,uint16_t height);
} pi_camera_api_t;

struct pi_camera_conf {
  int itf;
  pi_camera_api_t *api;
};

static inline int32_t pi_camera_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
  pi_camera_api_t *api = (pi_camera_api_t *)device->api;
  return api->control(device, cmd, arg);
}

static inline void pi_camera_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
  pi_camera_api_t *api = (pi_camera_api_t *)device->api;
  api->capture_async(device, buffer, bufferlen, task);
}

static inline void pi_camera_close(struct pi_device *device)
{
  pi_camera_api_t *api = (pi_camera_api_t *)device->api;
  api->close(device);
}

static inline int32_t pi_camera_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  pi_camera_api_t *api = (pi_camera_api_t *)device->api;
  return api->reg_set(device, addr, value);
}

static inline int32_t pi_camera_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  pi_camera_api_t *api = (pi_camera_api_t *)device->api;
  return api->reg_get(device, addr, value);
}

static inline void pi_camera_set_crop(struct pi_device *device, uint8_t offset_x, uint8_t offset_y,uint16_t width,uint16_t height)
{
  pi_camera_api_t *api = (pi_camera_api_t *)device->api;
  return api->set_crop(device, offset_x, offset_y,width,height);
}


void __camera_conf_init(struct pi_camera_conf *conf);

/// @endcond




#endif
