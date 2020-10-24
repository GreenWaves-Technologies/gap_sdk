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

#ifndef __ARCHI_CHIPS_VIVOSOC3_1_FLL_H__
#define __ARCHI_CHIPS_VIVOSOC3_1_FLL_H__

// register address offsets
#define FLL_CONTROL_OFFSET                  0x044 // base addr 0x1A104000 (ARCHI_APB_SOC_CTRL_ADDR)  
#define FLL_RESET_OFFSET                    0x048
#define FLL_GPIO_OUTPUT                     0x104

#define FLL_OFFSET                          0x040 // base addr 0x1A100000 (ARCHI_SOC_PERIPHERALS_ADDR)

#define FLL_SET_SEL_OFFSET                  0x000 // base addr 0x1A100000 + fll * FLL_OFFSET
#define FLL_REF_CLK_OFFSET                  0x004
#define FLL_FSM_OBS_OFFSET                  0x008
#define FLL_SHARED_CONFIG_OFFSET            0x00C
#define FLL_SET_OFFSET                      0x00C
#define FLL_FREQ_REGULATION_OFFSET          0x010 // + set * FLL_SET_OFFSET
#define FLL_FREQ_CLK_GOOD_OFFSET            0x014 // + set * FLL_SET_OFFSET
#define FLL_FREQ_CONFIG_OFFSET              0x018 // + set * FLL_SET_OFFSET
#define FLL_OBSERVATION_OFFSET              0x100 

#define FLL_ANALOG_CLKIPC_OFFSET            0x4638 // base addr ARCHI_ANALOG_ADDR

// register bitfields
#define FLL_SET_RSS_BIT                     0
#define FLL_SET_RSS_WIDTH                   2
#define FLL_SET_RSS_MASK                    (0x0003)


#define FLL_FREQ_REGULATION_LOW_BIT         0
#define FLL_FREQ_REGULATION_LOW_WIDTH       14
#define FLL_FREQ_REGULATION_LOW_MASK        (0x00003FFF)

#define FLL_FREQ_REGULATION_HIGH_BIT        16
#define FLL_FREQ_REGULATION_HIGH_WIDTH      14
#define FLL_FREQ_REGULATION_HIGH_MASK       (0x3FFF0000)


#define FLL_FREQ_CLK_GOOD_LOW_BIT           0
#define FLL_FREQ_CLK_GOOD_LOW_WIDTH         14
#define FLL_FREQ_CLK_GOOD_LOW_MASK          (0x00003FFF)

#define FLL_FREQ_CLK_GOOD_HIGH_BIT          16
#define FLL_FREQ_CLK_GOOD_HIGH_WIDTH        14
#define FLL_FREQ_CLK_GOOD_HIGH_MASK         (0x3FFF0000)


#define FLL_FREQ_CONFIG_LEVEL_BIT           0
#define FLL_FREQ_CONFIG_LEVEL_WIDTH         4
#define FLL_FREQ_CONFIG_LEVEL_MASK          (0x0000000F)

#define FLL_FREQ_CONFIG_DCO_LSB_BIT         4
#define FLL_FREQ_CONFIG_DCO_LSB_WIDTH       3
#define FLL_FREQ_CONFIG_DCO_LSB_MASK        (0x00000070)

#define FLL_FREQ_CONFIG_DCO_MSB_BIT         8
#define FLL_FREQ_CONFIG_DCO_MSB_WIDTH       6
#define FLL_FREQ_CONFIG_DCO_MSB_MASK        (0x00003F00)

#define FLL_FREQ_CONFIG_OL_BIT              16
#define FLL_FREQ_CONFIG_OL_WIDTH            1
#define FLL_FREQ_CONFIG_OL_MASK             (0x00010000)


#ifndef LANGUAGE_ASSEMBLY

typedef union {
  struct {
    unsigned int rss:1;       /* Register set selection, 0b00 or 0b01 */
    unsigned int reserved_0:31;
  };
  unsigned int raw;
} fll_reg_rss_t;

typedef union {
  struct {
    unsigned int nref_reg_low:14;      /* Regulation, lower boundary, in ref clk cycles */
    unsigned int reserved_0:2;     
    unsigned int nref_reg_high:14;      /* Regulation, upper boundary, in ref clk cycles */
    unsigned int reserved_1:2;       
  };
  unsigned int raw;
} fll_reg_fcr0_t;

typedef union {
  struct {
    unsigned int nref_cg_low:14;      /* Clock good, lower boundary, in ref clk cycles */
    unsigned int reserved_0:2;     
    unsigned int nref_cg_high:14;     /* Clock good, upper boundary, in ref clk cycles */
    unsigned int reserved_1:2;       
  };
  unsigned int raw;
} fll_reg_fcr1_t;

typedef union {
  struct {
    unsigned int level:4;             /* Start level of binary search, reset value: 0x0 */
    unsigned int dco_lsb:3;           /* Start point for the binary search, reset value: 0x0 */
    unsigned int reserved_0:1;
    unsigned int dco_msb:6;           /* Start point for the binary search, reset value: 0x0 */
    unsigned int reserved_1:2;      
    unsigned int open_loop:1;         /* Open-loop operation */
    unsigned int reserved_2:15; 
  };
  unsigned int raw;
} fll_reg_fcr2_t;

#endif

#endif