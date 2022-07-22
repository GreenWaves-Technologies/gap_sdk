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


/*! INFO. */
static inline uint8_t hal_soc_ctrl_nb_clusters_get(void)
{
    uint8_t nb_clusters = apb_soc_info_get((uint32_t) apb_soc_ctrl);
    nb_clusters &= APB_SOC_INFO_NB_CL_MASK;
    return nb_clusters;
}

static inline uint8_t hal_soc_ctrl_nb_cores_get(void)
{
    uint8_t nb_cores = apb_soc_info_get((uint32_t) apb_soc_ctrl);
    nb_cores &= APB_SOC_INFO_NB_CORES_MASK;
    return nb_cores;
}


/*! CL_ISOLATE. */
static inline void hal_soc_ctrl_cl_isolate_set(uint8_t isolate)
{
    apb_soc_cl_isolate_set((uint32_t) apb_soc_ctrl, APB_SOC_CL_ISOLATE_EN(isolate));
}


/*! CL_BUSY. */
static inline uint8_t hal_soc_ctrl_cl_busy_get(void)
{
    return (apb_soc_cl_busy_get((uint32_t) apb_soc_ctrl) & APB_SOC_CL_BUSY_BUSY_MASK);
}


/*! SLEEPCTRL. */
static inline uint32_t hal_soc_ctrl_pmu_sleepctrl_get(void)
{
    return apb_soc_safe_pmu_sleepctrl_get((uint32_t) apb_soc_ctrl);
}

static inline uint32_t hal_soc_ctrl_fast_pmu_sleepctrl_get(void)
{
    return apb_soc_sleep_ctrl_get((uint32_t) apb_soc_ctrl);
}

static inline void hal_soc_ctrl_pmu_sleepctrl_mask_set(uint32_t sleepctrl_mask)
{
    apb_soc_safe_pmu_sleepctrl_set((uint32_t) apb_soc_ctrl, sleepctrl_mask);
}

static inline void hal_soc_ctrl_pmu_sleepctrl_set(uint8_t cl_wake, uint8_t boot_type,
                                                  uint8_t ext_trigger, uint8_t boot_mode,
                                                  uint8_t wake_state, uint8_t ext_wake_ena,
                                                  uint8_t ext_wake_type, uint8_t ext_wake_src,
                                                  uint8_t cl_fll, uint8_t soc_fll,
                                                  uint8_t l2_r3_ret, uint8_t l2_r2_ret,
                                                  uint8_t l2_r1_ret, uint8_t l2_r0_ret)
{
}


/*! CLK_SEL. */
static inline void hal_soc_ctrl_clk_sel_set(uint8_t soc_fll_id, uint8_t cl_fll_id)
{
    uint32_t clk_sel = (APB_SOC_CLK_SEL_CLK_SOC(soc_fll_id) |
                        APB_SOC_CLK_SEL_CLK_CLUSTER(cl_fll_id));
    apb_soc_clk_sel_set((uint32_t) apb_soc_ctrl, clk_sel);
}


/*! CLK_DIV_I3C. */
static inline void hal_soc_ctrl_i3c_clk_div_set(uint8_t i3c_id, uint8_t clk_div)
{
    /**
     * I3C_CLK_DIV[15:8] = CLK_DIV
     * I3C_CLK_DIV[1]    = ENA_I3C_1
     * I3C_CLK_DIV[0]    = ENA_I3C_0
     */
#define __I3C_CLK_DIV_ENA(id)      (1 << (id))
#define __I3C_CLK_DIV_VAL(val)     ((val) << 8)
    uint32_t i3c_clk_div = ( __I3C_CLK_DIV_ENA(i3c_id) |
                             __I3C_CLK_DIV_VAL(clk_div));
    apb_soc_clk_div_i3c_set((uint32_t) apb_soc_ctrl, i3c_clk_div);
}

static inline void hal_soc_ctrl_i3c_clk_div_enable_set(uint8_t i3c_id)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    uint32_t i3c_clk_div = apb_soc_clk_div_i3c_get(base);
    i3c_clk_div |= __I3C_CLK_DIV_ENA(i3c_id);
    apb_soc_clk_div_i3c_set(base, i3c_clk_div);
}

static inline void hal_soc_ctrl_i3c_clk_div_disable_set(uint8_t i3c_id)
{
    uint32_t base = (uint32_t) apb_soc_ctrl;
    uint32_t i3c_clk_div = apb_soc_clk_div_i3c_get(base);
    i3c_clk_div &= ~__I3C_CLK_DIV_ENA(i3c_id);
    apb_soc_clk_div_i3c_set(base, i3c_clk_div);
}
