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
#include "archi/utils.h"
#include "vp/itf/i2s.hpp"


I2s_periph_v1::I2s_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "i2s" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new I2s_rx_channel(top, this, 0, UDMA_EVENT_ID(id), itf_name + "_0");
  channel1 = new I2s_rx_channel(top, this, 1, UDMA_EVENT_ID(id)+1, itf_name + "_1");

  top->new_slave_port(this, "i2s" + std::to_string(itf_id*2), &this->ch_itf[0]);
  top->new_slave_port(this, "i2s" + std::to_string(itf_id*2+1), &this->ch_itf[1]);

  this->ch_itf[0].set_sync_meth_muxed(&I2s_periph_v1::rx_sync, 0);
  this->ch_itf[1].set_sync_meth_muxed(&I2s_periph_v1::rx_sync, 1);

  this->top->new_reg(itf_name + "cfg_ext", &this->r_cfg_ext, 0);
  this->top->new_reg(itf_name + "cfg_clkgen0", &this->r_cfg_clkgen0, 0);
  this->top->new_reg(itf_name + "cfg_clkgen1", &this->r_cfg_clkgen1, 0);
  this->top->new_reg(itf_name + "chmode", &this->r_chmode, 0);
  this->top->new_reg(itf_name + "filt_ch0", &this->r_filt_ch0, 0);
  this->top->new_reg(itf_name + "filt_ch1", &this->r_filt_ch1, 0);

  this->clkgen0_event = this->top->event_new(this, I2s_periph_v1::clkgen_event_routine);
  this->clkgen0_event->get_args()[0] = (void *)0;

  this->clkgen1_event = this->top->event_new(this, I2s_periph_v1::clkgen_event_routine);
  this->clkgen1_event->get_args()[0] = (void *)1;
}
 

void I2s_periph_v1::reset(bool active)
{
  Udma_periph::reset(active);
}



void I2s_periph_v1::handle_clkgen_tick(int clkgen)
{
  this->trace.msg("Clock edge (clkgen: %d, sck: %d, ws: %d)\n", clkgen, this->sck[clkgen], this->current_ws[clkgen]);

  if (this->r_chmode.ch0_mode_get() == clkgen)
    this->ch_itf[0].sync(this->sck[clkgen], this->current_ws[clkgen], 0);

  if (this->r_chmode.ch1_mode_get() == clkgen)
    this->ch_itf[1].sync(this->sck[clkgen], this->current_ws[clkgen], 0);

  this->sck[clkgen] ^= 1;

  if (this->sck[clkgen] == 0)
  {
    int bits_word = clkgen == 0 ? this->r_cfg_clkgen0.bits_word_get() : this->r_cfg_clkgen1.bits_word_get();

    this->current_ws_count[clkgen]++;

    if (this->current_ws_count[clkgen] >= bits_word + 1)
    {
      this->current_ws_count[clkgen] = 0;
      this->current_ws[clkgen] ^= 1;

      if (this->current_ws[clkgen] == 0)
      {
        if (this->r_chmode.ch0_mode_get() == clkgen)
        {
          (static_cast<I2s_rx_channel *>(this->channel0))->wait_ws_start = false;
        }
      
        if (this->r_chmode.ch1_mode_get() == clkgen)
        {
          (static_cast<I2s_rx_channel *>(this->channel1))->wait_ws_start = false;
        }
      }
    }
  }

  if (clkgen == 0)
    this->check_clkgen0();
  else
    this->check_clkgen1();
}



void I2s_periph_v1::clkgen_event_routine(void *__this, vp::clock_event *event)
{
  I2s_periph_v1 *_this = (I2s_periph_v1 *)__this;

  int clkgen = *(int *)&(event->get_args()[0]);

  _this->handle_clkgen_tick(clkgen);
}



vp::io_req_status_e I2s_periph_v1::reset_clkgen0()
{
  if (this->clkgen0_event->is_enqueued())
    this->top->get_periph_clock()->cancel(this->clkgen0_event);

  this->sck[0] = 0;
  this->current_ws_count[0] = 0;
  this->current_ws[0] = 0;
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::reset_clkgen1()
{
  if (this->clkgen1_event->is_enqueued())
    this->top->get_periph_clock()->cancel(this->clkgen1_event);

  this->sck[1] = 0;
  this->current_ws_count[1] = 0;
  this->current_ws[1] = 0;
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::check_clkgen0()
{
  if (this->r_cfg_clkgen0.clk_en_get() && !this->clkgen0_event->is_enqueued())
    this->top->get_periph_clock()->enqueue(this->clkgen0_event, this->r_cfg_clkgen0.clk_div_get() + 1);
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::check_clkgen1()
{
  if (this->r_cfg_clkgen1.clk_en_get() && !this->clkgen1_event->is_enqueued())
    this->top->get_periph_clock()->enqueue(this->clkgen1_event, this->r_cfg_clkgen1.clk_div_get() + 1);
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::cfg_ext_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_cfg_ext.access(reg_offset, size, data, is_write);

  if (is_write)
    this->trace.msg("Modified I2S_CFG_EXT (bits_word: %d)\n",
      this->r_cfg_ext.ext_bits_word_get()
    );
  
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::cfg_clkgen0_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_cfg_clkgen0.access(reg_offset, size, data, is_write);

  if (is_write)
    this->trace.msg("Modified I2S_CFG_CLKGEN0 (bits_word: %d, clk_en: %d, clk_div: %d)\n",
      this->r_cfg_clkgen0.bits_word_get(), this->r_cfg_clkgen0.clk_en_get(), this->r_cfg_clkgen0.clk_div_get()
    );
  
  if (this->r_chmode.ch0_mode_get() == 0)
  {
    (static_cast<I2s_rx_channel *>(this->channel0))->wait_ws_start = true;
  }

  if (this->r_chmode.ch0_mode_get() == 0)
  {
    (static_cast<I2s_rx_channel *>(this->channel0))->wait_ws_start = true;
  }

  this->check_clkgen0();

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::cfg_clkgen1_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_cfg_clkgen1.access(reg_offset, size, data, is_write);

  if (is_write)
    this->trace.msg("Modified I2S_CFG_CLKGEN1 (bits_word: %d, clk_en: %d, clk_div: %d)\n",
      this->r_cfg_clkgen1.bits_word_get(), this->r_cfg_clkgen1.clk_en_get(), this->r_cfg_clkgen1.clk_div_get()
    );
  
  if (this->r_chmode.ch1_mode_get() == 1)
  {
    (static_cast<I2s_rx_channel *>(this->channel0))->wait_ws_start = true;
  }

  if (this->r_chmode.ch1_mode_get() == 1)
  {
    (static_cast<I2s_rx_channel *>(this->channel0))->wait_ws_start = true;
  }

  this->check_clkgen1();

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::chmode_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_chmode.access(reg_offset, size, data, is_write);

  if (is_write)
  {
    this->trace.msg("Modified I2S_CHMODE (raw: 0x%x, ch0_mode: %d, ch0_lsb_first: %d, ch0_pdm_usefilter: %d, ch0_pdm_en: %d, ch0_useddr: %d, ch1_mode: %d, ch1_lsb_first: %d, ch1_pdm_usefilter: %d, ch1_pdm_en: %d, ch1_useddr: %d)\n",
      this->r_chmode.get(), this->r_chmode.ch0_mode_get(), this->r_chmode.ch0_lsb_first_get(), this->r_chmode.ch0_pdm_usefilter_get(), this->r_chmode.ch0_pdm_en_get(), this->r_chmode.ch0_useddr_get(),
      this->r_chmode.ch1_mode_get(), this->r_chmode.ch1_lsb_first_get(), this->r_chmode.ch1_pdm_usefilter_get(), this->r_chmode.ch1_pdm_en_get(), this->r_chmode.ch1_useddr_get()
    );
  }

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::filt_ch0_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_filt_ch0.access(reg_offset, size, data, is_write);

  if (is_write)
    this->trace.msg("Modified I2S_FILT_CH0 (raw: 0x%x, decimation: %d, shift: %d)\n", this->r_filt_ch0.get(), this->r_filt_ch0.decimation_get(), this->r_filt_ch0.shift_get());
  
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph_v1::filt_ch1_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_filt_ch1.access(reg_offset, size, data, is_write);

  if (is_write)
    this->trace.msg("Modified I2S_FILT_CH1 (decimation: %d, shift: %d)\n", this->r_filt_ch1.decimation_get(), this->r_filt_ch1.shift_get());
  
  return vp::io_req_status_e::IO_REQ_OK;
}




vp::io_req_status_e I2s_periph_v1::custom_req(vp::io_req *req, uint64_t offset)
{
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  this->trace.msg("I2S custom register access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  offset += UDMA_CHANNEL_CUSTOM_OFFSET;

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  switch (reg_id) {
    case UDMA_I2S_CFG_EXT_OFFSET/4:
      err = this->cfg_ext_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_CFG_CLKGEN0_OFFSET/4:
      err = this->cfg_clkgen0_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_CFG_CLKGEN1_OFFSET/4:
      err = this->cfg_clkgen1_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_CHMODE_OFFSET/4:
      err = this->chmode_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_FILT_CH0_OFFSET/4:
      err = this->filt_ch0_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_FILT_CH1_OFFSET/4:
      err = this->filt_ch1_req(reg_offset, size, is_write, data);
      break;

  }

  if (err != vp::IO_REQ_OK)
    goto error; 


  return vp::IO_REQ_OK;

error:
  this->trace.force_warning("I2S invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}



void I2s_periph_v1::rx_sync(void *__this, int sck, int ws, int sd, int channel)
{
  I2s_periph_v1 *_this = (I2s_periph_v1 *)__this;

  _this->trace.msg("Received data (channel: %d, sck: %d, ws: %d, sd: %d)\n", channel, sck, ws, sd);

  if (channel == 0)
    (static_cast<I2s_rx_channel *>(_this->channel0))->handle_rx_bit(sck, ws, sd);
  else
  {
    (static_cast<I2s_rx_channel *>(_this->channel1))->handle_rx_bit(sck, ws, sd);
  }

  int clkgen = channel == 0 ? _this->r_chmode.ch0_mode_get() : _this->r_chmode.ch1_mode_get();

  if (_this->current_ws_count[clkgen] == 0)
  {
    if (channel == 0)
    {
      (static_cast<I2s_rx_channel *>(_this->channel0))->pending_bits[0] = 0;
      (static_cast<I2s_rx_channel *>(_this->channel0))->pending_bits[1] = 0;
    }
    else
    {
      (static_cast<I2s_rx_channel *>(_this->channel1))->pending_bits[0] = 0;
      (static_cast<I2s_rx_channel *>(_this->channel1))->pending_bits[1] = 0;
    }
  }
}



I2s_rx_channel::I2s_rx_channel(udma *top, I2s_periph_v1 *periph, int id, int event_id, string name) : Udma_rx_channel(top, event_id, name), periph(periph), id(id)
{
  for (int i=0; i<2; i++)
  {
    this->filters[i] = new I2s_cic_filter();
  }
}

void I2s_rx_channel::reset(bool active)
{
  Udma_rx_channel::reset(active);

  this->pending_bits[0] = 0;
  this->pending_bits[1] = 0;

  for (int i=0; i<2; i++)
  {
    this->filters[i]->reset();
  }
}


I2s_cic_filter::I2s_cic_filter() : pdm_pending_bits(0)
{
  this->pdm_y1_old = 0;
  this->pdm_y2_old = 0;
  this->pdm_y3_old = 0;
  this->pdm_y4_old = 0;
  this->pdm_y5_old = 0;
  this->pdm_z1_old = 0;
  this->pdm_z2_old = 0;
  this->pdm_z3_old = 0;
  this->pdm_z4_old = 0;
  this->pdm_z5_old = 0;
  this->pdm_zin1_old = 0;
  this->pdm_zin2_old = 0;
  this->pdm_zin3_old = 0;
  this->pdm_zin4_old = 0;
  this->pdm_zin5_old = 0;
}


void I2s_cic_filter::reset()
{
  this->pdm_pending_bits = 0;
}



bool I2s_cic_filter::handle_bit(int din, int pdm_decimation, int pdm_shift, uint32_t *dout)
{

  int64_t value = din == 0 ? -1 : 1;

  int64_t y1 = this->pdm_y1_old + value;
  int64_t y2 = this->pdm_y2_old + this->pdm_y1_old;
  int64_t y3 = this->pdm_y3_old + this->pdm_y2_old;
  int64_t y4 = this->pdm_y4_old + this->pdm_y3_old;
  int64_t y5 = this->pdm_y5_old + this->pdm_y4_old;

  //gv_trace_dumpMsg(&channel->trace, "Enqueueing bit to CIC filter (channel: %s, bit: %d, value: %d, pendingBits: %d, decimator: %d)\n", id ? "R" : "L", din, value, pdm_pending_bits, pdm_decimation);

  this->pdm_y1_old = y1;
  this->pdm_y2_old = y2;
  this->pdm_y3_old = y3;
  this->pdm_y4_old = y4;
  this->pdm_y5_old = y5;

        //printf("INT %lx %lx %lx %lx %lx\n", y1, y2, y3, y4, y5);

  this->pdm_pending_bits++;
  if (this->pdm_pending_bits == pdm_decimation)
  {
    this->pdm_pending_bits = 0;

    int64_t z1 = y5         - this->pdm_zin1_old;
    int64_t z2 = this->pdm_z1_old - this->pdm_zin2_old;
    int64_t z3 = this->pdm_z2_old - this->pdm_zin3_old;
    int64_t z4 = this->pdm_z3_old - this->pdm_zin4_old;
    int64_t z5 = this->pdm_z4_old - this->pdm_zin5_old;

    this->pdm_zin1_old = y5;
    this->pdm_zin2_old = this->pdm_z1_old;
    this->pdm_zin3_old = this->pdm_z2_old;
    this->pdm_zin4_old = this->pdm_z3_old;
    this->pdm_zin5_old = this->pdm_z4_old;

    this->pdm_z1_old = z1;
    this->pdm_z2_old = z2;
    this->pdm_z3_old = z3;
    this->pdm_z4_old = z4;
    this->pdm_z5_old = z5;

    int64_t result = z5 >> pdm_shift;


    //printf("Reached decimator, enqueueing value (value: %lx)\n", result);




    *dout = result;

    return true;
  }

  return false;
}



void I2s_rx_channel::handle_rx_bit(int sck, int ws, int bit)
{
  if (this->wait_ws_start)
  {
    return;
  }

  int pdm = this->id == 0 ? this->periph->r_chmode.ch0_pdm_en_get() : this->periph->r_chmode.ch1_pdm_en_get();
  int ddr = this->id == 0 ? this->periph->r_chmode.ch0_useddr_get() : this->periph->r_chmode.ch1_useddr_get();
  int lsb_first = this->id == 0 ? this->periph->r_chmode.ch0_lsb_first_get() : this->periph->r_chmode.ch1_lsb_first_get();
  int width;
  bool push = false;
  uint32_t result = 0;
  int ch_mode = this->id == 0 ? this->periph->r_chmode.ch0_mode_get() : this->periph->r_chmode.ch1_mode_get();

  switch (ch_mode)
  {
    case 0: width = this->periph->r_cfg_clkgen0.bits_word_get() + 1; break;
    case 1: width = this->periph->r_cfg_clkgen1.bits_word_get() + 1; break;
    case 2:
    case 3: width = this->periph->r_cfg_ext.ext_bits_word_get() + 1; break;
  }

  if (pdm)
  {
    int filter = this->id == 0 ? this->periph->r_chmode.ch0_pdm_usefilter_get() : this->periph->r_chmode.ch1_pdm_usefilter_get();
    int decimation = this->id == 0 ? this->periph->r_filt_ch0.decimation_get() : this->periph->r_filt_ch1.decimation_get();
    int shift = this->id == 0 ? this->periph->r_filt_ch0.shift_get() : this->periph->r_filt_ch1.shift_get();
    int filter_id = ddr && sck;

    push = this->filters[filter_id]->handle_bit(bit, decimation + 1, (7 - shift)*5, &result);
  }
  else
  {
    int sample_id = ddr && sck;

    if (lsb_first)
    {
      this->pending_samples[sample_id] = (this->pending_samples[sample_id] >> 1) | (bit << (width - 1));
    }
    else
    {
      this->pending_samples[sample_id] = (this->pending_samples[sample_id] << 1) | bit;
    }

    this->pending_bits[sample_id]++;

    if (this->pending_bits[sample_id] == width)
    {
      this->pending_bits[sample_id] = 0;
      push = true;
      result = this->pending_samples[sample_id];
    }
  }

  if (push)
  {
    if (width < 32)
      result = result & ((1<<width)-1);
    int bytes = width <= 8 ? 1 : width <= 16 ? 2 : 4;
    if (this->push_data((uint8_t *)&result, bytes))
      this->wait_ws_start = true;
  }
}
