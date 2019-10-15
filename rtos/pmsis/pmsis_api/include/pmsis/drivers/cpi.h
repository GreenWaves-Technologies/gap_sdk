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

#ifndef __PI_DRIVERS_CPI_H__
#define __PI_DRIVERS_CPI_H__

#include "pmsis/pmsis_types.h"


/**
* @ingroup groupDrivers
*/

/**
 * @defgroup CPI CPI
 *
 * This API provides support for capturing images from an image sensor through 
 * the Camera Parallel Interface (CPI) and processing them.
 *
 */

/**
 * @addtogroup CPI
 * @{
 */

/**@{*/

/** \enum pi_cpi_format_e
 * \brief Image format identifier.
 *
 * This can be used to describe the format of the image going through the
 * interface.
 */
typedef enum {
	PI_CPI_FORMAT_RGB565        = 0, /*!< RGB565 format. */
	PI_CPI_FORMAT_RGB555        = 1, /*!< RGB555 format. */
	PI_CPI_FORMAT_RGB444        = 2, /*!< RGB444 format. */
	PI_CPI_FORMAT_YUV422        = 3, /*!< YUV422 format. */
	PI_CPI_FORMAT_BYPASS_LITEND = 4, /*!< Only least significant byte is kept. */
	PI_CPI_FORMAT_BYPASS_BIGEND = 5  /*!< Only most significant byte is kept. */
} pi_cpi_format_e;

/** \struct pi_cpi_conf
 * \brief CPI device configuration structure.
 *
 * This structure is used to pass the desired CPI configuration to the runtime
 * when opening the device.
 */
struct pi_cpi_conf
{
    pi_device_e device; /*!< Device type.  */
    uint8_t itf;        /*!< CPI interface ID where the device is connected. */
};

/** \brief Initialize a CPI configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * camera device is opened.
 *
 * \param conf A pointer to the CPI configuration.
 */
void pi_cpi_conf_init(struct pi_cpi_conf *conf);

/** \brief Open a CPI device.
 *
 * This function must be called before the CPI device can be used. It
 * configures the specified device that can then be used to refer to the
 * opened device when calling other functions.
 *
 * \param device  A pointer to the structure describing the device. This
 * structure muts be allocated by the caller and kept alive until the device is
 * closed.
 * \return          0 if it succeeded or -1 if it failed.
 */
int pi_cpi_open(struct pi_device *device);

/** \brief Close an opened CPI device.
 *
 * This function can be called to close an opened CPI device once it is not
 * needed anymore in order to free all allocated resources. Once this function
 * is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device  A pointer to the structure describing the device.
 */
void pi_cpi_close(struct pi_device *device);

/** \brief Capture a sequence of samples.
 *
 * Queue a buffer that will receive samples from the CPI interface.
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
void pi_cpi_capture(struct pi_device *device, void *buffer, int32_t bufferlen);

/** \brief Capture a sequence of samples asynchronously.
 *
 * Queue a buffer that will receive samples from the CPI interface.
 * This function is asynchronous and will not block the caller.
 * It is possible to call it several times in order to queue several buffers.
 * At a minimum 2 buffers should be queued to ensure that no data sampled is
 * lost. This is also the most efficient way to retrieve data from the CPI
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
void pi_cpi_capture_async(struct pi_device *device, void *buffer,
	int32_t bufferlen, pi_task_t *task);

/** \brief Start capturing samples.
 *
 * The samples arriving at the CPI interface are dropped until the interface is
 * started by calling this function.
 * Once started, the sampling will start at the next beginning of frame.
 *
 * \param device    A pointer to the structure describing the device.
 */
static inline void pi_cpi_control_start(struct pi_device *device);

/** \brief Stop capturing samples.
 *
 * The samples arriving at the CPI interface are dropped after this call.
 *
 * \param device    A pointer to the structure describing the device.
 */
static inline void pi_cpi_control_stop(struct pi_device *device);

/** \brief Set frame format.
 *
 * This can be used either when filtering is not active to specify which 
 * part of the samples to keep, or when filtering is active, to specify what 
 * is the input format, to be able to properly convert the image.
 *
 * \param device    A pointer to the structure describing the device.
 * \param format    The format of the input frame.
 */
static inline void pi_cpi_set_format(struct pi_device *device,
	pi_cpi_format_e format);

/** \brief Configure frame drop.
 *
 * For each sampled frame, the specified number of frames will be dropped before
 * the next frame is sampled.
 *
 * \param device    A pointer to the structure describing the device.
 * \param nb_frame_dropped    Number of frames to drop. Can be set to 0 to
 *   deactivate this feature.
 */
static inline void pi_cpi_set_frame_drop(struct pi_device *device,
	uint32_t nb_frame_dropped);

/** \brief Configure frame filtering.
 *
 * Configure how to filter the input pixels to produce the 8bits output pixels.
 * Each channel is multiplied by a coefficient. They are then summed together
 * and shifted right to obtain an 8 bit pixel.
 *
 * \param device    A pointer to the structure describing the device.
 * \param r_coeff  Red channel is multipled by this coefficient.
 * \param g_coeff  Green channel is multipled by this coefficient.
 * \param b_coeff  Blue channel is multipled by this coefficient.
 * \param shift    The sum of all channels multiplied by their coefficient is
 *   shifted right by this value to fit an 8 bit pixel.
 */
static inline void pi_cpi_set_filter(struct pi_device *device, uint32_t r_coeff,
  uint32_t g_coeff, uint32_t b_coeff, uint32_t shift);

/** \brief Configure frame slicing.
 *
 * Configure how to extract a window slice out of the input frame.
 *
 * \param device    A pointer to the structure describing the device.
 * \param x  x position of the window to extract.
 * \param y  y position of the window to extract.
 * \param w  Width of the window to extract.
 * \param h  Height of the window to extract.
 */
static inline void pi_cpi_set_slice(struct pi_device *device, uint32_t x,
  uint32_t y, uint32_t w, uint32_t h);

//!@}

/**
 * @}
 */

#endif  /* __PI_DRIVERS_CPI_H__ */
