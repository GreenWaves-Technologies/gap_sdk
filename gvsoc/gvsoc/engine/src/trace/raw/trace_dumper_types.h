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