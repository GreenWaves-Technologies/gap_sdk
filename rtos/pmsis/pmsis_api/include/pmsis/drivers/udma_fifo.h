/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#pragma once
#include "pmsis/pmsis_types.h"

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup UDMA_FIFO UDMA_FIFO
 *
 * The UDMA FIFO driver provides an interface to use UDMA FIFO channel.
 *
 */

/**
 * @addtogroup UDMA_FIFO
 * @{
 */

/**@{*/

/**
 * \struct pi_udma_fifo_conf_t
 *
 * \brief UDMA FIFO configuration structure.
 *
 * This structure is used to pass the desired UDMA FIFO configuration to the runtime
 * when opening a device.
 */
typedef struct pi_udma_fifo_conf
{
    pi_device_e device;  /*!< Device type. */
    uint32_t size;       /*!< size of the FIFO in bytes, minimum value of 16 */
} pi_udma_fifo_conf_t;

/**
 * \enum pi_udma_fifo_ioctl_e
 *
 * \brief Commands for pi_udma_fifo_ioctl.
 *
 * This is used to tell which command to execute through pi_udma_fifo_ioctl.
 * Parameters are passed as pointers.
 */
typedef enum {
    PI_UDMA_FIFO_GET_ID,              /*!< type uint32_t: return the fifo id */
    PI_UDMA_FIFO_SET_SIZE,            /*!< set the FIFO size (in bytes, minimum 16) */
    PI_UDMA_FIFO_GET_LEVEL,           /*!< get current number of bytes in FIFO */
    PI_UDMA_FIFO_SET_EVENT_THRESHOLD, /*!< type uint32_t: set the number of bytes after which the
                                           event will be triggered. Setting a value greater than 0
                                           enable the event, setting 0 disables it */
    PI_UDMA_FIFO_SET_EVENT_CB,        /*!< type pi_task_t: set the event callback,
                                                           use NULL for no callback */
    PI_UDMA_FIFO_PUSH8,               /*!< type uint8_t: push 8 bits in the FIFO */
    PI_UDMA_FIFO_PUSH16,              /*!< type uint16_t: push 16 bits in the FIFO */
    PI_UDMA_FIFO_PUSH24,              /*!< type uint32_t: push 24 bits in the FIFO */
    PI_UDMA_FIFO_PUSH32,              /*!< type uint32_t: push 32 bits in the FIFO */
    PI_UDMA_FIFO_POP8,                /*!< type uint8_t: pop 8 bits from the FIFO */
    PI_UDMA_FIFO_POP16,               /*!< type uint16_t: pop 16 bits from the FIFO */
    PI_UDMA_FIFO_POP24,               /*!< type uint32_t: pop 24 bits from the FIFO */
    PI_UDMA_FIFO_POP32,               /*!< type uint32_t: pop 32 bits from the FIFO */
} pi_udma_fifo_ioctl_e;

/**
 * \brief Initialize an UDMA FIFO configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them. The structure containing the configuration must be
 * kept alive until the UDMA FIFO device is opened.
 *
 * \param conf A pointer to the UDMA FIFO configuration.
 */
void pi_udma_fifo_conf_init(pi_udma_fifo_conf_t *conf);

/**
 * \brief Open an UDMA FIFO device.
 *
 * This function must be called before the UDMA FIFO device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 *
 * \warning The fifo needs at least one UDMA peripheral enabled to be enabled.
 *          Else configuration will not be set, and application will get stuck on
 *          push/pop commands.
 */
int pi_udma_fifo_open(pi_device_t *device);

/**
 * \brief Close an opened UDMA FIFO device.
 *
 * This function can be called to close an opened UDMA FIFO device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_udma_fifo_close(pi_device_t *device);

/**
 * \brief UDMA FIFO IOCTL function
 *
 * \param device A pi device structure pointing to UDMA FIFO device
 * \param cmd ioctl number
 * \param arg argument to be passed to ioctl
 */
void pi_udma_fifo_ioctl(pi_device_t *device, uint32_t cmd, void *arg);

//!@}

/**
 * @}
 */
