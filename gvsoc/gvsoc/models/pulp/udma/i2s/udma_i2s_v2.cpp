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


#include "udma_i2s_v2.hpp"
#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/i2s.hpp"



I2s_periph::I2s_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "i2s" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new I2s_rx_channel(top, this, 0, UDMA_EVENT_ID(id), itf_name + "_0");
  channel1 = new I2s_rx_channel(top, this, 1, UDMA_EVENT_ID(id)+1, itf_name + "_1");

  top->new_slave_port(this, "i2s" + std::to_string(itf_id*2), &this->ch_itf[0]);
  top->new_slave_port(this, "i2s" + std::to_string(itf_id*2+1), &this->ch_itf[1]);

  this->ch_itf[0].set_sync_meth_muxed(&I2s_periph::rx_sync, 0);
  this->ch_itf[1].set_sync_meth_muxed(&I2s_periph::rx_sync, 1);

  this->top->new_reg(itf_name + "i2s_clkcfg_setup", &this->r_i2s_clkcfg_setup, 0);
  this->top->new_reg(itf_name + "i2s_slv_setup", &this->r_i2s_slv_setup, 0);
  this->top->new_reg(itf_name + "i2s_mst_setup", &this->r_i2s_mst_setup, 0);
  this->top->new_reg(itf_name + "i2s_pdm_setup", &this->r_i2s_pdm_setup, 0);

  this->clkgen0_event = this->top->event_new(this, I2s_periph::clkgen_event_routine);
  this->clkgen0_event->get_args()[0] = (void *)0;

  this->clkgen1_event = this->top->event_new(this, I2s_periph::clkgen_event_routine);
  this->clkgen1_event->get_args()[0] = (void *)1;
}
 

void I2s_periph::reset(bool active)
{
  Udma_periph::reset(active);
  this->reset_clkgen0();
  this->reset_clkgen1();
  this->current_channel = 0;
  this->current_bit = 0;
}



void I2s_periph::handle_clkgen_tick(int clkgen, int channel)
{
  this->trace.msg("Clock edge (sck: %d)\n", this->sck[clkgen]);

  if (this->r_i2s_pdm_setup.pdm_en_get() && this->r_i2s_pdm_setup.pdm_mode_get() >= 2)
  {
    this->ch_itf[0].sync(this->sck[clkgen], 1, 0);
    this->ch_itf[1].sync(this->sck[clkgen], 1, 0);
    this->sck[clkgen] ^= 1;
  }
  else
  {
    int strobe = 0;

    if (this->current_channel == 0)
      strobe = 1;

    this->ch_itf[channel].sync(this->sck[clkgen], strobe, 0);

    this->sck[clkgen] ^= 1;

    if (this->sck[clkgen] == 0)
    {
      this->current_bit++;
      if (this->current_bit == this->r_i2s_slv_setup.slave_bits_get() + 1)
      {
        this->current_bit = 0;

        this->current_channel++;
        if (this->current_channel == this->r_i2s_slv_setup.slave_words_get() + 1)
        {
          this->current_channel = 0;
        }
      }
    }
  }

  if (clkgen == 0)
    this->check_clkgen0();
  else
    this->check_clkgen1();
}



void I2s_periph::clkgen_event_routine(void *__this, vp::clock_event *event)
{
  I2s_periph *_this = (I2s_periph *)__this;

  int clkgen = *(int *)&(event->get_args()[0]);

  if (clkgen == 0 && (_this->r_i2s_clkcfg_setup.slave_num_get() == 1 || _this->r_i2s_clkcfg_setup.master_num_get() == 1))
    _this->handle_clkgen_tick(clkgen, 0);

  if (clkgen == 1 && (_this->r_i2s_clkcfg_setup.slave_num_get() == 0 || _this->r_i2s_clkcfg_setup.master_num_get() == 0))
    _this->handle_clkgen_tick(clkgen, 1);
}



vp::io_req_status_e I2s_periph::reset_clkgen0()
{
  if (this->clkgen0_event->is_enqueued())
    this->top->get_periph_clock()->cancel(this->clkgen0_event);

  this->sck[0] = 0;
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph::reset_clkgen1()
{
  if (this->clkgen1_event->is_enqueued())
    this->top->get_periph_clock()->cancel(this->clkgen1_event);

  this->sck[1] = 0;
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph::check_clkgen0()
{
  if (this->r_i2s_clkcfg_setup.slave_clk_en_get() && !this->clkgen0_event->is_enqueued())
  {
    int div = (this->r_i2s_clkcfg_setup.common_clk_div_get() << 8) | this->r_i2s_clkcfg_setup.slave_clk_div_get();

    this->top->get_periph_clock()->enqueue(this->clkgen0_event, div + 1);
  }
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph::check_clkgen1()
{
  if (this->r_i2s_clkcfg_setup.master_clk_en_get() && !this->clkgen1_event->is_enqueued())
  {
    int div = (this->r_i2s_clkcfg_setup.common_clk_div_get() << 8) | this->r_i2s_clkcfg_setup.master_clk_div_get();

    this->top->get_periph_clock()->enqueue(this->clkgen1_event, div + 1);
  }
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph::i2s_clkcfg_setup_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_i2s_clkcfg_setup.access(reg_offset, size, data, is_write);

  if (is_write)
  {
    this->trace.msg("Modified I2S_CLKCFG_SETUP (master_clk_div: %d, slave_clk_div: %d, common_clk_div: %d, slave_clk_en: %d, master_clk_en: %d, pdm_clk_en: %d, slave_ext: %d, slave_num: %d, master_ext: %d, master_num: %d)\n",
      this->r_i2s_clkcfg_setup.master_clk_div_get(),
      this->r_i2s_clkcfg_setup.slave_clk_div_get(),
      this->r_i2s_clkcfg_setup.common_clk_div_get(),
      this->r_i2s_clkcfg_setup.slave_clk_en_get(),
      this->r_i2s_clkcfg_setup.master_clk_en_get(),
      this->r_i2s_clkcfg_setup.pdm_clk_en_get(),
      this->r_i2s_clkcfg_setup.slave_ext_get(),
      this->r_i2s_clkcfg_setup.slave_num_get(),
      this->r_i2s_clkcfg_setup.master_ext_get(),
      this->r_i2s_clkcfg_setup.master_num_get()
    );
  }

  if (this->r_i2s_clkcfg_setup.slave_clk_en_get() == 0)
    this->reset_clkgen0();

  if (this->r_i2s_clkcfg_setup.master_clk_en_get() == 0)
    this->reset_clkgen1();

  this->check_clkgen0();
  this->check_clkgen1();

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph::i2s_slv_setup_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_i2s_slv_setup.access(reg_offset, size, data, is_write);

  if (is_write)
  {
    this->trace.msg("Modified I2S_SLV_SETUP (raw: 0x%8.8x, slave_words: %d, slave_bits: %d, slave_lsb: %d, slave_2ch: %d, slave_en: %d)\n",
      this->r_i2s_slv_setup.get_32(),
      this->r_i2s_slv_setup.slave_words_get(),
      this->r_i2s_slv_setup.slave_bits_get(),
      this->r_i2s_slv_setup.slave_lsb_get(),
      this->r_i2s_slv_setup.slave_2ch_get(),
      this->r_i2s_slv_setup.slave_en_get()
      );
  }

  return vp::io_req_status_e::IO_REQ_OK;
}




vp::io_req_status_e I2s_periph::i2s_mst_setup_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_i2s_mst_setup.access(reg_offset, size, data, is_write);

  if (is_write)
  {
    this->trace.msg("Modified I2S_MST_SETUP (slave_words: %d, slave_bits: %dd, slave_lsb: %d, slave_2ch: %d, slave_en: %d)\n",
      this->r_i2s_mst_setup.master_words_get(),
      this->r_i2s_mst_setup.master_bits_get(),
      this->r_i2s_mst_setup.master_lsb_get(),
      this->r_i2s_mst_setup.master_2ch_get(),
      this->r_i2s_mst_setup.master_en_get()
      );
  }

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e I2s_periph::i2s_pdm_setup_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  this->r_i2s_pdm_setup.access(reg_offset, size, data, is_write);

  if (is_write)
  {
    this->trace.msg("Modified I2S_PDM_SETUP (pdm_shift: %d, pdm_decimation: %d, pdm_mode: %d, pdm_en: %d)\n",
      this->r_i2s_pdm_setup.pdm_shift_get(),
      this->r_i2s_pdm_setup.pdm_decimation_get(),
      this->r_i2s_pdm_setup.pdm_mode_get(),
      this->r_i2s_pdm_setup.pdm_en_get()
    );
  }

  return vp::io_req_status_e::IO_REQ_OK;
}


vp::io_req_status_e I2s_periph::custom_req(vp::io_req *req, uint64_t offset)
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
    case UDMA_I2S_I2S_CLKCFG_SETUP_OFFSET/4:
      err = this->i2s_clkcfg_setup_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_I2S_SLV_SETUP_OFFSET/4:
      err = this->i2s_slv_setup_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_I2S_MST_SETUP_OFFSET/4:
      err = this->i2s_mst_setup_req(reg_offset, size, is_write, data);
      break;

    case UDMA_I2S_I2S_PDM_SETUP_OFFSET/4:
      err = this->i2s_pdm_setup_req(reg_offset, size, is_write, data);
      break;

  }

  if (err != vp::IO_REQ_OK)
    goto error; 


  return vp::IO_REQ_OK;

error:
  this->trace.force_warning("I2S invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}



void I2s_periph::rx_sync(void *__this, int sck, int ws, int sd, int channel)
{
  I2s_periph *_this = (I2s_periph *)__this;

  _this->trace.msg("Received data (channel: %d, sck: %d, ws: %d, sd: %d)\n", channel, sck, ws, sd);

  if (channel == 0)
    (static_cast<I2s_rx_channel *>(_this->channel0))->handle_rx_bit(sck, ws, sd);
  else
    (static_cast<I2s_rx_channel *>(_this->channel1))->handle_rx_bit(sck, ws, sd);
}



I2s_rx_channel::I2s_rx_channel(udma *top, I2s_periph *periph, int id, int event_id, string name) : Udma_rx_channel(top, event_id, name), periph(periph), id(id)
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


    //gv_trace_dumpMsg(&channel->trace, "Reached decimator, enqueueing value (value: %x)\n", result);


    *dout = result;

    return true;
  }

  return false;
}



void I2s_rx_channel::handle_rx_bit(int sck, int ws, int bit)
{
  int pdm = this->periph->r_i2s_pdm_setup.pdm_en_get();
  int ddr = this->periph->r_i2s_pdm_setup.pdm_mode_get() == 1 || this->periph->r_i2s_pdm_setup.pdm_mode_get() == 3;
  int lsb_first = this->periph->r_i2s_slv_setup.slave_lsb_get();
  bool push = false;
  uint32_t result = 0;
  int width = this->periph->r_i2s_slv_setup.slave_bits_get() + 1;

  if (pdm)
  {
    int decimation = this->periph->r_i2s_pdm_setup.pdm_decimation_get();
    int shift = this->periph->r_i2s_pdm_setup.pdm_shift_get();
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
    result = result & ((1<<width)-1);
    int bytes = width <= 8 ? 1 : width <= 16 ? 2 : 4;

    ((I2s_rx_channel *)this->periph->channel0)->push_data((uint8_t *)&result, bytes);
  }
}
