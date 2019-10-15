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

#include <stdlib.h>
#include "gap_fll.h"
#include "gap_pmu.h"

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
static uint32_t FLL_GetMultDivFromFrequency(uint32_t freq, uint32_t *mult, uint32_t *div)
{
    uint32_t D = __builtin_pulp_minsi(8, __MAX(1, (8 - (__FL1(freq) - 3 - LOG2_REFCLK))));
    uint32_t M = (freq >> LOG2_REFCLK) * (1 << (D-1));
    *mult = M;
    *div  = D;
    return (((FLL_REF_CLK)*M) / (1 << (D-1)));
}

static uint32_t FLL_GetFrequencyFromMultDiv(uint32_t mult, uint32_t div)
{
    /* FreqOut = Fref * Mult/2^(Div-1)    With Mult on 16 bits and Div on 4 bits */
    uint32_t fref = FLL_REF_CLK;
    uint32_t fres = (div == 0) ? (fref * mult) : (fref * mult) >> (div-1);
    return fres;
}

int FLL_SetFrequency(fll_type_t which_fll, uint32_t frequency, int check)
{
    #ifdef FEATURE_CLUSTER
    if ((which_fll == uFLL_CLUSTER) && PMU_ClusterIsOff())
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
    FLL_GetMultDivFromFrequency(frequency, &mult, &div);

    val1 = s_fllBases[which_fll]->CONF1 &
           ~(FLL_CTRL_CONF1_MULTI_FACTOR_MASK | FLL_CTRL_CONF1_CLK_OUT_DIV_MASK);

    val1 |= FLL_CTRL_CONF1_MULTI_FACTOR(mult) |
            FLL_CTRL_CONF1_CLK_OUT_DIV(div);

    val1 |= FLL_CTRL_CONF1_OUTPUT_LOCK_EN(1) | FLL_CTRL_CONF1_MODE(1);

    s_fllBases[which_fll]->CONF1 = val1;

    if (which_fll == uFLL_SOC) {
        SystemCoreClockUpdate();
        return SystemCoreClock;
    } else {
        return FLL_GetFrequency(which_fll);
    }
}

void FLL_Init(fll_type_t which_fll)
{
    #if !defined( __PLATFORM_FPGA__)

    uint32_t val, val2, inter;
    uint32_t mult, div;

    val = s_fllBases[which_fll]->CONF1;

    mult = READ_FLL_CTRL_CONF1_MULTI_FACTOR(val);
    div  = READ_FLL_CTRL_CONF1_CLK_OUT_DIV(val);

    if (READ_FLL_CTRL_CONF1_MODE(val) == 0) {

        val2 = s_fllBases[which_fll]->CONF2 &
               ~(FLL_CTRL_CONF2_ASSERT_CYCLES_MASK | FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK);

        /* Set Clock Ref lock assert count */
        val2 |= FLL_CTRL_CONF2_ASSERT_CYCLES(6) |
                FLL_CTRL_CONF2_LOCK_TOLERANCE(0x50) ;

        s_fllBases[which_fll]->CONF2 = val2;

        /* We are in open loop, prime the fll forcing dco input, approx 70 MHz */
        val2 = s_fllBases[which_fll]->INTEGRATOR & ~(FLL_CTRL_INTEGRATOR_INT_PART_MASK);

        /* Set int part to 332 */
        val2 |= FLL_CTRL_INTEGRATOR_INT_PART(332);

        s_fllBases[which_fll]->INTEGRATOR = val2;

        /* Lock Fll */
        val |= FLL_CTRL_CONF1_MODE(1)          |
               FLL_CTRL_CONF1_OUTPUT_LOCK_EN(1);

        s_fllBases[which_fll]->CONF1 = val;
    }

    /* In case the FLL frequency was set while it was off, update it immediately */
    uint32_t real_freq = FLL_GetFrequencyFromMultDiv(mult, div);
    flls_frequency[which_fll] = real_freq;

    /* Update Frequency */
    PMU_State.Frequency[which_fll] = real_freq;

    #endif
}

int FLL_GetFrequency(fll_type_t which_fll)
{
    #if !defined( __PLATFORM_FPGA__)
    /* Frequency calculation from real world */
    int real_freq = FLL_GetFrequencyFromMultDiv(s_fllBases[which_fll]->FLL_STATUS,
                                                READ_FLL_CTRL_CONF1_CLK_OUT_DIV(s_fllBases[which_fll]->CONF1));

    /* Update Frequency */
    flls_frequency[which_fll] = real_freq;
    PMU_State.Frequency[which_fll] = real_freq;

    return flls_frequency[which_fll];
    #else
    return SystemCoreClock;
    #endif
}

void FLL_DeInit(fll_type_t which_fll)
{
    flls_frequency[which_fll] = 0;
}

void FLL_Clear()
{
  for (int i = 0; i < FLL_NUM; i++)
  {
    flls_frequency[i] = 0;
  }
}
