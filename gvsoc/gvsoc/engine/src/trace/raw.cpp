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
