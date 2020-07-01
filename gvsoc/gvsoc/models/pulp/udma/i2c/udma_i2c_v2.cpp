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


#include "../udma_impl.hpp"
#include "archi/udma/i2c/udma_i2c_v2.h"
#include "archi/utils.h"
#include "vp/itf/i2c.hpp"


I2c_periph_v2::I2c_periph_v2(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "i2c" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new I2c_rx_channel(top, this, UDMA_EVENT_ID(id), itf_name + "_rx");
  channel1 = new I2c_tx_channel(top, this, UDMA_EVENT_ID(id) + 1, itf_name + "_tx");

  top->new_master_port(this, itf_name, &i2c_itf);

  i2c_itf.set_sync_meth(&I2c_periph_v2::rx_sync);
}
 

void I2c_periph_v2::reset(bool active)
{
  Udma_periph::reset(active);

  if (active)
  {
    this->state = I2C_PERIPH_STATE_WAIT_CMD;
    this->clkdiv = 0;
    this->repeat_count = 0;
    this->waiting_rx = false;
    this->gen_ack = false;
    this->waiting_pending_bits = true;
  }
}




vp::io_req_status_e I2c_periph_v2::custom_req(vp::io_req *req, uint64_t offset)
{
  return vp::IO_REQ_INVALID;
}


void I2c_periph_v2::rx_sync(void *__this, int data)
{
  I2c_periph_v2 *_this = (I2c_periph_v2 *)__this;
  (static_cast<I2c_rx_channel *>(_this->channel0))->handle_rx_bit(data);

  _this->trace.msg("Received bit (value: %d)\n", data);
}




I2c_tx_channel::I2c_tx_channel(udma *top, I2c_periph_v2 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  pending_word_event = top->event_new(this, I2c_tx_channel::handle_pending_word);
}


std::string get_command_name(unsigned int command)
{
  switch (command)
  {
    case I2C_CMD_START: return "START";
    case I2C_CMD_STOP: return "STOP";
    case I2C_CMD_RD_ACK: return "RD_ACK";
    case I2C_CMD_RD_NACK: return "RD_NACK";
    case I2C_CMD_WR: return "WR";
    case I2C_CMD_WAIT: return "WAIT";
    case I2C_CMD_RPT: return "RPT";
    case I2C_CMD_CFG: return "CFG";
    case I2C_CMD_WAIT_EV: return "WAIT_EV";
    default: return "UNKNOWN";
  }
}


void I2c_tx_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
  I2c_tx_channel *_this = (I2c_tx_channel *)__this;

  if (_this->periph->waiting_rx)
  {
    _this->periph->prev_scl ^= 1;
    _this->periph->i2c_itf.sync(_this->periph->prev_scl, 0);

    if (_this->periph->prev_scl)
    {
      _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->periph->clkdiv;

      if (_this->periph->gen_ack)
      {
        _this->periph->waiting_rx = false;
        _this->periph->gen_ack = false;
      }
      else
      {
        int bit = _this->periph->pending_rx_bit;

        _this->periph->pending_value = (_this->periph->pending_value << 1) | bit;
        _this->periph->pending_value_bits--;

        _this->periph->trace.msg("Sampled bit (value: 0x%x, pending_value: 0x%x, pending_word_bits: %d)\n", bit, _this->periph->pending_value & 0xff, _this->periph->pending_value_bits);

        if (_this->periph->pending_value_bits == 0)
        {
          _this->periph->trace.msg("Sampled byte, pushing to channel (value: 0x%x)\n", _this->periph->pending_value & 0xff);

          (static_cast<I2c_rx_channel *>(_this->periph->channel0))->push_data((uint8_t *)&_this->periph->pending_value, 1);
          
          _this->periph->gen_ack = true;
        }
      }
    }
  }
  else
  {
    int bit = -1;
    int scl = 0;
    bool end = false;
    bool stop = false;

    uint8_t pending_byte =  _this->pending_word & 0xff;
    _this->periph->trace.msg("Handling byte (value: 0x%x)\n", pending_byte);

    if (_this->periph->state == I2C_PERIPH_STATE_WAIT_CMD || _this->periph->state == I2C_PERIPH_STATE_WAIT_RPT_CMD)
    {
      unsigned int command = pending_byte;

      if (_this->periph->state == I2C_PERIPH_STATE_WAIT_CMD && _this->periph->repeat_count)
      {
        _this->periph->repeat_count--;
        command = _this->periph->repeat_command;
      }
      else
      {
        _this->pending_word >>= 8;
        _this->pending_bits -= 8;

        if (_this->periph->state == I2C_PERIPH_STATE_WAIT_RPT_CMD)
        {
          _this->periph->repeat_command = command;
          _this->periph->repeat_count--;
        }
      }

      _this->periph->trace.msg("Received command (value: 0x%x, name: %s)\n", command, get_command_name(command).c_str());

      switch (command)
      {
        case I2C_CMD_CFG:
          _this->periph->state = I2C_PERIPH_STATE_WAIT_CFG;
          _this->periph->pending_value_bits = 0;
          break;
        case I2C_CMD_START:
          _this->periph->state = I2C_PERIPH_STATE_START0;
          _this->periph->waiting_pending_bits = false;
          bit = 1;
          scl = 1;
          break;
        case I2C_CMD_STOP:
          _this->periph->state = I2C_PERIPH_STATE_STOP0;
          _this->periph->waiting_pending_bits = false;
          bit = 0;
          scl = 0;
          break;
        case I2C_CMD_WR:
          _this->periph->state = I2C_PERIPH_STATE_WR0;
          _this->periph->pending_value_bits = 8;
          break;
        case I2C_CMD_RD_NACK:
        case I2C_CMD_RD_ACK:
          _this->periph->state = I2C_PERIPH_STATE_WAIT_CMD;
          _this->periph->waiting_rx = true;
          _this->periph->pending_value_bits = 8;
          break;
        case I2C_CMD_RPT:
          _this->periph->state = I2C_PERIPH_STATE_WAIT_RPT;
          break;
        case I2C_CMD_WAIT:
          break;
        default: _this->periph->trace.force_warning("Unknown command (value: 0x%x)\n", command);
      }
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_WAIT_CFG)
    {
      if (_this->periph->pending_value_bits == 0)
        _this->periph->pending_value = pending_byte << 8;
      else
      {
        _this->periph->clkdiv = _this->periph->pending_value | pending_byte;
        _this->periph->state = I2C_PERIPH_STATE_WAIT_CMD;
        _this->periph->trace.msg("Configuring clock divider (value: %d)\n", _this->periph->clkdiv);
      }

      _this->periph->pending_value_bits += 8;
      _this->pending_bits -= 8;
      _this->pending_word >>= 8;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_WR0)
    {
      scl = 0;
      if (_this->periph->pending_value_bits == 8)
      {
        _this->periph->pending_value = _this->pending_word & 0xff;
      }


      bit = (_this->periph->pending_value >> 7) & 1;
      _this->periph->prev_sda = bit;
      _this->periph->pending_value <<= 1;
      _this->periph->pending_value_bits--;

      _this->periph->state = I2C_PERIPH_STATE_WR1;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_WR1)
    {
      scl = 1;
      bit = _this->periph->prev_sda;

      if (_this->periph->pending_value_bits == 0)
      {
        _this->periph->state = I2C_PERIPH_STATE_ACK0;
        _this->pending_bits -= 8;
        _this->pending_word = _this->pending_word >> 8;
      }
      else
      {
        _this->periph->state = I2C_PERIPH_STATE_WR0;
      }
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_ACK0)
    {
      scl = 0;
      bit = 1;
      _this->periph->state = I2C_PERIPH_STATE_ACK1;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_ACK1)
    {
      scl = 1;
      bit = 1;
      _this->periph->state = I2C_PERIPH_STATE_WAIT_CMD;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_WAIT_RPT)
    {
      _this->periph->repeat_count = pending_byte;
      _this->periph->trace.msg("Configuring repeat mode (iterations: %d)\n", _this->periph->repeat_count);

      _this->pending_bits -= 8;
      _this->pending_word >>= 8;
      if (pending_byte)
        _this->periph->state = I2C_PERIPH_STATE_WAIT_RPT_CMD;
      else
        _this->periph->state = I2C_PERIPH_STATE_WAIT_CMD;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_STOP0)
    {
      _this->periph->state = I2C_PERIPH_STATE_STOP1;
      bit = 0;
      scl = 1;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_START0)
    {
      _this->periph->state = I2C_PERIPH_STATE_WAIT_CMD;
      _this->periph->waiting_pending_bits = true;
      bit = 0;
      scl = 1;
    }
    else if (_this->periph->state == I2C_PERIPH_STATE_STOP1)
    {
      _this->periph->state = I2C_PERIPH_STATE_WAIT_CMD;
      _this->periph->waiting_pending_bits = true;
      bit = 1;
      scl = 1;
    }

    if (bit != -1)
    {
      if (!_this->periph->i2c_itf.is_bound())
      {
        _this->top->warning.force_warning("Trying to send to I2C interface while it is not connected\n");
      }
      else
      {
        _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->periph->clkdiv;
        _this->periph->trace.msg("Sending bit (scl: %d, sda: %d)\n", scl, bit);
        _this->periph->i2c_itf.sync(scl, bit);
        _this->periph->prev_scl = scl;
      }
    }

    if (_this->pending_bits == 0 && _this->pending_req)
    {
      _this->handle_ready_req_end(_this->pending_req);
      _this->pending_req = NULL;
    }
    _this->handle_ready_reqs();
  }

  _this->check_state();
}



void I2c_tx_channel::check_state()
{
  if (pending_word_event->is_enqueued())
    return;

  bool enqueue = false;

  if (this->pending_bits != 0)
    enqueue = true;

  if (!this->periph->waiting_pending_bits)
    enqueue = true;

  if (this->periph->waiting_rx)
    enqueue = true;

  if (enqueue)
  {
    int latency = 1;
    int64_t cycles = this->top->get_clock()->get_cycles();
    if (next_bit_cycle > cycles)
      latency = next_bit_cycle - cycles;

    top->event_enqueue(pending_word_event, latency);
  }
}



void I2c_tx_channel::handle_ready_reqs()
{
  if (this->pending_bits == 0 && !ready_reqs->is_empty())
  {
    vp::io_req *req = this->ready_reqs->pop();
    this->pending_req = req;
    this->pending_word = *(uint32_t *)req->get_data();
    this->pending_bits = req->get_actual_size() * 8;
    this->check_state();
  }
}



void I2c_tx_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);

  if (active)
  {
    this->next_bit_cycle = -1;
    this->pending_bits = 0;
  }
}



bool I2c_tx_channel::is_busy()
{
  return this->pending_bits != 0 || !ready_reqs->is_empty();
}


I2c_rx_channel::I2c_rx_channel(udma *top, I2c_periph_v2 *periph, int id, string name) : Udma_rx_channel(top, id, name), periph(periph)
{
}

void I2c_rx_channel::reset(bool active)
{
  Udma_rx_channel::reset(active);
  if (active)
  {
    this->nb_received_bits = 0;
  }
}

void I2c_rx_channel::handle_rx_bit(int bit)
{
  this->periph->pending_rx_bit = bit;
}

bool I2c_rx_channel::is_busy()
{
  return false;
}
