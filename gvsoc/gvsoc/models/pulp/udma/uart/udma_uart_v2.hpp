/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __PULP_UDMA_UDMA_UART_V2_HPP__
#define __PULP_UDMA_UDMA_UART_V2_HPP__

#include <udma_uart/udma_uart_regfields.h>
#include <udma_uart/udma_uart_gvsoc.h>

/*
 * UART
 */

class Uart_periph;

typedef enum
{
  UART_RX_STATE_WAIT_START,
  UART_RX_STATE_DATA,
  UART_RX_STATE_PARITY,
  UART_RX_STATE_WAIT_STOP
} uart_rx_state_e;

class Uart_rx_channel : public Udma_rx_channel
{
public:
  Uart_rx_channel(udma *top, Uart_periph *periph, string name);
  bool is_busy();
  void handle_rx_bit(int bit);

private:
  void reset(bool active);
  Uart_periph *periph;
  uart_rx_state_e state;
  int parity;
  int stop_bits;
  uint8_t  pending_rx_byte;
  int nb_received_bits;
};


typedef enum
{
  UART_TX_STATE_START,
  UART_TX_STATE_DATA,
  UART_TX_STATE_PARITY,
  UART_TX_STATE_STOP
} uart_tx_state_e;

class Uart_tx_channel : public Udma_tx_channel
{
public:
  Uart_tx_channel(udma *top, Uart_periph *periph, string name);
  void handle_ready_reqs();
  bool is_busy();

private:
  void reset(bool active);
  void check_state();
  static void handle_pending_word(void *__this, vp::clock_event *event);

  Uart_periph *periph;

  vp::clock_event *pending_word_event;

  uint32_t pending_word;
  int pending_bits;
  uart_tx_state_e state;
  vp::io_req *pending_req;
  int parity;
  int64_t next_bit_cycle;
  int stop_bits;
  int sent_bits;
};


class Uart_periph : public Udma_periph
{
  friend class Uart_tx_channel;
  friend class Uart_rx_channel;

public:
  Uart_periph(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);

protected:
  vp::uart_master uart_itf;

private:
  void rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
  void status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

  static void rx_sync(void *, int data);

  vp_regmap_udma_uart regmap;

  vp::trace     trace;
};

#endif