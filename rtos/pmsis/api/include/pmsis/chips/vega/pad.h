/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __PMSIS_CHIPS_GAP9_PAD_H__
#define __PMSIS_CHIPS_GAP9_PAD_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup groupChips
 */

/**
 * \addtogroup GAP9
 * @{
 */


/**
 * \defgroup GAP9_Padframe GAP9 Padframe
 *
 * This part enumerates GAP9 padframe.
 * There are 96 configurable pads.
 *
 * \addtogroup GAP9_Padframe
 * @{
 */

/**
 * \enum pi_pad_e
 * \brief Pad numbers.
 *
 * This is used to identify pads.
 */
typedef enum
{
    PI_PAD_000 = 0,
    PI_PAD_001 = 1,
    PI_PAD_002 = 2,
    PI_PAD_003 = 3,
    PI_PAD_004 = 4,
    PI_PAD_005 = 5,
    PI_PAD_006 = 6,
    PI_PAD_007 = 7,
    PI_PAD_008 = 8,
    PI_PAD_009 = 9,
    PI_PAD_010 = 10,
    PI_PAD_011 = 11,
    PI_PAD_012 = 12,
    PI_PAD_013 = 13,
    PI_PAD_014 = 14,
    PI_PAD_015 = 15,
    PI_PAD_016 = 16,
    PI_PAD_017 = 17,
    PI_PAD_018 = 18,
    PI_PAD_019 = 19,
    PI_PAD_020 = 20,
    PI_PAD_021 = 21,
    PI_PAD_022 = 22,
    PI_PAD_023 = 23,
    PI_PAD_024 = 24,
    PI_PAD_025 = 25,
    PI_PAD_026 = 26,
    PI_PAD_027 = 27,
    PI_PAD_028 = 28,
    PI_PAD_029 = 29,
    PI_PAD_030 = 30,
    PI_PAD_031 = 31,
    PI_PAD_032 = 32,
    PI_PAD_033 = 33,
    PI_PAD_034 = 34,
    PI_PAD_035 = 35,
    PI_PAD_036 = 36,
    PI_PAD_037 = 37,
    PI_PAD_038 = 38,
    PI_PAD_039 = 39,
    PI_PAD_040 = 40,
    PI_PAD_041 = 41,
    PI_PAD_042 = 42,
    PI_PAD_043 = 43,
    PI_PAD_044 = 44,
    PI_PAD_045 = 45,
    PI_PAD_046 = 46,
    PI_PAD_047 = 47,
    PI_PAD_048 = 48,
    PI_PAD_049 = 49,
    PI_PAD_050 = 50,
    PI_PAD_051 = 51,
    PI_PAD_052 = 52,
    PI_PAD_053 = 53,
    PI_PAD_054 = 54,
    PI_PAD_055 = 55,
    PI_PAD_056 = 56,
    PI_PAD_057 = 57,
    PI_PAD_058 = 58,
    PI_PAD_059 = 59,
    PI_PAD_060 = 60,
    PI_PAD_061 = 61,
    PI_PAD_062 = 62,
    PI_PAD_063 = 63,
    PI_PAD_064 = 64,
    PI_PAD_065 = 65,
    PI_PAD_066 = 66,
    PI_PAD_067 = 67,
    PI_PAD_068 = 68,
    PI_PAD_069 = 69,
    PI_PAD_070 = 70,
    PI_PAD_071 = 71,
    PI_PAD_072 = 72,
    PI_PAD_073 = 73,
    PI_PAD_074 = 74,
    PI_PAD_075 = 75,
    PI_PAD_076 = 76,
    PI_PAD_077 = 77,
    PI_PAD_078 = 78,
    PI_PAD_079 = 79,
    PI_PAD_080 = 80,
    PI_PAD_081 = 81,
    PI_PAD_082 = 82,
    PI_PAD_083 = 83,
    PI_PAD_084 = 84,
    PI_PAD_085 = 85,
    PI_PAD_086 = 86,
    PI_PAD_087 = 87,
    PI_PAD_088 = 88,
    PI_PAD_089 = 89,
    PI_PAD_090 = 90,
    PI_PAD_091 = 91,
    PI_PAD_092 = 92,
    PI_PAD_093 = 93,
    PI_PAD_094 = 94,
    PI_PAD_095 = 95,
    PI_PAD_096 = 96,
    PI_PAD_097 = 97,
    PI_PAD_098 = 98,
    PI_PAD_099 = 99,
    PI_PAD_100 = 100
} pi_pad_e;

/**
 * \cond IMPLEM
 */
/* Default pad function, alternate 2. */
#define PI_PAD_036_UART3_CTS_FUNC2  ( 2 )
#define PI_PAD_037_UART3_RTS_FUNC2  ( 2 )
#define PI_PAD_082_UART4_RX_FUNC2  ( 2 )
#define PI_PAD_083_UART4_TX_FUNC2  ( 2 )
#define PI_PAD_084_UART4_CTS_FUNC2  ( 2 )
#define PI_PAD_085_UART4_RTS_FUNC2  ( 2 )

/**
 * \endcond IMPLEM
 */


/**
 * \enum pi_pad_mux_group_sel_e
 * \brief Pad mux groups values.
 *
 * This is used to configure the mux group for customizable pads.
 */
typedef enum
{
    PI_PAD_MUX_GROUP_SPI0_CS0  = 0x00,
    PI_PAD_MUX_GROUP_SPI0_CS1  = 0x01,
    PI_PAD_MUX_GROUP_SPI0_CS2  = 0x02,
    PI_PAD_MUX_GROUP_SPI0_CS3  = 0x03,
    PI_PAD_MUX_GROUP_SPI1_CS0  = 0x04,
    PI_PAD_MUX_GROUP_UART3_TX  = 0x05,
    PI_PAD_MUX_GROUP_SPI1_CS1  = 0x06,
    PI_PAD_MUX_GROUP_UART3_RX  = 0x07,
    PI_PAD_MUX_GROUP_I2C0_SDA  = 0x08,
    PI_PAD_MUX_GROUP_I2C0_SCL  = 0x09,
    PI_PAD_MUX_GROUP_I2C1_SDA  = 0x0A,
    PI_PAD_MUX_GROUP_CSI2_SDA  = 0x0B,
    PI_PAD_MUX_GROUP_I2C1_SCL  = 0x0C,
    PI_PAD_MUX_GROUP_CSI2_SCL  = 0x0D,
    PI_PAD_MUX_GROUP_I2C2_SDA  = 0x0E,
    PI_PAD_MUX_GROUP_UART2_CTS = 0x0F,
    PI_PAD_MUX_GROUP_I2C2_SCL  = 0x10,
    PI_PAD_MUX_GROUP_UART2_RTS = 0x11,
    PI_PAD_MUX_GROUP_UART0_RX  = 0x12,
    PI_PAD_MUX_GROUP_UART0_TX  = 0x13,
    PI_PAD_MUX_GROUP_UART0_CTS = 0x14,
    PI_PAD_MUX_GROUP_UART2_RX  = 0x15,
    PI_PAD_MUX_GROUP_UART0_RTS = 0x16,
    PI_PAD_MUX_GROUP_UART2_TX  = 0x17,
    PI_PAD_MUX_GROUP_UART1_RX  = 0x18,
    PI_PAD_MUX_GROUP_PWM2      = 0x19,
    PI_PAD_MUX_GROUP_PWM4      = 0x1A,
    PI_PAD_MUX_GROUP_UART1_TX  = 0x1B,
    PI_PAD_MUX_GROUP_PWM3      = 0x1C,
    PI_PAD_MUX_GROUP_PWM5      = 0x1D,
    PI_PAD_MUX_GROUP_PWM0      = 0x1E,
    PI_PAD_MUX_GROUP_UART1_CTS = 0x1F,
    PI_PAD_MUX_GROUP_PWM6      = 0x20,
    PI_PAD_MUX_GROUP_PWM1      = 0x21,
    PI_PAD_MUX_GROUP_UART1_RTS = 0x22,
    PI_PAD_MUX_GROUP_PWM7      = 0x23,
} pi_pad_mux_group_sel_e;


/**
 * \brief Set the mux group of one pad.
 *
 * This function can be used to configure the mux group of the specified pad
 * in the case that it supports several mux groups.
 *
 * \param pad  Pad number.
 * \param function Pad mux group.
 */
void pi_pad_set_mux_group(pi_pad_e pad, pi_pad_mux_group_sel_e group_value);



/**
 * \enum pi_pad_sleep_e
 * \brief Pad identifiers for configuring sleep configuration.
 *
 * This is used to configure the pads in sleep mode.
 */
typedef enum
{
    PI_PAD_SLEEP_CFG_PULL_DOWN          = (1 << 0),
    PI_PAD_SLEEP_CFG_PULL_UP            = (1 << 1),
    PI_PAD_SLEEP_CFG_DRIVER_STRENGTH_0  = (0 << 2),
    PI_PAD_SLEEP_CFG_DRIVER_STRENGTH_1  = (1 << 2),
    PI_PAD_SLEEP_CFG_DRIVER_STRENGTH_2  = (2 << 2),
    PI_PAD_SLEEP_CFG_DRIVER_STRENGTH_3  = (3 << 2),
    PI_PAD_SLEEP_CFG_OUTPUT_VALUE_0     = (0 << 4),
    PI_PAD_SLEEP_CFG_OUTPUT_VALUE_1     = (1 << 4),
    PI_PAD_SLEEP_CFG_OUTPUT_DISABLED    = (1 << 5),
    PI_PAD_SLEEP_CFG_OUTPUT_ENABLED     = (0 << 5),
} pi_pad_sleep_cfg_e;


/**
 * \brief Set a pad configuration in deep sleep mode.
 *
 * This function can be used to configure the pad
 * when the chip is in deep sleep mode.
 *
 * \param pad  Pad number.
 * \param cfg  Pad config.
 */
void pi_pad_set_sleep_cfg(pi_pad_e pad, pi_pad_sleep_cfg_e cfg);


/**
 * \brief Force the pads deep sleep mode configuration also in normal mode
 *
 * This function can be used to keep the pads sleep configuration also applied
 * when the chip is not in deep sleep mode. This allows keeping the settings
 * until the pads are configured again after wakeup.
 *
 * \param enabled  Force deep sleep configuration if it is 1
 */
void pi_pad_sleep_cfg_force(int enabled);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif  /* __PMSIS_CHIPS_GAP9_PAD_H__ */
