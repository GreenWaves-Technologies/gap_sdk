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
