/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies
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
 * Authors: Andreas Traber
 *          Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */


#include "gdb-server/gdb-server.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <map>

#define INSN_IS_COMPRESSED(instr) ((instr & 0x3) != 0x3)
#define INSN_BP_COMPRESSED   0x9002
#define INSN_BP              0x00100073

Breakpoints::Breakpoints(Gdb_server * top)
: m_top(top) {
}

#define FIND(m, addr) (m).find((addr)) != (m).end()
#define FIND_IT(m, addr, it) ((it)=(m).find((addr))) != (m).end()

void Breakpoints::insert(unsigned int addr) {
  breakpoints_map_t::iterator it;

  if (FIND(breakpoints, addr))
  {
    m_top->log.error("breakpoint already inserted at 0x%08x\n", addr);
    return;
  }

  m_top->log.debug("Insert breakpoint at addr: 0x%08x\n", addr);

  breakpoint_ptr_t sp_bp;
  breakpoints_map_t::iterator it_disabled;

  if (FIND_IT(disabled_bps, addr, it_disabled)) {
    sp_bp = it_disabled->second;
    disabled_bps.erase(it_disabled);
  } else {
    sp_bp = std::make_shared<Breakpoint>(m_top, addr);
    enabled_bps[addr] = sp_bp;
  }

  breakpoints[addr] = sp_bp;
  sp_bp->enable();
}

void Breakpoints::remove_it(breakpoints_map_t::iterator it) {
  breakpoint_ptr_t sp_bp = it->second;
  sp_bp->disable();

  breakpoints_map_t::iterator it_enabled;
  if (FIND_IT(enabled_bps, it->first, it_enabled)) {
    enabled_bps.erase(it_enabled);
  } else {
    disabled_bps[it->first] = sp_bp;
  }

  breakpoints.erase(it);
}

void Breakpoints::remove(unsigned int addr) {
  breakpoints_map_t::iterator it;

  if (!(FIND_IT(breakpoints, addr, it)))
  {
    m_top->log.debug("No breakpoint to remove at 0x%08x\n", addr);
    return;
  }
  this->remove_it(it);
}

// Clears the history of additions and removals
void
Breakpoints::clear_history() {
  enabled_bps.clear();
  disabled_bps.clear();
}

// Sees if anything has changed since last history clear
bool
Breakpoints::have_changed() {
  return enabled_bps.size() > 0 || disabled_bps.size() > 0;
}

void Breakpoints::clear() {
  breakpoints_map_t::iterator it;
  for (it = breakpoints.begin(); it != breakpoints.end(); it++)
  {
    this->remove_it(it);
  }
}

bool
Breakpoints::at_addr(unsigned int addr) {
  return FIND(breakpoints, addr);
}

void Breakpoints::enable_all() {
  m_top->log.debug("Enable all breakpoints (size: %d)\n", breakpoints.size());
  
  for (breakpoints_map_t::iterator it = breakpoints.begin(); it != breakpoints.end(); it++) {
    breakpoints_map_t::iterator it_disabled;
    it->second->enable();
    if (FIND_IT(disabled_bps, it->first, it_disabled))
    {
      disabled_bps.erase(it_disabled);
    }
    else 
    {
      enabled_bps[it->first] = it->second;
    }
  }
}

void Breakpoints::disable_all() {

  m_top->log.debug("Disable all breakpoints\n");

  for (breakpoints_map_t::iterator it = breakpoints.begin(); it != breakpoints.end(); it++) {
    breakpoints_map_t::iterator it_enabled;
    it->second->disable();
    if (FIND_IT(enabled_bps, it->first, it_enabled))
    {
      enabled_bps.erase(it_enabled);
    }
    else 
    {
      disabled_bps[it->first] = it->second;
    }
  }
}

Breakpoints::Breakpoint::Breakpoint(Gdb_server * top, uint32_t addr) : m_top(top), addr(addr)
{
}

void Breakpoints::Breakpoint::enable()
{
  if (this->enabled)
    m_top->log.error("breakpoint at addr: 0x%08x was already enabled\n", addr);

  uint32_t insn_orig;
  m_top->target->mem_read(addr, 4, (char*)&insn_orig);
  is_compressed = INSN_IS_COMPRESSED(insn_orig);

  m_top->log.debug("Enable %sbreakpoint at addr: 0x%08x old_insn: 0x%08x\n", is_compressed?"compressed ":"", addr, insn_orig);

  if (is_compressed) {
    insn_orig16 = insn_orig & 0xffff;
    uint16_t data_bp = INSN_BP_COMPRESSED;
    m_top->target->mem_write(addr, 2, (char*)&data_bp);
  } else {
    insn_orig32 = insn_orig;
    uint32_t data_bp = INSN_BP;
    m_top->target->mem_write(addr, 4, (char*)&data_bp);
  }

  m_top->target->mem_read(addr, 4, (char*)&insn_orig);
  m_top->log.debug("Written INSN 0x%08x\n", insn_orig);
 
  this->enabled = true;
}

void Breakpoints::Breakpoint::disable()
{
  uint32_t data_bp;
  if (!this->enabled)
    m_top->log.error("breakpoint at addr: 0x%08x was already disabled\n", addr);

  m_top->target->mem_read(addr, 4, (char*)&data_bp);

  m_top->log.debug("Disable %sbreakpoint at addr: 0x%08x contents: 0x%08x\n", is_compressed?"compressed ":"", addr, data_bp);

  if (is_compressed != INSN_IS_COMPRESSED(data_bp))
    m_top->log.error("breakpoint at addr: 0x%08x compressed does not match actual instruction\n", addr);


  if (is_compressed)
  {
    if ((data_bp&0xffff) != INSN_BP_COMPRESSED)
      m_top->log.error("breakpoint at addr: 0x%08x contents 0x%08x is not a compressed ebrk\n", addr, data_bp);
  } else {
    if (data_bp != INSN_BP)
      m_top->log.error("breakpoint at addr: 0x%08x contents 0x%08x is not an ebrk\n", addr, data_bp);
  }

  if (is_compressed) {
    m_top->target->mem_write(addr, 2, (char*)&insn_orig16);
  } else {
    m_top->target->mem_write(addr, 4, (char*)&insn_orig32);
  }

  m_top->target->mem_read(addr, 4, (char*)&data_bp);
  m_top->log.debug("Written INSN 0x%08x\n", data_bp);

  this->enabled = false;
}

