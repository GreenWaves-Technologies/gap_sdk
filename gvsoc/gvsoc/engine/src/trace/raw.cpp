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
#include "raw/trace_dumper.hpp"

vp::Raw_file::Raw_file(vp::Event_dumper *dumper, string path)
{
    trace_dumper_client *td = new trace_dumper_client(path);
    this->dumper = td;

    if (td->open(ED_CONF_TIMESCALE_PS))
    {
        dumper->comp->get_engine()->fatal("Error while opening RAW file (path: %s, error: %s)\n", path.c_str(), strerror(errno));
    }
}



void vp::Raw_file::add_trace(string path, int id, int width, bool is_real, bool is_string)
{

    trace_dumper_client *td = (trace_dumper_client *)this->dumper;
    ed_trace_type_e type;

    if (is_real)
    {
        type = ED_TRACE_REAL;
        width = 8*8;
    }
    else if (is_string)
    {
        type = ED_TRACE_VARLEN;
    }
    else
    {
        type = ED_TRACE_BITFIELD;
    }

    trace_dumper_trace *trace = td->reg_trace(path, id, type, width);

    this->traces[id] = (void *)trace;
}

void vp::Raw_file::dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask)
{
    trace_dumper_trace *trace = (trace_dumper_trace *)this->traces[id];

    trace->dump(timestamp, event, width);
}

void vp::Raw_file::close()
{
    trace_dumper_client *td = (trace_dumper_client *)this->dumper;
    td->close();
}
