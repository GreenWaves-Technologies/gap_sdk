/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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