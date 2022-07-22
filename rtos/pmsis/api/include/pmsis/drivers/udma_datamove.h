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

#ifdef __cplusplus
extern "C" {
#endif

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup UDMA_DATAMOVE UDMA_DATAMOVE
 *
 * The UDMA DATAMOVE driver provides an interface to use UDMA DATAMOVE channel.
 *
 */

/**
 * @addtogroup UDMA_DATAMOVE
 * @{
 */

/**@{*/

/**
 * \enum pi_udma_datamove_transf_type_e
 *
 * \brief type of transfer
 */
typedef enum
{
    PI_UDMA_DATAMOVE_TRF_LINEAR,
    PI_UDMA_DATAMOVE_TRF_2D,
} pi_udma_datamove_transf_type_e;

typedef struct pi_udma_datamove_transf_cfg_s
{
    pi_udma_datamove_transf_type_e type;
    uint32_t row_len;
    uint32_t stride;
} pi_udma_datamove_transf_cfg_t;

/**
 * \brief UDMA DATAMOVE configuration structure.
 *
 * This structure is used to pass the desired UDMA DATAMOVE configuration to the runtime
 * when opening a device.
 */
typedef struct pi_udma_datamove_conf_s
{
    uint8_t device_id;                         /*!< device id */
    pi_udma_datamove_transf_cfg_t src_trf_cfg; /*!< Source data transfer configuration */
    pi_udma_datamove_transf_cfg_t dst_trf_cfg; /*!< Destination data transfer configuration */
} pi_udma_datamove_conf_t;

/**
 * \brief Initialize an UDMA DATAMOVE configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them. The structure containing the configuration must be
 * kept alive until the UDMA DATAMOVE device is opened.
 *
 * \param conf A pointer to the UDMA DATAMOVE configuration.
 */
void pi_udma_datamove_conf_init(pi_udma_datamove_conf_t *conf);

/**
 * \brief Open an UDMA DATAMOVE device.
 *
 * This function must be called before the UDMA DATAMOVE device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 *
 * \warning The datamove needs at least one UDMA peripheral enabled to be enabled.
 *          Else configuration will not be set, and application will get stuck on
 *          push/pop commands.
 */
int pi_udma_datamove_open(pi_device_t *device);

/**
 * \brief Close an opened UDMA DATAMOVE device.
 *
 * This function can be called to close an opened UDMA DATAMOVE device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_udma_datamove_close(pi_device_t *device);

/**
 * \brief copy data from L2 to L2 (blocking)
 *
 * \param device udma_datamove device pointer
 * \param src source address
 * \param dst destination address
 * \param len length of the transfer in bytes
 *
 * \return PI_OK if operation succeeded, else error code
 */
int32_t pi_udma_datamove_copy(pi_device_t *device, void* src, void* dst, uint32_t len);

/**
 * \brief copy data from L2 to L2 (asynchronous)
 *
 * \param device udma_datamove device pointer
 * \param src source address
 * \param dst destination address
 * \param len length of the transfer in bytes
 * \param task task executed at the end of transfer
 *
 * \return PI_OK if operation succeeded, else error code
 */
int32_t pi_udma_datamove_copy_async(pi_device_t *device, void* src, void* dst, uint32_t len, pi_task_t* task);

//!@}

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
