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

#ifndef __RT__RT_UART_H__
#define __RT__RT_UART_H__


/**
* @ingroup groupDrivers
*/



/**
 * @defgroup UART UART
 *
 * This API provides support for transferring data between an external UART device
 * and the processor running this driver.
 *
 */

/**
 * @addtogroup UART
 * @{
 */

/**@{*/


/** \struct rt_uart_conf_t
 * \brief UART device configuration structure.
 *
 * This structure is used to pass the desired UART configuration to the runtime when opening the device.
 */
typedef struct rt_uart_conf_s {
  unsigned int baudrate;         /*!< Required baudrate. */
  int itf;                       /*!< Interface ID on which the uart is connected. */
} rt_uart_conf_t;



/** \brief Initialize a UART configuration with default values.
 *
 * This function can be called to get default values for all parameters before setting some of them.
 * The structure containing the configuration must be kept alive until the uart device is opened.
 *
 * \param conf A pointer to the uart configuration.
 */
void rt_uart_conf_init(rt_uart_conf_t *conf);



/** \brief Open a uart device.
 *
 * This function must be called before the uart device can be used. It will do all the needed configuration to make it
 * usable and also return a handle used to refer to this opened device when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param dev_name  The device name. This name should correspond to the one used to configure the devices managed by the runtime.
 * \param conf      A pointer to the uart configuration. Can be NULL to take default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the device and which can be used with other functions
 */
rt_uart_t *rt_uart_open(char *dev_name, rt_uart_conf_t *conf, rt_event_t *event);



/** \brief Write data to a uart.
 *
 * Ths writes data to the specified uart.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param buffer    The memory buffer from where the data to write to the UART must be read.
 * \param size      The size in bytes of the memory buffer.
 * \param event     The event used for managing termination. The event will be notified as soon as the specified amount of bytes have been captured.
 */
static inline void rt_uart_write(rt_uart_t *handle, void *buffer, size_t size, rt_event_t *event);



/** \brief Close an opened uart device.
 *
 * This function closes an opened uart device once it is not needed anymore, in order to free
 * all allocated resources. Once this function is called, the device is not accessible anymore and must be opened
 * again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param event     The event used for managing termination.
 */
void rt_uart_close(rt_uart_t *handle, rt_event_t *event);



/** \brief Write data to a uart from cluster side.
 *
 * This function implements the same feature as rt_uart_write but can be called from cluster side in order to expose
 * the feature on the cluster.
 * This writes data to the specified uart.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param buffer    The memory buffer from where the data to write to the UART must be read.
 * \param size      The size in bytes of the memory buffer.
 * \param req       The request structure used for termination.
 */
void rt_uart_cluster_write(rt_uart_t *handle, void *buffer, size_t size, rt_uart_req_t *req);



/** \brief Wait until the specified uart cluster request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is finished.
 *
 * \param req       The request structure used for termination.
 */
static inline void rt_uart_cluster_wait(rt_uart_req_t *req);


//!@}

/**
 * @}
 */



/// @cond IMPLEM


rt_uart_t* __rt_uart_open(int channel, rt_uart_conf_t *conf, rt_event_t *event, char *name);

#if defined(UDMA_VERSION)

static inline void rt_uart_write(rt_uart_t *uart, void *buffer, size_t size, rt_event_t *event)
{
  rt_periph_copy(NULL, UDMA_CHANNEL_ID(uart->channel)+1, (unsigned int) buffer, size, UDMA_CHANNEL_CFG_SIZE_8, event);
}

static inline void rt_uart_read(rt_uart_t *uart, void *buffer, size_t size, rt_event_t *event)
{
  rt_periph_copy(NULL, UDMA_CHANNEL_ID(uart->channel), (unsigned int) buffer, size, UDMA_CHANNEL_CFG_SIZE_8, event);
}

#else

void __rt_uart_write(rt_uart_t *uart, void *buffer, size_t size, rt_event_t *event);

static inline void rt_uart_write(rt_uart_t *uart, void *buffer, size_t size, rt_event_t *event)
{
  __rt_uart_write(uart, buffer, size, event);
}

#endif

static inline void rt_uart_cluster_wait(rt_uart_req_t *req)
{
#if defined(ARCHI_HAS_CLUSTER)

  while((*(volatile char *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
#endif

}


/// @endcond

#endif
