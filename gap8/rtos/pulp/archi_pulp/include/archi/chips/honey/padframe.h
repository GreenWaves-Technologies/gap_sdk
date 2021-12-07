/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
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

#ifndef __ARCHI_CHIPS_HONEY_PADFRAME_H__
#define __ARCHI_CHIPS_HONEY_PADFRAME_H__

#define GPIO_REG_PADDIR               ( ARCHI_GPIO_ADDR + 0x00 )
#define GPIO_REG_PADIN                ( ARCHI_GPIO_ADDR + 0x04 )
#define GPIO_REG_PADOUT               ( ARCHI_GPIO_ADDR + 0x08 )
#define GPIO_REG_INTEN                ( ARCHI_GPIO_ADDR + 0x0C )
#define GPIO_REG_INTTYPE0             ( ARCHI_GPIO_ADDR + 0x10 )
#define GPIO_REG_INTTYPE1             ( ARCHI_GPIO_ADDR + 0x14 )
#define GPIO_REG_INTSTATUS            ( ARCHI_GPIO_ADDR + 0x18 )

#define GPIO_REG_PADCFG0              ( ARCHI_GPIO_ADDR + 0x20 )
#define GPIO_REG_PADCFG1              ( ARCHI_GPIO_ADDR + 0x24 )
#define GPIO_REG_PADCFG2              ( ARCHI_GPIO_ADDR + 0x28 )
#define GPIO_REG_PADCFG3              ( ARCHI_GPIO_ADDR + 0x2C )
#define GPIO_REG_PADCFG4              ( ARCHI_GPIO_ADDR + 0x30 )
#define GPIO_REG_PADCFG5              ( ARCHI_GPIO_ADDR + 0x24 )
#define GPIO_REG_PADCFG6              ( ARCHI_GPIO_ADDR + 0x38 )
#define GPIO_REG_PADCFG7              ( ARCHI_GPIO_ADDR + 0x3C )

#define SOC_CTRL_PADFUN0              ( ARCHI_APB_SOC_CTRL_ADDR + 0x00 )
#define SOC_CTRL_PADFUN1              ( ARCHI_APB_SOC_CTRL_ADDR + 0x04 )

#define GPIO_INT_RISE        0
#define GPIO_INT_FALL        1

#define GPIO_INT_ENABLE      0
#define GPIO_INT_DISABLE     1

#define PIN_SSPI_SIO0_GP0         0
#define PIN_SSPI_SIO1_GP1         1
#define PIN_SSPI_SIO2_GP2         2
#define PIN_SSPI_SIO3_GP3         3
#define PIN_SSPI_CSN_GP4          4
#define PIN_SSPI_SCK              5
#define PIN_MSPI_SIO0_GP5         6
#define PIN_MSPI_SIO1_GP6         7
#define PIN_MSPI_SIO2_GP7         8
#define PIN_MSPI_SIO3_GP8         9
#define PIN_MSPI_CSN0_GP9         10
#define PIN_MSPI_SCK_GP10         11
#define PIN_UART_TX_GP11          12
#define PIN_UART_RX_GP12          13
#define PIN_I2C_SDA_GP13          14
#define PIN_I2C_SCL_GP14          15
#define PIN_AUDIO_I2S_WS_GP15     16
#define PIN_AUDIO_I2S_SCK_GP16    17
#define PIN_AUDIO_I2S_SDI0_GP17   18
#define PIN_AUDIO_I2S_SDI1_GP18   19
#define PIN_CAM_I2S_SCK_GP19      20
#define PIN_CAM_I2S_WS_GP20       21
#define PIN_CAM_I2S_SDI0_GP21     22
#define PIN_CAM_I2S_SDI1_GP22     23
#define PIN_POWER_DOWN_GP23       24



#define PIN_SSPI_SIO0 0
#define PIN_SSPI_SIO1 1
#define PIN_SSPI_SIO2 2
#define PIN_SSPI_SIO3 3
#define PIN_SSPI_CSN  4

#define PIN_MSPI_SIO0 5
#define PIN_MSPI_SIO1 6
#define PIN_MSPI_SIO2 7
#define PIN_MSPI_SIO3 8
#define PIN_MSPI_CSN0 9
#define PIN_MSPI_SCK 10

#define PIN_UART_TX 11
#define PIN_UART_RX 12

#define PIN_I2C_SDA 13
#define PIN_I2C_SCL 14

#define PIN_AUDIO_I2S_WS 15
#define PIN_AUDIO_I2S_SCK 16
#define PIN_AUDIO_I2S_SDI0 17
#define PIN_AUDIO_I2S_SDI1 18

#define PIN_CAM_I2S_SCK 19
#define PIN_CAM_I2S_WS 20
#define PIN_CAM_I2S_SDI0 21
#define PIN_CAM_I2S_SDI1 22

#define PIN_POWER_DOWN 23

#define PIN_GPIO0 0
#define PIN_GPIO1 1
#define PIN_GPIO2 2
#define PIN_GPIO3 3
#define PIN_GPIO4 4
#define PIN_GPIO5 6
#define PIN_GPIO6 7
#define PIN_GPIO7 8
#define PIN_GPIO8 9
#define PIN_GPIO9 10
#define PIN_GPIO10 11
#define PIN_GPIO11 12
#define PIN_GPIO12 13
#define PIN_GPIO13 14
#define PIN_GPIO14 15
#define PIN_GPIO15 16
#define PIN_GPIO16 17
#define PIN_GPIO17 18
#define PIN_GPIO18 19
#define PIN_GPIO19 20
#define PIN_GPIO20 21
#define PIN_GPIO21 22
#define PIN_GPIO22 23
#define PIN_GPIO23 24


#define PIN_SSPI_SIO0_FUN 0
#define PIN_SSPI_SIO1_FUN 0
#define PIN_SSPI_SIO2_FUN 0
#define PIN_SSPI_SIO3_FUN 0
#define PIN_SSPI_CSN_FUN  0

#define PIN_MSPI_SIO0_FUN 0
#define PIN_MSPI_SIO1_FUN 0
#define PIN_MSPI_SIO2_FUN 0
#define PIN_MSPI_SIO3_FUN 0
#define PIN_MSPI_CSN0_FUN 0
#define PIN_MSPI_SCK_FUN 0

#define PIN_UART_TX_FUN 0
#define PIN_UART_RX_FUN 0

#define PIN_I2C_SDA_FUN 0
#define PIN_I2C_SCL_FUN 0

#define PIN_AUDIO_I2S_WS_FUN 0
#define PIN_AUDIO_I2S_SCK_FUN 0
#define PIN_AUDIO_I2S_SDI0_FUN 0
#define PIN_AUDIO_I2S_SDI1_FUN 0

#define PIN_CAM_I2S_SCK_FUN 0
#define PIN_CAM_I2S_WS_FUN 0
#define PIN_CAM_I2S_SDI0_FUN 0
#define PIN_CAM_I2S_SDI1_FUN 0

#define PIN_POWER_DOWN_FUN 0

#define PIN_GPIO0_FUN 1
#define PIN_GPIO1_FUN 1
#define PIN_GPIO2_FUN 1
#define PIN_GPIO3_FUN 1
#define PIN_GPIO4_FUN 1
#define PIN_GPIO5_FUN 1
#define PIN_GPIO6_FUN 1
#define PIN_GPIO7_FUN 1
#define PIN_GPIO8_FUN 1
#define PIN_GPIO9_FUN 1
#define PIN_GPIO10_FUN 1
#define PIN_GPIO11_FUN 1
#define PIN_GPIO12_FUN 1
#define PIN_GPIO13_FUN 1
#define PIN_GPIO14_FUN 1
#define PIN_GPIO15_FUN 1
#define PIN_GPIO16_FUN 1
#define PIN_GPIO17_FUN 1
#define PIN_GPIO18_FUN 1
#define PIN_GPIO19_FUN 1
#define PIN_GPIO20_FUN 1
#define PIN_GPIO21_FUN 1
#define PIN_GPIO22_FUN 1
#define PIN_GPIO23_FUN 1






#define FUNC_SPI  0
#define FUNC_NOM  0
#define FUNC_GPIO 1

#define DIR_IN  0
#define DIR_OUT 1

#endif // _GPIO_H_
