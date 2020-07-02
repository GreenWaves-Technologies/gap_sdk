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

#include "fst/fstapi.h"
#include "vp/vp.hpp"
#include "vp/trace/event_dumper.hpp"
#include <string.h>

vp::Fst_file::Fst_file(vp::Event_dumper *dumper, string path)
{
  this->writer = fstWriterCreate(path.c_str(), 0);
  if (this->writer == NULL)
  {
    dumper->comp->get_engine()->fatal("Error while opening FST file (path: %s)\n", path.c_str());
  }
  fstWriterSetTimescale(this->writer, -12);
}


string vp::Fst_file::parse_path(string path, bool begin)
{
  std::string delim = "/";
  
  auto start = 0U;
  auto end = path.find(delim);
  while (end != std::string::npos)
  {
    if (end != start)
    {
      if (begin) {
        fstWriterSetScope(this->writer, FST_ST_VCD_MODULE, path.substr(start, end - start).c_str(), NULL);
      } else {
        fstWriterSetUpscope(this->writer);
      }
    }
    start = end + delim.length();
    end = path.find(delim, start);
  }
  return path.substr(start, end);
}

void vp::Fst_file::add_trace(string path, int id, int width, bool is_real, bool is_string)
{
  string name = this->parse_path(path, true);

  fstHandle var;
  if (is_real)
  {
    var = fstWriterCreateVar(this->writer, FST_VT_VCD_REAL, FST_VD_INOUT, width, name.c_str(), 0);
  }
  else if (is_string)
  {
    var = fstWriterCreateVar(this->writer, FST_VT_GEN_STRING, FST_VD_INOUT, 0, name.c_str(), 0);
  }
  else
  {
    var = fstWriterCreateVar(this->writer, FST_VT_VCD_WIRE, FST_VD_INOUT, width, name.c_str(), 0);
  }
  this->vars.resize(id+1);
  this->vars[id] = var;

  this->parse_path(string(path), false);
}

void vp::Fst_file::dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask)
{
  fstWriterEmitTimeChange(this->writer, timestamp);
  if (is_real)
  {
    fstWriterEmitValueChange(this->writer, this->vars[id], event);
  }
  else if (is_string)
  {
    fstWriterEmitVariableLengthValueChange(this->writer, this->vars[id], event, width);
  }
  else
  {
    char val[width];
    for (int i=0; i<width; i++)
    {
      // For now we just support Z everywhere
      if (flags == 1)
        val[width - i - 1] = 'Z';
      else
        val[width - i - 1] = '0' + (((*(uint32_t *)event) >> i) & 1);
    }
    fstWriterEmitValueChange(this->writer, this->vars[id], val);
  }
}

void vp::Fst_file::close()
{
  fstWriterClose(this->writer);
}
