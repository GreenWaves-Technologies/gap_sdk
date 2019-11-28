/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#ifndef _GAP8_GPIO_H_
#define _GAP8_GPIO_H_

#include "memory_map.h"

#define	PADFUN0_OFFSET		0x140
#define	PADFUN1_OFFSET		0x144
#define	PADFUN2_OFFSET		0x148
#define	PADFUN3_OFFSET		0x14C

#define	PADCFG0_OFFSET		0x180
#define	PADCFG1_OFFSET		0x184
#define	PADCFG2_OFFSET		0x188
#define	PADCFG3_OFFSET		0x18C
#define	PADCFG4_OFFSET		0x190
#define	PADCFG5_OFFSET		0x194
#define	PADCFG6_OFFSET		0x198
#define	PADCFG7_OFFSET		0x19C
#define	PADCFG8_OFFSET		0x1A0
#define	PADCFG9_OFFSET		0x1A4
#define	PADCFG10_OFFSET		0x1A8
#define	PADCFG11_OFFSET		0x1AC
#define	PADCFG12_OFFSET		0x1B0
#define	PADCFG13_OFFSET		0x1B4
#define	PADCFG14_OFFSET		0x1B8
#define	PADCFG15_OFFSET		0x1BC
#define	PADCFG_GROUPS		16

#define ONE_PADFUN_SIZE		2
#define ONE_PADCFG_SIZE		8

#define GAP8_GPIO_NUM 32
#define GPIO_EDGE_FALL 0
#define GPIO_EDGE_RISE 1
#define GPIO_EDGE_FALL_RISE 0x2 //raising then falling edge
#define GPIO_IT_EN  1
#define GPIO_IT_DIS 0

#define	APB_GPIO_BASE_ADDR	(ARCHI_SOC_PERIPHERALS_ADDR+0x1000)

#define GPIO_DIR_OFFSET		    0x0
#define GPIO_IN_OFFSET		    0x4
#define GPIO_OUT_OFFSET		    0x8
#define GPIO_INTEN_OFFSET	    0xC
#define GPIO_INTCFG0_OFFSET	    0x10
#define GPIO_INTCFG1_OFFSET	    0x14
#define GPIO_INTSTATUS_OFFSET	    0x18
#define GPIO_EN_OFFSET       	    0x1C

#define GPIO_DIR_IN 0
#define GPIO_DIR_OUT 1

#define PAD_NO_ALTERNATE -1
#define PAD_NO_GPIO -1
#define PAD_NO_CONFIG_INDEX -1

//Always-on power register

#define AO_PAD_BASE_ADDR	(ARCHI_SOC_PERIPHERALS_ADDR+0x4000)

/*
 * 4 configuration registers from SLEEP_PADCFG0 to SLEEP_PADCFG3
 * each one configure 16 pads so 2 configuration bits by pad
 *   bit 0 is for OUTPUT ENABLE, 0/1 disable/enable
 *   bit 1 is for OUTPUT DATA when only Always On is powered.
 * 1 register PAD_SLEEP to disable/enable the pads output when only Always On is powered.
 */
#define SLEEP_PAD_REG_NB	 4//number of configuration registers
#define REG_SLEEP_PADCFG0_OFFSET 0X150 //0x1A104150
#define REG_SLEEP_PADCFG1_OFFSET 0X154 //0x1A104154
#define REG_SLEEP_PADCFG2_OFFSET 0X158 //0x1A104158
#define REG_SLEEP_PADCFG3_OFFSET 0X15C //0x1A10415C
#define REG_PAD_SLEEP_OFFSET     0X160 //0x1A104160

#define DISABLE_PAD_SLEEP 		0
#define ENABLE_PAD_SLEEP 		1
#define SLEEP_ONE_PADCFG_SIZE		2
#define SLEEP_PAD_OUTPUT_ENABLE 	1
#define SLEEP_PAD_OUTPUT_DISABLE 	0
#define SLEEP_PAD_OUTPUT_VALUE_LOW	0
#define SLEEP_PAD_OUTPUT_VALUE_HIGH	1

#endif

