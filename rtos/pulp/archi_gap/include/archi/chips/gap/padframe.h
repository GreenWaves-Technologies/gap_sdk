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

#ifndef __ARCHI_GAP_PADFRAME_H__
#define __ARCHI_GAP_PADFRAME_H__

#define PLP_PAD_GPIO_0           8
#define PLP_PAD_GPIO_1           9
#define PLP_PAD_GPIO_2           10
#define PLP_PAD_GPIO_3           11
#define PLP_PAD_GPIO_0_A         12
#define PLP_PAD_GPIO_1_A         13
#define PLP_PAD_GPIO_2_A         14
#define PLP_PAD_GPIO_3_A         15
#define PLP_PAD_GPIO_4_A         16
#define PLP_PAD_GPIO_5_A         17
#define PLP_PAD_GPIO_4           18
#define PLP_PAD_GPIO_5           19
#define PLP_PAD_GPIO_6           20
#define PLP_PAD_GPIO_7           21
#define PLP_PAD_GPIO_8           22
#define PLP_PAD_GPIO_9           23
#define PLP_PAD_GPIO_10          24
#define PLP_PAD_GPIO_11          25
#define PLP_PAD_GPIO_12          26
#define PLP_PAD_GPIO_13          27
#define PLP_PAD_GPIO_14          28
#define PLP_PAD_GPIO_15          29
#define PLP_PAD_GPIO_16          30
#define PLP_PAD_GPIO_17          31
#define PLP_PAD_GPIO_18          32
#define PLP_PAD_GPIO_19          33
#define PLP_PAD_GPIO_20          34
#define PLP_PAD_GPIO_21          35
#define PLP_PAD_GPIO_22          36
#define PLP_PAD_GPIO_23          37
#define PLP_PAD_GPIO_24          38
#define PLP_PAD_GPIO_25          39
#define PLP_PAD_GPIO_26          42
#define PLP_PAD_GPIO_27          43
#define PLP_PAD_GPIO_28          45
#define PLP_PAD_GPIO_29          47
#define PLP_PAD_GPIO_30          48
#define PLP_PAD_GPIO_31          49

#define PLP_PAD_RF_MISO          8
#define PLP_PAD_RF_MOSI          9
#define PLP_PAD_RF_CSN           10
#define PLP_PAD_RF_SCK           11
#define PLP_PAD_GPIO_PACTRL0     12
#define PLP_PAD_GPIO_PACTRL1     13
#define PLP_PAD_GPIO_PACTRL2     14
#define PLP_PAD_GPIO_PACTRL3     15
#define PLP_PAD_GPIO_PACTRL4     16
#define PLP_PAD_GPIO_PACTRL5     17
#define PLP_PAD_CAM_PCLK         18
#define PLP_PAD_CAM_HSYNC        19
#define PLP_PAD_CAM_DATA0        20
#define PLP_PAD_CAM_DATA1        21
#define PLP_PAD_CAM_DATA2        22
#define PLP_PAD_CAM_DATA3        23
#define PLP_PAD_CAM_DATA4        24
#define PLP_PAD_CAM_DATA5        25
#define PLP_PAD_CAM_DATA6        26
#define PLP_PAD_CAM_DATA7        27
#define PLP_PAD_CAM_VSYNC        28
#define PLP_PAD_CAM_SDA          29
#define PLP_PAD_CAM_SCL          30
#define PLP_PAD_TIMER0_CH0       31
#define PLP_PAD_TIMER0_CH1       32
#define PLP_PAD_TIMER0_CH2       33
#define PLP_PAD_TIMER0_CH3       34
#define PLP_PAD_I2S1_SCK         35
#define PLP_PAD_I2S1_WS          36
#define PLP_PAD_I2S1_SDI         37
#define PLP_PAD_UART_RX          38
#define PLP_PAD_UART_TX          39
#define PLP_PAD_SPIM_SDIO0       40
#define PLP_PAD_SPIM_SDIO1       41
#define PLP_PAD_SPIM_SDIO2       42
#define PLP_PAD_SPIM_SDIO3       43
#define PLP_PAD_SPIM_CSN0        44
#define PLP_PAD_SPIM_CSN1        45
#define PLP_PAD_SPIM_SCK         46
#define PLP_PAD_SPIM1_CSN        47
#define PLP_PAD_SPIM1_CS0        47
#define PLP_PAD_SPIM1_MISO       48
#define PLP_PAD_SPIM1_MOSI       49
#define PLP_PAD_SPIM1_SCK        50

#define PLP_PAD_SPIS0_SDIO0      16
#define PLP_PAD_SPIS0_SDIO1      17
#define PLP_PAD_SPIS0_SCK        35
#define PLP_PAD_SPIS0_CS         36
#define PLP_PAD_SPIS0_SDIO2      37
#define PLP_PAD_SPIS0_SDIO3      45

#define PLP_PAD_ORCA_TXSYNC      12
#define PLP_PAD_ORCA_RXSYNC      13
#define PLP_PAD_ORCA_TXI         14
#define PLP_PAD_ORCA_TXQ         15
#define PLP_PAD_ORCA_RXI         16
#define PLP_PAD_ORCA_RXQ         17
#define PLP_PAD_ORCA_CLK         30

#define PLP_PAD_HYPER_CKN        36
#define PLP_PAD_HYPER_CK         37
#define PLP_PAD_HYPER_DQ0        40
#define PLP_PAD_HYPER_DQ1        41
#define PLP_PAD_HYPER_DQ2        42
#define PLP_PAD_HYPER_DQ3        43
#define PLP_PAD_HYPER_DQ4        44
#define PLP_PAD_HYPER_DQ5        45
#define PLP_PAD_HYPER_DQ6        46
#define PLP_PAD_HYPER_DQ7        47
#define PLP_PAD_HYPER_CSN0       48
#define PLP_PAD_HYPER_CSN1       49
#define PLP_PAD_HYPER_RWDS       50

#define PLP_PAD_GPIO_DEFAULT     0
#define PLP_PAD_GPIO_ALTERNATE1  1
#define PLP_PAD_GPIO_ALTERNATE2  2
#define PLP_PAD_GPIO_ALTERNATE3  3

#define PLP_PAD_GPIO_NUM  32

#endif
