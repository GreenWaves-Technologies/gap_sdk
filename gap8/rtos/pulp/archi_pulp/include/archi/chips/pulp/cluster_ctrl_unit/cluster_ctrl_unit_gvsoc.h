
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_GVSOC__
#define __ARCHI_CLUSTER_CTRL_UNIT_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_cluster_ctrl_unit_fetch_en : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_fetch_en()
    {
        this->hw_name = "FETCH_EN";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_clock_gate : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_clock_gate()
    {
        this->hw_name = "CLOCK_GATE";
        this->offset = 0x20;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr0 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr0()
    {
        this->hw_name = "BOOT_ADDR0";
        this->offset = 0x40;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};


class vp_regmap_cluster_ctrl_unit : public vp::regmap
{
public:
    vp_cluster_ctrl_unit_fetch_en fetch_en;
    vp_cluster_ctrl_unit_clock_gate clock_gate;
    vp_cluster_ctrl_unit_boot_addr0 boot_addr0;
    vp_regmap_cluster_ctrl_unit()
    {
        this->registers.push_back(&this->fetch_en);
        this->registers.push_back(&this->clock_gate);
        this->registers.push_back(&this->boot_addr0);
    }
};

#endif

#endif
