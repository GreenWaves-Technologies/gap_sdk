/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PMSIS_CHIPS_GAP8_PAD_H__
#define __PMSIS_CHIPS_GAP8_PAD_H__

/**
 * \ingroup groupChips
 */

/**
 * \addtogroup GAP8
 * @{
 */


/**
 * \defgroup GAP8_Padframe GAP8 Padframe
 *
 * This part enumerates GAP8 padframe.
 * There are 48 configurable pads.
 *
 * \addtogroup GAP8_Padframe
 * @{
 */

/**
 * \enum pi_pad_e
 * \brief Pad numbers.
 *
 * This is used to identify pads.
 */
typedef enum {
  PI_PAD_8_A4_RF_SPIM1_MISO        = 0,
  PI_PAD_9_B3_RF_SPIM1_MOSI        = 1,
  PI_PAD_10_A5_RF_SPIM1_CSN        = 2,
  PI_PAD_11_B4_RF_SPIM1_SCK        = 3,
  PI_PAD_12_A3_RF_PACTRL0          = 4,
  PI_PAD_13_B2_RF_PACTRL1          = 5,
  PI_PAD_14_A2_RF_PACTRL2          = 6,
  PI_PAD_15_B1_RF_PACTRL3          = 7,
  PI_PAD_16_A44_RF_PACTRL4         = 8,
  PI_PAD_17_B40_RF_PACTRL5         = 9,
  PI_PAD_18_A43_CAM_PCLK           = 10,
  PI_PAD_19_A37_CAM_HSYNC          = 11,
  PI_PAD_20_B39_CAM_DATA0          = 12,
  PI_PAD_21_A42_CAM_DATA1          = 13,
  PI_PAD_22_B38_CAM_DATA2          = 14,
  PI_PAD_23_A41_CAM_DATA3          = 15,
  PI_PAD_24_B37_CAM_DATA4          = 16,
  PI_PAD_25_A40_CAM_DATA5          = 17,
  PI_PAD_26_B36_CAM_DATA6          = 18,
  PI_PAD_27_A38_CAM_DATA7          = 19,
  PI_PAD_28_A36_CAM_VSYNC          = 20,
  PI_PAD_29_B34_CAM_SDA            = 21,
  PI_PAD_30_D1_CAM_SCL             = 22,
  PI_PAD_31_B11_TIMER0_CH0         = 23,
  PI_PAD_32_A13_TIMER0_CH1         = 24,
  PI_PAD_33_B12_TIMER0_CH2         = 25,
  PI_PAD_34_A14_TIMER0_CH3         = 26,
  PI_PAD_35_B13_I2S1_SCK           = 27,
  PI_PAD_36_A15_I2S1_WS            = 28,
  PI_PAD_37_B14_I2S1_SDI           = 29,
  PI_PAD_38_B6_UART_RX             = 30,
  PI_PAD_39_A7_UART_TX             = 31,
  PI_PAD_40_D2_SPIM0_SDIO0         = 32,
  PI_PAD_41_A11_SPIM0_SDIO1        = 33,
  PI_PAD_42_B10_SPIM0_SDIO2        = 34,
  PI_PAD_43_A10_SPIM0_SDIO3        = 35,
  PI_PAD_44_B8_SPIM0_CSN0          = 36,
  PI_PAD_45_A8_SPIM0_CSN1          = 37,
  PI_PAD_46_B7_SPIM0_SCK           = 38,
  PI_PAD_47_A9_SPIS0_CSN           = 39,
  PI_PAD_48_B15_SPIS0_MISO         = 40,
  PI_PAD_49_A16_SPIS0_MOSI         = 41,
  PI_PAD_50_B9_SPIS0_SCK           = 42,
  PI_PAD_53_B22_I2C0_SDA           = 43,
  PI_PAD_54_A25_I2C0_SCL           = 44,
  PI_PAD_55_A24_I2S0_SCK           = 45,
  PI_PAD_56_A26_I2S0_WS            = 46,
  PI_PAD_57_B23_I2S0_SDI           = 47
} pi_pad_e;

/**
 * @}
 */

#endif  /* __PMSIS_CHIPS_GAP8_PAD_H__ */
