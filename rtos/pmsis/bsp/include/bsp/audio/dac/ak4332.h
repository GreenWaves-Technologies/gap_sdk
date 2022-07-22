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

#include "pmsis.h"

#pragma once

/**
 * @addtogroup Dac
 * @{
 */

/**
 * @defgroup Ak4332 Ak4332
 *
 * DAC AK4332
 */

/**
 * @addtogroup Ak4332
 * @{
 */

/* @brief Struct holding ak4332 display config. */
struct pi_ak4332_conf
{
    int i2c_itf;    /*!< I2C interface number where the device is connected. */
};

/**
 * @brief Initialize an ak4332 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until
 * the device is opened.
 * It can only be called from fabric-controller side.
 *
 * @param conf           Pointer to the device configuration.
 */
void pi_ak4332_conf_init(struct pi_ak4332_conf *conf);

/** \brief Open a ak4332 device.
 *
 * This function must be called before the ak4332 device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_ak4332_open(pi_device_t *device);

/**
 * @brief Set DAC digital input volume
 *
 * The volume can be set to 0 to mute it or from 0x01 (-12dB) to 0x1F (+3.0dB).
 *
 * @param device Pointer to the device structure.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_ak4332_set_dac_volume(pi_device_t *device, uint8_t volume);

/**
 * @brief Set headphone amplifier volume
 *
 * The volume can be set from 0x00 (-10dB) to 0x7 (+4dB).
 *
 * @param device Pointer to the device structure.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_ak4332_set_hp_volume(pi_device_t *device, uint8_t volume);

/** \brief Close an opened ak4332 device.
 *
 * This function can be called to close an opened ak4332 device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * \param device    The device structure of the device to close.
 */
void pi_ak4332_close(struct pi_device *device);

/**
 * @}
 */

/**
 * @}
 */
