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
