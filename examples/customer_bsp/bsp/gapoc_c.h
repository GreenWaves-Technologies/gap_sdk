/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BSP_GAPOC_C_H__
#define __BSP_GAPOC_C_H__

#define CONFIG_GC0308_CPI_ITF 0
#define CONFIG_GC0308_I2C_ITF 1

#define CONFIG_HYPERFLASH_HYPER_ITF    ( 0 )
#define CONFIG_HYPERFLASH_HYPER_CS     ( 1 )

#define CONFIG_HYPERRAM_HYPER_ITF      ( 0 )
#define CONFIG_HYPERRAM_HYPER_CS       ( 0 )
#define CONFIG_HYPERRAM_START          ( 0 )
#define CONFIG_HYPERRAM_SIZE           ( 8 << 20 )

#define CONFIG_HYPERBUS_DATA6_PAD      ( PI_PAD_46_B7_SPIM0_SCK )
#define CONFIG_UART_RX_PAD_FUNC        ( 0 )
#define CONFIG_HYPERRAM_DATA6_PAD_FUNC ( 3 )

#define CONFIG_SPIRAM_SPI_ITF   0
#define CONFIG_SPIRAM_SPI_CS    1
#define CONFIG_SPIRAM_START     0
#define CONFIG_SPIRAM_SIZE     (8<<20)

#define CONFIG_SPIFLASH_SPI_ITF     0
#define CONFIG_SPIFLASH_SPI_CS      0
#define CONFIG_SPIFLASH_START       0
#define CONFIG_SPIFLASH_SIZE        (16<<20)
#define CONFIG_SPIFLASH_SECTOR_SIZE (1<<12)

#define CONFIG_HM0360_CPI_ITF 0
#define CONFIG_HM0360_I2C_ITF 1


#define GPIO_FUNC1              (1)
#define GPIO_1V8MEM_EN          (PI_GPIO_A0_PAD_12_A3)      
#define GAPA9_WKUPIN            (PI_GPIO_A29_PAD_47_A9)
#define GAP_B11_PBUTTON         (PI_GPIO_A17_PAD_31_B11)
#define GAPB15_LED2             (PI_GPIO_A30_PAD_48_B15)
#define GPIO_ESP32_EN           (PI_GPIO_A23_PAD_37_B14)
#define GPIO_CAM_RESET          (PI_GPIO_A4_PAD_16_A44)
#define CAM_GPIO_A              (PI_GPIO_A1_PAD_13_B2)
#define CAM_GPIO_B              (PI_GPIO_A3_PAD_15_B1)
#define CAM_GPIO_C              (PI_GPIO_A2_PAD_14_A2)

/*LEDs*/

#define PAD_LED2  PI_PAD_48_B15_SPIS0_MISO
#define GPIO_LED2 PI_GPIO_A30_PAD_48_B15
#define FUNC_LED2 PI_PAD_48_B15_GPIO_A30_FUNC1

/*Cameras (GC0308, HM0360, OV5640)*/
#define PAD_CAM_GPIO_A PI_PAD_13_B2_RF_PACTRL1
#define PAD_CAM_GPIO_B PI_PAD_15_B1_RF_PACTRL3
#define PAD_CAM_GPIO_C PI_PAD_14_A2_RF_PACTRL2
#define PAD_CAM_RST    PI_PAD_16_A44_RF_PACTRL4

#define GC0308_GPIO_RST  GPIO_CAM_RESET
#define GC0308_GPIO_PWDN CAM_GPIO_A
#define GC0308_GPIO_MCLK CAM_GPIO_B

#define GC0308_PAD_RST  PAD_CAM_RST
#define GC0308_PAD_PWDN PAD_CAM_GPIO_A
#define GC0308_PAD_MCLK PAD_CAM_GPIO_B

#endif  /* __BSP_GAPOC_C_H__ */
