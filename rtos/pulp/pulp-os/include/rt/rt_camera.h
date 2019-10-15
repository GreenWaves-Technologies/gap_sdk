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

#ifndef __CAMERA_V2_H__
#define __CAMERA_V2_H__

/**
* @ingroup groupDrivers
*/

/**
 * @defgroup Camera Camera
 *
 * This API provides support for capturing images from an image sensor. It
 * also provides an API for configuring and controlling an image sensor.
 *
 */

/**
 * @addtogroup Camera
 * @{
 */

/**@{*/


/** \brief Initialize a camera configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the camera device is opened.
 * Can only be called from fabric-controller side.
 *
 * \param conf A pointer to the camera configuration.
 */
void rt_camera_conf_init(rt_cam_conf_t *conf);


/** \brief Open an image sensor device.
 *
 * This function must be called before the Camera device can be used. It configure the device
 * and return a handle that can be used to refer to the opened device when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 * Can only be called from fabric-controller side.
 *
 * \param dev_name  The device name. This name should correspond to the one used to configure the devices managed by the runtime.
 * \param conf      A pointer to the Camera device configuration. Can be NULL to use the default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the device which can be used with other Camera functions.
 */
rt_camera_t* rt_camera_open(char *dev_name, rt_cam_conf_t *conf, rt_event_t *event);


/** \brief Control the Camera device.
 *
 * This function is used to control and configure the Camera device. For each command, the arguments necessary are listed below:
 *       CMD         |     Type of argument
 *    --|--
 *    CMD_RESOL      |     rt_cam_resol_e
 *    CMD_FORMAT     |     rt_ov7670_format_e OR rt_himax_format_e
 *    CMD_FPS        |     rt_cam_fps_e
 *    CMD_SLICE      |     rt_img_slice_t
 *    CMD_FILTER     |     rt_img_filter_t
 *    CMD_SHIFT      |     unsigned char
 *    CMD_FRAMEDROP  |     unsigned int
 *    CMD_START      |     NULL
 *    CMD_STOP       |     NULL
 *    CMD_PAUSE      |     NULL
 * Can only be called from fabric-controller side.
 *
 * \param handle    The handle of the device camera which returned when the device was opened.
 * \param cmd       The command for controlling or configuring the camera. Check the description of rt_cam_cmd_e for further information.
 * \param *arg      A pointer to the arguments of the command.
 */
static inline void rt_cam_control(rt_camera_t *handle, rt_cam_cmd_e cmd, void *arg);

/** \brief Capture a sequence of samples.
 *
 * Queue a buffer that will receive Camera samples.
 * It is possible to call this function asynchronously and several times in order to queue several buffers.
 * At a minimum 2 buffers should be queued to ensure that no data sampled is lost. This is also the most efficient
 * way to retrieve data from the Camera device. You should always make sure that at least 2 buffers are always queued,
 * by queuing a new one as soon as the current one is full.
 * Can only be called from fabric-controller side.
 *
 * \param handle    The handle of the device which was returned when the device was opened.
 * \param buffer    The memory buffer where the captured samples will be transfered.
 * \param size      The size in bytes of the memory buffer.
 * \param event     The event used for managing termination. The event will be notified as soon as the specified amount of bytes have been captured.
 */
static inline void rt_camera_capture (rt_camera_t *handle, void *buffer, size_t size, rt_event_t *event);

/** \brief Close an opened Camera device.
 *
 * This function can be called to close an opened Camera device once it is not needed anymore in order to free
 * all allocated resources. Once this function is called, the device is not accessible anymore and must be opened
 * again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 * Can only be called from fabric-controller side.
 *
 * \param handle    The handle of the device which was returned when it was opened.
 * \param event     The event used for managing termination.
 */
static inline void rt_camera_close(rt_camera_t *handle, rt_event_t *event);



/** \brief Capture a sequence of samples from cluster side.
 *
 * This function is a remote call that the cluster can do to the fabric-controller
 * in order to ask for a camera capture.
 * Refer to the documentation of rt_camera_capture for more details.
 * Can only be called from cluster side.
 *
 * \param handle    The handle of the device which was returned when the device was opened.
 * \param buffer    The memory buffer where the captured samples will be transfered.
 * \param size      The size in bytes of the memory buffer.
 * \param req       A pointer to the camera request structure. It must be allocated by the caller and kept alive until the capture is finished.
 */
void rt_camera_cluster_capture (rt_camera_t *handle, void *buffer, size_t size, rt_camera_req_t *req);



/** \brief Control the Camera device from cluster side.
 *
 * This function is a remote call that the cluster can do to the fabric-controller
 * in order to ask for a camera control.
 * Refer to the documentation of rt_cam_control for more details.
 * Can only be called from cluster side.
 *
 * \param handle    The handle of the device camera which returned when the device was opened.
 * \param cmd       The command for controlling or configuring the camera. Check the description of rt_cam_cmd_e for further information.
 * \param *arg      A pointer to the arguments of the command.
 * \param req       A pointer to the camera request structure. It must be allocated by the caller and kept alive until the control is finished.
 */
void rt_camera_cluster_control(rt_camera_t *handle, rt_cam_cmd_e cmd, void *arg, rt_camera_req_t *req);



/** \brief Wait until the specified camera request has finished.
 *
 * This blocks the calling core until the specified cluster remote request is finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void rt_camera_cluster_wait(rt_camera_req_t *req);




//!@}

/**
 * @}
 */


/// @cond IMPLEM

// =================================================
// Some useful common defines
// =================================================
#define QVGA_W   	                         (320)
#define QVGA_H	                           (240)
#define IMG_QVGASIZE                       (QVGA_H*QVGA_W)    //320*240
#define PPM_HEADER                          40

#define MASK_1BIT                           0x1
#define MASK_2BITS                          0x3
#define MASK_3BITS                          0x7
#define MASK_4BITS                          0xF
#define MASK_5BITS                          0x1F
#define MASK_6BITS                          0x3F
#define MASK_7BITS                          0x7F
#define MASK_8BITS                          0xFF
#define MASK_16BITS                         0xFFFF

#define ENABLE            1
#define DISABLE           0

#define RGB565            0
#define RGB555            1
#define RGB444            2
#define YUV422            3
#define BYPASS_LITEND     4
#define BYPASS_BIGEND     5

typedef enum{
    presc6 = 5,
    presc3 = 2,
}rt_cam_presc_e;

extern rt_cam_dev_t himax_desc;
extern rt_cam_dev_t ov7670_desc;

static inline void rt_cam_control(rt_camera_t *dev, rt_cam_cmd_e cmd, void *arg)
{
  dev->desc.control(dev, cmd, arg);
}

static inline void rt_camera_capture (rt_camera_t *dev, void *buffer, size_t bufferlen, rt_event_t *event)
{
  dev->desc.capture(dev, buffer, bufferlen, event);
}

static inline void rt_camera_close(rt_camera_t *dev, rt_event_t *event)
{
  dev->desc.close(dev, event);
}


void _camera_drop_frame(rt_cam_conf_t *cam, unsigned int *arg);
void _camera_normlize(rt_cam_conf_t *cam, unsigned int *arg);
void _camera_filter(rt_cam_conf_t *cam, rt_img_filter_t *filter);
void _camera_extract(rt_cam_conf_t *cam, rt_img_slice_t *slicer);
void _camera_start();
void _camera_stop();

#if defined(ARCHI_HAS_CLUSTER)

static inline __attribute__((always_inline)) void rt_camera_cluster_wait(rt_camera_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

#endif

/// @endcond




#endif
