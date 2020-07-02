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