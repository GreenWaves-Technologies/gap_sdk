/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *              GAPX PMSIS_HAL File (VEGA)
 *  This file contains only the Hardware Abstraction Layer of GAPX microprocessor.
 *  Aims is to unify the low level SW of chips (PULP and CMSIS), and to give programs
 *  a standard access to the hardware resources.
 *
 *      Example: read/write SPIM0 reg CMD_CFG:
 *
 *  hal_write32(&SPIM0->CMD_CFG, 0xAA);
 *  hal_read32(&SPIM0->CMD_CFG);
 *
 */

#ifndef __PMSIS_TARGETS_H__
#define __PMSIS_TARGETS_H__

#include "pmsis/targets/periph.h"
#include "cores/TARGET_RISCV_32/core_gap.h"
#include "cores/TARGET_RISCV_32/core_gap_cluster.h"
#include "cores/TARGET_RISCV_32/core_utils.h"

/**
 *  Compiler barriers: essential to make sure that read/writes are all flushed
 *  by compiler before next instructions
 **/

// make the compiler believe we have potential side effects
static inline void hal_compiler_barrier()
{
    __asm__ __volatile__ ("" : : : "memory");
}

static inline void hal_write32(volatile void *addr, uint32_t value)
{
    hal_compiler_barrier();
    *(volatile uint32_t *)(volatile uint32_t)(addr) = (uint32_t)value;
    hal_compiler_barrier();
}

static inline void hal_write8(volatile void *addr, uint32_t value)
{
    hal_compiler_barrier();
    *(volatile uint8_t*)(addr) = (uint8_t)value;
    hal_compiler_barrier();
}

static inline void hal_or32(volatile void *addr, uint32_t value)
{
    hal_compiler_barrier();
    *(volatile uint32_t*)(volatile uint32_t)(addr) |= (uint32_t)value;
    hal_compiler_barrier();
}

static inline void hal_and32(volatile void *addr, uint32_t value)
{
    hal_compiler_barrier();
    *((volatile uint32_t*)(volatile uint32_t)addr) &= value;
    hal_compiler_barrier();
}

static inline uint32_t hal_read32(volatile void *addr)
{
    hal_compiler_barrier();
    uint32_t ret = (*((volatile uint32_t*)(volatile uint32_t)addr));
    hal_compiler_barrier();
    return ret;
}

static inline uint8_t hal_read8(volatile void *addr)
{
    hal_compiler_barrier();
    uint8_t ret = (*((volatile uint8_t*)addr));
    hal_compiler_barrier();
    return ret;
}

static inline uint32_t pi_core_id()
{
    return __native_core_id();
}

static inline uint32_t pi_cluster_id()
{
    return __native_cluster_id();
}

static inline uint32_t pi_is_fc()
{
    return __native_is_fc();
}

static inline uint32_t pi_cl_cluster_nb_pe_cores(void)
{
    return __builtin_pulp_CoreCount();
}

static inline int pi_cl_cluster_nb_cores()
{
    return __builtin_pulp_CoreCount();
}

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 *  Compatibility Section
 *  For ensuring our SW could run in both pulp-os and mbed-os, we keep these
 *  part until we have our whole PMSIS done.
 */

// PULP OS
#define pulp_read32        hal_read32
#define pulp_write32       hal_write32

#define hal_itc_wait_for_event_noirq    hal_itc_wait_for_event_no_irq
#define hal_itc_wait_for_event          hal_itc_wait_for_event_no_irq

#endif  /* __PMSIS_TARGETS_H__ */
