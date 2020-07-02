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

#ifndef __PULP_UDMA_UDMA_SPIM_v4_IMPL_HPP__
#define __PULP_UDMA_UDMA_SPIM_v4_IMPL_HPP__

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
#include "archi/udma/udma_v4.h"

/*
 * SPIM
 */

class Spim_periph_v4;


class Spim_v4_rx_channel : public Udma_rx_channel
{
public:
  Spim_v4_rx_channel(udma *top, Spim_periph_v4 *periph, string name) : Udma_rx_channel(top, name), periph(periph) {}

  void reset(bool active);

  void handle_rx_bits(int data_0, int data_1, int data_2, int data_3, int mask);

private:
  Spim_periph_v4 *periph;
  int current_byte;
  int current_bit;
};



class Spim_v4_tx_channel : public Udma_tx_channel
{
public:
  Spim_v4_tx_channel(udma *top, Spim_periph_v4 *periph, string name);
  void handle_ready_reqs();
  void check_state();

private:
  void reset(bool active);
  Spim_periph_v4 *periph;

  static void handle_pending_word(void *__this, vp::clock_event *event);
  void handle_data(uint32_t data);

  vp::clock_event *pending_word_event;

  uint32_t tx_pending_word;       // Word received by last L2 req
  bool     has_tx_pending_word;   // Tell if a TX pending word is present
  vp::io_req *pending_req;
  
};



class Spim_v4_cmd_channel : public Udma_tx_channel
{
public:
  Spim_v4_cmd_channel(udma *top, Spim_periph_v4 *periph, string name);
  void handle_ready_reqs();
  void check_state();

private:
  void reset(bool active);
  static void handle_pending_word(void *__this, vp::clock_event *event);
  void handle_eot(bool cs_keep);
  void handle_data(uint32_t data);
  bool push_tx_to_spi(uint32_t value, int nb_bits);
  bool push_rx_to_spi(int nb_bits);

  vp::clock_event *pending_word_event;

  Spim_periph_v4 *periph;

  bool     has_tx_pending_word;   // Tell if a TX pending word is present
  uint32_t tx_pending_word;       // Word received by last L2 req
  vp::io_req *pending_req;
  uint32_t command;
  int cs;
  bool gen_eot_with_evt;

  int qpi;
  int lsb_first;
  int bitsword;
  int wordtrans;

};



class Spim_periph_v4 : public Udma_periph
{
  friend class Spim_v4_cmd_channel;
  friend class Spim_v4_rx_channel;
  friend class Spim_v4_tx_channel;

public:
  Spim_periph_v4(udma *top, int id, int itf_id);
  static void slave_sync(void *_this, int data_0, int data_1, int data_2, int data_3, int mask);
  void reset(bool active);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  static void handle_spi_pending_word(void *__this, vp::clock_event *event);
  void check_state();
  bool push_tx_to_spi(uint32_t value, int nb_bits, int qpi, int lsb_first, int bitsword, int wordtrans);
  bool push_rx_to_spi(int nb_bits, int qpi, int lsb_first, int bitsword, int wordtrans);

protected:
  vp::clock_event *pending_spi_word_event;

  vp::qspim_master qspim_itf;
  int clkdiv;
  bool waiting_rx;
  bool waiting_tx;
  bool waiting_tx_flush;
  bool is_full_duplex;
  int cmd_pending_bits;
  int nb_received_bits;
  uint32_t rx_pending_word;
  uint32_t tx_pending_word;
  int eot_event;


  int qpi;
  int lsb_first;
  int bitsword;
  int wordtrans;

  int spi_qpi;
  int spi_lsb_first;
  int spi_bitsword;
  int spi_wordtrans;

  int      tx_pending_bits;

  uint32_t spi_tx_pending_word;   // Word being flushed to spi pads
  int      spi_tx_pending_bits;   // Tell how many bits are ready to be sent to SPI pads
  
  int      spi_rx_pending_bits;   // Tell how many bits should be received from spi pads

  int64_t next_bit_cycle;


  uint32_t rx_received_bits;
  int      rx_bit_offset;
  int      rx_counter_bits;
  int      rx_counter_transf;

  int      tx_bit_offset;
  int      tx_counter_bits;
  int      tx_counter_transf;

};

#endif