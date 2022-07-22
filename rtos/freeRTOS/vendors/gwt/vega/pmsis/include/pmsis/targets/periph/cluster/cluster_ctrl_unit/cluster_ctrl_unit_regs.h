
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_REGS__
#define __ARCHI_CLUSTER_CTRL_UNIT_REGS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS
//

// End Of Computation status register.
#define CLUSTER_CTRL_UNIT_EOC_OFFSET             0x0

// Cluster cores fetch enable configuration register.
#define CLUSTER_CTRL_UNIT_FETCH_EN_OFFSET        0x8

// Cluster clock gate configuration register.
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_OFFSET      0x20

// Cluster cores debug resume register.
#define CLUSTER_CTRL_UNIT_DBG_RESUME_OFFSET      0x28

// Cluster cores debug halt status register.
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_OFFSET 0x28

// Cluster cores debug halt mask configuration register.
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_OFFSET   0x38

// Cluster core 0 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_OFFSET      0x40

// Cluster core 1 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR1_OFFSET      0x44

// Cluster core 2 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR2_OFFSET      0x48

// Cluster core 3 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR3_OFFSET      0x4c

// Cluster core 4 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR4_OFFSET      0x50

// Cluster core 5 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR5_OFFSET      0x54

// Cluster core 6 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR6_OFFSET      0x58

// Cluster core 7 boot address configuration register.
#define CLUSTER_CTRL_UNIT_BOOT_ADDR7_OFFSET      0x5c

// TCDM arbitration policy ch0 for cluster cores configuration register.
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_OFFSET 0x80

// TCDM arbitration policy ch1 for DMA/HWCE configuration register.
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_OFFSET 0x88

// Read only duplicate of TCDM_ARB_POLICY_CH0 register
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_OFFSET 0xc0

// Read only duplicate of TCDM_ARB_POLICY_CH1 register
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_OFFSET 0xc8

#endif
