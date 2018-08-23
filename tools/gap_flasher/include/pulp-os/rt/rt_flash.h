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

#ifndef __RT_RT_FLASH_H__
#define __RT_RT_FLASH_H__




/**  
* @ingroup groupDrivers  
*/



/**        
 * @defgroup Flash Flash
 *
 * The flash driver provides support for transferring data between an external flash chip
 * and the chip running this driver.
 *
 */

/**        
 * @addtogroup Flash
 * @{        
 */

/**@{*/



/** \brief Initialize a flash configuration with default values.
 *
 * The structure containing the configuration must be kept allocated until the flash is opened.
 *
 * \param conf A pointer to the flash configuration.
 */
void rt_flash_conf_init(rt_flash_conf_t *conf);



/** \brief Open a flash device.
 *
 * This function must be called before the flash device is used. It will do all necessary configuration
 * and return a handle used to refer to this opened device when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param dev_name  The device name. This name should correspond to the one used to configure the devices managed by the runtime.
 * \param conf      A pointer to the flash configuration. Can be NULL to take default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the device and which can be used with other functions
 */
rt_flash_t *rt_flash_open(char *dev_name, rt_flash_conf_t *conf, rt_event_t *event);



/** \brief Close an opened flash device.
 *
 * This function can be called to close an opened flash device once it is not needed anymore, in order to free
 * all allocated resources. Once this function is called, the device is not accessible anymore and must be opened
 * again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param event     The event used for managing termination.
 */
void rt_flash_close(rt_flash_t *handle, rt_event_t *event);



/** \brief Enqueue a read copy to the flash (from flash to processor).
 *
 * The copy will make an asynchronous transfer between an external flash and one of the memories in the processor. 
 * An event can be specified in order to be notified when the transfer is finished.
 *
 * \param dev         The device descriptor of the flash on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param flash_addr  The address of the copy in the flash.
 * \param size        The size in bytes of the copy.
 * \param event       The event used to notify the end of transfer. See the documentation of rt_event_t for more details.
 */
static inline void rt_flash_read(rt_flash_t *dev, void *addr, void *flash_addr, size_t size, rt_event_t *event);



/** \brief Enqueue a read copy to the flash from cluster side (from flash to processor).
 *
 * This function is equivalent to rt_flash_read but can be called from cluster side.
 * The copy will make an asynchronous transfer between an external flash and one of the memories in the processor. 
 * An event can be specified in order to be notified when the transfer is finished.
 * Can only be called from cluster side.
 *
 * \param dev         The device descriptor of the flash on which to do the copy.
 * \param addr        The address of the copy in the processor.
 * \param flash_addr  The address of the copy in the flash.
 * \param size        The size in bytes of the copy.
 * \param req         The request structure used for termination.
 */
void rt_flash_cluster_read(rt_flash_t *dev, void *addr, void *flash_addr, size_t size, rt_flash_req_t *req);



/** \brief Wait until the specified flash request has finished.
 *
 * This blocks the calling core until the specified cluster remote copy is finished.
 * Can only be called from cluster side.
 *
 * \param req       The request structure used for termination.
 */
static inline void rt_flash_cluster_wait(rt_flash_req_t *req);



//!@}

/**        
 * @} end of Flash        
 */



/// @cond IMPLEM

extern rt_flash_dev_t hyperflash_desc;
extern rt_flash_dev_t spiflash_desc;

static inline void rt_flash_read(rt_flash_t *dev, void *addr, void *data, size_t size, rt_event_t *event)
{
  dev->desc.read(dev, addr, data, size, event);
}

#if defined(ARCHI_HAS_CLUSTER)

static inline __attribute__((always_inline)) void rt_flash_cluster_wait(rt_flash_req_t *req)
{
  while((*(volatile int *)&req->done) == 0)
  {
    eu_evt_maskWaitAndClr(1<<RT_CLUSTER_CALL_EVT);
  }
}

#endif

/// @endcond

#endif
