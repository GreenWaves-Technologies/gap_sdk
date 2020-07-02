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

#ifndef __PMSIS_DRIVERS_ASRC_H__
#define __PMSIS_DRIVERS_ASRC_H__

#include <stdint.h>

/**
 * \ingroup groupDrivers
 *
 * \defgroup ASRC ASRC
 *
 * \brief Aysnchronous Sample Rate Conversion(ASRC) Library.
 *
 * The ASRC API provides support for audio to be streamed with sample rate conversion.
 *
 * \addtogroup ASRC
 * @{
 */

typedef uint8_t pi_asrc_opt_t;

#define PI_ASRC_OPT_STREAM_MODE_OFFSET ( 0x0 )
#define PI_ASRC_OPT_STREAM_MODE_MASK   ( 0x3 << PI_ASRC_OPT_STREAM_MODE_OFFSET )

/**
 * \brief I2S to I2S stream
 *
 * I2S2I2S mode.
 * In this mode, the input is from a I2S interface, the output is to a I2S interface.
 * The sampling rate for input and output are given by both input and output interfaces.
 */
#define PI_ASRC_OPT_I2S_TO_I2S ( 0 << PI_ASRC_OPT_STREAM_MODE_OFFSET )

/**
 * \brief Memory to Memory stream.
 *
 * MEM2MEM mode.
 * In this mode, the input and output are from memory.
 */
#define PI_ASRC_OPT_MEM_TO_MEM ( 1 << PI_ASRC_OPT_STREAM_MODE_OFFSET )

/**
 * \brief I2S to Memory stream.
 *
 * I2S2MEM mode.
 * In this mode, the samples are taken from an I2S source and stored into memory
 * after sampling rate conversion.
 */
#define PI_ASRC_OPT_I2S_TO_MEM ( 2 << PI_ASRC_OPT_STREAM_MODE_OFFSET )

/**
 * \brief Memory to I2S stream.
 *
 * MEM2I2S mode.
 *
 * In this mode, the samples are taken from the memory and streamed to an I2S source
 * after sampling rate conversion.
 */
#define PI_ASRC_OPT_MEM_TO_I2S ( 3 << PI_ASRC_OPT_STREAM_MODE_OFFSET )

typedef enum
{
    PI_ASRC_FS_I2S_0 = 0,
    PI_ASRC_FS_I2S_1 = 1,
    PI_ASRC_FS_I2S_2 = 2,
    PI_ASRC_FS_PWM_0 = 3,
    PI_ASRC_FS_PWM_1 = 4,
    PI_ASRC_FS_PWM_2 = 5,
    PI_ASRC_FS_PWM_3 = 6
} pi_asrc_fs_e;

/**
 * \struct pi_asrc_conf
 *
 * \brief ASRC configuration struct.
 */
struct pi_asrc_conf
{
    uint8_t itf;                /*!< ASRC device ID. */
    pi_asrc_opt_t options;      /*!< ASRC configuration options. */
    pi_asrc_fs_e fs_in;         /*!< Input sample rate, chosen from an I2S source
                                  or PWM source for MEM2XXX mode. */
    pi_asrc_fs_e fs_out;        /*!< Output sample rate, chosen from an I2S source
                                  or PWM source for MEM2XXX mode. */
    uint16_t chan;              /*!< ASRC channels to use. */
};

/**
 * \brief Initialize ASRC config struct.
 *
 * This function initializes ASRC config struct with default values.
 *
 * \param conf           Pointer to ASRC config struct.
 */
void pi_asrc_conf_init(struct pi_asrc_conf *conf);

/**
 * \brief Open a ASRC device.
 *
 * This function opens a ASRC device. It does all the needed configurations.
 * After device is opened, the user can do sample rate conversion on the fly
 * on streams.
 *
 * \param device         Pointer to device structure.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note This function must be called before any use of the device.
 */
int pi_asrc_open(struct pi_device *device);

/**
 * \brief Close an opened ASRC device.
 *
 * This function closes a ASRC device.
 *
 * \param device         Pointer to device structure.
 */
void pi_asrc_close(struct pi_device *device);

/**
 * \brief Start a sample rate conversion.
 *
 * This function starts a sample rate conversion between given source and dest.
 * Source can be either an I2S interface or memory data. Same for the dest.
 * Depending on the pi_asrc_opt_t options given to device, source and dest will
 * be considered as I2S interface or memory block.
 *
 * \param device         Pointer to device structure.
 * \param src            Pointer to source data.
 * \param dest           Pointer to dest data.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 */
int pi_asrc_convert(struct pi_device *device, void *src, void *dst);

/**
 * \brief Start a sample rate conversion.
 *
 * This is the asynchronous version of pi_asrc_convert() function.
 *
 * \param device         Pointer to device structure.
 * \param src            Pointer to source data.
 * \param dest           Pointer to dest data.
 * \param task           Event task used to notify end of conversion.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 */
int pi_asrc_convert_async(struct pi_device *device, void *src, void *dst,
                          struct pi_task *task);

/**
 * \brief Status of the current conversion.
 *
 * \param device         Pointer to device structure.
 * \param task           Event task used to notify end of conversion.
 *
 * \return STATUS        Status of current conversion.
 */
int pi_asrc_convert_status(struct pi_device *device, struct pi_task *task);

/**
 * @}
 */

#endif  /* __PMSIS_DRIVERS_ASRC_H__ */
