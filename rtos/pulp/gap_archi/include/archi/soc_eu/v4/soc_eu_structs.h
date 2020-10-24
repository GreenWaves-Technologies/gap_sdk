
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_STRUCTS_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_STRUCTS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int event           :8 ; // Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_sw_event_t;

typedef union {
  struct {
    unsigned int fc_mask_msb     :32; // MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_msb_t;

typedef union {
  struct {
    unsigned int fc_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_lsb_t;

typedef union {
  struct {
    unsigned int cl_mask_msb     :32; // MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_msb_t;

typedef union {
  struct {
    unsigned int cl_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_lsb_t;

typedef union {
  struct {
    unsigned int pr_mask_msb     :32; // MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_msb_t;

typedef union {
  struct {
    unsigned int pr_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_lsb_t;

typedef union {
  struct {
    unsigned int err_msb         :32; // Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_msb_t;

typedef union {
  struct {
    unsigned int err_lsb         :32; // Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_lsb_t;

typedef union {
  struct {
    unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
    unsigned int padding0:23;
    unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_timer1_sel_hi_t;

typedef union {
  struct {
    unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.
    unsigned int padding0:23;
    unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer Low trigger event input.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_timer1_sel_lo_t;

typedef union {
  struct {
    unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
    unsigned int padding0:23;
    unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_timer2_sel_hi_t;

typedef union {
  struct {
    unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.
    unsigned int padding0:23;
    unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer Low trigger event input.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_timer2_sel_lo_t;

#endif

#endif
