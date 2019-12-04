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

/**
 * @file
 * @brief Public APIs for the I2S (Inter-IC Sound) bus drivers.
 */

#ifndef __PMSIS_DRIVERS_I2S_H__
#define __PMSIS_DRIVERS_I2S_H__

/**
* @ingroup groupDrivers
*/



/**
 * @defgroup I2S I2S Interface
 * @ingroup groupDrivers
 * @brief I2S (Inter-IC Sound) Interface
 *
 * The I2S API provides support for the I2S interface.
 * @{
 */

#include <stdint.h>

/*
 * The following #defines are used to configure the I2S controller.
 */

typedef uint8_t pi_i2s_fmt_t;

/** Data Format bit field position. */
#define PI_I2S_FMT_DATA_FORMAT_SHIFT       0
/** Data Format bit field mask. */
#define PI_I2S_FMT_DATA_FORMAT_MASK        (0x7 << PI_I2S_FMT_DATA_FORMAT_SHIFT)

/** @brief Standard I2S Data Format.
 *
 * Serial data is transmitted in two's complement with the MSB first. Both
 * Word Select (WS) and Serial Data (SD) signals are sampled on the rising edge
 * of the clock signal (SCK). The MSB is always sent one clock period after the
 * WS changes. Left channel data are sent first indicated by WS = 0, followed
 * by right channel data indicated by WS = 1.
 *
 *        -. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-. .-.
 *     SCK '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '-' '
 *        -.                               .-------------------------------.
 *     WS  '-------------------------------'                               '----
 *        -.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.
 *     SD  |   |MSB|   |...|   |LSB| x |...| x |MSB|   |...|   |LSB| x |...| x |
 *        -'---'---'---'---'---'---'---'---'---'---'---'---'---'---'---'---'---'
 *             | Left channel                  | Right channel                 |
 */
#define PI_I2S_FMT_DATA_FORMAT_I2S          (0 << PI_I2S_FMT_DATA_FORMAT_SHIFT)

/** @brief Pulse-Density Modulation Format.
 *
 * Serial data is transmitted using the pulse-density modulation. Each sample
 * is a one bit pulse, where the density of the pulses gives the amplitude of
 * the signal. The driver will filter the input signals so that classic PCM
 * samples are stored in the buffers. In single channel mode, the bits are
 * transmitted on clock signal (CLK) rising edges. In dual channel mode, left
 * chanel is transmitted on SCK rising edges and right channel on SCK falling
 * edges. Word Select (WS) is ignored.
 */
#define PI_I2S_FMT_DATA_FORMAT_PDM           (1 << PI_I2S_FMT_DATA_FORMAT_SHIFT)


typedef uint8_t pi_i2s_opt_t;

/** IOCTL command */
enum pi_i2s_ioctl_cmd {
    /** @brief Start the transmission / reception of data.
     *
     * This command can be used when the interface has been opened or stopped
     * to start sampling.
     */
    PI_I2S_IOCTL_START,
    /** @brief Stop the transmission / reception of data.
     *
     * Stop the transmission / reception of data at the end of the current
     * memory block. This command can be used when the interface is sampling and
     * is stopping the interface. When the current TX /
     * RX block is transmitted / received the interface is stopped.
     * Subsequent START command will resume transmission / reception where
     * it stopped.
     */
    PI_I2S_IOCTL_STOP,
};

/** @struct pi_i2s_conf
 * @brief Interface configuration options.
 *
 * @param word_size Number of bits representing one data word.
 * @param channels Number of words per frame.
 * @param format Data stream format as defined by PI_I2S_FMT_* constants.
 * @param options Configuration options as defined by PI_I2S_OPT_* constants.
 * @param frame_clk_freq Frame clock (WS) frequency, this is sampling rate.
 * @param block_size Size of one RX/TX memory block (buffer) in bytes.
 * @param pingpong_buffers Pair of buffers used in double-buffering mode to
 *   capture the incoming samples.
 * @param pdm_decimation_log2 In PDM mode, this gives the log2 of the
 *   decimation to be used, e.g. the number of bits on which the filter
 *   is applied.
 */
struct pi_i2s_conf {
    uint8_t word_size;
    uint8_t channels;
    uint8_t itf;
    pi_i2s_fmt_t format;
    pi_i2s_opt_t options;
    uint32_t frame_clk_freq;
    size_t block_size;
    void *pingpong_buffers[2];
    uint16_t pdm_decimation_log2;
};

/** \brief Initialize an I2S configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them.
 * The structure containing the configuration must be kept alive until the I2S
 * device is opened.
 *
 * \param conf A pointer to the I2S configuration.
 */
void pi_i2s_conf_init(struct pi_i2s_conf *conf);

/** \brief Open an I2S device.
 *
 * This function must be called before the I2S device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * The caller is blocked until the operation is finished.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the caller and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_i2s_open(struct pi_device *device);

/** \brief Close an opened I2S device.
 *
 * This function can be called to close an opened I2S device once it is not
 * needed anymore, in order to free all allocated resources. Once this function
 * is called, the device is not accessible anymore and must be opened
 * again before being used.
 * The caller is blocked until the operation is finished.
 *
 * \param device  A pointer to the structure describing the device.
 */
void pi_i2s_close(struct pi_device *device);

/** \brief Dynamically change the device configuration.
 *
 * This function can be called to change part of the device configuration after
 * it has been opened or to control it.
 *
 * \param device  A pointer to the structure describing the device.
 * \param cmd      The command which specifies which parameters of the driver
 *   to modify and for some of them also their values. The command must be one
 *   of those defined in pi_spi_ioctl_e.
 * \param arg       An additional value which is required for some parameters
 *   when they are set.
 */
int pi_i2s_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/**
 * @brief Read data from the RX queue.
 *
 * Data received by the I2S interface is stored in the RX queue consisting
 * of two memory blocks preallocated by the user and given to the driver in
 * the configuration. Calling this function will return the next available
 * buffer to the caller, which has to use it before the sampling for this buffer
 * starts again.
 *
 * The data is read in chunks equal to the size of the memory block.
 *
 * When using several channels, the organization of the samples for each channel
 * in the buffer, is chip-dependent, check the chip-specific documentation
 * to get more information.
 *
 * If there is no data in the RX queue the function will block waiting for
 * the next RX memory block to fill in.
 *
 * Due to hardware constraints, the address of the buffer must be aligned on
 * 4 bytes.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param mem_block Pointer to the variable storing the address of the RX memory
 *   block containing received data.
 * @param size Pointer to the variable storing the number of bytes read.
 *
 * @retval 0 If successful, -1 if not.
 */
int pi_i2s_read(struct pi_device *dev, void **mem_block, size_t *size);

/**
 * @brief Read data asynchronously from the RX queue.
 *
 * Data received by the I2S interface is stored in the RX queue consisting
 * of two memory blocks preallocated by the user and given to the driver in
 * the configuration. Calling this function will return the next available
 * buffer to the caller, which has to use it before the sampling for this buffer
 * starts again.
 *
 * The data is read in chunks equal to the size of the memory block.
 *
 * When using several channels, the organization of the samples for each channel
 * in the buffer, is chip-dependent, check the chip-specific documentation
 * to get more information.
 *
 * The specified task will be pushed as soon as data is ready in the RX queue,
 * and the information about the memory block and the size will be available
 * in the task.
 *
 * Due to hardware constraints, the address of the buffer must be aligned on
 * 4 bytes.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param task        The task used to notify the end of transfer.
 *
 * @retval 0 If successful, -1 if not.
 */
int pi_i2s_read_async(struct pi_device *dev, pi_task_t *task);

/**
 * @brief Read the status of an asynchronous read.
 *
 * After pi_i2s_read_async is called to be notified when a read buffer is
 * available, and the notification is received, the output information can
 * be retrieved by calling this function.
 *
 * @param task The task used for notification.
 * @param mem_block Pointer to the variable storing the address of the RX memory
 *   block containing received data.
 * @param size Pointer to the variable storing the number of bytes read.
 *
 * @retval 0 If successful, -1 if not.
 */
int pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size);

/**
 * @}
 */

#endif
