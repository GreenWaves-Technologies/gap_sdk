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

#ifndef __PI_DRIVERS_I3C_H__
#define __PI_DRIVERS_I3C_H__

#include "pmsis/pmsis_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup I3C I3C
 *
 * The I3C driver provides support for transferring data between an external
 * I3C device and the chip running this driver.
 *
 */

/**
 * @addtogroup I3C
 * @{
 */

/**@{*/

/// @cond IMPLEM

#define __PI_I3C_CTRL_SET_MAX_BAUDRATE_BIT 0

/// @endcond

/**
 * \struct pi_i3c_conf
 * \brief I3C master configuration structure.
 *
 * This structure is used to pass the desired I3C configuration to the runtime
 * when opening a device.
 */
typedef struct pi_i3c_conf
{
    pi_device_e device;  /* Device type. */
    uint8_t itf;            /*!< Specifies on which I3C interface the device is
      connected. */
    uint16_t cs;               /*!< i3c slave address (7 bits), the
      runtime will take care of the LSB of read and write. */
    uint32_t max_baudrate;   /*!< Maximum baudrate for the I3C bitstream which
      can be used with the opened device . */
} pi_i3c_conf_t;



/** \enum pi_i3c_ioctl_e
 * \brief Commands for pi_i3c_control.
 *
 * This is used to tell which command to execute through pi_i3c_control.
 */
typedef enum
{
    /**
     * \brief Change maximum baudrate.
     *
     * \param baudrate   Max baudrate.
     */
    PI_I3C_CTRL_SET_MAX_BAUDRATE  = 1 << __PI_I3C_CTRL_SET_MAX_BAUDRATE_BIT
} pi_i3c_ioctl_e;

/** \brief Initialize an I3C configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them. The structure containing the configuration must be
 * kept alive until the I3C device is opened.
 *
 * \param conf A pointer to the I3C configuration.
 */
void pi_i3c_conf_init(pi_i3c_conf_t *conf);


/**
 * \brief set slave addr in conf.
 *
 * \param conf           A pointer to the I3C configuration.
 * \param slave_addr     Address of the slave device.
 */
void pi_i3c_conf_set_slave_addr(struct pi_i3c_conf *conf, uint16_t slave_addr);
/** \brief Open an I3C device.
 *
 * This function must be called before the Hyperbus device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_i3c_open(struct pi_device *device);

/** \brief Close an opened I3C device.
 *
 * This function can be called to close an opened I3C device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_i3c_close (struct pi_device *device);

/** \brief Dynamically change the device configuration.
 *
 * This function can be called to change part of the device configuration after
 * it has been opened.
 *
 * \param device  A pointer to the structure describing the device.
 * \param cmd     The command which specifies which parameters of the driver to
 * modify and for some of them also their values. The command must be one of
 * those defined in pi_i3c_ioctl_e.
 * \param arg       An additional value which is required for some parameters
 * when they are set.
 */
void pi_i3c_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/** \brief Enqueue a burst read copy from the I3C (from I3C device to chip).
 *
 * This function can be used to read at least 1 byte of data from the I3C
 * device. The copy will make a synchronous transfer between the I3C and one of
 * the chip memory.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param rx_buff The address in the chip where the received data must be
 *   written.
 * \param length  The size in bytes of the copy.
 *
 * \return          PI_OK if request is successful
 *                  PI_ERR_I3C_NACK if a NACK was received during the request
 */
int pi_i3c_read(struct pi_device *device, uint8_t *rx_buff, int length);

/** \brief Enqueue a burst write copy to the I3C (from chip to I3C device).
 *
 * This function can be used to write at least 1 byte of data to the I3C device.
 * The copy will make a synchronous transfer between the I3C and one of the
 * chip memory.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param tx_data     The address in the chip where the data to be sent is read.
 * \param length      The size in bytes of the copy.
 *
 * \return          PI_OK if request is successful
 *                  PI_ERR_I3C_NACK if a NACK was received during the request
 */
int pi_i3c_write(struct pi_device *device, uint8_t *tx_data, int length);

/** \brief Enqueue an asynchronous burst read copy from the I3C (from I3C
 * device to chip).
 *
 * This function can be used to read at least 1 byte of data from the I3C
 * device.
 * The copy will make an asynchronous transfer between the I3C and one of the
 * chip memory.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * The task will contain the request status (success or error). It should be
 * retrieved using the pi_i3c_get_request_status function.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param rx_buff  The address in the chip where the received data must be
 *   written.
 * \param length   The size in bytes of the copy.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
void pi_i3c_read_async(struct pi_device *device, uint8_t *rx_buff, int length,
                       pi_task_t *task);

/** \brief Enqueue a burst write copy to the I3C (from chip to I3C device).
 *
 * This function can be used to write at least 1 byte of data to the I3C device.
 * The copy will make an asynchronous transfer between the I3C and one of the
 * chip memory.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * The task will contain the request status (success or error). It should be
 * retrieved using the pi_i3c_get_request_status function.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param tx_data   The address in the chip where the data to be sent is read.
 * \param length    The size in bytes of the copy
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
void pi_i3c_write_async(struct pi_device *device, uint8_t *tx_data, int length,
                        pi_task_t *task);

/**
 * \brief Scan i3c bus to detect a dev
 *
 * This function can be used to detect if a device is connected to the i3c bus.
 * The pi_i3c_conf_t structure is used to pass the address of the device
 * to look for, with different baudrate if needed.
 *
 * \param device         Pointer to device structure.
 * \param conf           Conf struct holding address and baudrate.
 * \param rx_data        Pointer to 1 Byte buffer to store data.
 *
 * \retval 0x00          If a device has been detected at given address.
 * \retval 0xFF          Otherwise.
 */
int pi_i3c_detect(struct pi_device *device, struct pi_i3c_conf *conf, uint8_t *rx_data);

//!@}

/**
 * @}
 */



#ifdef __cplusplus
}
#endif
#endif  /* __PI_DRIVERS_I3C_H__ */
