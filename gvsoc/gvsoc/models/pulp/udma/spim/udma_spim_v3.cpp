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

#include "../udma_v3_impl.hpp"
#include "archi/udma/spim/udma_spim_v3.h"
#include "archi/utils.h"
#include "vp/itf/qspim.hpp"




Spim_periph_v3::Spim_periph_v3(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "spim" + std::to_string(itf_id);

  channel0 = new Spim_v3_rx_channel(top, this, UDMA_CHANNEL_ID(id), itf_name + "_rx");
  channel1 = new Spim_v3_tx_channel(top, this, UDMA_CHANNEL_ID(id) + 1, itf_name + "_tx");
  channel2 = new Spim_v3_cmd_channel(top, this, UDMA_CHANNEL_ID(id) + 2, itf_name + "_cmd");

  qspim_itf.set_sync_meth(&Spim_periph_v3::slave_sync);
  top->new_master_port(this, itf_name, &qspim_itf);

  js::config *config = this->top->get_js_config()->get("spim/eot_events");
  if (config)
    this->eot_event = config->get_elem(itf_id)->get_int();
  else
    this->eot_event = -1;


  pending_spi_word_event = top->event_new(this, Spim_periph_v3::handle_spi_pending_word);
}

void Spim_periph_v3::reset(bool active)
{
  Udma_periph::reset(active);

  if (active)
  {
    this->waiting_rx = false;
    this->waiting_tx = false;
    this->waiting_tx_flush = false;
    this->is_full_duplex = false;
    this->cmd_pending_bits = 0;
    this->nb_received_bits = 0;
    this->rx_pending_word = 0x57575757;
    this->tx_pending_word = 0x57575757;
    this->spi_rx_pending_bits = 0;
    this->clkdiv = 0;
    this->next_bit_cycle = -1;
    this->spi_tx_pending_bits = 0;
    this->tx_pending_bits = 0;
  }
}

  
void Spim_periph_v3::slave_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask)
{
  Spim_periph_v3 *_this = (Spim_periph_v3 *)__this;
  (static_cast<Spim_v3_rx_channel *>(_this->channel0))->handle_rx_bits(data_0, data_1, data_2, data_3, mask);
}


void Spim_periph_v3::check_state()
{
  if ((this->spi_tx_pending_bits > 0 || (!this->is_full_duplex && this->spi_rx_pending_bits > 0)) && !this->pending_spi_word_event->is_enqueued())
  {
    int latency = 1;
    int64_t cycles = this->top->get_clock()->get_cycles();
    if (this->next_bit_cycle > cycles)
      latency = this->next_bit_cycle - cycles;

    top->event_enqueue(this->pending_spi_word_event, latency);
  }

  (static_cast<Spim_v3_tx_channel *>(this->channel1))->check_state();
  (static_cast<Spim_v3_cmd_channel *>(this->channel2))->check_state();
}



void Spim_v3_rx_channel::handle_rx_bits(int data_0, int data_1, int data_2, int data_3, int mask)
{
  this->periph->rx_received_bits = (data_3 << 3) | (data_2 << 2) | (data_1 << 1) | (data_0 << 0);
  this->top->get_trace()->msg("Received data (value: 0x%x)\n", this->periph->rx_received_bits);
}



Spim_v3_tx_channel::Spim_v3_tx_channel(udma *top, Spim_periph_v3 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  pending_word_event = top->event_new(this, Spim_v3_tx_channel::handle_pending_word);
}



void Spim_v3_tx_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
  Spim_v3_tx_channel *_this = (Spim_v3_tx_channel *)__this;

  _this->handle_data(_this->tx_pending_word);
  _this->periph->check_state();
}


void Spim_v3_tx_channel::handle_ready_reqs()
{
  if (!this->has_tx_pending_word && !ready_reqs->is_empty())
  {
    vp::io_req *req = this->ready_reqs->pop();
    this->pending_req = req;
    this->tx_pending_word = *(uint32_t *)req->get_data();
    this->has_tx_pending_word = true;
    this->periph->check_state();
  }
}



void Spim_v3_tx_channel::handle_data(uint32_t data)
{
  if (this->periph->spi_tx_pending_bits <= 0)
  {
    int nb_bits = this->periph->tx_pending_bits > 32 ? 32 : this->periph->tx_pending_bits;
    if (this->periph->push_tx_to_spi(this->tx_pending_word, nb_bits, this->periph->qpi, this->periph->lsb_first, this->periph->bitsword, this->periph->wordtrans))
    {
      this->periph->tx_pending_bits -= nb_bits;
      this->has_tx_pending_word = false;
    }
  }

  if (this->periph->tx_pending_bits == 0);
    this->periph->waiting_tx = false;

  if (!this->has_tx_pending_word && this->pending_req)
  {
    this->handle_ready_req_end(this->pending_req);
    this->pending_req = NULL;
  }

  this->handle_ready_reqs();
}



void Spim_v3_tx_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);

  if (active)
  {
    this->has_tx_pending_word = false;
    this->periph->spi_tx_pending_bits = 0;
  }
}



void Spim_v3_tx_channel::check_state()
{
  if (this->has_tx_pending_word && !pending_word_event->is_enqueued() && this->periph->tx_pending_bits > 0)
  {
    top->event_enqueue(pending_word_event, 1);
  }
}


Spim_v3_cmd_channel::Spim_v3_cmd_channel(udma *top, Spim_periph_v3 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  pending_word_event = top->event_new(this, Spim_v3_cmd_channel::handle_pending_word);
}


void Spim_v3_cmd_channel::handle_ready_reqs()
{
  if (!this->has_tx_pending_word && !ready_reqs->is_empty())
  {
    vp::io_req *req = this->ready_reqs->pop();
    this->pending_req = req;
    this->tx_pending_word = *(uint32_t *)req->get_data();
    this->has_tx_pending_word = true;
    this->periph->check_state();
  }
}

void Spim_v3_cmd_channel::check_state()
{
  if (this->has_tx_pending_word && !pending_word_event->is_enqueued() && !this->periph->waiting_rx && !this->periph->waiting_tx_flush && !this->periph->waiting_tx)
  {
    top->event_enqueue(pending_word_event, 1);
  }
}

void Spim_periph_v3::handle_spi_pending_word(void *__this, vp::clock_event *event)
{
  Spim_periph_v3 *_this = (Spim_periph_v3 *)__this;
  bool raised_edge = false;

  if (_this->spi_rx_pending_bits > 0 && (_this->spi_tx_pending_bits == 0 || _this->is_full_duplex))
  {
    int nb_bits = _this->qpi ? 4 : 1;
    unsigned int received_bits =  _this->qpi ? _this->rx_received_bits & ((1<<nb_bits)-1) : (_this->rx_received_bits >> 1) & 1;
    _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->clkdiv;

    _this->nb_received_bits += nb_bits;
    _this->spi_rx_pending_bits -= nb_bits;
    if (!_this->is_full_duplex)
      _this->cmd_pending_bits -= nb_bits;

    int bit_index;
    int shift;

    if (_this->spi_lsb_first)
      bit_index = _this->rx_bit_offset + _this->rx_counter_bits;
    else
      bit_index = _this->rx_bit_offset + _this->spi_bitsword - _this->rx_counter_bits;


    if (_this->spi_qpi)
    {
      shift = _this->spi_lsb_first ? bit_index : bit_index - 3;

      _this->rx_pending_word &= ~(0xf << shift);
      _this->rx_pending_word |= (received_bits & 0xf) << shift;

      _this->rx_counter_bits += 4;
    }
    else
    {
      shift = bit_index;

      _this->rx_pending_word &= ~(0x1 << bit_index);
      _this->rx_pending_word |= (received_bits & 0x1) << bit_index;

      _this->rx_counter_bits += 1;
    }


    _this->top->get_trace()->msg("Sampled bits (nb_bits: %d, shift: %d, value: 0x%x, pending_word: 0x%x, pending_word_bits: %d)\n", nb_bits, shift, received_bits, _this->rx_pending_word, _this->nb_received_bits);

    if (!_this->qspim_itf.is_bound())
    {
      _this->top->warning.force_warning("Trying to receive from SPIM interface while it is not connected\n");
    }
    else
    {
      if (!_this->is_full_duplex) {
        _this->qspim_itf.sync_cycle(0, 0, 0, 0, 0
      );
      }
    }


    if (_this->rx_counter_bits == _this->spi_bitsword + 1)
    {
      _this->rx_counter_bits = 0;
      _this->rx_bit_offset += _this->spi_wordtrans == 0 ? 0 : _this->spi_wordtrans == 1 ? 16 : 8;
      _this->rx_counter_transf++;
      if (_this->rx_counter_transf == 1<<_this->spi_wordtrans)
      {
        _this->top->get_trace()->msg("End of word transfer, pushing word (value: 0x%x)\n", _this->rx_pending_word);

        (static_cast<Spim_v3_rx_channel *>(_this->channel0))->push_data((uint8_t *)&_this->rx_pending_word, 4);
        
        _this->rx_counter_transf = 0;
        _this->rx_bit_offset = 0;
        _this->nb_received_bits = 0;
        _this->rx_pending_word = 0x57575757;
      }
    }

    if (_this->spi_rx_pending_bits <= 0)
    {
      _this->is_full_duplex = false;
      _this->waiting_rx = false;
      _this->channel1->handle_ready_reqs();
      _this->channel2->handle_ready_reqs();
    }
  }

  if (_this->spi_tx_pending_bits > 0)
  {
    _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->clkdiv;


    int bit_index;
    int shift;
    int nb_bits = _this->spi_qpi ? 4 : 1;

    if (_this->spi_lsb_first)
      bit_index = _this->tx_bit_offset + _this->tx_counter_bits;
    else
      bit_index = _this->tx_bit_offset + _this->spi_bitsword - _this->tx_counter_bits;

    if (_this->spi_qpi)
    {
      shift = _this->spi_lsb_first ? bit_index : bit_index - 3;
      _this->tx_counter_bits += 4;
    }
    else
    {
      shift = bit_index;
      _this->tx_counter_bits += 1;
    }

    unsigned int bits = ARCHI_REG_FIELD_GET(_this->spi_tx_pending_word, shift, nb_bits);
    _this->top->get_trace()->msg("Sending bits (nb_bits: %d, shift: %d, value: 0x%x)\n", nb_bits, shift, bits);

    raised_edge = true;

    if (!_this->qspim_itf.is_bound())
    {
      _this->top->warning.force_warning("Trying to send to SPIM interface while it is not connected\n");
    }
    else
    {
      _this->qspim_itf.sync_cycle(
        (bits >> 0) & 1, (bits >> 1) & 1, (bits >> 2) & 1, (bits >> 3) & 1, (1<<nb_bits)-1
      );
    }

    if (_this->tx_counter_bits == _this->spi_bitsword + 1)
    {
      _this->tx_counter_bits = 0;
      _this->tx_bit_offset += _this->spi_wordtrans == 0 ? 0 : _this->spi_wordtrans == 1 ? 16 : 8;
      _this->tx_counter_transf++;

      if (_this->tx_counter_transf == 1<<_this->spi_wordtrans)
      {
        _this->tx_counter_transf = 0;
        _this->tx_bit_offset = 0;
      }
    }


    _this->spi_tx_pending_bits -= nb_bits;

    if (_this->waiting_tx_flush && _this->spi_tx_pending_bits <= 0)
    {
      _this->waiting_tx_flush = false;
    }
  }

  _this->check_state();
}

void Spim_v3_cmd_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
  Spim_v3_cmd_channel *_this = (Spim_v3_cmd_channel *)__this;

  _this->handle_data(_this->tx_pending_word);
  _this->periph->check_state();
}

void Spim_v3_cmd_channel::handle_eot(bool cs_keep)
{
  if (!cs_keep)
  {
    if (!periph->qspim_itf.is_bound())
      periph->top->warning.force_warning("Trying to set chip select to unbound QSPIM interface\n");
    else
    {
      trace.msg("Deactivating chip select\n");
      periph->qspim_itf.cs_sync(this->cs, 0);
    }
  }

  if (this->gen_eot_with_evt && this->periph->eot_event != -1)
  {
    this->top->trigger_event(this->periph->eot_event);
  }
}

bool Spim_periph_v3::push_rx_to_spi(int nb_bits, int qpi, int lsb_first, int bitsword, int wordtrans)
{
  if (this->spi_tx_pending_bits == 0)
  {
    this->top->get_trace()->msg("Forwarding read data to input buffer (nb_bits: %d, qpi: %d, lsb_first: %d, bitsword: %d, wordtrans: %d)\n", nb_bits, lsb_first, bitsword, wordtrans);

    this->spi_qpi = this->qpi;
    this->spi_lsb_first = this->lsb_first;
    this->spi_bitsword = this->bitsword;
    this->spi_wordtrans = this->wordtrans;
    this->spi_rx_pending_bits = nb_bits;

    return true;
  }
  return false;
}

bool Spim_periph_v3::push_tx_to_spi(uint32_t value, int nb_bits, int qpi, int lsb_first, int bitsword, int wordtrans)
{
  if (this->spi_tx_pending_bits == 0)
  {
    this->top->get_trace()->msg("Forwarding data to output buffer (nb_bits: %d, value: 0x%x, qpi: %d, lsb_first: %d, bitsword: %d, wordtrans: %d)\n", nb_bits, value, qpi, lsb_first, bitsword, wordtrans);
    this->spi_tx_pending_word = value;
    this->spi_tx_pending_bits = nb_bits;
    this->spi_qpi = qpi;
    this->spi_lsb_first = lsb_first;
    this->spi_bitsword = bitsword;
    this->spi_wordtrans = wordtrans;
    this->tx_bit_offset = 0;
    this->tx_counter_bits = 0;
    this->tx_counter_transf = 0;
    this->qpi = qpi;
    this->lsb_first = lsb_first;
    this->bitsword = bitsword;
    this->wordtrans = wordtrans;
    return true;
  }
  return false;
}

bool Spim_v3_cmd_channel::push_tx_to_spi(uint32_t value, int nb_bits)
{
  return this->periph->push_tx_to_spi(value, nb_bits, this->qpi, this->lsb_first, this->bitsword, this->wordtrans);
}

bool Spim_v3_cmd_channel::push_rx_to_spi(int nb_bits)
{
  return this->periph->push_rx_to_spi(nb_bits, this->qpi, this->lsb_first, this->bitsword, this->wordtrans);
}

void Spim_v3_cmd_channel::handle_data(uint32_t data)
{
  if (this->periph->cmd_pending_bits <= 0)
  {
    this->command = data >> SPI_CMD_ID_OFFSET;
  }

  bool handled_all = true;

  switch (this->command)
  {
    case SPI_CMD_CFG_ID: {
      this->periph->clkdiv = (data >> SPI_CMD_CFG_CLK_DIV_OFFSET) & ((1<<SPI_CMD_CFG_CLK_DIV_WIDTH)-1);
      unsigned int cpol = (data >> SPI_CMD_CFG_CPOL_OFFSET) & 1;
      unsigned int cpha = (data >> SPI_CMD_CFG_CPHA_OFFSET) & 1;
      trace.msg("Handling command CFG (clock_div: %d, cpol: %d, cpha: %d)\n", this->periph->clkdiv, cpol, cpha);
      //period = top->getEngine()->getPeriod() * clockDiv * 2;
      break;
    }

    case SPI_CMD_SOT_ID: {
      this->cs = (data >> SPI_CMD_SOT_CS_OFFSET) & ((1<<SPI_CMD_SOT_CS_WIDTH)-1);
      trace.msg("Handling command SOT (cs: %d)\n", this->cs);
      if (!periph->qspim_itf.is_bound())
        periph->top->warning.force_warning("Trying to set chip select to unbound QSPIM interface\n");
      else
        periph->qspim_itf.cs_sync(this->cs, 1);
      break;
    }

    case SPI_CMD_SEND_BITS_ID: {
      unsigned short bits = (data >> SPI_CMD_SEND_BITS_BITS_OFFSET) & ((1<<SPI_CMD_SEND_BITS_BITS_WIDTH)-1);
      unsigned int size = ((data>>SPI_CMD_SEND_BITS_SIZE_OFFSET) & ((1<<SPI_CMD_SEND_BITS_SIZE_WIDTH)-1)) + 1;
      int qpi = (data>>SPI_CMD_SEND_BITS_QPI_OFFSET) & 0x1;
      trace.msg("Handling command SEND_BITS (bits: 0x%x, size: %d, qpi: %d)\n", bits, size, qpi);

      this->qpi = qpi;
      this->lsb_first = false;
      this->bitsword = size - 1;
      this->wordtrans = 0;

      if (!this->push_tx_to_spi(bits, size))
      {
        handled_all = false;
        this->periph->waiting_tx_flush = true;
      }
      break;
    }

    case SPI_CMD_DUMMY_ID: {
      int cycles = (data >> SPI_CMD_DUMMY_CYCLE_OFFSET) & ((1<<SPI_CMD_DUMMY_CYCLE_WIDTH)-1);
      trace.msg("Handling command DUMMY (cycles: %d)\n", cycles);
      break;
    }

    case SPI_CMD_WAIT_ID: {
      trace.msg("Handling command WAIT\n");
      //waitingEvent = (data >> SPI_CMD_WAIT_EVENT_OFFSET) & ((1<<SPI_CMD_WAIT_EVENT_WIDTH)-1);
      //isReadyToExecCommand = false;
      break;
    }

    case SPI_CMD_TX_DATA_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        unsigned int size = ((data >> SPI_CMD_TX_DATA_SIZE_OFFSET) & ((1<<SPI_CMD_TX_DATA_SIZE_WIDTH)-1))+1;
        this->periph->bitsword = ((data >> SPI_CMD_TX_DATA_BITSWORD_OFFSET) & ((1<<SPI_CMD_TX_DATA_BITSWORD_WIDTH)-1));
        this->periph->wordtrans = ((data >> SPI_CMD_TX_DATA_WORDTRANS_OFFSET) & ((1<<SPI_CMD_TX_DATA_WORDTRANS_WIDTH)-1));
        this->periph->lsb_first = ARCHI_REG_FIELD_GET(data, SPI_CMD_TX_DATA_LSBFIRST_OFFSET, 1);
        this->periph->qpi = ARCHI_REG_FIELD_GET(data, SPI_CMD_TX_DATA_QPI_OFFSET, 1);
        trace.msg("Handling command TX_DATA (size: %d, lsb_first: %d, qpi: %d, bitsword: %d, wordtrans: %d)\n", size, this->periph->lsb_first, this->periph->qpi, this->periph->bitsword, this->periph->wordtrans);
        this->periph->waiting_tx = true;
        this->periph->tx_pending_bits = size*(this->periph->bitsword+1);
      }
      break;
    }

    case SPI_CMD_RX_DATA_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        unsigned int size = ((data >> SPI_CMD_RX_DATA_SIZE_OFFSET) & ((1<<SPI_CMD_RX_DATA_SIZE_WIDTH)-1))+1;
        this->periph->bitsword = ((data >> SPI_CMD_RX_DATA_BITSWORD_OFFSET) & ((1<<SPI_CMD_RX_DATA_BITSWORD_WIDTH)-1));
        this->periph->wordtrans = ((data >> SPI_CMD_RX_DATA_WORDTRANS_OFFSET) & ((1<<SPI_CMD_RX_DATA_WORDTRANS_WIDTH)-1));
        this->periph->lsb_first = ARCHI_REG_FIELD_GET(data, SPI_CMD_RX_DATA_LSBFIRST_OFFSET, 1);
        this->periph->qpi = ARCHI_REG_FIELD_GET(data, SPI_CMD_RX_DATA_QPI_OFFSET, 1);
        trace.msg("Handling command RX_DATA (raw: 0x%x, size: %d, lsb_first: %d, qpi: %d, bitsword: %d, wordtrans: %d)\n", data, size, periph->lsb_first, this->periph->qpi, this->periph->bitsword, this->periph->wordtrans);
        this->periph->cmd_pending_bits = size*(this->periph->bitsword + 1);

        this->periph->rx_bit_offset = 0;
        this->periph->rx_counter_bits = 0;
        this->periph->rx_counter_transf = 0;
      }

      if (!this->push_rx_to_spi(this->periph->cmd_pending_bits))
      {
        handled_all = false;
        this->periph->waiting_tx_flush = true;
      }
      else
      {
        this->periph->cmd_pending_bits = 0;
        this->periph->waiting_rx = true;
      }

      //int size = (bits+7)/8;
      //uint8_t data[(size+3) & 0xfffffffc];
      //int64_t latency = 0;
      //if (checkCs(cmdCs)) break;
      //spiPorts[cmdCs]->bsData((uint8_t *)data, bits, &latency);
      //// TODO this should be controlled by the datasize
      //if (byte_align)
      //{
      //  for (int i=0; i<size/4; i++) {
      //    ((int *)data)[i] = swapData(((int *)data)[i], 4);
      //  }
      //}
      //rxChannel->enqueueData(data, size, 0);
      //nextCommandCycles = top->getEngine()->getCycles() + latency;
      break;
    }

    case SPI_CMD_FUL_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        unsigned int size = ((data >> SPI_CMD_FUL_SIZE_OFFSET) & ((1<<SPI_CMD_FUL_SIZE_WIDTH)-1))+1;
        this->periph->bitsword = ((data >> SPI_CMD_FUL_BITSWORD_OFFSET) & ((1<<SPI_CMD_FUL_BITSWORD_WIDTH)-1));
        this->periph->wordtrans = ((data >> SPI_CMD_FUL_WORDTRANS_OFFSET) & ((1<<SPI_CMD_FUL_WORDTRANS_WIDTH)-1));
        this->periph->lsb_first = ARCHI_REG_FIELD_GET(data, SPI_CMD_FUL_LSBFIRST_OFFSET, 1);
        trace.msg("Handling command FUL DUPLEX (size: %d, lsb_first: %d, qpi: %d, bitsword: %d, wordtrans: %d)\n", size, this->periph->lsb_first, this->periph->qpi, this->periph->bitsword, this->periph->wordtrans);
        this->periph->tx_pending_bits = size*(this->periph->bitsword+1);

        this->periph->spi_rx_pending_bits = size*(this->periph->bitsword+1);
        this->periph->is_full_duplex = true;
        this->periph->waiting_rx = true;


        this->periph->rx_bit_offset = 0;
        this->periph->rx_counter_bits = 0;
        this->periph->rx_counter_transf = 0;

        this->periph->spi_qpi = this->periph->qpi;
        this->periph->spi_lsb_first = this->periph->lsb_first;
        this->periph->spi_bitsword = this->periph->bitsword;
        this->periph->spi_wordtrans = this->periph->wordtrans;
      }
      break;
    }

    case SPI_CMD_RPT_ID: {
      unsigned short iter = (data >> SPI_CMD_RPT_NB_OFFSET) & ((1<<SPI_CMD_RPT_NB_WIDTH)-1);
      trace.msg("Handling command RPT (nbIter: %d)\n", iter);
      //pendingRepeatCount = iter;
      //if (!usingCommandCache) {
      //  fillingCommandCache = true;
      //  commandCacheIndex = 0;
      //}
      break;
    }
    case SPI_CMD_EOT_ID: {
      this->gen_eot_with_evt = (data >> SPI_CMD_EOT_GEN_EVT_OFFSET) & 1;
      int cs_keep = (data >> SPI_CMD_EOT_CS_KEEP_OFFSET) & 1;
      trace.msg("Handling command EOT (evt: %d)\n", this->gen_eot_with_evt);
      if (this->periph->tx_pending_bits > 0 || this->periph->spi_tx_pending_bits > 0)
      {
        trace.msg("Waiting TX flush (tx pending bits: %d, spi pending bits: %d)\n", this->periph->tx_pending_bits, this->periph->spi_tx_pending_bits);
        handled_all = false;
        this->periph->waiting_tx_flush = true;
      }
      else
      {
        this->handle_eot(cs_keep);
      }
      break;
    }
    case SPI_CMD_RPT_END_ID: {
      trace.msg("Handling command RPT_END\n");
      //pendingRepeatCount--;
      //if (pendingRepeatCount >= 0) {
      //  trace.msg("Repeating sequence (newCount: %d)\n", pendingRepeatCount);
      //  fillingCommandCache = false;
      //  usingCommandCache = true;
      //  isReadyToExecCommand = false;
      //  commandCacheIndex = 0;
      //} else {
      //  trace.msg("End of repeat reached\n");
      //  isReadyToExecCommand = true;
      //  usingCommandCache = false;
      //}
      break;
    }
    case SPI_CMD_RX_CHECK_ID: {
      trace.msg("Handling command RX_CHECK\n");
      //int mode = (data >> SPI_CMD_RX_CHECK_MODE_OFFSET) & ((1<<SPI_CMD_RX_CHECK_MODE_WIDTH)-1);
      //uint16_t value = (data >> SPI_CMD_RX_CHECK_VALUE_OFFSET) & ((1<<SPI_CMD_RX_CHECK_VALUE_WIDTH)-1);
      //uint16_t bits = ((data >> SPI_CMD_RX_CHECK_SIZE_OFFSET) & ((1<<SPI_CMD_RX_CHECK_SIZE_WIDTH)-1))+1;
      //int size = (bits+7)/8;
      //uint16_t data = 0;
      //int64_t latency = 0;
      //if (checkCs(cmdCs)) break;
      //spiPorts[cmdCs]->bsData((uint8_t *)&data, bits, &latency);
      ////data = swapData(data, 4);
      //rxChannel->enqueueData((uint8_t *)&data, size, 0);
      //nextCommandCycles = top->getEngine()->getCycles() + latency;
      //switch (mode) {
      //  case SPI_CMD_RX_CHECK_MODE_MATCH:
      //  reachedCheck = value == data;
      //  trace.msg("Checking exact match (result: %d, readValue: 0x%x, expected: 0x%x)\n", reachedCheck, data, value);
      //  break;
      //  case SPI_CMD_RX_CHECK_MODE_ONES:
      //  reachedCheck = (data & value) == value;
      //  trace.msg("Checking ones (result: %d, readValue: 0x%x, expected: 0x%x)\n", reachedCheck, data, value);
      //  break;
      //  case SPI_CMD_RX_CHECK_MODE_ZEROS:
      //  reachedCheck = (~data & ~value) == ~value;
      //  trace.msg("Checking zeros match (result: %d, readValue: 0x%x, expected: 0x%x)\n", reachedCheck, data, value);
      //  break;
      //}
      break;
    }

    default:
      periph->top->warning.force_warning("Received unknown SPI command: %x\n", command);
  }

  #if 0
  if (has_pending_word)
  {
    ready_reqs->push(req);
  }
  else
  {
    pending_word = *(uint32_t *)req->get_data();
    has_pending_word = true;
    trace.msg("Store L2 data to shift register (value: 0x%x)\n", pending_word);
  }
  #endif

  if (handled_all)
    this->has_tx_pending_word = false;

  if (!this->has_tx_pending_word && this->pending_req)
  {
    this->handle_ready_req_end(this->pending_req);
    this->pending_req = NULL;
  }

  this->handle_ready_reqs();
  this->periph->check_state();
}

vp::io_req_status_e Spim_periph_v3::custom_req(vp::io_req *req, uint64_t offset)
{
  return this->channel2->req(req, offset);
}

void Spim_v3_cmd_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);

  if (active)
  {
    this->has_tx_pending_word = false;
  }
}

void Spim_v3_rx_channel::reset(bool active)
{
  Udma_rx_channel::reset(active);

  if (active)
  {
    this->current_byte = 0;
    this->current_bit = 0;
  }
}
