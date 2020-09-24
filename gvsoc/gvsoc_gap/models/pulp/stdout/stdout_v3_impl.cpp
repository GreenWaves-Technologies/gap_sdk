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
#include <vector>

#define MAX_PUTC_LENGTH 1024

class Stdout : public vp::component
{

public:

  Stdout(js::config *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  vp::trace     trace;
  vp::io_slave in;

  int nb_cluster;
  int nb_core;

  std::vector <char *> putc_buffer;
  int *putc_buffer_pos;

};

Stdout::Stdout(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e Stdout::req(void *__this, vp::io_req *req)
{
  Stdout *_this = (Stdout *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();

  _this->trace.msg("Stdout access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, req->get_is_write());

  int core_id = (offset >> 3) & 0xf;
  int cluster_id = (offset >> 7) & 0x3f;

  if (core_id >= _this->nb_core || cluster_id >= _this->nb_cluster)
  {
    _this->trace.warning("Accessing invalid stdout channel (coreId: %d, clusterId: %d)\n", core_id, cluster_id);
    return vp::IO_REQ_INVALID;
  }
  
  _this->putc_buffer[cluster_id*_this->nb_core+core_id][_this->putc_buffer_pos[cluster_id*_this->nb_core+core_id]++] = *data;
  if (*data == '\n' || _this->putc_buffer_pos[cluster_id*_this->nb_core+core_id] == MAX_PUTC_LENGTH - 1) {
    _this->putc_buffer[cluster_id*_this->nb_core+core_id][_this->putc_buffer_pos[cluster_id*_this->nb_core+core_id]] = 0;
      //if (stdoutPrefix) fprintf(stdout, "# [STDOUT-CL%d_PE%d] ", clusterId, coreId);
    fwrite((void *)_this->putc_buffer[cluster_id*_this->nb_core+core_id], 1, _this->putc_buffer_pos[cluster_id*_this->nb_core+core_id], stdout);
  //  if (stdoutToFile) fwrite((void *)putcBuffer[clusterId][coreId], 1, putcBufferPos[clusterId][coreId], stdoutFiles[clusterId][coreId]);
    _this->putc_buffer_pos[cluster_id*_this->nb_core+core_id] = 0;
  }

  return vp::IO_REQ_OK;
}

int Stdout::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);
  in.set_req_meth(&Stdout::req);
  new_slave_port("input", &in);

  nb_cluster = get_config_int("max_cluster");
  nb_core = get_config_int("max_core_per_cluster");

  putc_buffer_pos = new int[nb_cluster*nb_core];
  for (int j=0; j<nb_cluster; j++) {
    for (int i=0; i<nb_core; i++) {
      putc_buffer.push_back(new char[MAX_PUTC_LENGTH]);
      putc_buffer_pos[j*nb_core+i] = 0;
    }
  }

  return 0;
}

void Stdout::start()
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Stdout(config);
}
