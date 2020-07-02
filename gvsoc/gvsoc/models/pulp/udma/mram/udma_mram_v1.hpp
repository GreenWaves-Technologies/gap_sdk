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

#ifndef __PULP_UDMA_MRAM_UDMA_MRAM_V1_HPP__
#define __PULP_UDMA_MRAM_UDMA_MRAM_V1_HPP__

#include <vp/vp.hpp>
#include "../udma_impl.hpp"
#include "archi/udma/mram/udma_mram_v1.h"
#include <pulp/mram/mram.hpp>


class Mram_v1_rx_channel;
class Mram_v1_tx_channel;


class Mram_periph_v1 : public Udma_periph
{
public:
  Mram_periph_v1(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);
  void handle_ready_reqs();
  
protected:
  vp_udma_mram_tx_daddr   r_tx_daddr;
  vp_udma_mram_rx_daddr   r_rx_daddr;
  vp_udma_mram_status     r_status;
  vp_udma_mram_mram_mode  r_mode;
  vp_udma_mram_erase_addr r_erase_addr;
  vp_udma_mram_erase_size r_erase_size;
  vp_udma_mram_clock_div  r_clock_div;
  vp_udma_mram_trigger    r_trigger;
  vp_udma_mram_isr        r_isr;
  vp_udma_mram_ier        r_ier;
  vp_udma_mram_icr        r_icr;

private:
  static void data_grant(void *_this, vp::io_req *req);
  static void data_response(void *_this, vp::io_req *req);
  static void handle_tx_pending_word(void *__this, vp::clock_event *event);
  static void handle_rx_pending_word(void *__this, vp::clock_event *event);
  void check_state();

  vp::io_master io_itf;
  vp::clock_event *pending_tx_access_event;
  vp::clock_event *pending_rx_access_event;
  vp::io_req *pending_req;
  uint32_t rx_data;
  bool pending_rx;
  vp::io_req io_req;
  int first_event;
  vp::trace     trace;

  Mram_v1_tx_channel *tx_channel;
  Mram_v1_rx_channel *rx_channel;

  Mram_itf *mram;
};



class Mram_v1_rx_channel : public Udma_rx_channel
{
public:
  Mram_v1_rx_channel(udma *top, Mram_periph_v1 *periph, int id, string name);
  void set_addr(unsigned int addr) { this->addr = addr; this->current_addr = addr; }
  unsigned int addr;
  unsigned int current_addr;
  void handle_ready();

private:
  Mram_periph_v1 *periph;
};

class Mram_v1_tx_channel : public Udma_tx_channel
{
  friend class Mram_periph_v1;

public:
  Mram_v1_tx_channel(udma *top, Mram_periph_v1 *periph, int id, string name);
  void set_addr(unsigned int addr) { this->addr = addr; this->current_addr = addr; }
  unsigned int addr;
  unsigned int current_addr;

protected:
  void handle_ready_reqs();

private:
  Mram_periph_v1 *periph;
};

#endif
