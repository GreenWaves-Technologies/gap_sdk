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


#ifndef __PULP_UDMA_UDMA_V2_IMPL_HPP__
#define __PULP_UDMA_UDMA_V2_IMPL_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/wire.hpp>
#include <vp/itf/hyper.hpp>
#include <vp/itf/i2c.hpp>
#include <vp/itf/clk.hpp>
#include <vp/itf/i2s.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "udma_ctrl/udma_ctrl_regs.h"
#include "udma_ctrl/udma_ctrl_regfields.h"
#include "udma_ctrl/udma_ctrl_gvsoc.h"
#include "udma_core_lin/udma_core_lin_regs.h"
#include "udma_core_lin/udma_core_lin_regfields.h"
#include "udma_core_lin/udma_core_lin_gvsoc.h"

class udma;
class Udma_channel;


class Udma_transfer
{
public:
  uint32_t addr;
  uint32_t size;
  int transfer_size;
  int continuous_mode;
  uint32_t current_addr;
  int32_t remaining_size;
  uint64_t received_size;
  Udma_channel *channel;

  bool prepare_req(vp::io_req *req);
  void set_next(Udma_transfer *next) { this->next = next; }
  Udma_transfer *get_next() { return next; }
  Udma_transfer *next;
};



template<class T>
class Udma_queue
{
public:
  Udma_queue(int size) : size(size) { init(); }
  void init() { first=NULL; last=NULL; nb_cmd=0; }
  T *pop();
  inline void push(T *cmd);
  void push_from_latency(T *cmd);
  bool is_full() { return nb_cmd >= size; }
  bool is_empty() { return nb_cmd == 0; }
  T *get_first() { return first; }

private:
  T *first;    // First command of the queue, commands are popped from here
  T *last;     // Last command of the queue, commands are pushed here
  int        nb_cmd;
  int        size;
};


class Udma_channel
{
public:
  Udma_channel(udma *top, std::string name);
  virtual vp::io_req_status_e req(vp::io_req *req, uint64_t offset);
  virtual vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write);
  virtual void reset(bool active);
  virtual bool is_tx() { return false; }
  void set_next(Udma_channel *next) { this->next = next; }
  Udma_channel *get_next() { return next; }
  void event_handler();
  bool prepare_req(vp::io_req *req);
  void push_ready_req(vp::io_req *req);
  bool handle_ready_req_end(vp::io_req *req);
  virtual bool is_busy() { return false; }
  virtual void handle_ready() { }
  virtual void handle_ready_reqs();
  virtual void handle_transfer_end();
  void check_state();
  void set_channel_id(int id) { this->channel_id = id; }

  Udma_transfer *current_cmd;

protected:
  vp::trace     trace;
  Udma_queue<vp::io_req> *ready_reqs;
  udma *top;

private:
  void cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void enqueue_transfer();
  virtual void handle_ready_req(vp::io_req *req);

  vp_regmap_udma_core_lin regmap;

  int transfer_size;
  bool continuous_mode;

  int channel_id;
  string name;
  Udma_channel *next;

  vp::clock_event *event;

  Udma_queue<Udma_transfer> *free_reqs;
  Udma_queue<Udma_transfer> *pending_reqs;

  vp::trace     state_event;
};



class Udma_rx_channel : public Udma_channel
{
public:
  Udma_rx_channel(udma *top, string name) : Udma_channel(top, name) {}
  bool is_tx() { return false; }
  void reset(bool active);
  void push_data(uint8_t *data, int size);
  bool has_cmd() { return this->current_cmd != NULL; }

private:
  int pending_byte_index;
  uint32_t pending_word;
};



class Udma_tx_channel : public Udma_channel
{
public:
  Udma_tx_channel(udma *top, string name) : Udma_channel(top, name) {}
  bool is_tx() { return true; }

  void handle_pending_word(void *__this, vp::clock_event *event);

};







class Io_Periph;


class Io_rx_channel : public Udma_rx_channel
{
  friend class Io_Periph;

public:
  Io_rx_channel(udma *top, Io_Periph *io_periph, int id, string name);

private:
  Io_Periph *io_periph;
};




class Io_tx_channel : public Udma_tx_channel
{
  friend class Io_Periph;

public:
  Io_tx_channel(udma *top, Io_Periph *io_periph, int id, string name);

protected:
  void handle_ready_reqs();

private:
  Io_Periph *io_periph;
};




class Udma_periph 
{
public:
  Udma_periph(udma *top, int id);
  vp::io_req_status_e req(vp::io_req *req, uint64_t offset);
  virtual void reset(bool active);
  void clock_gate(bool is_on);

  int id;
  
protected:
  Udma_channel *channel0 = NULL;
  Udma_channel *channel1 = NULL;
  Udma_channel *channel2 = NULL;
  udma *top;

private:
  virtual vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  bool is_on;
  bool reset_active;
};



class Io_Periph : public Udma_periph
{
  friend class Io_tx_channel;
  friend class Io_rx_channel;

public:
  Io_Periph(udma *top, int id, string itf_name);
  void handle_ready_reqs();
  void set_addr(unsigned int addr) { this->addr = addr; this->current_addr = addr; }
  void set_eot_event(int event) { this->eot_event = event; }

private:  
  static void data_grant(void *_this, vp::io_req *req);
  static void data_response(void *_this, vp::io_req *req);
  static void handle_pending_word(void *__this, vp::clock_event *event);
  void check_state();

  vp::io_master io_itf;
  vp::clock_event *pending_access_event;
  vp::io_req *pending_req;
  vp::io_req io_req;
  unsigned int addr;
  unsigned int current_addr;
  int eot_event = -1;
};






/*
 * UDMA
 */

template<class T>
inline void Udma_queue<T>::push(T *cmd)
{
  if (first)
    last->set_next(cmd);
  else
    first = cmd;
  cmd->set_next(NULL);
  last = cmd;
  nb_cmd++;
}



template<class T>
T *Udma_queue<T>::pop()
{
  if (!first) return NULL;
  T *cmd = first;
  first = cmd->get_next();
  nb_cmd--;

  return cmd;
}



class udma : public vp::component
{
  friend class Udma_periph;
  friend class Udma_rx_channel;

public:

  udma(js::config *config);

  int build();
  void start();
  void reset(bool active);

  void enqueue_ready(Udma_channel *channel);

  static void channel_handler(void *__this, vp::clock_event *event);
  void free_read_req(vp::io_req *req);

  void trigger_event(int event);

  vp::trace *get_trace() { return &this->trace; }
  vp::clock_engine *get_periph_clock() { return this->periph_clock; }

  void channel_register(int id, Udma_channel *channel);
  void channel_unregister(int id, Udma_channel *channel);

protected:
  vp::io_master l2_itf;
  void push_l2_write_req(vp::io_req *req);

private:

  vp_regmap_udma_ctrl ctrl_regmap;

  void check_state();

  void cfg_cg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void cfg_cg_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void cfg_cg_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void cfg_rstn_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void cfg_rstn_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void cfg_rstn_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  vp::io_req_status_e conf_req(vp::io_req *req, uint64_t offset);
  vp::io_req_status_e periph_req(vp::io_req *req, uint64_t offset);
  vp::io_req_status_e channel_req(vp::io_req *req, uint64_t offset);
  static vp::io_req_status_e req(void *__this, vp::io_req *req);
  static void event_handler(void *__this, vp::clock_event *event);
  static void l2_grant(void *__this, vp::io_req *req);
  static void l2_response(void *__this, vp::io_req *req);
  static void clk_reg(component *_this, component *clock);

  vp::trace     trace;
  vp::io_slave in;
  vp::clk_slave    periph_clock_itf;
  vp::clock_engine *periph_clock;
  
  int nb_periphs;
  int l2_read_fifo_size;
  int nb_channels;
  std::vector<Udma_periph *>periphs;
  Udma_queue<Udma_channel> *ready_rx_channels;
  Udma_queue<Udma_channel> *ready_tx_channels;
  uint32_t clock_gating;
  uint32_t ctrl_reset;
  vp::clock_event *event;
  Udma_queue<vp::io_req> *l2_read_reqs;
  Udma_queue<vp::io_req> *l2_write_reqs;
  Udma_queue<vp::io_req> *l2_read_waiting_reqs;
  
  vp::wire_master<int>    event_itf;

  std::vector<Udma_channel *> channels;
};

#ifdef HAS_SPIM
#include "spim/udma_spim_v4.hpp"
#endif

#ifdef HAS_UART
#include "uart/udma_uart_v2.hpp"
#endif

//#ifdef HAS_I2S
#include "i2s/udma_i2s_v3.hpp"
//#endif

#ifdef HAS_HYPER
#include "hyper/udma_hyper_v3.hpp"
#endif

#endif
