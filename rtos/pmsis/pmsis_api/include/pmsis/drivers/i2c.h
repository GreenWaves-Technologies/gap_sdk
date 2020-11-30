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

#ifndef __PI_DRIVERS_I2C_H__
#define __PI_DRIVERS_I2C_H__

#include "pmsis/pmsis_types.h"

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup I2C I2C
 *
 * The I2C driver provides support for transferring data between an external
 * I2C device and the chip running this driver.
 *
 */

/**
 * @addtogroup I2C
 * @{
 */

/**@{*/

/// @cond IMPLEM

#define __PI_I2C_CTRL_SET_MAX_BAUDRATE_BIT 0

/// @endcond

/**
 * \struct pi_i2c_conf_t
 * \brief I2C master configuration structure.
 *
 * This structure is used to pass the desired I2C configuration to the runtime
 * when opening a device.
 */
typedef struct pi_i2c_conf
{
    pi_device_e device;  /* Device type. */
    uint8_t itf;            /*!< Specifies on which I2C interface the device is
      connected. */
    uint16_t cs;               /*!< i2c slave address (7 or 10 bits), the
      runtime will take care of the LSB of read and write. */
    int8_t is_10_bits;
    uint16_t wait_cycles;
    uint32_t max_baudrate;   /*!< Maximum baudrate for the I2C bitstream which
      can be used with the opened device . */
} pi_i2c_conf_t;


/** \enum pi_i2c_xfer_flags_e
 * \brief Properties for I2C transfers
 *
 * This is used to specify additional behaviors when transfering data through
 * I2C.
 */
typedef enum {
  PI_I2C_XFER_STOP       = 0<<0, /*!< Generate a STOP bit at the end of the
    transfer. */
  PI_I2C_XFER_NO_STOP    = 1<<0, /*!< Don't generate a STOP bit at the end of
    the transfer. */
  PI_I2C_XFER_START      = 0<<1, /*!< Generate a START bit at the beginning of
    the transfer. */
  PI_I2C_XFER_NO_START   = 1<<1, /*!< Don't generate a START bit at the
    beginning of the transfer. */
  PI_I2C_XFER_RESTART    = 1<<2, /*!< Generate a RESTART bit at the beginning of
    the transfer. */
  PI_I2C_XFER_NO_RESTART = 0<<2  /*!< Don't generate a RESTART bit at the
    beginning of the transfer. */
} pi_i2c_xfer_flags_e;


/** \enum pi_i2c_ioctl_e
 * \brief Commands for pi_i2c_control.
 *
 * This is used to tell which command to execute through pi_i2c_control.
 */
typedef enum
{
    /**
     * \brief Change maximum baudrate.
     *
     * \param baudrate   Max baudrate.
     */
    PI_I2C_CTRL_SET_MAX_BAUDRATE  = 1 << __PI_I2C_CTRL_SET_MAX_BAUDRATE_BIT,
    /**
     * \brief Abort RX transfer.
     *
     * Abort current RX transfert.
     */
    PI_I2C_IOCTL_ABORT_RX   = 6,
    /**
     * \brief Abort TX transfer.
     *
     * Abort current TX transfert.
     */
    PI_I2C_IOCTL_ABORT_TX   = 7,
    /**
     * \brief Attach UDMA timer.
     *
     * This command attaches a UDMA timer channel to UDMA reception channel.
     *
     * \param timeout_id UDMA timeout channel ID.
     */
    PI_I2C_IOCTL_ATTACH_TIMEOUT_RX = 8,
    /**
     * \brief Detach UDMA timer.
     *
     * This command removes a UDMA timer channel attached to UDMA reception channel.
     *
     * \param timeout_id UDMA timeout channel ID.
     */
    PI_I2C_IOCTL_DETACH_TIMEOUT_RX = 9,
    /**
     * \brief Attach UDMA timer.
     *
     * This command attaches a UDMA timer channel to UDMA transmission channel.
     *
     * \param timeout_id UDMA timeout channel ID.
     */
    PI_I2C_IOCTL_ATTACH_TIMEOUT_TX = 10,
    /**
     * \brief Detach UDMA timer.
     *
     * This command removes a UDMA timer channel attached to UDMA transmission channel.
     *
     * \param timeout_id UDMA timeout channel ID.
     */
    PI_I2C_IOCTL_DETACH_TIMEOUT_TX = 11
} pi_i2c_ioctl_e;

/** \brief Initialize an I2C configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them. The structure containing the configuration must be
 * kept alive until the I2C device is opened.
 *
 * \param conf A pointer to the I2C configuration.
 */
void pi_i2c_conf_init(pi_i2c_conf_t *conf);


/**
 * \brief set slave addr in conf.
 *
 * \param conf           A pointer to the I2C configuration.
 * \param slave_addr     Address of the slave device.
 * \param is_10_bits     Indicate if slave address is 7 bits or 10 bits.
 */
void pi_i2c_conf_set_slave_addr(struct pi_i2c_conf *conf, uint16_t slave_addr,
                                int8_t is_10_bits);

/** \brief set wait_cycles in conf.
 *
 * \param conf A pointer to the I2C configuration.
 * \param wait_cycles Number of wait cycles applied at the end of transfer
 */
void pi_i2c_conf_set_wait_cycles(struct pi_i2c_conf *conf, uint16_t wait_cycles);

/** \brief Open an I2C device.
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
int pi_i2c_open(struct pi_device *device);

/** \brief Close an opened I2C device.
 *
 * This function can be called to close an opened I2C device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_i2c_close (struct pi_device *device);

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
void pi_i2c_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/** \brief Enqueue a burst read copy from the I2C (from I2C device to chip).
 *
 * This function can be used to read at least 1 byte of data from the I2C
 * device. The copy will make a synchronous transfer between the I2C and one of
 * the chip memory.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param rx_buff The address in the chip where the received data must be
 *   written.
 * \param length  The size in bytes of the copy.
 * \param flags  Specify additional transfer behaviors like start and stop bits
 *   management.
 *
 * \return          PI_OK if request is successful
 *                  PI_ERR_I2C_NACK if a NACK was received during the request
 */
int pi_i2c_read(struct pi_device *device, uint8_t *rx_buff, int length,
  pi_i2c_xfer_flags_e flags);

/** \brief Enqueue a burst write copy to the I2C (from chip to I2C device).
 *
 * This function can be used to write at least 1 byte of data to the I2C device.
 * The copy will make a synchronous transfer between the I2C and one of the
 * chip memory.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param tx_data     The address in the chip where the data to be sent is read.
 * \param length      The size in bytes of the copy.
 * \param flags  Specify additional transfer behaviors like start and stop bits
 *   management.
 *
 * \return          PI_OK if request is successful
 *                  PI_ERR_I2C_NACK if a NACK was received during the request
 */
int pi_i2c_write(struct pi_device *device, uint8_t *tx_data, int length,
  pi_i2c_xfer_flags_e flags);


void pi_i2c_write_read(struct pi_device *device, void *tx_buffer,
        void *rx_buffer, uint32_t tx_size, uint32_t rx_size);

void pi_i2c_write_dual(struct pi_device *device, void *tx_buffer0,
        void *tx_buffer1, uint32_t tx_size0, uint32_t tx_size1);

/** \brief Enqueue an asynchronous burst read copy from the I2C (from I2C
 * device to chip).
 *
 * This function can be used to read at least 1 byte of data from the I2C
 * device.
 * The copy will make an asynchronous transfer between the I2C and one of the
 * chip memory.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * The task will contain the request status (success or error). It should be
 * retrieved using the pi_i2c_get_request_status function.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param rx_buff  The address in the chip where the received data must be
 *   written.
 * \param length   The size in bytes of the copy.
 * \param flags  Specify additional transfer behaviors like start and stop
 *   bits management.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
void pi_i2c_read_async(struct pi_device *device, uint8_t *rx_buff, int length,
  pi_i2c_xfer_flags_e flags, pi_task_t *task);

/** \brief Enqueue a burst write copy to the I2C (from chip to I2C device).
 *
 * This function can be used to write at least 1 byte of data to the I2C device.
 * The copy will make an asynchronous transfer between the I2C and one of the
 * chip memory.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * The task will contain the request status (success or error). It should be
 * retrieved using the pi_i2c_get_request_status function.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param tx_data   The address in the chip where the data to be sent is read.
 * \param length    The size in bytes of the copy
 * \param flags  Specify additional transfer behaviors like start and stop bits
 *   management.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.
 */
void pi_i2c_write_async(struct pi_device *device, uint8_t *tx_data, int length,
  pi_i2c_xfer_flags_e flags, pi_task_t *task);

void pi_i2c_write_read_async(struct pi_device *device, void *tx_buffer,
        void *rx_buffer, uint32_t tx_size, uint32_t rx_size, pi_task_t *callback);

void pi_i2c_write_dual_async(struct pi_device *device, void *tx_buffer0,
        void *tx_buffer1, uint32_t tx_size0, uint32_t tx_size1, pi_task_t *callback);

/**
 * \brief Enqueue an asynchronous burst read copy from the I2C (from I2C
 * device to chip), with timeout.
 *
 * This function has the same behaviour as pi_i2c_read(), with timeout.
 * This timeout value is used to abort/cancel a transfer when timeout is reached.
 *
 * \param device         Pointer to the structure describing the device.
 * \param rx_buff        Address in the chip where the received data must be written.
 * \param length         Size in bytes of the copy.
 * \param flags          Specify additional transfer behaviors like start and stop
 *                       bits management.
 * \param timeout_us     Timeout value in us.
 *
 * \note To use this feature, a UDMA timeout channel must be allocated before a
 *       call to this functions :
 *       * pi_udma_timeout_alloc()
 *       * pi_i2c_ioctl()   //PI_I2C_IOCTL_ATTACH_TIMEOUT_RX
 *
 * \note To use this feature asynchronously, proceed as follows :
 *       * pi_udma_timeout_alloc()
 *       * pi_i2c_ioctl()   //PI_I2C_IOCTL_ATTACH_TIMEOUT_RX
 *       * pi_task_timeout_set()
 *       * pi_i2c_write_async()
 */
int pi_i2c_read_timeout(struct pi_device *device, uint8_t *rx_buff, int length,
                        pi_i2c_xfer_flags_e flags, uint32_t timeout_us);

/**
 * \brief Enqueue a burst write copy to the I2C (from chip to I2C device), with timeout.
 *
 * This function has the same behaviour as pi_i2c_write(), with timeout.
 * This timeout value is used to abort/cancel a transfer when timeout is reached.
 *
 * \param device         Pointer to the structure describing the device.
 * \param tx_data        Address in the chip where the data to be sent is read.
 * \param length         Size in bytes of the copy
 * \param flags          Specify additional transfer behaviors like start and stop bits
 *                       management.
 * \param timeout_us     Timeout value in us.
 *
 * \note To use this feature, a UDMA timeout channel must be allocated before a
 *       call to this functions :
 *       * pi_udma_timeout_alloc()
 *       * pi_i2c_ioctl()   //PI_I2C_IOCTL_ATTACH_TIMEOUT_TX
 *
 * \note To use this feature asynchronously, proceed as follows :
 *       * pi_udma_timeout_alloc()
 *       * pi_i2c_ioctl()   //PI_I2C_IOCTL_ATTACH_TIMEOUT_TX
 *       * pi_task_timeout_set()
 *       * pi_i2c_write_async()
 */
int pi_i2c_write_timeout(struct pi_device *device, uint8_t *tx_data, int length,
                         pi_i2c_xfer_flags_e flags, uint32_t timeout_us);

/**
 * \brief get the request status from a task
 *
 * This function can be used to retrieve the request status
 * from a task.
 *
 * \param task the task to retrieve the status from
 *
 * \warning in order to be able to retrieve the request status
 *          you need to pass a pointer to the task as the first
 *          callback argument. You can use next arguments as you please.
 *
 * \return  PI_OK if the request was successful
 *          PI_ERR_I2C_NACK if a NACK was received during the request
 */
int pi_i2c_get_request_status(pi_task_t* task);

/**
 * \brief Scan i2c bus to detect a dev
 *
 * This function can be used to detect if a device is connected to the i2c bus.
 * The pi_i2c_conf_t structure is used to pass the address of the device
 * to look for, with different baudrate if needed.
 *
 * \param device         Pointer to device structure.
 * \param conf           Conf struct holding address and baudrate.
 * \param rx_data        Pointer to 1 Byte buffer to store data.
 *
 * \retval 0x00          If a device has been detected at given address.
 * \retval 0xFF          Otherwise.
 */
int pi_i2c_detect(struct pi_device *device, struct pi_i2c_conf *conf, uint8_t *rx_data);

//!@}

/**
 * @}
 */




#endif  /* __PI_DRIVERS_I2C_H__ */
