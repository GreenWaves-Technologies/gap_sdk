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

#ifndef __PULP_UDMA_UDMA_HYPER_V3_IMPL_HPP__
#define __PULP_UDMA_UDMA_HYPER_V3_IMPL_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/wire.hpp>
#include <vp/itf/hyper.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <udma_hyper/udma_hyper_regfields.h>
#include <udma_hyper/udma_hyper_gvsoc.h>


typedef enum
{
  HYPER_STATE_IDLE,
  HYPER_STATE_DELAY,
  HYPER_STATE_CS,
  HYPER_STATE_CA,
  HYPER_STATE_DATA,
  HYPER_STATE_CS_OFF,
} hyper_state_e;


typedef enum
{
  HYPER_CHANNEL_STATE_IDLE,
  HYPER_CHANNEL_STATE_SEND_ADDR,
  HYPER_CHANNEL_STATE_SEND_SIZE,
  HYPER_CHANNEL_STATE_SEND_CFG,
  HYPER_CHANNEL_STATE_STOP
} hyper_channel_state_e;


class Hyper_periph;

class Hyper_rx_channel : public Udma_rx_channel
{
public:
  Hyper_rx_channel(udma *top, Hyper_periph *periph, string name);
  void handle_rx_data(int data);
  void handle_ready();
  void handle_transfer_end();

private:
  void reset(bool active);
  Hyper_periph *periph;
};


class Hyper_tx_channel : public Udma_tx_channel
{
  friend class Hyper_periph;

public:
  Hyper_tx_channel(udma *top, Hyper_periph *periph, string name);
  void handle_transfer_end();

protected:
  void handle_ready_reqs();

private:
  void reset(bool active);

  Hyper_periph *periph;

};


class Hyper_periph : public Udma_periph
{
  friend class Hyper_tx_channel;
  friend class Hyper_rx_channel;

public:
  Hyper_periph(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  static void rx_sync(void *__this, int data);
  void reset(bool active);
  static void handle_pending_word(void *__this, vp::clock_event *event);
  static void handle_pending_channel(void *__this, vp::clock_event *event);
  void check_state();
  void handle_ready_reqs();

protected:
  vp::hyper_master hyper_itf;
  Hyper_tx_channel *tx_channel;
  Hyper_rx_channel *rx_channel;
  vp::wire_master<bool>    irq_itf;

private:
  void trans_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

  vp_regmap_udma_hyper regmap;

  vector<Udma_transfer *> pending_transfers;

  int eot_event;
  int pending_bytes;
  vp::clock_event *pending_word_event;
  vp::clock_event *pending_channel_event;
  int64_t next_bit_cycle;
  vp::io_req *pending_req;
  uint32_t pending_word;
  int transfer_size;
  hyper_state_e state;
  hyper_channel_state_e channel_state;
  int delay;
  int ca_count;
  bool pending_tx;
  bool pending_rx;
  Udma_transfer *current_cmd;
  uint32_t pending_length;
  uint32_t pending_burst;
  uint32_t pending_2d_addr;
  union
  {
    struct {
      unsigned int low_addr:3;
      unsigned int reserved:13;
      unsigned int high_addr:29;
      unsigned int burst_type:1;
      unsigned int address_space:1;
      unsigned int read:1;
    } __attribute__((packed));
    uint8_t raw[6];
  } ca;

  vp::trace     trace;
};

#endif
