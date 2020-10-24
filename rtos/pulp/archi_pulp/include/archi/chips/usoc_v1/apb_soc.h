/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __ARCHI_CHIPS_USOC_V1_APB_SOC_H__
#define __ARCHI_CHIPS_USOC_V1_APB_SOC_H__
 
#define APB_SOC_INFO_OFFSET             0x000 //contains number of cores [31:16] and clusters [15:0]
#define APB_SOC_BOOTADDR_OFFSET         0x004
#define APB_SOC_GPIO_IFACE_CFG0_OFFSET  0x074
#define APB_SOC_GPIO_IFACE_CFG1_OFFSET  0x078
#define APB_SOC_GPIO_IFACE_CFG2_OFFSET  0x07C
#define APB_SOC_GPIO_IFACE_CFG3_OFFSET  0x080
#define APB_SOC_GPIO_IFACE_CFG4_OFFSET  0x084
#define APB_SOC_GPIO_IFACE_CFG5_OFFSET  0x088
#define APB_SOC_GPIO_IFACE_CFG6_OFFSET  0x08C
#define APB_SOC_GPIO_IFACE_CFG7_OFFSET  0x090
#define APB_SOC_GPIO_IFACE_CFG_OFFSET(x) (APB_SOC_GPIO_IFACE_CFG0_OFFSET + (x)*4)
#define APB_SOC_JTAG_REG_OFFSET         0x180
#define APB_SOC_CORESTATUS_OFFSET       0x184 //32bit GP register to be used during testing to return EOC(bit[31]) and status(bit[30:0])
#define APB_SOC_CORESTATUS_RO_OFFSET    0x188 //32bit GP register to be used during testing to return EOC(bit[31]) and status(bit[30:0])


#define APB_SOC_GPIO_IFACE_CFG_DIR_BIT      0
#define APB_SOC_GPIO_IFACE_CFG_PADMODE_BIT  1
#define APB_SOC_GPIO_IFACE_CFG_DRV0_BIT     2
#define APB_SOC_GPIO_IFACE_CFG_DRV1_BIT     3
#define APB_SOC_GPIO_IFACE_CFG_PDEN_BIT     4
#define APB_SOC_GPIO_IFACE_CFG_PUEN_BIT     5

#define APB_SOC_GPIO_IFACE_CFG_WIDTH        8
#define APB_SOC_GPIO_IFACE_CFG_NB_GPIO      4

#define APB_SOC_GPIO_IFACE_CFG_BIT(x)       ((x)*APB_SOC_GPIO_IFACE_CFG_WIDTH)

#define APB_SOC_JTAG_REG_EXT_BIT   8
#define APB_SOC_JTAG_REG_EXT_WIDTH 8

#define APB_SOC_JTAG_REG_LOC_BIT   0
#define APB_SOC_JTAG_REG_LOC_WIDTH 8


#define APB_SOC_STATUS_EOC_BIT  31



#ifndef LANGUAGE_ASSEMBLY

typedef union {
  struct {
    unsigned int dir:1;
    unsigned int padmode:1;
    unsigned int drv0:1;
    unsigned int drv1:1;
    unsigned int pden:1;
    unsigned int puen:1;
  };
  unsigned int raw;
} apb_soc_gpio_iface_cfg_t;

#endif

#endif
