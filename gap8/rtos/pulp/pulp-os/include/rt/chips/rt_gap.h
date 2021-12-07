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

#ifndef __RT_CHIPS_RT_GAP_H__
#define __RT_CHIPS_RT_GAP_H__

#ifdef CONFIG_GAP

/// @cond IMPLEM

#define __RT_GPIO_PULL_BIT 0
#define __RT_GPIO_STRENGTH_BIT 2

/// @endcond

/**
 * @addtogroup GPIO
 * @{
 */

/** \enum rt_gpio_conf_e
 * \brief Possible parameters which can be set through the rt_gpio_configure API function.
 *
 * This is used to configure GPIO aspects.
 */
typedef enum {
  RT_GPIO_PULL_DISABLED = 1 << __RT_GPIO_PULL_BIT, /*!< Disable pull. */
  RT_GPIO_PULL_ENABLED  = 2 << __RT_GPIO_PULL_BIT, /*!< Enable pull. */
  RT_GPIO_STRENGTH_LOW  = 1 << __RT_GPIO_STRENGTH_BIT, /*!< Set low drive strength. */
  RT_GPIO_STRENGTH_HIGH = 2 << __RT_GPIO_STRENGTH_BIT, /*!< Set high drive strength. */
} rt_gpio_conf_e;


//!@}



/**
 * @addtogroup PADFRAME
 * @{
 */

/** \enum rt_pad_e
 * \brief List of pads which can be specified when a pad is being configured.
 */
typedef enum {
  PAD_8_RF_MISO       = 0,
  PAD_9_RF_MOSI       = 1,
  PAD_10_RF_CSN       = 2,
  PAD_11_RF_SCK       = 3,
  PAD_12_RF_PACTRL0   = 4,
  PAD_13_RF_PACTRL1   = 5,
  PAD_14_RF_PACTRL2   = 6,
  PAD_15_RF_PACTRL3   = 7,
  PAD_16_RF_PACTRL4   = 8,
  PAD_17_RF_PACTRL5   = 9,
  PAD_18_CAM_PCLK     = 10,
  PAD_19_CAM_HSYNC    = 11,
  PAD_20_CAM_DATA0    = 12,
  PAD_21_CAM_DATA1    = 13,
  PAD_22_CAM_DATA2    = 14,
  PAD_23_CAM_DATA3    = 15,
  PAD_24_CAM_DATA4    = 16,
  PAD_25_CAM_DATA5    = 17,
  PAD_26_CAM_DATA6    = 18,
  PAD_27_CAM_DATA7    = 19,
  PAD_28_CAM_VSYNC    = 20,
  PAD_29_CAM_SDA      = 21,
  PAD_30_CAM_SCL      = 22,
  PAD_31_TIMER0_CH0   = 23,
  PAD_32_TIMER0_CH1   = 24,
  PAD_33_TIMER0_CH2   = 25,
  PAD_34_TIMER0_CH3   = 26,
  PAD_35_I2S1_SCK     = 27,
  PAD_36_I2S1_WS      = 28,
  PAD_37_I2S1_SDI     = 29,
  PAD_38_UART_RX      = 30,
  PAD_39_UART_TX      = 31,
  PAD_40_SPIM0_SDIO0  = 32,
  PAD_41_SPIM0_SDIO1  = 33,
  PAD_42_SPIM0_SDIO2  = 34,
  PAD_43_SPIM0_SDIO3  = 35,
  PAD_44_SPIM0_CSN0   = 36,
  PAD_45_SPIM0_CSN1   = 37,
  PAD_46_SPIM0_SCK    = 38,
  PAD_47_SPIS0_CSN    = 39,
  PAD_48_SPIS0_MISO   = 40,
  PAD_49_SPIS0_MOSI   = 41,
  PAD_50_SPIS0_SCK    = 42,
} rt_pad_e;

/** \enum rt_pad_func_e
 * \brief List of pad functions which can be specified when a pad is being configured.
 *
 * This can be used to specify which function a pad is using.
 */
typedef enum {
  PAD_8_FUNC0_SPIM1_MISO    = 0,
  PAD_9_FUNC0_SPIM1_MOSI    = 0,
  PAD_10_FUNC0_SPIM1_CS0    = 0,
  PAD_11_FUNC0_SPIM1_SCK    = 0,
  PAD_12_FUNC0_ORCA_TXSYNC  = 0,
  PAD_13_FUNC0_ORCA_RXSYNC  = 0,
  PAD_14_FUNC0_ORCA_TXI     = 0,
  PAD_15_FUNC0_ORCA_TXQ     = 0,
  PAD_16_FUNC0_ORCA_RXI     = 0,
  PAD_17_FUNC0_ORCA_RXQ     = 0,
  PAD_18_FUNC0_CAM_PCLK     = 0,
  PAD_19_FUNC0_CAM_HSYNC    = 0,
  PAD_20_FUNC0_CAM_DATA0    = 0,
  PAD_21_FUNC0_CAM_DATA1    = 0,
  PAD_22_FUNC0_CAM_DATA2    = 0,
  PAD_23_FUNC0_CAM_DATA3    = 0,
  PAD_24_FUNC0_CAM_DATA4    = 0,
  PAD_25_FUNC0_CAM_DATA5    = 0,
  PAD_26_FUNC0_CAM_DATA6    = 0,
  PAD_27_FUNC0_CAM_DATA7    = 0,
  PAD_28_FUNC0_CAM_VSYNC    = 0,
  PAD_29_FUNC0_I2C1_SDA     = 0,
  PAD_30_FUNC0_I2C1_SCL     = 0,
  PAD_31_FUNC0_TIMER0_CH0   = 0,
  PAD_32_FUNC0_TIMER0_CH1   = 0,
  PAD_33_FUNC0_TIMER0_CH2   = 0,
  PAD_34_FUNC0_TIMER0_CH3   = 0,
  PAD_35_FUNC0_I2S1_SCK     = 0,
  PAD_36_FUNC0_I2S1_WS      = 0,
  PAD_37_FUNC0_I2S1_SDI     = 0,
  PAD_38_FUNC0_UART_RX      = 0,
  PAD_39_FUNC0_UART_TX      = 0,
  PAD_40_FUNC0_SPIM0_SDIO0  = 0,
  PAD_41_FUNC0_SPIM0_SDIO1  = 0,
  PAD_42_FUNC0_SPIM0_SDIO2  = 0,
  PAD_43_FUNC0_SPIM0_SDIO3  = 0,
  PAD_44_FUNC0_SPIM0_CS0    = 0,
  PAD_45_FUNC0_SPIM0_CS1    = 0,
  PAD_46_FUNC0_SPIM0_SCK    = 0,
  PAD_47_FUNC0_SPIS0_CS     = 0,
  PAD_48_FUNC0_SPIS0_SDIO0  = 0,
  PAD_49_FUNC0_SPIS0_SDIO1  = 0,
  PAD_50_FUNC0_SPIS0_SCK    = 0,

  PAD_8_FUNC1_GPIOA0        = 1,
  PAD_9_FUNC1_GPIOA1        = 1,
  PAD_10_FUNC1_GPIOA2       = 1,
  PAD_11_FUNC1_GPIOA3       = 1,
  PAD_12_FUNC1_GPIOA0       = 1,
  PAD_13_FUNC1_GPIOA1       = 1,
  PAD_14_FUNC1_GPIOA2       = 1,
  PAD_15_FUNC1_GPIOA3       = 1,
  PAD_16_FUNC1_GPIOA4       = 1,
  PAD_17_FUNC1_GPIOA5       = 1,
  PAD_18_FUNC1_GPIOA4       = 1,
  PAD_19_FUNC1_GPIOA5       = 1,
  PAD_20_FUNC1_GPIOA6       = 1,
  PAD_21_FUNC1_GPIOA7       = 1,
  PAD_22_FUNC1_GPIOA8       = 1,
  PAD_23_FUNC1_GPIOA9       = 1,
  PAD_24_FUNC1_GPIOA10      = 1,
  PAD_25_FUNC1_GPIOA11      = 1,
  PAD_26_FUNC1_GPIOA12      = 1,
  PAD_27_FUNC1_GPIOA13      = 1,
  PAD_28_FUNC1_GPIOA14      = 1,
  PAD_29_FUNC1_GPIOA15      = 1,
  PAD_30_FUNC1_GPIOA16      = 1,
  PAD_31_FUNC1_GPIOA17      = 1,
  PAD_32_FUNC1_GPIOA18      = 1,
  PAD_33_FUNC1_GPIOA19      = 1,
  PAD_34_FUNC1_GPIOA20      = 1,
  PAD_35_FUNC1_GPIOA21      = 1,
  PAD_36_FUNC1_GPIOA22      = 1,
  PAD_37_FUNC1_GPIOA23      = 1,
  PAD_38_FUNC1_GPIOA24      = 1,
  PAD_39_FUNC1_GPIOA25      = 1,
  PAD_42_FUNC1_GPIOA26      = 1,
  PAD_43_FUNC1_GPIOA27      = 1,
  PAD_45_FUNC1_GPIOA28      = 1,
  PAD_47_FUNC1_GPIOA29      = 1,
  PAD_48_FUNC1_GPIOA30      = 1,
  PAD_49_FUNC1_GPIOA31      = 1,

  PAD_10_FUNC2_I2C1_SDA     = 2,
  PAD_11_FUNC2_I2C1_SCL     = 2,
  PAD_12_FUNC2_SPIM1_CS0    = 2,
  PAD_13_FUNC2_SPIM1_CS1    = 2,
  PAD_16_FUNC2_SPIS0_SDIO0  = 2,
  PAD_17_FUNC2_SPIS0_SDIO1  = 2,
  PAD_18_FUNC2_TIMER1_CH0   = 2,
  PAD_19_FUNC2_TIMER1_CH1   = 2,
  PAD_20_FUNC2_TIMER1_CH2   = 2,
  PAD_21_FUNC2_TIMER1_CH3   = 2,
  PAD_22_FUNC2_TIMER2_CH0   = 2,
  PAD_23_FUNC2_TIMER2_CH1   = 2,
  PAD_24_FUNC2_TIMER2_CH2   = 2,
  PAD_25_FUNC2_TIMER2_CH3   = 2,
  PAD_26_FUNC2_TIMER3_CH0   = 2,
  PAD_27_FUNC2_TIMER3_CH1   = 2,
  PAD_28_FUNC2_TIMER3_CH2   = 2,
  PAD_29_FUNC2_TIMER3_CH3   = 2,
  PAD_30_FUNC2_ORCA_CLK     = 2,
  PAD_32_FUNC2_TIMER1_CH0   = 2,
  PAD_33_FUNC2_TIMER2_CH0   = 2,
  PAD_34_FUNC2_TIMER3_CH0   = 2,
  PAD_35_FUNC2_SPIS0_SCK    = 2,
  PAD_36_FUNC2_SPIS0_CS     = 2,
  PAD_37_FUNC2_SPIS0_SDIO2  = 2,
  PAD_42_FUNC2_I2C1_SDA     = 2,
  PAD_43_FUNC2_I2C1_SCL     = 2,
  PAD_45_FUNC2_SPIS0_SDIO3  = 2,
  PAD_47_FUNC2_SPIM1_CS0    = 2,
  PAD_48_FUNC2_SPIM1_CS1    = 2,

  PAD_16_FUNC3_SPIS0_SDIO2  = 3,
  PAD_17_FUNC3_SPIS0_SDIO3  = 3,
  PAD_35_FUNC3_I2S1_SDI     = 3,
  PAD_36_FUNC3_HYPER_CKN    = 3,
  PAD_37_FUNC3_HYPER_CK     = 3,
  PAD_40_FUNC3_HYPER_DQ0    = 3,
  PAD_41_FUNC3_HYPER_DQ1    = 3,
  PAD_42_FUNC3_HYPER_DQ2    = 3,
  PAD_43_FUNC3_HYPER_DQ3    = 3,
  PAD_44_FUNC3_HYPER_DQ4    = 3,
  PAD_45_FUNC3_HYPER_DQ5    = 3,
  PAD_46_FUNC3_HYPER_DQ6    = 3,
  PAD_47_FUNC3_HYPER_DQ7    = 3,
  PAD_48_FUNC3_HYPER_CSN0   = 3,
  PAD_49_FUNC3_HYPER_CSN1   = 3,
  PAD_50_FUNC3_HYPER_RWDS   = 3,
} rt_pad_func_e;

//!@}


#endif

#endif