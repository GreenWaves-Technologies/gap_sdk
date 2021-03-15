/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#ifndef __CLUSTER_FLL_OPTIMAL_SETTINGS__
#define __CLUSTER_FLL_OPTIMAL_SETTINGS__
#include "rt/rt_api.h"

typedef struct {
        unsigned int MultFactor:16;     /* Fll current multiplication factor */
        unsigned int Pad:16;
} FllStatusT;

typedef struct {
        unsigned int MultFactor:16;     /* Fll requested multiplication factor, reset: 0x5f5.
                                           If RefClk=32768 and Div=2 Freq= 24.98 MHz */
        unsigned int DCOInput:10;       /* DCO input code for stand alone mode, reset: 0x121 */
        unsigned int ClockOutDivider:4; /* Fll clock output divider, reset: 0x1 e.g div 2 */
        unsigned int OutputLockEnable:1;/* Fll output gated by lock signal (active high), reset 1 */
        unsigned int Mode:1;            /* Fll mode. 0: stand alone (unlocked), 1: normal, reset 0 */
} FllConfiguration1T;

typedef struct {
        unsigned int LoopGain:4;        /* Fll loop gain, reset: 0x9 */
        unsigned int DeAssertCycles:6;  /* Normal mode: number of refclock unstable cycles till lock de-assert
                                           Standalone mode: lower 6 bits of lock assert counter. Reset: 0x10 */
        unsigned int AssertCycles:6;    /* Normal mode: number of refclock stable cycles till lock assert
                                           Standalone mode: upper 6 bits of lock assert counter. Reset: 0x10 */
        unsigned int LockTolerance:12;  /* Lock tolerance: margin arounf the target mult factor where clock is
                                           considered as stable. Reset: 0x200
                                           With Fmax=250Mhw (Div=2^4), Fmin=32K (Div=2^15)
                                           Tolerance: 32K*(512/16)=1.048MHz .. 512 Hz */
        unsigned int Pad:1;
        unsigned int ConfigClockSel:1;  /* Select ref clock when mode = standalone, 0:RefClock, 1: DcoOut. Reset:1 */
        unsigned int OpenLoop:1;        /* When 1 clock operates in open loop when locked */
        unsigned int Dithering:1;       /* When 1 Dithering is enabled */
} FllConfiguration2T;

typedef struct {
        unsigned int Pad1:6;
        unsigned int StateFractPart:10; /* Integrator state: fractional part (dithering input unit) */
        unsigned int StateIntPart:10;   /* Integratot state: integer part (DCO input bits) */
        unsigned int Pad2:6;
} FllIntegratorT;

typedef union {
        FllConfiguration1T      ConfigReg1;
        FllConfiguration2T      ConfigReg2;
        FllIntegratorT          Integrator;
        unsigned int            Raw;
} FllConfigT;

void __attribute__ ((noinline)) SetFllConfig2(hal_fll_e WhichFll, unsigned int Assert, unsigned int Deassert, unsigned int Tolerance, unsigned int Gain, unsigned int Dithering, unsigned int OpenLoop);
#endif
