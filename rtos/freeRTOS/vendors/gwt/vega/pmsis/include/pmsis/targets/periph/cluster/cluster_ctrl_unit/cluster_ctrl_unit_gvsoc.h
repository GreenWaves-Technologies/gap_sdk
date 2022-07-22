
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

class vp_cluster_ctrl_unit_eoc : public vp::reg_32
{
public:
    inline void eoc_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_EOC_EOC_BIT, CLUSTER_CTRL_UNIT_EOC_EOC_WIDTH); }
    inline uint32_t eoc_get() { return this->get_field(CLUSTER_CTRL_UNIT_EOC_EOC_BIT, CLUSTER_CTRL_UNIT_EOC_EOC_WIDTH); }
    vp_cluster_ctrl_unit_eoc()
    {
        this->hw_name = "EOC";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EOC", 0, 1));
    }
};

class vp_cluster_ctrl_unit_fetch_en : public vp::reg_32
{
public:
    inline void core0_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_WIDTH); }
    inline uint32_t core0_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_WIDTH); }
    inline void core1_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_WIDTH); }
    inline uint32_t core1_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_WIDTH); }
    inline void core2_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_WIDTH); }
    inline uint32_t core2_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_WIDTH); }
    inline void core3_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_WIDTH); }
    inline uint32_t core3_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_WIDTH); }
    inline void core4_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_WIDTH); }
    inline uint32_t core4_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_WIDTH); }
    inline void core5_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_WIDTH); }
    inline uint32_t core5_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_WIDTH); }
    inline void core6_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_WIDTH); }
    inline uint32_t core6_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_WIDTH); }
    inline void core7_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_WIDTH); }
    inline uint32_t core7_get() { return this->get_field(CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_BIT, CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_WIDTH); }
    vp_cluster_ctrl_unit_fetch_en()
    {
        this->hw_name = "FETCH_EN";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("CORE0", 0, 1));
        this->regfields.push_back(new vp::regfield("CORE1", 1, 1));
        this->regfields.push_back(new vp::regfield("CORE2", 2, 1));
        this->regfields.push_back(new vp::regfield("CORE3", 3, 1));
        this->regfields.push_back(new vp::regfield("CORE4", 4, 1));
        this->regfields.push_back(new vp::regfield("CORE5", 5, 1));
        this->regfields.push_back(new vp::regfield("CORE6", 6, 1));
        this->regfields.push_back(new vp::regfield("CORE7", 7, 1));
    }
};

class vp_cluster_ctrl_unit_clock_gate : public vp::reg_32
{
public:
    inline void en_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_BIT, CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_WIDTH); }
    inline uint32_t en_get() { return this->get_field(CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_BIT, CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_WIDTH); }
    vp_cluster_ctrl_unit_clock_gate()
    {
        this->hw_name = "CLOCK_GATE";
        this->offset = 0x20;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("EN", 0, 1));
    }
};

class vp_cluster_ctrl_unit_dbg_resume : public vp::reg_32
{
public:
    inline void core0_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_WIDTH); }
    inline uint32_t core0_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_WIDTH); }
    inline void core1_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_WIDTH); }
    inline uint32_t core1_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_WIDTH); }
    inline void core2_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_WIDTH); }
    inline uint32_t core2_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_WIDTH); }
    inline void core3_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_WIDTH); }
    inline uint32_t core3_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_WIDTH); }
    inline void core4_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_WIDTH); }
    inline uint32_t core4_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_WIDTH); }
    inline void core5_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_WIDTH); }
    inline uint32_t core5_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_WIDTH); }
    inline void core6_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_WIDTH); }
    inline uint32_t core6_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_WIDTH); }
    inline void core7_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_WIDTH); }
    inline uint32_t core7_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_BIT, CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_WIDTH); }
    vp_cluster_ctrl_unit_dbg_resume()
    {
        this->hw_name = "DBG_RESUME";
        this->offset = 0x28;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("CORE0", 0, 1));
        this->regfields.push_back(new vp::regfield("CORE1", 1, 1));
        this->regfields.push_back(new vp::regfield("CORE2", 2, 1));
        this->regfields.push_back(new vp::regfield("CORE3", 3, 1));
        this->regfields.push_back(new vp::regfield("CORE4", 4, 1));
        this->regfields.push_back(new vp::regfield("CORE5", 5, 1));
        this->regfields.push_back(new vp::regfield("CORE6", 6, 1));
        this->regfields.push_back(new vp::regfield("CORE7", 7, 1));
    }
};

class vp_cluster_ctrl_unit_dbg_halt_status : public vp::reg_32
{
public:
    inline void core0_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_WIDTH); }
    inline uint32_t core0_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_WIDTH); }
    inline void core1_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_WIDTH); }
    inline uint32_t core1_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_WIDTH); }
    inline void core2_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_WIDTH); }
    inline uint32_t core2_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_WIDTH); }
    inline void core3_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_WIDTH); }
    inline uint32_t core3_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_WIDTH); }
    inline void core4_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_WIDTH); }
    inline uint32_t core4_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_WIDTH); }
    inline void core5_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_WIDTH); }
    inline uint32_t core5_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_WIDTH); }
    inline void core6_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_WIDTH); }
    inline uint32_t core6_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_WIDTH); }
    inline void core7_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_WIDTH); }
    inline uint32_t core7_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_WIDTH); }
    vp_cluster_ctrl_unit_dbg_halt_status()
    {
        this->hw_name = "DBG_HALT_STATUS";
        this->offset = 0x28;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("CORE0", 0, 1));
        this->regfields.push_back(new vp::regfield("CORE1", 1, 1));
        this->regfields.push_back(new vp::regfield("CORE2", 2, 1));
        this->regfields.push_back(new vp::regfield("CORE3", 3, 1));
        this->regfields.push_back(new vp::regfield("CORE4", 4, 1));
        this->regfields.push_back(new vp::regfield("CORE5", 5, 1));
        this->regfields.push_back(new vp::regfield("CORE6", 6, 1));
        this->regfields.push_back(new vp::regfield("CORE7", 7, 1));
    }
};

class vp_cluster_ctrl_unit_dbg_halt_mask : public vp::reg_32
{
public:
    inline void core0_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_WIDTH); }
    inline uint32_t core0_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_WIDTH); }
    inline void core1_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_WIDTH); }
    inline uint32_t core1_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_WIDTH); }
    inline void core2_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_WIDTH); }
    inline uint32_t core2_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_WIDTH); }
    inline void core3_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_WIDTH); }
    inline uint32_t core3_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_WIDTH); }
    inline void core4_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_WIDTH); }
    inline uint32_t core4_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_WIDTH); }
    inline void core5_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_WIDTH); }
    inline uint32_t core5_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_WIDTH); }
    inline void core6_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_WIDTH); }
    inline uint32_t core6_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_WIDTH); }
    inline void core7_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_WIDTH); }
    inline uint32_t core7_get() { return this->get_field(CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_BIT, CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_WIDTH); }
    vp_cluster_ctrl_unit_dbg_halt_mask()
    {
        this->hw_name = "DBG_HALT_MASK";
        this->offset = 0x38;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("CORE0", 0, 1));
        this->regfields.push_back(new vp::regfield("CORE1", 1, 1));
        this->regfields.push_back(new vp::regfield("CORE2", 2, 1));
        this->regfields.push_back(new vp::regfield("CORE3", 3, 1));
        this->regfields.push_back(new vp::regfield("CORE4", 4, 1));
        this->regfields.push_back(new vp::regfield("CORE5", 5, 1));
        this->regfields.push_back(new vp::regfield("CORE6", 6, 1));
        this->regfields.push_back(new vp::regfield("CORE7", 7, 1));
    }
};

class vp_cluster_ctrl_unit_boot_addr0 : public vp::reg_32
{
public:
    inline void ba_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_BIT, CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_WIDTH); }
    inline uint32_t ba_get() { return this->get_field(CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_BIT, CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_WIDTH); }
    vp_cluster_ctrl_unit_boot_addr0()
    {
        this->hw_name = "BOOT_ADDR0";
        this->offset = 0x40;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("BA", 0, 32));
    }
};

class vp_cluster_ctrl_unit_boot_addr1 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr1()
    {
        this->hw_name = "BOOT_ADDR1";
        this->offset = 0x44;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr2 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr2()
    {
        this->hw_name = "BOOT_ADDR2";
        this->offset = 0x48;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr3 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr3()
    {
        this->hw_name = "BOOT_ADDR3";
        this->offset = 0x4c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr4 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr4()
    {
        this->hw_name = "BOOT_ADDR4";
        this->offset = 0x50;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr5 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr5()
    {
        this->hw_name = "BOOT_ADDR5";
        this->offset = 0x54;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr6 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr6()
    {
        this->hw_name = "BOOT_ADDR6";
        this->offset = 0x58;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_boot_addr7 : public vp::reg_32
{
public:
    vp_cluster_ctrl_unit_boot_addr7()
    {
        this->hw_name = "BOOT_ADDR7";
        this->offset = 0x5c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_cluster_ctrl_unit_tcdm_arb_policy_ch0 : public vp::reg_32
{
public:
    inline void pol_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_WIDTH); }
    inline uint32_t pol_get() { return this->get_field(CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_WIDTH); }
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch0()
    {
        this->hw_name = "TCDM_ARB_POLICY_CH0";
        this->offset = 0x80;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("POL", 0, 1));
    }
};

class vp_cluster_ctrl_unit_tcdm_arb_policy_ch1 : public vp::reg_32
{
public:
    inline void pol_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_WIDTH); }
    inline uint32_t pol_get() { return this->get_field(CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_WIDTH); }
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch1()
    {
        this->hw_name = "TCDM_ARB_POLICY_CH1";
        this->offset = 0x88;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("POL", 0, 1));
    }
};

class vp_cluster_ctrl_unit_tcdm_arb_policy_ch0_rep : public vp::reg_32
{
public:
    inline void pol_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_WIDTH); }
    inline uint32_t pol_get() { return this->get_field(CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_WIDTH); }
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch0_rep()
    {
        this->hw_name = "TCDM_ARB_POLICY_CH0_REP";
        this->offset = 0xc0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("POL", 0, 1));
    }
};

class vp_cluster_ctrl_unit_tcdm_arb_policy_ch1_rep : public vp::reg_32
{
public:
    inline void pol_set(uint32_t value) { this->set_field(value, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_WIDTH); }
    inline uint32_t pol_get() { return this->get_field(CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_BIT, CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_WIDTH); }
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch1_rep()
    {
        this->hw_name = "TCDM_ARB_POLICY_CH1_REP";
        this->offset = 0xc8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("POL", 0, 1));
    }
};


class vp_regmap_cluster_ctrl_unit : public vp::regmap
{
public:
    vp_cluster_ctrl_unit_eoc eoc;
    vp_cluster_ctrl_unit_fetch_en fetch_en;
    vp_cluster_ctrl_unit_clock_gate clock_gate;
    vp_cluster_ctrl_unit_dbg_resume dbg_resume;
    vp_cluster_ctrl_unit_dbg_halt_status dbg_halt_status;
    vp_cluster_ctrl_unit_dbg_halt_mask dbg_halt_mask;
    vp_cluster_ctrl_unit_boot_addr0 boot_addr0;
    vp_cluster_ctrl_unit_boot_addr1 boot_addr1;
    vp_cluster_ctrl_unit_boot_addr2 boot_addr2;
    vp_cluster_ctrl_unit_boot_addr3 boot_addr3;
    vp_cluster_ctrl_unit_boot_addr4 boot_addr4;
    vp_cluster_ctrl_unit_boot_addr5 boot_addr5;
    vp_cluster_ctrl_unit_boot_addr6 boot_addr6;
    vp_cluster_ctrl_unit_boot_addr7 boot_addr7;
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch0 tcdm_arb_policy_ch0;
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch1 tcdm_arb_policy_ch1;
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch0_rep tcdm_arb_policy_ch0_rep;
    vp_cluster_ctrl_unit_tcdm_arb_policy_ch1_rep tcdm_arb_policy_ch1_rep;
    vp_regmap_cluster_ctrl_unit()
    {
        this->registers.push_back(&this->eoc);
        this->registers.push_back(&this->fetch_en);
        this->registers.push_back(&this->clock_gate);
        this->registers.push_back(&this->dbg_resume);
        this->registers.push_back(&this->dbg_halt_status);
        this->registers.push_back(&this->dbg_halt_mask);
        this->registers.push_back(&this->boot_addr0);
        this->registers.push_back(&this->boot_addr1);
        this->registers.push_back(&this->boot_addr2);
        this->registers.push_back(&this->boot_addr3);
        this->registers.push_back(&this->boot_addr4);
        this->registers.push_back(&this->boot_addr5);
        this->registers.push_back(&this->boot_addr6);
        this->registers.push_back(&this->boot_addr7);
        this->registers.push_back(&this->tcdm_arb_policy_ch0);
        this->registers.push_back(&this->tcdm_arb_policy_ch1);
        this->registers.push_back(&this->tcdm_arb_policy_ch0_rep);
        this->registers.push_back(&this->tcdm_arb_policy_ch1_rep);
    }
};

#endif

#endif
