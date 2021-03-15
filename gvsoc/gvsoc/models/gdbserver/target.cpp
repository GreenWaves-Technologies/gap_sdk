/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 *          Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */

#include "gdb-server/target.hpp"

#define IE_TRAP_EBRK (1<<3)
#define IE_TRAP_ILL (1<<2)

#define IE_TRAPS (IE_TRAP_EBRK|IE_TRAP_ILL)


Memory_wall::Memory_wall(uint32_t start, uint32_t len)
: start(start), len(len)
{

}


bool Memory_wall::overlaps(uint32_t start, uint32_t len)
{
  return (start < this->start + this->len) && (this->start < start + len);
}



Target_cluster_cache::Target_cluster_cache(Gdb_server * top, uint32_t addr)
: m_top(top), addr(addr)
{

}

void Target_cluster_cache::flush()
{
  uint32_t cache_addr = addr + 4;
  m_top->log.detail("Flushing cluster cache (addr: 0x%x)\n", cache_addr);

  uint32_t data = 0xFFFFFFFF;
  if (!m_top->cable->access(true, cache_addr, 4, (char*)&data))
    throw CableException("Error writing to 0x%08x", cache_addr);
}



Target_fc_cache::Target_fc_cache(Gdb_server * top, uint32_t addr)
: m_top(top), addr(addr)
{

}

void Target_fc_cache::flush()
{

  m_top->log.detail("Flushing FC cache (addr: 0x%x)\n", addr);

  uint32_t data = 0xFFFFFFFF;
  if (!m_top->cable->access(true, addr + 0x04, 4, (char*)&data))
    throw CableException("Error writing to 0x%08x", addr + 0x04);
  data = 0;
  if (!m_top->cable->access(false, addr + 0x0c, 4, (char*)&data))
    throw CableException("Error reading from 0x%08x", addr + 0x0c);
  m_top->log.detail("FC cache status 0x%x\n", data);
}


Target_cluster_ctrl_xtrigger::Target_cluster_ctrl_xtrigger(Gdb_server * top, uint32_t cluster_ctrl_addr)
: m_top(top), cluster_ctrl_addr(cluster_ctrl_addr)
{

}

void Target_cluster_ctrl_xtrigger::init()
{
  current_mask = 0;
  this->set_halt_mask(0xFFFFFFFF);
}

void Target_cluster_ctrl_xtrigger::set_halt_mask(uint32_t mask)
{
  if (current_mask != mask) {
    m_top->log.detail("set xtrigger halt mask 0x%08x 0x%02x\n", cluster_ctrl_addr + 0x000038, mask&0xff);
    if (!m_top->cable->access(true, cluster_ctrl_addr + 0x000038, 4, (char*)&mask))
      throw CableException("Target_cluster_ctrl_xtrigger::set_halt_mask() Error writing to 0x%08x", cluster_ctrl_addr + 0x000038);
    current_mask = mask;
  }
}

void Target_cluster_ctrl_xtrigger::set_resume(uint32_t mask)
{
  m_top->log.detail("set xtrigger resume register 0x%08x 0x%02x\n", cluster_ctrl_addr + 0x000028, mask&0xff);
  if (!m_top->cable->access(true, cluster_ctrl_addr + 0x000028, 4, (char*)&mask))
    throw CableException("Target_cluster_ctrl_xtrigger::set_resume() Error writing to 0x%08x", cluster_ctrl_addr + 0x000028);
}

uint32_t Target_cluster_ctrl_xtrigger::get_halt_mask() { 
  return this->current_mask;
}

uint32_t Target_cluster_ctrl_xtrigger::get_halt_status()
{
  uint32_t status = 0;
  if (!m_top->cable->access(false, cluster_ctrl_addr + 0x000028, 4, (char*)&status))
    throw CableException("Target_cluster_ctrl_xtrigger::get_halt_status() Error reading from 0x%08x\n", cluster_ctrl_addr + 0x000028);
  m_top->log.detail("get xtrigger halt mask 0x%08x 0x%02x\n", cluster_ctrl_addr + 0x000028, status&0xff);
  return status&current_mask;
}

Target_cluster_power_bypass::Target_cluster_power_bypass(Gdb_server * top, uint32_t reg_addr, int bit)
: m_top(top), reg_addr(reg_addr), bit(bit)
{
}



bool Target_cluster_power_bypass::is_on()
{
  uint32_t info = 0;
  if (!m_top->cable->access(false, reg_addr, 4, (char*)&info))
    throw CableException("Error reading from 0x%08x", reg_addr);
  int power = (info >> bit) & 1;
  // if (power && !m_stay_on_indicated) {
  //   info |= (1<<(bit+1));
  //   if (!m_top->cable->access(false, reg_addr, 4, (char*)&info))
  //     throw CableException("Error writing to 0x%08x", reg_addr);
  //   m_stay_on_indicated = true;
  // }
  m_top->log.debug("Cluster power bypass 0x%08x=0x%08x bit %d=%d\n", reg_addr, info, bit, power);
  return power;
}



Target_core::Target_core(Gdb_server * top, uint32_t dbg_unit_addr, Target_cluster_common *cluster, int core_id)
: m_top(top), dbg_unit_addr(dbg_unit_addr), cluster(cluster), core_id(core_id)
{
  m_top->log.print(LOG_DEBUG, "Instantiated core %d:%d\n", this->get_cluster_id(), this->core_id);
  this->thread_id = first_free_thread_id++;
}



void Target_core::init()
{
  m_top->log.print(LOG_DEBUG, "Init core\n");
  this->is_on = false;
  pc_is_cached = false;
  stopped = false;
  step = false;
  commit_step = false;
}



int Target_core::first_free_thread_id = 0;



void Target_core::flush()
{
  m_top->log.debug("Flushing core prefetch buffer (cluster: %d, core: %d)\n", this->get_cluster_id(), core_id);
  // Write back the value of NPC so that it triggers a flush of the prefetch buffer
  uint32_t npc;
  this->read(DBG_NPC_REG, &npc);
  this->write(DBG_NPC_REG, npc);
}



void Target_core::gpr_read_all(uint32_t *data)
{
  if (!is_on) throw OffCoreAccessException();

  m_top->log.debug("Reading all registers (cluster: %d, core: %d)\n", this->get_cluster_id(), core_id);

  memset(data, 0, sizeof(uint32_t) * 32);

  if (!m_top->cable->access(false, dbg_unit_addr + 0x0400, 32 * 4, (char*)data))
    throw CableException("Error reading from to 0x%08x", dbg_unit_addr + 0x0400);
}



void Target_core::gpr_read(unsigned int i, uint32_t *data)
{
  if (!is_on) throw OffCoreAccessException();
  this->read(0x0400 + i * 4, data);
}



void Target_core::gpr_write(unsigned int i, uint32_t data)
{
  if (!is_on) throw OffCoreAccessException();
  this->write(0x0400 + i * 4, data);
}


void Target_core::ie_write(uint32_t data)
{
  if (!is_on) return;
  m_top->log.print(LOG_DEBUG, "%d:%d -----> TRAP ENABLED\n", this->get_cluster_id(), core_id);
  this->write(DBG_IE_REG, data);
  uint32_t verif = 0;
  this->read(DBG_IE_REG, &verif);
  if (data != verif)
    m_top->log.error("TRAP ENABLE FAILED!!!!!!");
}

bool Target_core::has_power_state_change()
{
  bool state_changed = this->power_state_changed;
  this->power_state_changed = false;
  return state_changed;
}

bool Target_core::get_power()
{
  return this->is_on;
}

void Target_core::set_power(bool is_on)
{

  m_top->log.detail("Core %d:%d check power %d -> %d\n", this->get_cluster_id(), core_id, this->is_on, is_on);
  if (is_on != this->is_on)
  {
    m_top->log.debug("Core %d:%d power state changed\n", this->get_cluster_id(), core_id, this->is_on, is_on);
    this->power_state_changed = true;
    this->pc_is_cached = false; // power state has changed - pc cannot be cached
    this->is_on = is_on;
    if (is_on) {
      m_top->log.print(LOG_DEBUG, "Core %d:%d on\n", this->get_cluster_id(), core_id);
      // // // let's discover core id and cluster id
      // this->stop();
      // uint32_t hartid_tmp;
      // csr_read(0x014, &hartid_tmp);
      // // csr_read(0xF14, &hartid);
      // m_top->log.print(LOG_DEBUG, "Read hart id %d\n", hartid_tmp);

      // // cluster_id = hartid >> 5;
      // // core_id = hartid & 0x1f;

      // m_top->log.print(LOG_DEBUG, "Found a core with id %X (cluster: %d, core: %d)\n", hartid, this->get_cluster_id(), core_id);
      // uint32_t old_ctrl;
      // this->read(DBG_CTRL_REG, &old_ctrl);
      // this->write(DBG_CTRL_REG, old_ctrl|0x10000);
      this->ie_write(IE_TRAPS); // traps on illegal instructions and ebrks
      // if (!stopped) this->write(DBG_CTRL_REG, old_ctrl);
      // if (!stopped) resume();
    } else {
      m_top->log.print(LOG_DEBUG, "Core %d:%d off\n", this->get_cluster_id(), core_id);
    }
  }
}


bool Target_core::register_available(int UNUSED(num))
{
  return is_on && !m_half_stopped;
}

void Target_core::read(uint32_t addr, uint32_t* rdata)
{
  if (!is_on) throw OffCoreAccessException();

  uint32_t offset = dbg_unit_addr + addr;
  bool res = m_top->cable->access(false, offset, 4, (char*)rdata);
  if (res) {
    m_top->log.detail("Reading register (addr: 0x%x, value: 0x%08x)\n", offset, *rdata);
  } else {
    throw CableException("Error reading register (addr: 0x%x)", offset);
  }
}



void Target_core::write(uint32_t addr, uint32_t wdata)
{
  if (!is_on) throw OffCoreAccessException();

  uint32_t offset = dbg_unit_addr + addr;
  bool res = m_top->cable->access(true, offset, 4, (char*)&wdata);
  if (res) {
    m_top->log.detail("Writing register (addr: 0x%x, value: 0x%08x)\n", offset, wdata);
  } else {
    throw CableException("Error writing register (addr: 0x%x)", offset);
  }
}



void Target_core::csr_read(unsigned int i, uint32_t *data)
{
  this->read(0x4000 + i * 4, data);
  m_top->log.detail("Reading CSR at offset 0x%08x (value: 0x%08x)\n", i, *data);
}

void Target_core::csr_write(unsigned int i, uint32_t data)
{
  m_top->log.detail("Writing CSR at offset 0x%08x\n", i);
  this->write(0x4000 + i * 4, data);
}


bool Target_core::is_stopped() {
  if (!is_on) return false;
  if (this->stopped) return true;

  uint32_t data;
  this->read(DBG_CTRL_REG, &data);

  this->stopped = data & 0x10000;

  m_top->log.debug("Checking core status (cluster: %d, core: %d, stopped: %d)\n", this->get_cluster_id(), core_id, this->stopped);

  return this->stopped;
}


void Target_core::stop()
{
  if (!is_on||this->stopped) return;

  m_top->log.debug("Halting core (cluster: %d, core: %d, is_on: %d)\n", this->get_cluster_id(), core_id, is_on);
  uint32_t data = 0;

  this->read(DBG_CTRL_REG, &data);

  if (!(data&0x10000)) {
    data |= 0x10000;
    this->write(DBG_CTRL_REG, data);
  }
  // verify
  data = 0;
  this->read(DBG_CTRL_REG, &data);
  if (!(data&0x10000)) {
    m_half_stopped = true;
    m_top->log.error("read 0x%08x from CTRL_REG after stop\n", data);
  } else {
    m_half_stopped = false;
  }

  this->stopped = true;
}



void Target_core::halt()
{
  stop();
}



void Target_core::set_step_mode(bool new_step)
{
  if (new_step != step) {
    m_top->log.debug("Setting step mode (cluster: %d, core: %d, step: %d, new_step: %d)\n",  this->get_cluster_id(), core_id, step, new_step);
    this->step = new_step;
    this->commit_step = true;
  }
}


void Target_core::commit_step_mode()
{
  if (!is_on) return;

  if (commit_step) {
    m_top->log.debug("Committing step mode (cluster: %d, core: %d, step: %d)\n",  this->get_cluster_id(), core_id, step);
    this->write(DBG_CTRL_REG, (1<<16) | step);
    this->commit_step = false;
  }
}

// internal helper functions
bool Target_core::actual_pc_read(unsigned int* pc)
{
  uint32_t npc;
  uint32_t ppc;
  uint32_t cause;
  bool is_hit;

  if (pc_is_cached) {
    m_top->log.debug("PC was cached at 0x%08x Core %d:%d (is_BP: %d)\n", 
      pc_cached, this->get_cluster_id(), this->get_core_id(), on_trap);
    *pc = pc_cached;
    return true;
  }

  if (!is_on) return false;

  this->read(DBG_PPC_REG, &ppc);
  this->read(DBG_NPC_REG, &npc);
  this->read_hit(&is_hit, &m_is_sleeping);

  if (is_hit) {
    *pc = npc;
    on_trap = false;
  } else {
    cause = this->get_cause();
    *pc = (EXC_CAUSE_INTERUPT(cause)||cause==EXC_CAUSE_DBG_HALT ? npc : ppc);
    on_trap = cause == EXC_CAUSE_BREAKPOINT;
  }
  m_top->log.debug("PPC 0x%x NPC 0x%x PC 0x%x Core %d:%d (is_BP: %d)\n", 
    ppc, npc, *pc, this->get_cluster_id(), this->get_core_id(), on_trap);
  pc_cached = *pc;
  pc_is_cached = true;
  return true;
}

void Target_core::read_hit(bool *is_hit, bool *is_sleeping)
{
  if (!is_on){
    *is_hit = false; 
    *is_sleeping = false;
    return;
  }
  uint32_t hit;
  this->read(DBG_HIT_REG, &hit);

  *is_hit = step && ((hit & 0x1) == 0x1);
  *is_sleeping = ((hit & 0x10) == 0x10);
}

uint32_t Target_core::get_cause()
{
  if (!is_on) return 0x1f;

  uint32_t cause;
  this->read(DBG_CAUSE_REG, &cause);

  m_top->log.debug("core %d:%d stop cause %x\n", this->get_cluster_id(), this->get_core_id(), cause);
  return cause;
}


uint32_t Target_core::check_stopped()
{

  bool stopped = this->is_stopped();
  m_top->log.debug("Check core %d stopped %d resume %d\n", core_id, stopped, this->should_resume());

  if (this->should_resume() && stopped) {
    uint32_t cause;
    bool is_hit, is_sleeping;
    this->read_hit(&is_hit, &is_sleeping);
    if (is_hit) {
      m_top->log.debug("core %d:%d tid %d single stepped\n", this->get_cluster_id(), this->get_core_id(), this->get_thread_id()+1);
      return EXC_CAUSE_BREAKPOINT;
    }
    cause = this->get_cause();
    if (cause == EXC_CAUSE_BREAKPOINT) {
      m_top->log.debug("core %d:%d tid %d hit breakpoint\n", this->get_cluster_id(), this->get_core_id(), this->get_thread_id()+1);
      return cause;
    } else {
      m_top->log.debug("core %d:%d tid %d is stopped with cause 0x%08x\n", this->get_cluster_id(), this->get_core_id(), this->get_thread_id()+1, cause);
      return cause;
    }

  }
  return EXC_CAUSE_NONE;
}



int Target_core::get_cluster_id() { return cluster->get_id(); }

void Target_core::get_name(char* str, size_t len) {
  snprintf(str, len, "Cluster %02d - Core %01d%s", get_cluster_id(), get_core_id(), (get_power()?"":" (Off)"));
}


void Target_core::prepare_resume(bool step)
{
  if (resume_prepared) return;

  m_top->log.debug("Preparing core %d:%d to resume (step: %d)\n", this->get_cluster_id(), this->get_core_id(), step);

  resume_prepared = true;
  cluster->prepare_resume(step);

  this->set_step_mode(step);
}

bool Target_core::decide_resume()
{
  return !(pc_is_cached && m_top->bkp->at_addr(pc_cached) && is_stopped_on_trap());
}

void Target_core::commit_resume()
{
  if (!this->is_on) {
    this->stopped = false;
    this->pc_is_cached = false;
    on_trap = false;
    return;
  }

  // we previously stopped on a breakpoint so step back to reexecute the instruction
  if (on_trap) {
    m_top->log.debug("Core %d:%d was on breakpoint. Re-executing\n", get_cluster_id(), get_core_id());
    write(DBG_NPC_REG, pc_cached); // re-execute this instruction - this will also flush prefetch
    on_trap = false;
  } else if (m_top->bkp->have_changed()) {
    this->flush();
  }

  this->stopped = false;
  this->pc_is_cached = false;

  m_top->log.debug("Commit resume (cluster: %d, core: %d, step: %d)\n",  this->get_cluster_id(), core_id, step);

  this->commit_step_mode();
  // clear hit register, has to be done before CTRL
  this->write(DBG_HIT_REG, 0);
}



void Target_core::resume()
{
  if (!is_on) return;
  uint32_t ppc, npc, ctrl;
  this->read(DBG_PPC_REG, &ppc);
  this->read(DBG_NPC_REG, &npc);
  this->read(DBG_CTRL_REG, &ctrl);
  m_top->log.debug("Resuming (cluster: %d, core: %d, step: %d) ppc 0x%08x npc 0x%08x ctrl 0x%08x\n",  this->get_cluster_id(), core_id, step, ppc, npc, ctrl);

  this->write(DBG_CTRL_REG, step);

  this->read(DBG_CTRL_REG, &ctrl);

  if (ctrl != step) {
    m_top->log.debug("Core %d:%d - wrote 0x%08x got 0x%08x\n", this->get_cluster_id(), this->get_core_id(), step, ctrl);
  } else {
    m_top->log.debug("Core %d:%d - started ok\n", this->get_cluster_id(), this->get_core_id(), step, ctrl);
  }
}



Target_cluster_common::Target_cluster_common(js::config *, Gdb_server * top, uint32_t cluster_addr, uint32_t xtrigger_addr, int cluster_id)
: m_top(top), cluster_id(cluster_id), cluster_addr(cluster_addr), xtrigger_addr(xtrigger_addr)
{
  m_top->log.debug("Instantiating cluster %d\n",  cluster_id);
}



Target_cluster_common::~Target_cluster_common()
{
}


void Target_cluster_common::add_memory_wall(uint32_t addr, uint32_t len)
{
  m_top->log.debug("Add memory wall to cluster %d start: 0x%08x end: 0x%08x\n", cluster_id, addr, addr + len - 1);
  mem_walls.push_back(std::make_shared<Memory_wall>(addr, len));
}

bool Target_cluster_common::memory_wall_overlaps(uint32_t addr, uint32_t len)
{
  for (auto &wall: mem_walls)
  {
    if (wall->overlaps(addr, len)) return true;
  }
  return false;
}

void Target_cluster_common::init()
{
  m_top->log.debug("Init cluster %d\n", cluster_id);
  is_on = false;
  nb_on_cores = 0;
  power->init();
  for (auto &core: cores)
  {
    core->init();
  }
}



std::shared_ptr<Target_core> Target_cluster_common::check_stopped(uint32_t *stopped_cause)
{
  *stopped_cause = EXC_CAUSE_NONE;

  this->update_power();
  if (!is_on) return nullptr;

  m_top->log.debug("Check if cluster %d stopped\n", get_id());

  if (this->ctrl->has_xtrigger()) {
    if (!std::dynamic_pointer_cast<Target_cluster_ctrl_xtrigger>(this->ctrl)->get_halt_status())
      return nullptr;
  }

  std::shared_ptr<Target_core> stopped_core = nullptr;
  for (auto &core: cores)
  {
    // core didn't resume
    if (!core->should_resume()) continue;

    uint32_t cause = core->check_stopped();
    if (cause == EXC_CAUSE_BREAKPOINT) {
      stopped_core = core;
      *stopped_cause = cause;
      break;
#define ONLY_CHECK_ONE
#ifdef ONLY_CHECK_ONE
    } else if (cause != EXC_CAUSE_NONE) {
      if (!stopped_core) {
        stopped_core = core;
        *stopped_cause = cause;
      }
    } else {
      if (!stopped_core && this->ctrl->has_xtrigger()) {
        return nullptr;
      }
    }
#else
    } else if (!stopped_core && cause != EXC_CAUSE_NONE) {
      stopped_core = core;
      *stopped_cause = cause;
    }
#endif
  }
  return stopped_core;
}

void Target_cluster_common::prepare_resume(bool step)
{
  if (resume_prepared) return;

  m_top->log.debug("Preparing cluster %d to resume (step: %d)\n", cluster_id, step);

  resume_prepared = true;
}

void Target_cluster_common::clear_resume()
{
  resume_prepared = false;
  for (auto &core: cores) {
    core->clear_resume();
  }
}

void Target_cluster_common::flush()
{
  if (!is_on) return;

  m_top->log.debug("Flushing cluster instruction cache (cluster: %d, is_on: %d)\n", cluster_id, is_on);

  if (this->cache) this->cache->flush();
}

bool Target_cluster_common::decide_resume()
{
  bool res = true;
  for (auto &core: cores) {
    if (core->should_resume()) {
      // If at least one core is stepping then we must resume
      // since gdb will continue trying until it actually steps
      if (core->get_step_mode()) return true;
      // check if the core is already on a breakpoint
      if (!core->decide_resume()) {
        res = false;
      }
    } 
  }
  return res;
}

void Target_cluster_common::commit_resume()
{


  if (!is_on) {
    m_top->log.debug("Cluster %d is off - not committing resume\n", cluster_id);
    return;
  }

  if (!resume_prepared) {
    m_top->log.debug("Cluster %d is not resuming - not committing resume\n", cluster_id);
    return;
  }

  m_top->log.debug("Committing resume (cluster: %d)\n", cluster_id);

  if (m_top->bkp->have_changed()) {
    this->flush();
  }

  for (auto &core: cores) {
    if (core->should_resume()) {
      core->commit_resume();
    }
  }
}

void Target_cluster_common::resume()
{

  if (!is_on) {
    m_top->log.debug("Cluster %d is off - not resuming\n", cluster_id);
    return;
  }

  if (!resume_prepared) {
    m_top->log.debug("Cluster %d is not resuming\n", cluster_id);
    return;
  }

  m_top->log.debug("Resuming (cluster: %d)\n", cluster_id);

  if (this->ctrl->has_xtrigger()) {

    // This cluster is a one with the cross-trigger matrix, use it to resume all cores
    // As the step mode is cached and committed when writing to the individual core
    // debug register, we have to commit it now before resuming the core through the 
    // global register

    uint32_t xtrigger_mask = 0;
    for (auto &core: cores) {
      if (core->should_resume()) {
        xtrigger_mask|=1<<core->get_core_id();
      }
    }

    std::shared_ptr<Target_cluster_ctrl_xtrigger> xctrl = 
      std::dynamic_pointer_cast<Target_cluster_ctrl_xtrigger>(this->ctrl);

    xctrl->set_halt_mask(xtrigger_mask);

    m_top->log.debug("Resuming cluster through global register (cluster: %d, mask: %x)\n", cluster_id, xtrigger_mask);
    xctrl->set_resume(xtrigger_mask);
  } else {
    // Otherwise, just resume them individually
    for (auto &core: cores) {
      if (core->should_resume()) {
        core->resume();
      }
    }
  }
}


void Target_cluster_common::update_power()
{
  set_power(get_power());
}


bool Target_cluster_common::get_power()
{
  return power->is_on();
}

void Target_cluster_common::set_power(bool is_on)
{

  m_top->log.detail("Cluster %d check power %d -> %d\n", this->cluster_id, this->is_on, is_on);
  if (is_on != this->is_on) {
    m_top->log.debug("Cluster %d power state changed\n", this->cluster_id, this->is_on, is_on);
    this->is_on = is_on;
    if (this-is_on) {
      m_top->log.debug("Do controller init\n");
      ctrl->init();
    }
  }

  if (is_on) {
    if (nb_on_cores != nb_core)
    {
      nb_on_cores=0;
      m_top->log.debug("Set all on (is_on: %d, nb_on_cores: %d, nb_core: %d)\n", is_on, nb_on_cores, nb_core);
      for(auto const& core: cores)
      {
        core->set_power(is_on);
        nb_on_cores++;
      }
    }
  }
  else
  {
    nb_on_cores = 0;
  }
}



void Target_cluster_common::halt()
{
  // Either the core is alone (FC) or the cluster is using a cross-trigger matrix to stop all cores
  // thus only stop the first one
  this->update_power();

  if (!is_on) {
    m_top->log.debug("Cluster %d is off\n", cluster_id);
    return;
  }
  m_top->log.debug("Halting cluster (cluster: %d)\n", cluster_id);

  if (this->ctrl->has_xtrigger()) {
    std::dynamic_pointer_cast<Target_cluster_ctrl_xtrigger>(this->ctrl)->set_resume(0);
  }

  // Cache all the PCs and halt status and move PCs back over breakpoints before doing anything else
  uint32_t pc;
  for (auto &core: cores)
  {
    if (!this->ctrl->has_xtrigger())
      core->halt();
    else
      core->is_stopped();
    
    if (!core->actual_pc_read(&pc))
      return;

    // This is no longer needed but keeping it around in case
    // Now the resume does not occur if a core is still on a breakpoint.
    // // If there is a breakpoint at the address of the actual program counter
    // // it must have executed at the same time as a breakpoint on another
    // // core so reexecute it
    // if (m_top->bkp->at_addr(pc) && core->is_stopped_on_trap()) {
    //   m_top->log.debug("Core %d:%d was on breakpoint. Re-executing\n", core->get_cluster_id(), core->get_core_id());
    //   core->write(DBG_NPC_REG, pc); // re-execute this instruction
    // }
  }
}



Target_cluster::Target_cluster(js::config *system_config, js::config *config, Gdb_server * top, uint32_t cluster_base, uint32_t xtrigger_addr, int cluster_id)
: Target_cluster_common(config, top, cluster_base, xtrigger_addr, cluster_id)
{
  auto _top = m_top;
  int nb_pe = config->get("nb_pe")->get_int();
  _top->log.debug("creating cluster %d with %d cores\n", cluster_id, nb_pe);
  for (int i=0; i<nb_pe; i++)
  {
    cores.push_back(std::make_shared<Target_core>(top, cluster_base + 0x300000 + i * 0x8000, this, i));
    nb_core++;
  }

  // Figure out if the cluster can be powered down
  js::config *bypass_config = system_config->get("**/apb_soc_ctrl/regmap/power/bypass");
  _top->log.debug("cluster %d power bypass %d\n", cluster_id, bypass_config != NULL);
  if (bypass_config)
  {
    uint32_t addr = system_config->get("**/apb_soc_ctrl/base")->get_int() +
      bypass_config->get("offset")->get_int();
    // int bit = bypass_config->get("content/dbg1/bit")->get_int();
    int bit = 16; // patched manually for GAP ... needs updating in JSON

    // Case where there is an apb soc ctrl register which tells if the cluster is on
    power = std::make_shared<Target_cluster_power_bypass>(top, addr, bit);
  }
  else
  {
    // Otherwise, the cluster will always be on
    power = std::make_shared<Target_cluster_power>();
  }

#ifdef __USE_XTRIGGER__
  ctrl = std::make_shared<Target_cluster_ctrl_xtrigger>(top, cluster_base + 0x00200000);
#else
  ctrl = std::make_shared<Target_cluster_ctrl>();
  auto xctrl = std::make_shared<Target_cluster_ctrl_xtrigger>(top, cluster_base + 0x00200000);
  xctrl->set_halt_mask(0);
#endif
  cache = std::make_shared<Target_cluster_cache>(top, cluster_base + 0x00201400);

  this->update_power();
}



Target_fc::Target_fc(js::config *system_config, js::config *config, Gdb_server * top, uint32_t fc_dbg_base, uint32_t fc_cache_base, int cluster_id)
: Target_cluster_common(config, top, fc_dbg_base, -1, cluster_id)
{
  js::config *eu_status = system_config->get("**/fc_eu/regmap/registers/status");
  js::config *eu_base = system_config->get("**/fc_eu/base");

  if (eu_status && eu_base) {
    m_fc_eu_status = eu_base->get_int() + eu_status->get("offset")->get_int();
    m_top->log.debug("FC EU clock status register at 0x%x\n", m_fc_eu_status);
  }

  cores.push_back(std::make_shared<Target_core>(top, fc_dbg_base, this, 0));
  nb_core++;

  // the FC will always be on
  power = std::make_shared<Target_cluster_power>();

  ctrl = std::make_shared<Target_cluster_ctrl>();

  if (fc_cache_base != 0xffffffff)
    cache = std::make_shared<Target_fc_cache>(top, fc_cache_base);

  this->update_power();
}

std::shared_ptr<Target_core> Target_fc::check_stopped(uint32_t *stopped_cause)
{
  *stopped_cause = EXC_CAUSE_NONE;

  this->update_power();

  if (!is_on) return nullptr;

  if (!cores[0]->should_resume()) return nullptr;

  // if (m_fc_eu_status && event_unit_core_gated()) return nullptr;

  m_top->log.debug("Check if cluster %d stopped\n", get_id());

  uint32_t cause = cores[0]->check_stopped();

  if (cause != EXC_CAUSE_NONE) {
    *stopped_cause = cause;
    return cores[0];
  }

  return nullptr;
}

bool Target_fc::event_unit_core_gated()
{
  uint32_t clk_status = 0;

  if (!m_top->cable->access(false, m_fc_eu_status, 4, (char*)&clk_status))
     throw CableException("Error reading clock gate status");

  m_top->log.detail("FC EU clock gate status %d\n", clk_status);
  return clk_status&0x1;
}

#if 0
void Target_cluster::set_power(bool is_on)
{
  if (is_on != this->is_on) {
    this->is_on = is_on;

    if (is_on && base_addr != -1) {
      uint32_t info;
      // set all-stop mode, so that all cores go to debug when one enters debug mode
      info = 0xFFFFFFFF;
      m_mem->access(1, base_addr + 0x000038, 4, (char*)&info);
    }
  }

  if (is_on && nb_on_cores != itfs.size()) {
    uint32_t info = -1;
    if (base_addr != -1) {
      m_mem->access(0, base_addr + 0x000008, 4, (char*)&info);
    }
    int i = 0;
    for (std::list<DbgIF_core*>::iterator it = itfs.begin(); it != itfs.end(); it++, i++) {
      if ((info >> i) & 1) {
        (*it)->set_power(is_on);
      }
    }
  } else {
    nb_on_cores = 0;
  }
}
#endif



Target::Target(Gdb_server * top)
:m_top(top)
{
  auto _top = m_top;
  _top->log.debug("Init target\n");
  js::config *config = m_top->config;

  js::config *fc_config = config->get("**/soc/fc");
  if (fc_config != NULL)
  {
    unsigned int fc_dbg_addr = config->get("**/fc_dbg_unit/base")->get_int();
    js::config *cache_config = config->get("**/fc_icache/base");
    unsigned int fc_icache_addr = -1;
    if (cache_config)
      fc_icache_addr = cache_config->get_int();
    auto cluster = std::make_shared<Target_fc>(config, fc_config, top, fc_dbg_addr, fc_icache_addr, fc_config->get("cluster_id")->get_int());
    clusters.push_back(cluster);
    std::shared_ptr<Target_core> core = cluster->get_core(0);
    _top->log.debug("Init FC Core %d:%d Thread Id %d\n", core->get_cluster_id(), core->get_core_id(), core->get_thread_id());
    cores.push_back(core);
    cores_from_threadid[core->get_thread_id()] = core;
    js::config *fc_base_config = config->get("**/soc/base");
    if (fc_base_config != NULL) {
      cluster->add_memory_wall(fc_base_config->get_int(), config->get("**/soc/size")->get_int());
    }
  }

  js::config *cluster_config = config->get("**/soc/cluster");
  if (cluster_config != NULL)
  {
    int nb_clusters = config->get("**/nb_cluster")->get_int();
    for (int i=0; i<nb_clusters; i++)
    {
      unsigned int cluster_base = 0x10000000;
      js::config *base_config = config->get("**/cluster/base");
      if (base_config != NULL)
        cluster_base = base_config->get_int();

      uint32_t this_cluster_base = cluster_base + 0x400000 * i;
      auto cluster = std::make_shared<Target_cluster>(config, cluster_config, top, this_cluster_base, this_cluster_base, i);
      cluster->add_memory_wall(this_cluster_base, 0x400000);
      clusters.push_back(cluster);
      for (int j=0; j<cluster->get_nb_core(); j++)
      {
        auto core = cluster->get_core(j);
        _top->log.debug("Init Cluster Core %d:%d Thread Id %d\n", core->get_cluster_id(), core->get_core_id(), core->get_thread_id());
        cores.push_back(core);
        cores_from_threadid[core->get_thread_id()] = core;
      }
    }
  }
  _top->log.debug("Finish target init\n");
}

Target::~Target()
{
}

void Target::flush()
{
  for (auto &cluster : this->clusters)
  {
    cluster->flush();
  }
}


void Target::clear_resume_all()
{
  for (auto &cluster : this->clusters)
  {
    cluster->clear_resume();
  }
}

void Target::prepare_resume_all(bool step)
{
  for (auto &thread : this->get_threads())
  {
    thread->prepare_resume(step);
  }
}

void Target::resume_all()
{
  m_top->log.detail("resume target\n");
  started = true;
  // first check if a core is still on a breakpoint. If it is then we don't actually resume.
  // Just fake it and let rsp that it stopped again
  for (auto &cluster : this->clusters)
  {
    if (!cluster->decide_resume()) {
      m_top->log.detail("resume aborted - core is on breakpoint\n");
      return;
    }
  }
  for (auto &cluster : this->clusters)
  {
    cluster->commit_resume();
  }
  for (auto &cluster : this->clusters)
  {
    cluster->resume();
  }
  // all the cores have resumed so clear the enable / disable history
  m_top->bkp->clear_history();
}

std::shared_ptr<Target_core> Target::check_stopped()
{
  m_top->log.debug("Check if target stopped\n");
  std::shared_ptr<Target_core> stopped_core = NULL;
  for (auto &cluster : this->clusters)
  {
    uint32_t cause;

    auto core = cluster->check_stopped(&cause);
    if (cause == EXC_CAUSE_BREAKPOINT) {
      stopped_core = core;
      break;
    } else if (!stopped_core && cause != EXC_CAUSE_NONE) {
      stopped_core = core;
    }
  }
  return stopped_core;
}

void Target::reinitialize()
{
  m_top->log.debug("Reinitialize target\n");
  for (auto &cluster: clusters) {
    cluster->init();
  }
}

void Target::update_power()
{
  for (auto &cluster: clusters) {
    cluster->update_power();
  }
}

void Target::mem_read(uint32_t addr, uint32_t length, char * buffer)
{

  if (!m_top->cable->access(false, addr, length, buffer))
    throw CableException("Error reading memory (addr: 0x%08x, len: %d)", addr, length);
  m_top->log.detail("read memory (addr: 0x%08x, len: %d)\n", addr, length);
}

void Target::mem_write(uint32_t addr, uint32_t length, char * buffer)
{

  if(!m_top->cable->access(true, addr, length, buffer))
    throw CableException("Error writing memory (addr: 0x%08x, len: %d)", addr, length);
  m_top->log.detail("write memory (addr: 0x%08x, len: %d)\n", addr, length);
}

bool Target::check_mem_access(uint32_t addr, uint32_t length)
{
  for (auto &cluster: clusters) {
    if (!cluster->get_power() && cluster->memory_wall_overlaps(addr, length))
      return false;
  }
  return true;
}

void Target::halt()
{
  if (!started) return;
  m_top->log.detail("stop target\n");
  started = false;
  for (auto &cluster: this->clusters)
  {
    cluster->halt();
  }
}