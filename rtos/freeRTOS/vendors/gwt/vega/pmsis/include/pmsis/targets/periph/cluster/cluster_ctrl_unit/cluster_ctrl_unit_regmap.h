
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_REGMAP__
#define __ARCHI_CLUSTER_CTRL_UNIT_REGMAP__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

/** CLUSTER_CTRL_UNIT_Type Register Layout Typedef */
typedef struct {
    volatile uint32_t eoc;  // End Of Computation status register.
    volatile uint32_t reserved_0[1];  // Reserved/Not used.
    volatile uint32_t fetch_en;  // Cluster cores fetch enable configuration register.
    volatile uint32_t reserved_1[5];  // Reserved/Not used.
    volatile uint32_t clock_gate;  // Cluster clock gate configuration register.
    volatile uint32_t reserved_2[1];  // Reserved/Not used.
    volatile uint32_t dbg_resume;  // Cluster cores debug resume register.
    volatile uint32_t dbg_halt_status;  // Cluster cores debug halt status register.
    volatile uint32_t reserved_3[3];  // Reserved/Not used.
    volatile uint32_t dbg_halt_mask;  // Cluster cores debug halt mask configuration register.
    volatile uint32_t reserved_4[1];  // Reserved/Not used.
    volatile uint32_t boot_addr0;  // Cluster core 0 boot address configuration register.
    volatile uint32_t boot_addr1;  // Cluster core 1 boot address configuration register.
    volatile uint32_t boot_addr2;  // Cluster core 2 boot address configuration register.
    volatile uint32_t boot_addr3;  // Cluster core 3 boot address configuration register.
    volatile uint32_t boot_addr4;  // Cluster core 4 boot address configuration register.
    volatile uint32_t boot_addr5;  // Cluster core 5 boot address configuration register.
    volatile uint32_t boot_addr6;  // Cluster core 6 boot address configuration register.
    volatile uint32_t boot_addr7;  // Cluster core 7 boot address configuration register.
    volatile uint32_t reserved_5[8];  // Reserved/Not used.
    volatile uint32_t tcdm_arb_policy_ch0;  // TCDM arbitration policy ch0 for cluster cores configuration register.
    volatile uint32_t reserved_6[1];  // Reserved/Not used.
    volatile uint32_t tcdm_arb_policy_ch1;  // TCDM arbitration policy ch1 for DMA/HWCE configuration register.
    volatile uint32_t reserved_7[13];  // Reserved/Not used.
    volatile uint32_t tcdm_arb_policy_ch0_rep;  // Read only duplicate of TCDM_ARB_POLICY_CH0 register
    volatile uint32_t reserved_8[1];  // Reserved/Not used.
    volatile uint32_t tcdm_arb_policy_ch1_rep;  // Read only duplicate of TCDM_ARB_POLICY_CH1 register
} __attribute__((packed)) cluster_ctrl_unit_t;

#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */

#endif
