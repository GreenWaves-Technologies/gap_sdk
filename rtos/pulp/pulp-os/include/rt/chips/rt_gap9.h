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

#ifndef __RT_CHIPS_RT_GAP9_H__
#define __RT_CHIPS_RT_GAP9_H__

#ifdef CONFIG_GAP9

/// @cond IMPLEM

#define __RT_GPIO_PULL_BIT 0
#define __RT_GPIO_STRENGTH_BIT 2

#define __RT_FLAGS_MUX_BIT   16
#define __RT_FLAGS_MUX_WIDTH 2

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
  PAD_0_PWM0          = 0,
  PAD_1_PWM1          = 1,
  PAD_2_PWM2          = 2,
  PAD_3_PWM3          = 3,
  PAD_4_SPIM0_SCK     = 4,
  PAD_5_SPIM0_CS0     = 5,
  PAD_6_SPIM0_MISO    = 6,
  PAD_7_SPIM0_MOSI    = 7,
  PAD_8_I3C0_SDA      = 8,
  PAD_9_I3C0_SCL      = 9,
  PAD_10_UART0_RX     = 10,
  PAD_11_UART0_TX     = 11,
  PAD_12_QSPI_SDIO0   = 12,
  PAD_13_QSPI_SDIO1   = 13,
  PAD_14_QSPI_SDIO2   = 14,
  PAD_15_QSPI_SDIO3   = 15,
  PAD_16_QSPI_CS0     = 16,
  PAD_17_QSPI_SCK     = 17,
  PAD_18_UART1_RX     = 18,
  PAD_19_UART1_TX     = 19,
  PAD_20_SPIM1_SCK    = 20,
  PAD_21_SPIM1_CS0    = 21,
  PAD_22_SPIM1_MISO   = 22,
  PAD_23_SPIM1_MOSI   = 23,
  PAD_24_I3C1_SDA     = 24,
  PAD_25_I3C1_SCL     = 25,
  PAD_26_I2S_MST_SCK  = 26,
  PAD_27_I2S_MST_WS   = 27,
  PAD_28_I2S_MST_SDO0 = 28,
  PAD_29_I2S_MST_SDO1 = 29,
  PAD_30_I2S_SLV_SCK  = 30,
  PAD_31_I2S_SLV_WS   = 31,
  PAD_32_I2S_SLV_SDI0 = 32,
  PAD_33_I2S_SLV_SDI1 = 33,
  PAD_34_SPIM2_SCK    = 34,
  PAD_35_SPIM2_CS0    = 35,
  PAD_36_SPIM2_MISO   = 36,
  PAD_37_SPIM2_MOSI   = 37,
  PAD_38_I3C2_SDA     = 38,
  PAD_39_I3C2_SCL     = 39,
  PAD_40_CAM_PCLK     = 40,
  PAD_41_CAM_HSYNC    = 41,
  PAD_42_CAM_DATA0    = 42,
  PAD_43_CAM_DATA1    = 43,
  PAD_44_CAM_DATA2    = 44,
  PAD_45_CAM_DATA3    = 45,
  PAD_46_CAM_DATA4    = 46,
  PAD_47_CAM_DATA5    = 47,
  PAD_48_CAM_DATA6    = 48,
  PAD_49_CAM_DATA7    = 49,
  PAD_50_CAM_VSYNC    = 50,
  PAD_51_HYPER_CKN    = 51,
  PAD_52_HYPER_CK     = 52,
  PAD_53_HYPER_DQ0    = 53,
  PAD_54_HYPER_DQ1    = 54,
  PAD_55_HYPER_DQ2    = 55,
  PAD_56_HYPER_DQ3    = 56,
  PAD_57_HYPER_DQ4    = 57,
  PAD_58_HYPER_DQ5    = 58,
  PAD_59_HYPER_DQ6    = 59,
  PAD_60_HYPER_DQ7    = 60,
  PAD_61_HYPER_CSN0   = 61,
  PAD_62_HYPER_CSN1   = 62,
  PAD_63_HYPER_RWDS   = 63
} rt_pad_e;



/** \enum rt_pad_func_e
 * \brief List of pad functions which can be specified when a pad is being configured.
 * 
 * This can be used to specify which function a pad is using.
 */
typedef enum {
  PAD_0_FUNC0_WM0           = 0,
  PAD_1_FUNC0_WM1           = 0,
  PAD_2_FUNC0_WM2           = 0,
  PAD_3_FUNC0_WM3           = 0,
  PAD_4_FUNC0_PIM0_SCK      = 0,
  PAD_5_FUNC0_PIM0_CS0      = 0,
  PAD_6_FUNC0_PIM0_MISO     = 0,
  PAD_7_FUNC0_PIM0_MOSI     = 0,
  PAD_8_FUNC0_3C0_SDA       = 0,
  PAD_9_FUNC0_3C0_SCL       = 0,
  PAD_10_FUNC0_UART0_RX     = 0,
  PAD_11_FUNC0_UART0_TX     = 0,
  PAD_12_FUNC0_QSPI_SDIO0   = 0,
  PAD_13_FUNC0_QSPI_SDIO1   = 0,
  PAD_14_FUNC0_QSPI_SDIO2   = 0,
  PAD_15_FUNC0_QSPI_SDIO3   = 0,
  PAD_16_FUNC0_QSPI_CS0     = 0,
  PAD_17_FUNC0_QSPI_SCK     = 0,
  PAD_18_FUNC0_UART1_RX     = 0,
  PAD_19_FUNC0_UART1_TX     = 0,
  PAD_20_FUNC0_SPIM1_SCK    = 0,
  PAD_21_FUNC0_SPIM1_CS0    = 0,
  PAD_22_FUNC0_SPIM1_MISO   = 0,
  PAD_23_FUNC0_SPIM1_MOSI   = 0,
  PAD_24_FUNC0_I3C1_SDA     = 0,
  PAD_25_FUNC0_I3C1_SCL     = 0,
  PAD_26_FUNC0_I2S_MST_SCK  = 0,
  PAD_27_FUNC0_I2S_MST_WS   = 0,
  PAD_28_FUNC0_I2S_MST_SDO0 = 0,
  PAD_29_FUNC0_I2S_MST_SDO1 = 0,
  PAD_30_FUNC0_I2S_SLV_SCK  = 0,
  PAD_31_FUNC0_I2S_SLV_WS   = 0,
  PAD_32_FUNC0_I2S_SLV_SDI0 = 0,
  PAD_33_FUNC0_I2S_SLV_SDI1 = 0,
  PAD_34_FUNC0_SPIM2_SCK    = 0,
  PAD_35_FUNC0_SPIM2_CS0    = 0,
  PAD_36_FUNC0_SPIM2_MISO   = 0,
  PAD_37_FUNC0_SPIM2_MOSI   = 0,
  PAD_38_FUNC0_I3C2_SDA     = 0,
  PAD_39_FUNC0_I3C2_SCL     = 0,
  PAD_40_FUNC0_CAM_PCLK     = 0,
  PAD_41_FUNC0_CAM_HSYNC    = 0,
  PAD_42_FUNC0_CAM_DATA0    = 0,
  PAD_43_FUNC0_CAM_DATA1    = 0,
  PAD_44_FUNC0_CAM_DATA2    = 0,
  PAD_45_FUNC0_CAM_DATA3    = 0,
  PAD_46_FUNC0_CAM_DATA4    = 0,
  PAD_47_FUNC0_CAM_DATA5    = 0,
  PAD_48_FUNC0_CAM_DATA6    = 0,
  PAD_49_FUNC0_CAM_DATA7    = 0,
  PAD_50_FUNC0_CAM_VSYNC    = 0,
  PAD_51_FUNC0_HYPER_CKN    = 0,
  PAD_52_FUNC0_HYPER_CK     = 0,
  PAD_53_FUNC0_HYPER_DQ0    = 0,
  PAD_54_FUNC0_HYPER_DQ1    = 0,
  PAD_55_FUNC0_HYPER_DQ2    = 0,
  PAD_56_FUNC0_HYPER_DQ3    = 0,
  PAD_57_FUNC0_HYPER_DQ4    = 0,
  PAD_58_FUNC0_HYPER_DQ5    = 0,
  PAD_59_FUNC0_HYPER_DQ6    = 0,
  PAD_60_FUNC0_HYPER_DQ7    = 0,
  PAD_61_FUNC0_HYPER_CSN0   = 0,
  PAD_62_FUNC0_HYPER_CSN1   = 0,
  PAD_63_FUNC0_HYPER_RWDS   = 0,

  PAD_0_FUNC1_GPIO0         = 1,
  PAD_1_FUNC1_GPIO1         = 1,
  PAD_2_FUNC1_GPIO2         = 1,
  PAD_3_FUNC1_GPIO3         = 1,
  PAD_4_FUNC1_GPIO4         = 1,
  PAD_5_FUNC1_GPIO5         = 1,
  PAD_6_FUNC1_GPIO6         = 1,
  PAD_7_FUNC1_GPIO7         = 1,
  PAD_8_FUNC1_GPIO8         = 1,
  PAD_9_FUNC1_GPIO9         = 1,
  PAD_10_FUNC1_GPIO10       = 1,
  PAD_11_FUNC1_GPIO11       = 1,
  PAD_12_FUNC1_GPIO12       = 1,
  PAD_13_FUNC1_GPIO13       = 1,
  PAD_14_FUNC1_GPIO14       = 1,
  PAD_15_FUNC1_GPIO15       = 1,
  PAD_16_FUNC1_GPIO16       = 1,
  PAD_17_FUNC1_GPIO17       = 1,
  PAD_18_FUNC1_GPIO18       = 1,
  PAD_19_FUNC1_GPIO19       = 1,
  PAD_20_FUNC1_GPIO20       = 1,
  PAD_21_FUNC1_GPIO21       = 1,
  PAD_22_FUNC1_GPIO22       = 1,
  PAD_23_FUNC1_GPIO23       = 1,
  PAD_24_FUNC1_GPIO24       = 1,
  PAD_25_FUNC1_GPIO25       = 1,
  PAD_26_FUNC1_GPIO26       = 1,
  PAD_27_FUNC1_GPIO27       = 1,
  PAD_28_FUNC1_GPIO28       = 1,
  PAD_29_FUNC1_GPIO29       = 1,
  PAD_30_FUNC1_GPIO30       = 1,
  PAD_31_FUNC1_GPIO31       = 1,
  PAD_32_FUNC1_GPIO32       = 1,
  PAD_33_FUNC1_GPIO33       = 1,
  PAD_34_FUNC1_GPIO34       = 1,
  PAD_35_FUNC1_GPIO35       = 1,
  PAD_36_FUNC1_GPIO36       = 1,
  PAD_37_FUNC1_GPIO37       = 1,
  PAD_38_FUNC1_GPIO38       = 1,
  PAD_39_FUNC1_GPIO39       = 1,
  PAD_40_FUNC1_GPIO40       = 1,
  PAD_41_FUNC1_GPIO41       = 1,
  PAD_42_FUNC1_GPIO42       = 1,
  PAD_43_FUNC1_GPIO43       = 1,
  PAD_44_FUNC1_GPIO44       = 1,
  PAD_45_FUNC1_GPIO45       = 1,
  PAD_46_FUNC1_GPIO46       = 1,
  PAD_47_FUNC1_GPIO47       = 1,
  PAD_48_FUNC1_GPIO48       = 1,
  PAD_49_FUNC1_GPIO49       = 1,
  PAD_50_FUNC1_GPIO50       = 1,
  PAD_51_FUNC1_GPIO51       = 1,
  PAD_52_FUNC1_GPIO52       = 1,
  PAD_53_FUNC1_GPIO53       = 1,
  PAD_54_FUNC1_GPIO54       = 1,
  PAD_55_FUNC1_GPIO55       = 1,
  PAD_56_FUNC1_GPIO56       = 1,
  PAD_57_FUNC1_GPIO57       = 1,
  PAD_58_FUNC1_GPIO58       = 1,
  PAD_59_FUNC1_GPIO59       = 1,
  PAD_60_FUNC1_GPIO60       = 1,
  PAD_61_FUNC1_GPIO61       = 1,
  PAD_62_FUNC1_GPIO62       = 1,
  PAD_63_FUNC1_GPIO63       = 1,

  PAD_1_FUNC_2_TIMER1_CH0    = 2,
  PAD_2_FUNC_2_TIMER2_CH0    = 2,
  PAD_3_FUNC_2_TIMER3_CH0    = 2,
  PAD_12_FUNC2_SDIO_DATA0 = 2,
  PAD_13_FUNC2_SDIO_DATA1 = 2,
  PAD_14_FUNC2_SDIO_DATA2 = 2,
  PAD_15_FUNC2_SDIO_DATA3 = 2,
  PAD_16_FUNC2_SDIO_CMD = 2,
  PAD_17_FUNC2_SDIO_CLK = 2,
  PAD_24_FUNC2_CSI2_SDA = 2,
  PAD_25_FUNC2_CSI2_SCL = 2,
  PAD_36_FUNC2_UART2_RX = 2,
  PAD_37_FUNC2_UART2_TX = 2
} rt_pad_func_e;


/** \enum rt_pad_sleep_state_e
 * \brief Pad state when the chip is sleeping.
 *
 * Describes the available pad state when the chip is sleeping.
 */
typedef enum {
  RT_PAD_SLEEP_STATE_TRI     = 0, /*!< Tristate with no pull-up and no pull-down. */
  RT_PAD_SLEEP_STATE_TRI_PU  = 1, /*!< Tristate with pull-up. */
  RT_PAD_SLEEP_STATE_TRI_PD  = 2, /*!< Tristate with pull-down. */
  RT_PAD_SLEEP_STATE_DRI_0   = 3, /*!< Drive to 0. */
  RT_PAD_SLEEP_STATE_DRI_1   = 4, /*!< Drive to 1. */
} rt_pad_sleep_state_e;


//!@}


/**
 * @addtogroup PM
 * @{
 */

/** \enum rt_pm_domain_e
 * \brief Power domains.
 *
 * Describes the available power domains (other than the chip itself).
 */
typedef enum {
  RT_PM_DOMAIN_SWU =0,   /*!< Smart wakeup. */
  RT_PM_DOMAIN_MRAM=1,  /*!< Embedded MRAM. */
  RT_PM_DOMAIN_CSI =2   /*!< CSI2 interface. */
} rt_pm_domain_e;




/** \enum rt_pm_state_e
 * \brief Power states.
 *
 * Describes the power state of the chip.
 */
typedef enum {
  RT_PM_STATE_SLEEP       = 0,     /*!< Sleep state. Everything is shut down, except few memories which can still be in retentive mode to keep some data. Pads are on OFF. */
  RT_PM_STATE_SLEEP_PADS_ON = 2,     /*!< Sleep state. Everything is shut down, except few memories which can still be in retentive mode to keep some data. Low-speed pads are ON, others are OFF. */
  RT_PM_STATE_DEEP_SLEEP  = 1,     /*!< Deep sleep state. Everything is shut down, inluding memories, and all data is lost.  Pads are on OFF. */
  RT_PM_STATE_DEEP_SLEEP_PADS_ON  = 3,     /*!< Deep sleep state. Everything is shut down, inluding memories, and all data is lost. Low-speed pads are ON, others are OFF. */
} rt_pm_state_e;

//!@}





/** \enum rt_voltage_domain_e
 * \brief Voltage domains.
 *
 * Describes the available voltage domains.
 */
typedef enum {
  RT_VOLTAGE_DOMAIN_LOGIC      = 0,
  RT_VOLTAGE_DOMAIN_MEMORY     = 1,
  RT_VOLTAGE_DOMAIN_RETENTION  = 2,
} rt_voltage_domain_e;



/**
 * @addtogroup I2S
 * @{
 */

/** Specify that the I2S channel should be multiplexed with only 1 stream. */
#define RT_I2S_FLAGS_MUX_1CH       (0<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 2 streams on the same pad. */
#define RT_I2S_FLAGS_MUX_1CH_DUAL  (1<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 2 streams, each one on a distinct pad. */
#define RT_I2S_FLAGS_MUX_2CH       (2<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 4 streams, with the 2 first on the first pad, and the 2 lasts on the second pad. */
#define RT_I2S_FLAGS_MUX_2CH_DUAL  (3<<__RT_FLAGS_MUX_BIT)

//!@}



/// @cond IMPLEM

typedef enum {
  RT_L1_POWER_CTRL_POWER_DOWN = 1,
  RT_L1_POWER_CTRL_POWER_UP = 2,
  RT_L1_POWER_CTRL_POWER_RET = 3
} rt_l1_power_ctrl_e;

void rt_l1_power_ctrl(unsigned int banks, rt_l1_power_ctrl_e flags);

/// @endcond

#endif

#endif