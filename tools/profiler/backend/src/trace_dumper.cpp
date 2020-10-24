/*
 * Copyright (C) 2019 Green_waves Technologies
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

#include "trace_dumper.hpp"
#include <string>
#include <cstring>
#include <iostream>

trace_dumper_trace::trace_dumper_trace(trace_dumper_client *client, int id, ed_trace_type_e type, int width)
: type(type), width(width), client(client)
{
    this->id = encode_id(id, &this->id_size);
}


void trace_dumper_trace::dump(int64_t timestamp, uint8_t *value, int width)
{
    this->client->dump_trace(timestamp, this->id, this->id_size, this->type, value, width);
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
        /* register a new trace in the server */
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
        packet->timestamp = this->timestamp;

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
            size = 1;
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


void trace_packet::dump() const
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
            printf("[%ld] Trace (id: %d, path: %s, type: %s, width: %d, size:%d)\n",
                this->timestamp, this->trace->id, this->trace->path.c_str(),
                this->trace->type == ED_TRACE_BITFIELD ? (this->header.type == ED_TYPE_TRACE_SET_0 ? "BIT 0" : "BIT 1") : this->trace->type == ED_TRACE_REAL ? "real" : "varlen",
                this->trace->width, this->size);

            //if (this->trace->type == ED_TRACE_VARLEN) printf("func, content = %s\n", (char*) this->data);
        }
    }
}


Trace::Trace(std::string path, int id, uint32_t type, int width)
: path(path), id(id), type(type), width(width)
{
}
