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

#ifndef __CHIPS_GAP8_DRIVERS_GPIO_H__
#define __CHIPS_GAP8_DRIVERS_GPIO_H__

#include "pmsis/chips/gap8/pad.h"

/**
 * @ingroup groupChips
 */

/**
 * @addtogroup GAP8
 * @{
 */

/**
 * @defgroup GAP8_GPIO GAP8 GPIOs
 *
 * This part enumerates available GPIOs name on chip GAP8.
 * There are a total of 32 GPIOs.
 */

/**
 * @}
 */

#define PI_GPIO_NUM_SHIFT     0
#define PI_GPIO_NUM_MASK      0xFF
#define PI_GPIO_PAD_SHIFT     8
#define PI_GPIO_PAD_MASK      0xFF00
#define PI_GPIO_IS_GPIO_SHIFT 31
#define PI_GPIO_IS_GPIO_MASK  0x80000000

/**
 * @addtogroup GAP8_GPIO
 * @{
 */

/**
 * @{
 */

/**
 * \enum pi_gpio_e
 * \brief GPIO pins.
 *
 * List of available GPIO pins.
 * This is used to identify pads.
 *
 * GPIO :
 *
 * |    31   |         |15      8|7        0|
 * |---------|---------|---------|----------|
 * | IS_GPIO |         | PAD_NUM | GPIO_NUM |
 */
typedef enum
{
    PI_GPIO_A0_PAD_8_A4    = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_8_A4_RF_SPIM1_MISO << PI_GPIO_PAD_SHIFT) | 0), /*!< GPIO_A0 */
    PI_GPIO_A1_PAD_9_B3    = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_9_B3_RF_SPIM1_MOSI << PI_GPIO_PAD_SHIFT) | 1), /*!< GPIO_A1 */
    PI_GPIO_A2_PAD_10_A5   = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_10_A5_RF_SPIM1_CSN << PI_GPIO_PAD_SHIFT) | 2), /*!< GPIO_A2 */
    PI_GPIO_A3_PAD_11_B4   = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_11_B4_RF_SPIM1_SCK << PI_GPIO_PAD_SHIFT) | 3), /*!< GPIO_A3 */
    PI_GPIO_A0_PAD_12_A3   = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_12_A3_RF_PACTRL0   << PI_GPIO_PAD_SHIFT) | 0), /*!< GPIO_A0 */
    PI_GPIO_A1_PAD_13_B2   = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_13_B2_RF_PACTRL1   << PI_GPIO_PAD_SHIFT) | 1), /*!< GPIO_A1 */
    PI_GPIO_A2_PAD_14_A2   = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_14_A2_RF_PACTRL2   << PI_GPIO_PAD_SHIFT) | 2), /*!< GPIO_A2 */
    PI_GPIO_A3_PAD_15_B1   = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_15_B1_RF_PACTRL3   << PI_GPIO_PAD_SHIFT) | 3), /*!< GPIO_A3 */
    PI_GPIO_A4_PAD_16_A44  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_16_A44_RF_PACTRL4  << PI_GPIO_PAD_SHIFT) | 4), /*!< GPIO_A4 */
    PI_GPIO_A5_PAD_17_B40  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_17_B40_RF_PACTRL5  << PI_GPIO_PAD_SHIFT) | 5), /*!< GPIO_A5 */
    PI_GPIO_A4_PAD_18_A43  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_18_A43_CAM_PCLK    << PI_GPIO_PAD_SHIFT) | 4), /*!< GPIO_A4 */
    PI_GPIO_A5_PAD_19_A37  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_19_A37_CAM_HSYNC   << PI_GPIO_PAD_SHIFT) | 5), /*!< GPIO_A5 */
    PI_GPIO_A6_PAD_20_B39  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_20_B39_CAM_DATA0   << PI_GPIO_PAD_SHIFT) | 6), /*!< GPIO_A6 */
    PI_GPIO_A7_PAD_21_A42  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_21_A42_CAM_DATA1   << PI_GPIO_PAD_SHIFT) | 7), /*!< GPIO_A7 */
    PI_GPIO_A8_PAD_22_B38  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_22_B38_CAM_DATA2   << PI_GPIO_PAD_SHIFT) | 8), /*!< GPIO_A8 */
    PI_GPIO_A9_PAD_23_A41  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_23_A41_CAM_DATA3   << PI_GPIO_PAD_SHIFT) | 9), /*!< GPIO_A9 */
    PI_GPIO_A10_PAD_24_B37 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_24_B37_CAM_DATA4   << PI_GPIO_PAD_SHIFT) | 10), /*!< GPIO_A10 */
    PI_GPIO_A11_PAD_25_A40 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_25_A40_CAM_DATA5   << PI_GPIO_PAD_SHIFT) | 11), /*!< GPIO_A11 */
    PI_GPIO_A12_PAD_26_B36 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_26_B36_CAM_DATA6   << PI_GPIO_PAD_SHIFT) | 12), /*!< GPIO_A12 */
    PI_GPIO_A13_PAD_27_A38 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_27_A38_CAM_DATA7   << PI_GPIO_PAD_SHIFT) | 13), /*!< GPIO_A13 */
    PI_GPIO_A14_PAD_28_A36 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_28_A36_CAM_VSYNC   << PI_GPIO_PAD_SHIFT) | 14), /*!< GPIO_A14 */
    PI_GPIO_A15_PAD_29_B34 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_29_B34_CAM_SDA     << PI_GPIO_PAD_SHIFT) | 15), /*!< GPIO_A15 */
    PI_GPIO_A16_PAD_30_D1  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_30_D1_CAM_SCL      << PI_GPIO_PAD_SHIFT) | 16), /*!< GPIO_A16 */
    PI_GPIO_A17_PAD_31_B11 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_31_B11_TIMER0_CH0  << PI_GPIO_PAD_SHIFT) | 17), /*!< GPIO_A17 */
    PI_GPIO_A18_PAD_32_A13 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_32_A13_TIMER0_CH1  << PI_GPIO_PAD_SHIFT) | 18), /*!< GPIO_A18 */
    PI_GPIO_A19_PAD_33_B12 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_33_B12_TIMER0_CH2  << PI_GPIO_PAD_SHIFT) | 19), /*!< GPIO_A19 */
    PI_GPIO_A20_PAD_34_A14 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_34_A14_TIMER0_CH3  << PI_GPIO_PAD_SHIFT) | 20), /*!< GPIO_A20 */
    PI_GPIO_A21_PAD_35_B13 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_35_B13_I2S1_SCK    << PI_GPIO_PAD_SHIFT) | 21), /*!< GPIO_A21 */
    PI_GPIO_A22_PAD_36_A15 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_36_A15_I2S1_WS     << PI_GPIO_PAD_SHIFT) | 22), /*!< GPIO_A22 */
    PI_GPIO_A23_PAD_37_B14 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_37_B14_I2S1_SDI    << PI_GPIO_PAD_SHIFT) | 23), /*!< GPIO_A23 */
    PI_GPIO_A24_PAD_38_B6  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_38_B6_UART_RX      << PI_GPIO_PAD_SHIFT) | 24), /*!< GPIO_A24 */
    PI_GPIO_A25_PAD_39_A7  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_39_A7_UART_TX      << PI_GPIO_PAD_SHIFT) | 25), /*!< GPIO_A25 */
    PI_GPIO_A26_PAD_42_B10 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_42_B10_SPIM0_SDIO2 << PI_GPIO_PAD_SHIFT) | 26), /*!< GPIO_A26 */
    PI_GPIO_A27_PAD_43_A10 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_43_A10_SPIM0_SDIO3 << PI_GPIO_PAD_SHIFT) | 27), /*!< GPIO_A27 */
    PI_GPIO_A28_PAD_45_A8  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_45_A8_SPIM0_CSN1   << PI_GPIO_PAD_SHIFT) | 28), /*!< GPIO_A28 */
    PI_GPIO_A29_PAD_47_A9  = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_47_A9_SPIS0_CSN    << PI_GPIO_PAD_SHIFT) | 29), /*!< GPIO_A29 */
    PI_GPIO_A30_PAD_48_B15 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_48_B15_SPIS0_MISO  << PI_GPIO_PAD_SHIFT) | 30), /*!< GPIO_A30 */
    PI_GPIO_A31_PAD_49_A16 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_49_A16_SPIS0_MOSI  << PI_GPIO_PAD_SHIFT) | 31) /*!< GPIO_A31 */
} pi_gpio_e;

/**
 * !@}
 */

/**
 * @}
 */

#endif  /* __CHIPS_GAP8_DRIVERS_GPIO_H__ */
