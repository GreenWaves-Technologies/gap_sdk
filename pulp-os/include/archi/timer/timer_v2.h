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

#ifndef __ARCHI_TIMER_TIMER_V2_H__
#define __ARCHI_TIMER_TIMER_V2_H__

#define PLP_TIMER_CFG_REG_LO   0x00
#define PLP_TIMER_CFG_REG_HI   0x04
#define PLP_TIMER_VALUE_LO     0x08
#define PLP_TIMER_VALUE_HI     0x0C
#define PLP_TIMER_CMP_LO       0x10
#define PLP_TIMER_CMP_HI       0x14
#define PLP_TIMER_START_LOW    0X18
#define PLP_TIMER_START_HIGH   0x1C
#define PLP_TIMER_RESET_LOW    0x20
#define PLP_TIMER_RESET_HIGH   0x24

#define PLP_TIMER_AREA_SIZE    0x40

#define PLP_TIMER_ENABLE_BIT           0
#define PLP_TIMER_RESET_BIT            1
#define PLP_TIMER_IRQ_ENABLE_BIT       2
#define PLP_TIMER_IEM_BIT              3
#define PLP_TIMER_CMP_CLR_BIT          4
#define PLP_TIMER_ONE_SHOT_BIT         5
#define PLP_TIMER_PRESCALER_ENABLE_BIT 6
#define PLP_TIMER_CLOCK_SOURCE_BIT     7
#define PLP_TIMER_PRESCALER_VALUE_BIT  8
#define PLP_TIMER_PRESCALER_VALUE_BITS 8
#define PLP_TIMER_64_BIT               31

#define PLP_TIMER_ACTIVE             1
#define PLP_TIMER_IDLE               0

#define PLP_TIMER_RESET_ENABLED      1
#define PLP_TIMER_RESET_DISABLED     0

#define PLP_TIMER_IRQ_ENABLED        1
#define PLP_TIMER_IRQ_DISABLED       0

#define PLP_TIMER_IEM_ENABLED        1
#define PLP_TIMER_IEM_DISABLED       0

#define PLP_TIMER_CMPCLR_ENABLED     1
#define PLP_TIMER_CMPCLR_DISABLED    0

#define PLP_TIMER_ONE_SHOT_ENABLED   1
#define PLP_TIMER_ONE_SHOT_DISABLED  0

#define PLP_TIMER_REFCLK_ENABLED     1
#define PLP_TIMER_REFCLK_DISABLED    0

#define PLP_TIMER_PRESCALER_ENABLED  1
#define PLP_TIMER_PRESCALER_DISABLED 0

#define PLP_TIMER_MODE_64_ENABLED    1
#define PLP_TIMER_MODE_64_DISABLED   0


#endif
