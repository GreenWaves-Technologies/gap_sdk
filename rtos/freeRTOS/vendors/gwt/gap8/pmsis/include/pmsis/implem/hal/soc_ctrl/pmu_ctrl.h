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

#ifndef __PMSIS_IMPLEM_HAL_SOC_CTRL_PMU_CTRL_H__
#define __PMSIS_IMPLEM_HAL_SOC_CTRL_PMU_CTRL_H__

#include "pmsis/targets/target.h"


/* INFO register, get number of clusters and cores. */
static inline uint32_t soc_ctrl_info_get(void)
{
    return hal_read32(&(soc_ctrl->info));
}


/* CL_ISOLATE register, isolate cluster, stop transactions on AXI bus between cluster and SoC. */
static inline void soc_ctrl_cl_isolate_set(uint32_t value)
{
    hal_write32(&(soc_ctrl->cl_isolate), value);
}

static inline uint32_t soc_ctrl_cl_isolate_get(void)
{
    return hal_read32(&(soc_ctrl->cl_isolate));
}


/* CL_BUSY register. */
static inline uint32_t soc_ctrl_cl_busy_get(void)
{
    return hal_read32(&(soc_ctrl->cl_busy));
}


/* CL_BYPASS. */
static inline void soc_ctrl_cl_bypass_set(uint32_t value)
{
    hal_write32(&(soc_ctrl->cl_bypass), value);
}

static inline uint32_t soc_ctrl_cl_bypass_get(void)
{
    return hal_read32(&(soc_ctrl->cl_bypass));
}


/* PMU_RAR register, DC/DC configuration. */
static inline void soc_ctrl_safe_pmu_rar_set(uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_pmu_rar), value);
}

static inline uint32_t soc_ctrl_safe_pmu_rar_get(void)
{
    return hal_read32(&(soc_ctrl->safe_pmu_rar));
}


/* SLEEPCTRL register, sleep modes configuration. */
static inline void soc_ctrl_safe_pmu_sleepctrl_set(uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_pmu_sleepctrl), value);
}

static inline uint32_t soc_ctrl_safe_pmu_sleepctrl_get(void)
{
    return hal_read32(&(soc_ctrl->safe_pmu_sleepctrl));
}

static inline uint32_t soc_ctrl_fast_pmu_sleepctrl_get(void)
{
    return hal_read32(&(soc_ctrl->sleep_ctrl));
}

/* FORCE register, L2 memory banks power/retentive configuration. */
static inline void soc_ctrl_safe_pmu_force_set(uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_pmu_force), value);
}

static inline uint32_t soc_ctrl_safe_pmu_force_get(void)
{
    return hal_read32(&(soc_ctrl->safe_pmu_force));
}


/*! INFO. */
static inline uint8_t hal_soc_ctrl_nb_clusters_get(void)
{
    uint8_t nb_clusters = soc_ctrl_info_get();
    nb_clusters &= SOC_CTRL_INFO_NB_CL_MASK;
    return nb_clusters;
}

static inline uint8_t hal_soc_ctrl_nb_cores_get(void)
{
    uint8_t nb_cores = soc_ctrl_info_get();
    nb_cores &= SOC_CTRL_INFO_NB_CORES_MASK;
    return nb_cores;
}


/*! CL_ISOLATE. */
static inline void hal_soc_ctrl_cl_isolate_set(uint8_t isolate)
{
    soc_ctrl_cl_isolate_set(SOC_CTRL_CL_ISOLATE_EN(isolate));
}


/*! CL_BUSY. */
static inline uint8_t hal_soc_ctrl_cl_busy_get(void)
{
    return (soc_ctrl_cl_busy_get() & SOC_CTRL_CL_BUSY_BUSY_MASK);
}


/*! CL_BYPASS. */
static inline uint32_t hal_soc_ctrl_cl_bypass_get(void)
{
    return soc_ctrl_cl_bypass_get();
}

static inline void hal_soc_ctrl_cl_bypass_mask_set(uint32_t bypass_mask)
{
    soc_ctrl_cl_bypass_set(bypass_mask);
}

static inline void hal_soc_ctrl_cl_bypass_set(uint8_t reset, uint8_t fll_ret,
                                              uint8_t fll_pwd, uint8_t cg,
                                              uint8_t byp_clk, uint8_t prog_delay,
                                              uint8_t curr_set, uint8_t state,
                                              uint8_t byp_cfg, uint8_t byp_pow)
{
    soc_ctrl_cl_bypass_t bypass;
    bypass.word = 0;
    bypass.field.rst = reset;
    bypass.field.fll_ret = fll_ret;
    bypass.field.fll_pwd = fll_pwd;
    bypass.field.cg = cg;
    bypass.field.byp_clk = byp_clk;
    bypass.field.prog_del = prog_delay;
    bypass.field.currset = curr_set;
    bypass.field.cl_state = state;
    bypass.field.byp_cfg = byp_cfg;
    bypass.field.byp_pow = byp_pow;
    soc_ctrl_cl_bypass_set(bypass.word);
}

static inline void hal_soc_ctrl_cl_bypass_bypass_set(uint8_t byp_clk,
                                                     uint8_t byp_cfg,
                                                     uint8_t byp_pow)
{
    soc_ctrl_cl_bypass_t bypass;
    bypass.word = soc_ctrl_cl_bypass_get();
    bypass.field.byp_clk = byp_clk;
    bypass.field.byp_cfg = byp_cfg;
    bypass.field.byp_pow = byp_pow;
    soc_ctrl_cl_bypass_set(bypass.word);
}

static inline uint8_t hal_soc_ctrl_cl_bypass_state_get(void)
{
    soc_ctrl_cl_bypass_t bypass;
    bypass.word = soc_ctrl_cl_bypass_get();
    uint8_t cl_state = ((bypass.word & SOC_CTRL_CL_BYPASS_CL_STATE_MASK)
                        >> SOC_CTRL_CL_BYPASS_CL_STATE_SHIFT);
    return cl_state;
}


/*! RAR. */
static inline uint32_t hal_soc_ctrl_pmu_rar_get(void)
{
    return soc_ctrl_safe_pmu_rar_get();
}

static inline void hal_soc_ctrl_pmu_regulator_set(uint32_t rar_value)
{
    /* soc_ctrl_safe_pmu_rar_t rar; */
    /* rar.word = 0; */
    /* rar.field.rv_volt = ret_volt; */
    /* rar.field.lv_volt = low_volt; */
    /* rar.field.mv_volt = med_volt; */
    /* rar.field.nv_volt = nom_volt; */
    soc_ctrl_safe_pmu_rar_set(rar_value);
}

static inline void hal_soc_ctrl_pmu_rar_set(uint8_t ret_volt, uint8_t low_volt,
                                            uint8_t med_volt, uint8_t nom_volt)
{
    soc_ctrl_safe_pmu_rar_t rar;
    rar.word = 0;
    rar.field.rv_volt = ret_volt;
    rar.field.lv_volt = low_volt;
    rar.field.mv_volt = med_volt;
    rar.field.nv_volt = nom_volt;
    soc_ctrl_safe_pmu_rar_set(rar.word);
}

static inline uint8_t hal_soc_ctrl_pmu_ret_volt_get(void)
{
    uint32_t rar = soc_ctrl_safe_pmu_rar_get();
    uint8_t ret_volt = ((rar & SOC_CTRL_SAFE_PMU_RAR_RV_VOLT_MASK) >> SOC_CTRL_SAFE_PMU_RAR_RV_VOLT_SHIFT);
    return ret_volt;
}

static inline void hal_soc_ctrl_pmu_ret_volt_set(uint8_t ret_volt)
{
    soc_ctrl_safe_pmu_rar_t rar;
    rar.word =  soc_ctrl_safe_pmu_rar_get();
    rar.field.rv_volt = ret_volt;
    soc_ctrl_safe_pmu_rar_set(rar.word);
}

static inline uint8_t hal_soc_ctrl_pmu_low_volt_get(void)
{
    uint32_t rar = soc_ctrl_safe_pmu_rar_get();
    uint8_t low_volt = ((rar & SOC_CTRL_SAFE_PMU_RAR_LV_VOLT_MASK) >> SOC_CTRL_SAFE_PMU_RAR_LV_VOLT_SHIFT);
    return low_volt;
}

static inline void hal_soc_ctrl_pmu_low_volt_set(uint8_t low_volt)
{
    soc_ctrl_safe_pmu_rar_t rar;
    rar.word =  soc_ctrl_safe_pmu_rar_get();
    rar.field.lv_volt = low_volt;
    soc_ctrl_safe_pmu_rar_set(rar.word);
}

static inline uint8_t hal_soc_ctrl_pmu_med_volt_get(void)
{
    uint32_t rar = soc_ctrl_safe_pmu_rar_get();
    uint8_t med_volt = ((rar & SOC_CTRL_SAFE_PMU_RAR_MV_VOLT_MASK) >> SOC_CTRL_SAFE_PMU_RAR_MV_VOLT_SHIFT);
    return med_volt;
}

static inline void hal_soc_ctrl_pmu_med_volt_set(uint8_t med_volt)
{
    soc_ctrl_safe_pmu_rar_t rar;
    rar.word =  soc_ctrl_safe_pmu_rar_get();
    rar.field.mv_volt = med_volt;
    soc_ctrl_safe_pmu_rar_set(rar.word);
}

static inline uint8_t hal_soc_ctrl_pmu_nom_volt_get(void)
{
    uint32_t rar = soc_ctrl_safe_pmu_rar_get();
    uint8_t nom_volt = ((rar & SOC_CTRL_SAFE_PMU_RAR_NV_VOLT_MASK) >> SOC_CTRL_SAFE_PMU_RAR_NV_VOLT_SHIFT);
    return nom_volt;
}

static inline void hal_soc_ctrl_pmu_nom_volt_set(uint8_t nom_volt)
{
    soc_ctrl_safe_pmu_rar_t rar;
    rar.word =  soc_ctrl_safe_pmu_rar_get();
    rar.field.nv_volt = nom_volt;
    soc_ctrl_safe_pmu_rar_set(rar.word);
}


/*! SLEEPCTRL. */
static inline uint32_t hal_soc_ctrl_pmu_sleepctrl_get(void)
{
    return soc_ctrl_safe_pmu_sleepctrl_get();
}

static inline uint32_t hal_soc_ctrl_fast_pmu_sleepctrl_get(void)
{
    return soc_ctrl_fast_pmu_sleepctrl_get();
}

static inline void hal_soc_ctrl_pmu_sleepctrl_mask_set(uint32_t sleepctrl_mask)
{
    soc_ctrl_safe_pmu_sleepctrl_set(sleepctrl_mask);
}

static inline void hal_soc_ctrl_pmu_sleepctrl_set(uint8_t cl_wake, uint8_t boot_type,
                                                  uint8_t ext_trigger, uint8_t boot_mode,
                                                  uint8_t wake_state, uint8_t ext_wake_ena,
                                                  uint8_t ext_wake_type, uint8_t ext_wake_src,
                                                  uint8_t cl_fll, uint8_t soc_fll,
                                                  uint8_t l2_r3_ret, uint8_t l2_r2_ret,
                                                  uint8_t l2_r1_ret, uint8_t l2_r0_ret)
{
    soc_ctrl_safe_pmu_sleepctrl_t sleepctrl;
    sleepctrl.word = 0;
    sleepctrl.field.cl_wake = cl_wake;
    sleepctrl.field.bttype = boot_type;
    sleepctrl.field.extint = ext_trigger;
    sleepctrl.field.btdev = boot_mode;
    sleepctrl.field.wakestate = wake_state;
    sleepctrl.field.extwake_en = ext_wake_ena;
    sleepctrl.field.extwake_type = ext_wake_type;
    sleepctrl.field.extwake_src = ext_wake_src;
    sleepctrl.field.cl_fll = cl_fll;
    sleepctrl.field.soc_fll = soc_fll;
    sleepctrl.field.l2_r3 = l2_r3_ret;
    sleepctrl.field.l2_r2 = l2_r2_ret;
    sleepctrl.field.l2_r1 = l2_r1_ret;
    sleepctrl.field.l2_r0 = l2_r0_ret;
    soc_ctrl_safe_pmu_sleepctrl_set(sleepctrl.word);
}


/*! FORCE. */
static inline uint32_t hal_soc_ctrl_pmu_force_pd_get(void)
{
    return soc_ctrl_safe_pmu_force_get();
}

static inline void hal_soc_ctrl_pmu_force_pd_set(uint8_t l2_r3_off, uint8_t l2_r2_off,
                                                 uint8_t l2_r1_off, uint8_t l2_r0_off,
                                                 uint8_t l2_r3_ret, uint8_t l2_r2_ret,
                                                 uint8_t l2_r1_ret, uint8_t l2_r0_ret)
{
    soc_ctrl_safe_pmu_force_t force;
    force.word = 0;
    force.field.pd_l2_r3 = l2_r3_off;
    force.field.pd_l2_r2 = l2_r2_off;
    force.field.pd_l2_r1 = l2_r1_off;
    force.field.pd_l2_r0 = l2_r0_off;
    force.field.ret_l2_r3 = l2_r3_ret;
    force.field.ret_l2_r2 = l2_r2_ret;
    force.field.ret_l2_r1 = l2_r1_ret;
    force.field.ret_l2_r0 = l2_r0_ret;
    soc_ctrl_safe_pmu_force_set(force.word);
}

#endif  /* __PMSIS_IMPLEM_HAL_SOC_CTRL_PMU_CTRL_H__ */
