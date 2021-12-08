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

#pragma once

#include <vp/vp.hpp>

typedef enum
{
    MRAM_OP_TRIM_CFG    = 0x01,
    MRAM_OP_NORMAL_TX   = 0x02,
    MRAM_OP_ERASE_CHIP  = 0x04,
    MRAM_OP_ERASE_SECT  = 0x08,
    MRAM_OP_ERASE_WORD  = 0x10,
    MRAM_OP_PWDN        = 0x20,
    MRAM_OP_READ_RX     = 0x40,
    MRAM_OP_REF_LINE_P  = 0x80,
    MRAM_OP_REF_LINE_AP = 0xC0,
} mram_op_e;

typedef struct {
    uint32_t div;
} mram_conf_t;

typedef struct {
    mram_op_e operation;
    uint32_t addr;
    uint32_t size;
} mram_req_t;

typedef struct {
    uint8_t *data;
    int size;
} mram_data_t;
