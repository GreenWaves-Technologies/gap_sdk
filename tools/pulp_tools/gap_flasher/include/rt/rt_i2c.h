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

#ifndef __RT__RT_API_I2C_H__
#define __RT__RT_API_I2C_H__

#include "rt/data/rt_data_i2c.h"

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup I2C I2C driver
 *
 * The I2C driver provides support for transferring data between an external I2C device
 * and the chip running this driver.
 *
 */

/**
 * @addtogroup I2C
 * @{
 */

/**@{*/

/// @cond IMPLEM

#define __RT_I2C_CTRL_SET_MAX_BAUDRATE_BIT 0

/// @endcond

/** \struct rt_i2c_conf_t
 * \brief I2C master configuration structure.
 *
 * This structure is used to pass the desired I2C configuration to the runtime when opening a device.
 */

typedef struct{
    signed char id;               /*!< If it is different from -1, this specifies on which I2C interface the device is connected. */
    signed char cs;               /*!< i2c slave address (7 bits on MSB), the runtime will take care of the LSB of READ and Write. */
    unsigned int  max_baudrate;     /*!< Maximum baudrate for the I2C bitstream which can be used with the opened device . */
} rt_i2c_conf_t;



typedef enum {
  RT_I2C_CTRL_SET_MAX_BAUDRATE  = 1 << __RT_I2C_CTRL_SET_MAX_BAUDRATE_BIT, /*!< Change maximum baudrate. */
} rt_i2c_control_e;



/** \brief Open an I2C device.
 *
 * This function must be called before the I2C device can be used. It will do all the needed configuration to make it
 * usable and also return a handle used to refer to this opened device when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param dev_name  The device name in case the connection must be configured automatically. This name should correspond to the one used to configure the devices managed by the runtime. If it is NULL, all connection information must be provided in the configuration structure (interface id and chip select).
 * \param conf      A pointer to the I2C configuration. Can be NULL to take default configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the device and which can be used with other functions.
 */
rt_i2c_t *rt_i2c_open(char *dev_name, rt_i2c_conf_t *conf, rt_event_t *event);

/** \brief Initialize an I2C configuration with default values.
 *
 * This function can be called to get default values for all parameters before setting some of them.
 * The structure containing the configuration must be kept alive until the I2C device is opened.
 *
 * \param conf A pointer to the I2C configuration.
 */
void rt_i2c_conf_init(rt_i2c_conf_t *conf);

/** \brief Close an opened I2C device.
 *
 * This function can be called to close an opened I2C device once it is not needed anymore, in order to free all allocated resources. Once this function is called, the device is not accessible anymore and must be opened again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param handle    The handler of the device which was returned when the device was opened.
 * \param event     The event used for managing termination.
 */
void rt_i2c_close (rt_i2c_t *handle, rt_event_t *event);

/** \brief Dynamically change the device configuration.
 *
 * This function can be called to change part of the device configuration after it has been opened.
 *
 * \param handle    The handle of the I2C device which was returned when the device was opened.
 * \param cmd       The command which specifies which parameters of the driver to modify and for some of them also their values.
 * \param arg       An additional value which is required for some parameters when they are set.
 */
static inline void rt_i2c_control(rt_i2c_t *handle, rt_i2c_control_e cmd, uint32_t arg);

/** \brief Enqueue a burst read copy from the I2C (from I2C device to chip).
 *
 * This function can be used to read at least 1 byte of data from the I2C device.
 * The copy will make an asynchronous transfer between the I2C and one of the chip memory.
 * An event can be specified in order to be notified when the transfer is finished.
 *
 * \param handle        The handler of the device which was returned when the device was opened.
 * \param rx_buff       The address in the chip where the received data must be written.
 * \param length        The size in bytes of the copy
 * \param xfer_pending  If 1, the stop bit is not generated so that the same transfer can be continued with another call to rt_i2c_read or rt_i2c_write 
 * \param event         The event used for managing termination.
 */
void rt_i2c_read(rt_i2c_t *handle, unsigned char *rx_buff, int length, int xfer_pending, rt_event_t *event);

/** \brief Enqueue a burst write copy to the I2C (from chip to I2C device).
 *
 * This function can be used to write at least 1 byte of data to the I2C device.
 * The copy will make an asynchronous transfer between the I2C and one of the chip memory.
 * An event can be specified in order to be notified when the transfer is finished.
 *
 * \param handle        The handler of the device which was returned when the device was opened.
 * \param tx_data       The address in the chip where the data to be sent.
 * \param length        The size in bytes of the copy
 * \param xfer_pending  If 1, the stop bit is not generated so that the same transfer can be continued with another call to rt_i2c_read or rt_i2c_write 
 * \param event         The event used for managing termination.
 */
void rt_i2c_write(rt_i2c_t *handle, unsigned char *tx_data, int length, int xfer_pending, rt_event_t *event);


/** \brief Enqueue a burst write copy to the I2C without START bit and address (from chip to I2C device).
 *
 * This function can be used to write at least 1 byte of data to the I2C device.
 * The copy will make an asynchronous transfer between the I2C and one of the chip memory.
 * Compared to rt_i2c_write, the START bit is not generated and the slave
 * address is not sent.
 * An event can be specified in order to be notified when the transfer is finished.
 *
 * \param handle        The handler of the device which was returned when the device was opened.
 * \param tx_data       The address in the chip where the data to be sent.
 * \param length        The size in bytes of the copy
 * \param xfer_pending  If 1, the stop bit is not generated so that the same transfer can be continued with another call to rt_i2c_read or rt_i2c_write 
 * \param event         The event used for managing termination.
 */
void rt_i2c_write_append(rt_i2c_t *handle, unsigned char *tx_data, int length, int xfer_pending, rt_event_t *event);

//!@}

/**
 * @}
 */



/// @cond IMPLEM

void __rt_i2c_control(rt_i2c_t *handle, rt_i2c_control_e cmd, uint32_t arg);

static inline void rt_i2c_control(rt_i2c_t *handle, rt_i2c_control_e cmd, uint32_t arg)
{
  __rt_i2c_control(handle, cmd, arg);
}

/// @endcond


#endif


