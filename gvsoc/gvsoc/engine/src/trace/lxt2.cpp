/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "lxt2_write.h"
#include "vp/vp.hpp"
#include "vp/trace/event_dumper.hpp"
#include <string.h>

vp::Lxt2_file::Lxt2_file(vp::Event_dumper *dumper, string path)
{
  this->trace = lxt2_wr_init(path.c_str());
  if (this->trace == NULL)
  {
    dumper->comp->get_engine()->fatal("Error while opening LXT file (path: %s)\n", path.c_str());
  }
}


void vp::Lxt2_file::add_trace(string path, int id, int width, bool is_real, bool is_string)
{
  struct lxt2_wr_symbol *symbol = lxt2_wr_symbol_add(this->trace, strdup(path.c_str()), 0, 31, 0, LXT2_WR_SYM_F_BITS);
  lxt2_wr_set_initial_value(this->trace, 0);
  this->symbols.resize(id+1);
  this->symbols[id] = symbol;
}

void vp::Lxt2_file::dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask)
{
  lxt2_wr_set_time64(this->trace, timestamp);
  lxt2_wr_emit_value_int(this->trace, this->symbols[id], 0, *(uint32_t *)event);
}

void vp::Lxt2_file::close()
{
  lxt2_wr_close(this->trace);
}
