/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __ARCHI_FLL_FLL_V1_H__
#define __ARCHI_FLL_FLL_V1_H__

#define HAL_FLL_STATUS_OFFSET        0
#define HAL_FLL_CONF1_OFFSET         4
#define HAL_FLL_CONF2_OFFSET         8
#define HAL_FLL_INTEGRATOR_OFFSET    0xC

#define HAL_FLL_STATUS_FACTOR_BIT    0
#define HAL_FLL_STATUS_FACTOR_WIDTH  16

#define HAL_FLL_CONF1_MODE_BIT       31
#define HAL_FLL_CONF1_MODE_WIDTH     1
#define HAL_FLL_CONF1_LOCK_BIT       30
#define HAL_FLL_CONF1_LOCK_WIDTH     1
#define HAL_FLL_CONF1_DIV_BIT        26
#define HAL_FLL_CONF1_DIV_WIDTH      4
#define HAL_FLL_CONF1_DCO_BIT        16
#define HAL_FLL_CONF1_DCO_WIDTH      10
#define HAL_FLL_CONF1_FACTOR_BIT     0
#define HAL_FLL_CONF1_FACTOR_WIDTH   16


#define HAL_FLL_CONF2_DITH_BIT           31
#define HAL_FLL_CONF2_DITH_WIDTH         1

#define HAL_FLL_CONF2_OPEN_BIT           30
#define HAL_FLL_CONF2_OPEN_WIDTH         1

#define HAL_FLL_CONF2_CLKSEL_BIT         29
#define HAL_FLL_CONF2_CLKSEL_WIDTH       1

#define HAL_FLL_CONF2_LOCK_TOL_BIT       16
#define HAL_FLL_CONF2_LOCK_TOL_WIDTH     12

#define HAL_FLL_CONF2_NB_STABLE_BIT      10
#define HAL_FLL_CONF2_NB_STABLE_WIDTH    6

#define HAL_FLL_CONF2_NB_UNSTABLE_BIT    4
#define HAL_FLL_CONF2_NB_UNSTABLE_WIDTH  6 

#define HAL_FLL_CONF2_GAIN_BIT           0
#define HAL_FLL_CONF2_GAIN_WIDTH         4



#define HAL_FLL_INT_STATE_INT_BIT         16
#define HAL_FLL_INT_STATE_INT_WIDTH       10  

#define HAL_FLL_INT_STATE_FRACT_BIT       6  
#define HAL_FLL_INT_STATE_FRACT_WIDTH     10  

#define HAL_FLL_AREA_SIZE                 0x10


/* Fll ressources */
#define FLL_SOC_OFFSET                  	0
#define FLL_CLUSTER_OFFSET              	0x10
#define FLL_OFFSET_SOC_CLUSTER          	(FLL_CLUSTER_OFFSET - FLL_SOC_OFFSET)
#define FLL_CONF_OFFSET_REG2REG         	0x4

#define FLL_STATUS_REG_OFFSET           	0
#define FLL_CONF_REG1_OFFSET            	0x4
#define FLL_CONF_REG2_OFFSET            	0x8
#define FLL_INTEGRATOR_REG_OFFSET       	0xC

#define N_FLL     2

#define FLL_REF_CLOCK  32768


/* Maximum Log2(DCO Frequency) */
#define FLL_LOG2_MAXDCO     29
/* Maximum Log2(Clok Divider) */
#define FLL_LOG2_MAXDIV     15
/* Maximum Log2(Multiplier) */
#define FLL_LOG2_MAXM       (FLL_LOG2_MAXDCO - ARCHI_REF_CLOCK_LOG2)


#ifndef LANGUAGE_ASSEMBLY

typedef enum {
  FLL_SOC=0,
  FLL_CLUSTER=1,
} hal_fll_e;

typedef union {
  struct {
    unsigned int mult_factor:16;     /* Fll current multiplication factor */
    unsigned int pad:16;
  };
  unsigned int raw;
} hal_fll_status_t;

typedef union {
  struct {
    unsigned int mult_factor:16;      /* Fll requested multiplication factor, reset: 0x5f5.
                                         If RefClk=32768 and Div=2 Freq= 24.98 MHz */
    unsigned int DCO_input:10;        /* DCO input code for stand alone mode, reset: 0x121 */
    unsigned int clock_out_divider:4; /* Fll clock output divider, reset: 0x1 e.g div 2 */
    unsigned int output_lock_enable:1;/* Fll output gated by lock signal (active high), reset 1 */
    unsigned int mode:1;              /* Fll mode. 0: stand alone (unlocked), 1: normal, reset 0 */
  };
  unsigned int raw;
} hal_fll_reg1_t;

typedef union {
  struct {
    unsigned int loop_gain:4;         /* Fll loop gain, reset: 0x9 */
    unsigned int de_assert_cycles:6;  /* Normal mode: number of refclock unstable cycles till lock de-assert
                                         Standalone mode: lower 6 bits of lock assert counter. Reset: 0x10 */
    unsigned int assert_cycles:6;     /* Normal mode: number of refclock stable cycles till lock assert
                                         Standalone mode: upper 6 bits of lock assert counter. Reset: 0x10 */
    unsigned int lock_tolerance:12;   /* Lock tolerance: margin arounf the target mult factor where clock is
                                         considered as stable. Reset: 0x200
                                         With Fmax=250Mhw (Div=2^4), Fmin=32K (Div=2^15)
                                         Tolerance: 32K*(512/16)=1.048MHz .. 512 Hz */
    unsigned int pad:1;
    unsigned int config_clock_sel:1;  /* Select ref clock when mode = standalone, 0:RefClock, 1: DcoOut. Reset:1 */
    unsigned int open_loop:1;         /* When 1 clock operates in open loop when locked */
    unsigned int dithering:1;         /* When 1 Dithering is enabled */
  };
  unsigned int raw;
} hal_fll_reg2_t;

typedef union {
  struct {
    unsigned int pad1:6;
    unsigned int state_fract_part:10; /* Integrator state: fractional part (dithering input unit) */
    unsigned int state_int_part:10;   /* Integratot state: integer part (DCO input bits) */
    unsigned int pad2:6;
  };
  unsigned int raw;
} hal_fll_integrator_t;

#endif

#endif