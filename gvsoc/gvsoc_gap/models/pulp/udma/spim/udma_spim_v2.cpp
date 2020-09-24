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
#include "archi/udma/spim/udma_spim_v2.h"
#include "archi/utils.h"
#include "vp/itf/qspim.hpp"




Spim_periph_v2::Spim_periph_v2(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "spim" + std::to_string(itf_id);

  channel0 = new Spim_rx_channel(top, this, UDMA_CHANNEL_ID(id), itf_name + "_rx");
  channel1 = new Spim_tx_channel(top, this, UDMA_CHANNEL_ID(id) + 1, itf_name + "_tx");

  qspim_itf.set_sync_meth(&Spim_periph_v2::slave_sync);
  top->new_master_port(this, itf_name, &qspim_itf);

  js::config *config = this->top->get_js_config()->get("spim/eot_events");
  if (config)
    this->eot_event = config->get_elem(itf_id)->get_int();
  else
    this->eot_event = -1;
}

void Spim_periph_v2::reset(bool active)
{
  Udma_periph::reset(active);

  if (active)
  {
    this->waiting_rx = false;
    this->waiting_tx = false;
    this->is_full_duplex = false;
    this->cmd_pending_bits = 0;
    this->nb_received_bits = 0;
    this->rx_pending_word = 0x57575757;
    this->tx_pending_word = 0x57575757;
    this->spi_rx_pending_bits = 0;
    this->clkdiv = 0;
    this->rx_count = 0;
  }
}

  
void Spim_periph_v2::slave_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask)
{
  Spim_periph_v2 *_this = (Spim_periph_v2 *)__this;
  (static_cast<Spim_rx_channel *>(_this->channel0))->handle_rx_bits(data_0, data_1, data_2, data_3, mask);
}

void Spim_rx_channel::handle_rx_bits(int data_0, int data_1, int data_2, int data_3, int mask)
{
  this->periph->received_bits = (data_3 << 3) | (data_2 << 2) | (data_1 << 1) | (data_0 << 0);
}

void Spim_tx_channel::check_state()
{
  if (this->has_tx_pending_word && !pending_word_event->is_enqueued() && !this->periph->waiting_rx && !this->periph->waiting_tx)
  {
    top->event_enqueue(pending_word_event, 1);
  }

  if ((this->spi_tx_pending_bits > 0 || (!this->periph->is_full_duplex && this->periph->spi_rx_pending_bits > 0)) && !pending_spi_word_event->is_enqueued())
  {
    int latency = 1;
    int64_t cycles = this->top->get_clock()->get_cycles();
    if (next_bit_cycle > cycles)
      latency = next_bit_cycle - cycles;

    top->event_enqueue(pending_spi_word_event, latency);
  }
}


Spim_tx_channel::Spim_tx_channel(udma *top, Spim_periph_v2 *periph, int id, string name)
: Udma_tx_channel(top, id, name), periph(periph)
{
  pending_word_event = top->event_new(this, Spim_tx_channel::handle_pending_word);

  pending_spi_word_event = top->event_new(this, Spim_tx_channel::handle_spi_pending_word);
}


void Spim_tx_channel::handle_ready_reqs()
{
  if (!this->has_tx_pending_word && !ready_reqs->is_empty())
  {
    vp::io_req *req = this->ready_reqs->pop();
    this->pending_req = req;
    this->tx_pending_word = *(uint32_t *)req->get_data();
    this->has_tx_pending_word = true;
    this->check_state();
  }
}

void Spim_tx_channel::handle_spi_pending_word(void *__this, vp::clock_event *event)
{
  Spim_tx_channel *_this = (Spim_tx_channel *)__this;
  bool raised_edge = false;

  if (_this->periph->spi_rx_pending_bits > 0 && (static_cast<Spim_rx_channel *>(_this->periph->channel0))->has_cmd() && (_this->spi_tx_pending_bits == 0 || _this->periph->is_full_duplex))
  {
    int nb_bits = _this->periph->qpi ? 4 : 1;
    _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->periph->clkdiv*2;

    _this->periph->nb_received_bits += nb_bits;
    _this->periph->spi_rx_pending_bits -= nb_bits;
    if (!_this->periph->is_full_duplex)
      _this->periph->cmd_pending_bits -= nb_bits;


  if (_this->periph->qpi)
  {
    uint32_t value = _this->periph->received_bits;

    if (_this->periph->byte_align)
    {
      _this->periph->rx_pending_word = value | (_this->periph->rx_pending_word << 4);
    }
    else
    {
      int byte = _this->periph->rx_count / 2;
      int shift = (1 - (_this->periph->rx_count & 1)) * 4 + byte*8;
      _this->periph->rx_pending_word = (_this->periph->rx_pending_word & ~((0xf << shift))) | (value << shift);

      _this->periph->rx_count++;
      if (_this->periph->rx_count == 8)
        _this->periph->rx_count = 0;
    }
  }
  else
  {
    uint32_t value = (_this->periph->received_bits >> 1) & 1;
    if (_this->periph->byte_align)
    {
      _this->periph->rx_pending_word = value | (_this->periph->rx_pending_word << 1);
    }
    else
    {
      int byte = _this->periph->rx_count / 8;
      int shift = 7 - (_this->periph->rx_count % 8) + byte*8;

      _this->periph->rx_pending_word = (_this->periph->rx_pending_word & ~((0x1 << shift))) | (value << shift);

      _this->periph->rx_count++;
      if (_this->periph->rx_count == 32)
        _this->periph->rx_count = 0;
    }
  }


    _this->top->get_trace()->msg("Received bits (nb_bits: %d, value: 0x%x, pending_word_bits: %d)\n", nb_bits, _this->periph->rx_pending_word & ((1<<nb_bits) - 1), _this->periph->nb_received_bits);


    if (_this->periph->nb_received_bits == 32 || _this->periph->spi_rx_pending_bits <= 0)
    {
      (static_cast<Spim_rx_channel *>(_this->periph->channel0))->push_data((uint8_t *)&_this->periph->rx_pending_word, 4);
      
      _this->periph->nb_received_bits = 0;
      _this->rx_pending_word = 0x57575757;
    }


    if (!_this->periph->qspim_itf.is_bound())
    {
      _this->top->get_trace()->warning("Trying to receive from SPIM interface while it is not connected\n");
    }
    else
    {
      if (!_this->periph->is_full_duplex)
        _this->periph->qspim_itf.sync_cycle(0, 0, 0, 0, 0
      );
    }

    if (_this->periph->spi_rx_pending_bits <= 0)
    {
      _this->periph->is_full_duplex = false;
      _this->periph->waiting_rx = false;
      _this->handle_ready_reqs();
    }
  }
  if (_this->spi_tx_pending_bits > 0)
  {
    int nb_bits = _this->spi_tx_quad ? 4 : 1;
    _this->next_bit_cycle = _this->top->get_clock()->get_cycles() + _this->periph->clkdiv*2;

    unsigned int bits;

    if (_this->spi_tx_byte_align)
    {
      bits = ARCHI_REG_FIELD_GET(_this->spi_tx_pending_word, 32 - nb_bits, nb_bits);
      _this->spi_tx_pending_word <<= nb_bits;
    }
    else
    {
      bits = ARCHI_REG_FIELD_GET(_this->spi_tx_pending_word, 8 - nb_bits, nb_bits);

      if (_this->spi_tx_quad)
      {
        if (_this->periph->tx_count)
        {
          _this->spi_tx_pending_word >>= 8;
          _this->periph->tx_count = 0;
        }
        else
        {
          _this->periph->tx_count = 1;
          _this->spi_tx_pending_word = (_this->spi_tx_pending_word & 0xFFFFFF00) | ((_this->spi_tx_pending_word & 0xF) << 4);
        }
      }
      else
      {
        if (_this->periph->tx_count == 7)
        {
          _this->spi_tx_pending_word >>= 8;

          _this->periph->tx_count = 0;
        }
        else
        {
          _this->periph->tx_count++;
          _this->spi_tx_pending_word = (_this->spi_tx_pending_word & 0xFFFFFF00) | ((_this->spi_tx_pending_word & 0x7F) << 1);
        }
      }
    }

    _this->top->get_trace()->msg("Sending bits (nb_bits: %d, value: 0x%x)\n", nb_bits, bits);

    raised_edge = true;

    if (!_this->periph->qspim_itf.is_bound())
    {
      _this->top->get_trace()->warning("Trying to send to SPIM interface while it is not connected\n");
    }
    else
    {
      _this->periph->qspim_itf.sync_cycle(
        (bits >> 0) & 1, (bits >> 1) & 1, (bits >> 2) & 1, (bits >> 3) & 1, (1<<nb_bits)-1
      );
    }
    _this->spi_tx_pending_bits -= nb_bits;

    if (_this->periph->waiting_tx && _this->spi_tx_pending_bits <= 0)
    {
      _this->periph->waiting_tx = false;
    }

    if (_this->spi_tx_pending_bits <= 0)
      _this->periph->tx_count = 0;
  }

  _this->check_state();
}

void Spim_tx_channel::handle_pending_word(void *__this, vp::clock_event *event)
{
  Spim_tx_channel *_this = (Spim_tx_channel *)__this;

  _this->handle_data(_this->tx_pending_word);
  _this->check_state();
}

void Spim_tx_channel::handle_eot()
{
  if (!periph->qspim_itf.is_bound())
    trace.warning("Trying to set chip select to unbound QSPIM interface\n");
  else
    periph->qspim_itf.cs_sync(this->cs, 0);

  if (this->gen_eot_with_evt && this->periph->eot_event != -1)
  {
    this->top->trigger_event(this->periph->eot_event);
  }
}

void Spim_tx_channel::handle_data(uint32_t data)
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
      trace.msg("Received command CFG (clock_div: %d, cpol: %d, cpha: %d)\n", this->periph->clkdiv, cpol, cpha);
      //period = top->getEngine()->getPeriod() * clockDiv * 2;
      break;
    }

    case SPI_CMD_SOT_ID: {
      this->cs = (data >> SPI_CMD_SOT_CS_OFFSET) & ((1<<SPI_CMD_SOT_CS_WIDTH)-1);
      trace.msg("Received command SOT (cs: %d)\n", this->cs);
      if (!periph->qspim_itf.is_bound())
        trace.warning("Trying to set chip select to unbound QSPIM interface\n");
      else
        periph->qspim_itf.cs_sync(this->cs, 1);
      break;
    }

    case SPI_CMD_SEND_CMD_ID: {
      unsigned short cmd = (data >> SPI_CMD_SEND_CMD_CMD_OFFSET) & ((1<<SPI_CMD_SEND_CMD_CMD_WIDTH)-1);
      unsigned int size = ((data>>SPI_CMD_SEND_CMD_SIZE_OFFSET) & ((1<<SPI_CMD_SEND_CMD_SIZE_WIDTH)-1)) + 1;
      this->periph->qpi = (data>>SPI_CMD_SEND_CMD_QPI_OFFSET) & 0x1;
      trace.msg("Received command SEND_CMD (cmd: 0x%x, size: %d, qpi: %d)\n", cmd, size, this->periph->qpi);
      if (this->spi_tx_pending_bits == 0)
      {
        this->spi_tx_pending_word = ((cmd >> 8) & 0xff) | (cmd << 8); 
        this->spi_tx_pending_bits = size;
        this->spi_tx_quad = this->periph->qpi;
        this->spi_tx_byte_align = false;
      }
      else
      {
        handled_all = false;
        this->periph->waiting_tx = true;     
      }
      break;
    }

    case SPI_CMD_SEND_ADDR_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        uint32_t addr_size = ((data >> SPI_CMD_SEND_ADDR_SIZE_OFFSET) & ((1<<SPI_CMD_SEND_ADDR_SIZE_WIDTH)-1)) + 1;
        this->periph->qpi = (data>>SPI_CMD_SEND_ADDR_QPI_OFFSET) & 0x1;
        trace.msg("Received command SEND_ADDR (size: %d, qpi: %d)\n", addr_size, this->periph->qpi);
        this->periph->cmd_pending_bits = addr_size;
      }
      else
      {
        if (this->spi_tx_pending_bits > 0)
        {
          handled_all = false;
          this->periph->waiting_tx = true;
        }
        else
        {
          this->spi_tx_quad = this->periph->qpi;
          this->spi_tx_byte_align = false;
          this->spi_tx_pending_word = __bswap_32(this->tx_pending_word);
          this->spi_tx_pending_bits = this->periph->cmd_pending_bits;
          this->periph->cmd_pending_bits = 0;

          //if (!this->spi_tx_byte_align) this->spi_tx_pending_word = __bswap_32(this->spi_tx_pending_word);
        }
      }
      break;
    }

    case SPI_CMD_DUMMY_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        int cycles = ((data >> SPI_CMD_DUMMY_CYCLE_OFFSET) & ((1<<SPI_CMD_DUMMY_CYCLE_WIDTH)-1)) + 1;
        trace.msg("Received command DUMMY (cycles: %d)\n", cycles);
        // TODO dirty hack to avoir adding wait cycles to spiflash model
        if (cycles <= 8)
          this->periph->cmd_pending_bits = cycles;
      }
      else
      {
        if (this->spi_tx_pending_bits > 0)
        {

          handled_all = false;
          this->periph->waiting_tx = true;
        }
        else
        {
          if (this->periph->qspim_itf.is_bound())
          {
            this->periph->qspim_itf.sync_cycle(0, 0, 0, 0, 0);
          }
          this->periph->cmd_pending_bits--;
          handled_all = false;
        }
      }
      break;
    }

    case SPI_CMD_WAIT_ID: {
      trace.msg("Received command WAIT\n");
      //waitingEvent = (data >> SPI_CMD_WAIT_EVENT_OFFSET) & ((1<<SPI_CMD_WAIT_EVENT_WIDTH)-1);
      //isReadyToExecCommand = false;
      break;
    }

    case SPI_CMD_TX_DATA_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        unsigned int bits = ((data >> SPI_CMD_TX_DATA_SIZE_OFFSET) & ((1<<SPI_CMD_TX_DATA_SIZE_WIDTH)-1))+1;
        this->periph->byte_align = (data >> SPI_CMD_TX_DATA_BYTE_ALIGN_OFFSET) & 1;
        this->periph->qpi = ARCHI_REG_FIELD_GET(data, SPI_CMD_TX_DATA_QPI_OFFSET, 1);
        trace.msg("Received command TX_DATA (size: %d, byte_align: %d, qpi: %d)\n", bits, this->periph->byte_align, this->periph->qpi);
        this->periph->cmd_pending_bits = bits;
      }
      else
      {
        if (this->spi_tx_pending_bits > 0)
        {
          handled_all = false;
        }
        else
        {
          int nb_bits = this->periph->cmd_pending_bits > 32 ? 32 : this->periph->cmd_pending_bits;
          this->spi_tx_byte_align = this->periph->byte_align;
          this->spi_tx_quad = this->periph->qpi;
          this->spi_tx_pending_word = this->tx_pending_word;
          this->spi_tx_pending_bits = nb_bits;
          this->periph->cmd_pending_bits -= nb_bits;
      
          //if (!this->spi_tx_byte_align) this->spi_tx_pending_word = __bswap_32(this->spi_tx_pending_word);
        }
      }
      break;
    }

    case SPI_CMD_RX_DATA_ID: {
      if (this->periph->cmd_pending_bits <= 0)
      {
        unsigned int bits = ((data >> SPI_CMD_RX_DATA_SIZE_OFFSET) & ((1<<SPI_CMD_RX_DATA_SIZE_WIDTH)-1))+1;
        this->periph->byte_align = (data >> SPI_CMD_RX_DATA_BYTE_ALIGN_OFFSET) & 1;
        this->periph->qpi = ARCHI_REG_FIELD_GET(data, SPI_CMD_RX_DATA_QPI_OFFSET, 1);
        trace.msg("Received command RX_DATA (size: %d, byte_align: %d, qpi: %d)\n", bits, periph->byte_align, this->periph->qpi);
        this->periph->cmd_pending_bits = bits;
        this->periph->spi_rx_pending_bits = bits;
        this->periph->waiting_rx = true;
        this->periph->rx_count = 0;
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
        unsigned int bits = ((data >> SPI_CMD_FUL_SIZE_OFFSET) & ((1<<SPI_CMD_FUL_SIZE_WIDTH)-1))+1;
        this->periph->byte_align = (data >> SPI_CMD_FUL_BYTE_ALIGN_OFFSET) & 1;
        trace.msg("Received command FULL_DUPLEX (size: %d, byte_align: %d)\n", bits, periph->byte_align);
        this->periph->cmd_pending_bits = bits;
        this->periph->spi_rx_pending_bits = bits;
        this->periph->is_full_duplex = true;
      }
      else
      {
        if (this->spi_tx_pending_bits > 0)
        {
          handled_all = false;
        }
        else
        {
          int nb_bits = this->periph->cmd_pending_bits > 32 ? 32 : this->periph->cmd_pending_bits;
          this->spi_tx_byte_align = this->periph->byte_align;
          this->spi_tx_quad = this->periph->qpi;
          this->spi_tx_pending_word = this->tx_pending_word;
          this->spi_tx_pending_bits = nb_bits;
          this->periph->cmd_pending_bits -= nb_bits;
          this->periph->rx_count = 0;
        }
      }
      break;

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

    case SPI_CMD_RPT_ID: {
      unsigned short iter = (data >> SPI_CMD_RPT_NB_OFFSET) & ((1<<SPI_CMD_RPT_NB_WIDTH)-1);
      trace.msg("Received command RPT (nbIter: %d)\n", iter);
      //pendingRepeatCount = iter;
      //if (!usingCommandCache) {
      //  fillingCommandCache = true;
      //  commandCacheIndex = 0;
      //}
      break;
    }
    case SPI_CMD_EOT_ID: {
      this->gen_eot_with_evt = (data >> SPI_CMD_EOT_GEN_EVT_OFFSET) & 1;
      trace.msg("Received command EOT (evt: %d)\n", this->gen_eot_with_evt);
      if (this->spi_tx_pending_bits > 0)
      {

        handled_all = false;
        this->periph->waiting_tx = true;
      }
      else
        this->handle_eot();
      break;
    }
    case SPI_CMD_RPT_END_ID: {
      trace.msg("Received command RPT_END\n");
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
      trace.msg("Received command RX_CHECK\n");
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
      trace.warning("Received unknown SPI command: %x\n", command);
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
}

void Spim_tx_channel::reset(bool active)
{
  Udma_tx_channel::reset(active);

  if (active)
  {
    this->next_bit_cycle = -1;
    this->has_tx_pending_word = false;
    this->spi_tx_pending_bits = 0;
  }
}

void Spim_rx_channel::reset(bool active)
{
  Udma_rx_channel::reset(active);
}
