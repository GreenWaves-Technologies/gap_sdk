
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

#ifndef __ARCHI_RISCV_DBG_DM_REGMAP__
#define __ARCHI_RISCV_DBG_DM_REGMAP__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS GLOBAL STRUCT
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

/** RISCV_DBG_DM_Type Register Layout Typedef */
typedef struct {
    volatile uint32_t data0;  // Abstract Data 0
    volatile uint32_t data1;  // Abstract Data 1
    volatile uint32_t reserved_0[1];  // Reserved/Not used.
    volatile uint32_t dmcontrol;  // Debug Module Control
    volatile uint32_t dmstatus;  // Debug Module Status
    volatile uint32_t hartinfo;  // Hart Info
    volatile uint32_t abstractcs;  // Abstract Control and Status
    volatile uint32_t command;  // Abstract Command
    volatile uint32_t reserved_1[7];  // Reserved/Not used.
    volatile uint32_t sbcs;  // System Bus Access Control and Status
    volatile uint32_t progbuf0;  // Program Buffer 0
    volatile uint32_t progbuf1;  // Program Buffer 1
    volatile uint32_t progbuf2;  // Program Buffer 2
    volatile uint32_t progbuf3;  // Program Buffer 3
    volatile uint32_t progbuf4;  // Program Buffer 4
    volatile uint32_t progbuf5;  // Program Buffer 5
    volatile uint32_t progbuf6;  // Program Buffer 6
    volatile uint32_t progbuf7;  // Program Buffer 7
    volatile uint32_t reserved_2[3];  // Reserved/Not used.
    volatile uint32_t sbaddress0;  // System Bus Address 31:0
    volatile uint32_t dmcs2;  // Debug Module Control and Status 2
    volatile uint32_t reserved_3[1];  // Reserved/Not used.
    volatile uint32_t sbdata0;  // System Bus data 31:0
} __attribute__((packed)) riscv_dbg_dm_t;
/** RISCV_DBG_DM_Type Register Layout Typedef */
typedef struct {

    volatile riscv_dbg_dm_data0_t data0;  // Abstract Data 0
    volatile riscv_dbg_dm_data1_t data1;  // Abstract Data 1
    volatile uint32_t reserved_0[1];  // Reserved/Not used.
    volatile riscv_dbg_dm_dmcontrol_t dmcontrol;  // Debug Module Control
    volatile riscv_dbg_dm_dmstatus_t dmstatus;  // Debug Module Status
    volatile riscv_dbg_dm_hartinfo_t hartinfo;  // Hart Info
    volatile riscv_dbg_dm_abstractcs_t abstractcs;  // Abstract Control and Status
    volatile riscv_dbg_dm_command_t command;  // Abstract Command
    volatile uint32_t reserved_1[7];  // Reserved/Not used.
    volatile riscv_dbg_dm_sbcs_t sbcs;  // System Bus Access Control and Status
    volatile riscv_dbg_dm_progbuf0_t progbuf0;  // Program Buffer 0
    volatile riscv_dbg_dm_progbuf1_t progbuf1;  // Program Buffer 1
    volatile riscv_dbg_dm_progbuf2_t progbuf2;  // Program Buffer 2
    volatile riscv_dbg_dm_progbuf3_t progbuf3;  // Program Buffer 3
    volatile riscv_dbg_dm_progbuf4_t progbuf4;  // Program Buffer 4
    volatile riscv_dbg_dm_progbuf5_t progbuf5;  // Program Buffer 5
    volatile riscv_dbg_dm_progbuf6_t progbuf6;  // Program Buffer 6
    volatile riscv_dbg_dm_progbuf7_t progbuf7;  // Program Buffer 7
    volatile uint32_t reserved_2[3];  // Reserved/Not used.
    volatile riscv_dbg_dm_sbaddress0_t sbaddress0;  // System Bus Address 31:0
    volatile riscv_dbg_dm_dmcs2_t dmcs2;  // Debug Module Control and Status 2
    volatile uint32_t reserved_3[1];  // Reserved/Not used.
    volatile riscv_dbg_dm_sbdata0_t sbdata0;  // System Bus data 31:0
} __attribute__((packed)) riscv_dbg_dm_struct_t;

#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */

#endif
