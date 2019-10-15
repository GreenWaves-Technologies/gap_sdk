/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __RT_IMPLEM_VEGA_H__
#define __RT_IMPLEM_VEGA_H__

#include "rt/rt_api.h"

#define CONFIG_ALLOC_TRACK_PWD 1
#define CONFIG_ALLOC_L2_PWD_NB_BANKS 12
#define CONFIG_ALLOC_L2_PWD_BANK_SIZE_LOG2 17

extern RT_FC_TINY_DATA uint32_t __rt_alloc_l2_btrim_stdby;
extern RT_FC_TINY_DATA uint32_t __rt_alloc_l2_pwr_ctrl;

static inline void __rt_alloc_power_ctrl(uint32_t banks, uint32_t power, uint32_t standby)
{
  uint32_t base = ARCHI_APB_SOC_CTRL_ADDR;
  if (standby)
  {
    if (power)
    {
      __rt_alloc_l2_btrim_stdby |= (banks << 4);
    }
    else
    {
      __rt_alloc_l2_btrim_stdby &= ~(banks << 4);
    }
    apb_soc_safe_l2_btrim_stdby_set(base, __rt_alloc_l2_btrim_stdby);
  }

  if (power)
  {
    __rt_alloc_l2_pwr_ctrl &= ~(banks << 0);
    if (!standby)
      __rt_alloc_l2_pwr_ctrl &= ~(banks << 16);
  }
  else
  {
    __rt_alloc_l2_pwr_ctrl |= (banks << 0);
    if (!standby)
      __rt_alloc_l2_pwr_ctrl |= (banks << 16);
  }
  apb_soc_safe_l2_pwr_ctrl_set(base, __rt_alloc_l2_pwr_ctrl);
}

#endif