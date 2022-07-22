/*
 * Copyright (C) 2019 GreenWaves Technologies
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


#ifndef __BSP__FLASH__MRAM_H__
#define __BSP__FLASH__MRAM_H__

#include "bsp/flash.h"

/**
 * @addtogroup Flash
 * @{
 */

/**
 * @defgroup Mram Mram
 *
 */

/**
 * @addtogroup Mram
 * @{
 */

/**@{*/

/** \struct mram_conf
 * \brief Mram configuration structure.
 *
 * This structure is used to pass the desired Mram configuration to the
 * runtime when opening the device.
 */
struct pi_mram_conf
{
  struct pi_flash_conf flash;  /*!< Generic flash configuration. */
  int itf;                /*!< Mram interface where the flash is
      connected. */
  int baudrate;                /*!< Baudrate in byte/s. */
  int xip_en;
};

/** \brief Mram sense amplifier trim configuration during DFT
 *
 * The structure containing the configuration must be kept alive until the
 * mram device is opened.
 *
 * When the MRAM is powered-on we need to check the number of failure bits
 * in the MRAM array. We start with a max or min value for the sense amplifier
 * trim depending on temperature tested and decrement or increment depending
 * on the number of failure bits found
 *
 * \param device A pointer to the mram device.
 * \param sense_amplifier_trim Configures bits [153:148] to control sense-amp sensitivity
 *                             during DFT testing
 */
void pi_mram_sa_trim_config(struct pi_device *device, u_int8_t sense_amplifier_trim);

/** \brief MRAM enable ECC
 *
 * Enables error code correction for the MRAM.
 * It can correct up to 2 corrupted bits with 100% success
 *
 * \param device A pointer to the mram device.
 */
void pi_mram_enable_ecc(struct pi_device *device);

/** \brief MRAM disable ECC
 *
 * Disables error code correction for the MRAM.
 *
 * \param device A pointer to the mram device.
 */
void pi_mram_disable_ecc(struct pi_device *device);

/** \brief Open access to NVR
 *
 * Opens access to non-volatile rows (NVR) and switch
 * memory accesses from mram array to the NVR memory.
 * It contains 2 sectors each one containing 128 words
 * of 144 bits (128 if ECC)
 *
 * \param device A pointer to the mram device.
 */
void pi_open_access_to_nvr(struct pi_device *device);

/** \brief Close access to NVR
 *
 * Close access to non-volatile rows. And switch
 * memory accesses to the emram array
 *
 * \param device A pointer to the mram device.
 */
void pi_close_access_to_nvr(struct pi_device *device);

/** \brief Initialize an Mram configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * mram device is opened.
 *
 * \param conf A pointer to the mram configuration.
 */
void pi_mram_conf_init(struct pi_mram_conf *conf);

//!@}

/**
 * @} end of Mram
 */

/**
 * @} end of Flash
 */

#endif
