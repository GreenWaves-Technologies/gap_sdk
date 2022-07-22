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

#ifndef __PI_DRIVERS_CSI2_H__
#define __PI_DRIVERS_CSI2_H__

#include "pmsis/pmsis_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @ingroup groupDrivers
*/

/**
 * @defgroup CSI2 CSI2
 *
 * This API provides support for capturing images from an image sensor through
 * the Camera Parallel Interface (CSI2) and processing them.
 *
 */

/**
 * @addtogroup CSI2
 * @{
 */

/**@{*/

/** \enum pi_csi2_format_e
 * \brief Image format identifier.
 *
 * This can be used to describe the format of the image going through the
 * interface.
 */
typedef enum {
    PI_CSI2_FORMAT_RAW8        = 0, /*!< RAW8 format. */
    PI_CSI2_FORMAT_RAW10       = 1 /*!< RAW10 format. */
} pi_csi2_format_e;

/** \struct pi_csi2_conf
 * \brief CSI2 device configuration structure.
 *
 * This structure is used to pass the desired CSI2 configuration to the runtime
 * when opening the device.
 */
struct pi_csi2_conf
{
    pi_device_e device; /*!< Device type.  */
    uint8_t itf;        /*!< CSI2 interface ID where the device is connected. */
    uint8_t datasize;   /*!< CSI2 transfer datasize */
};

/** \brief Initialize a CSI2 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * camera device is opened.
 *
 * \param conf A pointer to the CSI2 configuration.
 */
void pi_csi2_conf_init(struct pi_csi2_conf *conf);

/** \brief Open a CSI2 device.
 *
 * This function must be called before the CSI2 device can be used. It
 * configures the specified device that can then be used to refer to the
 * opened device when calling other functions.
 *
 * \param device  A pointer to the structure describing the device. This
 * structure muts be allocated by the caller and kept alive until the device is
 * closed.
 * \return          0 if it succeeded or -1 if it failed.
 */
int pi_csi2_open(struct pi_device *device);

/** \brief Close an opened CSI2 device.
 *
 * This function can be called to close an opened CSI2 device once it is not
 * needed anymore in order to free all allocated resources. Once this function
 * is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device  A pointer to the structure describing the device.
 */
void pi_csi2_close(struct pi_device *device);

/** \brief Capture a sequence of samples.
 *
 * Queue a buffer that will receive samples from the CSI2 interface.
 * This function is synchronous and will block the caller until the specified
 * amount of bytes is received.
 * The buffer will receive samples only if the interface is started.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param buffer    The memory buffer where the captured samples will be
 * transferred.
 * \param bufferlen The size in bytes of the memory buffer.
 */
void pi_csi2_capture(struct pi_device *device, void *buffer, int32_t bufferlen);

/** \brief Capture a sequence of samples asynchronously.
 *
 * Queue a buffer that will receive samples from the CSI2 interface.
 * This function is asynchronous and will not block the caller.
 * It is possible to call it several times in order to queue several buffers.
 * At a minimum 2 buffers should be queued to ensure that no data sampled is
 * lost. This is also the most efficient way to retrieve data from the CSI2
 * device. You should always make sure that at least 2 buffers are always
 * queued, by queuing a new one as soon as the current one is full.
 * The buffer will receive samples only if the interface is started.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param buffer    The memory buffer where the captured samples will be
 *   transferred.
 * \param bufferlen   The size in bytes of the memory buffer.
 * \param task        The task used to notify the end of transfer. See the
 *   documentation of pi_task for more details.
 */
void pi_csi2_capture_async(struct pi_device *device, void *buffer,
                           int32_t bufferlen, pi_task_t *task);

/** \brief Start capturing samples.
 *
 * The samples arriving at the CSI2 interface are dropped until the interface is
 * started by calling this function.
 * Once started, the sampling will start at the next beginning of frame.
 *
 * \param device    A pointer to the structure describing the device.
 */
static inline void pi_csi2_control_start(struct pi_device *device);

/** \brief Stop capturing samples.
 *
 * The samples arriving at the CSI2 interface are dropped after this call.
 *
 * \param device    A pointer to the structure describing the device.
 */
static inline void pi_csi2_control_stop(struct pi_device *device);

/** \brief Set lane number format.
 *
 * This can be used configure the MIPI CSI2 lane number.
 *
 * \param device    A pointer to the structure describing the device.
 * \param num       Number of lanes, 0 - 1lane, 1 - 2lanes.
 */
static inline void pi_csi2_set_lane(struct pi_device *device,
                                    uint8_t num);

/** \brief Set pixel clock divider.
 *
 * This can be used configure the CSI2's receiver's pixel clock divder to match
&* the frequency of MIPI CSI2 lane.
 *
 * \param device    A pointer to the structure describing the device.
 * \param pixel_div Clock divider [0, 255].
 */
static inline void pi_csi2_set_pixel_clk_div(struct pi_device *device,
                                             uint8_t pixel_div);

/** \brief Set APB clock divider.
 *
 * This can be used configure the CSI2's DPHY's APB clock divder to configure
&* the DPHY receiver.
 *
 * \param device    A pointer to the structure describing the device.
 * \param apb_div Clock divider [0, 255].
 */
static inline void pi_csi2_set_apb_clk_div(struct pi_device *device,
                                             uint8_t apb_div);

/** \brief Set Virtual channel ID.
 *
 * This can be used configure the CSI2's DPHY's virtual channel number,
 * only support VC0 and VC1.
 *
 * \param device    A pointer to the structure describing the device.
 * \param vc        VC channel number, [0, 1]. If vc is 2, it means two outstanding streams
 */
static inline void pi_csi2_set_virtual_channel(struct pi_device *device,
                                             uint8_t vc);

//!@}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif  /* __PI_DRIVERS_CSI2_H__ */
