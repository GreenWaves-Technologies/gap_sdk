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

#include "rt/rt_api.h"

void __rt_gpio_set_dir(uint8_t group, uint32_t mask, rt_gpio_dir_e is_out)
{
  for (int i=0; i<32; i++)
  {
    if ((mask >> i) & 1)
    {
      apb_soc_gpio_iface_cfg_set_dir(i, is_out);
    }
  }
}
