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

#ifndef __SA_SA_ISS_HPP__
#define __SA_SA_ISS_HPP__

#include "iss.hpp"

int load_binary(iss_t *iss, const char *name, int argc, char **argv, iss_reg_t *bootaddr);

static inline void storeWord(iss_t *cpu, unsigned int addr, uint32_t value)
{
  if (addr + 4 <= cpu->mem_size)
    *(uint32_t *)(cpu->mem_array + addr) = value;
}

static inline void storeByte(iss_t *cpu, unsigned int addr, uint8_t value)
{
  if (addr + 1 <= cpu->mem_size)
    *(uint8_t *)(cpu->mem_array + addr) = value;
}

static inline void loadWord(iss_t *cpu, unsigned int addr, uint32_t *value)
{
  if (addr + 4 <= cpu->mem_size)
    *value = *(uint32_t *)(cpu->mem_array + addr);
}

static inline void loadByte(iss_t *cpu, unsigned int addr, uint8_t *value)
{
  if (addr + 1 <= cpu->mem_size)
    *value = *(uint8_t *)(cpu->mem_array + addr);
}

static inline void iss_exit(iss_t *iss, int status)
{
  iss->exit_status = status;
  iss->hit_exit = 1;
  iss->fast_mode = 0;
}

#endif