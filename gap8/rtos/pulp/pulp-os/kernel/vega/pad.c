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

#include "rt/rt_api.h"


#define APB_SOC_SAFE_SLEEPPADCFG0_NB_PADS (32/APB_SOC_SAFE_SLEEPPADCFG0_STATE_WIDTH)


void rt_pad_sleep_state(rt_pad_e pad, rt_pad_sleep_state_e state)
{
  int reg_id = pad / APB_SOC_SAFE_SLEEPPADCFG0_NB_PADS;
  int reg_pad = pad % APB_SOC_SAFE_SLEEPPADCFG0_NB_PADS;

  uint32_t value = ARCHI_READ(ARCHI_APB_SOC_CTRL_ADDR, APB_SOC_SAFE_SLEEPPADCFG0_OFFSET + reg_id*4);
  value = ARCHI_BINSERT_R(value, state, APB_SOC_SAFE_SLEEPPADCFG0_STATE_WIDTH, reg_pad*APB_SOC_SAFE_SLEEPPADCFG0_STATE_WIDTH);

  ARCHI_WRITE(ARCHI_APB_SOC_CTRL_ADDR, APB_SOC_SAFE_SLEEPPADCFG0_OFFSET + reg_id*4, value);
}