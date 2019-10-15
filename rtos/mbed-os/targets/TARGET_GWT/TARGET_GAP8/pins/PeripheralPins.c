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

#include "PeripheralPins.h"

/************RTC***************/
const PinMap PinMap_RTC[] = {
    {NC, OSC32KCLK, 0},
};

/************QSPI***************/
const PinMap PinMap_QSPI_SDIO0[] = {
    {D2 , SPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_QSPI_SDIO1[] = {
    {A11 , SPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_QSPI_SDIO2[] = {
    {B10 , SPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_QSPI_SDIO3[] = {
    {A10 , SPI_0, 0},
    {NC   , NC   , 0}
};

/************SPI***************/
const PinMap PinMap_SPI_MISO[] = {
    {A11 , SPI_0, 0},
    {A4 , SPI_1, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPI_MOSI[] = {
    {D2 , SPI_0, 0},
    {B3 , SPI_1, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPI_SSEL[] = {
    {B8 , SPI_0, 0},
    {A8 , SPI_0, 0},
    {A5 , SPI_1, 0},
    {A3 , SPI_1, 2},
    {A9 , SPI_1, 2},
    {B2 , SPI_1, 2},
    {B15 , SPI_1, 2},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPI_SCLK[] = {
    {B7 , SPI_0, 0},
    {B4 , SPI_1, 0},
    {NC   , NC   , 0}
};

/************SPIS***************/
const PinMap PinMap_SPIS_CSN[] = {
    {A9 , SPIS_0, 0},
    {A15, SPIS_0, 2},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPIS_SDIO0[] = {
    {B15, SPIS_0, 0},
    {A44, SPIS_0, 2},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPIS_SDIO1[] = {
    {A16 , SPIS_0, 0},
    {B40 , SPIS_0, 2},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPIS_SDIO2[] = {
    {B14 , SPIS_0, 2},
    {A44 , SPIS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPIS_SDIO3[] = {
    {A8 , SPIS_0, 2},
    {B40, SPIS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPIS_CLK[] = {
    {B9 , SPIS_0, 0},
    {B13, SPIS_0, 2},
    {NC   , NC   , 0}
};

/************I2C***************/
const PinMap PinMap_I2C_SDA[] = {
    {B22, I2C_0, 0},
    {B34, I2C_1, 0},
    {A5 , I2C_1, 2},
    {B10, I2C_1, 2},
    {NC   , NC   , 0}
};

const PinMap PinMap_I2C_SCL[] = {
    {A25, I2C_0, 0},
    {D1,  I2C_1, 0},
    {B4 , I2C_1, 2},
    {A10, I2C_1, 2},
    {NC   , NC   , 0}
};

/************I2S***************/
const PinMap PinMap_I2S_SCK[] = {
    {A24 , I2S_0, 0},
    {B13 , I2S_1, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_I2S_WS[] = {
    {A26 , I2S_0, 0},
    {A15 , I2S_1, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_I2S_SDI[] = {
    {B23 , I2S_0, 0},
    {B14 , I2S_1, 0},
    {B13 , I2S_1, 3},
    {NC   , NC   , 0}
};

/************ORCA***************/
const PinMap PinMap_ORCA_TXSYNC[] = {
    {A3 , ORCA_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_ORCA_RXSYNC[] = {
    {B2 , ORCA_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_ORCA_TXI[] = {
    {A2 , ORCA_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_ORCA_TXQ[] = {
    {B1 , ORCA_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_ORCA_RXI[] = {
    {A44 , ORCA_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_ORCA_RXQ[] = {
    {B40 , ORCA_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_ORCA_CLK[] = {
    {D1 , ORCA_0, 2},
    {NC   , NC   , 0}
};

/************LVDS***************/
const PinMap PinMap_LVDS_TXD_P[] = {
    {B20 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_TXD_N[] = {
    {A22 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_TXCLK_P[] = {
    {B19 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_TXCLK_N[] = {
    {A21 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_RXD_P[] = {
    {A19 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_RXD_N[] = {
    {B18 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_RXCLK_P[] = {
    {A18 , LVDS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_LVDS_RXCLK_N[] = {
    {B17 , LVDS_0, 0},
    {NC   , NC   , 0}
};

/************HYPERBUS***************/
const PinMap PinMap_HYPERBUS_CLK[] = {
    {B14 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_CLKN[] = {
    {A15 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_DQ0[] = {
    {D2 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ1[] = {
    {A11 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ2[] = {
    {B10 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ3[] = {
    {A10 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ4[] = {
    {B8 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ5[] = {
    {A8 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ6[] = {
    {B7 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ7[] = {
    {A9 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_CSN0[] = {
    {B15 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_CSN1[] = {
    {A16 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_RWDS[] = {
    {B9 , HYPERBUS_0, 3},
    {NC   , NC   , 0}
};
/************CPI***************/
const PinMap PinMap_CPI_PCLK[] = {
    {A43 , CPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_CPI_HSYNC[] = {
    {A37 , CPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_CPI_DATA0[] = {
    {B39 , CPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_CPI_DATA1[] = {
    {A42 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_DATA2[] = {
    {B38 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_DATA3[] = {
    {A41 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_DATA4[] = {
    {B37 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_DATA5[] = {
    {A40 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_DATA6[] = {
    {B36 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_DATA7[] = {
    {A38 , CPI_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_CPI_VSYNC[] = {
    {A36 , CPI_0, 0},
    {NC   , NC   , 0}
};

/************UART***************/
const PinMap PinMap_UART_TX[] = {
    {A7, UART_0, 0},
    {NC  ,  NC    , 0}
};

const PinMap PinMap_UART_RX[] = {
    {B7, UART_0, 0},
    {NC  ,  NC    , 0}
};

/************PWM***************/
const PinMap PinMap_PWM[] = {
    {B11, PWM_1, 0},
    {A13, PWM_2, 0},
    {B12, PWM_3, 0},
    {A14, PWM_4, 0},

    {A43, PWM_5, 2},
    {A13, PWM_5, 2},
    {A37, PWM_6, 2},
    {B39, PWM_7, 2},
    {A42, PWM_8, 2},

    {B38, PWM_9, 2},
    {B12, PWM_9, 2},
    {A41, PWM_10, 2},
    {B37, PWM_11, 2},
    {A40, PWM_12, 2},

    {B36, PWM_13, 2},
    {A14, PWM_13, 2},
    {A38, PWM_14, 2},
    {A36, PWM_15, 2},
    {B34, PWM_16, 2},

    {NC   , NC    , 0}
};


const PinName GPIO_A[] = {
    GPIO_A0_A3, GPIO_A1_B2, GPIO_A2_A2, GPIO_A3_B1, GPIO_A4_A44, GPIO_A5_B40, GPIO_A6, GPIO_A7,
    GPIO_A8, GPIO_A9, GPIO_A10, GPIO_A11, GPIO_A12, GPIO_A13, GPIO_A14, GPIO_A15,
    GPIO_A16, GPIO_A17, GPIO_A18, GPIO_A19, GPIO_A20, GPIO_A21, GPIO_A22, GPIO_A23,
    GPIO_A24, GPIO_A25, GPIO_A26, GPIO_A27, GPIO_A28, GPIO_A29, GPIO_A30, GPIO_A31
};
