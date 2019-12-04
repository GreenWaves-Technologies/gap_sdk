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
#include "pmsis_hal/GAP8/fll/fll_gap8.h"
#include "pmsis_driver/pmu/pmsis_pmu.h"

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

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t fll_get_mult_div_from_frequency(uint32_t freq, uint32_t *mult, uint32_t *div)
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

int pi_fll_set_frequency(fll_type_t which_fll, uint32_t frequency, int check)
{
    uint32_t val1, val2;
    uint32_t mult, div, mult_factor_diff;

    int irq =  __disable_irq();

    if (check)
    {
        uint32_t curr_voltage = DCDC_TO_mV(pmu_state.DCDC_Settings[READ_PMU_REGULATOR_STATE(pmu_state.State)]);

        if (which_fll == FLL_SOC)
        {
            if (pi_fll_soc_max_freq_at_V(curr_voltage) < (int)frequency)
            {
                __restore_irq(irq);
                return -1;
            }
        }
        else
        {
            if (pi_fll_cluster_max_freq_at_V(curr_voltage) < (int)frequency)
            {
                __restore_irq(irq);
                return -1;
            }
        }
    }

    /* Frequency calculation from theory */
    fll_get_mult_div_from_frequency(frequency, &mult, &div);

    /* Gain : 2-1 - 2-10 (0x2-0xB) */
    /* Return to close loop mode and give gain to feedback loop */
    val2 = FLL_CTRL_CONF2_LOOPGAIN(0x7)         |
           FLL_CTRL_CONF2_DEASSERT_CYCLES(0x10) |
           FLL_CTRL_CONF2_ASSERT_CYCLES(0x10)   |
           FLL_CTRL_CONF2_LOCK_TOLERANCE(0x100) |
           FLL_CTRL_CONF2_CONF_CLK_SEL(0x0)     |
           FLL_CTRL_CONF2_OPEN_LOOP(0x0)        |
           FLL_CTRL_CONF2_DITHERING(0x1);

    if (which_fll) {
        FLL_CTRL->CLUSTER_CONF2 = val2;
    } else {
        FLL_CTRL->SOC_CONF2 = val2;
    }

    /* Configure register 1 */
    val1 = FLL_CTRL_CONF1_MODE(1)            |
           FLL_CTRL_CONF1_MULTI_FACTOR(mult) |
           FLL_CTRL_CONF1_CLK_OUT_DIV(div);

    if (which_fll) {
        FLL_CTRL->CLUSTER_CONF1 = val1;
    } else {
        FLL_CTRL->SOC_CONF1 = val1;
    }

    /* Check FLL converge by compare status register with multiply factor */
    do {
        mult_factor_diff = which_fll ? abs(FLL_CTRL->CLUSTER_FLL_STATUS - mult) :
                                       abs(FLL_CTRL->SOC_FLL_STATUS - mult);
    } while ( mult_factor_diff > 0x10 );

    val2 = FLL_CTRL_CONF2_LOOPGAIN(0xB)      |
        FLL_CTRL_CONF2_DEASSERT_CYCLES(0x10) |
        FLL_CTRL_CONF2_ASSERT_CYCLES(0x10)   |
        FLL_CTRL_CONF2_LOCK_TOLERANCE(0x100) |
        FLL_CTRL_CONF2_CONF_CLK_SEL(0x0)     |
        FLL_CTRL_CONF2_OPEN_LOOP(0x0)        |
        FLL_CTRL_CONF2_DITHERING(0x1);

    if (which_fll) {
        FLL_CTRL->CLUSTER_CONF2 = val2;
    } else {
        FLL_CTRL->SOC_CONF2 = val2;
    }

    if (which_fll == FLL_SOC)
        system_core_clock_update();

    __restore_irq(irq);

    return pi_fll_get_frequency(which_fll);
}

void pi_fll_init(fll_type_t which_fll, uint32_t ret_state)
{
    uint32_t val;

    if (ret_state) {
        pi_fll_get_frequency(which_fll);
    } else {
        val = (which_fll) ? FLL_CTRL->CLUSTER_CONF1 : FLL_CTRL->SOC_CONF1;

        /* Don't set the gain and integrator in case it has already been set by the boot code */
        /* as it totally blocks the fll on the RTL platform. */
        /* The boot code is anyway setting the same configuration. */
        if(!READ_FLL_CTRL_CONF1_MODE(val)) {
            /* Intergrator register */
            /* We are in open loop, prime the fll forcing dco input, approx 50 MHz */
            /* Set int part to 1*/
            val = FLL_CTRL_INTEGRATOR_INT_PART(332);

            if (which_fll) {
                FLL_CTRL->CLUSTER_INTEGRATOR = val;
            } else {
                FLL_CTRL->SOC_INTEGRATOR = val;
            }
        }

        if (pi_fll_set_frequency(which_fll, DEFAULT_SYSTEM_CLOCK, 0) == -1 )
            exit(-1);
    }
}

int pi_fll_get_frequency(fll_type_t which_fll)
{
    /* Frequency calculation from real world */
    int real_freq = which_fll ? fll_get_frequency_from_mult_div(FLL_CTRL->CLUSTER_FLL_STATUS,
                                                            READ_FLL_CTRL_CONF1_CLK_OUT_DIV(FLL_CTRL->CLUSTER_CONF1))
                              : fll_get_frequency_from_mult_div(FLL_CTRL->SOC_FLL_STATUS,
                                                            READ_FLL_CTRL_CONF1_CLK_OUT_DIV(FLL_CTRL->SOC_CONF1));
    /* Update Frequency */
    flls_frequency[which_fll] = real_freq;
    pmu_state.frequency[which_fll] = real_freq;

    return flls_frequency[which_fll];
}

void pi_fll_deinit(fll_type_t which_fll)
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
