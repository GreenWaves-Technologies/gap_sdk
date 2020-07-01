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
#include "archi/udma/cpi/udma_cpi_v1_old.h"
#include "archi/utils.h"
#include "vp/itf/cpi.hpp"


Cpi_periph_v1::Cpi_periph_v1(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "cpi" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  channel0 = new Cpi_rx_channel(top, this, UDMA_EVENT_ID(id), itf_name + "_rx");

  top->new_slave_port(this, itf_name, &cpi_itf);

  cpi_itf.set_sync_meth(&Cpi_periph_v1::sync);
  cpi_itf.set_sync_cycle_meth(&Cpi_periph_v1::sync_cycle);
}
 

void Cpi_periph_v1::reset(bool active)
{
  Udma_periph::reset(active);

  this->glob = 0;
}



vp::io_req_status_e Cpi_periph_v1::handle_global_access(bool is_write, uint32_t *data) {
  if (!is_write) *data = this->glob;
  else {
    this->glob = *data;
    this->wroteGlob = true;
  }
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Cpi_periph_v1::handle_l1_access(bool is_write, uint32_t *data) {
  if (!is_write) *data = this->ll;
  else {
    this->ll = *data;
    this->wroteLl = true;
  }
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Cpi_periph_v1::handle_ur_access(bool is_write, uint32_t *data) {
  if (!is_write) *data = this->ur;
  else {
    this->ur = *data;
    this->wroteUr = true;
  }
  return vp::IO_REQ_OK;
}


vp::io_req_status_e Cpi_periph_v1::handle_size_access(bool is_write, uint32_t *data) {
  if (!is_write) *data = this->size;
  else {
    this->size = *data;
    this->wroteSize = true;
  }
  return vp::IO_REQ_OK;
}

vp::io_req_status_e Cpi_periph_v1::handle_filter_access(bool is_write, uint32_t *data) {
  if (!is_write) *data = this->filter;
  else {
    this->filter = *data;
    this->wroteFilter = true;
  }
  return vp::IO_REQ_OK;
}




vp::io_req_status_e Cpi_periph_v1::custom_req(vp::io_req *req, uint64_t offset)
{
  bool is_write = req->get_is_write();
  uint32_t *data = (uint32_t *)req->get_data();

  if (req->get_size() != 4)
    return vp::IO_REQ_INVALID;

  switch (offset) {
    case CAM_GLOB_OFFSET: return handle_global_access(is_write, data);
    case CAM_LL_OFFSET: return handle_l1_access(is_write, data);
    case CAM_UR_OFFSET: return handle_ur_access(is_write, data);
    case CAM_SIZE_OFFSET: return handle_size_access(is_write, data);
    case CAM_FILTER_OFFSET: return handle_filter_access(is_write, data);
  }

  return vp::IO_REQ_INVALID;
}


void Cpi_periph_v1::push_pixel(uint32_t pixel)
{
  // Check frame slice
  if (this->frameSliceEn) {

    bool drop_pixel = false;

    // The window is activated, check if the pixel is inside
    if (this->currentLine < this->frameSliceLly || this->currentLine > this->frameSliceUry || this->currentRow < this->frameSliceLlx || this->currentRow > this->frameSliceUrx) drop_pixel = true;

    // Updated current coordinates
    this->currentRow++;
    if (this->currentRow == this->rowLen) {
      this->currentRow = 0;
      this->currentLine++;
    }

    if (drop_pixel) return;
  }

  this->trace.msg("Pushing pixel (value: %x)\n", pixel);

  int output_size = 1;

  // Apply the conversion
  if (this->format == ARCHI_CAM_CFG_GLOB_FORMAT_BYPASS_LITEND)
  {
    // Do nothing the pixel is already in the right order
    output_size = 2;
  }
  else if (this->format == ARCHI_CAM_CFG_GLOB_FORMAT_BYPASS_BIGEND)
  {
    pixel = (ARCHI_REG_FIELD_GET(pixel, 0, 8) << 8) | ARCHI_REG_FIELD_GET(pixel, 8, 8);
    output_size = 2;
  }
  else
  {
    uint64_t r=0, g=0, b=0;
    switch (this->format) {
      case ARCHI_CAM_CFG_GLOB_FORMAT_RGB565:
      b = ARCHI_REG_FIELD_GET(pixel, 0, 5) << 3;
      g = ARCHI_REG_FIELD_GET(pixel, 5, 6) << 2;
      r = ARCHI_REG_FIELD_GET(pixel, 11, 5) << 3;
      break;
      case ARCHI_CAM_CFG_GLOB_FORMAT_RGB555:
      b = ARCHI_REG_FIELD_GET(pixel, 0, 5) << 3;
      g = ARCHI_REG_FIELD_GET(pixel, 5, 5) << 3;
      r = ARCHI_REG_FIELD_GET(pixel, 10, 5) << 3;
      break;
      case ARCHI_CAM_CFG_GLOB_FORMAT_RGB444:
      b = ARCHI_REG_FIELD_GET(pixel, 0, 4) << 4;
      g = ARCHI_REG_FIELD_GET(pixel, 4, 4) << 4;
      r = ARCHI_REG_FIELD_GET(pixel, 8, 4) << 4;
      break;
    }
    pixel = (b*this->bCoeff + g*this->gCoeff + r*this->rCoeff) >> shift;
    this->trace.msg("Applied pixel filtering (value: %x)\n", pixel);

  }

  // Finally push the pixel to the UDMA
  (static_cast<Cpi_rx_channel *>(this->channel0))->push_data((uint8_t *)&pixel, 2);
}


void Cpi_periph_v1::sync(void *__this, int pclk, int href, int vsync, int data)
{
  Cpi_periph_v1 *_this = (Cpi_periph_v1 *)__this;
  _this->trace.msg("Sync (pclk: %d, href: %d, vsync: %d, data: %d)\n", pclk, href, vsync, data);
  if (pclk) Cpi_periph_v1::sync_cycle(__this, href, vsync, data);
}

void Cpi_periph_v1::handle_sof()
{
  this->trace.msg("Starting new frame\n");

  // In case we are dropping frames, account the current one
  if (this->frameDrop) {
    if (!this->frameDropCount) {
      // If we were on the enabled frame, just start dropping frames
      this->frameDropCount = this->nbFrameDrop;
    } else {
      this->frameDropCount--;
    }
  }

  Cpi_rx_channel *channel = (static_cast<Cpi_rx_channel *>(this->channel0));
  this->cmd_ready = channel->has_cmd();


  this->currentRow = 0;
  this->currentLine = 0;
  this->has_pending_byte = false;

  // Extract the new configuration from registers as it is taken into account when a frame starts
  if (this->wroteGlob) {
    this->wroteGlob = false;
    this->enabled = ARCHI_REG_FIELD_GET(glob, CAM_CFG_GLOB_EN_BIT, 1);
    this->frameDrop = ARCHI_REG_FIELD_GET(glob, CAM_CFG_GLOB_FRAMEDROP_EN_BIT, 1);
    this->nbFrameDrop = ARCHI_REG_FIELD_GET(glob, CAM_CFG_GLOB_FRAMEDROP_VAL_BIT, CAM_CFG_GLOB_FRAMEDROP_VAL_SIZE);
    this->frameSliceEn = ARCHI_REG_FIELD_GET(glob, CAM_CFG_GLOB_FRAMESLICE_EN_BIT, 1);
    this->format = ARCHI_REG_FIELD_GET(glob, CAM_CFG_GLOB_FORMAT_BIT, CAM_CFG_GLOB_FORMAT_SIZE);
    this->shift = ARCHI_REG_FIELD_GET(glob, CAM_CFG_GLOB_SHIFT_BIT, CAM_CFG_GLOB_SHIFT_SIZE);

    // In case of frame dropping, start with an enabled one
    if (this->frameDrop) {
      this->frameDropCount = 0;
    }

    this->trace.msg("Updating global configuration (enabled: %d, frameDrop: %d, nbFrameDrop: %d, frameSlice: %d, format: %d, shift: %d)\n", enabled, frameDrop, nbFrameDrop, frameSliceEn, format, shift);
  }

  if (this->wroteLl) {
    this->wroteLl = false;
    this->frameSliceLlx = ARCHI_REG_FIELD_GET(ll, CAM_CFG_LL_FRAMESLICE_LLX_BIT, CAM_CFG_LL_FRAMESLICE_LLX_SIZE);
    this->frameSliceLly = ARCHI_REG_FIELD_GET(ll, CAM_CFG_LL_FRAMESLICE_LLY_BIT, CAM_CFG_LL_FRAMESLICE_LLY_SIZE);
    this->trace.msg("Updating low-left corner (x: %d, y: %d)\n", frameSliceLlx, frameSliceLly);
  }

  if (this->wroteUr) {
    this->wroteUr = false;
    this->frameSliceUrx = ARCHI_REG_FIELD_GET(ur, CAM_CFG_UR_FRAMESLICE_URX_BIT, CAM_CFG_UR_FRAMESLICE_URX_SIZE);
    this->frameSliceUry = ARCHI_REG_FIELD_GET(ur, CAM_CFG_UR_FRAMESLICE_URY_BIT, CAM_CFG_UR_FRAMESLICE_URY_SIZE);
    this->trace.msg("Updating up-right corner (x: %d, y: %d)\n", frameSliceUrx, frameSliceUry);
  }
  
  if (this->wroteSize) {
    this->wroteSize = false;
    this->rowLen = ARCHI_REG_FIELD_GET(size, CAM_CFG_SIZE_ROWLEN_BIT, CAM_CFG_SIZE_ROWLEN_SIZE) + 1;
    this->trace.msg("Updating horizontal resolution (value: %d)\n", rowLen);
  }

  if (this->wroteFilter) {
    this->wroteFilter = false;
    this->bCoeff = ARCHI_REG_FIELD_GET(filter, CAM_CFG_FILTER_B_COEFF_BIT, CAM_CFG_FILTER_B_COEFF_SIZE);
    this->gCoeff = ARCHI_REG_FIELD_GET(filter, CAM_CFG_FILTER_G_COEFF_BIT, CAM_CFG_FILTER_G_COEFF_SIZE);
    this->rCoeff = ARCHI_REG_FIELD_GET(filter, CAM_CFG_FILTER_R_COEFF_BIT, CAM_CFG_FILTER_R_COEFF_SIZE);
    this->trace.msg("Updating coefficients (b: %d, g: %d, r: %d)\n", bCoeff, gCoeff, rCoeff);
  }
}

void Cpi_periph_v1::sync_cycle(void *__this, int href, int vsync, int data)
{
  Cpi_periph_v1 *_this = (Cpi_periph_v1 *)__this;

  if (vsync)
  {
    _this->handle_sof();
  }
  else
  {
    if (href)
    {
      // To transmit the data, the channel must be enabled with no frame dropping or with the enabled frame
      if (_this->enabled && (!_this->frameDrop || !_this->frameDropCount) && _this->cmd_ready) {
        if (_this->has_pending_byte)
        {
          _this->push_pixel((_this->pending_byte << 8) | data);
          _this->has_pending_byte = false;
        }
        else
        {
          _this->has_pending_byte = true;
          _this->pending_byte = data;
        }
      }
    }
  }
}





Cpi_rx_channel::Cpi_rx_channel(udma *top, Cpi_periph_v1 *periph, int id, string name) : Udma_rx_channel(top, id, name), periph(periph)
{
}

void Cpi_rx_channel::reset(bool active)
{
  Udma_rx_channel::reset(active);
}
