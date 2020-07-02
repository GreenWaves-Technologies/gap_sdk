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

#pragma once
#include "pmsis/pmsis_types.h"

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup I2C_SLAVE I2C_SLAVE
 *
 * The I2C driver provides support for transferring data between an external
 * I2C device and the chip running this driver.
 *
 */

/**
 * @addtogroup I2C_SLVA
 * @{
 */

/**@{*/

/// @cond IMPLEM

#define __PI_I2C_SLAVE_CTRL_SET_MAX_BAUDRATE_BIT 0

/// @endcond


typedef struct pi_i2c_slave_args
{
    uint16_t slave_addr;
    int ret; // set by the app
    void *l2_buffer;
    uint32_t nb_bytes;
    void *handle;
    uint8_t itf_id;
} pi_i2c_slave_args_t;

typedef void (*pi_i2c_callback_t)(pi_i2c_slave_args_t *);

/** \struct pi_i2c_slave_conf_t
 * \brief I2C master configuration structure.
 *
 * This structure is used to pass the desired I2C Slave configuration to the runtime
 * when opening a device.
 */
typedef struct pi_i2c_slave_conf
{
    pi_device_e device;  /* Device type. */
    uint8_t itf;         /*!< Specifies on which I2C interface the device is
      connected. */
    uint8_t sof0;        /*!< Specifies whether addr0 triggers irq on START */
    uint8_t eof0;        /*!< Specifies whether addr1 triggers irq on STOP */
    uint8_t sof1;        /*!< Specifies whether addr1 triggers irq on START */
    uint8_t eof1;        /*!< Specifies whether addr1 triggers irq on STOP */
    uint8_t addr0_10_bit;/*!< Specifies whether addr0 is 10 bits */
    uint8_t addr1_10_bit;/*!< Specifies whether addr1 is 10 bits */
    uint8_t mask0;       /*!< Specifies addr matching mask for addr0 */
    uint8_t mask1;       /*!< Specifies addr matching mask for addr1 */
    uint16_t addr0;      /*!< Addr0 to which itf match */
    uint16_t addr1;      /*!< Addr1 to which itf match */
    uint32_t max_baudrate;   /*!< Maximum baudrate for the I2C bitstream which
      can be used with the opened device . */
    pi_i2c_callback_t rx_callback; /*!< callback for rx transfers*/
    pi_i2c_callback_t tx_callback; /*!< callback for tx transfers*/
} pi_i2c_slave_conf_t;

/** \enum pi_i2c_ioctl_e
 * \brief Commands for pi_i2c_control.
 *
 * This is used to tell which command to execute through pi_i2c_control.
 */
typedef enum {
  PI_I2C_SLAVE_CTRL_SET_MAX_BAUDRATE  = 1 << __PI_I2C_SLAVE_CTRL_SET_MAX_BAUDRATE_BIT, /*!<
    Change maximum baudrate. */
} pi_i2c_slave_ioctl_e;

/** \brief Initialize an I2C Slave configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them. The structure containing the configuration must be
 * kept alive until the I2C Slave device is opened.
 *
 * \param conf A pointer to the I2C Slave configuration.
 */
void pi_i2c_slave_conf_init(pi_i2c_slave_conf_t *conf);


/** \brief set gap9 slave addr0 in conf.
 *
 * \param conf A pointer to the I2C slave configuration.
 * \param addr addr0 for gap slave mode
 * \param mask choose a match mask
 * \param is_10_bit if 1, addr is 10 bit
 */
void pi_i2c_slave_conf_set_addr0(struct pi_i2c_slave_conf *conf, uint16_t addr,
        uint8_t mask, uint8_t is_10_bit, uint8_t eof, uint8_t sof);

/** \brief set gap9 slave addr1 in conf.
 *
 * \param conf A pointer to the I2C slave configuration.
 * \param addr addr1 for gap slave mode
 * \param mask choose a match mask
 * \param is_10_bit if 1, addr is 10 bit
 */
void pi_i2c_slave_conf_set_addr1(struct pi_i2c_slave_conf *conf, uint16_t addr,
        uint8_t mask, uint8_t is_10_bit, uint8_t eof, uint8_t sof);

/** \brief Open an I2C slave device.
 *
 * This function must be called before the I2C slave device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_i2c_slave_open(struct pi_device *device);

/** \brief Close an opened I2C slave device.
 *
 * This function can be called to close an opened I2C slave device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_i2c_slave_close(struct pi_device *device);

/** \brief Dynamically change the device configuration.
 *
 * This function can be called to change part of the device configuration after
 * it has been opened.
 *
 * \param device  A pointer to the structure describing the device.
 * \param cmd     The command which specifies which parameters of the driver to
 * modify and for some of them also their values. The command must be one of
 * those defined in pi_i2c_ioctl_e.
 * \param arg       An additional value which is required for some parameters
 * when they are set.
 */


/** \brief set rx channel registers for I2C slave
 *
 * \param handle A pointer to the low level handle passed by driver in slave
 * irq context.
 * \param l2_addr L2 address for the rx buffer
 * \param size size of the rx buffer
 */
void pi_i2c_slave_set_rx(void *handle,
        void *l2_addr, uint32_t size);

/** \brief set tx channel registers for I2C slave
 *
 * \param handle A pointer to the low level handle passed by driver in slave
 * irq context.
 * \param l2_addr L2 address for the tx buffer
 * \param size size of the tx buffer
 */
void pi_i2c_slave_set_tx(void *handle,
        void *l2_addr, uint32_t size);

/** \brief Flush & unlock I2C slave bus (need to stop transfers before)
 *
 * \param handle A pointer to the low level handle passed by driver in slave
 * irq context.
 * \param is_rd if 1 flush for rx transfer, else for tx
 */
void pi_i2c_slave_unlock(void *handle, int is_rd);

/** \brief I2C slave IOCTL function
 *
 * \param device A pi device structure pointing to I2C slave device
 * \param cmd ioctl number
 * \param arg argument to be passed to ioctl
 */
void pi_i2c_slave_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/** \brief Stop transfer on I2C slave rx channel
 *
 * \param handle A pointer to the low level handle passed by driver in slave
 * irq context.
 */
void pi_i2c_slave_stop_rx(void *handle);

/** \brief Stop transfer on I2C slave tx channel
 *
 * \param handle A pointer to the low level handle passed by driver in slave
 * irq context.
 */
void pi_i2c_slave_stop_tx(void *handle);
//!@}

/**
 * @}
 */
