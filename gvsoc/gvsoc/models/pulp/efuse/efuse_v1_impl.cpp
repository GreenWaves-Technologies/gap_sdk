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
#include "archi/efuse/efuse_v1.h"

class efuse : public vp::component
{

public:

  efuse(const char *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::io_req_status_e cmd_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e cfg_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e reg_req(int reg_offset, int size, bool is_write, uint8_t *data);


  vp::io_slave in;

  int current_cmd;
  int nb_regs;
  uint32_t *efuse_regs;

  uint32_t cfg_reg;
};



efuse::efuse(const char *config)
: vp::component(config)
{

}



vp::io_req_status_e efuse::cmd_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (!is_write)
    return vp::IO_REQ_INVALID;

  int cmd = 0;
  
  memcpy(&(((uint8_t *)&cmd)[reg_offset]), data, size);

  this->get_trace()->msg("Setting command (value: %d, name: %s)\n", cmd, cmd == EFUSE_CMD_READ ? "read" : cmd == EFUSE_CMD_WRITE ? "write" : cmd == EFUSE_CMD_SLEEP ? "sleep" : "unknown");

  switch (cmd)
  {
    case EFUSE_CMD_READ:
    case EFUSE_CMD_WRITE:
    case EFUSE_CMD_SLEEP:
    this->current_cmd = cmd;
    return vp::IO_REQ_OK;
  }

  return vp::IO_REQ_INVALID;
}



vp::io_req_status_e efuse::cfg_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
  {
    memcpy(&(((uint8_t *)&this->cfg_reg)[reg_offset]), data, size);

    this->get_trace()->msg("Setting configuration (value: 0x%x)\n", this->cfg_reg);
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e efuse::reg_req(int reg_id, int size, bool is_write, uint8_t *data)
{
  if (reg_id >= this->nb_regs || reg_id + size > this->nb_regs)
    return vp::IO_REQ_INVALID;

  if (is_write)
  {
    this->get_trace()->msg("Writing efuse (id: %d, bit: %d)\n", reg_id, *data);

    if (this->current_cmd != EFUSE_CMD_WRITE)
    {
      this->warning.warning("Writing efuse when the write command is not set\n");
      return vp::IO_REQ_INVALID;
    }

    if (((this->efuse_regs[reg_id]) >> (*data)) & 1)
    {
      this->warning.warning("Writing efuse while it has already been programmed (id: %d, bit: %d)\n", reg_id, *data);
      return vp::IO_REQ_INVALID;
    }

    this->efuse_regs[reg_id] |= 1 << *data;
  }
  else
  {
    this->get_trace()->msg("Reading efuse (id: %d, value: 0x%x)\n", reg_id, this->efuse_regs[reg_id]);

    if (this->current_cmd != EFUSE_CMD_READ)
    {
      this->warning.warning("Reading efuse when the read command is not set\n");
      return vp::IO_REQ_INVALID;
    }

    memcpy(data, &(this->efuse_regs[reg_id]), size);
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e efuse::req(void *__this, vp::io_req *req)
{
  efuse *_this = (efuse *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  uint64_t is_write = req->get_is_write();

  _this->get_trace()->msg("Efuse access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  if (reg_offset + size > 4) {
    _this->get_trace()->warning("Accessing 2 registers in one access\n");
    goto error;
  }

  switch (reg_id)
  {
    case EFUSE_CMD_OFFSET/4:
      return _this->cmd_req(reg_offset, size, is_write, data);

    case EFUSE_CFG_OFFSET/4:
      return _this->cfg_req(reg_offset, size, is_write, data);
  }

  if (offset >= EFUSE_REGS_OFFSET)  
    return _this->reg_req((offset - EFUSE_REGS_OFFSET)/4, size, is_write, data);

error:
  _this->get_trace()->msg("FLL invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}



int efuse::build()
{
  this->in.set_req_meth(&efuse::req);
  this->new_slave_port("input", &this->in);

  this->nb_regs = get_config_int("nb_regs");

  this->current_cmd = EFUSE_CMD_SLEEP;

  this->efuse_regs = new uint32_t[this->nb_regs];
  memset(this->efuse_regs, 0, this->nb_regs*4);

  return 0;
}



void efuse::start()
{
  string path;

  js::config *stim_file_conf = this->get_js_config()->get("stim_file");
  if (stim_file_conf)
  {
    js::config *format = this->get_js_config()->get("format");

    path = stim_file_conf->get_str();

    this->get_trace()->msg("Preloading file (path: %s)\n", path.c_str());

    char buffer[this->nb_regs*8*2 + 10];

    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL)
      goto error;

    if (format && format->get_str() == "binary")
    {
      char * line = NULL;
      size_t len = 0;
      ssize_t read;
      int index = 0;
      while ((read = getline(&line, &len, file)) != -1) {
        this->efuse_regs[index] = strtol(line, NULL, 2);
        index++;
      }
    }
    else
    {
      if (fread(buffer, 1, this->nb_regs*8*2 + 10, file) == 0)
        goto error;

      char *str = strtok(buffer, " ");
      int index = 0;
      while (str) {
        int value = atoi(str);
        if (value) {
          int bit = index / 128;
          int reg = index % 128;
          this->efuse_regs[reg] |= 1 << bit;
          this->get_trace()->msg("Setting efuse bit (reg: %d, bit: %d, regValue: %x)\n", bit, reg, this->efuse_regs[reg]);
        }

        str = strtok(NULL, " ");
        index++;
      }
    }
  }
  return;

error:
  this->warning.fatal("Unable to load stimuli file: %s: %s\n", path.c_str(), strerror(errno));
  return;
}



extern "C" void *vp_constructor(const char *config)
{
  return (void *)new efuse(config);
}
