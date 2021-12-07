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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include <string.h>

#if PULP_CHIP_FAMILY == CHIP_GAP

__attribute__((weak)) rt_dev_t __rt_devices[] = {
  {"camera", 0x509, -1, (void *)&himax_desc, {{}}},
  {"microphone", 0x8, -1, (void *)&i2s_desc, {{}}},
  {"microphone0", 0x8, -1, (void *)&i2s_desc, {{}}},
  {"microphone1", 0x18, -1, (void *)&i2s_desc, {{}}},
  {"hyperflash", 0x3, -1, (void *)&hyperflash_desc, {{8388608}}},
  {"hyperram", 0x3, -1, (void *)NULL, {{8388608}}},
  {"jtag_proxy", -1, 0, (void *)NULL, {{}}},
  {"uart", -1, 0, (void *)NULL, {{}}},
};

__attribute__((weak)) int __rt_nb_devices = 8;

#else

__attribute__((weak)) rt_dev_t __rt_devices[0] = {
};

__attribute__((weak)) int __rt_nb_devices = 0;

#endif


static inline int __rt_dev_nb_devices()
{
  return __rt_nb_devices;
}

rt_dev_t *rt_dev_get(char *dev_string) {

  for (int i=0; i<__rt_dev_nb_devices(); i++) {
    rt_dev_t *dev = &__rt_devices[i];
    if (strcmp(dev_string, dev->name) == 0) return dev;
  }

  return NULL;
}


int rt_dev_mount(rt_dev_t *dev, rt_event_t *event)
{

  return 0;
}
