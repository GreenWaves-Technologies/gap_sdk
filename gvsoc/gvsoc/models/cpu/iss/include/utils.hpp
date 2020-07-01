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

#ifndef __CPU_ISS_UTILS_HPP
#define __CPU_ISS_UTILS_HPP

#include "types.hpp"

static inline iss_opcode_t iss_get_field(iss_opcode_t val, int shift, int bits)
{
  return (val >> shift) & ((1<<bits) - 1);
}

static inline iss_opcode_t iss_get_signed_value(iss_opcode_t val, int bits)
{
  return ((int)val) << (ISS_REG_WIDTH-bits) >> (ISS_REG_WIDTH-bits);
}

static inline iss_opcode_t iss_get_zext_value(iss_opcode_t val, int bits)
{
  return ((unsigned int)val) << (ISS_REG_WIDTH-bits) >> (ISS_REG_WIDTH-bits);
}

static inline uint64_t iss_get_field64(iss_opcode_t val, int shift, int bits)
{
  return (val >> shift) & ((1<<bits) - 1);
}

static inline int64_t iss_get_signed_value64(iss_opcode_t val, int bits)
{
  return ((int64_t)val) << (ISS_REG_WIDTH-bits) >> (ISS_REG_WIDTH-bits);
}

static inline uint64_t iss_get_zext_value64(iss_opcode_t val, int bits)
{
  return ((uint64_t)val) << (ISS_REG_WIDTH-bits) >> (ISS_REG_WIDTH-bits);
}

#endif
