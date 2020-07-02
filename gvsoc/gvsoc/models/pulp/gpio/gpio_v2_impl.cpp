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
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "archi/gpio/gpio_v2.h"

class Gpio : public vp::component
{

public:

  Gpio(js::config *config);

  int build();
  void start();

private:

  static void gpio_sync(void *__this, bool value, int gpio);
  static vp::io_req_status_e req(void *__this, vp::io_req *req);

  vp::io_req_status_e paddir_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e padin_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e padout_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e inten_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e inttype0_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e inttype1_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e intstatus_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e gpioen_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e padcfg_req(int id, int reg_offset, int size, bool is_write, uint8_t *data);

  vp::io_slave in;

  std::vector<vp::wire_slave<bool> *> gpio_itf;
  vp::wire_master<int>  event_itf;
  vp::wire_master<bool> irq_itf;

  int nb_gpio;
  int soc_event;

  vp_gpio_apbgpio_paddir               r_paddir;
  vp_gpio_apbgpio_padin                r_padin;
  vp_gpio_apbgpio_padout               r_padout;
  vp_gpio_apbgpio_inten                r_inten;
  vp_gpio_apbgpio_inttype0             r_inttype0;
  vp_gpio_apbgpio_inttype1             r_inttype1;
  vp_gpio_apbgpio_intstatus            r_intstatus;
  vp_gpio_apbgpio_gpioen               r_gpioen;
  std::vector<vp_gpio_apbgpio_padcfg0> r_padcfg;
};



Gpio::Gpio(js::config *config)
: vp::component(config)
{

}





vp::io_req_status_e Gpio::paddir_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_paddir.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::padin_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_padin.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::padout_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  // TODO GPIO output should be propagated to pads. This should take gpioen into account only on some architecture
  uint32_t old_val = this->r_padout.get();

  this->r_padout.access(reg_offset, size, data, is_write);

  uint32_t new_val = this->r_padout.get();

  uint32_t changed = old_val ^ new_val;

  for (int i=0; i<this->nb_gpio; i++)
  {
    if ((changed >> i) & 1)
    {
      if (this->gpio_itf[i]->is_bound())
      {
        this->gpio_itf[i]->sync((new_val >> i) & 1);
      }
    }
  }


  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::inten_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_inten.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::inttype0_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_inttype0.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::inttype1_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_inttype1.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::intstatus_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_intstatus.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::gpioen_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_gpioen.access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Gpio::padcfg_req(int id, int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_padcfg[id].access(reg_offset, size, data, is_write);
  return vp::IO_REQ_OK;
}



vp::io_req_status_e Gpio::req(void *__this, vp::io_req *req)
{
  Gpio *_this = (Gpio *)__this;

  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  uint64_t is_write = req->get_is_write();

  _this->get_trace()->msg("GPIO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  if (reg_offset + size > 4) {
    _this->get_trace()->force_warning("Accessing 2 registers in one access\n");
    goto error;
  }

  switch (reg_id)
  {
    case GPIO_APBGPIO_PADDIR_OFFSET/4:
      err = _this->paddir_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_PADIN_OFFSET/4:
      err = _this->padin_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_PADOUT_OFFSET/4:
      err = _this->padout_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_INTEN_OFFSET/4:
      err = _this->inten_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_INTTYPE0_OFFSET/4:
      err = _this->inttype0_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_INTTYPE1_OFFSET/4:
      err = _this->inttype1_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_INTSTATUS_OFFSET/4:
      err = _this->intstatus_req(reg_offset, size, is_write, data);
      break;

    case GPIO_APBGPIO_GPIOEN_OFFSET/4:
      err = _this->gpioen_req(reg_offset, size, is_write, data);
      break;

  }

  if (reg_id >= GPIO_APBGPIO_PADCFG0_OFFSET/4 && reg_id <= GPIO_APBGPIO_PADCFG7_OFFSET/4)
    err = _this->padcfg_req(reg_id - GPIO_APBGPIO_PADCFG0_OFFSET/4, reg_offset, size, is_write, data);

  if (err != vp::IO_REQ_OK)
    goto error; 


  return vp::IO_REQ_OK;

error:
  _this->get_trace()->force_warning("GPIO invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}



void Gpio::gpio_sync(void *__this, bool value, int gpio)
{
  Gpio *_this = (Gpio *)__this;

  _this->get_trace()->msg("Received new gpio value (gpio: %d, value: %d)\n", gpio, value);

  unsigned int old_val = (_this->r_padin.get() >> gpio) & 1;
  _this->r_padin.set((_this->r_padin.get() & ~(1<<gpio)) | (value << gpio));

  if (((_this->r_inten.get() >> gpio) & 1) && old_val != value)
  {
    // The inttype is coded with 2 bits per gpio
    // Extract it for our gpio in the proper register
    int reg_id = gpio  / 16;
    int bit = gpio % 16;
    uint32_t inttype_reg = reg_id ? _this->r_inttype1.get() : _this->r_inttype0.get();
    int inttype = (inttype_reg >> (2*bit)) & 0x3;

    // The interrupt should be raised if the edge is matching the mode
    // (0: falling, 1: rising, 2: both)
    int edge = (old_val == 0 && (inttype == 1 || inttype == 2)) ||
      (old_val == 1 && (inttype == 0 || inttype == 2));

    if (edge)
    {
      _this->r_intstatus.set(_this->r_intstatus.get() | (1 << gpio));

      _this->get_trace()->msg("Raising interrupt (intstatus: 0x%x)\n", _this->r_intstatus.get());

      if (_this->event_itf.is_bound())
        _this->event_itf.sync(_this->soc_event);

      if (_this->irq_itf.is_bound())
        _this->irq_itf.sync(true);
    }
  }
}



int Gpio::build()
{
  this->in.set_req_meth(&Gpio::req);
  this->new_slave_port("input", &this->in);
  this->new_master_port("event", &this->event_itf);
  this->new_master_port("irq", &this->irq_itf);

  this->soc_event = this->get_js_config()->get_child_int("soc_event");
  this->nb_gpio = this->get_js_config()->get_int("nb_gpio");

  for (int i=0; i<this->nb_gpio; i++)
  {
    vp::wire_slave<bool> *itf = new vp::wire_slave<bool>();
    itf->set_sync_meth_muxed(&Gpio::gpio_sync, i);
    new_slave_port("gpio" + std::to_string(i), itf);
    this->gpio_itf.push_back(itf);
  }

  this->new_reg("paddir", &this->r_paddir, 0);
  this->new_reg("padin", &this->r_padin, 0);
  this->new_reg("padout", &this->r_padout, 0);
  this->new_reg("inten", &this->r_inten, 0);
  this->new_reg("inttype0", &this->r_inttype0, 0);
  this->new_reg("inttype1", &this->r_inttype1, 0);
  this->new_reg("intstatus", &this->r_intstatus, 0);
  this->new_reg("gpioen", &this->r_gpioen, 0);

  this->r_padcfg.resize(this->nb_gpio/4);

  for (int i=0; i<this->nb_gpio/4; i++)
  {
    this->new_reg("padcfg" + std::to_string(i), &this->r_padcfg[i], 0);
  }

  return 0;
}



void Gpio::start()
{
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Gpio(config);
}
