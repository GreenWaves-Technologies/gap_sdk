
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_SOC_EU_STRUCTS__
#define __ARCHI_SOC_EU_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


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
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_0_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_1_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_2_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_3_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_4_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_5_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_6_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_fc_mask_7_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_0_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_1_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_2_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_3_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_4_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_5_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_6_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_cl_mask_7_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_0_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_1_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_2_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_3_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_4_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_5_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_6_t;

typedef union {
  struct {
    unsigned int mask            :32; // Event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_pr_mask_7_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_0_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_1_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_2_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_3_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_4_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_5_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_6_t;

typedef union {
  struct {
    unsigned int mask            :32; // Report MSB queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_err_7_t;

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
    unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
    unsigned int padding0:23;
    unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
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
    unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
    unsigned int padding0:23;
    unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
  };
  unsigned int raw;
} __attribute__((packed)) soc_eu_timer2_sel_lo_t;

#endif

#endif
