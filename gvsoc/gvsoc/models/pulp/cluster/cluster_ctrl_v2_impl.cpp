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
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>

#include "archi/cluster_ctrl/cluster_ctrl_v2.h"

class Core_cluster_ctrl
{
public:
  vp::wire_master<uint32_t> bootaddr_itf;
  vp::wire_master<bool> fetchen_itf;
  vp::wire_master<bool> halt_itf;
  vp::wire_slave<bool> halt_status_itf;
  uint32_t bootaddr;
};

class cluster_ctrl : public vp::component
{

public:

  cluster_ctrl(const char *config);

  int build();
  void start();
  void reset(bool active);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:


  static void halt_status_sync(void *__this, bool status, int id);
  vp::io_req_status_e fetch_en_req(bool is_write, uint32_t *data);
  vp::io_req_status_e dbg_halt_status_req(bool is_write, uint32_t *data);
  vp::io_req_status_e dbg_halt_mask_req(bool is_write, uint32_t *data);
  vp::io_req_status_e bootaddr_req(int core, bool is_write, uint32_t *data);
  void check_dbg_halt();

  vp::trace     trace;
  vp::io_slave in;
  int nb_core;
  Core_cluster_ctrl *cores;

  uint32_t dbg_halt_mask;
  uint32_t dbg_halt_status;
  uint32_t dbg_halt_status_sync;
};

cluster_ctrl::cluster_ctrl(const char *config)
: vp::component(config)
{
  nb_core = get_config_int("nb_core");
}

vp::io_req_status_e cluster_ctrl::req(void *__this, vp::io_req *req)
{
  cluster_ctrl *_this = (cluster_ctrl *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("cluster_ctrl access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4)
  {
    _this->trace.warning("Only 32 bits accesses are allowed\n");
    return vp::IO_REQ_INVALID;
  } 

  if (offset == ARCHI_CLUSTER_CTRL_FETCH_EN)
  {
    return _this->fetch_en_req(is_write, (uint32_t *)data);
  }
  else if (offset >= ARCHI_CLUSTER_CTRL_BOOTADDR(0) && offset < ARCHI_CLUSTER_CTRL_BOOTADDR(_this->nb_core))
  {
    return _this->bootaddr_req(ARCHI_CLUSTER_CTRL_BOOTADDR_COREID(offset), is_write, (uint32_t *)data);
  }
  else if (offset == ARCHI_CLUSTER_CTRL_EVENT)
  {
    return vp::IO_REQ_OK;
  }
  else if (offset == ARCHI_CLUSTER_CTRL_CLUSTER_CFG)
  {
    return vp::IO_REQ_OK;
  }
  else if (offset == ARCHI_CLUSTER_CTRL_CLUSTER_CLK_GATE)
  {
    return vp::IO_REQ_OK;
  }
  else if (offset == ARCHI_CLUSTER_CTRL_DBG_STATUS)
  {
    return _this->dbg_halt_status_req(is_write, (uint32_t *)data);
  }
  else if (offset == ARCHI_CLUSTER_CTRL_DBG_HALT_MASK)
  {
    return _this->dbg_halt_mask_req(is_write, (uint32_t *)data);
  }


  vp_warning_always(&_this->warning, "Invalid access\n");

  return vp::IO_REQ_INVALID;
}



vp::io_req_status_e cluster_ctrl::dbg_halt_status_req(bool is_write, uint32_t *data)
{
  if (is_write)
  {
    this->trace.msg("Writing dbg status (dbg_status: 0x%x)\n", *data);

    // When writing to the halt status register, each bit set to 1 indicates
    // a core to resume
    this->dbg_halt_status &= ~(*data);

    this->check_dbg_halt();
  }
  else
  {
    *data = this->dbg_halt_status;
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e cluster_ctrl::dbg_halt_mask_req(bool is_write, uint32_t *data)
{
  if (is_write)
  {
    this->trace.msg("Writing dbg halt mask (dbg_halt_mask: 0x%x)\n", *data);
    this->dbg_halt_mask = *data;

    this->check_dbg_halt();
  }
  else
  {
    *data = this->dbg_halt_mask;
  }

  return vp::IO_REQ_OK;
}



void cluster_ctrl::check_dbg_halt()
{
  uint32_t trig_mask = this->dbg_halt_status & this->dbg_halt_mask;

  if (trig_mask && trig_mask != this->dbg_halt_mask)
  {
    trace.msg("Propagating cross-trigger halt signal (halt_status: 0x%x, new_halt_status: 0x%x)\n",
      this->dbg_halt_status & this->dbg_halt_mask, this->dbg_halt_mask);

    this->dbg_halt_status |= this->dbg_halt_mask;
  }

  if (this->dbg_halt_status != this->dbg_halt_status_sync)
  {
    uint32_t dbg_halt_status_sync = this->dbg_halt_status_sync;
    this->dbg_halt_status_sync = this->dbg_halt_status;

    for (int i=0; i<nb_core; i++)
    {
      int current_status = (this->dbg_halt_status >> i) & 1;
      int sync_status = (dbg_halt_status_sync >> i) & 1;

      if (current_status != sync_status)
      {
        trace.msg("Synchronizing core halt (core: %d, halt: %d)\n", i, current_status);

        this->dbg_halt_status_sync |= current_status << i;
        cores[i].halt_itf.sync(current_status);
      }
    }
  }
}



vp::io_req_status_e cluster_ctrl::fetch_en_req(bool is_write, uint32_t *data)
{
  for (int i=0; i<nb_core; i++)
  {
    cores[i].fetchen_itf.sync(((*data) >> i) & 1);
  }
  return vp::IO_REQ_OK;
}



vp::io_req_status_e cluster_ctrl::bootaddr_req(int core, bool is_write, uint32_t *data)
{
  if (is_write)
  {
    trace.msg("Setting boot address (core: %d, addr: 0x%x)\n", core, *data);

    cores[core].bootaddr_itf.sync(*data);
    cores[core].bootaddr = *data;
  }
  else
  {
    *data = cores[core].bootaddr;
  }
  return vp::IO_REQ_OK;
}



void cluster_ctrl::halt_status_sync(void *__this, bool status, int id)
{
  cluster_ctrl *_this = (cluster_ctrl *)__this;

  _this->trace.msg("Received new core halt status (core: %d, halt: %d)\n", id, status);
  _this->dbg_halt_status = (_this->dbg_halt_status & ~(1<<id)) | (status << id);
  _this->dbg_halt_status_sync = (_this->dbg_halt_status & ~(1<<id)) | (status << id);

  if (status)
    _this->check_dbg_halt();
}



int cluster_ctrl::build()
{
  cores = (Core_cluster_ctrl *)new Core_cluster_ctrl[nb_core];

  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&cluster_ctrl::req);
  new_slave_port("input", &in);

  for (int i; i<nb_core; i++)
  {
    cores[i].bootaddr = 0x57575757;

    new_master_port("bootaddr_" + std::to_string(i), &cores[i].bootaddr_itf);
    new_master_port("fetchen_" + std::to_string(i), &cores[i].fetchen_itf);
    new_master_port("halt_" + std::to_string(i), &cores[i].halt_itf);

    cores[i].halt_status_itf.set_sync_meth_muxed(&cluster_ctrl::halt_status_sync, i);
    new_slave_port("core_halt_" + std::to_string(i), &cores[i].halt_status_itf);

  }

  return 0;
}

void cluster_ctrl::start()
{
}

void cluster_ctrl::reset(bool active)
{
  if (active)
  {
    this->dbg_halt_mask = 0;
    this->dbg_halt_status = 0;
    this->dbg_halt_status_sync = 0;
  }
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new cluster_ctrl(config);
}
