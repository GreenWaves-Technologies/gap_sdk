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

#ifndef __RT_RT_DEV_H__
#define __RT_RT_DEV_H__

/// @cond IMPLEM


/**  
* @ingroup groupKernel  
*/



/**        
 * @defgroup Device Device descriptor
 *
 * Device descriptors are used to describe the devices which can be used by the chip, in order to let
 * the runtime know how to control them.
 *
 * The runtime is compiled with one descriptor per device. The application must first get the descriptor
 * associated to a device, and then use it with the device API in order to refer to this device.
 */

/**        
 * @addtogroup Device
 * @{        
 */

/**@{*/


/** \brief Get a device descriptor
 *
 * \param dev   The string identifying the device to get.
 * \return      A pointer to the device descriptor if it is found, or NULL.
 */
rt_dev_t *rt_dev_get(char *dev);

/** \brief Mount a device
 *
 * This operation makes sure the device is available. After this operation, the returned device descriptor can be used
 * with other APIs to access the device. Calling this operation will activate everything necessary to access
 * the device, and will increase the power consumption so the device should be unmounted as soon as it is not
 * needed anymore.
 *
 * \param dev   The device to mount.
 * \param event The event used to notify the end of the operation. See the documentation of rt_event_t for more details.
 * \return 0 if successful, 1 otherwise.
 */
int rt_dev_mount(rt_dev_t *dev, rt_event_t *event);

/** \brief Unmount a device
 *
 * This operation will make the device unaccessible and release all the resources which are not needed anymore.
 *
 * \param dev   The device to unmount.
 * \param event The event used to notify the end of the operation. See the documentation of rt_event_t for more details.
 */
void rt_dev_unmount(rt_dev_t *dev, rt_event_t *event);

//!@}

/**        
 * @} end of Device      
 */



/// @cond IMPLEM

/// @endcond

/// @endcond

#endif
