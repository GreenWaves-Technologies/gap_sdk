/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_HAL_FLL_H__
#define __PMSIS_IMPLEM_HAL_FLL_H__

#include "pmsis/targets/target.h"


static inline uint32_t fll_ctrl_status_get(uint32_t base)
{
    return hal_read32((volatile void *) (base + FLL_CTRL_STATUS_OFFSET));
}


static inline uint32_t fll_ctrl_conf1_get(uint32_t base)
{
    return hal_read32((volatile void *) (base + FLL_CTRL_CONF1_OFFSET));
}

static inline void fll_ctrl_conf1_set(uint32_t base, uint32_t value)
{
    hal_write32((volatile void *) (base + FLL_CTRL_CONF1_OFFSET), value);
}


static inline uint32_t fll_ctrl_conf2_get(uint32_t base)
{
    return hal_read32((volatile void *) (base + FLL_CTRL_CONF2_OFFSET));
}

static inline void fll_ctrl_conf2_set(uint32_t base, uint32_t value)
{
    hal_write32((volatile void *) (base + FLL_CTRL_CONF2_OFFSET), value);
}


static inline uint32_t fll_ctrl_integrator_get(uint32_t base)
{
    return hal_read32((volatile void *) (base + FLL_CTRL_INTEGRATOR_OFFSET));
}

static inline void fll_ctrl_integrator_set(uint32_t base, uint32_t value)
{
    hal_write32((volatile void *) (base + FLL_CTRL_INTEGRATOR_OFFSET), value);
}


/* Status. */
static inline uint32_t hal_fll_status_get(uint8_t fll_id)
{
    uint32_t base = (uint32_t) fll(fll_id);
    return fll_ctrl_status_get(base);
}

static inline uint32_t hal_fll_status_mult_factor_get(uint8_t fll_id)
{
    uint32_t base = (uint32_t) fll(fll_id);
    fll_ctrl_status_t status = {0};
    status.raw = fll_ctrl_status_get(base);
    return status.mult_factor;
}


/* CONF1. */
static inline uint32_t hal_fll_conf1_get(uint8_t fll_id)
{
    uint32_t base = (uint32_t) fll(fll_id);
    return fll_ctrl_conf1_get(base);
}

static inline void hal_fll_conf1_mask_set(uint8_t fll_id, uint32_t conf1)
{
    uint32_t base = (uint32_t) fll(fll_id);
    fll_ctrl_conf1_set(base, conf1);
}

static inline uint32_t hal_fll_conf1_mult_get(uint8_t fll_id)
{
    //return READ_FLL_CTRL_CONF1_MULTI_FACTOR(conf1);
    fll_ctrl_conf1_t conf1 = {0};
    conf1.raw = hal_fll_conf1_get(fll_id);
    return conf1.mult_factor;
}

static inline uint32_t hal_fll_conf1_div_get(uint8_t fll_id)
{
    //return READ_FLL_CTRL_CONF1_CLK_OUT_DIV(conf1);
    fll_ctrl_conf1_t conf1 = {0};
    conf1.raw = hal_fll_conf1_get(fll_id);
    return conf1.clock_out_divider;
}

#if 0
static inline void hal_fll_conf1_mult_div_update(uint8_t fll_id, uint16_t mult,
                                                 uint8_t div)
{
    uint32_t conf1 = hal_fll_conf1_get(fll_id);
    conf1 &= ~(FLL_CTRL_CONF1_MULTI_FACTOR_MASK | FLL_CTRL_CONF1_CLK_OUT_DIV_MASK);
    conf1 |= (FLL_CTRL_CONF1_MULTI_FACTOR(mult) | FLL_CTRL_CONF1_CLK_OUT_DIV(div));
    hal_fll_conf1_mask_set(fll_id, conf1);
}
#endif

static inline void hal_fll_conf1_mult_div_update(uint8_t fll_id, uint16_t mult,
                                                 uint8_t div)
{
    fll_ctrl_conf1_t conf1 = {0};
    conf1.raw = hal_fll_conf1_get(fll_id);
    conf1.mult_factor = mult;
    conf1.clock_out_divider = div;
    hal_fll_conf1_mask_set(fll_id, conf1.raw);
}


/* CONF2. */
static inline uint32_t hal_fll_conf2_get(uint8_t fll_id)
{
    uint32_t base = (uint32_t) fll(fll_id);
    return fll_ctrl_conf2_get(base);
}

static inline void hal_fll_conf2_mask_set(uint8_t fll_id, uint32_t conf2)
{
    uint32_t base = (uint32_t) fll(fll_id);
    fll_ctrl_conf2_set(base, conf2);
}


/* INTEGRATOR. */
static inline uint32_t hal_fll_integrator_get(uint8_t fll_id)
{
    uint32_t base = (uint32_t) fll(fll_id);
    return fll_ctrl_integrator_get(base);
}

static inline void hal_fll_integrator_set(uint8_t fll_id, uint32_t integrator)
{
    uint32_t base = (uint32_t) fll(fll_id);
    fll_ctrl_integrator_set(base, integrator);
}

static inline void hal_fll_integrator_int_set(uint8_t fll_id, uint16_t integer)
{
    fll_ctrl_integrator_t integrator = {0};
    integrator.raw = hal_fll_integrator_get(fll_id);
    integrator.int_part = integer;
    hal_fll_integrator_set(fll_id, integrator.raw);
}

static inline void hal_fll_integrator_fract_set(uint8_t fll_id, uint16_t fract)
{
    fll_ctrl_integrator_t integrator = {0};
    integrator.raw = hal_fll_integrator_get(fll_id);
    integrator.fract_part = fract;
    hal_fll_integrator_set(fll_id, integrator.raw);
}

static inline void hal_fll_integrator_int_fract_set(uint8_t fll_id,
                                                    uint16_t integer,
                                                    uint16_t fract)
{
    fll_ctrl_integrator_t integrator = {0};
    integrator.int_part = integer;
    integrator.fract_part = fract;
    hal_fll_integrator_set(fll_id, integrator.raw);
}

#endif  /* __PMSIS_IMPLEM_HAL_FLL_H__ */
