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

#ifndef __HAL_CHIPS_HONEY_FLL_H__
#define __HAL_CHIPS_HONEY_FLL_H__

#include "archi/fll/fll_v0.h"

#define CLK_REG_DIV0 (ARCHI_APB_SOC_CTRL_ADDR + 0x08)
#define CLK_REG_DIV1 (ARCHI_APB_SOC_CTRL_ADDR + 0x0C)
#define CLK_REG_DIV2 (ARCHI_APB_SOC_CTRL_ADDR + 0x4C)

// FREQUENCY IN MHz
static inline int fll_set_soc_frequency(int frequency) {
  int multfact = frequency / 32768;

  *(volatile int*)SOC_FLL_CONFIG_REG_1 = ((frequency*0x170/10) << 16) | (multfact & 0xFFFF);

  return multfact * 32768;
}

static inline void fll_wait_lock(void) {
  while(*((volatile int*)SOC_FLL_LOCK_REG) == 0);
}

// This setup the divider between the the soc clock (which can come either from fll or external input clock)
// and the output clock delivered on a pad
static inline void out_clk_div(int divider) {
  *(volatile int*)CLK_REG_DIV2 = divider;
}

#endif
