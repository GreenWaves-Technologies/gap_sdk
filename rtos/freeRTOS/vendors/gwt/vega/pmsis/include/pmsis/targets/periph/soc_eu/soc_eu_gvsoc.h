
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

#ifndef __ARCHI_SOC_EU_GVSOC__
#define __ARCHI_SOC_EU_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_soc_eu_sw_event : public vp::reg_32
{
public:
    inline void event_set(uint32_t value) { this->set_field(value, SOC_EU_SW_EVENT_EVENT_BIT, SOC_EU_SW_EVENT_EVENT_WIDTH); }
    inline uint32_t event_get() { return this->get_field(SOC_EU_SW_EVENT_EVENT_BIT, SOC_EU_SW_EVENT_EVENT_WIDTH); }
    vp_soc_eu_sw_event()
    {
        this->hw_name = "SW_EVENT";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EVENT", 0, 8));
    }
};

class vp_soc_eu_fc_mask_0 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_0_MASK_BIT, SOC_EU_FC_MASK_0_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_0_MASK_BIT, SOC_EU_FC_MASK_0_MASK_WIDTH); }
    vp_soc_eu_fc_mask_0()
    {
        this->hw_name = "FC_MASK_0";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_1 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_1_MASK_BIT, SOC_EU_FC_MASK_1_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_1_MASK_BIT, SOC_EU_FC_MASK_1_MASK_WIDTH); }
    vp_soc_eu_fc_mask_1()
    {
        this->hw_name = "FC_MASK_1";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_2 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_2_MASK_BIT, SOC_EU_FC_MASK_2_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_2_MASK_BIT, SOC_EU_FC_MASK_2_MASK_WIDTH); }
    vp_soc_eu_fc_mask_2()
    {
        this->hw_name = "FC_MASK_2";
        this->offset = 0xc;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_3 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_3_MASK_BIT, SOC_EU_FC_MASK_3_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_3_MASK_BIT, SOC_EU_FC_MASK_3_MASK_WIDTH); }
    vp_soc_eu_fc_mask_3()
    {
        this->hw_name = "FC_MASK_3";
        this->offset = 0x10;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_4 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_4_MASK_BIT, SOC_EU_FC_MASK_4_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_4_MASK_BIT, SOC_EU_FC_MASK_4_MASK_WIDTH); }
    vp_soc_eu_fc_mask_4()
    {
        this->hw_name = "FC_MASK_4";
        this->offset = 0x14;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_5 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_5_MASK_BIT, SOC_EU_FC_MASK_5_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_5_MASK_BIT, SOC_EU_FC_MASK_5_MASK_WIDTH); }
    vp_soc_eu_fc_mask_5()
    {
        this->hw_name = "FC_MASK_5";
        this->offset = 0x18;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_6 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_6_MASK_BIT, SOC_EU_FC_MASK_6_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_6_MASK_BIT, SOC_EU_FC_MASK_6_MASK_WIDTH); }
    vp_soc_eu_fc_mask_6()
    {
        this->hw_name = "FC_MASK_6";
        this->offset = 0x1c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_fc_mask_7 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_7_MASK_BIT, SOC_EU_FC_MASK_7_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_FC_MASK_7_MASK_BIT, SOC_EU_FC_MASK_7_MASK_WIDTH); }
    vp_soc_eu_fc_mask_7()
    {
        this->hw_name = "FC_MASK_7";
        this->offset = 0x20;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_0 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_0_MASK_BIT, SOC_EU_CL_MASK_0_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_0_MASK_BIT, SOC_EU_CL_MASK_0_MASK_WIDTH); }
    vp_soc_eu_cl_mask_0()
    {
        this->hw_name = "CL_MASK_0";
        this->offset = 0x24;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_1 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_1_MASK_BIT, SOC_EU_CL_MASK_1_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_1_MASK_BIT, SOC_EU_CL_MASK_1_MASK_WIDTH); }
    vp_soc_eu_cl_mask_1()
    {
        this->hw_name = "CL_MASK_1";
        this->offset = 0x28;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_2 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_2_MASK_BIT, SOC_EU_CL_MASK_2_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_2_MASK_BIT, SOC_EU_CL_MASK_2_MASK_WIDTH); }
    vp_soc_eu_cl_mask_2()
    {
        this->hw_name = "CL_MASK_2";
        this->offset = 0x2c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_3 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_3_MASK_BIT, SOC_EU_CL_MASK_3_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_3_MASK_BIT, SOC_EU_CL_MASK_3_MASK_WIDTH); }
    vp_soc_eu_cl_mask_3()
    {
        this->hw_name = "CL_MASK_3";
        this->offset = 0x30;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_4 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_4_MASK_BIT, SOC_EU_CL_MASK_4_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_4_MASK_BIT, SOC_EU_CL_MASK_4_MASK_WIDTH); }
    vp_soc_eu_cl_mask_4()
    {
        this->hw_name = "CL_MASK_4";
        this->offset = 0x34;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_5 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_5_MASK_BIT, SOC_EU_CL_MASK_5_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_5_MASK_BIT, SOC_EU_CL_MASK_5_MASK_WIDTH); }
    vp_soc_eu_cl_mask_5()
    {
        this->hw_name = "CL_MASK_5";
        this->offset = 0x38;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_6 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_6_MASK_BIT, SOC_EU_CL_MASK_6_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_6_MASK_BIT, SOC_EU_CL_MASK_6_MASK_WIDTH); }
    vp_soc_eu_cl_mask_6()
    {
        this->hw_name = "CL_MASK_6";
        this->offset = 0x3c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_cl_mask_7 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_7_MASK_BIT, SOC_EU_CL_MASK_7_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_CL_MASK_7_MASK_BIT, SOC_EU_CL_MASK_7_MASK_WIDTH); }
    vp_soc_eu_cl_mask_7()
    {
        this->hw_name = "CL_MASK_7";
        this->offset = 0x40;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_0 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_0_MASK_BIT, SOC_EU_PR_MASK_0_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_0_MASK_BIT, SOC_EU_PR_MASK_0_MASK_WIDTH); }
    vp_soc_eu_pr_mask_0()
    {
        this->hw_name = "PR_MASK_0";
        this->offset = 0x44;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_1 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_1_MASK_BIT, SOC_EU_PR_MASK_1_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_1_MASK_BIT, SOC_EU_PR_MASK_1_MASK_WIDTH); }
    vp_soc_eu_pr_mask_1()
    {
        this->hw_name = "PR_MASK_1";
        this->offset = 0x48;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_2 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_2_MASK_BIT, SOC_EU_PR_MASK_2_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_2_MASK_BIT, SOC_EU_PR_MASK_2_MASK_WIDTH); }
    vp_soc_eu_pr_mask_2()
    {
        this->hw_name = "PR_MASK_2";
        this->offset = 0x4c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_3 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_3_MASK_BIT, SOC_EU_PR_MASK_3_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_3_MASK_BIT, SOC_EU_PR_MASK_3_MASK_WIDTH); }
    vp_soc_eu_pr_mask_3()
    {
        this->hw_name = "PR_MASK_3";
        this->offset = 0x50;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_4 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_4_MASK_BIT, SOC_EU_PR_MASK_4_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_4_MASK_BIT, SOC_EU_PR_MASK_4_MASK_WIDTH); }
    vp_soc_eu_pr_mask_4()
    {
        this->hw_name = "PR_MASK_4";
        this->offset = 0x54;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_5 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_5_MASK_BIT, SOC_EU_PR_MASK_5_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_5_MASK_BIT, SOC_EU_PR_MASK_5_MASK_WIDTH); }
    vp_soc_eu_pr_mask_5()
    {
        this->hw_name = "PR_MASK_5";
        this->offset = 0x58;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_6 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_6_MASK_BIT, SOC_EU_PR_MASK_6_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_6_MASK_BIT, SOC_EU_PR_MASK_6_MASK_WIDTH); }
    vp_soc_eu_pr_mask_6()
    {
        this->hw_name = "PR_MASK_6";
        this->offset = 0x5c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_pr_mask_7 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_7_MASK_BIT, SOC_EU_PR_MASK_7_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_PR_MASK_7_MASK_BIT, SOC_EU_PR_MASK_7_MASK_WIDTH); }
    vp_soc_eu_pr_mask_7()
    {
        this->hw_name = "PR_MASK_7";
        this->offset = 0x60;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0xffffffff;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_0 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_0_MASK_BIT, SOC_EU_ERR_0_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_0_MASK_BIT, SOC_EU_ERR_0_MASK_WIDTH); }
    vp_soc_eu_err_0()
    {
        this->hw_name = "ERR_0";
        this->offset = 0x64;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_1 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_1_MASK_BIT, SOC_EU_ERR_1_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_1_MASK_BIT, SOC_EU_ERR_1_MASK_WIDTH); }
    vp_soc_eu_err_1()
    {
        this->hw_name = "ERR_1";
        this->offset = 0x68;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_2 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_2_MASK_BIT, SOC_EU_ERR_2_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_2_MASK_BIT, SOC_EU_ERR_2_MASK_WIDTH); }
    vp_soc_eu_err_2()
    {
        this->hw_name = "ERR_2";
        this->offset = 0x6c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_3 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_3_MASK_BIT, SOC_EU_ERR_3_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_3_MASK_BIT, SOC_EU_ERR_3_MASK_WIDTH); }
    vp_soc_eu_err_3()
    {
        this->hw_name = "ERR_3";
        this->offset = 0x70;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_4 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_4_MASK_BIT, SOC_EU_ERR_4_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_4_MASK_BIT, SOC_EU_ERR_4_MASK_WIDTH); }
    vp_soc_eu_err_4()
    {
        this->hw_name = "ERR_4";
        this->offset = 0x74;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_5 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_5_MASK_BIT, SOC_EU_ERR_5_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_5_MASK_BIT, SOC_EU_ERR_5_MASK_WIDTH); }
    vp_soc_eu_err_5()
    {
        this->hw_name = "ERR_5";
        this->offset = 0x78;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_6 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_6_MASK_BIT, SOC_EU_ERR_6_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_6_MASK_BIT, SOC_EU_ERR_6_MASK_WIDTH); }
    vp_soc_eu_err_6()
    {
        this->hw_name = "ERR_6";
        this->offset = 0x7c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_err_7 : public vp::reg_32
{
public:
    inline void mask_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_7_MASK_BIT, SOC_EU_ERR_7_MASK_WIDTH); }
    inline uint32_t mask_get() { return this->get_field(SOC_EU_ERR_7_MASK_BIT, SOC_EU_ERR_7_MASK_WIDTH); }
    vp_soc_eu_err_7()
    {
        this->hw_name = "ERR_7";
        this->offset = 0x80;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("MASK", 0, 32));
    }
};

class vp_soc_eu_timer1_sel_hi : public vp::reg_32
{
public:
    inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_HI_EVT_BIT, SOC_EU_TIMER1_SEL_HI_EVT_WIDTH); }
    inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER1_SEL_HI_EVT_BIT, SOC_EU_TIMER1_SEL_HI_EVT_WIDTH); }
    inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_HI_ENA_BIT, SOC_EU_TIMER1_SEL_HI_ENA_WIDTH); }
    inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER1_SEL_HI_ENA_BIT, SOC_EU_TIMER1_SEL_HI_ENA_WIDTH); }
    vp_soc_eu_timer1_sel_hi()
    {
        this->hw_name = "TIMER1_SEL_HI";
        this->offset = 0x84;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x800000ff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EVT", 0, 8));
        this->regfields.push_back(new vp::regfield("ENA", 31, 1));
    }
};

class vp_soc_eu_timer1_sel_lo : public vp::reg_32
{
public:
    inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_LO_EVT_BIT, SOC_EU_TIMER1_SEL_LO_EVT_WIDTH); }
    inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER1_SEL_LO_EVT_BIT, SOC_EU_TIMER1_SEL_LO_EVT_WIDTH); }
    inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_LO_ENA_BIT, SOC_EU_TIMER1_SEL_LO_ENA_WIDTH); }
    inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER1_SEL_LO_ENA_BIT, SOC_EU_TIMER1_SEL_LO_ENA_WIDTH); }
    vp_soc_eu_timer1_sel_lo()
    {
        this->hw_name = "TIMER1_SEL_LO";
        this->offset = 0x88;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x800000ff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EVT", 0, 8));
        this->regfields.push_back(new vp::regfield("ENA", 31, 1));
    }
};

class vp_soc_eu_timer2_sel_hi : public vp::reg_32
{
public:
    inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_HI_EVT_BIT, SOC_EU_TIMER2_SEL_HI_EVT_WIDTH); }
    inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER2_SEL_HI_EVT_BIT, SOC_EU_TIMER2_SEL_HI_EVT_WIDTH); }
    inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_HI_ENA_BIT, SOC_EU_TIMER2_SEL_HI_ENA_WIDTH); }
    inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER2_SEL_HI_ENA_BIT, SOC_EU_TIMER2_SEL_HI_ENA_WIDTH); }
    vp_soc_eu_timer2_sel_hi()
    {
        this->hw_name = "TIMER2_SEL_HI";
        this->offset = 0x8c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x800000ff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EVT", 0, 8));
        this->regfields.push_back(new vp::regfield("ENA", 31, 1));
    }
};

class vp_soc_eu_timer2_sel_lo : public vp::reg_32
{
public:
    inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_LO_EVT_BIT, SOC_EU_TIMER2_SEL_LO_EVT_WIDTH); }
    inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER2_SEL_LO_EVT_BIT, SOC_EU_TIMER2_SEL_LO_EVT_WIDTH); }
    inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_LO_ENA_BIT, SOC_EU_TIMER2_SEL_LO_ENA_WIDTH); }
    inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER2_SEL_LO_ENA_BIT, SOC_EU_TIMER2_SEL_LO_ENA_WIDTH); }
    vp_soc_eu_timer2_sel_lo()
    {
        this->hw_name = "TIMER2_SEL_LO";
        this->offset = 0x90;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x800000ff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EVT", 0, 8));
        this->regfields.push_back(new vp::regfield("ENA", 31, 1));
    }
};

class vp_regmap_soc_eu : public vp::regmap
{
public:
    vp_soc_eu_sw_event sw_event;
    vp_soc_eu_fc_mask_0 fc_mask_0;
    vp_soc_eu_fc_mask_1 fc_mask_1;
    vp_soc_eu_fc_mask_2 fc_mask_2;
    vp_soc_eu_fc_mask_3 fc_mask_3;
    vp_soc_eu_fc_mask_4 fc_mask_4;
    vp_soc_eu_fc_mask_5 fc_mask_5;
    vp_soc_eu_fc_mask_6 fc_mask_6;
    vp_soc_eu_fc_mask_7 fc_mask_7;
    vp_soc_eu_cl_mask_0 cl_mask_0;
    vp_soc_eu_cl_mask_1 cl_mask_1;
    vp_soc_eu_cl_mask_2 cl_mask_2;
    vp_soc_eu_cl_mask_3 cl_mask_3;
    vp_soc_eu_cl_mask_4 cl_mask_4;
    vp_soc_eu_cl_mask_5 cl_mask_5;
    vp_soc_eu_cl_mask_6 cl_mask_6;
    vp_soc_eu_cl_mask_7 cl_mask_7;
    vp_soc_eu_pr_mask_0 pr_mask_0;
    vp_soc_eu_pr_mask_1 pr_mask_1;
    vp_soc_eu_pr_mask_2 pr_mask_2;
    vp_soc_eu_pr_mask_3 pr_mask_3;
    vp_soc_eu_pr_mask_4 pr_mask_4;
    vp_soc_eu_pr_mask_5 pr_mask_5;
    vp_soc_eu_pr_mask_6 pr_mask_6;
    vp_soc_eu_pr_mask_7 pr_mask_7;
    vp_soc_eu_err_0 err_0;
    vp_soc_eu_err_1 err_1;
    vp_soc_eu_err_2 err_2;
    vp_soc_eu_err_3 err_3;
    vp_soc_eu_err_4 err_4;
    vp_soc_eu_err_5 err_5;
    vp_soc_eu_err_6 err_6;
    vp_soc_eu_err_7 err_7;
    vp_soc_eu_timer1_sel_hi timer1_sel_hi;
    vp_soc_eu_timer1_sel_lo timer1_sel_lo;
    vp_soc_eu_timer2_sel_hi timer2_sel_hi;
    vp_soc_eu_timer2_sel_lo timer2_sel_lo;
    vp_regmap_soc_eu()
    {
        this->registers.push_back(&this->sw_event);
        this->registers.push_back(&this->fc_mask_0);
        this->registers.push_back(&this->fc_mask_1);
        this->registers.push_back(&this->fc_mask_2);
        this->registers.push_back(&this->fc_mask_3);
        this->registers.push_back(&this->fc_mask_4);
        this->registers.push_back(&this->fc_mask_5);
        this->registers.push_back(&this->fc_mask_6);
        this->registers.push_back(&this->fc_mask_7);
        this->registers.push_back(&this->cl_mask_0);
        this->registers.push_back(&this->cl_mask_1);
        this->registers.push_back(&this->cl_mask_2);
        this->registers.push_back(&this->cl_mask_3);
        this->registers.push_back(&this->cl_mask_4);
        this->registers.push_back(&this->cl_mask_5);
        this->registers.push_back(&this->cl_mask_6);
        this->registers.push_back(&this->cl_mask_7);
        this->registers.push_back(&this->pr_mask_0);
        this->registers.push_back(&this->pr_mask_1);
        this->registers.push_back(&this->pr_mask_2);
        this->registers.push_back(&this->pr_mask_3);
        this->registers.push_back(&this->pr_mask_4);
        this->registers.push_back(&this->pr_mask_5);
        this->registers.push_back(&this->pr_mask_6);
        this->registers.push_back(&this->pr_mask_7);
        this->registers.push_back(&this->err_0);
        this->registers.push_back(&this->err_1);
        this->registers.push_back(&this->err_2);
        this->registers.push_back(&this->err_3);
        this->registers.push_back(&this->err_4);
        this->registers.push_back(&this->err_5);
        this->registers.push_back(&this->err_6);
        this->registers.push_back(&this->err_7);
        this->registers.push_back(&this->timer1_sel_hi);
        this->registers.push_back(&this->timer1_sel_lo);
        this->registers.push_back(&this->timer2_sel_hi);
        this->registers.push_back(&this->timer2_sel_lo);
    }
};

#endif

#endif
