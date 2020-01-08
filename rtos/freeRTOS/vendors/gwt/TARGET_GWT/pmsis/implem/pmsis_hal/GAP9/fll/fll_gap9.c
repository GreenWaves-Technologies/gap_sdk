/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#ifdef __GAP9__
#include "targets/pmsis_targets.h"
#include "pmsis_hal/pmsis_hal.h"
#include "pmsis_driver/pmu/pmsis_pmu.h"

extern pmu_state_t pmu_state;
extern uint32_t system_core_clock;
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*
 * Fll control
 * FreqOut = Fref * Mult/2^(Div-1)
 * With Mult on 16 bits and Div on 4 bits
 */

/* Maximum Log2(DCO Frequency) */
#define LOG2_MAXDCO     29
/* Maximum Log2(Clok Divider) */
#define LOG2_MAXDIV     15
/* Log2(FLL_REF_CLK=32768) */
#define LOG2_REFCLK     15
/* Maximum Log2(Multiplier) */
#define LOG2_MAXM       (LOG2_MAXDCO - LOG2_REFCLK)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint32_t flls_frequency[FLL_NUM];

/*! @brief Pointers to fll bases for each instance. */
static FLL_CTRL_Type *const s_fllBases[] = FLL_CTRL_BASE_PTRS;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t fll_get_mult_div_from_frequency(uint32_t freq, uint32_t *mult,
        uint32_t *div)
{
    uint32_t D = __builtin_pulp_minsi(8, __MAX(1, (8 - (__FL1(freq) - 3 - LOG2_REFCLK))));
    uint32_t M = (freq >> LOG2_REFCLK) * (1 << (D-1));
    *mult = M;
    *div  = D;
    return (((FLL_REF_CLK)*M) / (1 << (D-1)));
}

static uint32_t fll_get_frequency_from_mult_div(uint32_t mult, uint32_t div)
{
    /* FreqOut = Fref * Mult/2^(Div-1)    With Mult on 16 bits and Div on 4 bits */
    uint32_t fref = FLL_REF_CLK;
    uint32_t fres = (div == 0) ? (fref * mult) : (fref * mult) >> (div-1);
    return fres;
}

int fll_set_frequency(fll_type_t which_fll, uint32_t frequency, int check)
{
    #ifdef FEATURE_CLUSTER
    if ((which_fll == FLL_CLUSTER) && pmu_cluster_is_off())
        return -1;
    #endif

    uint32_t val1;
    uint32_t mult, div;

    /* if (check) { */
    /*     uint32_t curr_voltage = DCDC_TO_mV(PMU_State.DCDC_Settings[READ_PMU_REGULATOR_STATE(PMU_State.State)]); */

    /*     if (which_fll == uFLL_SOC) { */
    /*         if (FLL_SoCMaxFreqAtV(curr_voltage) < (int)frequency) */
    /*             return -1; */
    /*     } else { */
    /*         if (FLL_ClusterMaxFreqAtV(curr_voltage) < (int)frequency) */
    /*             return -1; */
    /*     } */
    /* } */

    /* Frequency calculation from theory */
    fll_get_mult_div_from_frequency(frequency, &mult, &div);

    val1 = s_fllBases[which_fll]->SOC_CONF1 &
           ~(FLL_CTRL_CONF1_MULTI_FACTOR_MASK | FLL_CTRL_CONF1_CLK_OUT_DIV_MASK);

    val1 |= FLL_CTRL_CONF1_MULTI_FACTOR(mult) |
            FLL_CTRL_CONF1_CLK_OUT_DIV(div);

    s_fllBases[which_fll]->SOC_CONF1 = val1;

    system_core_clock_update();

    return fll_get_frequency(which_fll);
}

void fll_init(fll_type_t which_fll)
{
    #if !defined( __PLATFORM_FPGA__)

    uint32_t val, val2, inter;
    uint32_t mult, div;

    val = s_fllBases[which_fll]->SOC_CONF1;

    mult = READ_FLL_CTRL_CONF1_MULTI_FACTOR(val);
    div  = READ_FLL_CTRL_CONF1_CLK_OUT_DIV(val);

    if (READ_FLL_CTRL_CONF1_MODE(val) == 0) {

        val2 = s_fllBases[which_fll]->SOC_CONF2 &
               ~(FLL_CTRL_CONF2_ASSERT_CYCLES_MASK | FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK);

        /* Set Clock Ref lock assert count */
        val2 |= FLL_CTRL_CONF2_ASSERT_CYCLES(6) |
                FLL_CTRL_CONF2_LOCK_TOLERANCE(0x50) ;

        s_fllBases[which_fll]->SOC_CONF2 = val2;

        /* We are in open loop, prime the fll forcing dco input, approx 70 MHz */
        val2 = s_fllBases[which_fll]->SOC_INTEGRATOR & ~(FLL_CTRL_INTEGRATOR_INT_PART_MASK);

        /* Set int part to 332 */
        val2 |= FLL_CTRL_INTEGRATOR_INT_PART(332);

        s_fllBases[which_fll]->SOC_INTEGRATOR = val2;

        /* Lock Fll */
        val |= FLL_CTRL_CONF1_MODE(1)          |
               FLL_CTRL_CONF1_OUTPUT_LOCK_EN(1);

        s_fllBases[which_fll]->SOC_CONF1 = val;
    }

    /* In case the FLL frequency was set while it was off, update it immediately */
    uint32_t real_freq = fll_get_frequency_from_mult_div(mult, div);
    flls_frequency[which_fll] = real_freq;

    /* Update Frequency */
    pmu_state.frequency[which_fll] = real_freq;

    #endif
}

int fll_get_frequency(fll_type_t which_fll)
{
    #if !defined( __PLATFORM_FPGA__)
    /* Frequency calculation from real world */
    int real_freq = fll_get_frequency_from_mult_div(s_fllBases[which_fll]->SOC_FLL_STATUS,
                                                READ_FLL_CTRL_CONF1_CLK_OUT_DIV(s_fllBases[which_fll]->SOC_CONF1));

    /* Update Frequency */
    flls_frequency[which_fll] = real_freq;
    pmu_state.frequency[which_fll] = real_freq;

    return flls_frequency[which_fll];
    #else
    return system_core_clock;
    #endif
}

void fll_deinit(fll_type_t which_fll)
{
    flls_frequency[which_fll] = 0;
}

void fll_clear()
{
  for (int i = 0; i < FLL_NUM; i++)
  {
    flls_frequency[i] = 0;
  }
}
#endif  /* __GAP9__ */
