/* 
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Eric Flamand (eric.flamand@iis.ee.ethz.ch)
 */

/* Wait for xtal stabilization.

  We first set SOC FLL to open loop mode
  Then we measure how much FLL oscillations we have between 2 edges of the ref clock (the xtal one).
  When the measured time variation goes bellow a certain threshold (defined by AllowedDelta) for long enough
  (defined by MinStablePeriod) we consider the xtal as stable.
  If we do not converge in less than MaxPeriod we give up and return an error: 1, otherwise we return 0.

  Before returning the FLL is re locked

  AllowedDelta : A Q15 fractional, variation between 2 consecutive measurement should not be higher than this percentage

  AllowedDelta:   Should come from eFUSE, 16bits is enough
  MinStablePeriod:  Should come from eFUSE, 8bits is enough
  MaxPeriod:    Should come from eFUSE, 8bits is enough

  Note that this step should be made optionnal thanks to yet another eFUSE to control it (8bits)
*/

#include "boot.h"
#include "hal/pulp.h"

static int __attribute__ ((noinline)) CheckStable(unsigned int Stable, int MinStablePeriod)

{
  return (Stable<MinStablePeriod);
}

int WaitStableXtal(int AllowedDelta, int MinStablePeriod, int MaxPeriod)

{
#define Abs(x)                  ((x<0)?-(x):(x))
  unsigned int PrevTime, Stable, ElapsedTime, KnownPeriod;
  int i;


  hal_fll_reg1_t reg1 = { .raw = hal_fll_conf_reg1_get(0) };
  /* Unlock FLL SoC */
  reg1.mode = 0;
  hal_fll_conf_reg1_set(0, reg1.raw);

  plp_timer_conf_low(1, 1, 0, 0, 0, 0, 0, 0, 0);
  PrevTime=0; Stable = 0;
  for (i=0; i<MaxPeriod; i++) {
    unsigned int CurTime;
    wait_clock_ref(1);
    CurTime = plp_timer_get_count_low();
    ElapsedTime = CurTime-PrevTime;
    PrevTime = CurTime;
    if (!Stable) {
      KnownPeriod = ElapsedTime; Stable = 1;
    } else {
      int TimeDiff = KnownPeriod - ElapsedTime;
      if (Abs(TimeDiff) <= ((AllowedDelta*KnownPeriod)>>15)) {
        Stable++;
        if (Stable>=MinStablePeriod) break;
      } else Stable = 0;
    }
  }

  return !CheckStable(Stable, MinStablePeriod);
#undef Abs
}
