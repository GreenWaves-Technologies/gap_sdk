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
 * \{
 */

/**
 * \enum pi_pad_e
 * \brief Pad numbers.
 *
 * This is used to identify pads.
 */
typedef enum
{
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
 * \cond IMPLEM
 */
/* Default pad function, alternate 0. */
#define PI_PAD_8_A4_RF_SPIM1_MISO_FUNC0  ( 0 )
#define PI_PAD_9_B3_RF_SPIM1_MOSI_FUNC0  ( 0 )
#define PI_PAD_10_A5_RF_SPIM1_CSN_FUNC0  ( 0 )
#define PI_PAD_11_B4_RF_SPIM1_SCK_FUNC0  ( 0 )
#define PI_PAD_12_A3_RF_PACTRL0_FUNC0    ( 0 )
#define PI_PAD_13_B2_RF_PACTRL1_FUNC0    ( 0 )
#define PI_PAD_14_A2_RF_PACTRL2_FUNC0    ( 0 )
#define PI_PAD_15_B1_RF_PACTRL3_FUNC0    ( 0 )
#define PI_PAD_16_A44_RF_PACTRL4_FUNC0   ( 0 )
#define PI_PAD_17_B40_RF_PACTRL5_FUNC0   ( 0 )
#define PI_PAD_18_A43_CAM_PCLK_FUNC0     ( 0 )
#define PI_PAD_19_A37_CAM_HSYNC_FUNC0    ( 0 )
#define PI_PAD_20_B39_CAM_DATA0_FUNC0    ( 0 )
#define PI_PAD_21_A42_CAM_DATA1_FUNC0    ( 0 )
#define PI_PAD_22_B38_CAM_DATA2_FUNC0    ( 0 )
#define PI_PAD_23_A41_CAM_DATA3_FUNC0    ( 0 )
#define PI_PAD_24_B37_CAM_DATA4_FUNC0    ( 0 )
#define PI_PAD_25_A40_CAM_DATA5_FUNC0    ( 0 )
#define PI_PAD_26_B36_CAM_DATA6_FUNC0    ( 0 )
#define PI_PAD_27_A38_CAM_DATA7_FUNC0    ( 0 )
#define PI_PAD_28_A36_CAM_VSYNC_FUNC0    ( 0 )
#define PI_PAD_29_B34_CAM_SDA_FUNC0      ( 0 )
#define PI_PAD_30_D1_CAM_SCL_FUNC0       ( 0 )
#define PI_PAD_31_B11_TIMER0_CH0_FUNC0   ( 0 )
#define PI_PAD_32_A13_TIMER0_CH1_FUNC0   ( 0 )
#define PI_PAD_33_B12_TIMER0_CH2_FUNC0   ( 0 )
#define PI_PAD_34_A14_TIMER0_CH3_FUNC0   ( 0 )
#define PI_PAD_35_B13_I2S1_SCK_FUNC0     ( 0 )
#define PI_PAD_36_A15_I2S1_WS_FUNC0      ( 0 )
#define PI_PAD_37_B14_I2S1_SDI_FUNC0     ( 0 )
#define PI_PAD_38_B6_UART_RX_FUNC0       ( 0 )
#define PI_PAD_39_A7_UART_TX_FUNC0       ( 0 )
#define PI_PAD_40_D2_SPIM0_SDIO0_FUNC0   ( 0 )
#define PI_PAD_41_A11_SPIM0_SDIO1_FUNC0  ( 0 )
#define PI_PAD_42_B10_SPIM0_SDIO2_FUNC0  ( 0 )
#define PI_PAD_43_A10_SPIM0_SDIO3_FUNC0  ( 0 )
#define PI_PAD_44_B8_SPIM0_CSN0_FUNC0    ( 0 )
#define PI_PAD_45_A8_SPIM0_CSN1_FUNC0    ( 0 )
#define PI_PAD_46_B7_SPIM0_SCK_FUNC0     ( 0 )
#define PI_PAD_47_A9_SPIS0_CSN_FUNC0     ( 0 )
#define PI_PAD_48_B15_SPIS0_MISO_FUNC0   ( 0 )
#define PI_PAD_49_A16_SPIS0_MOSI_FUNC0   ( 0 )
#define PI_PAD_50_B9_SPIS0_SCK_FUNC0     ( 0 )
#define PI_PAD_53_B22_I2C0_SDA_FUNC0     ( 0 )
#define PI_PAD_54_A25_I2C0_SCL_FUNC0     ( 0 )
#define PI_PAD_55_A24_I2S0_SCK_FUNC0     ( 0 )
#define PI_PAD_56_A26_I2S0_WS_FUNC0      ( 0 )
#define PI_PAD_57_B23_I2S0_SDI_FUNC0     ( 0 )

/* Alternate 1, pads as GPIOs. */
#define PI_PAD_8_A4_GPIO_A0_FUNC1        ( 1 )
#define PI_PAD_9_B3_GPIO_A1_FUNC1        ( 1 )
#define PI_PAD_10_A5_GPIO_A2_FUNC1       ( 1 )
#define PI_PAD_11_B4_GPIO_A3_FUNC1       ( 1 )
#define PI_PAD_12_A3_GPIO_A0_FUNC1       ( 1 )
#define PI_PAD_13_B2_GPIO_A1_FUNC1       ( 1 )
#define PI_PAD_14_A2_GPIO_A2_FUNC1       ( 1 )
#define PI_PAD_15_B1_GPIO_A3_FUNC1       ( 1 )
#define PI_PAD_16_A44_GPIO_A4_FUNC1      ( 1 )
#define PI_PAD_17_B40_GPIO_A5_FUNC1      ( 1 )
#define PI_PAD_18_A43_GPIO_A4_FUNC1      ( 1 )
#define PI_PAD_19_A37_GPIO_A5_FUNC1      ( 1 )
#define PI_PAD_20_B39_GPIO_A6_FUNC1      ( 1 )
#define PI_PAD_21_A42_GPIO_A7_FUNC1      ( 1 )
#define PI_PAD_22_B38_GPIO_A8_FUNC1      ( 1 )
#define PI_PAD_23_A41_GPIO_A9_FUNC1      ( 1 )
#define PI_PAD_24_B37_GPIO_A10_FUNC1     ( 1 )
#define PI_PAD_25_A40_GPIO_A11_FUNC1     ( 1 )
#define PI_PAD_26_B36_GPIO_A12_FUNC1     ( 1 )
#define PI_PAD_27_A38_GPIO_A13_FUNC1     ( 1 )
#define PI_PAD_28_A36_GPIO_A14_FUNC1     ( 1 )
#define PI_PAD_29_B34_GPIO_A15_FUNC1     ( 1 )
#define PI_PAD_30_D1_GPIO_A16_FUNC1      ( 1 )
#define PI_PAD_31_B11_GPIO_A17_FUNC1     ( 1 )
#define PI_PAD_32_A13_GPIO_A18_FUNC1     ( 1 )
#define PI_PAD_33_B12_GPIO_A19_FUNC1     ( 1 )
#define PI_PAD_34_A14_GPIO_A20_FUNC1     ( 1 )
#define PI_PAD_35_B13_GPIO_A21_FUNC1     ( 1 )
#define PI_PAD_36_A15_GPIO_A22_FUNC1     ( 1 )
#define PI_PAD_37_B14_GPIO_A23_FUNC1     ( 1 )
#define PI_PAD_38_B6_GPIO_A24_FUNC1      ( 1 )
#define PI_PAD_39_A7_GPIO_A25_FUNC1      ( 1 )
#define PI_PAD_42_B10_GPIO_A26_FUNC1     ( 1 )
#define PI_PAD_43_A10_GPIO_A27_FUNC1     ( 1 )
#define PI_PAD_45_A8_GPIO_A28_FUNC1      ( 1 )
#define PI_PAD_47_A9_GPIO_A29_FUNC1      ( 1 )
#define PI_PAD_48_B15_GPIO_A30_FUNC1     ( 1 )
#define PI_PAD_49_A16_GPIO_A31_FUNC1     ( 1 )

/* Alternate 2. */
#define PI_PAD_10_A5_I2C1_SDA_FUNC2      ( 2 )
#define PI_PAD_11_B4_I2C1_SCL_FUNC2      ( 2 )
#define PI_PAD_12_A3_SPIM1_CS0_FUNC2     ( 2 )
#define PI_PAD_13_B2_SPIM1_CS1_FUNC2     ( 2 )
#define PI_PAD_16_A44_SPIS0_SDIO0_FUNC2  ( 2 )
#define PI_PAD_17_B40_SPIS0_SDIO1_FUNC2  ( 2 )
#define PI_PAD_18_A43_TIMER1_CH0_FUNC2   ( 2 )
#define PI_PAD_19_A37_TIMER1_CH1_FUNC2   ( 2 )
#define PI_PAD_20_B39_TIMER1_CH2_FUNC2   ( 2 )
#define PI_PAD_21_A42_TIMER1_CH3_FUNC2   ( 2 )
#define PI_PAD_22_B38_TIMER2_CH0_FUNC2   ( 2 )
#define PI_PAD_23_A41_TIMER2_CH1_FUNC2   ( 2 )
#define PI_PAD_24_B37_TIMER2_CH2_FUNC2   ( 2 )
#define PI_PAD_25_A40_TIMER2_CH3_FUNC2   ( 2 )
#define PI_PAD_26_B36_TIMER3_CH0_FUNC2   ( 2 )
#define PI_PAD_27_A38_TIMER3_CH1_FUNC2   ( 2 )
#define PI_PAD_28_A36_TIMER3_CH2_FUNC2   ( 2 )
#define PI_PAD_29_B34_TIMER3_CH3_FUNC2   ( 2 )
#define PI_PAD_30_D1_ORCA_CLK_FUNC2      ( 2 )
#define PI_PAD_32_A13_TIMER1_CH0_FUNC2   ( 2 )
#define PI_PAD_33_B12_TIMER2_CH0_FUNC2   ( 2 )
#define PI_PAD_34_A14_TIMER3_CH0_FUNC2   ( 2 )
#define PI_PAD_35_B13_SPIS0_SCK_FUNC2    ( 2 )
#define PI_PAD_36_A15_SPIS0_CS_FUNC2     ( 2 )
#define PI_PAD_37_B14_SPIS0_SDIO2_FUNC2  ( 2 )
#define PI_PAD_42_B10_I2C1_SDA_FUNC2     ( 2 )
#define PI_PAD_43_A10_I2C1_SCL_FUNC2     ( 2 )
#define PI_PAD_45_A8_SPIS0_SDIO3_FUNC2   ( 2 )
#define PI_PAD_47_A9_SPIM1_CS0_FUNC2     ( 2 )
#define PI_PAD_48_B15_SPIM1_CS1_FUNC2    ( 2 )

/* Alternate 3. */
#define PI_PAD_16_A44_SPIS0_SDIO2_FUNC3  ( 3 )
#define PI_PAD_17_B40_SPIS0_SDIO3_FUNC3  ( 3 )
#define PI_PAD_35_B13_I2S1_SDI_FUNC3     ( 3 )
#define PI_PAD_36_A15_HYPER_CKN_FUNC3    ( 3 )
#define PI_PAD_37_B14_HYPER_CK_FUNC3     ( 3 )
#define PI_PAD_40_D2_HYPER_DQ0_FUNC3     ( 3 )
#define PI_PAD_41_A11_HYPER_DQ1_FUNC3    ( 3 )
#define PI_PAD_42_B10_HYPER_DQ2_FUNC3    ( 3 )
#define PI_PAD_43_A10_HYPER_DQ3_FUNC3    ( 3 )
#define PI_PAD_44_B8_HYPER_DQ4_FUNC3     ( 3 )
#define PI_PAD_45_A8_HYPER_DQ5_FUNC3     ( 3 )
#define PI_PAD_46_B7_HYPER_DQ6_FUNC3     ( 3 )
#define PI_PAD_47_A9_HYPER_DQ7_FUNC3     ( 3 )
#define PI_PAD_48_B15_HYPER_CSN0_FUNC3   ( 3 )
#define PI_PAD_49_A16_HYPER_CSN1_FUNC3   ( 3 )
#define PI_PAD_50_B9_HYPER_RWDS_FUNC3    ( 3 )

/**
 * \endcond IMPLEM
 */


/**
 * \}
 */

/**
 * \}
 */

#endif  /* __PMSIS_CHIPS_GAP8_PAD_H__ */
