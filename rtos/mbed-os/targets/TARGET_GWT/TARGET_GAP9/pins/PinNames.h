/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
/*
 * Copyright (C) 2018 Greenwaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Jie Chen (owenchj@gmail.com)
 */
#ifndef MBED_PINNAMES_H
#define MBED_PINNAMES_H

#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT
} PinDirection;

#define GAP_PIN_OFFSET   0
#define GAP_PORT_PIN_NUM 63

/* Mapping for GPIO type pinname */
#define GPIO_PIN_NUM_SHIFT 0
#define GPIO_PIN_NUM_MASK  0xFF
#define GET_GPIO_PIN_NUM(x)                 ((((uint32_t)(((uint32_t)(x)) & GPIO_PIN_NUM_MASK )) /* >> GPIO_PIN_NUM_SHIFT */) - GAP_PIN_OFFSET)

#define GPIO_PORT_SHIFT 8
#define GPIO_PORT_MASK  0xF00
#define GET_GPIO_PORT(x)                    (((uint32_t)(((uint32_t)(x)) & GPIO_PORT_MASK )) >> GPIO_PORT_SHIFT)

#define GPIO_NUM_SHIFT  12
#define GPIO_NUM_MASK   0xFF000
#define GET_GPIO_NUM(x)                     (((uint32_t)(((uint32_t)(x)) & GPIO_NUM_MASK )) >> GPIO_NUM_SHIFT)

#define GPIO_IS_SHIFT   20
#define GPIO_IS_MASK   0x100000
#define GET_IS_GPIO(x)                      (((uint32_t)(((uint32_t)(x)) & GPIO_IS_MASK )) >> GPIO_IS_SHIFT)

typedef enum {
    B20  = 0,
    A22  = 1,
    B19  = 2,
    A21  = 3,
    A19  = 4,
    B18  = 5,
    A18  = 6,
    B17  = 7,
    A4   = 8,
    B3   = 9,
    A5   = 10,
    B4   = 11,
    A3   = 12,
    B2   = 13,
    A2   = 14,
    B1   = 15,
    A44  = 16,
    B40  = 17,
    A43  = 18,
    A37  = 19,
    B39  = 20,
    A42  = 21,
    B38  = 22,
    A41  = 23,
    B37  = 24,
    A40  = 25,
    B36  = 26,
    A38  = 27,
    A36  = 28,
    B34  = 29,
    D1   = 30,
    B11  = 31,
    A13  = 32,
    B12  = 33,
    A14  = 34,
    B13  = 35,
    A15  = 36,
    B14  = 37,
    B6   = 38,
    A7   = 39,
    D2   = 40,
    A11  = 41,
    B10  = 42,
    A10  = 43,
    B8   = 44,
    A8   = 45,
    B7   = 46,
    A9   = 47,
    B15  = 48,
    A16  = 49,
    B9   = 50,
    A32  = 51,
    B30  = 52,
    B22  = 53,
    A25  = 54,
    A24  = 55,
    A26  = 56,
    B23  = 57,
    A23  = 58,
    B25  = 59,
    A28  = 60,
    B26  = 61,
    A29  = 62,
    B27  = 63,

    // GPIO PortA
    GPIO_A0  = (1 << GPIO_IS_SHIFT | 0 << GPIO_NUM_SHIFT | 0),
    GPIO_A1  = (1 << GPIO_IS_SHIFT | 1 << GPIO_NUM_SHIFT | 1),
    GPIO_A2  = (1 << GPIO_IS_SHIFT | 2 << GPIO_NUM_SHIFT | 2),
    GPIO_A3  = (1 << GPIO_IS_SHIFT | 3 << GPIO_NUM_SHIFT | 3),
    GPIO_A4  = (1 << GPIO_IS_SHIFT | 4 << GPIO_NUM_SHIFT | 4),
    GPIO_A5  = (1 << GPIO_IS_SHIFT | 5 << GPIO_NUM_SHIFT | 5),
    GPIO_A6  = (1 << GPIO_IS_SHIFT | 6 << GPIO_NUM_SHIFT | 6),
    GPIO_A7  = (1 << GPIO_IS_SHIFT | 7 << GPIO_NUM_SHIFT | 7),
    GPIO_A8  = (1 << GPIO_IS_SHIFT | 8 << GPIO_NUM_SHIFT | 8),
    GPIO_A9  = (1 << GPIO_IS_SHIFT | 9 << GPIO_NUM_SHIFT | 9),
    GPIO_A10 = (1 << GPIO_IS_SHIFT | 10 << GPIO_NUM_SHIFT | 10),
    GPIO_A11 = (1 << GPIO_IS_SHIFT | 11 << GPIO_NUM_SHIFT | 11),
    GPIO_A12 = (1 << GPIO_IS_SHIFT | 12 << GPIO_NUM_SHIFT | 12),
    GPIO_A13 = (1 << GPIO_IS_SHIFT | 13 << GPIO_NUM_SHIFT | 13),
    GPIO_A14 = (1 << GPIO_IS_SHIFT | 14 << GPIO_NUM_SHIFT | 14),
    GPIO_A15 = (1 << GPIO_IS_SHIFT | 15 << GPIO_NUM_SHIFT | 15),
    GPIO_A16 = (1 << GPIO_IS_SHIFT | 16 << GPIO_NUM_SHIFT | 16),
    GPIO_A17 = (1 << GPIO_IS_SHIFT | 17 << GPIO_NUM_SHIFT | 17),
    GPIO_A18 = (1 << GPIO_IS_SHIFT | 18 << GPIO_NUM_SHIFT | 18),
    GPIO_A19 = (1 << GPIO_IS_SHIFT | 19 << GPIO_NUM_SHIFT | 19),
    GPIO_A20 = (1 << GPIO_IS_SHIFT | 20 << GPIO_NUM_SHIFT | 20),
    GPIO_A21 = (1 << GPIO_IS_SHIFT | 21 << GPIO_NUM_SHIFT | 21),
    GPIO_A22 = (1 << GPIO_IS_SHIFT | 22 << GPIO_NUM_SHIFT | 22),
    GPIO_A23 = (1 << GPIO_IS_SHIFT | 23 << GPIO_NUM_SHIFT | 23),
    GPIO_A24 = (1 << GPIO_IS_SHIFT | 24 << GPIO_NUM_SHIFT | 24),
    GPIO_A25 = (1 << GPIO_IS_SHIFT | 25 << GPIO_NUM_SHIFT | 25),
    GPIO_A26 = (1 << GPIO_IS_SHIFT | 26 << GPIO_NUM_SHIFT | 26),
    GPIO_A27 = (1 << GPIO_IS_SHIFT | 27 << GPIO_NUM_SHIFT | 27),
    GPIO_A28 = (1 << GPIO_IS_SHIFT | 28 << GPIO_NUM_SHIFT | 28),
    GPIO_A29 = (1 << GPIO_IS_SHIFT | 29 << GPIO_NUM_SHIFT | 29),
    GPIO_A30 = (1 << GPIO_IS_SHIFT | 30 << GPIO_NUM_SHIFT | 30),
    GPIO_A31 = (1 << GPIO_IS_SHIFT | 31 << GPIO_NUM_SHIFT | 31),

    GPIO_B0  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 0 << GPIO_NUM_SHIFT | 32),
    GPIO_B1  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 1 << GPIO_NUM_SHIFT | 33),
    GPIO_B2  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 2 << GPIO_NUM_SHIFT | 34),
    GPIO_B3  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 3 << GPIO_NUM_SHIFT | 35),
    GPIO_B4  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 4 << GPIO_NUM_SHIFT | 36),
    GPIO_B5  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 5 << GPIO_NUM_SHIFT | 37),
    GPIO_B6  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 6 << GPIO_NUM_SHIFT | 38),
    GPIO_B7  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 7 << GPIO_NUM_SHIFT | 39),
    GPIO_B8  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 8 << GPIO_NUM_SHIFT | 40),
    GPIO_B9  = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 9 << GPIO_NUM_SHIFT | 41),
    GPIO_B10 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 10 << GPIO_NUM_SHIFT | 42),
    GPIO_B11 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 11 << GPIO_NUM_SHIFT | 43),
    GPIO_B12 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 12 << GPIO_NUM_SHIFT | 44),
    GPIO_B13 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 13 << GPIO_NUM_SHIFT | 45),
    GPIO_B14 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 14 << GPIO_NUM_SHIFT | 46),
    GPIO_B15 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 15 << GPIO_NUM_SHIFT | 47),
    GPIO_B16 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 16 << GPIO_NUM_SHIFT | 48),
    GPIO_B17 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 17 << GPIO_NUM_SHIFT | 49),
    GPIO_B18 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 18 << GPIO_NUM_SHIFT | 50),
    GPIO_B19 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 19 << GPIO_NUM_SHIFT | 51),
    GPIO_B20 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 20 << GPIO_NUM_SHIFT | 52),
    GPIO_B21 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 21 << GPIO_NUM_SHIFT | 53),
    GPIO_B22 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 22 << GPIO_NUM_SHIFT | 54),
    GPIO_B23 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 23 << GPIO_NUM_SHIFT | 55),
    GPIO_B24 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 24 << GPIO_NUM_SHIFT | 56),
    GPIO_B25 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 25 << GPIO_NUM_SHIFT | 57),
    GPIO_B26 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 26 << GPIO_NUM_SHIFT | 58),
    GPIO_B27 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 27 << GPIO_NUM_SHIFT | 59),
    GPIO_B28 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 28 << GPIO_NUM_SHIFT | 60),
    GPIO_B29 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 29 << GPIO_NUM_SHIFT | 61),
    GPIO_B30 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 30 << GPIO_NUM_SHIFT | 62),
    GPIO_B31 = (1 << GPIO_IS_SHIFT | 1 << GPIO_PORT_SHIFT | 31 << GPIO_NUM_SHIFT | 63),

    // SPI master QSPI
    SPI0_MOSI  = D2,
    SPI0_MISO  = A11,
    SPI0_SDIO0 = D2,
    SPI0_SDIO1 = A11,
    SPI0_SDIO2 = B10,
    SPI0_SDIO3 = A10,
    SPI0_CSN0  = GPIO_A4,
    SPI0_SCLK  = B7,

    // SPI master 1
    SPI1_MISO     = A4,
    SPI1_MOSI     = B3,
    SPI1_CSN0     = GPIO_A7,
    SPI1_SCLK     = B4,
    // SPI master 2
    SPI2_MISO     = A5,
    SPI2_MOSI     = B4,
    SPI2_CSN0     = GPIO_A17,
    SPI2_SCLK     = B8,
    // SPI master 3
    SPI3_MISO     = A7,
    SPI3_MOSI     = B6,
    SPI3_CSN0     = GPIO_A31,
    SPI3_SCLK     = B9,

    // I2C0
    I2C0_SDA = B22,
    I2C0_SCL = A25,

    // I2C1
    I2C1_SDA_B34 = B34,
    I2C1_SDA_A5  = A5,
    I2C1_SDA_B10 = B10,

    I2C1_SCL_D1  = D1,
    I2C1_SCL_B4  = B4,
    I2C1_SCL_A10 = A10,

    // I2S0
    I2S0_SCK = A24,
    I2S0_WS  = A26,
    I2S0_SDI = B23,

    // I2S1
    I2S1_SCK = B13,
    I2S1_WS  = A15,
    I2S1_SDI_B14 = B14,
    I2S1_SDI_B13 = B13,

    // ORCA
    ORCA_TXSYNC = A3,
    ORCA_RXSYNC = B2,
    ORCA_TXI    = A2,
    ORCA_TXQ    = B1,
    ORCA_RXI    = A44,
    ORCA_RXQ    = B40,
    ORCA_CLK    = D1,

    // LVDS
    LVDS_TXD_P   = B20,
    LVDS_TXD_N   = A22,
    LVDS_TXCLK_P = B19,
    LVDS_TXCLK_N = A21,
    LVDS_RXD_P   = A19,
    LVDS_RXD_N   = B18,
    LVDS_RXCLK_P = A18,
    LVDS_RXCLK_N = B17,

    // HYPERBUS
    HYPERBUS_CLK  = B14,
    HYPERBUS_CLKN = A15,
    HYPERBUS_DQ0  = D2,
    HYPERBUS_DQ1  = A11,
    HYPERBUS_DQ2  = B10,
    HYPERBUS_DQ3  = A10,
    HYPERBUS_DQ4  = B8,
    HYPERBUS_DQ5  = A8,
    HYPERBUS_DQ6  = B7,
    HYPERBUS_DQ7  = A9,
    HYPERBUS_CSN0 = B15,
    HYPERBUS_CSN1 = A16,
    HYPERBUS_RWDS = B9,

    // CPI (Camera Parallel Interface)
    CPI_PCLK   = A43,
    CPI_HSYNC  = A37,
    CPI_DATA0  = B39,
    CPI_DATA1  = A42,
    CPI_DATA2  = B38,
    CPI_DATA3  = A41,
    CPI_DATA4  = B37,
    CPI_DATA5  = A40,
    CPI_DATA6  = B36,
    CPI_DATA7  = A38,
    CPI_VSYNC  = A36,

    // UART Pins
    USBTX = A7,
    USBRX = B6,
    UART0_TX = A7,
    UART0_RX = B6,

    // UART Pins
    UART1_TX = A8,
    UART1_RX = B7,

    // UART Pins
    UART2_TX = B14,
    UART2_RX = A15,

    // PWM0 Pins
    PWM0_CH0 = B11,
    PWM0_CH1 = A13,
    PWM0_CH2 = B12,
    PWM0_CH3 = A14,

    // PWM1 Pins
    PWM1_CH0_A43 = A43,
    PWM1_CH0_A13 = A13,
    PWM1_CH1 = A37,
    PWM1_CH2 = B39,
    PWM1_CH3 = A42,

    // PWM2 Pins
    PWM2_CH0_B38 = B38,
    PWM2_CH0_B12 = B12,
    PWM2_CH1 = A41,
    PWM2_CH2 = B37,
    PWM2_CH3 = A40,

    // PWM3 Pins
    PWM3_CH0_B36 = B36,
    PWM3_CH0_A14 = A14,
    PWM3_CH1 = A38,
    PWM3_CH2 = A36,
    PWM3_CH3 = B34,

    // Dummy LED
    LED1 = GPIO_A30,

    // Not connected
    NC = (int)0xFFFFFFFF
} PinName;

extern const PinName GPIO_A[];
extern const PinName GPIO_B[];

typedef enum {
    PullNone = 0,
    PullDown = 1,
    PullUp   = 2,
    PullDefault = PullUp
} PinMode;


#ifdef __cplusplus
}
#endif

#endif
