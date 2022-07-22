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
 * @defgroup FFC FFC
 *
 * The FFC (Fixed point/Floating point Converter) driver provides an
 * interface to use the FFC IP.
 */

/**
 * @addtogroup FFC
 * @{
 */

/**@{*/

/**
 * \enum pi_ffc_float_type_e
 */
typedef enum {
    PI_FFC_FLOAT_FP16  = 0,
    PI_FFC_FLOAT_BFP16 = 1,
    PI_FFC_FLOAT_FP32  = 3,
} pi_ffc_float_type_e;

/**
 * \enum pi_ffc_fixed_type_e
 */
typedef enum {
    PI_FFC_FIXED_8  = 0,
    PI_FFC_FIXED_16 = 1,
    PI_FFC_FIXED_24 = 2,
    PI_FFC_FIXED_32 = 3,
} pi_ffc_fixed_type_e;

/**
 * \enum pi_ffc_mode_e
 */
typedef enum {
    PI_FFC_FLOAT_TO_FIXED = 0,
    PI_FFC_FIXED_TO_FLOAT = 1,
} pi_ffc_mode_e;

/**
 * \enum pi_ffc_io_mode_e
 */
typedef enum {
    PI_FFC_MEMORY_IN_MEMORY_OUT = 0,
    PI_FFC_STREAM_IN_MEMORY_OUT = 1,
    PI_FFC_MEMORY_IN_STREAM_OUT = 2,
    PI_FFC_STREAM_IN_STREAM_OUT = 3,
} pi_ffc_io_mode_e;

/**
 * \brief FFC configuration structure.
 *
 * This structure is used to pass the desired FFC configuration to the runtime
 * when opening a device.
 *
 */
typedef struct pi_ffc_conf
{
    int8_t itf;                     /*!< FFC device number */
    pi_ffc_fixed_type_e fixed_type; /*!< FFC fixed point data type (8bits, 16bits, 24bits, 32bits)*/
    uint32_t fixed_scale;           /*!< Scale number of Fixed point: Fixed Point = -1^Sign * (Q(Size-Precision).Precision << Scale)  */
    uint32_t fixed_precision;       /*!< Precision size fraction: Q(Size-precision).precision */
    pi_ffc_float_type_e float_type; /*!< FFC floating point data type (8bits, 16bits, 32bits)*/

    pi_ffc_mode_e mode;             /*!< FFC mode: float to fixed, or fixed to float */
    pi_ffc_io_mode_e io_mode;       /*!< FFC IO mode: Mem In/out, stream In/Out */
} pi_ffc_conf_t;

/**
 * \brief Initialize an FFC configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them. The structure containing the configuration must be
 * kept alive until the FFC device is opened.
 *
 * \param conf A pointer to the FFC configuration.
 */
void pi_ffc_conf_init(pi_ffc_conf_t *conf);

/**
 * \brief Open an FFC device.
 *
 * This function must be called before the FFC device can be used.
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
int pi_ffc_open(pi_device_t *device);

/**
 * \brief Close an opened FFC device.
 *
 * This function can be called to close an opened FFC device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_ffc_close(pi_device_t *device);

/**
 * \enum pi_ffc_ioctl_e
 *
 * \brief Commands for pi_ffc_ioctl.
 */
typedef enum
{
    /**
     * \brief Set the IO mode
     *
     * This command can be used to change the IO mode
     */
    PI_FFC_IOCTL_SET_IO_MODE,
    /**
     * \brief Set the continuous mode
     *
     * This command can be used to change the continuous operating mode
     * - 1 enable continuous, ie ffc will continuously convert data
     * - 0 stops continuous mode. If a transfer is in progress it is stopped
     *   immediately.
     * \warning if continuous mode is enabled with io mode
     *          PI_FFC_STREAM_IN_STREAM_OUT there is no way to know
     *          when transfers are done.
     */
    PI_FFC_IOCTL_CONTINUOUS_ENABLE,
} pi_ffc_ioctl_e;

/**
 * \brief FFC IOCTL function
 *
 * \param device A pi device structure pointing to FFC device
 * \param cmd ioctl number
 * \param arg argument to be passed to ioctl
 *
 * \return PI_OK if operation successfull, else error code
 */
int32_t pi_ffc_ioctl(pi_device_t *device, uint32_t cmd, void *arg);

/**
 * \brief TODO
 */
void pi_ffc_convert(pi_device_t *device, void* src, void* dst, uint16_t size);

/**
 * \brief TODO
 */
void pi_ffc_convert_async(pi_device_t *device, void* src, void* dst,
                          uint16_t size, pi_task_t* task);

//!@}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
