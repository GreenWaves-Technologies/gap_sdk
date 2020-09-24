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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <vp/itf/wire.hpp>
#include <string.h>
#include "archi/maestro/maestro_v2_new.h"

#define NB_PICL_SLAVES 16

class pmu;

class pmu_picl_slave
{
public:
  pmu_picl_slave(pmu *top) : top(top) {}
  virtual void handle_req(int addr, bool is_write, uint16_t pwdata);

protected:
  pmu *top;
};

class pmu_scu_seq_step
{
public:
  unsigned int cs;
  unsigned int addr;
  uint8_t data;
};

class pmu_scu_seq
{
public:
  pmu_scu_seq(): nb_step(0) {}

  void add_step(unsigned int cs, unsigned int addr, uint8_t data)
  {
    this->steps[nb_step].cs = cs;
    this->steps[nb_step].addr = addr;
    this->steps[nb_step].data = data;
    nb_step++;
  }

  pmu_scu_seq_step steps[32];

  int nb_step;
};

class pmu_icu_state
{
public:
  int supply;
  int clock;
  int regulator;
};

class pmu_icu : public pmu_picl_slave
{
public:
  pmu_icu(pmu *top, int index);
  void set_state(int index, int supply, int clock, int regulator);

  void handle_req(int addr, bool is_write, uint16_t pwdata);
  void icu_ctrl_req(bool is_write, uint16_t pwdata);
  void icu_mode_req(bool is_write, uint16_t pwdata);
  void island_mode_req(bool is_write, uint16_t pwdata);
  void dmu_mode_req(bool is_write, uint16_t pwdata);

private:
  pmu *top;
  vp::wire_master<bool>  reset;
  pmu_icu_state states[16];
  int index;
};

class pmu_wiu : public pmu_picl_slave
{
public:
  pmu_wiu(pmu *top);
  void handle_req(int addr, bool is_write, uint16_t pwdata);
  void set_irq(unsigned int irq_mask);
  void reset(bool active);

  vp::reg_8    r_ifr[2];
  vp::reg_8    r_ispmr[2];
  vp::reg_8    r_icr[16];

private:

  void ispmr_req(int index, bool is_write, uint16_t pwdata);
  void ifr_req(int index, bool is_write, uint16_t pwdata);
  void icr_req(int index, bool is_write, uint16_t pwdata);
  void check_state();

  pmu *top;
  unsigned int pending_irqs;

  int nb_irq_regs;

};

class pmu : public vp::component
{

  friend class pmu_icu;
  friend class pmu_wiu;

public:

  pmu(js::config *config);

  int build();
  void start();
  void reset(bool active);
  void picl_reply();
  void picl_set_rdata(uint8_t data);
  bool is_busy() { return this->active_sequence != -1; }

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

  void exec_sequence(int seq);

private:

  static void wakeup_sync(void *__this, bool wakeup);
  static void wakeup_seq_sync(void *__this, unsigned int seq);

  vp::io_req_status_e pctrl_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e prdata_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e dlc_sr_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e dlc_imr_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e dlc_ifr_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e dlc_ioifr_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e dlc_idifr_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e dlc_imcifr_req(int reg_offset, int size, bool is_write, uint8_t *data);

  void check_state();

  static void sequence_event_handle(void *__this, vp::clock_event *event);
  static void ref_clock_reg(component *_this, component *clock);

  vp::trace     trace;
  vp::io_slave in;

  pmu_scu_seq boot_sequence;
  pmu_scu_seq sequences[16];

  pmu_picl_slave *picl_slaves[NB_PICL_SLAVES];
  pmu_wiu *wiu;

  int active_sequence;
  int active_sequence_step;
  bool pending_access;
  int wakeup_seq;

  int nb_interrupts;
  int nb_icu;

  vp::clock_event *sequence_event;

  vp::wire_master<int>  event_itf;
  vp::wire_slave<bool>          wakeup_itf;
  vp::wire_slave<unsigned int>  wakeup_seq_itf;
  vp::clk_slave    ref_clock_itf;
  vp::clock_engine *ref_clock;

  vp::reg_32   r_dlc_pctrl;
  vp::reg_32   r_dlc_rdata;
};

pmu::pmu(js::config *config)
: vp::component(config)
{

}


pmu_wiu::pmu_wiu(pmu *top)
: pmu_picl_slave(top), top(top)
{
  this->top->new_reg("wiu_ispmr_0", &this->r_ispmr[0], 0xFF);
  this->top->new_reg("wiu_ispmr_1", &this->r_ispmr[1], 0xC0);
  this->top->new_reg("wiu_ifr_0", &this->r_ifr[0], 0x00);
  this->top->new_reg("wiu_ifr_1", &this->r_ifr[1], 0x00);
  for (int i=0; i<16; i++)
  {
    this->top->new_reg("wiu_icr_" + std::to_string(i), &this->r_icr[i], 0x00);
  }
  this->nb_irq_regs = 2;
}

void pmu_wiu::ispmr_req(int index, bool is_write, uint16_t pwdata)
{
  if (is_write)
  {
    this->top->trace.msg("Writing WIU ISPMR (index: %d, value: 0x%2.2x)\n", index, pwdata);
    this->r_ispmr[index].set(pwdata);
    this->check_state();
  }
  else
  {
    top->picl_set_rdata(this->r_ispmr[index].get());
  }

  top->picl_reply();
}

void pmu_wiu::ifr_req(int index, bool is_write, uint16_t pwdata)
{
  if (is_write)
  {
    this->top->trace.msg("Writing WIU IFR (index: %d, value: 0x%2.2x)\n", index, pwdata);
    this->r_ifr[index].set(pwdata);
    this->check_state();
  }
  else
  {
    top->picl_set_rdata(this->r_ifr[index].get());
  }

  top->picl_reply();
}

void pmu_wiu::icr_req(int index, bool is_write, uint16_t pwdata)
{
  if (is_write)
  {
    this->top->trace.msg("Writing WIU ICR (index: %d, value: 0x%2.2x)\n", index, pwdata);
    this->r_icr[index].set(pwdata);
    this->check_state();
  }
  else
  {
    top->picl_set_rdata(this->r_icr[index].get());
  }

  top->picl_reply();
}

void pmu_wiu::handle_req(int addr, bool is_write, uint16_t pwdata)
{
  if (addr <= MAESTRO_WIU_ISPMR_1_OFFSET)
  {
    this->ispmr_req(addr - MAESTRO_WIU_ISPMR_0_OFFSET, is_write, pwdata);
  }
  else if (addr <= MAESTRO_WIU_IFR_1_OFFSET)
  {
    this->ifr_req(addr - MAESTRO_WIU_IFR_0_OFFSET, is_write, pwdata);
  }
  else if (addr <= MAESTRO_WIU_ICR_15_OFFSET)
  {
    this->icr_req(addr - MAESTRO_WIU_ICR_0_OFFSET, is_write, pwdata);
  }
  else
  {
    this->top->warning.warning("Accessing invalid WIU register (addr: 0x%x)\n", addr);
  }
}

void pmu_wiu::set_irq(unsigned int irq_mask)
{
  unsigned int pending_irqs = this->pending_irqs;
  this->pending_irqs = irq_mask;

  this->top->trace.msg("Received interrupt (mask: 0x%x)\n", irq_mask);

  for (int i=0; i<16; i++)
  {
    if (((pending_irqs >> i) & 1) == 0 && ((irq_mask >> i) & 1) == 1)
    {
      this->top->trace.msg("Detected IRQ raising edge (irq: %d)\n", i);
      this->r_ifr[i/8].set(r_ifr[i/8].get() | (1<<(i%8)));
    }
  }

  this->check_state();
}

void pmu_wiu::check_state()
{
  if (!this->top->is_busy())
  {
    for (int i=0; i<this->nb_irq_regs; i++)
    {
      unsigned int irq_mask = this->r_ifr[i].get() & ~this->r_ispmr[i].get();
      if (irq_mask)
      {
        for (int j=0; j<8; j++)
        {
          if ((irq_mask >> j) & 1)
          {
            int irq = i*8 + j;
            this->top->trace.msg("Detected active interrupt (irq: %d)\n", irq);
            this->top->exec_sequence(this->r_icr[irq].get());
            return;
          }
        }
      }
    }
  }
}

void pmu_wiu::reset(bool active)
{
  if (active)
  {
    this->pending_irqs = 0;
  }
}



void pmu::sequence_event_handle(void *__this, vp::clock_event *event)
{
  pmu *_this = (pmu *)__this;

  if (_this->active_sequence != -1)
  {
    if (_this->active_sequence_step != -1)
    {
      _this->trace.msg("Executing sequence step (sequence: %d, step: %d)\n", _this->active_sequence, _this->active_sequence_step);

      pmu_scu_seq *seq = &_this->sequences[_this->active_sequence];
      pmu_scu_seq_step *step = &seq->steps[_this->active_sequence_step];
      maestro_dlc_pctrl_t reg;

      reg.start = 1;
      reg.paddr = step->addr | (step->cs << 5);
      reg.dir = 0;
      reg.pwdata = step->data;

      _this->pending_access = true;
      _this->active_sequence_step++;
      if (_this->active_sequence_step == seq->nb_step)
        _this->active_sequence_step = -1;

      _this->pctrl_req(0, 32, true, (uint8_t *)&reg.raw);
    }
    else
    {
      _this->trace.msg("Finished sequence (sequence: %d)\n", _this->active_sequence);
      _this->active_sequence = -1;
    }
  }
}

void pmu::picl_set_rdata(uint8_t data)
{
  this->r_dlc_rdata.set(data);
}

void pmu::check_state()
{
  if (!this->sequence_event->is_enqueued() && this->active_sequence != -1 && !this->pending_access)
  {
    this->ref_clock->enqueue(this->sequence_event, 1);
  }
}

void pmu::exec_sequence(int seq)
{
  this->trace.msg("Executing sequence (sequence: %d)\n", seq);
  this->active_sequence = seq;
  this->active_sequence_step = 0;
  this->check_state();
}

vp::io_req_status_e pmu::pctrl_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
  {
    this->r_dlc_pctrl.write(reg_offset, size, data);
    maestro_dlc_pctrl_t reg { .raw=this->r_dlc_pctrl.get()};

    if (reg.start)
    {
      unsigned int cs = reg.paddr >> 5;
      unsigned int addr = reg.paddr & 0x1f;
      if (reg.dir == 0)
        this->trace.msg("Generating PICL write access (cs: %d, addr: 0x%x, value: 0x%x)\n", cs, addr, reg.pwdata);
      else
        this->trace.msg("Generating PICL read access (cs: %d, addr: 0x%x)\n", cs, addr);

      if (cs >= NB_PICL_SLAVES || this->picl_slaves[cs] == NULL)
      {
        this->warning.force_warning("Trying to access invalid PICL slave (cs: %d)\n", cs);
        return vp::IO_REQ_INVALID;
      }

      pmu_picl_slave *slave = this->picl_slaves[cs];
      slave->handle_req(addr, reg.dir == 0, reg.pwdata);
    }
  }
  else
  {
    this->r_dlc_pctrl.read(reg_offset, size, data);
  }

  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::prdata_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::dlc_sr_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::dlc_imr_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::dlc_ifr_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::dlc_ioifr_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::dlc_idifr_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}

vp::io_req_status_e pmu::dlc_imcifr_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::IO_REQ_OK;
}


void pmu_icu::set_state(int index, int supply, int clock, int regulator)
{
  this->states[index].supply = supply;
  this->states[index].clock = clock;
  this->states[index].regulator = regulator;
}

void pmu_icu::icu_ctrl_req(bool is_write, uint16_t pwdata)
{
  this->top->trace.msg("Switching to new state (state: %d)\n", pwdata);

  if (pwdata >= 16)
  {
    this->top->warning.force_warning("Trying to set invalid ICU state (state: %d)\n", pwdata);
    return;
  }

  pmu_icu_state *state = &this->states[pwdata];
  if (state->supply != MAESTRO_ICU_SUPPLY_ON)
  {
    this->top->trace.msg("Shutting down island (index: %d)\n", this->index);
    if (this->reset.is_bound())
    {
      this->reset.sync(1);
    }
  }
  else
  {
    this->top->trace.msg("Powering-up island (index: %d)\n", this->index);
    if (this->reset.is_bound())
    {
      this->reset.sync(0);
    }
  }

  top->picl_reply();
}

void pmu_icu::icu_mode_req(bool is_write, uint16_t pwdata)
{
  top->picl_reply();
}

void pmu_icu::island_mode_req(bool is_write, uint16_t pwdata)
{
  top->picl_reply();
}

void pmu_icu::dmu_mode_req(bool is_write, uint16_t pwdata)
{
  top->picl_reply();
}

void pmu_icu::handle_req(int addr, bool is_write, uint16_t pwdata)
{
  if (addr == MAESTRO_ICU_CTRL_OFFSET)
  {
    this->icu_ctrl_req(is_write, pwdata);
  }
  else if (addr == MAESTRO_ICU_MODE_OFFSET)
  {
    this->icu_mode_req(is_write, pwdata);
  }
  else if (addr == MAESTRO_ISLAND_MODE_OFFSET)
  {
    this->island_mode_req(is_write, pwdata);
  }
  else if (addr == MAESTRO_DMU_MODE_OFFSET)
  {
    this->dmu_mode_req(is_write, pwdata);
  }
  else
  {
    this->top->warning.force_warning("Accessing invalid ICU register (addr: 0x%x)\n", addr);
  }
}


vp::io_req_status_e pmu::req(void *__this, vp::io_req *req)
{
  pmu *_this = (pmu *)__this;

  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("PMU access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  switch (reg_id) {
    case MAESTRO_DLC_PCTRL_OFFSET/4  : err = _this->pctrl_req(reg_offset, size, is_write, data); break;
    case MAESTRO_PRDATA_OFFSET/4     : err = _this->prdata_req(reg_offset, size, is_write, data); break;
    case MAESTRO_DLC_SR_OFFSET/4     : err = _this->dlc_sr_req(reg_offset, size, is_write, data); break;
    case MAESTRO_DLC_IMR_OFFSET/4    : err = _this->dlc_imr_req(reg_offset, size, is_write, data); break;
    case MAESTRO_DLC_IFR_OFFSET/4    : err = _this->dlc_ifr_req(reg_offset, size, is_write, data); break;
    case MAESTRO_DLC_IOIFR_OFFSET/4  : err = _this->dlc_ioifr_req(reg_offset, size, is_write, data); break;
    case MAESTRO_DLC_IDIFR_OFFSET/4  : err = _this->dlc_idifr_req(reg_offset, size, is_write, data); break;
    case MAESTRO_DLC_IMCIFR_OFFSET/4 : err = _this->dlc_imcifr_req(reg_offset, size, is_write, data); break;
  }

  if (err != vp::IO_REQ_OK)
    goto error; 


  return vp::IO_REQ_OK;

error:
  _this->warning.force_warning("PMU invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}


pmu_icu::pmu_icu(pmu *top, int index)
: pmu_picl_slave(top), top(top), index(index)
{
  top->new_master_port("icu" + std::to_string(index) + "_reset", &this->reset);

  for (int i=0; i<16; i++)
  {
    this->set_state(i, MAESTRO_ICU_SUPPLY_EXT, MAESTRO_ICU_CLK_FNONE, MAESTRO_ICU_REGU_OFF);
  }
}

void pmu_picl_slave::handle_req(int addr, bool is_write, uint16_t pwdata)
{
  // Default behavior for all slaves is to quickly reply 0. Any slave can then
  // overload this behvior
  top->picl_reply();
}

void pmu::picl_reply()
{
  // Notify in the pctrl register that the access is done
  ((maestro_dlc_pctrl_t *)this->r_dlc_pctrl.get_bytes())->start = 0;

  // 
  this->pending_access = false;

  this->check_state();
}

void pmu::wakeup_sync(void *__this, bool wakeup)
{
  pmu *_this = (pmu *)__this;
  _this->trace.msg("Sync wakeup signal (wakeup: %d)\n", wakeup);
  if (wakeup)
    _this->wiu->set_irq(1<<(_this->wakeup_seq*2));
}

void pmu::wakeup_seq_sync(void *__this, unsigned int seq)
{
  pmu *_this = (pmu *)__this;
  _this->trace.msg("Sync wakeup sequence signal (sequence: %d)\n", seq);
  _this->wakeup_seq = seq;
}

void pmu::ref_clock_reg(component *__this, component *clock)
{
  pmu *_this = (pmu *)__this;
  _this->ref_clock = (vp::clock_engine *)clock;
}

int pmu::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&pmu::req);
  new_slave_port("input", &in);

  this->new_reg("pctrl", &this->r_dlc_pctrl, 0x00000000);
  this->new_reg("rdata", &this->r_dlc_rdata, 0x00000000);

  new_master_port("event", &event_itf);

  this->wakeup_itf.set_sync_meth(&pmu::wakeup_sync);
  new_slave_port("wakeup", &this->wakeup_itf);

  this->wakeup_seq_itf.set_sync_meth(&pmu::wakeup_seq_sync);
  new_slave_port("wakeup_seq", &this->wakeup_seq_itf);

  this->ref_clock_itf.set_reg_meth(&pmu::ref_clock_reg);
  new_slave_port("ref_clock", &this->ref_clock_itf);

  sequence_event = event_new(pmu::sequence_event_handle);

  this->nb_interrupts = this->get_js_config()->get_child_int("nb_interrupts");
  this->nb_icu = this->get_js_config()->get_child_int("nb_icu");

  int wiu_offset = this->get_js_config()->get_child_int("regmap/wiu/offset");

  for (int i=0; i<NB_PICL_SLAVES; i++)
  {
    this->picl_slaves[i] == NULL;
  }

  this->wiu = new pmu_wiu(this);
  this->picl_slaves[wiu_offset] = this->wiu;

  for (int i=0; i<this->nb_icu; i++)
  {
    pmu_icu *icu = new pmu_icu(this, i);
    this->picl_slaves[i+2] = icu;
  }

  return 0;
}

void pmu::reset(bool active)
{
  if (active)
  {
    this->active_sequence = -1;
    this->pending_access = false;
    this->wakeup_seq = 0;

    // These are the sequences corresponding to the interrupts
    js::config *icrs_config = this->get_js_config()->get("icrs");
    if (icrs_config != NULL)
    {
      for (int i=0; i<this->nb_interrupts; i++)
      {
        int icr_value = icrs_config->get_elem(i)->get_int();
        this->trace.msg("Setting initial ICR value (icr: %d, value: 0x%x)\n", i, icr_value);
        this->wiu->r_icr[i].set(icr_value);
      }
    }
  }

  this->wiu->reset(active);
}

void pmu::start()
{
  js::config *sequences = this->get_js_config()->get("regmap/sequences");
  for (auto x: sequences->get_childs())
  {
    js::config *seq_config = x.second;
    int id = seq_config->get_child_int("id");
    js::config *cmd_config = seq_config->get("commands");
    if (cmd_config != NULL)
    {
      for (int i=0; i<cmd_config->get_size(); i++)
      {
        js::config *step_config = cmd_config->get_elem(i);
        int cs = step_config->get_elem(0)->get_int();
        unsigned int offset = step_config->get_elem(1)->get_int();
        unsigned int value = step_config->get_elem(2)->get_int();
        std::string desc = step_config->get_elem(3)->get_str();
        this->trace.msg("Recording sequence step (sequence: %d, cs: %d, offset: 0x%x, value: 0x%2.2x, desc: %s)\n", id, cs, offset, value, desc.c_str());
        if (id == -1)
          this->boot_sequence.add_step(cs, offset, value);
        else
          this->sequences[id].add_step(cs, offset, value);
      }
    }
  }

  for (int i=0; i<this->nb_icu; i++)
  {
    pmu_icu *icu = (pmu_icu *)this->picl_slaves[i+2];

    js::config *icu_states_config = this->get_js_config()->get("icu_states");
    if (icu_states_config != NULL)
    {
      for (int j=0; j<icu_states_config->get_size(); j++)
      {
        js::config *state = icu_states_config->get_elem(j);
        int supply = state->get_elem(0)->get_int();
        int freq = state->get_elem(1)->get_int();
        int regu = state->get_elem(2)->get_int();
        this->trace.msg("Recording ICU state (state: %d, supply: %d, freq: %d, regu: %d)\n", j, supply, freq, regu);
        icu->set_state(j, supply,  freq, regu);
      }
    }
  }

}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new pmu(config);
}
