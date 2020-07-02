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


#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include <pulp/mram/mram.hpp>

class mram : public vp::component, public Mram_itf
{

public:

  mram(js::config *config);

  int build();
  void start();
  void reset(bool active);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::trace     trace;
  vp::io_slave in;

  uint64_t size = 0;

  uint8_t *mem_data;
};

mram::mram(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e mram::req(void *__this, vp::io_req *req)
{
  mram *_this = (mram *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  if (_this->cmd == MRAM_CMD_NORMAL_TX || !req->get_is_write())
  {
    _this->trace.msg("MRAM access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

    if (offset + size > _this->size) {
      _this->trace.warning("Received out-of-bound request (addr: 0x%x, size: 0x%x, mram_size: 0x%x)\n", offset, size, _this->size);
      return vp::IO_REQ_INVALID;
    }

    if (req->get_is_write())
    {
      memcpy((void *)&_this->mem_data[offset], (void *)data, size);
    }
    else
    {
      memcpy((void *)data, (void *)&_this->mem_data[offset], size);
    }
  }

  return vp::IO_REQ_OK;
}


void mram::reset(bool active)
{
}

int mram::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&mram::req);
  new_slave_port("input", &in);

  in.set_itf(static_cast<Mram_itf *>(this));

  return 0;
}

void mram::start()
{
  this->size = get_config_int("size");

  trace.msg("Building MRAM (size: 0x%x)\n", this->size);

  this->mem_data = new uint8_t[this->size];

  // Initialize the mram with a special value to detect uninitialized
  // variables
  memset(mem_data, 0x57, size);
  // Preload the mram
  js::config *stim_file_conf = this->get_js_config()->get("stim_file");
  if (stim_file_conf != NULL)
  {
    string path = stim_file_conf->get_str();
    trace.msg("Preloading mram with stimuli file (path: %s)\n", path.c_str());

    FILE *file = fopen(path.c_str(), "rb");
    if (file == NULL)
    {
      this->trace.fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
    if (fread(this->mem_data, 1, size, file) == 0)
    {
      this->trace.fatal("Failed to read stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
  }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new mram(config);
}
