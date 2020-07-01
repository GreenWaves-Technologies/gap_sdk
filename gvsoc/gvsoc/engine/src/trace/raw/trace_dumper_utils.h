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