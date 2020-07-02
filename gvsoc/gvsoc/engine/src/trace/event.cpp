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

#include "vp/vp.hpp"
#include "vp/trace/event_dumper.hpp"
#include <string.h>

static int vcd_id = 0;



vp::Event_trace::Event_trace(string trace_name, Event_file *file, int width, bool is_real, bool(is_string)) : is_real(is_real), is_string(is_string), is_enqueued(false), file(file)
{
  id = vcd_id++;
  file->add_trace(trace_name, id, width, is_real, is_string);
  this->width = width;
  this->bytes = (width+7)/8;
  if (width)
  {
    this->buffer = new uint8_t[width];
    this->flags_mask = new uint8_t[width];
  }
  else
  {
    this->buffer = NULL;
    this->flags_mask = NULL;
  }
}


void vp::Event_trace::reg(int64_t timestamp, uint8_t *event, int width, uint8_t flags, uint8_t *flags_mask)
{
  int bytes = (width+7)/8;
  if (bytes > this->bytes)
  {
    this->bytes = bytes;
    //if (this->buffer)
    //  delete this->buffer;
    this->buffer = new uint8_t[width];
    //if (this->flags_mask)
    //  delete this->flags_mask;
    this->flags_mask = new uint8_t[width];
  }

  memcpy(this->buffer, event, bytes);
  this->flags = flags;
  if (flags == 1)
  {
    memcpy(this->flags_mask, flags_mask, bytes);
  }
}



vp::Event_trace *vp::Event_dumper::get_trace(string trace_name, string file_name, int width, bool is_real, bool is_string)
{
  vp::Event_trace *trace = event_traces[trace_name];
  if (trace == NULL)
  {
    vp::Event_file *event_file = event_files[file_name];

    if (event_file == NULL)
    {
      std::string format = this->comp->get_js_config()->get_child_str("**/events/format");

      if (format == "vcd")
      {
        event_file = new Vcd_file(this, file_name);
      }
      else if (format == "fst")
      {
        event_file = new Fst_file(this, file_name);
      }
      else if (format == "raw")
      {
        event_file = new Raw_file(this, file_name);
      }
      else
      {
        this->comp->get_trace()->fatal("Unknown trace format (name: %s)\n", format.c_str());
      }
      event_files[file_name] = event_file;
    }

    trace = new Event_trace(trace_name, event_file, width, is_real, is_string);
    event_traces[trace_name] = trace;
  }

  return trace;
}

vp::Event_trace *vp::Event_dumper::get_trace_real(string trace_name, string file_name)
{
  vp::Event_trace *trace = this->get_trace(trace_name, file_name, 8, true);
  return trace;
}

vp::Event_trace *vp::Event_dumper::get_trace_string(string trace_name, string file_name)
{
  vp::Event_trace *trace = this->get_trace(trace_name, file_name, 0, false, true);
  return trace;
}

void vp::Event_dumper::close()
{
  for (auto &x: event_files)
  {
    x.second->close();
  }
}