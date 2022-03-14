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


#include "pmsis.h"


typedef struct
{
    uint32_t base;
    uint32_t freq;
} pi_fll_t;



static inline unsigned int __pi_fll_addr_get(int fll_id)
{
    return FLL_ADDR + fll_id*8;
}


// Target integration period (frequency at which the fll will check real frequency against target frequency)
// Since we are dividing the ref clock by an integer, the real integration frequency is an approximation
// of the target one.
#define FLL_TARGET_ITG_FREQ       (1000000)

// Integration period. The ref clock will be divided by this number to trigger the the comparison
// of estimated frequency against target frequency.
#define FLL_ITG_PER       (ARCHI_FLL_REF_CLOCK / FLL_TARGET_ITG_FREQ)

// Since we divide the ref clock by an integer, we need to compute the real integration frequency to be
// more precise in the fll settings
#define FLL_REAL_ITG_FREQ    (ARCHI_FLL_REF_CLOCK / FLL_ITG_PER)


// FLL SW state will not be reset after retentive wakeup since the HW is keeping the config
static PI_FC_TINY pi_fll_t __pi_fll[ARCHI_NB_FLL] = {0};


static uint32_t __pi_fll_get_max_mul()
{
    // TODO should get voltage from pmu driver
    int voltage = 800;
    int max_dco_freq = voltage == 650 ? 600000000 : 900000000;
    // Feedback loop is dividing DCO freq by 2
    return max_dco_freq / 2 / FLL_REAL_ITG_FREQ;
}


static unsigned int __pi_fll_get_mult_div_from_freq(unsigned int freq, unsigned int *mult, unsigned int *div)
{
    unsigned int fres;

    // Try to have M as close as possible to 1000 without getting above
    int maxm = __pi_fll_get_max_mul();
    if (maxm > 1000)
    {
        maxm = 1000;
    }

    // Compute the minimum divider we can get so that we are as close as possible to the
    // maximum multiplication factor
    unsigned int D = maxm * FLL_REAL_ITG_FREQ / freq;

    // Compute the multiplication factor for this divider
    unsigned int M = D * freq / FLL_REAL_ITG_FREQ;

    // And compute the real frequency
    fres = FLL_REAL_ITG_FREQ * M / D;

    *mult = M;
    *div = D;

    return fres;
}


static void __pi_fll_set_mult_div(int fll, uint32_t base, int mult, int div)
{
    fll_f0cr2_set(base, FLL_F0CR2_MFI(mult));
    uint32_t ccr1 = fll_ccr1_get(FLL_ADDR);
    ccr1 = __BITINSERT_R(ccr1, div, 8, 8*fll);
    fll_ccr1_set(FLL_ADDR, ccr1);
}


static int __pi_fll_freq_set(int id, int frequency, pi_fll_t *fll)
{
    FREQ_INF("Setting FLL frequency (fll: %d, frequency: %d)\n", id, frequency);

    unsigned int real_freq, mult, div;
    real_freq = __pi_fll_get_mult_div_from_freq(frequency, &mult, &div);

    fll->freq = real_freq;
    __pi_fll_set_mult_div(id, fll->base, mult, div);

    return real_freq;
}



// This function should be placed in standard L2 location, not XIP, as it will change FLL frequency
// and adapt boot device divider
PI_TEXT_L2 int __pi_fll_init(int id, int frequency)
{
#if !defined(__PLATFORM_FPGA__)
    pi_fll_t *fll = &__pi_fll[id];
    uint32_t base = fll->base;

    FREQ_INF("Initializing FLL (fll: %d)\n", id);

    if (frequency == 0)
    {
        frequency = fll->freq;
    }

    int freq;

    /* Lock Fll */
    __pi_fll_freq_set(id, frequency, fll);

    fll_f0cr1_set(base,
        FLL_F0CR1_DCO_EN(1) |
        FLL_F0CR1_OP_MODE(1) |
        FLL_F0CR1_LOOP_GAIN(0x4) |
        FLL_F0CR1_LOCK_TOL(10) |
        FLL_F0CR1_ITG_PER(FLL_ITG_PER - 1) |
        FLL_F0CR1_STBL(6)
    );

    // Power on DCO
    uint32_t ccr2 = fll_ccr2_get(FLL_ADDR);
    ccr2 = __BITSET_R(ccr2, 1, 16 + id);
    fll_ccr2_set(FLL_ADDR, ccr2);

    FREQ_INF("FLL is locked (fll: %d, freq: %d)\n", id, freq);

    return 0;
#else
    return 0;
#endif
}


void __pi_fll_deinit(int id)
{
    pi_fll_t *fll = &__pi_fll[id];

    // Shutdown DCO
    fll_f0cr1_set(fll->base,
        FLL_F0CR1_DCO_EN(0) |
        FLL_F0CR1_OP_MODE(0)
    );

    uint32_t ccr2 = fll_ccr2_get(FLL_ADDR);
    ccr2 = __BITCLR_R(ccr2, 1, 16 + id);
    fll_ccr2_set(FLL_ADDR, ccr2);
}


// This function should be placed in standard L2 location, not XIP, as it will change FLL frequency
// and adapt boot device divider
PI_TEXT_L2 void __pi_fll_init_all()
{
#if __PLATFORM__ != ARCHI_PLATFORM_FPGA

    for (int i=0; i<ARCHI_NB_FLL; i++)
    {
        __pi_fll[i].base = __pi_fll_addr_get(i);
    }

#ifndef CONFIG_OPEN_LOOP
    // In retentive mode, the FLL will automatically get back its previous settings
#ifndef __FREERTOS__
    if (pi_pmu_get_prev_state(PI_PMU_DOMAIN_CHIP) != PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE)
#endif
    {
        // Set maximum DCO value to the maximum possible value
        fll_drr_set(FLL_ADDR, 0x1ff << 16);

        __pi_fll_init(PI_FREQ_DOMAIN_FC, CONFIG_FREQUENCY_FC);
        __pi_fll_init(PI_FREQ_DOMAIN_PERIPH, CONFIG_FREQUENCY_PERIPH);
        __pi_fll_freq_set(PI_FREQ_DOMAIN_CL, CONFIG_FREQUENCY_CLUSTER, &__pi_fll[PI_FREQ_DOMAIN_CL]);
        __pi_fll_freq_set(PI_FREQ_DOMAIN_SFU, CONFIG_FREQUENCY_SFU, &__pi_fll[PI_FREQ_DOMAIN_SFU]);

        // Connect all DCOs separatly to each clock domain and only turn-on
        // FC and periph DCOs. apply this settings only once DCO are configured to not start
        // with invalid config.
        fll_ccr2_set(FLL_ADDR, (0x3 << 16) | (0b0100<<12) | (0b11 << 8) | (0b10 << 4) | (0x1<<0));
    }
    
#else
    __pi_fll[PI_FREQ_DOMAIN_FC].freq = 50000000;
    __pi_fll[PI_FREQ_DOMAIN_PERIPH].freq = 50000000;
    __pi_fll[PI_FREQ_DOMAIN_CL].freq = 50000000;
    __pi_fll[PI_FREQ_DOMAIN_SFU].freq = 50000000;
#endif

#else
    __pi_fll[PI_FREQ_DOMAIN_FC].freq = CONFIG_FREQUENCY_FPGA;
    __pi_fll[PI_FREQ_DOMAIN_PERIPH].freq = CONFIG_FREQUENCY_FPGA;
    __pi_fll[PI_FREQ_DOMAIN_CL].freq = CONFIG_FREQUENCY_FPGA;
    __pi_fll[PI_FREQ_DOMAIN_SFU].freq = CONFIG_FREQUENCY_FPGA;
#endif

}


uint32_t pi_freq_get(pi_freq_domain_e domain)
{
    return __pi_fll[domain].freq;
}



static inline uint32_t __pi_freq_check_max_freq(pi_freq_domain_e domain, uint32_t freq)
{
#if defined CONFIG_MAX_FREQUENCY_PERIPH
    if (domain == PI_FREQ_DOMAIN_PERIPH)
    {
        freq = CONFIG_MAX_FREQUENCY_PERIPH;
    }
#endif

#if defined CONFIG_MAX_FREQUENCY_FC
    if (domain == PI_FREQ_DOMAIN_FC)
    {
        freq = CONFIG_MAX_FREQUENCY_FC;
    }
#endif

#if defined CONFIG_MAX_FREQUENCY_CLUSTER
    if (domain == PI_FREQ_DOMAIN_CL)
    {
        freq = CONFIG_MAX_FREQUENCY_CLUSTER;
    }
#endif

#if defined CONFIG_MAX_FREQUENCY_SFU
    if (domain == PI_FREQ_DOMAIN_SFU)
    {
        freq = CONFIG_MAX_FREQUENCY_SFU;
    }
#endif

    return freq;
}



int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq)
{
    freq = __pi_freq_check_max_freq(domain, freq);

#ifndef __FREERTOS__
    pos_cbsys_exec(POS_CBSYS_PERIPH_SETFREQ_BEFORE);
#endif

    __pi_fll_freq_set(domain, freq, &__pi_fll[domain]);

#ifndef __FREERTOS__
    pos_cbsys_exec(POS_CBSYS_PERIPH_SETFREQ_AFTER);
#endif

    return 0;
}



#if defined(__FREERTOS__)

#if !defined(CONFIG_NO_FAST_OSC)
/* Fast clock setting. */
int32_t pi_ref_fast_clock_frequency_set(uint32_t freq)
{
    uint32_t res = 0;
    uint32_t freq_def = (uint32_t) ARCHI_FLL_REF_CLOCK;
    res = freq_def / freq;
    uint32_t clk_div_pow2 = __FL1(res);
    uint32_t nb_bits = 0;
    asm volatile("p.cnt %0, %1" : "=r"(nb_bits) : "r"(res));

    if ((clk_div_pow2 > APB_SOC_CTRL_CLK_DIV_REF_FAST_POW2_DIVIDER_MASK) ||
        (nb_bits > 1))
    {
        printf("Can not set freq=%ld\n", freq);
        return -1;
    }
    hal_soc_ctrl_ref_fast_clk_set(clk_div_pow2);

    /* Update timer using Ref Fast Clk as clock source. */
    if (TIMER_CLK_SRC == TIMER_CFG_CLK_SRC_REF)
    {
        system_core_clock_update(freq);
    }
    return 0;
}

uint32_t pi_ref_fast_clock_frequency_get(void)
{
    uint32_t res = 0;
    uint32_t freq = 0;
    uint32_t freq_def = (uint32_t) ARCHI_FLL_REF_CLOCK;
    uint8_t clk_div_pow2 = hal_soc_ctrl_ref_fast_clk_div_pow2_get();
    res = (1 << clk_div_pow2);
    freq = freq_def / res;
    return freq;
}
#endif  /* CONFIG_NO_FAST_OSC */

#endif



// TODO could be reintegrated when needed
#if 0
static inline uint32_t pi_fll_mult_get(int fll)
{
    uint32_t base = __pi_fll_addr_get(fll);
    uint32_t cr2 = fll_f0cr2_get(base);
    return FLL_F0CR2_MFI_GET(cr2);
}


static inline void pi_fll_mult_set(int fll, int mult)
{
    uint32_t base = __pi_fll_addr_get(fll);
    fll_f0cr2_set(base, FLL_F0CR2_MFI(mult));
}


static inline uint32_t pi_fll_div_get(int fll)
{
    uint32_t ccr1 = fll_ccr1_get(ARCHI_FLL_ADDR);
    return GAP_BEXTRACTU(ccr1, 8, 8*fll);
}


static inline void pi_fll_div_set(int fll, int div)
{
    uint32_t ccr1 = fll_ccr1_get(ARCHI_FLL_ADDR);
    ccr1 = __BITINSERT_R(ccr1, div, 8, 8*fll);
    fll_ccr1_set(ARCHI_FLL_ADDR, ccr1);
}
#endif
