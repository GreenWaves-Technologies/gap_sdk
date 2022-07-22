
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_STRUCTS__
#define __ARCHI_CLUSTER_CTRL_UNIT_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int eoc             :1 ; // End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_eoc_t;

typedef union {
  struct {
    unsigned int core0           :1 ; // Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core1           :1 ; // Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core2           :1 ; // Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core3           :1 ; // Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core4           :1 ; // Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core5           :1 ; // Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core6           :1 ; // Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
    unsigned int core7           :1 ; // Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_fetch_en_t;

typedef union {
  struct {
    unsigned int en              :1 ; // Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_clock_gate_t;

typedef union {
  struct {
    unsigned int core0           :1 ; // Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0
    unsigned int core1           :1 ; // Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1
    unsigned int core2           :1 ; // Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2
    unsigned int core3           :1 ; // Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3
    unsigned int core4           :1 ; // Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4
    unsigned int core5           :1 ; // Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5
    unsigned int core6           :1 ; // Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6
    unsigned int core7           :1 ; // Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_dbg_resume_t;

typedef union {
  struct {
    unsigned int core0           :1 ; // Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core1           :1 ; // Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core2           :1 ; // Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core3           :1 ; // Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core4           :1 ; // Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core5           :1 ; // Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core6           :1 ; // Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
    unsigned int core7           :1 ; // Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_dbg_halt_status_t;

typedef union {
  struct {
    unsigned int core0           :1 ; // Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core1           :1 ; // Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core2           :1 ; // Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core3           :1 ; // Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core4           :1 ; // Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core5           :1 ; // Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core6           :1 ; // Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
    unsigned int core7           :1 ; // Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_dbg_halt_mask_t;

typedef union {
  struct {
    unsigned int ba              :32; // Cluster core 0 boot address configuration bitfield.
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr2_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr3_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr4_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr5_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr6_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_boot_addr7_t;

typedef union {
  struct {
    unsigned int pol             :1 ; // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch0_t;

typedef union {
  struct {
    unsigned int pol             :1 ; // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch1_t;

typedef union {
  struct {
    unsigned int pol             :1 ; // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_t;

typedef union {
  struct {
    unsigned int pol             :1 ; // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
  };
  unsigned int raw;
} __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_t;

#endif

#endif
