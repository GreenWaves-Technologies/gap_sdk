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

#ifndef __TRACE_DUMPER_TYPES_H__
#define __TRACE_DUMPER_TYPES_H__

#include <stdint.h>

typedef enum
{
	ED_CONF_VERSION = 1,
	ED_CONF_LAST_VERSION = ED_CONF_VERSION,
} ed_conf_version_e;

typedef enum
{
	ED_CONF_TIMESCALE_PS = 0,
	ED_CONF_TIMESCALE_NS = 1,
} ed_conf_timescale_e;

typedef enum
{
    ED_TYPE_CONF          = 0x1,
    ED_TYPE_REG_TRACE     = 0x2,
    ED_TYPE_TIMESTAMP8    = 0x3,
    ED_TYPE_TIMESTAMP16   = 0x4,
    ED_TYPE_TIMESTAMP32   = 0x5,
    ED_TYPE_TIMESTAMP64   = 0x6,
    ED_TYPE_TRACE         = 0x7,
    ED_TYPE_TRACE_SET_0   = 0x8,
    ED_TYPE_TRACE_SET_1   = 0x9,
} ed_header_type_e;

typedef enum
{
	ED_TRACE_BITFIELD = 0x1,
    ED_TRACE_REAL     = 0x2,
    ED_TRACE_VARLEN   = 0x3,
} ed_trace_type_e;

typedef struct
{
    uint8_t type;
} ed_header_t;

typedef struct
{
	uint32_t version;
	uint32_t timescale;
} ed_conf_t;

typedef struct
{
	uint32_t type;
	uint32_t width;
	uint32_t path_len;
	uint32_t id;
} ed_reg_trace_t;



#endif