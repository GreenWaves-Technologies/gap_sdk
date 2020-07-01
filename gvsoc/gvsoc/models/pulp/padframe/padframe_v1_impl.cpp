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
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/jtag.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/hyper.hpp>
#include <vp/itf/clock.hpp>
#include <vp/itf/i2c.hpp>
#include <vp/itf/i2s.hpp>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class Pad_group
{
public:
  Pad_group(std::string name) : name(name) {}

  std::string name;
};

class Qspim_group : public Pad_group
{
public:
  Qspim_group(std::string name) : Pad_group(name) {}
  vp::qspim_slave slave;
  vp::trace data_0_trace;
  vp::trace data_1_trace;
  vp::trace data_2_trace;
  vp::trace data_3_trace;
  int nb_cs;
  vector<vp::trace *> cs_trace;
  vector<vp::qspim_master *> master;
  vector<vp::wire_master<bool> *> cs_master;
  int active_cs;
};

class Cpi_group : public Pad_group
{
public:
  Cpi_group(std::string name) : Pad_group(name) {}
  vp::cpi_slave slave;
  vp::cpi_master master;
  vp::trace pclk_trace;
  vp::trace href_trace;
  vp::trace vsync_trace;
  vp::trace data_trace;
};

class Jtag_group : public Pad_group
{
public:
  Jtag_group(std::string name) : Pad_group(name) {}
  vp::jtag_slave pad_slave;
  vp::jtag_slave chip_slave;
  vp::jtag_master master;
  vp::trace tck_trace;
  vp::trace tdi_trace;
  vp::trace tms_trace;
  vp::trace trst_trace;
  vp::trace tdo_trace;
};

class Uart_group : public Pad_group
{
public:
  Uart_group(std::string name) : Pad_group(name) {}
  vp::uart_slave slave;
  vp::uart_master master;
  vp::trace tx_trace;
  vp::trace rx_trace;
};

class I2s_group : public Pad_group
{
public:
  I2s_group(std::string name) : Pad_group(name) {}
  vp::i2s_slave slave;
  vp::i2s_master master;
  vp::trace sck_trace;
  vp::trace ws_trace;
  vp::trace sd_trace;
};

class I2c_group : public Pad_group
{
public:
  I2c_group(std::string name) : Pad_group(name) {}
  vp::i2c_slave slave;
  vp::i2c_master master;
  vp::trace scl_trace;
  vp::trace sda_trace;
};

class Hyper_group : public Pad_group
{
public:
  Hyper_group(std::string name) : Pad_group(name) {}
  vp::hyper_slave slave;
  vp::trace data_trace;
  int nb_cs;
  vector<vp::trace *> cs_trace;
  vector<vp::hyper_master *> master;
  vector<vp::wire_master<bool> *> cs_master;
  int active_cs;
};

class Wire_group : public Pad_group
{
public:
  Wire_group(std::string name) : Pad_group(name) {}
  vp::wire_slave<int> slave;
  vp::wire_master<int> master;
};

class Gpio_group : public Pad_group
{
public:
  Gpio_group(std::string name) : Pad_group(name) {}
  vp::wire_master<int> master;
};

class padframe : public vp::component
{

public:

  padframe(js::config *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  static void qspim_master_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  static void qspim_sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  static void qspim_sync_cycle(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  static void qspim_cs_sync(void *__this, int cs, int active, int id);

  static void jtag_pad_slave_sync(void *__this, int tck, int tdi, int tms, int trst, int id);
  static void jtag_pad_slave_sync_cycle(void *__this, int tdi, int tms, int trst, int id);

  static void jtag_chip_slave_sync(void *__this, int tck, int tdi, int tms, int trst, int id);
  static void jtag_chip_slave_sync_cycle(void *__this, int tdi, int tms, int trst, int id);
  
  static void jtag_master_sync(void *__this, int tdo, int id);

  static void cpi_sync(void *__this, int pclk, int href, int vsync, int data, int id);
  static void cpi_sync_cycle(void *__this, int href, int vsync, int data, int id);

  static void uart_chip_sync(void *__this, int data, int id);
  static void uart_master_sync(void *__this, int data, int id);

  static void i2s_internal_edge(void *__this, int sck, int ws, int sd, int id);
  static void i2s_external_edge(void *__this, int sck, int ws, int sd, int id);

  static void i2c_chip_sync(void *__this, int scl, int sda, int id);
  static void i2c_chip_sync_cycle(void *__this, int sda, int id);
  static void i2c_master_sync(void *__this, int data, int id);

  static void hyper_master_sync_cycle(void *__this, int data, int id);
  static void hyper_sync_cycle(void *__this, int data, int id);
  static void hyper_cs_sync(void *__this, int cs, int active, int id);

  static void master_wire_sync(void *__this, int value, int id);
  static void wire_sync(void *__this, int value, int id);

  static void master_gpio_sync(void *__this, int value, int id);
  static void gpio_sync(void *__this, int value, int id);

  static void ref_clock_sync(void *__this, bool value);
  static void ref_clock_set_frequency(void *, int64_t value);

  vp::trace     trace;
  vp::io_slave in;

  vector<Pad_group *> groups;
  vp::clock_slave    ref_clock_pad_itf;
  vp::clock_master    ref_clock_itf;

  vp::trace ref_clock_trace;

  int nb_itf = 0;
};

padframe::padframe(js::config *config)
: vp::component(config)
{

}

void padframe::qspim_sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int id)
{

  printf("%s %d\n", __FILE__, __LINE__);
}

void padframe::qspim_sync_cycle(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id)
{
  padframe *_this = (padframe *)__this;
  Qspim_group *group = static_cast<Qspim_group *>(_this->groups[id]);
  unsigned int data = (data_0 << 0) | (data_1 << 1) | (data_2 << 2)| (data_3 << 3);

  if (mask & (1<<0))
    group->data_0_trace.event((uint8_t *)&data_0);
  if (mask & (1<<1))
    group->data_1_trace.event((uint8_t *)&data_1);
  if (mask & (1<<2))
    group->data_2_trace.event((uint8_t *)&data_2);
  if (mask & (1<<3))
    group->data_3_trace.event((uint8_t *)&data_3);

  if (group->active_cs == -1)
  {
    vp_warning_always(&_this->warning, "Trying to send QSPIM stream while no cs is active\n");
  }
  else if (!group->master[group->active_cs]->is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send QSPIM stream while pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->master[group->active_cs]->sync_cycle(data_0, data_1, data_2, data_3, mask);
  }
}

void padframe::qspim_cs_sync(void *__this, int cs, int active, int id)
{
  padframe *_this = (padframe *)__this;
  Qspim_group *group = static_cast<Qspim_group *>(_this->groups[id]);

  if (cs >= group->nb_cs)
  {
    vp_warning_always(&_this->warning, "Trying to activate invalid cs (interface: %s, cs: %d, nb_cs: %d)\n", group->name.c_str(), cs, group->nb_cs);
    return;
  }

  group->cs_trace[cs]->event((uint8_t *)&active);
  group->active_cs = active ? cs : -1;

  if (!group->cs_master[cs]->is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send QSPIM stream while cs pad is not connected (interface: %s, cs: %d)\n", group->name.c_str(), cs);
  }
  else
  {
    group->cs_master[cs]->sync(!active);
  }
} 

void padframe::qspim_master_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id)
{
  padframe *_this = (padframe *)__this;
  Qspim_group *group = static_cast<Qspim_group *>(_this->groups[id]);

  if (mask & (1<<0))
    group->data_0_trace.event((uint8_t *)&data_0);
  if (mask & (1<<1))
    group->data_1_trace.event((uint8_t *)&data_1);
  if (mask & (1<<2))
    group->data_2_trace.event((uint8_t *)&data_2);
  if (mask & (1<<3))
    group->data_3_trace.event((uint8_t *)&data_3);

  group->slave.sync(data_0, data_1, data_2, data_3, mask);
}



void padframe::jtag_pad_slave_sync(void *__this, int tck, int tdi, int tms, int trst, int id)
{
  padframe *_this = (padframe *)__this;
  Jtag_group *group = static_cast<Jtag_group *>(_this->groups[id]);

  group->tck_trace.event((uint8_t *)&tck);
  group->tdi_trace.event((uint8_t *)&tdi);
  group->tms_trace.event((uint8_t *)&tms);
  group->trst_trace.event((uint8_t *)&trst);

  group->master.sync(tck, tdi, tms, trst);
}


void padframe::jtag_pad_slave_sync_cycle(void *__this, int tdi, int tms, int trst, int id)
{
  padframe *_this = (padframe *)__this;
  Jtag_group *group = static_cast<Jtag_group *>(_this->groups[id]);

  group->tdi_trace.event((uint8_t *)&tdi);
  group->tms_trace.event((uint8_t *)&tms);
  group->trst_trace.event((uint8_t *)&trst);

  group->master.sync_cycle(tdi, tms, trst);
}



void padframe::jtag_chip_slave_sync(void *__this, int tck, int tdi, int tms, int trst, int id)
{
  padframe *_this = (padframe *)__this;
  Jtag_group *group = static_cast<Jtag_group *>(_this->groups[id]);

  group->tdo_trace.event((uint8_t *)&tdi);

  if (tck)
    group->pad_slave.sync(tdi);
}


void padframe::jtag_chip_slave_sync_cycle(void *__this, int tdi, int tms, int trst, int id)
{
  padframe *_this = (padframe *)__this;
  Jtag_group *group = static_cast<Jtag_group *>(_this->groups[id]);

  group->tdo_trace.event((uint8_t *)&tdi);

  group->pad_slave.sync(tdi);
}



void padframe::cpi_sync(void *__this, int pclk, int href, int vsync, int data, int id)
{
  padframe *_this = (padframe *)__this;
  Cpi_group *group = static_cast<Cpi_group *>(_this->groups[id]);

  group->pclk_trace.event((uint8_t *)&pclk);
  group->href_trace.event((uint8_t *)&href);
  group->vsync_trace.event((uint8_t *)&vsync);
  group->data_trace.event((uint8_t *)&data);

  group->master.sync(pclk, href, vsync, data);
}


void padframe::cpi_sync_cycle(void *__this, int href, int vsync, int data, int id)
{
  padframe *_this = (padframe *)__this;
  Cpi_group *group = static_cast<Cpi_group *>(_this->groups[id]);

  group->href_trace.event((uint8_t *)&href);
  group->vsync_trace.event((uint8_t *)&vsync);
  group->data_trace.event((uint8_t *)&data);

  group->master.sync_cycle(href, vsync, data);
}


void padframe::uart_chip_sync(void *__this, int data, int id)
{
  padframe *_this = (padframe *)__this;
  Uart_group *group = static_cast<Uart_group *>(_this->groups[id]);
  group->tx_trace.event((uint8_t *)&data);
  if (!group->master.is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send UART stream while pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->master.sync(data);
  }
}



void padframe::uart_master_sync(void *__this, int data, int id)
{
  padframe *_this = (padframe *)__this;
  Uart_group *group = static_cast<Uart_group *>(_this->groups[id]);

  group->rx_trace.event((uint8_t *)&data);

  group->slave.sync(data);
}



void padframe::i2s_internal_edge(void *__this, int sck, int ws, int sd, int id)
{
  padframe *_this = (padframe *)__this;
  I2s_group *group = static_cast<I2s_group *>(_this->groups[id]);
  //group->tx_trace.event((uint8_t *)&data);
  if (!group->slave.is_bound())
  {
    //vp_warning_always(&_this->warning, "Trying to send I2S stream while pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->slave.sync(sck, ws, sd);
  }
}



void padframe::i2s_external_edge(void *__this, int sck, int ws, int sd, int id)
{
  padframe *_this = (padframe *)__this;
  I2s_group *group = static_cast<I2s_group *>(_this->groups[id]);

  group->sck_trace.event((uint8_t *)&sck);
  group->ws_trace.event((uint8_t *)&ws);
  group->sd_trace.event((uint8_t *)&sd);

  group->master.sync(sck, ws, sd);
}



void padframe::i2c_chip_sync(void *__this, int scl, int sda, int id)
{
  padframe *_this = (padframe *)__this;
  I2c_group *group = static_cast<I2c_group *>(_this->groups[id]);
  group->scl_trace.event((uint8_t *)&scl);
  group->sda_trace.event((uint8_t *)&sda);
  if (!group->master.is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send I2C stream while pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->master.sync(scl, sda);
  }
}

void padframe::i2c_chip_sync_cycle(void *__this, int sda, int id)
{
  padframe *_this = (padframe *)__this;
  I2c_group *group = static_cast<I2c_group *>(_this->groups[id]);
  group->sda_trace.event((uint8_t *)&sda);
  if (!group->master.is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send I2C stream while pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->master.sync_cycle(sda);
  }
}

void padframe::i2c_master_sync(void *__this, int sda, int id)
{
  padframe *_this = (padframe *)__this;
  I2c_group *group = static_cast<I2c_group *>(_this->groups[id]);

  group->sda_trace.event((uint8_t *)&sda);

  group->slave.sync(sda);
}




void padframe::hyper_master_sync_cycle(void *__this, int data, int id)
{
  padframe *_this = (padframe *)__this;

  Hyper_group *group = static_cast<Hyper_group *>(_this->groups[id]);
  group->data_trace.event((uint8_t *)&data);
  group->slave.sync_cycle(data);
}

void padframe::hyper_sync_cycle(void *__this, int data, int id)
{
  padframe *_this = (padframe *)__this;
  Hyper_group *group = static_cast<Hyper_group *>(_this->groups[id]);
  group->data_trace.event((uint8_t *)&data);
  if (!group->master[group->active_cs]->is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send HYPER stream while pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->master[group->active_cs]->sync_cycle(data);
  }
}


void padframe::hyper_cs_sync(void *__this, int cs, int active, int id)
{
  padframe *_this = (padframe *)__this;
  Hyper_group *group = static_cast<Hyper_group *>(_this->groups[id]);

  if (cs >= group->nb_cs)
  {
    vp_warning_always(&_this->warning, "Trying to activate invalid cs (interface: %s, cs: %d, nb_cs: %d)\n", group->name.c_str(), cs, group->nb_cs);
    return;
  }

  group->cs_trace[cs]->event((uint8_t *)&active);
  group->active_cs = cs;

  if (!group->cs_master[cs]->is_bound())
  {
    vp_warning_always(&_this->warning, "Trying to send HYPER stream while cs pad is not connected (interface: %s)\n", group->name.c_str());
  }
  else
  {
    group->cs_master[cs]->sync(!active);
  }
}

void padframe::master_gpio_sync(void *__this, int value, int id)
{
  padframe *_this = (padframe *)__this;
  Gpio_group *group = static_cast<Gpio_group *>(_this->groups[id]);
  group->master.sync(value);  
}

void padframe::gpio_sync(void *__this, int value, int id)
{
  padframe *_this = (padframe *)__this;
  Gpio_group *group = static_cast<Gpio_group *>(_this->groups[id]);
  group->master.sync(value);  
}

void padframe::master_wire_sync(void *__this, int value, int id)
{
  padframe *_this = (padframe *)__this;
  Wire_group *group = static_cast<Wire_group *>(_this->groups[id]);
  group->slave.sync(value);  
}

void padframe::wire_sync(void *__this, int value, int id)
{
  padframe *_this = (padframe *)__this;
  Wire_group *group = static_cast<Wire_group *>(_this->groups[id]);
  group->master.sync(value);  
}


vp::io_req_status_e padframe::req(void *__this, vp::io_req *req)
{
  padframe *_this = (padframe *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  _this->trace.msg("IO access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  return vp::IO_REQ_OK;
}

void padframe::ref_clock_sync(void *__this, bool value)
{
  padframe *_this = (padframe *)__this;
  _this->ref_clock_trace.event((uint8_t *)&value);
  _this->ref_clock_itf.sync(value);
}

void padframe::ref_clock_set_frequency(void *__this, int64_t value)
{
  padframe *_this = (padframe *)__this;
  _this->ref_clock_itf.set_frequency(value);
}

int padframe::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&padframe::req);

  new_slave_port("in", &in);

  ref_clock_pad_itf.set_sync_meth(&padframe::ref_clock_sync);
  ref_clock_pad_itf.set_set_frequency_meth(&padframe::ref_clock_set_frequency);
  new_slave_port("ref_clock_pad", &this->ref_clock_pad_itf);

  new_master_port("ref_clock", &this->ref_clock_itf);

  this->traces.new_trace_event("ref_clock", &this->ref_clock_trace, 1);

  js::config *groups = get_js_config()->get("groups");

  for (auto& group: groups->get_childs())
  {
    std::string name = group.first;
    js::config *config = group.second;
    js::config *type_config = config->get("type");
    if (type_config)
    {
      std::string type = type_config->get_str();

      trace.msg("Found pad group (group: %s, type: %s)\n",
        name.c_str(), type.c_str());

      if (type == "qspim")
      {
        Qspim_group *group = new Qspim_group(name);
        new_slave_port(name, &group->slave);
        group->active_cs = -1;
        group->slave.set_sync_meth_muxed(&padframe::qspim_sync, nb_itf);
        group->slave.set_sync_cycle_meth_muxed(&padframe::qspim_sync_cycle, nb_itf);
        group->slave.set_cs_sync_meth_muxed(&padframe::qspim_cs_sync, nb_itf);
        this->groups.push_back(group);

        traces.new_trace_event(name + "/data_0", &group->data_0_trace, 1);
        traces.new_trace_event(name + "/data_1", &group->data_1_trace, 1);
        traces.new_trace_event(name + "/data_2", &group->data_2_trace, 1);
        traces.new_trace_event(name + "/data_3", &group->data_3_trace, 1);
        js::config *nb_cs_config = config->get("nb_cs");
        group->nb_cs = nb_cs_config ? nb_cs_config->get_int() : 1;
        for (int i=0; i<group->nb_cs; i++)
        {
          vp::trace *trace = new vp::trace;
          traces.new_trace_event(name + "/cs_" + std::to_string(i), trace, 4);
          group->cs_trace.push_back(trace);
          vp::qspim_master *itf = new vp::qspim_master;
          itf->set_sync_meth_muxed(&padframe::qspim_master_sync, nb_itf);

          new_master_port(name + "_cs" + std::to_string(i) + "_data_pad", itf);
          group->master.push_back(itf);

          vp::wire_master<bool> *cs_itf = new vp::wire_master<bool>;
          new_master_port(name + "_cs" + std::to_string(i) + "_pad", cs_itf);
          group->cs_master.push_back(cs_itf);
        }

        nb_itf++;
      }
      else if (type == "jtag")
      {
        Jtag_group *group = new Jtag_group(name);
        new_master_port(name, &group->master);
        new_slave_port(name + "_pad", &group->pad_slave);
        new_slave_port(name + "_out", &group->chip_slave);

        group->pad_slave.set_sync_meth_muxed(&padframe::jtag_pad_slave_sync, nb_itf);
        group->pad_slave.set_sync_cycle_meth_muxed(&padframe::jtag_pad_slave_sync_cycle, nb_itf);

        group->chip_slave.set_sync_meth_muxed(&padframe::jtag_chip_slave_sync, nb_itf);
        group->chip_slave.set_sync_cycle_meth_muxed(&padframe::jtag_chip_slave_sync_cycle, nb_itf);

        this->groups.push_back(group);

        traces.new_trace_event(name + "/tck", &group->tck_trace, 1);
        traces.new_trace_event(name + "/tdi", &group->tdi_trace, 1);
        traces.new_trace_event(name + "/tdo", &group->tdo_trace, 1);
        traces.new_trace_event(name + "/tms", &group->tms_trace, 1);
        traces.new_trace_event(name + "/trst", &group->trst_trace, 1);

        nb_itf++;
      }
      else if (type == "cpi")
      {
        Cpi_group *group = new Cpi_group(name);
        new_master_port(name, &group->master);
        new_slave_port(name + "_pad", &group->slave);
        group->slave.set_sync_meth_muxed(&padframe::cpi_sync, nb_itf);
        group->slave.set_sync_cycle_meth_muxed(&padframe::cpi_sync_cycle, nb_itf);
        this->groups.push_back(group);
        traces.new_trace_event(name + "/pclk", &group->pclk_trace, 1);
        traces.new_trace_event(name + "/href", &group->href_trace, 1);
        traces.new_trace_event(name + "/vsync", &group->vsync_trace, 1);
        traces.new_trace_event(name + "/data", &group->data_trace, 8);
        nb_itf++;
      }
      else if (type == "uart")
      {
        Uart_group *group = new Uart_group(name);
        new_master_port(name + "_pad", &group->master);
        new_slave_port(name, &group->slave);
        group->master.set_sync_meth_muxed(&padframe::uart_master_sync, nb_itf);
        group->slave.set_sync_meth_muxed(&padframe::uart_chip_sync, nb_itf);
        this->groups.push_back(group);
        traces.new_trace_event(name + "/tx", &group->tx_trace, 1);
        traces.new_trace_event(name + "/rx", &group->rx_trace, 1);
        nb_itf++;
      }
      else if (type == "i2s")
      {
        I2s_group *group = new I2s_group(name);
        new_slave_port(name + "_pad", &group->slave);
        new_master_port(name, &group->master);
        group->master.set_sync_meth_muxed(&padframe::i2s_internal_edge, nb_itf);
        group->slave.set_sync_meth_muxed(&padframe::i2s_external_edge, nb_itf);
        this->groups.push_back(group);
        traces.new_trace_event(name + "/sck", &group->sck_trace, 1);
        traces.new_trace_event(name + "/ws", &group->ws_trace, 1);
        traces.new_trace_event(name + "/sd", &group->sd_trace, 1);
        nb_itf++;
      }
      else if (type == "i2c")
      {
        I2c_group *group = new I2c_group(name);
        new_master_port(name + "_pad", &group->master);
        new_slave_port(name, &group->slave);
        group->master.set_sync_meth_muxed(&padframe::i2c_master_sync, nb_itf);
        group->slave.set_sync_meth_muxed(&padframe::i2c_chip_sync, nb_itf);
        group->slave.set_sync_cycle_meth_muxed(&padframe::i2c_chip_sync_cycle, nb_itf);
        this->groups.push_back(group);
        traces.new_trace_event(name + "/scl", &group->scl_trace, 1);
        traces.new_trace_event(name + "/sda", &group->sda_trace, 1);
        nb_itf++;
      }
      else if (type == "hyper")
      {
        Hyper_group *group = new Hyper_group(name);
        new_slave_port(name, &group->slave);
        group->slave.set_sync_cycle_meth_muxed(&padframe::hyper_sync_cycle, nb_itf);
        group->slave.set_cs_sync_meth_muxed(&padframe::hyper_cs_sync, nb_itf);
        this->groups.push_back(group);
        traces.new_trace_event(name + "/data", &group->data_trace, 8);
        js::config *nb_cs_config = config->get("nb_cs");
        group->nb_cs = nb_cs_config ? nb_cs_config->get_int() : 1;
        for (int i=0; i<group->nb_cs; i++)
        {
          vp::trace *trace = new vp::trace;
          traces.new_trace_event(name + "/cs_" + std::to_string(i), trace, 1);
          group->cs_trace.push_back(trace);
          vp::hyper_master *itf = new vp::hyper_master;
          itf->set_sync_cycle_meth_muxed(&padframe::hyper_master_sync_cycle, nb_itf);

          new_master_port(name + "_cs" + std::to_string(i) + "_data_pad", itf);
          group->master.push_back(itf);

          vp::wire_master<bool> *cs_itf = new vp::wire_master<bool>;
          new_master_port(name + "_cs" + std::to_string(i) + "_pad", cs_itf);
          group->cs_master.push_back(cs_itf);
        }
        nb_itf++;
      }
      else if (type == "wire")
      {
        Wire_group *group = new Wire_group(name);
        this->groups.push_back(group);
        js::config *is_master_config = config->get("is_master");
        js::config *is_slave_config = config->get("is_slave");

        if (is_master_config != NULL && is_master_config->get_bool())
        {
          group->master.set_sync_meth_muxed(&padframe::master_wire_sync, nb_itf);
          this->new_master_port(name + "_pad", &group->master);
          group->slave.set_sync_meth_muxed(&padframe::wire_sync, nb_itf);
          this->new_slave_port(name, &group->slave);
        }
        else if (is_slave_config != NULL && is_slave_config->get_bool())
        {
          group->master.set_sync_meth_muxed(&padframe::master_wire_sync, nb_itf);
          this->new_master_port(name, &group->master);
          group->slave.set_sync_meth_muxed(&padframe::wire_sync, nb_itf);
          this->new_slave_port(name + "_pad", &group->slave);
        }

        nb_itf++;
      }
      else if (type == "gpio")
      {
        Gpio_group *group = new Gpio_group(name);
        this->groups.push_back(group);
        js::config *is_master_config = config->get("is_master");
        js::config *is_slave_config = config->get("is_slave");

        if (is_master_config != NULL && is_master_config->get_bool())
        {
          group->master.set_sync_meth_muxed(&padframe::master_gpio_sync, nb_itf);
          this->new_master_port(name + "_pad", &group->master);
        }

        nb_itf++;
      }
      else
      {
        warning.warning("Unknown pad group type (group: %s, type: %s)\n",
          name.c_str(), type.c_str());
      }
    }
  }

  return 0;
}

void padframe::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new padframe(config);
}
