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

#ifndef __DRIVERS__SPI_H__
#define __DRIVERS__SPI_H__

#include <stdint.h>


/// @cond IMPLEM

#define __PI_SPI_CTRL_CPOL_BIT 0
#define __PI_SPI_CTRL_CPHA_BIT 2
#define __PI_SPI_CTRL_WORDSIZE_BIT 4
#define __PI_SPI_CTRL_ENDIANNESS_BIT 6
#define __PI_SPI_CTRL_SET_MAX_BAUDRATE_BIT 8

/// @endcond



/**
* @ingroup groupDrivers
*/



/**
 * @defgroup SPI SPI
 *
 * The SPIM driver provides support for transferring data between an external
 * SPIM device and the chip running this driver.
 *
 */

/**
 * @addtogroup SPI
 * @{
 */

/**@{*/

/** \enum pi_spi_wordsize_e
 * \brief Wordsize of the SPI bitstream elements.
 *
 * This is used to know how the endianness must be applied.
 * Not all sizes are supported on all chips, check the chip-specific section
 * to get more information.
 */
typedef enum {
  PI_SPI_WORDSIZE_8 = 0,     /*!< Each element is 8 bits. Thus the endianness
    has no effect. */
  PI_SPI_WORDSIZE_16 = 1,     /*!< Each element is 16 bits. The way each
    element is stored in memory can then be specified with the endianness. */
  PI_SPI_WORDSIZE_32 = 2     /*!< Each element is 32 bits. The way each
    element is stored in memory can then be specified with the endianness. */
} pi_spi_wordsize_e;

/**
 * \enum pi_spi_polarity_e
 *
 * Clock polarity.
 */
typedef enum
{
    PI_SPI_POLARITY_0 = 0, /*!< Leading edge is rising edge, trailing edge is falling edge. */
    PI_SPI_POLARITY_1 = 1  /*!< Leading edge is falling edge, trailing edge is rising edge. */
} pi_spi_polarity_e;

/**
 * \enum pi_spi_phase_e
 *
 * Clock phase.
 */
typedef enum
{
    PI_SPI_PHASE_0 = 0, /*!< Data shifted out on trailing edge of preceding clock cycle.
                         *   Data sampled on leading edge of clock cycle.
                         */
    PI_SPI_PHASE_1 = 1  /*!< Data shifted out on leading edge of current clock cycle.
                         *   Data sampled on trailing edge of clock cycle.
                         */
} pi_spi_phase_e;

/** \struct pi_spi_conf_t
 * \brief SPI master configuration structure.
 *
 * This structure is used to pass the desired SPI master configuration to the
 * runtime when opening a device.
 */
struct pi_spi_conf
{
    int max_baudrate;           /*!< Maximum baudrate for the SPI bitstream which can
                                  be used with the opened device . */
    char wordsize;              /*!< Wordsize of the elements in the bitstream. Can
                                  be PI_SPI_WORDSIZE_8 for 8 bits data or PI_SPI_WORDSIZE_32 for 32 bits
                                  data. This is used to interpret the endianness. */
    char big_endian;            /*!< If 1, the elements are stored in memory in a
                                  big-endian way, i.e. the most significant byte is stored at the lowest
                                  address. This is taken into account only if the wordsize is 32 bits. */
    pi_spi_polarity_e polarity; /*!< Polarity of the clock. */
    pi_spi_phase_e phase;       /*!< Phase of the clock. */
    signed char cs;             /*!< Specifies which SPI chip select is used for the
                                  device. */
    signed char itf;            /*!< Specifies on which SPI interface the device is
                                  connected. */
    int max_rcv_chunk_size;     /*!< Specifies maximum chunk size for reception when
                                  using copies. */
    int max_snd_chunk_size;     /*!< Specifies maximum chunk size for sending when
                                  using copies. */
};

/** \enum pi_spi_ioctl_e
 * \brief Possible parameters which can be set through the pi_spi_control API
   function.
 *
 * This is used to reconfigure dynamically some of the parameters of an
 * opened device.
 */
typedef enum {
  PI_SPI_CTRL_CPOL0             = 1 << __PI_SPI_CTRL_CPOL_BIT, /*!< Set the
    clock polarity to 0. */
  PI_SPI_CTRL_CPOL1             = 2 << __PI_SPI_CTRL_CPOL_BIT, /*!< Set the
    clock polarity to 1. */
  PI_SPI_CTRL_CPHA0             = 1 << __PI_SPI_CTRL_CPHA_BIT, /*!< Set the
    clock phase to 0. */
  PI_SPI_CTRL_CPHA1             = 2 << __PI_SPI_CTRL_CPHA_BIT, /*!< Set the
    clock phase to 1. */
  PI_SPI_CTRL_WORDSIZE_8        = 1 << __PI_SPI_CTRL_WORDSIZE_BIT, /*!< Set the
    wordsize to 8 bits. */
  PI_SPI_CTRL_WORDSIZE_32       = 2 << __PI_SPI_CTRL_WORDSIZE_BIT, /*!< Set the
    wordsize to 32 bits. */
  PI_SPI_CTRL_BIG_ENDIAN        = 1 << __PI_SPI_CTRL_ENDIANNESS_BIT, /*!<
    Handle the elements in memory in a big-endian way. */
  PI_SPI_CTRL_LITTLE_ENDIAN     = 2 << __PI_SPI_CTRL_ENDIANNESS_BIT, /*!<
    Handle the elements in memory in a little-endian way. */
  PI_SPI_CTRL_SET_MAX_BAUDRATE  = 1 << __PI_SPI_CTRL_SET_MAX_BAUDRATE_BIT, /*!<
    Change maximum baudrate. */
} pi_spi_ioctl_e;



/** \enum pi_spi_flags_e
 * \brief Specifies additional behaviors for transfers.
 *
 * This flags can be given when transfering data.
 * 
 */
typedef enum {
  PI_SPI_CS_AUTO       = 0 << 0,    /*!< Handles the chip select automatically.
    It is set low just before the transfer is started and set back high when
    the transfer is finished. */
  PI_SPI_CS_KEEP       = 1 << 0,    /*!< Handle the chip select manually. It is
    set low just before the transfer is started and is kept low until the next
    transfer. */
  PI_SPI_CS_NONE       = 2 << 0,    /*!< Don't do anything with the chip
    select. */
  PI_SPI_LINES_SINGLE  = 0 << 2,    /*!< Use a single MISO line. */
  PI_SPI_LINES_QUAD    = 1 << 2,    /*!< Use quad MISO lines. */
  PI_SPI_LINES_OCTAL   = 2 << 2,    /*!< Use octal MISO lines. */
  PI_SPI_COPY_EXT2LOC  = 1 << 4,    /*!< Do a copy from external memory to local
   chip memory. */
  PI_SPI_COPY_LOC2EXT  = 0 << 4,    /*!< Do a copy from local chip memory to
    external memory. */
} pi_spi_flags_e;

/** \brief Initialize an SPI master configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them.
 * The structure containing the configuration must be kept alive until the SPI
 * device is opened.
 *
 * \param conf A pointer to the SPI master configuration.
 */
void pi_spi_conf_init(struct pi_spi_conf *conf);

/** \brief Open an SPI device.
 *
 * This function must be called before the SPI device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * The caller is blocked until the operation is finished.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the caller and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_spi_open(struct pi_device *device);

/** \brief Close an opened SPI device.
 *
 * This function can be called to close an opened SPI device once it is not
 * needed anymore, in order to free all allocated resources. Once this function
 * is called, the device is not accessible anymore and must be opened
 * again before being used.
 * The caller is blocked until the operation is finished.
 *
 * \param device  A pointer to the structure describing the device.
 */
void pi_spi_close(struct pi_device *device);

/** \brief Dynamically change the device configuration.
 *
 * This function can be called to change part of the device configuration after
 * it has been opened.
 *
 * \param device  A pointer to the structure describing the device.
 * \param cmd      The command which specifies which parameters of the driver
 *   to modify and for some of them also their values. The command must be one
 *   of those defined in pi_spi_ioctl_e.
 * \param arg       An additional value which is required for some parameters
 *   when they are set.
 */
void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/** \brief Enqueue a write copy to the SPI (from Chip to SPI device).
 *
 * This function can be used to send data to the SPI device.
 * The copy will make a synchronous transfer between the SPI and one of the
 * chip memory.
 * This is by default using classic SPI transfer with MOSI and MISO lines, but
 * other kind of transfers like quad SPI  can be used by specifying additional
 * flags.
 * Due to hardware constraints, the address of the buffer must be aligned on
 * 4 bytes and the size must be a multiple of 4.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device    A pointer to the structure describing the device.
 * \param data   The address in the chip where the data to be sent must be read.
 * \param len    The size in bits of the copy.
 * \param flag   Additional behaviors for the transfer can be specified using
 *   this flag. Can be 0 to use the default flag, which is using PI_SPI_CS_AUTO
 *   and PI_SPI_LINES_SINGLE.
 */
void pi_spi_send(struct pi_device *device, void *data, size_t len,
  pi_spi_flags_e flag);

/** \brief Enqueue a read copy to the SPI (from Chip to SPI device).
 *
 * This function can be used to receive data from the SPI device.
 * The copy will make a synchronous transfer between the SPI and one of the
 * chip memory.
 * This is by default using classic SPI transfer with MOSI and MISO lines, but
 * other kind of transfers like quad SPI 
 * can be used by specifying additional flags.
 * Due to hardware constraints, the address of the buffer must be aligned on 4
 * bytes and the size must be a multiple of 4.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param data  The address in the chip where the received data must be written.
 * \param len   The size in bits of the copy.
 * \param flag  Additional behaviors for the transfer can be specified using
 *   this flag. Can be 0 to use the default flag, which is using PI_SPI_CS_AUTO
 *   and PI_SPI_LINES_SINGLE.
 */
void pi_spi_receive(struct pi_device *device, void *data, size_t len,
  pi_spi_flags_e flag);

/** \brief Enqueue a read and write copy to the SPI (using full duplex mode).
 *
 * This function can be used to send and receive data with the SPI device using
 * full duplex mode.
 * The copy will make a synchronous transfer between the SPI and one of the
 * chip memory.
 * This is using classic SPI transfer with MOSI and MISO lines.
 * Due to hardware constraints, the address of the buffer must be aligned on 4
 * bytes and the size must be a multiple of 4.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param tx_data The address in the chip where the data to be sent must be
 *   read.
 * \param rx_data The address in the chip where the received data must be
 *   written.
 * \param len     The size in bits of the copy.
 * \param flag   Additional behaviors for the transfer can be specified using
 *   this flag. Can be 0 to use the default flag, which is using PI_SPI_CS_AUTO
 *   and PI_SPI_LINES_SINGLE.
 */
void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data,
  size_t len, pi_spi_flags_e flag);

/** \brief Enqueue an asynchronous write copy to the SPI (from Chip to SPI
 *   device).
 *
 * This function can be used to send data to the SPI device.
 * The copy will make an asynchronous transfer between the SPI and one of the
 * chip memory. This is by default using classic SPI transfer with MOSI and
 * MISO lines, but other kind of transfers like quad SPI 
 * can be used by specifying additional flags.
 * Due to hardware constraints, the address of the buffer must be aligned on 4
 * bytes and the size must be a multiple of 4.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param data   The address in the chip where the data to be sent must be read.
 * \param len    The size in bits of the copy.
 * \param flag   Additional behaviors for the transfer can be specified using
 *   this flag. Can be 0 to use the default flag, which is using
 *   PI_SPI_CS_AUTO and PI_SPI_LINES_SINGLE.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.details.
 */
void pi_spi_send_async(struct pi_device *device, void *data, size_t len,
  pi_spi_flags_e flag, pi_task_t *task);

/** \brief Enqueue an asynchronous read copy to the SPI (from Chip to SPI
 * device).
 *
 * This function can be used to receive data from the SPI device.
 * The copy will make an asynchronous transfer between the SPI and one of the
 * chip memory.
 * This is by default using classic SPI transfer with MOSI and MISO lines, but
 * other kind of transfers like quad SPI 
 * can be used by specifying additional flags.
 * Due to hardware constraints, the address of the buffer must be aligned on 4
 * bytes and the size must be a multiple of 4.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param data    The address in the chip where the received data must be
 *   written.
 * \param len     The size in bits of the copy.
 * \param flag   Additional behaviors for the transfer can be specified using
 *   this flag. Can be 0 to use the default flag, which is using PI_SPI_CS_AUTO
 *   and PI_SPI_LINES_SINGLE.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.details.details.
 */
void pi_spi_receive_async(struct pi_device *device, void *data, size_t len,
  pi_spi_flags_e flag, pi_task_t *task);

/** \brief Enqueue an asynchronous read and write copy to the SPI (using full
 * duplex mode).
 *
 * This function can be used to send and receive data with the SPI device using
 * full duplex flag.
 * The copy will make an asynchronous transfer between the SPI and one of the
 * chip memory.
 * This is using classic SPI transfer with MOSI and MISO lines.
 * Due to hardware constraints, the address of the buffer must be aligned on 4
 * bytes and the size must be a multiple of 4.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * \param device  A pointer to the structure describing the device.
 * \param tx_data  The address in the chip where the data to be sent must be
 *   read.
 * \param rx_data  The address in the chip where the received data must be
 *   written.
 * \param len      The size in bits of the copy.
 * \param flag   Additional behaviors for the transfer can be specified using
 *   this flag. Can be 0 to use the default flag, which is using PI_SPI_CS_AUTO
 *   and PI_SPI_LINES_SINGLE.
 * \param task        The task used to notify the end of transfer.
   See the documentation of pi_task_t for more details.details.details.
 */
void pi_spi_transfer_async(struct pi_device *device, void *tx_data,
  void *rx_data, size_t len, pi_spi_flags_e flag, pi_task_t *task);

//!@}

/**
 * @} end of SPI master
 */

/// @cond IMPLEM

/**
 * Enqueue receive ucode given by the user, and receive answer in data buffer
 */
void pi_spi_receive_with_ucode(struct pi_device *device, void *data,
        size_t len, pi_spi_flags_e flags, int ucode_size,
        void *ucode);

/**
 * Enqueue receive ucode given by the user, and receive answer in data buffer
 */
void pi_spi_send_with_ucode(struct pi_device *device, void *data,
        size_t len, pi_spi_flags_e flags, int ucode_size,
        void *ucode);

/**
 * Extract config from device (useful to craft ucode)
 */
uint32_t pi_spi_get_config(struct pi_device *device);

#define SPI_UCODE_CMD_SEND_CMD(cmd,bits,qpi)    ((2<<28) | ((qpi)<<27) | (((bits)-1)<<16) | (((cmd)>>8)<<0) | (((cmd)&0xff)<<(0+8)))
#define SPI_UCODE_CMD_SEND_ADDR(bits,qpi)       ((3<<28) | ((qpi)<<27) | (((bits)-1)<<16))
#define SPI_UCODE_CMD_DUMMY(cycles)             ((4<<28) | (((cycles)-1)<<16))

void *pi_spi_receive_ucode_set(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size);

void pi_spi_receive_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size);

void *pi_spi_send_ucode_set(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size);

void pi_spi_send_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode, uint32_t ucode_size);

void pi_spi_copy(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size,
  pi_spi_flags_e flags);

void pi_spi_copy_async(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size,
  pi_spi_flags_e flags, pi_task_t *task);

void pi_spi_copy_2d(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_spi_flags_e flags);

void pi_spi_copy_2d_async(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_spi_flags_e flags, pi_task_t *task);

/// @endcond


#endif
