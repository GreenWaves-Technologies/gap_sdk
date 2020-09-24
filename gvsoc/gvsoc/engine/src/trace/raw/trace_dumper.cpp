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

#include "trace_dumper.hpp"
#include <string>


trace_dumper_trace::trace_dumper_trace(trace_dumper_client *client, int id, ed_trace_type_e type, int width)
: type(type), width(width), client(client)
{
    this->id = encode_id(id, &this->id_size);
}


void trace_dumper_trace::dump(int64_t timestamp, uint8_t *value, int width)
{
    this->client->dump_trace(timestamp, this->id, this->id_size, this->type, value, width);
}


trace_dumper_client::trace_dumper_client(std::string file)
: filepath(file), current_timestamp(-1)
{
}    


int trace_dumper_client::open(ed_conf_timescale_e timescale)
{
    this->file.open(this->filepath);
    if (this->file.fail())
        return -1;

    ed_header_t header = {.type=ED_TYPE_CONF};
    ed_conf_t conf = { .version=ED_CONF_LAST_VERSION, .timescale=timescale };

    this->file.write((char *)&header, sizeof(header));
    if (this->file.fail())
        return -1;

    this->file.write((char *)&conf, sizeof(conf));
    if (this->file.fail())
        return -1;

    return 0;
}


void trace_dumper_client::close()
{
    this->file.close();
}


trace_dumper_trace *trace_dumper_client::reg_trace(std::string path, uint32_t id, ed_trace_type_e type, uint32_t width)
{
    trace_dumper_trace *trace = new trace_dumper_trace(this, id, type, width);

    ed_header_t header = {.type=ED_TYPE_REG_TRACE};
    ed_reg_trace_t reg_trace = { .type=type, .width=width, .path_len=(uint32_t)path.size(), .id=id };

    this->file.write((char *)&header, sizeof(header));
    if (this->file.fail())
        return NULL;

    this->file.write((char *)&reg_trace, sizeof(reg_trace));
    if (this->file.fail())
        return NULL;

    this->file.write((char *)path.c_str(), path.size());
    if (this->file.fail())
        return NULL;

    return trace;
}


int trace_dumper_client::dump_trace(int64_t timestamp, int id, int id_size, ed_trace_type_e type, uint8_t *value, int width)
{
    // First dump the timestamp if it is different from previous one
    if (timestamp > this->current_timestamp)
    {
        uint64_t diff = this->current_timestamp >= 0 ? timestamp - this->current_timestamp : timestamp;
        ed_header_t header;
        int ts_size;

        header.type = td_get_timestamp(diff, &ts_size);

        this->file.write((char *)&header, sizeof(header));
        if (this->file.fail())
            return -1;

        this->file.write((char *)&diff, ts_size);
        if (this->file.fail())
            return -1;

        this->current_timestamp = timestamp;
    }


    // And then the trace
    if (type == ED_TRACE_BITFIELD)
    {
        if (width == 1)
        {
            ed_header_t header = { .type = *value == 0 ? ED_TYPE_TRACE_SET_0 : ED_TYPE_TRACE_SET_1 };

            this->file.write((char *)&header, sizeof(header));
            if (this->file.fail())
                return -1;

            this->file.write((char *)&id, id_size);
            if (this->file.fail())
                return -1;
        }
        else
        {
            ed_header_t header = { .type = ED_TYPE_TRACE };

            this->file.write((char *)&header, sizeof(header));
            if (this->file.fail())
                return -1;

            this->file.write((char *)&id, id_size);
            if (this->file.fail())
                return -1;

            int size = (width + 7) / 8;

            this->file.write((char *)value, size);
            if (this->file.fail())
                return -1;
        }
    }
    else if (type == ED_TRACE_REAL)
    {
        ed_header_t header = { .type = ED_TYPE_TRACE };

        this->file.write((char *)&header, sizeof(header));
        if (this->file.fail())
            return -1;

        this->file.write((char *)&id, id_size);
        if (this->file.fail())
            return -1;

        this->file.write((char *)value, width);
        if (this->file.fail())
            return -1;
    }
    else if (type == ED_TRACE_VARLEN)
    {
        ed_header_t header = { .type = ED_TYPE_TRACE };

        this->file.write((char *)&header, sizeof(header));
        if (this->file.fail())
            return -1;

        this->file.write((char *)&id, id_size);
        if (this->file.fail())
            return -1;

        int size = (width + 7) / 8;

        this->file.write((char *)&size, 4);
        if (this->file.fail())
            return -1;

        this->file.write((char *)value, size);
        if (this->file.fail())
            return -1;
    }

    return 0;
}


trace_dumper_server::trace_dumper_server(std::string file)
: filepath(file), timestamp(0)
{
}    


int trace_dumper_server::open()
{
    this->file.open(this->filepath);
    if (this->file.fail())
        return -1;

    return 0;
}


int trace_dumper_server::get_packet(trace_packet *packet)
{
    ed_header_t header;
    this->file.read((char *)&packet->header, sizeof(header));
    if (this->file.fail())
        return -1;

    if (packet->header.type == ED_TYPE_CONF)
    {
        this->file.read((char *)&packet->conf, sizeof(packet->conf));
        if (this->file.fail())
            return -1;
    }
    else if (packet->header.type == ED_TYPE_REG_TRACE)
    {
        this->file.read((char *)&packet->reg_trace, sizeof(packet->reg_trace));
        if (this->file.fail())
            return -1;

        char path[packet->reg_trace.path_len+1];
        this->file.read(path, packet->reg_trace.path_len);
        if (this->file.fail())
            return -1;
        path[packet->reg_trace.path_len] = 0;

        this->traces[packet->reg_trace.id] = new Trace(path, packet->reg_trace.id, packet->reg_trace.type, packet->reg_trace.width);
        packet->trace = this->traces[packet->reg_trace.id];
    }
    else if (packet->header.type == ED_TYPE_TIMESTAMP8 || 
             packet->header.type == ED_TYPE_TIMESTAMP16 ||
             packet->header.type == ED_TYPE_TIMESTAMP32 ||
             packet->header.type == ED_TYPE_TIMESTAMP64)
    {
        int width = packet->header.type == ED_TYPE_TIMESTAMP8 ? 1 : packet->header.type == ED_TYPE_TIMESTAMP16 ? 2 : packet->header.type == ED_TYPE_TIMESTAMP32 ? 4 : 8;

        uint64_t diff = 0;
        this->file.read((char *)&diff, width);
        if (this->file.fail())
            return -1;

        this->timestamp += diff;
        packet->timestamp = this->timestamp;
    }
    else if (packet->header.type == ED_TYPE_TRACE ||
             packet->header.type == ED_TYPE_TRACE_SET_0 || 
             packet->header.type == ED_TYPE_TRACE_SET_1)
    {
        uint32_t id = decode_id(&this->file, NULL);
        uint32_t size;

        packet->trace = this->traces[id];

        if (packet->trace == NULL)
        {
            printf("Didn't find trace ID %d\n", id);
            return -1;
        }

        if (packet->data)
            delete packet->data;

        if (packet->header.type == ED_TYPE_TRACE)
        {
            if (packet->trace->type == ED_TRACE_BITFIELD || packet->trace->type == ED_TRACE_REAL)
            {
                size = (packet->trace->width + 7) / 8;
                packet->data = new uint8_t[size];
                this->file.read((char *)packet->data, size);
                if (this->file.fail())
                    return -1;
            }
            else
            {
                this->file.read((char *)&size, 4);
                if (this->file.fail())
                    return -1;

                packet->data = new uint8_t[size];

                this->file.read((char *)packet->data, size);
                if (this->file.fail())
                    return -1;
            }
        }
        else
        {
            packet->data = new uint8_t[1];
            *(packet->data) = packet->header.type == ED_TYPE_TRACE_SET_1;
        }

        packet->size = size;
    }

    return 0;
}


trace_packet::trace_packet()
: data(NULL)
{

}


trace_packet::~trace_packet()
{
    if (this->data)
        delete this->data;
}


void trace_packet::dump()
{
    if (this->header.type == ED_TYPE_CONF)
    {
        printf("Configuration (version: %d, timescale: %s)\n", this->conf.version, this->conf.timescale == ED_CONF_TIMESCALE_PS ? "ps" : "ns");
    }
    else if (this->header.type == ED_TYPE_REG_TRACE)
    {
        printf("Trace registration (id: %d, type: %s, width: %d, path: %s)\n", this->trace->id, this->trace->type == ED_TRACE_BITFIELD ? "bitfield" : this->trace->type == ED_TRACE_REAL ? "real" : "varlen", this->trace->width, this->trace->path.c_str());
    }
    else if (this->header.type == ED_TYPE_TIMESTAMP8 || 
             this->header.type == ED_TYPE_TIMESTAMP16 || 
             this->header.type == ED_TYPE_TIMESTAMP32 || 
             this->header.type == ED_TYPE_TIMESTAMP64)
    {
    }
    else if (this->header.type == ED_TYPE_TRACE || 
             this->header.type == ED_TYPE_TRACE_SET_0 || 
             this->header.type == ED_TYPE_TRACE_SET_1)
    {
        if (this->trace->type == ED_TRACE_REAL)
        {
            double value = 0;

            if (this->size == 4)
                value = *(float *)this->data;
            else
                value = *(double *)this->data;


            printf("[%ld] Trace (id: %d, path: %s, type: real, size: %d, value: %f)\n", this->timestamp, this->trace->id, this->trace->path.c_str(), this->size, value);
        }
        else
        {
            printf("[%ld] Trace (id: %d, path: %s, type: %s, width: %d, size:%d)\n", this->timestamp, this->trace->id, this->trace->path.c_str(), this->trace->type == ED_TRACE_BITFIELD ? "bitfield" : this->trace->type == ED_TRACE_REAL ? "real" : "varlen", this->trace->width, this->size);
        }
    }
}


Trace::Trace(std::string path, int id, uint32_t type, int width)
: path(path), id(id), type(type), width(width)
{
}