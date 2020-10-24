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

#ifndef __HAL_CHIPS_VIVOSOC3_1_FLL_H__
#define __HAL_CHIPS_VIVOSOC3_1_FLL_H__

#include "hal/pulp.h"

typedef enum {
  HAL_FLL_REG_SET_0 = 0,
  HAL_FLL_REG_SET_1,
} hal_fll_reg_set_e;

typedef enum {
  HAL_FLL_CL=0,
  HAL_FLL_SOC,
  HAL_FLL_PER,
} hal_fll_e;

static inline void hal_fll_set_sel_set(hal_fll_e fll, hal_fll_reg_set_e set)
{
  IP_WRITE(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll,set);
}

static inline hal_fll_reg_set_e hal_fll_set_sel_get(hal_fll_e fll)
{
  return IP_READ(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll);
}

static inline void hal_fll_set_regulation_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int value, unsigned int tolerance)
{
  fll_reg_fcr0_t reg = {.raw = 0};
  reg.nref_reg_low = value-tolerance;
  reg.nref_reg_high = value+tolerance;
  IP_WRITE(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll+FLL_SET_OFFSET*set+FLL_FREQ_REGULATION_OFFSET,reg.raw);
}

static inline fll_reg_fcr0_t hal_fll_set_regulation_get(hal_fll_e fll, hal_fll_reg_set_e set)
{
  fll_reg_fcr0_t reg = { .raw = IP_READ(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll+FLL_SET_OFFSET*set+FLL_FREQ_REGULATION_OFFSET)};
  return reg;  
}

static inline void hal_fll_set_clk_good_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int value, unsigned int tolerance)
{
  fll_reg_fcr1_t reg = { .raw = 0};
  reg.nref_cg_low = value-tolerance;
  reg.nref_cg_high = value+tolerance;
  IP_WRITE(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll+FLL_SET_OFFSET*set+FLL_FREQ_CLK_GOOD_OFFSET,reg.raw);
}

static inline fll_reg_fcr1_t hal_fll_set_clk_good_get(hal_fll_e fll, hal_fll_reg_set_e set)
{
  fll_reg_fcr1_t reg = { .raw = IP_READ(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll+FLL_SET_OFFSET*set+FLL_FREQ_CLK_GOOD_OFFSET)};
  return reg;    
}

static inline void hal_fll_set_config_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int level, unsigned int dco_lsb, unsigned int dco_msb, unsigned int open_loop)
{
  fll_reg_fcr2_t reg = { .raw = 0};
  reg.level = level;             
  reg.dco_lsb = dco_lsb;          
  reg.dco_msb = dco_msb;           
  reg.open_loop = open_loop;     
  IP_WRITE(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll+FLL_SET_OFFSET*set+FLL_FREQ_CONFIG_OFFSET,reg.raw);
}

static inline fll_reg_fcr2_t hal_fll_set_config_get(hal_fll_e fll, hal_fll_reg_set_e set)
{
  fll_reg_fcr2_t reg = { .raw = IP_READ(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OFFSET*fll+FLL_SET_OFFSET*set+FLL_FREQ_CONFIG_OFFSET)};
  return reg;    
}

static inline unsigned int hal_fll_check_ref_fast(void)
{
  if(0x7 & IP_READ(ARCHI_ANALOG_ADDR, FLL_ANALOG_CLKIPC_OFFSET)) return 1;
  else return 0;
}

static inline unsigned int hal_fll_status_get(hal_fll_e fll)
{
  return IP_READ(ARCHI_SOC_PERIPHERALS_ADDR,FLL_OBSERVATION_OFFSET + FLL_OFFSET*fll);
}

static inline void hal_fll_enable_set(hal_fll_e fll, unsigned int value)
{
  unsigned int reg = IP_READ(ARCHI_APB_SOC_CTRL_ADDR,FLL_CONTROL_OFFSET);

  reg &= ~(1<<fll); // delete old value
  reg |= 1<<fll;

  IP_WRITE(ARCHI_APB_SOC_CTRL_ADDR,FLL_CONTROL_OFFSET,reg);
}

#endif
