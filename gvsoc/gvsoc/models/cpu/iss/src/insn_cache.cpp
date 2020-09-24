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

#include "iss.hpp"
#include <string.h>


static void insn_block_init(iss_insn_block_t *b, iss_addr_t pc);
void insn_init(iss_insn_t *insn, iss_addr_t addr);

static void flush_cache(iss_t *iss, iss_insn_cache_t *cache)
{
  prefetcher_flush(iss);

  for (int i=0; i<ISS_INSN_NB_BLOCKS; i++)
  {
    iss_insn_block_t *b = cache->blocks[i];
    while(b)
    {
      iss_insn_block_t *next = b->next;
      b->is_init = false;
      b = next;
    }
 }
}


int insn_cache_init(iss_t *iss)
{
  iss_insn_cache_t *cache = &iss->cpu.insn_cache;
  memset(cache->blocks, 0, sizeof(iss_insn_block_t *)*ISS_INSN_NB_BLOCKS);
  return 0;
}

void insn_init(iss_insn_t *insn, iss_addr_t addr) {
  insn->handler = iss_decode_pc;
  insn->fast_handler = iss_decode_pc;
  insn->addr = addr;
  insn->next = NULL;
  insn->hwloop_handler = NULL;
}

static void insn_block_init(iss_insn_block_t *b, iss_addr_t pc)
{
  b->is_init = true;
  for (int i=0; i<ISS_INSN_BLOCK_SIZE; i++)
  {
    iss_insn_t *insn = &b->insns[i];
    insn_init(insn, pc + (i<<ISS_INSN_PC_BITS));
  }
}



void iss_cache_flush(iss_t *iss)
{
  flush_cache(iss, &iss->cpu.insn_cache);
}



iss_insn_t *insn_cache_get(iss_t *iss, iss_addr_t pc)
{
  iss_addr_t pc_base = pc & ~((1 << (ISS_INSN_BLOCK_SIZE_LOG2 + ISS_INSN_PC_BITS)) - 1);
  unsigned insn_id = (pc >> ISS_INSN_PC_BITS) & (ISS_INSN_BLOCK_SIZE - 1);
  unsigned int block_id = pc_base & (ISS_INSN_NB_BLOCKS - 1);
  iss_insn_cache_t *cache = &iss->cpu.insn_cache;
  iss_insn_block_t *block = cache->blocks[block_id];
  iss_insn_block_t *first_free = NULL;

  while (block)
  {
    if (block->pc == pc_base)
    {
      return &block->insns[insn_id];
    }
    if (!block->is_init)
    {
      first_free = block;
    }

    block = block->next;
  }

  iss_insn_block_t *b = first_free;
  if (b == NULL)
  {
    b = (iss_insn_block_t *)malloc(sizeof(iss_insn_block_t));
    b->next = cache->blocks[block_id];
    cache->blocks[block_id] = b;
  }

  b->pc = pc_base;
  
  insn_block_init(b, pc_base);

  return &b->insns[insn_id];
}

iss_insn_t *insn_cache_get_decoded(iss_t *iss, iss_addr_t pc)
{
  iss_insn_t *insn = insn_cache_get(iss, pc);
  if (insn->handler != iss_decode_pc) return insn;
  return iss_decode_pc_noexec(iss, insn);
}
