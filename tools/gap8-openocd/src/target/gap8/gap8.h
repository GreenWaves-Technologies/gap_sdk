/***************************************************************************
 *   Copyright (C) 2011 by Julius Baxter                                   *
 *   julius@opencores.org                                                  *
 *                                                                         *
 *   Copyright (C) 2013 by Marek Czerski                                   *
 *   ma.czerski@gmail.com                                                  *
 *                                                                         *
 *   Copyright (C) 2013 by Franck Jullien                                  *
 *   elec4fun@gmail.com                                                    *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef OPENOCD_TARGET_GAP8_H
#define OPENOCD_TARGET_GAP8_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <target/target.h>

#ifndef offsetof
#define offsetof(st, m) __builtin_offsetof(st, m)
#endif

/*
 * Support only FC for now
 * Cluster support will add 8 cores
 * TODO: rename with _FCX_/_CLX_ suffixes
 */
#define CORE_NUM 1
#define DEBUG_UNIT0_BASE        (0x1B300000)
#define EVENT_UNIT0_BASE        (0x1B200800) // TODO/FIXME? is it alias evt unit?

#define DEBUG_GPR_OFFSET 0x400
#define DEBUG_FPR_OFFSET 0x500
#define DEBUG_REG_OFFSET 0x2000
#define DEBUG_CSR_OFFSET 0x4000

/* RV32M1 registers */
enum gap8_reg_nums {
    GAP8_REG_R0 = 0,
	GAP8_REGNO_A0 = 10,         /* First argument.  */
	GAP8_REGNO_A1 = 11,         /* Second argument.  */
	GAP8_REGNO_A2,
	GAP8_REGNO_A3,
	GAP8_REGNO_A4,
	GAP8_REGNO_A5,
	GAP8_REGNO_A6,
	GAP8_REGNO_A7,
    GAP8_REG_R31 = 31,
    GAP8_REG_NPC = 32, // read at DBG Unit + 0x2000
    GAP8_REG_PPC = 33, // read at DBG Unit + 0x2004
    GAP8_CORE_REG_NUM,
    GAP8_REG_CSR0 = 65,
    GAP8_REG_CSR4095 = 4160, // TODO/FIXME?: check CSR regs on our side
    GAP8_ALL_REG_NUM
};

#if 0
enum gap8_gdb_regno {
	GDB_REGNO_ZERO = 0,        /* Read-only register, always 0.  */
	GDB_REGNO_RA = 1,          /* Return Address.  */
	GDB_REGNO_SP = 2,          /* Stack Pointer.  */
	GDB_REGNO_GP = 3,          /* Global Pointer.  */
	GDB_REGNO_TP = 4,          /* Thread Pointer.  */
	GDB_REGNO_T0,
	GDB_REGNO_T1,
	GDB_REGNO_T2,
	GDB_REGNO_S0 = 8,
	GDB_REGNO_FP = 8,          /* Frame Pointer.  */
	GDB_REGNO_S1,
	GDB_REGNO_A0 = 10,         /* First argument.  */
	GDB_REGNO_A1 = 11,         /* Second argument.  */
	GDB_REGNO_A2,
	GDB_REGNO_A3,
	GDB_REGNO_A4,
	GDB_REGNO_A5,
	GDB_REGNO_A6,
	GDB_REGNO_A7,
	GDB_REGNO_S2,
	GDB_REGNO_S3,
	GDB_REGNO_S4,
	GDB_REGNO_S5,
	GDB_REGNO_S6,
	GDB_REGNO_S7,
	GDB_REGNO_S8,
	GDB_REGNO_S9,
	GDB_REGNO_S10,
	GDB_REGNO_S11,
	GDB_REGNO_T3,
	GDB_REGNO_T4,
	GDB_REGNO_T5,
	GDB_REGNO_T6,
	GDB_REGNO_XPR31 = GDB_REGNO_T6,
	GDB_REGNO_PC = 32,
	GDB_REGNO_CSR0 = 65,
	GDB_REGNO_TSELECT = CSR_TSELECT + GDB_REGNO_CSR0,
	GDB_REGNO_TDATA1 = CSR_TDATA1 + GDB_REGNO_CSR0,
	GDB_REGNO_TDATA2 = CSR_TDATA2 + GDB_REGNO_CSR0,
	GDB_REGNO_MISA = CSR_MISA + GDB_REGNO_CSR0,
	//GDB_REGNO_DPC = CSR_DPC + GDB_REGNO_CSR0,
	GDB_REGNO_DCSR = CSR_DCSR + GDB_REGNO_CSR0,
	GDB_REGNO_DSCRATCH = CSR_DSCRATCH + GDB_REGNO_CSR0,
	GDB_REGNO_MSTATUS = CSR_MSTATUS + GDB_REGNO_CSR0,
	GDB_REGNO_CSR4095 = GDB_REGNO_CSR0 + 4095,
	GDB_REGNO_PRIV = 4161,
	GDB_REGNO_COUNT
};
#endif


struct gap8_debug_unit {
    volatile uint32_t DBG_CTRL;      /* Offset 0x0000. */
    volatile uint32_t DBG_HIT;       /* Offset 0x0004. */
    volatile uint32_t DBG_IE;        /* Offset 0x0008. */
    volatile uint32_t DBG_CAUSE;     /* Offset 0x000c. */
    uint8_t reserved0[0x30];
    struct gap8_debug_bp {
        volatile uint32_t CTRL;
        volatile uint32_t DATA;
    } DBG_BP[8];                     /* Offset 0x0040. */
    uint8_t reserved1[0x380];
    volatile uint32_t GPR[32];       /* Offset 0x0400. */
    uint8_t reserved2[0x80];
    volatile uint32_t FPR_LSP[32];   /* Offset 0x0500. */
    volatile uint32_t FPR_MSP[32];   /* Offset 0x0580. */
    uint8_t reserved3[0x1A00];
    volatile uint32_t DBG_NPC;       /* Offset 0x2000. */
    volatile uint32_t DBG_PPC;       /* Offset 0x2004. */
    uint8_t reserved4[0x1FF8];
    volatile uint32_t CSR[4906];     /* Offset 0x4000. */
};

extern int gap_core_id;
extern uint32_t gap8_debug_unit_reg_addr[];
extern uint32_t gap8_event_unit_reg_addr[];

struct gap8_jtag {
    struct jtag_tap *tap;
    int gap8_jtag_inited;
    int gap8_jtag_module_selected;
    uint8_t *current_reg_idx;
    struct gap8_tap_ip *tap_ip;
    struct gap8_dbg_unit *du_core;
    struct target *target;
};

struct gap8_info {
    struct gap8_jtag jtag;
    uint32_t* reg_cache_mem;
    char * reg_name_mem;
};

typedef uint32_t gap8_reg_t;

static inline struct gap8_info *
target_to_gap8(struct target *target)
{
    return (struct gap8_info *)target->arch_info;
}

void gap8_semihosting_init(struct target *target);

int gap8_semihosting(struct target *target, int *retval);

/* Set a register of GAP8 FC core
 * Prototype is temporary, need to add smtgh to choose core instead
 * And need to detect which core did the call
 */
int gap8_get_register(struct target *target, gap8_reg_t *buffer, int reg_num);
int gap8_set_register(struct target *target, int reg_num, gap8_reg_t buffer);


#define NO_SINGLE_STEP     0
#define SINGLE_STEP        1

/* GAP8 Debug registers and bits needed for resuming */
#define GAP8_DEBUG_CTRL_HALT   (1<<16) /* HALT. */
#define GAP8_DEBUG_CTRL_SSTE   (1<<0)  /* SSTE, single step enable. */
#define GAP8_DEBUG_IE_ECALL (1 << 11) // Environment call from M-Mode
#define GAP8_DEBUG_IE_SAF   (1 << 7 ) // Store Access Fault (together with LAF)
#define GAP8_DEBUG_IE_SAM   (1 << 6 ) // Store Address Misaligned (never traps)
#define GAP8_DEBUG_IE_LAF   (1 << 5 ) // Load Access Fault (together with SAF)
#define GAP8_DEBUG_IE_LAM   (1 << 4 ) // Load Address Misaligned (never traps)
#define GAP8_DEBUG_IE_BP    (1 << 3 ) // EBREAK instruction causes trap
#define GAP8_DEBUG_IE_ILL   (1 << 2 ) // Illegal Instruction
#define GAP8_DEBUG_IE_IAF   (1 << 1 ) // Instruction Access Fault (not implemented)
#define GAP8_DEBUG_IE_IAM   (1 << 0 ) // Instruction Address Misaligned (never traps)

/* GAP8 DEBUG cause. */
#define GAP8_DEBUG_CAUSE_ECALL (11) // Environment call from M-Mode
#define GAP8_DEBUG_CAUSE_SAF   (7 ) // Store Access Fault (together with LAF)
#define GAP8_DEBUG_CAUSE_SAM   (6 ) // Store Address Misaligned (never traps)
#define GAP8_DEBUG_CAUSE_LAF   (5 ) // Load Access Fault (together with SAF)
#define GAP8_DEBUG_CAUSE_LAM   (4 ) // Load Address Misaligned (never traps)
#define GAP8_DEBUG_CAUSE_BP    (3 ) // EBREAK instruction causes trap
#define GAP8_DEBUG_CAUSE_ILL   (2 ) // Illegal Instruction
#define GAP8_DEBUG_CAUSE_IAF   (1 ) // Instruction Access Fault (not implemented)
#define GAP8_DEBUG_CAUSE_IAM   (0 ) // Instruction Address Misaligned (never traps)

#define GAP8_DEBUG_CAUSE_MASK  (0x7FFFFFFFU)

#define GAP8_DEBUG_REG_ADDR(gap_core_id, reg) ((uint32_t)(gap8_debug_unit_reg_addr[gap_core_id] + \
            offsetof(struct gap8_debug_unit, reg)))

#endif /* OPENOCD_TARGET_GAP8_H */
