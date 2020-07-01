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

#ifndef __CPU_ISS_PREFETCHER_HPP
#define __CPU_ISS_PREFETCHER_HPP

#include "types.hpp"
#include <stdio.h>
#include "platform_wrapper.hpp"

static inline void prefetcher_init(iss_t *iss);
static inline iss_opcode_t prefetcher_get_word(iss_t *iss, iss_addr_t addr);
static inline iss_opcode_t prefetcher_fill(iss_t *iss, iss_addr_t addr, bool timed);



static inline iss_opcode_t prefetcher_fill(iss_prefetcher_t *prefetcher, iss_t *iss, iss_addr_t addr, bool timed, bool fill)
{
  uint32_t aligned_addr = addr & ~(ISS_PREFETCHER_SIZE-1);
  iss_fetch_req(iss, aligned_addr, fill ? prefetcher->data : NULL, ISS_PREFETCHER_SIZE, false);
  prefetcher->addr = aligned_addr;
  return 0;
}



static inline iss_opcode_t __attribute__((always_inline)) prefetcher_get_word_common(iss_prefetcher_t *prefetcher, iss_t *iss, iss_addr_t addr, bool timed)
{
  int index = addr - prefetcher->addr;

  if (likely(index >= 0 && index  <= ISS_PREFETCHER_SIZE - sizeof(iss_opcode_t)))
  {
    return *(iss_opcode_t *)&prefetcher->data[index];
  }

  if (unlikely(index < 0 || index >= ISS_PREFETCHER_SIZE))
  {
    prefetcher_fill(prefetcher, iss, addr, timed, 1);
    index = addr - prefetcher->addr;
  }

  if (likely(index + ISS_OPCODE_MAX_SIZE <= ISS_PREFETCHER_SIZE))
  {
    return *(iss_opcode_t *)&prefetcher->data[index];
  }
  else
  {
    // Case where the opcode is between 2 lines. The prefetcher can only store one line so we have
    // to temporarly store the first part to return it with the next one coming from the final line.
    iss_opcode_t opcode = 0;

    // Compute address of next line
    uint32_t next_addr = (addr + ISS_PREFETCHER_SIZE - 1) & ~(ISS_PREFETCHER_SIZE-1);
    // Number of bytes of the opcode which fits the first line
    int nb_bytes = next_addr - addr;
    // Copy first part from first line
    memcpy((void *)&opcode, (void *)&prefetcher->data[index], nb_bytes);
    // Fetch next line
    prefetcher_fill(prefetcher, iss, next_addr, timed, 1);
    // And append the second part from second line
    opcode = opcode | (( *(iss_opcode_t *)&prefetcher->data[0]) << (nb_bytes*8));

    return opcode;
  }
}



static inline void __attribute__((always_inline)) prefetcher_fetch_word(iss_prefetcher_t *prefetcher, iss_t *iss, iss_addr_t addr)
{
  int index = addr - prefetcher->addr;

  if (likely(index >= 0 && index  <= ISS_PREFETCHER_SIZE - sizeof(iss_opcode_t)))
  {
    return;
  }

  if (unlikely(index < 0 || index >= ISS_PREFETCHER_SIZE))
  {
    prefetcher_fill(prefetcher, iss, addr, 1, 0);
    index = addr - prefetcher->addr;
  }

  if (unlikely(index + ISS_OPCODE_MAX_SIZE > ISS_PREFETCHER_SIZE))
  {
    prefetcher_fill(prefetcher, iss, prefetcher->addr + ISS_PREFETCHER_SIZE, 1, 0);
  }
}



static inline iss_opcode_t prefetcher_get_word(iss_t *iss, iss_addr_t addr)
{
  return prefetcher_get_word_common(&iss->cpu.decode_prefetcher, iss, addr, 1);
}



static inline void __attribute__((always_inline)) prefetcher_fetch(iss_t *iss, iss_addr_t addr)
{
  prefetcher_fetch_word(&iss->cpu.prefetcher, iss, addr);
  int64_t cycles = iss->cpu.state.fetch_cycles;
  if (cycles)
  {
    iss->cpu.state.insn_cycles += cycles;
    iss->cpu.state.fetch_cycles = 0;
  }
}



static inline void prefetcher_flush(iss_t *iss)
{
  iss->cpu.decode_prefetcher.addr = -1;
  iss->cpu.prefetcher.addr = -1;
}



static inline void prefetcher_init(iss_t *iss)
{
  prefetcher_flush(iss);
}



#endif
