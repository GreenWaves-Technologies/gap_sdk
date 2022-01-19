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

#pragma once

#include <stdint.h>

#define CONFIG_AK4332

#define CONFIG_AK4332_I2C_ITF 1
#define CONFIG_AK4332_I2S_ITF 2

#ifdef __cplusplus
extern "C" {
#endif

int pi_bsp_ak4332_power_ctrl(int power_enable);

int pi_bsp_ak4332_power_ctrl(int power_enable);

uint8_t pi_bsp_fxl6408_read_id();


/// @cond IMPLEM

void __bsp_audio_addon_init();

#define CONFIG_FXL6408UMX_I2C_ITF 1
#define CONFIG_FXL6408UMX_I2C_ADDR 0x86
#define CONFIG_FXL6408UMX_AK4332_GPIO 1
#define CONFIG_FXL6408UMX_TLV320_GPIO 2
/// @endcond

#ifdef __cplusplus
}
#endif