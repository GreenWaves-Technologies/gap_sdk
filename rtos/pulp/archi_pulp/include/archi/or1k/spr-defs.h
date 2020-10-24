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

#ifndef _ARCHI_OR1K_SPR_DEFS_H
#define _ARCHI_OR1K_SPR_DEFS_H

/* Definition of special-purpose registers (SPRs). */
#define MAX_GRPS (32)
#define MAX_SPRS_PER_GRP_BITS (11)
#define MAX_SPRS_PER_GRP (1 << MAX_SPRS_PER_GRP_BITS)
#define MAX_SPRS (0x10000)

/* Base addresses for the groups */
#define SPRGROUP_SYS	 (0<< MAX_SPRS_PER_GRP_BITS)
#define SPRGROUP_PC     (7<< MAX_SPRS_PER_GRP_BITS)

/* System control and status group */
#define SPR_SR         (SPRGROUP_SYS + 17)
#define SPR_EPCR_BASE  (SPRGROUP_SYS + 32)

/* PULP registers */
#define SPR_CORE_ID    (SPRGROUP_SYS + 0x680)
#define SPR_CLUSTER_ID (SPRGROUP_SYS + 0x681)

/* Performance counters group */
#define SPR_PCCR(N)     (SPRGROUP_PC + (N))
#define SPR_PCER        (SPRGROUP_PC + 32)
#define SPR_PCMR        (SPRGROUP_PC + 33)


/*
 * Bit definitions for the Supervision Register
 *
 */


#define SPR_SR_IEE         0x00000004  /* Interrupt Exception Enable */

#define SPR_PCER_CYCLES         0  /* Count the number of cycles the core was running */
#define SPR_PCER_INSTR          1  /* Count the number of instructions executed */
#define SPR_PCER_LD_STALL               2  /* Number of load data hazards */
#define SPR_PCER_JMP_STALL              3  /* Number of jump register data hazards */
#define SPR_PCER_IMISS          4  /* Cycles waiting for instruction fetches. i.e. the number of instructions wasted due to non-ideal caches */
#define SPR_PCER_WBRANCH                5  /* Number of wrong predicted branches */
#define SPR_PCER_WBRANCH_CYC    6  /* Cycles wasted due to wrong predicted branches */
#define SPR_PCER_LD             7  /* Number of memory loads executed. Misaligned accesses are counted twice */
#define SPR_PCER_ST             8  /* Number of memory stores executed. Misaligned accesses are counted twice */
#define SPR_PCER_JUMP           9  /* Number of jump instructions seen, i.e. j, jr, jal, jalr */
#define SPR_PCER_BRANCH         10  /* Number of jump instructions seen, i.e. bf, bnf */
#define SPR_PCER_DELAY_NOP              11  /* Number of empty delay slots, i.e. delay slots filled with a nop */
#define SPR_PCER_LD_EXT         12  /* Number of memory loads to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external */
#define SPR_PCER_ST_EXT         13  /* Number of memory stores to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external */
#define SPR_PCER_LD_EXT_CYC     14  /* Cycles used for memory loads to EXT. Every non-TCDM access is considered external */
#define SPR_PCER_ST_EXT_CYC     15  /* Cycles used for memory stores to EXT. Every non-TCDM access is considered external */
#define SPR_PCER_TCDM_CONT      16  /* Cycles wasted due to TCDM/log-interconnect contention */

#define CSR_PCER_NB_EVENTS      17
#define SPR_PCER_NB_EVENTS      17
#define CSR_PCER_NB_INTERNAL_EVENTS    12

#define CSR_NB_PCCR             31

// Gives from the event ID, the HW mask that can be stored (with an OR with other events mask) to the PCER
#define SPR_PCER_EVENT_MASK(eventId)    (1<<(eventId))
#define SPR_PCER_ALL_EVENTS_MASK  0xffffffff

#define SPR_PCMR_ACTIVE           0x1 /* Activate counting */
#define SPR_PCMR_SATURATE         0x2 /* Activate saturation */




#endif	/* SPR_DEFS__H */
