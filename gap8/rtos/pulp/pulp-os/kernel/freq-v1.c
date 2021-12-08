/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

/*
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

typedef struct rt_freq_constraint_s {
  unsigned int freq;
  struct rt_freq_constraint_s *next;
} rt_freq_constraint_t;

int __rt_freq_domains[RT_FREQ_NB_DOMAIN];
static rt_freq_constraint_t *__rt_freq_first_constraint = NULL;
static int __rt_periph_lcm = 0;
static int __rt_periph_gcd = 0;

static int __rt_check_fc_freq(unsigned int freq)
{
  // TODO we should check if the freq is allowed, for now just always report it is fine
  return 0;
}

static int __rt_check_fc_fll_freq(unsigned int freq)
{
  // TODO we should check if the freq is allowed, for now just always report it is fine
  return 0;
}

static int gcd(int m, int n)
{
  int r;

  if((m == 0) || (n == 0))
    return 0;
  else if((m < 0) || (n < 0))
    return -1;

  do
  {
    r = m % n;
    if(r == 0)
      break;
    m = n;
    n = r;
  }
  while(1);

  return n;
}

static int lcm(int a, int b)
{
  return a / gcd(a, b) * b;
}

#if PULP_CHIP_FAMILY != CHIP_GAP
static int __rt_freq_set_periph_freq(unsigned int freq, int fll, int domain, unsigned int *out_freq)
{
  unsigned int fll_freq = lcm(freq, __rt_periph_lcm);
  int div = fll_freq / freq;

  // On other architectures, as there is a divider between fll and fc clock, check both that
  // the FC and FLL frequencies are allowed
  if (__rt_check_fc_freq(freq) || __rt_check_fc_fll_freq(fll_freq)) {
    *out_freq = fll_freq;
    return -1;
  }

  __rt_cbsys_exec(RT_CBSYS_PERIPH_SETFREQ_BEFORE);

  rt_trace(RT_TRACE_FREQ, "Updating soc FLL settings (fll: %d, fll freq: %d, fc div: %d)\n", fll, fll_freq, div);

  // As we set FLL and dividers separatly, make sure the state in between is always with
  // lower frequency by setting first the FLL or the dividers
  if (fll_freq > __rt_fll_freq_get(fll))
  {
    apb_soc_fll_clkdiv_soc_set(div);
    apb_soc_fll_clkdiv_periph_set(1);
    __rt_fll_set_freq(fll, freq);
  }
  else
  {
    __rt_fll_set_freq(fll, freq);
    apb_soc_fll_clkdiv_soc_set(div);
    apb_soc_fll_clkdiv_periph_set(1);
  }

  if (domain != -1) {
    int fc_freq = freq;
    if (div)
      fc_freq = freq / div;
    __rt_freq_domains[domain] = fc_freq;
  }

  __rt_cbsys_exec(RT_CBSYS_PERIPH_SETFREQ_AFTER);

  return 0;
}

#endif

int rt_freq_set_and_get(rt_freq_domain_e domain, unsigned int freq, unsigned int *out_freq)
{
  int irq = rt_irq_disable();
  int err = 0;

  rt_trace(RT_TRACE_FREQ, "Setting domain frequency (domain: %d, freq: %d)\n", domain, freq);

#if defined(__RT_FREQ_DOMAIN_CL) && (__RT_FREQ_DOMAIN_CL < RT_FREQ_NB_DOMAIN)

  if (domain == RT_FREQ_DOMAIN_CL)
  {
    // On cluster side, this is straight forward as the fll is not shared
    __rt_fll_set_freq(__RT_FLL_CL, freq);
    __rt_freq_domains[RT_FREQ_DOMAIN_CL] = freq;
  }
  else
#endif
  {
    // On FC side, be careful to respect the constraints coming from the drivers as the FLL
    // is shared between periphs and FC

#if PULP_CHIP_FAMILY == CHIP_GAP

    unsigned int lcm_val;
    if (__rt_periph_lcm == 0)
    {
      lcm_val = freq;
    }
    else
    {
      lcm_val = lcm(freq, __rt_periph_lcm);
    }

    // On gap, as there is no divider between fll and fc clock, the fc frequency must
    // be exactly the lcm. ALso check that the required frequency is allowed.
    if (freq != lcm_val || __rt_check_fc_freq(freq)) {
      // If the required frequency is not good, just report the possible frequency
      if (out_freq) *out_freq = lcm_val;
      err = -1;
      goto end;
    }

    __rt_cbsys_exec(RT_CBSYS_PERIPH_SETFREQ_BEFORE);

    __rt_fll_set_freq(0, freq);

    __rt_freq_domains[RT_FREQ_DOMAIN_FC] = freq;

    __rt_cbsys_exec(RT_CBSYS_PERIPH_SETFREQ_AFTER);

#else

#if PULP_CHIP == CHIP_QUENTIN

    __rt_fll_set_freq(domain, freq);

    __rt_freq_domains[domain] = freq;

    goto end;

#else

    if (__rt_freq_set_periph_freq(freq, __RT_FLL_FC, RT_FREQ_DOMAIN_FC, out_freq)) {
      err = -1;
      goto end;
    }

#endif

#endif

  }

end:
  rt_irq_restore(irq);

  return err;
}

int __rt_freq_set_constraint_multiple(unsigned int freq)
{

  // Insert the constraint into the list of active constraints
  rt_freq_constraint_t *constraint = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_freq_constraint_t));
  if (constraint == NULL) return -1;

  constraint->freq = freq;
  constraint->next = __rt_freq_first_constraint;
  __rt_freq_first_constraint = constraint;

  // Each time a constraint is added, compute the new gcd of all constraints
  // so that we can use it to compute the common frequency between periph
  // and soc when the soc frequency is changed
  if (__rt_periph_lcm == 0) {
    __rt_periph_lcm = freq;
    __rt_periph_gcd = freq;
  }
  else
  {
    __rt_periph_gcd = gcd(__rt_periph_gcd, freq);
    __rt_periph_lcm = __rt_periph_lcm / __rt_periph_gcd * freq;
  }

  // TODO we must check if the current frequency fits
  unsigned int periph_freq = __rt_freq_periph_get();
  unsigned int current_lcm = lcm(__rt_periph_lcm, periph_freq);

  if (current_lcm > periph_freq)
  {
    unsigned int div = periph_freq / __rt_periph_gcd;
    periph_freq = __rt_periph_gcd * div;

#if PULP_CHIP_FAMILY == CHIP_GAP
    rt_freq_set(RT_FREQ_DOMAIN_PERIPH, periph_freq);
#else

#if PULP_CHIP == CHIP_QUENTIN

    rt_freq_set(RT_FREQ_DOMAIN_PERIPH, periph_freq);

#else

    if (__rt_freq_set_periph_freq(periph_freq, __RT_FLL_PERIPH, -1, NULL)) {
      return -1;
    }

#endif

#endif

  }

  return 0;
}

void __rt_freq_remove_constraint_multiple(unsigned int freq)
{
  // Find the constraint in the list and remove it
  rt_freq_constraint_t *constraint = __rt_freq_first_constraint;
  rt_freq_constraint_t *prev = NULL;
  while (constraint && constraint->freq != freq) {
    prev = constraint;
    constraint = constraint->next;
  }
  if (prev) prev->next = constraint->next;
  else __rt_freq_first_constraint = constraint->next;

  rt_free(RT_ALLOC_FC_DATA, (void *)constraint, sizeof(rt_freq_constraint_t));

  // As we removed one constraint we must recompute the lcm of all constraints
  constraint = __rt_freq_first_constraint;
  __rt_periph_lcm = 0;
  while (constraint) {
    if (__rt_periph_lcm == 0) __rt_periph_lcm = freq;
    else __rt_periph_lcm = lcm(__rt_periph_lcm, freq);
    constraint = constraint->next;
  }
}

void __rt_freq_init()
{
  __rt_flls_constructor();

#if PULP_CHIP == CHIP_KERBIN

  __rt_freq_domains[RT_FREQ_DOMAIN_CL] = __rt_fll_init(__RT_FLL_CL);

#else

  // On all chips only FLL 0 is initialized here as it is shared between periph and soc
  // while FLL 1 is used for cluster only and thus is initialized when the cluster is set on.
  if (rt_platform() != ARCHI_PLATFORM_FPGA)
  {
    __rt_freq_domains[RT_FREQ_DOMAIN_FC] = __rt_fll_init(__RT_FLL_FC);
  }
  else
  {
    __rt_freq_domains[RT_FREQ_DOMAIN_FC] = 40000000;
  }

#if defined(__RT_FREQ_DOMAIN_CL) && __RT_FREQ_DOMAIN_CL < RT_FREQ_NB_DOMAIN
  __rt_freq_domains[RT_FREQ_DOMAIN_CL] = 0;
#endif

#if PULP_CHIP == CHIP_WOLFE

  // On wolfe, there are 2 flls, we have to configure how they are connected and also
  // to specify dividers.

  // FLL 0 is still shared between periph and soc, and FLL 1 is used for cluster
  // so that we can shutdown the FLL when the cluster is off, so only FLL 0
  // is configured for now.
  apb_soc_fll_ctrl_set(1<<APB_SOC_FLL_CTRL_CLUSTER_BIT);

  // For now just set default frequencies for everyone with no div
  apb_soc_fll_clkdiv_cluster_set(1);
  apb_soc_fll_clkdiv_periph_set(1);
  apb_soc_fll_clkdiv_soc_set(1);

#elif PULP_CHIP_FAMILY == CHIP_GAP

  // On GAP don't do anything as the periph fll is the FC one

#else

  // On other chips there is an fll dedicated to periphs
  __rt_freq_domains[RT_FREQ_DOMAIN_PERIPH] = __rt_fll_init(__RT_FLL_PERIPH);

#endif

#endif

}
