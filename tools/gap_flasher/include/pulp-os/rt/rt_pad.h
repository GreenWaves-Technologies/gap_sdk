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
#ifndef __RT__RT_PAD_H__
#define __RT__RT_PAD_H__

#include "rt/rt_api.h"

#if defined(APB_SOC_VERSION)

/// @cond IMPLEM

void __rt_padframe_init();
rt_padframe_profile_t *rt_pad_profile_get(char *profile_string) ;
void rt_padframe_set(rt_padframe_profile_t *profile) ;

void rt_pad_apply(int pad);

static inline void rt_pad_set_power(int pad, int is_on);

static inline void rt_pad_set_function(int pad, int function);

void rt_pad_config(int pad, int function);



typedef union {
  struct {
#if APB_SOC_VERSION == 1
    uint8_t function :1;

#elif APB_SOC_VERSION >= 2
    uint8_t function :2;
    uint8_t is_on    :1;

#endif
  };
  uint8_t raw;
} rt_pad_cfg_t;


extern rt_pad_cfg_t __rt_pads_config[];


static inline void rt_pad_set_function(int pad_id, int function)
{
  rt_pad_cfg_t *pad = &__rt_pads_config[pad_id];
  pad->function = function;
}

static inline void rt_pad_set_power(int pad_id, int is_on)
{
#if APB_SOC_VERSION >= 2
  rt_pad_cfg_t *pad = &__rt_pads_config[pad_id];
  pad->is_on = is_on;
#endif
}

#endif

/// @endcond


#endif
