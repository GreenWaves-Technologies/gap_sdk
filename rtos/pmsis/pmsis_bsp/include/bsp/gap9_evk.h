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

#ifndef __BSP__GAP9_EVK_H__
#define __BSP__GAP9_EVK_H__

#if defined(CONFIG_GAP9_EVK_AUDIO_ADDON)
#include "bsp/boards/gap9_evk/audio_addon.h"
#endif

#define CONFIG_HIMAX
#define CONFIG_MRAM
#define CONFIG_24XX1025
#define CONFIG_APS25XXXN
#define CONFIG_VIRTUAL_EEPROM
#define CONFIG_MX25U51245G
#define CONFIG_NINA_B112

#define CONFIG_HIMAX_CPI_ITF 0
#define CONFIG_HIMAX_I2C_ITF 0

#define CONFIG_APS25XXXN_SPI_ITF   0
#define CONFIG_APS25XXXN_SPI_CS    1
#define CONFIG_APS25XXXN_START     0
#define CONFIG_APS25XXXN_SIZE     (1<<25)

#define CONFIG_MX25U51245G_SPI_ITF   0
#define CONFIG_MX25U51245G_SPI_CS    0

#define CONFIG_24XX1025_I2C_ADDR         0xA0
#define CONFIG_24XX1025_I2C_ITF          0
#define CONFIG_24XX1025_I2C_SCL_PAD      PI_PAD_040
#define CONFIG_24XX1025_I2C_SCL_PADFUN   PI_PAD_FUNC0
#define CONFIG_24XX1025_I2C_SDA_PAD      PI_PAD_041
#define CONFIG_24XX1025_I2C_SDA_PADFUN   PI_PAD_FUNC0

#define CONFIG_VIRTUAL_EEPROM_I2C_ADDR   0x14
#define CONFIG_VIRTUAL_EEPROM_I2C_ITF    0

#define CONFIG_NINA_B112_UART_ID             ( 1 )
/* BLE Nina. */
#define GPIOA2_NINA_RST                ( PI_PAD_040)
#define GPIOA21_NINA17                 ( PI_PAD_041)
#define GPIO_NINA_PWRON                ( PI_PAD_042)
#define GPIO_NINA17_DSR                ( PI_PAD_043)

#define pi_default_flash_conf pi_mx25u51245g_conf
#define pi_default_flash_conf_init pi_mx25u51245g_conf_init

#define pi_default_ram_conf pi_aps25xxxn_conf
#define pi_default_ram_conf_init pi_aps25xxxn_conf_init

#endif
