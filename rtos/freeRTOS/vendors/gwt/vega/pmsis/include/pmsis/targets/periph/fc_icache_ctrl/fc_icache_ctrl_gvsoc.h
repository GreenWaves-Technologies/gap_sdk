
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_FC_ICACHE_CTRL_GVSOC__
#define __ARCHI_FC_ICACHE_CTRL_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_fc_icache_ctrl_enable : public vp::reg_32
{
public:
    inline void en_set(uint32_t value) { this->set_field(value, FC_ICACHE_CTRL_ENABLE_EN_BIT, FC_ICACHE_CTRL_ENABLE_EN_WIDTH); }
    inline uint32_t en_get() { return this->get_field(FC_ICACHE_CTRL_ENABLE_EN_BIT, FC_ICACHE_CTRL_ENABLE_EN_WIDTH); }
    vp_fc_icache_ctrl_enable()
    {
        this->hw_name = "ENABLE";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EN", 0, 1));
    }
};

class vp_fc_icache_ctrl_flush : public vp::reg_32
{
public:
    inline void fl_set(uint32_t value) { this->set_field(value, FC_ICACHE_CTRL_FLUSH_FL_BIT, FC_ICACHE_CTRL_FLUSH_FL_WIDTH); }
    inline uint32_t fl_get() { return this->get_field(FC_ICACHE_CTRL_FLUSH_FL_BIT, FC_ICACHE_CTRL_FLUSH_FL_WIDTH); }
    vp_fc_icache_ctrl_flush()
    {
        this->hw_name = "FLUSH";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("FL", 0, 1));
    }
};

class vp_fc_icache_ctrl_sel_flush : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, FC_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT, FC_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(FC_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT, FC_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH); }
    vp_fc_icache_ctrl_sel_flush()
    {
        this->hw_name = "SEL_FLUSH";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_fc_icache_ctrl_status : public vp::reg_32
{
public:
    inline void status_set(uint32_t value) { this->set_field(value, FC_ICACHE_CTRL_STATUS_STATUS_BIT, FC_ICACHE_CTRL_STATUS_STATUS_WIDTH); }
    inline uint32_t status_get() { return this->get_field(FC_ICACHE_CTRL_STATUS_STATUS_BIT, FC_ICACHE_CTRL_STATUS_STATUS_WIDTH); }
    vp_fc_icache_ctrl_status()
    {
        this->hw_name = "STATUS";
        this->offset = 0xc;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("STATUS", 0, 1));
    }
};


class vp_regmap_fc_icache_ctrl : public vp::regmap
{
public:
    vp_fc_icache_ctrl_enable enable;
    vp_fc_icache_ctrl_flush flush;
    vp_fc_icache_ctrl_sel_flush sel_flush;
    vp_fc_icache_ctrl_status status;
    vp_regmap_fc_icache_ctrl()
    {
        this->registers.push_back(&this->enable);
        this->registers.push_back(&this->flush);
        this->registers.push_back(&this->sel_flush);
        this->registers.push_back(&this->status);
    }
};

#endif

#endif
