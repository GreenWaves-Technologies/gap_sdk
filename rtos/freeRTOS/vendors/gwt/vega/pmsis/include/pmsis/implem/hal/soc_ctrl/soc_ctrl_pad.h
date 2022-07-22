/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "pmsis/targets/target.h"

#define ARCHI_PAD_NB_PADFUNC_REG         ( 0x4 )
#define ARCHI_PAD_NB_PAD_PER_PADFUNC     ( ARCHI_NB_PAD / ARCHI_PAD_NB_PADFUNC_REG )
#define ARCHI_PADFUNC_REG_OFFSET(pad)    ( (pad) / ARCHI_PAD_NB_PAD_PER_PADFUNC )
#define ARCHI_PADFUNC_POS_OFFSET(pad)    ( (pad) % ARCHI_PAD_NB_PAD_PER_PADFUNC )

#define ARCHI_PAD_NB_PADCFG_REG          ( 0x10 )
#define ARCHI_PAD_NB_PAD_PER_PADCFG      ( ARCHI_NB_PAD / ARCHI_PAD_NB_PADCFG_REG )
#define ARCHI_PADCFG_REG_OFFSET(pad)     ( (pad) / ARCHI_PAD_NB_PAD_PER_PADCFG )
#define ARCHI_PADCFG_POS_OFFSET(pad)     ( (pad) % ARCHI_PAD_NB_PAD_PER_PADCFG )

#define ARCHI_PAD_NB_SLEEPPADCFG_REG     ( 0x4 )
#define ARCHI_PAD_NB_PAD_PER_SLEEPPADCFG ( ARCHI_NB_PAD >> ARCHI_PAD_NB_SLEEPPADCFG_REG )

#define APB_SOC_STATUS_EOC_SHIFT         ( 31 )


/** PADFUN Register. */
static inline void soc_ctrl_padfun_set(uint32_t base, uint32_t reg_num,
                                       uint32_t value)
{
    uint32_t reg_offset = (uint32_t) APB_SOC_PADFUN0_OFFSET + (reg_num << 2);
    GAP_WRITE(base, reg_offset, value);
}

static inline uint32_t soc_ctrl_padfun_get(uint32_t base, uint32_t reg_num)
{
    uint32_t reg_offset = (uint32_t) APB_SOC_PADFUN0_OFFSET + (reg_num << 2);
    return GAP_READ(base, reg_offset);
}


/** PADCFG Register. */
static inline void soc_ctrl_padcfg_set(uint32_t base, uint32_t reg_num,
                                       uint32_t value)
{
    uint32_t reg_offset = (uint32_t) APB_SOC_PADCFG0_OFFSET + (reg_num << 2);
    GAP_WRITE(base, reg_offset, value);
}

static inline uint32_t soc_ctrl_padcfg_get(uint32_t base, uint32_t reg_num)
{
    uint32_t reg_offset = (uint32_t) APB_SOC_PADCFG0_OFFSET + (reg_num << 2);
    return GAP_READ(base, reg_offset);
}


/*! Corestatus. */
static inline void hal_soc_ctrl_corestatus_set(int32_t value)
{
    uint32_t status = ((1 << APB_SOC_STATUS_EOC_SHIFT) | value);
    apb_soc_corestatus_set((uint32_t) apb_soc_ctrl, status);
}


/*! Padfun. */
/**
 * Pad function configuration.
 * 2b00 - Func0
 * 2b01 - Func1 - GPIO
 * 2b02 - Func2
 * 2b03 - Func3
 */
static inline void hal_pad_pad_function_set(uint32_t pad, uint8_t func)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    uint32_t pad_reg = (uint32_t) ARCHI_PADFUNC_REG_OFFSET(pad);
    uint32_t pad_pos = (uint32_t) ARCHI_PADFUNC_POS_OFFSET(pad);
    uint32_t mask = 0x3, shift = (pad_pos << 1);
    uint32_t val = soc_ctrl_padfun_get(base, pad_reg);
    val &= ~(mask << shift);
    val |= (func << shift);
    soc_ctrl_padfun_set(base, pad_reg, val);
}

static inline void hal_pad_padfunc_set(uint32_t pad_reg, uint32_t value)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    soc_ctrl_padfun_set(base, pad_reg, value);
}

static inline uint32_t hal_pad_padfunc_get(uint32_t pad_reg)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    return soc_ctrl_padfun_get(base, pad_reg);
}


/*! Safe_padcfg. */
/**
 * 8 bits(2 first MSB not used) : NU | DS | PE
 * 2b00 - Not Used
 * xb00 - Drive strength
 * xb00 - Pull enable
 */
/* Set pull activation of a pad. */
static inline void hal_pad_pull_set(uint32_t pad, uint8_t pe)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    uint32_t pad_reg = (uint32_t) ARCHI_PADCFG_REG_OFFSET(pad);
    uint32_t pad_pos = (uint32_t) ARCHI_PADCFG_POS_OFFSET(pad);
    uint32_t mask = 0x1, shift = (pad_pos << 3);
    uint32_t val = soc_ctrl_padcfg_get(base, pad_reg);
    val &= ~(mask << shift);
    val |= (pe << shift);
    soc_ctrl_padcfg_set(base, pad_reg, val);
}

/* Set drive strength of a pad. */
static inline void hal_pad_drive_strength_set(uint32_t pad, uint8_t ds)
{
    /* Get reg number and its position in the register. */
    uint32_t base = (uint32_t) apb_soc_ctrl;
    uint32_t pad_reg = (uint32_t) ARCHI_PADCFG_REG_OFFSET(pad);
    uint32_t pad_pos = (uint32_t) ARCHI_PADCFG_POS_OFFSET(pad);
    uint32_t mask = 0x2, shift = (pad_pos << 3);
    uint32_t val = soc_ctrl_padcfg_get(base, pad_reg);
    val &= ~(mask << shift);
    val |= ((ds << 1) << shift);
    soc_ctrl_padcfg_set(base, pad_reg, val);
}

/* Padcfg pull activation & drive strength configuration. */
static inline void hal_pad_padcfg_config_set(uint32_t pad, uint8_t pe, uint8_t ds)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    uint32_t pad_reg = (uint32_t) ARCHI_PADCFG_REG_OFFSET(pad);
    uint32_t pad_pos = (uint32_t) ARCHI_PADCFG_POS_OFFSET(pad);
    uint32_t mask = 0x3, shift = (pad_pos << 3);
    uint32_t val = soc_ctrl_padcfg_get(base, pad_reg);
    val &= ~(mask << shift);
    val |= (((ds << 1) | pe) << shift);
    soc_ctrl_padcfg_set(base, pad_reg, val);
}

static inline uint32_t hal_pad_padcfg_config_get(uint32_t pad_reg)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    return soc_ctrl_padcfg_get(base, pad_reg);
}


/*! Safe_sleeppadcfg. */
static inline void hal_pad_sleeppadcfg_set(uint8_t pad_reg, uint32_t value)
{
}

static inline uint32_t hal_pad_sleeppadcfg_get(uint8_t pad_reg)
{
    return 0;
}


/*! Safe_padsleep. */
/* Enable sleep mode for pads. */
static inline void hal_pad_padsleep_enable(uint8_t enable)
{
}
