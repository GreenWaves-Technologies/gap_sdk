/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __CPU_ISS_ISS_CORE_HPP
#define __CPU_ISS_ISS_CORE_HPP

#include "types.hpp"

static inline void iss_exec_insn_stall(iss_t *iss);
static inline void iss_exec_insn_resume(iss_t *iss);
static inline void iss_exec_insn_terminate(iss_t *iss);

#include "utils.hpp"
#include "iss_api.hpp"
#include "platform_wrapper.hpp"
#include "regs.hpp"
#include "perf.hpp"
#include "lsu.hpp"
#include "prefetcher.hpp"
#include "insn_cache.hpp"
#include "irq.hpp"
#include "exceptions.hpp"
#include "exec.hpp"


int iss_open(iss_t *iss);
void iss_reset(iss_t *iss, int active);
void iss_start(iss_t *iss);

void iss_register_debug_info(iss_t *iss, const char *binary);

void iss_pc_set(iss_t *iss, iss_addr_t value);

iss_insn_t *iss_decode_pc(iss_t *cpu, iss_insn_t *pc);
iss_insn_t *iss_decode_pc_noexec(iss_t *cpu, iss_insn_t *pc);
void iss_decode_activate_isa(iss_t *cpu, char *isa);





void iss_csr_init(iss_t *iss, int active);
bool iss_csr_read(iss_t *iss, iss_reg_t reg, iss_reg_t *value);
bool iss_csr_write(iss_t *iss, iss_reg_t reg, iss_reg_t value);

int iss_trace_pc_info(iss_addr_t addr, const char **func, const char **inline_func, const char **file, int *line);

#endif
