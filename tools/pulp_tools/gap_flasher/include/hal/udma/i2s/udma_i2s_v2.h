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

#ifndef __HAL_UDMA_UDMA_PERIPH_I2S_V2_H__
#define __HAL_UDMA_UDMA_PERIPH_I2S_V2_H__

#include "archi/udma/i2s/udma_i2s_v2.h"

static inline uint32_t udma_i2s_addr(int itf)
{
  return ARCHI_UDMA_ADDR + UDMA_PERIPH_OFFSET(ARCHI_UDMA_I2S_ID(itf));
}

#endif