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
 * @addtogroup ADC
 * @{
 */

/**
 * @defgroup tlv320 tlv320
 *
 * DAC tlv320
 */

/**
 * @addtogroup tlv320
 * @{
 */

/* @brief Struct holding tlv320 display config. */
struct pi_tlv320_conf
{
    int i2c_itf;
};

/**
 * @brief Initialize an tlv320 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until
 * the device is opened.
 * It can only be called from fabric-controller side.
 *
 * @param conf           Pointer to the device configuration.
 */
void pi_tlv320_conf_init(struct pi_tlv320_conf *conf);

int pi_tlv320_open(pi_device_t *device);

void pi_tlv320_close(struct pi_device *device);

/**
 * @}
 */

/**
 * @}
 */
