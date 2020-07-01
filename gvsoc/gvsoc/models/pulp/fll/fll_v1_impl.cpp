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
#include <vp/itf/clock.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "archi/fll/fll_v1.h"

#define REG_MASK(offset,size) (((1<<(size))-1) << (offset))
#define REG_GET(fullValue,offset,size) (((fullValue) & REG_MASK(offset, size)) >> offset)

class fll : public vp::component
{

public:

  fll(js::config *config);

  int build();
  void start();
  void reset(bool active);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::io_req_status_e status_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e conf1_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e conf2_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e integrator_req(int reg_offset, int size, bool is_write, uint8_t *_data);
  void fll_check_state();

  double get_dco_frequency(int dco_input);
  void handle_req();

  static void ref_clock_sync(void *__this, bool value);
  static void ref_clock_set_frequency(void *__this, int64_t frequency);

  uint32_t status_reg_reset;
  uint32_t conf1_reg_reset;
  uint32_t conf2_reg_reset;
  uint32_t integrator_reg_reset;

  fll_reg_status_t     status_reg;
  fll_reg_conf1_t      conf1_reg;
  fll_reg_conf2_t      conf2_reg;
  fll_reg_integrator_t integrator_reg;

  double dco_freq;

  vp::clock_slave ref_clock_itf;
  vp::clock_master fll_clock_itf;
  vp::io_slave in;

  int nb_stability_cycles;
  int locked;
  int dco_input;

  vp::io_req *first_pending;
  vp::io_req *last_pending;
};

fll::fll(js::config *config)
: vp::component(config)
{

}

void fll::ref_clock_set_frequency(void *__this, int64_t frequency)
{
}

void fll::ref_clock_sync(void *__this, bool value)
{
  fll *_this = (fll *)__this;

  if (value == false)
    return;

  _this->get_trace()->msg("Received ref clock\n");

  // DCO freq is the number of pulses so we have to divide by 2
  _this->status_reg.actual_mult_factor = _this->dco_freq * 1000000 / 32768 / 2;

  if (_this->conf1_reg.mode == 1)
  {
    int delta = _this->conf1_reg.mult_factor - _this->status_reg.actual_mult_factor;
    int delta_ext = delta << 10;
    int gain = _this->conf2_reg.loop_gain;
    int delta_ext_amp = gain < 10 ? delta_ext >> gain : 0;
    int integrator = (_this->integrator_reg.state_int_part << 10) | _this->integrator_reg.state_fract_part;
    int integrator_output = integrator + delta_ext_amp;

    _this->get_trace()->msg("Adapting DCO input (factor: %d, actual_factor: %d)\n", _this->conf1_reg.mult_factor, _this->status_reg.actual_mult_factor);

    _this->integrator_reg.state_int_part = integrator_output >> 10;
    _this->integrator_reg.state_fract_part = integrator_output;

    _this->dco_input = _this->integrator_reg.state_int_part;

  }

  _this->fll_check_state();

  _this->handle_req();
}

double fll::get_dco_frequency(int dco_input)
{
  double result;
  if (dco_input < 219)
    result = 0.000001;
  else if (dco_input < 512)
    result = 21.897 - 33.1341 - 0.752*dco_input + 0.003674*pow(dco_input, 2);
  else
    result = -642.570 + 2.590*dco_input - 0.000445*pow(dco_input, 2);

  return result;
}

void fll::fll_check_state()
{
  this->dco_freq = this->get_dco_frequency(this->dco_input);

  this->get_trace()->msg("Setting DCO frequency (frequency: %f MHz, dco_in: %d)\n", this->dco_freq, this->dco_input);

  int frequency = (int)(this->dco_freq / (1 << this->conf1_reg.clock_out_divider) * 1000000);

  if (this->conf1_reg.mode == 1 && this->conf1_reg.output_lock_enable)
  {
    // Locked mode, check if the fll output must be asserted or de-asserted
    int delta = this->conf1_reg.mult_factor - this->status_reg.actual_mult_factor;
    if (delta < 0)
      delta = -delta;

    if (this->locked)
    {
      if (delta > this->conf2_reg.lock_tolerance)
      {
        this->nb_stability_cycles++;
        this->get_trace()->msg("FLL frequency outside tolerance, increasing unstable cycles (unstable_cycles: %d, deassert_cycles: %d)\n", this->nb_stability_cycles, this->conf2_reg.de_assert_cycles);
        if (this->nb_stability_cycles == this->conf2_reg.de_assert_cycles)
        {
          this->get_trace()->msg("Unlocking FLL\n");
          this->locked = 0;
          this->nb_stability_cycles = 0;
        }
      }
    }
    else
    {
      if (delta <= this->conf2_reg.lock_tolerance)
      {
        this->nb_stability_cycles++;
        this->get_trace()->msg("FLL frequency within tolerance, increasing stable cycles (stable_cycles: %d, assert_cycles: %d)\n", this->nb_stability_cycles, this->conf2_reg.assert_cycles);
        if (this->nb_stability_cycles == this->conf2_reg.assert_cycles)
        {
          this->get_trace()->msg("Locking FLL\n");
          this->locked = 1;
          this->nb_stability_cycles = 0;
        }
      }
    }

    if (!this->locked)
      frequency = 0;
  }
  else
  {
    // Non-locked mode, just keep the frequency out of the oscillator
  }


  this->get_trace()->msg("Setting new frequency (frequency: %d Hz)\n", frequency);
  this->fll_clock_itf.set_frequency(frequency);
}

//void Fll::propagate_frequency() {
//  if (div_factor != 0) {
//    int div = div_factor > 1 ? (1 << (div_factor - 1)) : 1;
//    int newFreq = refClock * mult_factor / div;
//    if (newFreq == 0) {
//      gv_trace_dumpWarning(&warning, "Trying to set frequency 0\n");
//      return;
//    }
//    gv_trace_dumpMsg(&trace, "Setting new frequency (frequency: %d Hz)\n", newFreq);
//    if (clockItf) clockItf->setFreq(newFreq);
//  }
//}

vp::io_req_status_e fll::status_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
    return vp::io_req_status_e::IO_REQ_INVALID;

  memcpy(data, &(((uint8_t *)&this->status_reg.raw)[reg_offset]), size);

  return vp::io_req_status_e::IO_REQ_OK;
}

vp::io_req_status_e fll::conf1_req(int reg_offset, int size, bool is_write, uint8_t *data) {
  if (!is_write)
  {
    memcpy(data, &(((uint8_t *)&this->conf1_reg.raw)[reg_offset]), size);
  }
  else
  {
    memcpy(&(((uint8_t *)&this->conf1_reg.raw)[reg_offset]), data, size);

    this->get_trace()->msg("Setting configuration 1 register (raw: 0x%x, mode: %d, lock: %d, div: %d, dco: %d, factor: %d)\n", this->conf1_reg.raw, this->conf1_reg.mode, this->conf1_reg.output_lock_enable, this->conf1_reg.clock_out_divider, this->conf1_reg.dco_input, this->conf1_reg.mult_factor);

    if (this->conf1_reg.mode == 0)
      this->dco_input = this->conf1_reg.dco_input;

    // Writing to conf1 reg, whatever the mode, is unlocking the fll
    this->locked = 0;

    this->fll_check_state();
  }
  return vp::io_req_status_e::IO_REQ_OK;
}

vp::io_req_status_e fll::conf2_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (!is_write)
  {
    memcpy(data, &(((uint8_t *)&this->conf2_reg)[reg_offset]), size);
  }
  else
  {
    memcpy(&(((uint8_t *)&this->conf2_reg)[reg_offset]), data, size);

    this->get_trace()->msg("Setting configuration 2 register (raw: 0x%x, dith: %d, mode: %d, clkSel: %d, lockTol: %d, nbStable: %d, nbUnstable: %d, gain: %d)\n",
      this->conf2_reg.raw, this->conf2_reg.dithering, this->conf2_reg.open_loop, this->conf2_reg.config_clock_sel, this->conf2_reg.lock_tolerance, this->conf2_reg.assert_cycles, this->conf2_reg.de_assert_cycles, this->conf2_reg.loop_gain);

    this->fll_check_state();
  }
  return vp::io_req_status_e::IO_REQ_OK;
}

vp::io_req_status_e fll::integrator_req(int reg_offset, int size, bool is_write, uint8_t *data) {
  if (!is_write)
  {
    memcpy(data, &(((uint8_t *)&this->integrator_reg.raw)[reg_offset]), size);
  }
  else
  {
    memcpy(&(((uint8_t *)&this->integrator_reg.raw)[reg_offset]), data, size);

    this->get_trace()->msg("Setting integrator register (raw: 0x%x, integrator_int: %d, integrator_fract: %d)\n",
      this->integrator_reg.raw, this->integrator_reg.state_int_part, this->integrator_reg.state_fract_part);

    // Writing to integrator register is unlocking the FLL only in close loop
    if (this->conf1_reg.mode == 1)
      this->locked = 0;

    this->fll_check_state();
  }
  return vp::io_req_status_e::IO_REQ_OK;
}

void fll::handle_req()
{
  vp::io_req *req = this->first_pending;

  if (req)
  {
    this->first_pending = req->get_next();

    vp::io_req_status_e err = vp::IO_REQ_INVALID;
    uint64_t offset = req->get_addr();
    uint8_t *data = req->get_data();
    uint64_t size = req->get_size();
    bool is_write = req->get_is_write();

    this->get_trace()->msg("FLL access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

    int reg_id = offset / 4;
    int reg_offset = offset % 4;

    if (reg_offset + size > 4) {
      this->get_trace()->warning("Accessing 2 registers in one access\n");
      err = vp::IO_REQ_INVALID;
      goto end;
    }

    switch (reg_id) {
      case FLL_STATUS_OFFSET/4:     err = this->status_req(reg_offset, size, is_write, data); break;
      case FLL_CONF1_OFFSET/4:      err = this->conf1_req(reg_offset, size, is_write, data); break;
      case FLL_CONF2_OFFSET/4:      err = this->conf2_req(reg_offset, size, is_write, data); break;
      case FLL_INTEGRATOR_OFFSET/4: err = this->integrator_req(reg_offset, size, is_write, data); break;
    }

    // TODO we should be able to report an error in asynchronous requests

end:
    if (err != vp::IO_REQ_OK)
      this->get_trace()->msg("FLL invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

    req->resp_port->resp(req);
  }
}



vp::io_req_status_e fll::req(void *__this, vp::io_req *req)
{
  fll *_this = (fll *)__this;

  if (_this->first_pending)
    _this->last_pending->set_next(req);
  else
    _this->first_pending = req;

  _this->last_pending = req;
  req->set_next(NULL);

  return vp::IO_REQ_PENDING;
}



int fll::build()
{
  in.set_req_meth(&fll::req);
  new_slave_port("input", &in);

  ref_clock_itf.set_sync_meth(&fll::ref_clock_sync);
  ref_clock_itf.set_set_frequency_meth(&fll::ref_clock_set_frequency);
  new_slave_port("ref_clock", &ref_clock_itf);

  new_master_port("clock_out", &fll_clock_itf);

  this->status_reg_reset = get_js_config()->get("regmap/status/reset")->get_int();
  this->conf1_reg_reset = get_js_config()->get("regmap/conf1/reset")->get_int();
  this->conf2_reg_reset = get_js_config()->get("regmap/conf2/reset")->get_int();
  this->integrator_reg_reset = get_js_config()->get("regmap/integrator/reset")->get_int();

  return 0;
}

void fll::start()
{
}

void fll::reset(bool active)
{
  if (active)
  {
    this->status_reg.raw      = this->status_reg_reset;
    this->conf1_reg.raw       = this->conf1_reg_reset;
    this->conf2_reg.raw       = this->conf2_reg_reset;
    this->integrator_reg.raw  = this->integrator_reg_reset;
    this->nb_stability_cycles = 0;
    this->locked              = 0;
    this->dco_input = this->conf1_reg.dco_input;
    this->first_pending = NULL;
    this->last_pending = NULL;
  }
  else
  {
    this->fll_check_state();
  }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new fll(config);
}
