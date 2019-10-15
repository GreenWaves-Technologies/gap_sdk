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

#ifndef __RT__RT_I2S_H__
#define __RT__RT_I2S_H__



/**
* @ingroup groupDrivers
*/



/**
 * @defgroup I2S I2S
 *
 * This API provides support for transferring data between an external I2S device
 * and the processor running this driver. One API can actually drive up to 4 I2S coupled together.
 *
 */

/**
 * @addtogroup I2S
 * @{
 */

/**@{*/


/** \struct rt_i2s_conf_t
 * \brief I2S device configuration structure.
 *
 * This structure is used to pass the desired I2S configuration to the runtime when opening the device.
 */
typedef struct rt_i2s_conf_s {
  unsigned short frequency;         /*!< Sampling frequency in Hz (e.g. 44100). */
  unsigned short decimation_log2;   /*!< In case PDM is active, give the log2 of the decimation factor. */
  unsigned char  pdm;               /*!< Activate pulse-density demodulation. */
  unsigned char  dual;              /*!< Activate dual mode (left and right channels). */
  unsigned char  width;             /*!< Width of the samples (8 or 16 bits). */
  signed char id;                   /*!< If it is different from -1, this specifies on which I2S interface the device is connected. */
  unsigned int flags;               /*!< Additional flags. Some of these flags may be platform-specific. */
  unsigned int channels;            /*!< Number of channels for this interface. */
} rt_i2s_conf_t;



/** \brief Initialize an I2S configuration with default values.
 *
 * This function can be called to get default values for all parameters before setting some of them.
 * The structure containing the configuration must be kept alive until the i2s device is opened.
 *
 * \param conf A pointer to the I2S configuration.
 */
void rt_i2s_conf_init(rt_i2s_conf_t *conf);



/** \brief Open an I2S device.
 *
 * This function must be called before the I2S device can be used. It will do all the needed configuration to make it
 * usable and also return a handle used to refer to this opened device when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param dev_name  The device name. This name should correspond to the one used to configure the devices managed by the runtime.
 * \param conf      A pointer to the I2S configuration. Can be NULL to take default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the device and which can be used with other functions
 */
rt_i2s_t *rt_i2s_open(char *dev_name, rt_i2s_conf_t *conf, rt_event_t *event);



/** \brief Capture a sequence of samples.
 *
 * Enqueue the specified buffer so that the I2S samples are transfered to this buffer.
 * It is possible to call this function asynchronously and several time in order to enqueue several buffers.
 * At least 2 buffers should be queued to ensure that no data sampled is lost and also enables maximum performance.
 * Queue a new buffer when one is full to ensure that at least 2 are always available.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param buffer    The memory buffer where the captured samples will be transfered.
 * \param size      The size in bytes of the memory buffer.
 * \param event     The event used for managing termination. The event will be notified as soon as the specified amount of bytes have been captured.
 */
static inline void rt_i2s_capture(rt_i2s_t *handle, void *buffer, size_t size, rt_event_t *event);

static inline void rt_i2s_channel_capture(rt_i2s_t *handle, int channel, void *buffer, size_t size, rt_event_t *event);



/** \brief Start sampling.
 *
 * This immediately starts sampling of the data stream. If no capture buffer is configured yet,
 * the samples are just lost, otherwise they are transfered to the capture buffer.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 */
static inline void rt_i2s_start(rt_i2s_t *handle);



/** \brief Stop sampling.
 *
 * This immediately stops sampling of the data stream.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 */
static inline void rt_i2s_stop(rt_i2s_t *handle);



/** \brief Close an opened I2S device.
 *
 * This function closes an opened I2S device once it is not needed anymore, in order to free
 * all allocated resources. Once this function is called, the device is not accessible anymore and must be opened
 * again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param event     The event used for managing termination.
 */
static inline void rt_i2s_close(rt_i2s_t *handle, rt_event_t *event);



//!@}

/**
 * @}
 */



/// @cond IMPLEM

extern rt_i2s_dev_t i2s_desc;

static inline void rt_i2s_close(rt_i2s_t *dev, rt_event_t *event)
{
  dev->desc.close(dev, event);
}

static inline void rt_i2s_capture(rt_i2s_t *dev, void *buffer, size_t size, rt_event_t *event)
{
  dev->desc.capture(dev, buffer, size, event);
}

static inline void rt_i2s_channel_capture(rt_i2s_t *dev, int channel, void *buffer, size_t size, rt_event_t *event)
{
  dev->desc.channel_capture(dev, channel, buffer, size, event);
}

static inline void rt_i2s_start(rt_i2s_t *dev)
{
  dev->desc.start(dev);
}

static inline void rt_i2s_stop(rt_i2s_t *dev)
{
  dev->desc.stop(dev);
}

/// @endcond




#endif
