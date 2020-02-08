/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
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
