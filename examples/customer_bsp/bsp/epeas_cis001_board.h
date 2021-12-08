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

#ifndef __BSP_EPEAS_CIS001_BOARD_H__
#define __BSP_EPEAS_CIS001_BOARD_H__

#define CONFIG_EPEAS_CIS001_CPI_ITF      0
#define CONFIG_EPEAS_CIS001_SPI_ITF      1
#define CONFIG_EPEAS_CIS001_SPI_CS       0
#define CONFIG_EPEAS_CIS001_PWM_ID       0
#define CONFIG_EPEAS_CIS001_PWM_CH       1

#define CONFIG_HYPERBUS_DATA6_PAD        (PI_PAD_46_B7_SPIM0_SCK)
#define CONFIG_UART_RX_PAD_FUNC          (0)
#define CONFIG_HYPERRAM_DATA6_PAD_FUNC   (3)

#define CONFIG_SPIRAM_SPI_ITF            0
#define CONFIG_SPIRAM_SPI_CS             1 
#define CONFIG_SPIRAM_START              0
#define CONFIG_SPIRAM_SIZE               (8<<20)

#define CONFIG_SPIFLASH_SPI_ITF          0
#define CONFIG_SPIFLASH_SPI_CS           0
#define CONFIG_SPIFLASH_START            0
#define CONFIG_SPIFLASH_SIZE             (16<<20)
#define CONFIG_SPIFLASH_SECTOR_SIZE      (1<<12)

#define GPIO_FUNC1                       (1) 
#define GPIO_CAM_RESET                   (PI_GPIO_A4_PAD_16_A44)
#define GPIO_CAM_CAPTURE                 (PI_GPIO_A17_PAD_31_B11)


/*LEDs*/


/*Camera */
#define PAD_CAM_RESET                    (PI_PAD_16_A44_RF_PACTRL4)
#define PAD_CAM_CAPTURE                  (PI_PAD_31_B11_TIMER0_CH0)

#define EPEAS_CIS001_GPIO_RESET          GPIO_CAM_RESET
#define EPEAS_CIS001_GPIO_CAPTURE        GPIO_CAM_CAPTURE

#define EPEAS_CIS001_PAD_RST             PAD_CAM_RESET
#define EPEAS_CIS001_PAD_CAPTURE         PAD_CAM_CAPTURE


#endif  /* __BSP_EPEAS_CIS001_BOARD_H__ */
