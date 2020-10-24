/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __HAL_UDMA_UDMA_V4_H__
#define __HAL_UDMA_UDMA_V4_H__

#include <stdint.h>
#include <archi/udma/udma_v4.h>
#include "udma_ctrl/udma_ctrl.h"
#include "udma_core_lin/udma_core_lin.h"

static inline unsigned int udma_periph_base(int id)
{
  return ARCHI_UDMA_ADDR + UDMA_PERIPH_OFFSET(id);
}

static inline void udma_clockgate_set(uint32_t base, uint32_t id)
{
    base += UDMA_CTRL_OFFSET;
    udma_ctrl_cfg_cg_set(base, __BITCLR_R(udma_ctrl_cfg_cg_get(base), 1, id));
}

static inline void udma_clockgate_clr(uint32_t base, uint32_t id)
{
    base += UDMA_CTRL_OFFSET;
    udma_ctrl_cfg_cg_set(base, __BITSET_R(udma_ctrl_cfg_cg_get(base), 1, id));
}

static inline void udma_reset_set(uint32_t base, uint32_t id)
{
    base += UDMA_CTRL_OFFSET;
    udma_ctrl_cfg_rstn_set(base, __BITCLR_R(udma_ctrl_cfg_rstn_get(base), 1, id));
}

static inline void udma_reset_clr(uint32_t base, uint32_t id)
{
    base += UDMA_CTRL_OFFSET;
    udma_ctrl_cfg_rstn_set(base, __BITSET_R(udma_ctrl_cfg_rstn_get(base), 1, id));
}

static inline void udma_enqueue(unsigned base, unsigned int addr, unsigned int size, unsigned int cfg)
{
    udma_core_lin_addrgen_cfg_sa_buf0_set(base, addr);
    udma_core_lin_addrgen_cfg_size_set(base, size);
    udma_core_lin_addrgen_cfg_ctrl_set(base, cfg | UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));
}

#endif
