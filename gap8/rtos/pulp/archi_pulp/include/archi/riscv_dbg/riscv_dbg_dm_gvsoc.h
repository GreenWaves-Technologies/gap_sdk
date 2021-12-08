
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

#ifndef __ARCHI_RISCV_DBG_DM_GVSOC__
#define __ARCHI_RISCV_DBG_DM_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_riscv_dbg_dm_data0 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DATA0_DATA_BIT, RISCV_DBG_DM_DATA0_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_DATA0_DATA_BIT, RISCV_DBG_DM_DATA0_DATA_WIDTH); }
    vp_riscv_dbg_dm_data0()
    {
        this->hw_name = "data0";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_data1 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DATA1_DATA_BIT, RISCV_DBG_DM_DATA1_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_DATA1_DATA_BIT, RISCV_DBG_DM_DATA1_DATA_WIDTH); }
    vp_riscv_dbg_dm_data1()
    {
        this->hw_name = "data1";
        this->offset = 0x5;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_dmcontrol : public vp::reg_32
{
public:
    inline void dmactive_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_DMACTIVE_BIT, RISCV_DBG_DM_DMCONTROL_DMACTIVE_WIDTH); }
    inline uint32_t dmactive_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_DMACTIVE_BIT, RISCV_DBG_DM_DMCONTROL_DMACTIVE_WIDTH); }
    inline void ndmreset_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_NDMRESET_BIT, RISCV_DBG_DM_DMCONTROL_NDMRESET_WIDTH); }
    inline uint32_t ndmreset_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_NDMRESET_BIT, RISCV_DBG_DM_DMCONTROL_NDMRESET_WIDTH); }
    inline void clrresethaltreq_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_BIT, RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_WIDTH); }
    inline uint32_t clrresethaltreq_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_BIT, RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_WIDTH); }
    inline void setresethaltreq_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_BIT, RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_WIDTH); }
    inline uint32_t setresethaltreq_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_BIT, RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_WIDTH); }
    inline void hartselhi_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_HARTSELHI_BIT, RISCV_DBG_DM_DMCONTROL_HARTSELHI_WIDTH); }
    inline uint32_t hartselhi_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_HARTSELHI_BIT, RISCV_DBG_DM_DMCONTROL_HARTSELHI_WIDTH); }
    inline void hartsello_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_HARTSELLO_BIT, RISCV_DBG_DM_DMCONTROL_HARTSELLO_WIDTH); }
    inline uint32_t hartsello_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_HARTSELLO_BIT, RISCV_DBG_DM_DMCONTROL_HARTSELLO_WIDTH); }
    inline void hasel_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_HASEL_BIT, RISCV_DBG_DM_DMCONTROL_HASEL_WIDTH); }
    inline uint32_t hasel_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_HASEL_BIT, RISCV_DBG_DM_DMCONTROL_HASEL_WIDTH); }
    inline void ackhavereset_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_BIT, RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_WIDTH); }
    inline uint32_t ackhavereset_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_BIT, RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_WIDTH); }
    inline void hartreset_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_HARTRESET_BIT, RISCV_DBG_DM_DMCONTROL_HARTRESET_WIDTH); }
    inline uint32_t hartreset_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_HARTRESET_BIT, RISCV_DBG_DM_DMCONTROL_HARTRESET_WIDTH); }
    inline void resumereq_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_RESUMEREQ_BIT, RISCV_DBG_DM_DMCONTROL_RESUMEREQ_WIDTH); }
    inline uint32_t resumereq_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_RESUMEREQ_BIT, RISCV_DBG_DM_DMCONTROL_RESUMEREQ_WIDTH); }
    inline void haltreq_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCONTROL_HALTREQ_BIT, RISCV_DBG_DM_DMCONTROL_HALTREQ_WIDTH); }
    inline uint32_t haltreq_get() { return this->get_field(RISCV_DBG_DM_DMCONTROL_HALTREQ_BIT, RISCV_DBG_DM_DMCONTROL_HALTREQ_WIDTH); }
    vp_riscv_dbg_dm_dmcontrol()
    {
        this->hw_name = "dmcontrol";
        this->offset = 0x10;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xf7ffffcf;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("dmactive", 0, 1));
        this->regfields.push_back(new vp::regfield("ndmreset", 1, 1));
        this->regfields.push_back(new vp::regfield("clrresethaltreq", 2, 1));
        this->regfields.push_back(new vp::regfield("setresethaltreq", 3, 1));
        this->regfields.push_back(new vp::regfield("hartselhi", 6, 10));
        this->regfields.push_back(new vp::regfield("hartsello", 16, 10));
        this->regfields.push_back(new vp::regfield("hasel", 26, 1));
        this->regfields.push_back(new vp::regfield("ackhavereset", 28, 1));
        this->regfields.push_back(new vp::regfield("hartreset", 29, 1));
        this->regfields.push_back(new vp::regfield("resumereq", 30, 1));
        this->regfields.push_back(new vp::regfield("haltreq", 31, 1));
    }
};

class vp_riscv_dbg_dm_dmstatus : public vp::reg_32
{
public:
    inline void version_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_VERSION_BIT, RISCV_DBG_DM_DMSTATUS_VERSION_WIDTH); }
    inline uint32_t version_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_VERSION_BIT, RISCV_DBG_DM_DMSTATUS_VERSION_WIDTH); }
    inline void confstrptrvalid_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_BIT, RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_WIDTH); }
    inline uint32_t confstrptrvalid_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_BIT, RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_WIDTH); }
    inline void hasresethaltreq_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_BIT, RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_WIDTH); }
    inline uint32_t hasresethaltreq_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_BIT, RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_WIDTH); }
    inline void authbusy_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_AUTHBUSY_BIT, RISCV_DBG_DM_DMSTATUS_AUTHBUSY_WIDTH); }
    inline uint32_t authbusy_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_AUTHBUSY_BIT, RISCV_DBG_DM_DMSTATUS_AUTHBUSY_WIDTH); }
    inline void authenticated_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_BIT, RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_WIDTH); }
    inline uint32_t authenticated_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_BIT, RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_WIDTH); }
    inline void anyhalted_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ANYHALTED_BIT, RISCV_DBG_DM_DMSTATUS_ANYHALTED_WIDTH); }
    inline uint32_t anyhalted_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ANYHALTED_BIT, RISCV_DBG_DM_DMSTATUS_ANYHALTED_WIDTH); }
    inline void allhalted_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ALLHALTED_BIT, RISCV_DBG_DM_DMSTATUS_ALLHALTED_WIDTH); }
    inline uint32_t allhalted_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ALLHALTED_BIT, RISCV_DBG_DM_DMSTATUS_ALLHALTED_WIDTH); }
    inline void anyrunning_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ANYRUNNING_BIT, RISCV_DBG_DM_DMSTATUS_ANYRUNNING_WIDTH); }
    inline uint32_t anyrunning_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ANYRUNNING_BIT, RISCV_DBG_DM_DMSTATUS_ANYRUNNING_WIDTH); }
    inline void allrunning_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ALLRUNNING_BIT, RISCV_DBG_DM_DMSTATUS_ALLRUNNING_WIDTH); }
    inline uint32_t allrunning_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ALLRUNNING_BIT, RISCV_DBG_DM_DMSTATUS_ALLRUNNING_WIDTH); }
    inline void anyunavail_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_BIT, RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_WIDTH); }
    inline uint32_t anyunavail_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_BIT, RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_WIDTH); }
    inline void allunavail_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_BIT, RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_WIDTH); }
    inline uint32_t allunavail_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_BIT, RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_WIDTH); }
    inline void anynonexistent_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_BIT, RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_WIDTH); }
    inline uint32_t anynonexistent_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_BIT, RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_WIDTH); }
    inline void allnonexistent_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_BIT, RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_WIDTH); }
    inline uint32_t allnonexistent_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_BIT, RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_WIDTH); }
    inline void anyresumeack_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_BIT, RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_WIDTH); }
    inline uint32_t anyresumeack_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_BIT, RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_WIDTH); }
    inline void allresumeack_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_BIT, RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_WIDTH); }
    inline uint32_t allresumeack_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_BIT, RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_WIDTH); }
    inline void anyhavereset_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_BIT, RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_WIDTH); }
    inline uint32_t anyhavereset_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_BIT, RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_WIDTH); }
    inline void allhavereset_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_BIT, RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_WIDTH); }
    inline uint32_t allhavereset_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_BIT, RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_WIDTH); }
    inline void impebreak_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMSTATUS_IMPEBREAK_BIT, RISCV_DBG_DM_DMSTATUS_IMPEBREAK_WIDTH); }
    inline uint32_t impebreak_get() { return this->get_field(RISCV_DBG_DM_DMSTATUS_IMPEBREAK_BIT, RISCV_DBG_DM_DMSTATUS_IMPEBREAK_WIDTH); }
    vp_riscv_dbg_dm_dmstatus()
    {
        this->hw_name = "dmstatus";
        this->offset = 0x11;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x0;
        this->reset_val = 0x82;
        this->regfields.push_back(new vp::regfield("version", 0, 4));
        this->regfields.push_back(new vp::regfield("confstrptrvalid", 4, 1));
        this->regfields.push_back(new vp::regfield("hasresethaltreq", 5, 1));
        this->regfields.push_back(new vp::regfield("authbusy", 6, 1));
        this->regfields.push_back(new vp::regfield("authenticated", 7, 1));
        this->regfields.push_back(new vp::regfield("anyhalted", 8, 1));
        this->regfields.push_back(new vp::regfield("allhalted", 9, 1));
        this->regfields.push_back(new vp::regfield("anyrunning", 10, 1));
        this->regfields.push_back(new vp::regfield("allrunning", 11, 1));
        this->regfields.push_back(new vp::regfield("anyunavail", 12, 1));
        this->regfields.push_back(new vp::regfield("allunavail", 13, 1));
        this->regfields.push_back(new vp::regfield("anynonexistent", 14, 1));
        this->regfields.push_back(new vp::regfield("allnonexistent", 15, 1));
        this->regfields.push_back(new vp::regfield("anyresumeack", 16, 1));
        this->regfields.push_back(new vp::regfield("allresumeack", 17, 1));
        this->regfields.push_back(new vp::regfield("anyhavereset", 18, 1));
        this->regfields.push_back(new vp::regfield("allhavereset", 19, 1));
        this->regfields.push_back(new vp::regfield("impebreak", 22, 1));
    }
};

class vp_riscv_dbg_dm_hartinfo : public vp::reg_32
{
public:
    inline void dataaddr_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_HARTINFO_DATAADDR_BIT, RISCV_DBG_DM_HARTINFO_DATAADDR_WIDTH); }
    inline uint32_t dataaddr_get() { return this->get_field(RISCV_DBG_DM_HARTINFO_DATAADDR_BIT, RISCV_DBG_DM_HARTINFO_DATAADDR_WIDTH); }
    inline void datasize_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_HARTINFO_DATASIZE_BIT, RISCV_DBG_DM_HARTINFO_DATASIZE_WIDTH); }
    inline uint32_t datasize_get() { return this->get_field(RISCV_DBG_DM_HARTINFO_DATASIZE_BIT, RISCV_DBG_DM_HARTINFO_DATASIZE_WIDTH); }
    inline void dataaccess_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_HARTINFO_DATAACCESS_BIT, RISCV_DBG_DM_HARTINFO_DATAACCESS_WIDTH); }
    inline uint32_t dataaccess_get() { return this->get_field(RISCV_DBG_DM_HARTINFO_DATAACCESS_BIT, RISCV_DBG_DM_HARTINFO_DATAACCESS_WIDTH); }
    inline void nscratch_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_HARTINFO_NSCRATCH_BIT, RISCV_DBG_DM_HARTINFO_NSCRATCH_WIDTH); }
    inline uint32_t nscratch_get() { return this->get_field(RISCV_DBG_DM_HARTINFO_NSCRATCH_BIT, RISCV_DBG_DM_HARTINFO_NSCRATCH_WIDTH); }
    vp_riscv_dbg_dm_hartinfo()
    {
        this->hw_name = "hartinfo";
        this->offset = 0x12;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x0;
        this->reset_val = 0x212380;
        this->regfields.push_back(new vp::regfield("dataaddr", 0, 12));
        this->regfields.push_back(new vp::regfield("datasize", 12, 4));
        this->regfields.push_back(new vp::regfield("dataaccess", 16, 1));
        this->regfields.push_back(new vp::regfield("nscratch", 20, 4));
    }
};

class vp_riscv_dbg_dm_abstractcs : public vp::reg_32
{
public:
    inline void datacount_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_BIT, RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_WIDTH); }
    inline uint32_t datacount_get() { return this->get_field(RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_BIT, RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_WIDTH); }
    inline void cmderr_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_ABSTRACTCS_CMDERR_BIT, RISCV_DBG_DM_ABSTRACTCS_CMDERR_WIDTH); }
    inline uint32_t cmderr_get() { return this->get_field(RISCV_DBG_DM_ABSTRACTCS_CMDERR_BIT, RISCV_DBG_DM_ABSTRACTCS_CMDERR_WIDTH); }
    inline void busy_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_ABSTRACTCS_BUSY_BIT, RISCV_DBG_DM_ABSTRACTCS_BUSY_WIDTH); }
    inline uint32_t busy_get() { return this->get_field(RISCV_DBG_DM_ABSTRACTCS_BUSY_BIT, RISCV_DBG_DM_ABSTRACTCS_BUSY_WIDTH); }
    inline void progbufsize_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_BIT, RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_WIDTH); }
    inline uint32_t progbufsize_get() { return this->get_field(RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_BIT, RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_WIDTH); }
    vp_riscv_dbg_dm_abstractcs()
    {
        this->hw_name = "abstractcs";
        this->offset = 0x16;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0x700;
        this->reset_val = 0x8000002;
        this->regfields.push_back(new vp::regfield("datacount", 0, 4));
        this->regfields.push_back(new vp::regfield("cmderr", 8, 3));
        this->regfields.push_back(new vp::regfield("busy", 12, 1));
        this->regfields.push_back(new vp::regfield("progbufsize", 24, 5));
    }
};

class vp_riscv_dbg_dm_command : public vp::reg_32
{
public:
    inline void control_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_COMMAND_CONTROL_BIT, RISCV_DBG_DM_COMMAND_CONTROL_WIDTH); }
    inline uint32_t control_get() { return this->get_field(RISCV_DBG_DM_COMMAND_CONTROL_BIT, RISCV_DBG_DM_COMMAND_CONTROL_WIDTH); }
    inline void cmdtype_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_COMMAND_CMDTYPE_BIT, RISCV_DBG_DM_COMMAND_CMDTYPE_WIDTH); }
    inline uint32_t cmdtype_get() { return this->get_field(RISCV_DBG_DM_COMMAND_CMDTYPE_BIT, RISCV_DBG_DM_COMMAND_CMDTYPE_WIDTH); }
    vp_riscv_dbg_dm_command()
    {
        this->hw_name = "command";
        this->offset = 0x17;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("control", 0, 24));
        this->regfields.push_back(new vp::regfield("cmdtype", 24, 8));
    }
};

class vp_riscv_dbg_dm_sbcs : public vp::reg_32
{
public:
    inline void sbaccess8_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBACCESS8_BIT, RISCV_DBG_DM_SBCS_SBACCESS8_WIDTH); }
    inline uint32_t sbaccess8_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBACCESS8_BIT, RISCV_DBG_DM_SBCS_SBACCESS8_WIDTH); }
    inline void sbaccess16_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBACCESS16_BIT, RISCV_DBG_DM_SBCS_SBACCESS16_WIDTH); }
    inline uint32_t sbaccess16_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBACCESS16_BIT, RISCV_DBG_DM_SBCS_SBACCESS16_WIDTH); }
    inline void sbaccess32_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBACCESS32_BIT, RISCV_DBG_DM_SBCS_SBACCESS32_WIDTH); }
    inline uint32_t sbaccess32_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBACCESS32_BIT, RISCV_DBG_DM_SBCS_SBACCESS32_WIDTH); }
    inline void sbaccess64_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBACCESS64_BIT, RISCV_DBG_DM_SBCS_SBACCESS64_WIDTH); }
    inline uint32_t sbaccess64_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBACCESS64_BIT, RISCV_DBG_DM_SBCS_SBACCESS64_WIDTH); }
    inline void sbaccess128_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBACCESS128_BIT, RISCV_DBG_DM_SBCS_SBACCESS128_WIDTH); }
    inline uint32_t sbaccess128_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBACCESS128_BIT, RISCV_DBG_DM_SBCS_SBACCESS128_WIDTH); }
    inline void sbasize_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBASIZE_BIT, RISCV_DBG_DM_SBCS_SBASIZE_WIDTH); }
    inline uint32_t sbasize_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBASIZE_BIT, RISCV_DBG_DM_SBCS_SBASIZE_WIDTH); }
    inline void sberror_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBERROR_BIT, RISCV_DBG_DM_SBCS_SBERROR_WIDTH); }
    inline uint32_t sberror_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBERROR_BIT, RISCV_DBG_DM_SBCS_SBERROR_WIDTH); }
    inline void sbreadondata_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBREADONDATA_BIT, RISCV_DBG_DM_SBCS_SBREADONDATA_WIDTH); }
    inline uint32_t sbreadondata_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBREADONDATA_BIT, RISCV_DBG_DM_SBCS_SBREADONDATA_WIDTH); }
    inline void sbautoincrement_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_BIT, RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_WIDTH); }
    inline uint32_t sbautoincrement_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_BIT, RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_WIDTH); }
    inline void sbaccess_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBACCESS_BIT, RISCV_DBG_DM_SBCS_SBACCESS_WIDTH); }
    inline uint32_t sbaccess_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBACCESS_BIT, RISCV_DBG_DM_SBCS_SBACCESS_WIDTH); }
    inline void sbreadonaddr_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBREADONADDR_BIT, RISCV_DBG_DM_SBCS_SBREADONADDR_WIDTH); }
    inline uint32_t sbreadonaddr_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBREADONADDR_BIT, RISCV_DBG_DM_SBCS_SBREADONADDR_WIDTH); }
    inline void sbbusy_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBBUSY_BIT, RISCV_DBG_DM_SBCS_SBBUSY_WIDTH); }
    inline uint32_t sbbusy_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBBUSY_BIT, RISCV_DBG_DM_SBCS_SBBUSY_WIDTH); }
    inline void sbbusyerror_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBBUSYERROR_BIT, RISCV_DBG_DM_SBCS_SBBUSYERROR_WIDTH); }
    inline uint32_t sbbusyerror_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBBUSYERROR_BIT, RISCV_DBG_DM_SBCS_SBBUSYERROR_WIDTH); }
    inline void sbversion_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBCS_SBVERSION_BIT, RISCV_DBG_DM_SBCS_SBVERSION_WIDTH); }
    inline uint32_t sbversion_get() { return this->get_field(RISCV_DBG_DM_SBCS_SBVERSION_BIT, RISCV_DBG_DM_SBCS_SBVERSION_WIDTH); }
    vp_riscv_dbg_dm_sbcs()
    {
        this->hw_name = "sbcs";
        this->offset = 0x38;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xfdff000;
        this->reset_val = 0x20040404;
        this->regfields.push_back(new vp::regfield("sbaccess8", 0, 1));
        this->regfields.push_back(new vp::regfield("sbaccess16", 1, 1));
        this->regfields.push_back(new vp::regfield("sbaccess32", 2, 1));
        this->regfields.push_back(new vp::regfield("sbaccess64", 3, 1));
        this->regfields.push_back(new vp::regfield("sbaccess128", 4, 1));
        this->regfields.push_back(new vp::regfield("sbasize", 5, 7));
        this->regfields.push_back(new vp::regfield("sberror", 12, 3));
        this->regfields.push_back(new vp::regfield("sbreadondata", 15, 1));
        this->regfields.push_back(new vp::regfield("sbautoincrement", 16, 1));
        this->regfields.push_back(new vp::regfield("sbaccess", 17, 3));
        this->regfields.push_back(new vp::regfield("sbreadonaddr", 20, 1));
        this->regfields.push_back(new vp::regfield("sbbusy", 21, 1));
        this->regfields.push_back(new vp::regfield("sbbusyerror", 22, 6));
        this->regfields.push_back(new vp::regfield("sbversion", 29, 3));
    }
};

class vp_riscv_dbg_dm_progbuf0 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF0_DATA_BIT, RISCV_DBG_DM_PROGBUF0_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF0_DATA_BIT, RISCV_DBG_DM_PROGBUF0_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf0()
    {
        this->hw_name = "progbuf0";
        this->offset = 0x20;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf1 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF1_DATA_BIT, RISCV_DBG_DM_PROGBUF1_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF1_DATA_BIT, RISCV_DBG_DM_PROGBUF1_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf1()
    {
        this->hw_name = "progbuf1";
        this->offset = 0x21;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf2 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF2_DATA_BIT, RISCV_DBG_DM_PROGBUF2_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF2_DATA_BIT, RISCV_DBG_DM_PROGBUF2_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf2()
    {
        this->hw_name = "progbuf2";
        this->offset = 0x22;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf3 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF3_DATA_BIT, RISCV_DBG_DM_PROGBUF3_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF3_DATA_BIT, RISCV_DBG_DM_PROGBUF3_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf3()
    {
        this->hw_name = "progbuf3";
        this->offset = 0x23;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf4 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF4_DATA_BIT, RISCV_DBG_DM_PROGBUF4_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF4_DATA_BIT, RISCV_DBG_DM_PROGBUF4_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf4()
    {
        this->hw_name = "progbuf4";
        this->offset = 0x24;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf5 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF5_DATA_BIT, RISCV_DBG_DM_PROGBUF5_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF5_DATA_BIT, RISCV_DBG_DM_PROGBUF5_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf5()
    {
        this->hw_name = "progbuf5";
        this->offset = 0x25;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf6 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF6_DATA_BIT, RISCV_DBG_DM_PROGBUF6_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF6_DATA_BIT, RISCV_DBG_DM_PROGBUF6_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf6()
    {
        this->hw_name = "progbuf6";
        this->offset = 0x26;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_progbuf7 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_PROGBUF7_DATA_BIT, RISCV_DBG_DM_PROGBUF7_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_PROGBUF7_DATA_BIT, RISCV_DBG_DM_PROGBUF7_DATA_WIDTH); }
    vp_riscv_dbg_dm_progbuf7()
    {
        this->hw_name = "progbuf7";
        this->offset = 0x27;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};

class vp_riscv_dbg_dm_sbaddress0 : public vp::reg_32
{
public:
    inline void address_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBADDRESS0_ADDRESS_BIT, RISCV_DBG_DM_SBADDRESS0_ADDRESS_WIDTH); }
    inline uint32_t address_get() { return this->get_field(RISCV_DBG_DM_SBADDRESS0_ADDRESS_BIT, RISCV_DBG_DM_SBADDRESS0_ADDRESS_WIDTH); }
    vp_riscv_dbg_dm_sbaddress0()
    {
        this->hw_name = "sbaddress0";
        this->offset = 0x39;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("address", 0, 32));
    }
};

class vp_riscv_dbg_dm_dmcs2 : public vp::reg_32
{
public:
    inline void hgselect_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCS2_HGSELECT_BIT, RISCV_DBG_DM_DMCS2_HGSELECT_WIDTH); }
    inline uint32_t hgselect_get() { return this->get_field(RISCV_DBG_DM_DMCS2_HGSELECT_BIT, RISCV_DBG_DM_DMCS2_HGSELECT_WIDTH); }
    inline void hgwrite_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCS2_HGWRITE_BIT, RISCV_DBG_DM_DMCS2_HGWRITE_WIDTH); }
    inline uint32_t hgwrite_get() { return this->get_field(RISCV_DBG_DM_DMCS2_HGWRITE_BIT, RISCV_DBG_DM_DMCS2_HGWRITE_WIDTH); }
    inline void group_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCS2_GROUP_BIT, RISCV_DBG_DM_DMCS2_GROUP_WIDTH); }
    inline uint32_t group_get() { return this->get_field(RISCV_DBG_DM_DMCS2_GROUP_BIT, RISCV_DBG_DM_DMCS2_GROUP_WIDTH); }
    inline void dmexttrigger_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_BIT, RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_WIDTH); }
    inline uint32_t dmexttrigger_get() { return this->get_field(RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_BIT, RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_WIDTH); }
    inline void grouptype_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_DMCS2_GROUPTYPE_BIT, RISCV_DBG_DM_DMCS2_GROUPTYPE_WIDTH); }
    inline uint32_t grouptype_get() { return this->get_field(RISCV_DBG_DM_DMCS2_GROUPTYPE_BIT, RISCV_DBG_DM_DMCS2_GROUPTYPE_WIDTH); }
    vp_riscv_dbg_dm_dmcs2()
    {
        this->hw_name = "dmcs2";
        this->offset = 0x32;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xfff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("hgselect", 0, 1));
        this->regfields.push_back(new vp::regfield("hgwrite", 1, 1));
        this->regfields.push_back(new vp::regfield("group", 2, 5));
        this->regfields.push_back(new vp::regfield("dmexttrigger", 7, 4));
        this->regfields.push_back(new vp::regfield("grouptype", 11, 1));
    }
};

class vp_riscv_dbg_dm_sbdata0 : public vp::reg_32
{
public:
    inline void data_set(uint32_t value) { this->set_field(value, RISCV_DBG_DM_SBDATA0_DATA_BIT, RISCV_DBG_DM_SBDATA0_DATA_WIDTH); }
    inline uint32_t data_get() { return this->get_field(RISCV_DBG_DM_SBDATA0_DATA_BIT, RISCV_DBG_DM_SBDATA0_DATA_WIDTH); }
    vp_riscv_dbg_dm_sbdata0()
    {
        this->hw_name = "sbdata0";
        this->offset = 0x3c;
        this->width = 32;
        this->do_reset = 1;
        this->write_mask = 0xffffffff;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("data", 0, 32));
    }
};


class vp_regmap_riscv_dbg_dm : public vp::regmap
{
public:
    vp_riscv_dbg_dm_data0 data0;
    vp_riscv_dbg_dm_data1 data1;
    vp_riscv_dbg_dm_dmcontrol dmcontrol;
    vp_riscv_dbg_dm_dmstatus dmstatus;
    vp_riscv_dbg_dm_hartinfo hartinfo;
    vp_riscv_dbg_dm_abstractcs abstractcs;
    vp_riscv_dbg_dm_command command;
    vp_riscv_dbg_dm_sbcs sbcs;
    vp_riscv_dbg_dm_progbuf0 progbuf0;
    vp_riscv_dbg_dm_progbuf1 progbuf1;
    vp_riscv_dbg_dm_progbuf2 progbuf2;
    vp_riscv_dbg_dm_progbuf3 progbuf3;
    vp_riscv_dbg_dm_progbuf4 progbuf4;
    vp_riscv_dbg_dm_progbuf5 progbuf5;
    vp_riscv_dbg_dm_progbuf6 progbuf6;
    vp_riscv_dbg_dm_progbuf7 progbuf7;
    vp_riscv_dbg_dm_sbaddress0 sbaddress0;
    vp_riscv_dbg_dm_dmcs2 dmcs2;
    vp_riscv_dbg_dm_sbdata0 sbdata0;
    vp_regmap_riscv_dbg_dm()
    {
        this->registers.push_back(&this->data0);
        this->registers.push_back(&this->data1);
        this->registers.push_back(&this->dmcontrol);
        this->registers.push_back(&this->dmstatus);
        this->registers.push_back(&this->hartinfo);
        this->registers.push_back(&this->abstractcs);
        this->registers.push_back(&this->command);
        this->registers.push_back(&this->sbcs);
        this->registers.push_back(&this->progbuf0);
        this->registers.push_back(&this->progbuf1);
        this->registers.push_back(&this->progbuf2);
        this->registers.push_back(&this->progbuf3);
        this->registers.push_back(&this->progbuf4);
        this->registers.push_back(&this->progbuf5);
        this->registers.push_back(&this->progbuf6);
        this->registers.push_back(&this->progbuf7);
        this->registers.push_back(&this->sbaddress0);
        this->registers.push_back(&this->dmcs2);
        this->registers.push_back(&this->sbdata0);
    }
};

#endif

#endif
