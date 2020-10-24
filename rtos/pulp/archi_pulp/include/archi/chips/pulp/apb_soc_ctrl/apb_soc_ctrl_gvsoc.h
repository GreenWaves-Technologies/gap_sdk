
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

#ifndef __ARCHI_APB_SOC_CTRL_GVSOC__
#define __ARCHI_APB_SOC_CTRL_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_apb_soc_ctrl_info : public vp::reg_32
{
public:
    inline void nb_cl_set(uint32_t value) { this->set_field(value, APB_SOC_CTRL_INFO_NB_CL_BIT, APB_SOC_CTRL_INFO_NB_CL_WIDTH); }
    inline uint32_t nb_cl_get() { return this->get_field(APB_SOC_CTRL_INFO_NB_CL_BIT, APB_SOC_CTRL_INFO_NB_CL_WIDTH); }
    inline void nb_cores_set(uint32_t value) { this->set_field(value, APB_SOC_CTRL_INFO_NB_CORES_BIT, APB_SOC_CTRL_INFO_NB_CORES_WIDTH); }
    inline uint32_t nb_cores_get() { return this->get_field(APB_SOC_CTRL_INFO_NB_CORES_BIT, APB_SOC_CTRL_INFO_NB_CORES_WIDTH); }
    vp_apb_soc_ctrl_info()
    {
        this->hw_name = "INFO";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x18;
        this->regfields.push_back(new vp::regfield("NB_CL", 0, 16));
        this->regfields.push_back(new vp::regfield("NB_CORES", 16, 16));
    }
};

class vp_apb_soc_ctrl_fc_boot : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, APB_SOC_CTRL_FC_BOOT_ADDR_BIT, APB_SOC_CTRL_FC_BOOT_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(APB_SOC_CTRL_FC_BOOT_ADDR_BIT, APB_SOC_CTRL_FC_BOOT_ADDR_WIDTH); }
    vp_apb_soc_ctrl_fc_boot()
    {
        this->hw_name = "FC_BOOT";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x1a000000;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_apb_soc_ctrl_fc_fetch : public vp::reg_32
{
public:
    inline void fc_fe_set(uint32_t value) { this->set_field(value, APB_SOC_CTRL_FC_FETCH_FC_FE_BIT, APB_SOC_CTRL_FC_FETCH_FC_FE_WIDTH); }
    inline uint32_t fc_fe_get() { return this->get_field(APB_SOC_CTRL_FC_FETCH_FC_FE_BIT, APB_SOC_CTRL_FC_FETCH_FC_FE_WIDTH); }
    vp_apb_soc_ctrl_fc_fetch()
    {
        this->hw_name = "FC_FETCH";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("FC_FE", 0, 1));
    }
};

class vp_apb_soc_ctrl_corestatus : public vp::reg_32
{
public:
    inline void status_set(uint32_t value) { this->set_field(value, APB_SOC_CTRL_CORESTATUS_STATUS_BIT, APB_SOC_CTRL_CORESTATUS_STATUS_WIDTH); }
    inline uint32_t status_get() { return this->get_field(APB_SOC_CTRL_CORESTATUS_STATUS_BIT, APB_SOC_CTRL_CORESTATUS_STATUS_WIDTH); }
    inline void eoc_set(uint32_t value) { this->set_field(value, APB_SOC_CTRL_CORESTATUS_EOC_BIT, APB_SOC_CTRL_CORESTATUS_EOC_WIDTH); }
    inline uint32_t eoc_get() { return this->get_field(APB_SOC_CTRL_CORESTATUS_EOC_BIT, APB_SOC_CTRL_CORESTATUS_EOC_WIDTH); }
    vp_apb_soc_ctrl_corestatus()
    {
        this->hw_name = "CORESTATUS";
        this->offset = 0xa0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("STATUS", 0, 32));
        this->regfields.push_back(new vp::regfield("EOC", 31, 1));
    }
};


class vp_regmap_apb_soc_ctrl : public vp::regmap
{
public:
    vp_apb_soc_ctrl_info info;
    vp_apb_soc_ctrl_fc_boot fc_boot;
    vp_apb_soc_ctrl_fc_fetch fc_fetch;
    vp_apb_soc_ctrl_corestatus corestatus;
    vp_regmap_apb_soc_ctrl()
    {
        this->registers.push_back(&this->info);
        this->registers.push_back(&this->fc_boot);
        this->registers.push_back(&this->fc_fetch);
        this->registers.push_back(&this->corestatus);
    }
};

#endif

#endif
