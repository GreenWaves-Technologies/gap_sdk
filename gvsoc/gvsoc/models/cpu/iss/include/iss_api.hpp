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

#ifndef __CPU_ISS_ISS_API_HPP
#define __CPU_ISS_ISS_API_HPP

#include "types.hpp"



static inline void iss_set_reg(iss_t *iss, int reg, iss_reg_t value);

static inline iss_reg_t iss_get_reg(iss_t *iss, int reg);

static inline iss_reg_t iss_get_reg_untimed(iss_t *iss, int reg);

static inline iss_reg_t *iss_reg_ref(iss_t *iss, int reg);

static inline iss_reg_t *iss_reg_store_ref(iss_t *iss, int reg);


static inline void iss_pccr_account_event(iss_t *iss, unsigned int event, int incr);

static inline void iss_perf_account_taken_branch(iss_t *iss);

static inline void iss_perf_account_ld_stall(iss_t *iss);

static inline void iss_perf_account_jump(iss_t *iss);



#endif