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

#ifndef __TRACE_DUMPER_UTILS_H__
#define __TRACE_DUMPER_UTILS_H__


#include <stdint.h>
#include <fstream>

static inline uint32_t encode_id(uint32_t id, int *_id_size)
{
    uint32_t encoded_id = 0;
    int id_size = 1;

    // ID is encoded with 7 bits every 8 bits
    while (id)
    {
        encoded_id |= (id & 0x7f) << ((id_size - 1)*8);

        // Stop once the whole ID is encoded
        if (id <= 0x7f)
            break;

        // Otherwise, set 8th bit of the byte to 1, to specify we continue
        // to next byte
        encoded_id |= 1<<(id_size*8-1);

        id_size++;
        id >>= 7;
    }

    if (_id_size)
        *_id_size = id_size;

    return encoded_id;
}


static inline uint32_t decode_id(std::ifstream *file, int *id_size)
{
    uint32_t id = 0;
    int offset = 0;

    while(1)
    {
        uint8_t byte;
        file->read((char *)&byte, 1);
        if (file->fail())
            return -1;

        id = id | ((byte & 0x7f) << offset);
        offset += 7;

        if (byte < 128)
            break;
    }

    if (id_size)
        *id_size = (offset + 7) / 8;

    return id;
}


static inline unsigned int td_get_timestamp(uint64_t diff, int *ts_size)
{
    unsigned int type;

    if (diff < (1ULL << 8))
    {
        type = ED_TYPE_TIMESTAMP8;
        *ts_size = 1;
    }
    else if (diff < (1ULL << 16))
    {
        type = ED_TYPE_TIMESTAMP16;
        *ts_size = 2;
    }
    else if (diff < (1ULL << 32))
    {
        type = ED_TYPE_TIMESTAMP32;
        *ts_size = 4;
    }
    else
    {
        type = ED_TYPE_TIMESTAMP64;
        *ts_size = 8;
    }

    return type;
}


#endif