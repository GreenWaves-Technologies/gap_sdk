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

#ifndef __PULP_UDMA_UDMA_CPI_V1_HPP__
#define __PULP_UDMA_UDMA_CPI_V1_HPP__

class Cpi_periph;

class Cpi_rx_channel : public Udma_rx_channel
{
public:
  Cpi_rx_channel(udma *top, Cpi_periph *periph, int id, string name);

private:
  void reset(bool active);
  Cpi_periph *periph;
};

class Cpi_periph : public Udma_periph
{
  friend class Cpi_rx_channel;

public:
  Cpi_periph(udma *top, int id, int itf_id);
  vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
  void reset(bool active);
  void handle_sof();

protected:
  vp::cpi_slave cpi_itf;

private:
  static void sync(void *__this, int pclk, int href, int vsync, int data);
  static void sync_cycle(void *__this, int href, int vsync, int data);
  vp::io_req_status_e handle_global_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_l1_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_ur_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_size_access(bool is_write, uint32_t *data);
  vp::io_req_status_e handle_filter_access(bool is_write, uint32_t *data);
  void push_pixel(uint32_t pixel);

  vp::trace     trace;

  int pending_byte;
  bool has_pending_byte;
  bool cmd_ready;

  uint32_t glob;
  uint32_t ll;
  uint32_t ur;
  uint32_t size;
  uint32_t filter;

  bool wroteGlob;
  bool wroteLl;
  bool wroteUr;
  bool wroteSize;
  bool wroteFilter;

  unsigned int enabled;
  unsigned int frameDrop;
  unsigned int nbFrameDrop;
  unsigned int frameSliceEn;
  unsigned int format;
  unsigned int shift;

  unsigned int frameSliceLlx;
  unsigned int frameSliceLly;
  unsigned int frameSliceUrx;
  unsigned int frameSliceUry;

  unsigned int rowLen;

  unsigned int bCoeff;
  unsigned int gCoeff;
  unsigned int rCoeff;

  unsigned int frameDropCount;
  unsigned int currentLine;
  unsigned int currentRow;

};

#endif