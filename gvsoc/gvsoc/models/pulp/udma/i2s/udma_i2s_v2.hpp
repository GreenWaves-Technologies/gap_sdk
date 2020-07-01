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

#ifndef __PULP_UDMA_I2S_UDMA_I2S_V2_HPP__
#define __PULP_UDMA_I2S_UDMA_I2S_V2_HPP__

#include <vp/vp.hpp>
#include "../udma_impl.hpp"
#include "archi/udma/i2s/udma_i2s_v2.h"


/*
 * I2S
 */

#ifdef HAS_I2S

class I2s_periph;

class I2s_cic_filter {
public:
  I2s_cic_filter();

  bool handle_bit(int din, int pdm_decimation, int pdm_shift, uint32_t *dout);
  void reset();

  int     pdm_pending_bits;
  int64_t pdm_y1_old;
  int64_t pdm_y2_old;
  int64_t pdm_y3_old;
  int64_t pdm_y4_old;
  int64_t pdm_y5_old;
  int64_t pdm_z1_old;
  int64_t pdm_z2_old;
  int64_t pdm_z3_old;
  int64_t pdm_z4_old;
  int64_t pdm_z5_old;
  int64_t pdm_zin1_old;
  int64_t pdm_zin2_old;
  int64_t pdm_zin3_old;
  int64_t pdm_zin4_old;
  int64_t pdm_zin5_old;
};

class I2s_rx_channel : public Udma_rx_channel
{
public:
  I2s_rx_channel(udma *top, I2s_periph *periph, int id, int event_id, string name);
  void handle_rx_bit(int sck, int ws, int bit);

private:
  void reset(bool active);
  I2s_periph *periph;

  I2s_cic_filter *filters[2];
  int id;
  uint32_t pending_samples[2];
  int pending_bits[2];
};

class I2s_periph : public Udma_periph
{
  friend class I2s_rx_channel;

public:
  I2s_periph(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);

protected:
  static void rx_sync(void *, int sck, int ws, int sd, int channel);

private:

  vp::io_req_status_e i2s_clkcfg_setup_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e i2s_slv_setup_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e i2s_mst_setup_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e i2s_pdm_setup_req(int reg_offset, int size, bool is_write, uint8_t *data);

  static void clkgen_event_routine(void *__this, vp::clock_event *event);
  vp::io_req_status_e check_clkgen0();
  vp::io_req_status_e check_clkgen1();
  vp::io_req_status_e reset_clkgen0();
  vp::io_req_status_e reset_clkgen1();
  void handle_clkgen_tick(int clkgen, int itf);

  vp::trace     trace;
  vp::i2s_slave ch_itf[2];

  vp_udma_i2s_i2s_clkcfg_setup  r_i2s_clkcfg_setup;
  vp_udma_i2s_i2s_slv_setup     r_i2s_slv_setup;
  vp_udma_i2s_i2s_mst_setup     r_i2s_mst_setup;
  vp_udma_i2s_i2s_pdm_setup     r_i2s_pdm_setup;

  vp::clock_event *clkgen0_event;
  vp::clock_event *clkgen1_event;

  int sck[2];
  int current_channel;
  int current_bit;
};

#endif


#endif
