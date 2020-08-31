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

#ifndef __BSP__GAPUINO_H__
#define __BSP__GAPUINO_H__

#define CONFIG_HIMAX
#define CONFIG_OV7670
#define CONFIG_GC0308
#define CONFIG_OV5640
#define CONFIG_NINA_W10
#define CONFIG_ILI9341
#define CONFIG_HYPERFLASH
#define CONFIG_HYPERRAM
#define CONFIG_SPIRAM
#define CONFIG_SPIFLASH
#define CONFIG_PIXART

#define CONFIG_HIMAX_CPI_ITF 0
#define CONFIG_HIMAX_I2C_ITF 1

#define CONFIG_OV7670_CPI_ITF 0
#define CONFIG_OV7670_I2C_ITF 1

#define CONFIG_GC0308_CPI_ITF 0
#define CONFIG_GC0308_I2C_ITF 1

#define CONFIG_OV5640_CPI_ID         ( 0 )
#define CONFIG_OV5640_I2C_ID         ( 1 )

#define CONFIG_NINA_W10_SPI_ITF            1
#define CONFIG_NINA_W10_SPI_CS             0
#define CONFIG_NINA_GPIO_NINA_ACK          18
#define CONFIG_NINA_GPIO_NINA_ACK_PAD      PI_PAD_32_A13_TIMER0_CH1
#define CONFIG_NINA_GPIO_NINA_ACK_PAD_FUNC PI_PAD_32_A13_GPIO_A18_FUNC1
#define CONFIG_NINA_GPIO_NINA_NOTIF          3
#define CONFIG_NINA_GPIO_NINA_NOTIF_PAD      PI_PAD_15_B1_RF_PACTRL3
#define CONFIG_NINA_GPIO_NINA_NOTIF_PAD_FUNC PI_PAD_15_B1_GPIO_A3_FUNC1

#define CONFIG_ILI9341_SPI_ITF       1
#define CONFIG_ILI9341_SPI_CS        0
#define CONFIG_ILI9341_GPIO          19
#define CONFIG_ILI9341_GPIO_PAD      PI_PAD_33_B12_TIMER0_CH2
#define CONFIG_ILI9341_GPIO_PAD_FUNC PI_PAD_33_B12_GPIO_A19_FUNC1

#define CONFIG_HYPERFLASH_HYPER_ITF 0
#define CONFIG_HYPERFLASH_HYPER_CS  1

#define CONFIG_HYPERRAM_HYPER_ITF 0
#define CONFIG_HYPERRAM_HYPER_CS  0
#define CONFIG_HYPERRAM_START     0
#define CONFIG_HYPERRAM_SIZE     (8<<20)

#define CONFIG_SPIRAM_SPI_ITF   0
#define CONFIG_SPIRAM_SPI_CS    1
#define CONFIG_SPIRAM_START     0
#define CONFIG_SPIRAM_SIZE     (1<<20)

#define CONFIG_SPIFLASH_SPI_ITF     0
#define CONFIG_SPIFLASH_SPI_CS      0
#define CONFIG_SPIFLASH_START       0
#define CONFIG_SPIFLASH_SIZE        (1<<24)
#define CONFIG_SPIFLASH_SECTOR_SIZE (1<<12)

#define CONFIG_PIXART_CPI_ITF          ( 0 )
#define CONFIG_PIXART_SPI_ITF          ( 1 )
#define CONFIG_PIXART_SPI_CS           ( 0 )
#define CONFIG_PIXART_PWM_ID           ( 0 )
#define CONFIG_PIXART_PWM_CH           ( 2 )
#define CONFIG_PIXART_GPIO_PWR_4V      ( GPIO_VANA_4V )
#define CONFIG_PIXART_GPIO_PWR_2V5     ( GPIO_VSENSOR_2V5 )
#define CONFIG_PIXART_GPIO_RESET       ( GPIO_SENSOR_NRST )

#define GPIO_VANA_4V                   ( PI_GPIO_A5_PAD_17_B40 )
#define GPIO_VSENSOR_2V5               ( PI_GPIO_A16_PAD_30_D1 )
#define GPIO_SENSOR_NRST               ( PI_GPIO_A2_PAD_14_A2 )

#if defined(CHIP_VERSION)
#if (CHIP_VERSION == 2)
#define GPIO_USER_LED                        ( PI_GPIO_A3_PAD_15_B1 )
#endif  /* CHIP_VERSION */
#endif  /* CHIP_VERSION */

#define PI_BSP_PROFILE_GAPUINO_0 0   // Default profile
#define PI_BSP_PROFILE_GAPUINO_1 1   // I2S0 and I2S1 with different clock

#endif
