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

#ifndef __BSP__GAP9_V2_H__
#define __BSP__GAP9_V2_H__

#define CONFIG_HIMAX
#define CONFIG_HYPERFLASH
#define CONFIG_MRAM
#define CONFIG_HYPERRAM
#define CONFIG_SPIRAM
#define CONFIG_SPIFLASH
#define CONFIG_24XX1025
#define CONFIG_APS25XXXN
#define CONFIG_VIRTUAL_EEPROM

#define CONFIG_HIMAX_CPI_ITF 0
#define CONFIG_HIMAX_I2C_ITF 1

#define CONFIG_HYPERFLASH_HYPER_ITF 0
#define CONFIG_HYPERFLASH_HYPER_CS  1

#define CONFIG_HYPERRAM_HYPER_ITF 0
#define CONFIG_HYPERRAM_HYPER_CS  0
#define CONFIG_HYPERRAM_START     0
#define CONFIG_HYPERRAM_SIZE     (8<<20)

#define CONFIG_SPIRAM_SPI_ITF   0
#define CONFIG_SPIRAM_SPI_CS    0
#define CONFIG_SPIRAM_START     0
#define CONFIG_SPIRAM_SIZE     (1<<20)

#define CONFIG_APS25XXXN_SPI_ITF   1
#define CONFIG_APS25XXXN_SPI_CS    0
#define CONFIG_APS25XXXN_START     0
#define CONFIG_APS25XXXN_SIZE     (1<<25)

#define CONFIG_SPIFLASH_SPI_ITF     0
#define CONFIG_SPIFLASH_SPI_CS      0
#define CONFIG_SPIFLASH_START       0
#define CONFIG_SPIFLASH_SIZE        (1<<24)
#define CONFIG_SPIFLASH_SECTOR_SIZE (1<<12)

#define CONFIG_24XX1025_I2C_ADDR    0xA0
#define CONFIG_24XX1025_I2C_ITF     0

#define CONFIG_VIRTUAL_EEPROM_I2C_ADDR   0x14
#define CONFIG_VIRTUAL_EEPROM_I2C_ITF    0

#endif
