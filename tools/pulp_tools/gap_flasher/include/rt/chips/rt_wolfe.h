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

#ifndef __RT_CHIPS_RT_WOLFE_H__
#define __RT_CHIPS_RT_WOLFE_H__

#ifdef CONFIG_WOLFE

/// @cond IMPLEM

#define __RT_GPIO_PULL_BIT 0

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
  PAD_0_SPIM_SDIO0  = 0,
  PAD_1_SPIM_SDIO1  = 1,
  PAD_2_SPIM_SDIO2  = 2,
  PAD_3_SPIM_SDIO3  = 3,
  PAD_4_SPIM_CSN0   = 4,
  PAD_5_SPIM_CSN1   = 5,
  PAD_6_SPIM_SCK    = 6,
  PAD_7_UART_RX     = 7,
  PAD_8_UART_TX     = 8,
  PAD_9_CAM_PCLK    = 9,
  PAD_10_CAM_HSYNC  = 10,
  PAD_11_CAM_DATA0  = 11,
  PAD_12_CAM_DATA1  = 12,
  PAD_13_CAM_DATA2  = 13,
  PAD_14_CAM_DATA3  = 14,
  PAD_15_CAM_DATA4  = 15,
  PAD_16_CAM_DATA5  = 16,
  PAD_17_CAM_DATA6  = 17,
  PAD_18_CAM_DATA7  = 18,
  PAD_19_CAM_VSYNC  = 19,
  PAD_20_HYPER_CKN  = 20,
  PAD_21_HYPER_CK   = 21,
  PAD_22_HYPER_DQ0  = 22,
  PAD_23_HYPER_DQ1  = 23,
  PAD_24_HYPER_DQ2  = 24,
  PAD_25_HYPER_DQ3  = 25,
  PAD_26_HYPER_DQ4  = 26,
  PAD_27_HYPER_DQ5  = 27,
  PAD_28_HYPER_DQ6  = 28,
  PAD_29_HYPER_DQ7  = 29,
  PAD_30_HYPER_CSN0 = 30,
  PAD_31_HYPER_CSN1 = 31,
  PAD_32_HYPER_RWDS = 32,
  PAD_33_I2C0_SDA   = 33,
  PAD_34_I2C0_SCL   = 34,
  PAD_35_I2S0_SCK   = 35,
  PAD_36_I2S0_WS    = 36,
  PAD_37_I2S0_SDI   = 37,
  PAD_38_I2S1_SDI   = 38,
} rt_pad_e;

/** \enum rt_pad_func_e
 * \brief List of pad functions which can be specified when a pad is being configured.
 * 
 * This can be used to specify which function a pad is using.
 */
typedef enum {
  PAD_0_FUNC0_SPIM_SDIO0  = 0,
  PAD_1_FUNC0_SPIM_SDIO1  = 0,
  PAD_2_FUNC0_SPIM_SDIO2  = 0,
  PAD_3_FUNC0_SPIM_SDIO3  = 0,
  PAD_4_FUNC0_SPIM_CSN0   = 0,
  PAD_5_FUNC0_SPIM_CSN1   = 0,
  PAD_6_FUNC0_SPIM_SCK    = 0,
  PAD_7_FUNC0_UART_RX     = 0,
  PAD_8_FUNC0_UART_TX     = 0,
  PAD_9_FUNC0_CAM_PCLK    = 0,
  PAD_10_FUNC0_CAM_HSYNC  = 0,
  PAD_11_FUNC0_CAM_DATA0  = 0,
  PAD_12_FUNC0_CAM_DATA1  = 0,
  PAD_13_FUNC0_CAM_DATA2  = 0,
  PAD_14_FUNC0_CAM_DATA3  = 0,
  PAD_15_FUNC0_CAM_DATA4  = 0,
  PAD_16_FUNC0_CAM_DATA5  = 0,
  PAD_17_FUNC0_CAM_DATA6  = 0,
  PAD_18_FUNC0_CAM_DATA7  = 0,
  PAD_19_FUNC0_CAM_VSYNC  = 0,
  PAD_20_FUNC0_HYPER_CKN  = 0,
  PAD_21_FUNC0_HYPER_CK   = 0,
  PAD_22_FUNC0_HYPER_DQ0  = 0,
  PAD_23_FUNC0_HYPER_DQ1  = 0,
  PAD_24_FUNC0_HYPER_DQ2  = 0,
  PAD_25_FUNC0_HYPER_DQ3  = 0,
  PAD_26_FUNC0_HYPER_DQ4  = 0,
  PAD_27_FUNC0_HYPER_DQ5  = 0,
  PAD_28_FUNC0_HYPER_DQ6  = 0,
  PAD_29_FUNC0_HYPER_DQ7  = 0,
  PAD_30_FUNC0_HYPER_CSN0 = 0,
  PAD_31_FUNC0_HYPER_CSN1 = 0,
  PAD_32_FUNC0_HYPER_RWDS = 0,
  PAD_33_FUNC0_I2C0_SDA   = 0,
  PAD_34_FUNC0_I2C0_SCL   = 0,
  PAD_35_FUNC0_I2S0_SCK   = 0,
  PAD_36_FUNC0_I2S0_WS    = 0,
  PAD_37_FUNC0_I2S0_SDI   = 0,
  PAD_38_FUNC0_I2S1_SDI   = 0,

  PAD_0_FUNC1_GPIOA0      = 1,
  PAD_1_FUNC1_GPIOA1      = 1,
  PAD_2_FUNC1_GPIOA2     = 1,
  PAD_3_FUNC1_GPIOA3     = 1,
  PAD_4_FUNC1_GPIOA4     = 1,
  PAD_5_FUNC1_GPIOA5     = 1,
  PAD_6_FUNC1_GPIOA6     = 1,
  PAD_7_FUNC1_GPIOA7     = 1,
  PAD_8_FUNC1_GPIOA8     = 1,
  PAD_9_FUNC1_GPIOA9     = 1,
  PAD_10_FUNC1_GPIOA10    = 1,
  PAD_11_FUNC1_GPIOA11    = 1,
  PAD_12_FUNC1_GPIOA12    = 1,
  PAD_13_FUNC1_GPIOA13    = 1,
  PAD_14_FUNC1_GPIOA14    = 1,
  PAD_15_FUNC1_GPIOA15    = 1,
  PAD_16_FUNC1_GPIOA16    = 1,
  PAD_17_FUNC1_GPIOA17    = 1,
  PAD_18_FUNC1_GPIOA18    = 1,
  PAD_19_FUNC1_GPIOA19    = 1,
  PAD_20_FUNC1_GPIOA20    = 1,
  PAD_21_FUNC1_GPIOA21    = 1,
  PAD_22_FUNC1_GPIOA22    = 1,
  PAD_23_FUNC1_GPIOA23    = 1,
  PAD_24_FUNC1_GPIOA24    = 1,
  PAD_25_FUNC1_GPIOA25    = 1,
  PAD_33_FUNC1_GPIOA26    = 1,
  PAD_34_FUNC1_GPIOA27    = 1,
  PAD_35_FUNC1_GPIOA28    = 1,
  PAD_36_FUNC1_GPIOA29    = 1,
  PAD_37_FUNC1_GPIOA30    = 1,
  PAD_38_FUNC1_GPIOA31    = 1,

  PAD_2_FUNC2_I2C1_SDA    = 2,
  PAD_3_FUNC2_I2C1_SCL    = 2,
  PAD_7_FUNC2_I2C1_SDA    = 2,
  PAD_8_FUNC2_I2C1_SCL    = 2,
  PAD_9_FUNC2_TIMER1_CH0  = 2,
  PAD_10_FUNC2_TIMER1_CH1 = 2,
  PAD_11_FUNC2_TIMER1_CH2 = 2,
  PAD_12_FUNC2_TIMER1_CH3 = 2,
  PAD_13_FUNC2_TIMER2_CH0 = 2,
  PAD_14_FUNC2_TIMER2_CH1 = 2,
  PAD_15_FUNC2_TIMER2_CH2 = 2,
  PAD_16_FUNC2_TIMER2_CH3 = 2,
  PAD_17_FUNC2_TIMER3_CH0 = 2,
  PAD_18_FUNC2_TIMER3_CH1 = 2,
  PAD_19_FUNC2_TIMER3_CH2 = 2,
  PAD_27_FUNC2_I2C1_SDA   = 2,
  PAD_28_FUNC2_I2C1_SCL   = 2,
  PAD_35_FUNC2_TIMER0_CH0 = 2,
  PAD_36_FUNC2_TIMER0_CH1 = 2,
  PAD_37_FUNC2_TIMER0_CH2 = 2,
  PAD_38_FUNC2_TIMER0_CH3 = 2,

  PAD_2_FUNC3_TIMER0_CH0  = 3,
  PAD_3_FUNC3_TIMER0_CH1  = 3,
  PAD_7_FUNC3_TIMER0_CH2  = 3,
  PAD_8_FUNC3_TIMER0_CH3  = 3,
  PAD_36_FUNC3_TIMER1_CH0 = 3,
  PAD_37_FUNC3_TIMER2_CH0 = 3,
  PAD_38_FUNC3_TIMER3_CH0 = 3,
} rt_pad_func_e;

//!@}


#endif

#endif