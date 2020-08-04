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

#ifndef __PMSIS_DRIVERS_I2S_H__
#define __PMSIS_DRIVERS_I2S_H__

#include <stdint.h>


/**
 * @ingroup groupDrivers
 *
 * @defgroup I2S I2S Interface
 *
 * \brief I2S (Inter-IC Sound) Interface
 *
 * The I2S API provides support for the I2S interface.
 *
 * @addtogroup I2S
 * @{
 */


/*
 * The following #defines are used to configure the I2S controller.
 */

typedef uint8_t pi_i2s_fmt_t;

/**
 * \cond IMPLEM
 */
#define PI_I2S_FMT_DATA_FORMAT_SHIFT              ( 0 )
#define PI_I2S_FMT_DATA_FORMAT_MASK               ( 1 << PI_I2S_FMT_DATA_FORMAT_SHIFT )

#define PI_I2S_CH_FMT_DATA_ORDER_SHIFT            ( 3 )
#define PI_I2S_CH_FMT_DATA_ORDER_MASK             ( 1 << PI_I2S_CH_FMT_DATA_ORDER_SHIFT )

#define PI_I2S_CH_FMT_DATA_ALIGN_SHIFT            ( 4 )
#define PI_I2S_CH_FMT_DATA_ALIGN_MASK             ( 1 << PI_I2S_CH_FMT_DATA_ALIGN_SHIFT )

#define PI_I2S_CH_FMT_DATA_SIGN_SHIFT             ( 5 )
#define PI_I2S_CH_FMT_DATA_SIGN_MASK              ( 1 << PI_I2S_CH_FMT_DATA_SIGN_SHIFT )
/**
 * \endcond
 */

/**
 * \brief Standard I2S Data Format.
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
#define PI_I2S_FMT_DATA_FORMAT_I2S                ( 0 << PI_I2S_FMT_DATA_FORMAT_SHIFT )

/**
 * \brief Pulse-Density Modulation Format.
 *
 * Serial data is transmitted using the pulse-density modulation. Each sample
 * is a one bit pulse, where the density of the pulses gives the amplitude of
 * the signal. The driver will filter the input signals so that classic PCM
 * samples are stored in the buffers. In single channel mode, the bits are
 * transmitted on clock signal (CLK) rising edges. In dual channel mode, left
 * chanel is transmitted on SCK rising edges and right channel on SCK falling
 * edges. Word Select (WS) is ignored.
 */
#define PI_I2S_FMT_DATA_FORMAT_PDM                ( 1 << PI_I2S_FMT_DATA_FORMAT_SHIFT )


/**
 * \brief Data order MSB
 *
 * Data bits are transferred MSB first.
 */
#define PI_I2S_CH_FMT_DATA_ORDER_MSB              ( 0 <<  PI_I2S_CH_FMT_DATA_ORDER_SHIFT )

/**
 * \brief Data order LSB
 *
 * Data bits are transferred LSB first.
 */
#define PI_I2S_CH_FMT_DATA_ORDER_LSB              ( 1 <<  PI_I2S_CH_FMT_DATA_ORDER_SHIFT )


/**
 * \brief Data alignment left
 *
 * Left Justified.
 */
#define PI_I2S_CH_FMT_DATA_ALIGN_LEFT             ( 0 << PI_I2S_CH_FMT_DATA_ALIGN_SHIFT )

/**
 * \brief Data alignment right
 *
 * Right Justified.
 */
#define PI_I2S_CH_FMT_DATA_ALIGN_RIGHT            ( 1 << PI_I2S_CH_FMT_DATA_ALIGN_SHIFT )


/**
 * \brief Data sign extension disabled
 *
 * Data are not sign extended when I2S data size is inferior to memory data size.
 */
#define PI_I2S_CH_FMT_DATA_SIGN_NO_EXTEND         ( 0 << PI_I2S_CH_FMT_DATA_SIGN_SHIFT )

/**
 * \brief Data sign extension enabled
 *
 * Data are sign extended when I2S data size is inferior to memory data size.
 * For example when I2S data are on 16 bits but are stored in a 32 bits word,
 * MSB sign is extended.
 */
#define PI_I2S_CH_FMT_DATA_SIGN_EXTEND            ( 1 << PI_I2S_CH_FMT_DATA_SIGN_SHIFT )


typedef uint16_t pi_i2s_opt_t;

/**
 * \cond IMPLEM
 */
#define PI_I2S_OPT_BUFFER_MODE_SHIFT              ( 0 )
#define PI_I2S_OPT_BUFFER_MODE_MASK               ( 1 << PI_I2S_OPT_BUFFER_MODE_SHIFT )

#define PI_I2S_OPT_FULL_DUPLEX_SHIFT              ( 1 )
#define PI_I2S_OPT_FULL_DUPLEX_MASK               ( 1 << PI_I2S_OPT_FULL_DUPLEX_SHIFT )

#define PI_I2S_OPT_CH_ENABLE_SHIFT                ( 2 )
#define PI_I2S_OPT_CH_ENABLE_MASK                 ( 1 << PI_I2S_OPT_CH_ENABLE_SHIFT )

#define PI_I2S_OPT_LOOPBACK_ENA_SHIFT             ( 3 )
#define PI_I2S_OPT_LOOPBACK_ENA_MASK              ( 1 << PI_I2S_OPT_LOOPBACK_ENA_SHIFT )

#define PI_I2S_OPT_RX_TX_SHIFT                    ( 4 )
#define PI_I2S_OPT_RX_TX_MASK                     ( 1 << PI_I2S_OPT_RX_TX_SHIFT )

#define PI_I2S_OPT_TDM_ENA_SHIFT                  ( 5 )
#define PI_I2S_OPT_TDM_ENA_MASK                   ( 1 << PI_I2S_OPT_TDM_ENA_SHIFT )

#define PI_I2S_OPT_CLK_SRC_SHIFT                  ( 6 )
#define PI_I2S_OPT_CLK_SRC_MASK                   ( 1 << PI_I2S_OPT_CLK_SRC_SHIFT )

#define PI_I2S_OPT_WS_SRC_SHIFT                   ( 7 )
#define PI_I2S_OPT_WS_SRC_MASK                    ( 1 << PI_I2S_OPT_WS_SRC_SHIFT )
/**
 * \endcond
 */

/**
 * \brief Ping pong mode
 *
 * In ping pong mode TX output or RX sampling will keep alternating between a
 * ping buffer and a pong buffer.
 * This is normally used in audio streams when one buffer
 * is being populated while the other is being played (DMAed) and vice versa.
 * So, in this mode, 2 sets of buffers fixed in size are used. These 2 buffers
 * must be given in the configuration when the driver is opened and kept alive
 * until the driver is closed.
 */
#define PI_I2S_OPT_PINGPONG                       ( 0 << PI_I2S_OPT_BUFFER_MODE_SHIFT )

/**
 * \brief Mem slab mode
 *
 * In mem slab mode TX output or RX sampling will keep alternating between a
 * a set of buffers given by the user.
 * Memory slab pointed to by the mem_slab field has to be defined and
 * initialized by the user. For I2S driver to function correctly number of
 * memory blocks in a slab has to be at least 2 per queue. Size of the memory
 * block should be multiple of frame_size where frame_size = (channels *
 * word_size_bytes).
 * As an example 16 bit word will occupy 2 bytes, 24 or 32
 * bit word will occupy 4 bytes.
 */
#define PI_I2S_OPT_MEM_SLAB                       ( 1 << PI_I2S_OPT_BUFFER_MODE_SHIFT )

/**
 * \brief Full duplex mode
 *
 * The normal and default mode is to use a single pin for both TX and RX.
 * In full duplex mode, RX and TX will use 2 different pins(called sdi and sdo
 * so that samples can be received and sent at the same time).
 */
#define PI_I2S_OPT_FULL_DUPLEX                    ( 1 << PI_I2S_OPT_FULL_DUPLEX_SHIFT )

/**
 * \brief Disable the channel
 *
 * If set, this desactivates the channel being configured (either RX or TX).
 * Once the sampling is running, this will make sure this channel is not
 * receiving or sending samples.
 */
#define PI_I2S_OPT_DISABLED                       ( 0 << PI_I2S_OPT_CH_ENABLE_SHIFT )

/**
 * \brief Enable the channel
 *
 * If set, this activates the channel being configured (either RX or TX).
 * Once the sampling is running, this will make sure this channel is receiving
 * or sending samples.
 */
#define PI_I2S_OPT_ENABLED                        ( 1 << PI_I2S_OPT_CH_ENABLE_SHIFT )

/**
 * \brief Configure TX channel
 *
 * If set, the configuration will apply to the TX channel and won't change
 * anything for what concerns the RX channel. Note that this does not prevent
 * from using the RX channel, they must just be configured separately.
 *
 * Setting this configuration allows to send data from memory to speakers.
 */
#define PI_I2S_OPT_IS_TX                          ( 0 << PI_I2S_OPT_RX_TX_SHIFT )

/**
 * \brief Configure RX channel
 *
 * If set, the configuration will apply to the RX channel and won't change
 * anything for what concerns the TX channel. Note that this does not prevent
 * from using the TX channel, they must just be configured separately.
 *
 * Setting this configuration allows to receive data from microphones to memory.
 */
#define PI_I2S_OPT_IS_RX                          ( 1 << PI_I2S_OPT_RX_TX_SHIFT )

/**
 * \brief TX loopback
 *
 * If set, this activates an internal loopback between the RX pin and TX
 * pin. All data received on the RX will be sent to the TX pin. Note that this
 * does not prevent from receiving data. Data received on the RX can be sent
 * both to a memory buffer (if RX channel is enabled) and to the TX (if the
 * loopback is enabled). The loopback must be applied on the TX channel.
 */
#define PI_I2S_OPT_LOOPBACK                       ( 1 << PI_I2S_OPT_LOOPBACK_ENA_SHIFT )

/**
 * \brief TDM mode
 *
 * In TDM mode, the same interface is time-multiplexed to transmit data
 * for multiple channels where each channel can have a specific
 * configuration. As for classic I2S mode, TX and RX channels are grouped
 * together within a slot. Each slot can send and receive 1 data per frame.
 * In this mode each slot must be configured separately
 * using the I2S configuration. Each slot can have an RX and a TX channel.
 */
#define PI_I2S_OPT_TDM                            ( 1 << PI_I2S_OPT_TDM_ENA_SHIFT )

/**
 * \brief Use internal clock
 *
 * Clock is generated internally from SOC's clock.
 *
 * This is default clock source.
 */
#define PI_I2S_OPT_INT_CLK                        ( 0 << PI_I2S_OPT_CLK_SRC_SHIFT )

/**
 * \brief Use external clock
 *
 * If this option is specified, no clock is generated and an external clock
 * is used.
 */
#define PI_I2S_OPT_EXT_CLK                        ( 1 << PI_I2S_OPT_CLK_SRC_SHIFT )

/**
 * \brief Use internal word strobe
 *
 * WS signal is generated internally from SCK signal.
 *
 * This is default WS source.
 */
#define PI_I2S_OPT_INT_WS                         ( 0 << PI_I2S_OPT_WS_SRC_SHIFT )

/**
 * \brief Use external word strobe
 *
 * If this option is specified, no word strobe is generated and an external
 * one is used.
 */
#define PI_I2S_OPT_EXT_WS                         ( 1 << PI_I2S_OPT_WS_SRC_SHIFT )


/**
 * \enum pi_i2s_ioctl_cmd_e
 *
 * \brief IOCTL command
 */
typedef enum
{
    /**
     * \brief Start the transmission / reception of data.
     *
     * This command can be used when the interface has been opened or stopped
     * to start sampling.
     */
    PI_I2S_IOCTL_START,

    /**
     * \brief Stop the transmission / reception of data.
     *
     * Stop the transmission / reception of data at the end of the current
     * memory block. This command can be used when the interface is sampling and
     * is stopping the interface. When the current TX /
     * RX block is transmitted / received the interface is stopped.
     * Subsequent START command will resume transmission / reception where
     * it stopped.
     */
    PI_I2S_IOCTL_STOP,

    /**
     * \brief Configure a channel in TDM mode.
     *
     * In TDM mode, the same interface is time-multiplexed to transmit data
     * for multiple channels, and each channel can have a specific
     * configuration. This command can be used to give the configuration
     * of one channel. The argument must be a pointer to a structure of type
     * struct pi_i2s_conf containing the channel configuration.
     */
    PI_I2S_IOCTL_CONF_SET,

    /**
     * \brief Get the current configuration of a channel in TDM mode.
     *
     * In TDM mode, the same interface is time-multiplexed to transmit data
     * for multiple channels, and each channel can have a specific
     * configuration. This command can be used to get the current configuration
     * of one channel. The argument must be a pointer to a structure of type
     * struct pi_i2s_conf where the current channel configuration will be
     * stored.
     */
    PI_I2S_IOCTL_CONF_GET,

    /**
     * \brief Enable clock
     *
     * Enable clock for the i2s interface.
     * This command does not sample data, and does not save data. It allows
     * the interface to receive clock signals from SoC and propgate it to slave
     * devices on end line.
     */
    PI_I2S_IOCTL_CLOCK_ENABLE,

    /**
     * \brief Disable clock
     *
     * Disable clock for the i2s interface.
     */
    PI_I2S_IOCTL_CLOCK_DISABLE
} pi_i2s_ioctl_cmd_e;

/**
 * \struct pi_i2s_conf
 *
 * \brief Interface configuration options.
 */
struct pi_i2s_conf
{
    uint32_t frame_clk_freq;    /*!< Frame clock (WS) frequency, this is sampling rate. */
    size_t block_size;          /*!< Size of one RX/TX memory block(buffer) in bytes.
                                  On some chips, this size may have to be set under a
                                  maximum size, check the chip-specific section. */
    pi_mem_slab_t *mem_slab;    /*!< memory slab to store RX/TX data. */
    void *pingpong_buffers[2];  /*!< Pair of buffers used in double-bufferin
                                  mode to capture the incoming samples.  */
    uint16_t pdm_decimation;    /*!< In PDM mode, this gives the decimation factor
                                  to be used, e.g. the number of bits on which
                                  the filter is applied. This factor is
                                  usually in the range between 48 and 128.
                                  PDM_freq = sampling_rate * pdm_decimation.
                                  - PDM_freq is the clock frequency of the microphone.
                                  - sampling_rate is the audio sampling rate(22050kHz, 44100kHZ,
                                  48000kHZ,...).
                                  - pdm_decimation is the decimation factor to apply. */
    pi_i2s_fmt_t format;        /*!< Data stream format as defined by PI_I2S_FMT_* constants. */
    uint8_t word_size;          /*!< Number of bits representing one data word. */
    int8_t mem_word_size;       /*!< Number of bits representing one data word in memory.
                                  If it is -1, this is equal to word_size. */
    uint8_t channels;           /*!< Number of words per frame. */
    uint8_t itf;                /*!< I2S device ID. */
    pi_i2s_opt_t options;       /*!< Configuration options as defined by PI_I2S_OPT_* constants. */
    int8_t pdm_shift;           /*!< In PDM mode, the shift value to shift data
                                  when applying filter. */
    uint8_t pdm_filter_ena;     /*!< When using PDM mode, enable PDM filter. */
    uint8_t channel_id;         /*!< Channel ID, from 0 to the number of channels minus 1.
                                  In TDM mode this gives the ID of the slot to be configured.
                                  The options field can be used to specify if the RX or TX
                                  channel must be configured. */
    int8_t asrc_channel;        /*!< If different from -1, this redirect the specified
                                  stream(can be input or output) to/from the ASRC block with
                                  the channel specified here. */
};


/**
 * \brief Setup specific I2S aspects.
 *
 * This function can be called to set specific I2S properties such as the
 * number of clock generator. This is typically used by the BSP to give
 * board specific information.
 *
 * \param flags  A bitfield of chip-dependant properties.
 */
void pi_i2s_setup(uint32_t flags);

/**
 * \brief Initialize an I2S configuration with default values.
 *
 * This function can be called to get default values for all parameters before
 * setting some of them.
 * The structure containing the configuration must be kept alive until the I2S
 * device is opened.
 *
 * \param conf A pointer to the I2S configuration.
 */
void pi_i2s_conf_init(struct pi_i2s_conf *conf);

/**
 * \brief Open an I2S device.
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

/**
 * \brief Close an opened I2S device.
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

/**
 * \brief Dynamically change the device configuration.
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
 * @brief Write data to the TX queue of a channel.
 *
 * Data to be sent by the I2S interface is stored first in the TX queue
 * consisting of memory blocks preallocated by the user with either pingpong
 * buffers or a memory slab allocator.
 *
 * In pingpong mode, the driver will automatically alternate between 2 buffers
 * and the user code is supposed to call this function to notify the driver
 * that the specified buffer is ready to be sent. This is used by the driver
 * to report when an underrun or an overrun occurs.
 *
 * In memory slab allocator mode, the user has to allocate buffers from the
 * memory slab allocator and pass them to the driver by calling this function
 * when they are ready to be sent.
 *
 * This fonction will block until the specified buffer has been transfered.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param mem_block Pointer to the TX memory block containing data to be sent.
 * @param size Number of bytes to write. This value has to be equal or smaller
 *        than the size of the memory block.
 *
 * @retval 0 If successful.
 * @retval -1 An error occured.
 */
int pi_i2s_write(struct pi_device *dev, void *mem_block,
    size_t size);

/**
 * @brief Write data asynchronously to the TX queue of a channel.
 *
 * Data to be sent by the I2S interface is stored first in the TX queue
 * consisting of memory blocks preallocated by the user with either pingpong
 * buffers or a memory slab allocator.
 *
 * In pingpong mode, the driver will automatically alternate between 2 buffers
 * and the user code is supposed to call this function to notify the driver
 * that the specified buffer is ready to be sent. This is used by the driver
 * to report when an underrun or an overrun occurs.
 *
 * In memory slab allocator mode, the user has to allocate buffers from the
 * memory slab allocator and pass them to the driver by calling this function
 * when they are ready to be sent.
 *
 * The specified task will be pushed as soon as data is has been transfered.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param mem_block Pointer to the TX memory block containing data to be sent.
 * @param size Number of bytes to write. This value has to be equal or smaller
 *        than the size of the memory block.
 * @param task        The task used to notify the end of transfer.
 *
 * @retval 0 If successful.
 * @retval -1 An error occured.
 */
int pi_i2s_write_async(struct pi_device *dev,
    void *mem_block, size_t size, pi_task_t *task);

/**
 * @brief Read data from the RX queue of a channel in TDM mode.
 *
 * Data received by the I2S interface is stored in the RX queue consisting
 * of two memory blocks preallocated by the user and given to the driver in
 * the configuration. Calling this function will return the next available
 * buffer to the caller, which has to use it before the sampling for this buffer
 * starts again.
 *
 * The data is read in chunks equal to the size of the memory block.
 *
 * This will return data for the specified channel and must only be used in
 * TDM mode.
 *
 * If there is no data in the RX queue the function will block waiting for
 * the next RX memory block to fill in.
 *
 * Due to hardware constraints, the address of the buffer must be aligned on
 * 4 bytes.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param channel ID of the slot, from 0 to the number of channels minus 1.
 * @param mem_block Pointer to the variable storing the address of the RX memory
 *   block containing received data.
 * @param size Pointer to the variable storing the number of bytes read.
 *
 * @retval 0 If successful, -1 if not.
 */
int pi_i2s_channel_read(struct pi_device *dev, int channel, void **mem_block,
    size_t *size);

/**
 * @brief Read data asynchronously from the RX queue of a channel in TDM mode.
 *
 * Data received by the I2S interface is stored in the RX queue consisting
 * of two memory blocks preallocated by the user and given to the driver in
 * the configuration. Calling this function will return the next available
 * buffer to the caller, which has to use it before the sampling for this buffer
 * starts again.
 *
 * The data is read in chunks equal to the size of the memory block.
 *
 * This will return data for the specified channel and must only be used in
 * TDM mode.
 *
 * The specified task will be pushed as soon as data is ready in the RX queue,
 * and the information about the memory block and the size will be available
 * in the task.
 *
 * Due to hardware constraints, the address of the buffer must be aligned on
 * 4 bytes.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param channel ID of the slot, from 0 to the number of channels minus 1.
 * @param task        The task used to notify the end of transfer.
 *
 * @retval 0 If successful, -1 if not.
 */
int pi_i2s_channel_read_async(struct pi_device *dev, int channel,
    pi_task_t *task);

/**
 * @brief Read the status of an asynchronous read.
 *
 * After pi_i2s_read_async or pi_i2s_channel_read_async is called to be notified
 * when a read buffer is available, and the notification is received, the output
 * information can be retrieved by calling this function.
 *
 * @param task The task used for notification.
 * @param mem_block Pointer to the variable storing the address of the RX memory
 *   block containing received data.
 * @param size Pointer to the variable storing the number of bytes read.
 *
 * @retval 0 If successful.
 * @retval -1 An error occured.
 */
int pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size);

/**
 * @brief Write data to the TX queue of a channel in TDM mode.
 *
 * Data to be sent by the I2S interface is stored first in the TX queue
 * consisting of memory blocks preallocated by the user with either pingpong
 * buffers or a memory slab allocator.
 *
 * In pingpong mode, the driver will automatically alternate between 2 buffers
 * and the user code is supposed to call this function to notify the driver
 * that the specified buffer is ready to be sent. This is used by the driver
 * to report when an underrun or an overrun occurs.
 *
 * In memory slab allocator mode, the user has to allocate buffers from the
 * memory slab allocator and pass them to the driver by calling this function
 * when they are ready to be sent.
 *
 * This fonction will block until the specified buffer has been transfered.
 *
 * This will sent data to the specified channel and must only be used in
 * TDM mode.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param channel ID of the slot, from 0 to the number of channels minus 1.
 * @param mem_block Pointer to the TX memory block containing data to be sent.
 * @param size Number of bytes to write. This value has to be equal or smaller
 *        than the size of the memory block.
 *
 * @retval 0 If successful.
 * @retval -1 An error occured.
 */
int pi_i2s_channel_write(struct pi_device *dev, int channel, void *mem_block,
    size_t size);

/**
 * @brief Write data asynchronously to the TX queue of a channel in TDM mode.
 *
 * Data to be sent by the I2S interface is stored first in the TX queue
 * consisting of memory blocks preallocated by the user with either pingpong
 * buffers or a memory slab allocator.
 *
 * In pingpong mode, the driver will automatically alternate between 2 buffers
 * and the user code is supposed to call this function to notify the driver
 * that the specified buffer is ready to be sent. This is used by the driver
 * to report when an underrun or an overrun occurs.
 *
 * In memory slab allocator mode, the user has to allocate buffers from the
 * memory slab allocator and pass them to the driver by calling this function
 * when they are ready to be sent.
 *
 * This will sent data to the specified channel and must only be used in
 * TDM mode.
 *
 * The specified task will be pushed as soon as data is has been transfered.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param channel ID of the slot, from 0 to the number of channels minus 1.
 * @param mem_block Pointer to the TX memory block containing data to be sent.
 * @param size Number of bytes to write. This value has to be equal or smaller
 *        than the size of the memory block.
 * @param task        The task used to notify the end of transfer.
 *
 * @retval 0 If successful.
 * @retval -1 An error occured.
 */
int pi_i2s_channel_write_async(struct pi_device *dev, int channel,
    void *mem_block, size_t size, pi_task_t *task);

/**
 * @brief Read the status of an asynchronous write.
 *
 * After pi_i2s_write_async or pi_i2s_channel_write_async and the notification
 * for the end of transfer is received, return value can
 * be retrieved by calling this function.
 *
 * @param task The task used for notification.
 *
 * @retval 0 If successful.
 * @retval -1 An error occured.
 */
int pi_i2s_write_status(pi_task_t *task);

/**
 * @}
 */



/// @cond IMPLEM

#define PI_I2S_SETUP_SINGLE_CLOCK (1<<0)


/// @endcond

#endif
