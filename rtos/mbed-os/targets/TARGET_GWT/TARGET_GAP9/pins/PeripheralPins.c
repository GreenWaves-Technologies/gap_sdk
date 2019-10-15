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

/************SPIQ***************/
const PinMap PinMap_QSPI_SDIO0[] = {
    {A11 , SPI_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_QSPI_SDIO1[] = {
    {D2 , SPI_0, 0},
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
    {A4  , SPI_1, 0},
    {A5  , SPI_2, 0},
    {A7  , SPI_3, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPI_MOSI[] = {
    {D2 , SPI_0, 0},
    {B3 , SPI_1, 0},
    {B4 , SPI_2, 0},
    {B6 , SPI_3, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPI_SSEL[] = {
    {4  , SPI_0, 0},
    {7  , SPI_1, 0},
    {17 , SPI_2, 0},
    {31 , SPI_3, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_SPI_SCLK[] = {
    {B7 , SPI_0, 0},
    {B4 , SPI_1, 0},
    {B8 , SPI_2, 0},
    {B9 , SPI_3, 0},
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

/************HYPERBUS***************/
const PinMap PinMap_HYPERBUS_CLK[] = {
    {B14 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_CLKN[] = {
    {A15 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_DQ0[] = {
    {D2 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ1[] = {
    {A11 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ2[] = {
    {B10 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ3[] = {
    {A10 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ4[] = {
    {B8 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ5[] = {
    {A8 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ6[] = {
    {B7 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};
const PinMap PinMap_HYPERBUS_DQ7[] = {
    {A9 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_CSN0[] = {
    {B15 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_CSN1[] = {
    {A16 , HYPERBUS_0, 0},
    {NC   , NC   , 0}
};

const PinMap PinMap_HYPERBUS_RWDS[] = {
    {B9 , HYPERBUS_0, 0},
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
    {A8, UART_1, 0},
    {B14, UART_2, 2},
    {NC  ,  NC    , 0}
};

const PinMap PinMap_UART_RX[] = {
    {B6, UART_0, 0},
    {B7, UART_1, 0},
    {A15, UART_2, 2},
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
    GPIO_A0, GPIO_A1, GPIO_A2, GPIO_A3, GPIO_A4, GPIO_A5, GPIO_A6, GPIO_A7,
    GPIO_A8, GPIO_A9, GPIO_A10, GPIO_A11, GPIO_A12, GPIO_A13, GPIO_A14, GPIO_A15,
    GPIO_A16, GPIO_A17, GPIO_A18, GPIO_A19, GPIO_A20, GPIO_A21, GPIO_A22, GPIO_A23,
    GPIO_A24, GPIO_A25, GPIO_A26, GPIO_A27, GPIO_A28, GPIO_A29, GPIO_A30, GPIO_A31
};

const PinName GPIO_B[] = {
    GPIO_B0, GPIO_B1, GPIO_B2, GPIO_B3, GPIO_B4, GPIO_B5, GPIO_B6, GPIO_B7,
    GPIO_B8, GPIO_B9, GPIO_B10, GPIO_B11, GPIO_B12, GPIO_B13, GPIO_B14, GPIO_B15,
    GPIO_B16, GPIO_B17, GPIO_B18, GPIO_B19, GPIO_B20, GPIO_B21, GPIO_B22, GPIO_B23,
    GPIO_B24, GPIO_B25, GPIO_B26, GPIO_B27, GPIO_B28, GPIO_B29, GPIO_B30, GPIO_B31
};
