
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

#ifndef __ARCHI_GPIO_GVSOC__
#define __ARCHI_GPIO_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_gpio_paddir_00_31 : public vp::reg_32
{
public:
    inline void paddir_set(uint32_t value) { this->set_field(value, GPIO_PADDIR_00_31_PADDIR_BIT, GPIO_PADDIR_00_31_PADDIR_WIDTH); }
    inline uint32_t paddir_get() { return this->get_field(GPIO_PADDIR_00_31_PADDIR_BIT, GPIO_PADDIR_00_31_PADDIR_WIDTH); }
    vp_gpio_paddir_00_31()
    {
        this->hw_name = "PADDIR_00_31";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADDIR", 0, 32));
    }
};

class vp_gpio_gpioen_00_31 : public vp::reg_32
{
public:
    inline void gpioen_set(uint32_t value) { this->set_field(value, GPIO_GPIOEN_00_31_GPIOEN_BIT, GPIO_GPIOEN_00_31_GPIOEN_WIDTH); }
    inline uint32_t gpioen_get() { return this->get_field(GPIO_GPIOEN_00_31_GPIOEN_BIT, GPIO_GPIOEN_00_31_GPIOEN_WIDTH); }
    vp_gpio_gpioen_00_31()
    {
        this->hw_name = "GPIOEN_00_31";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("GPIOEN", 0, 32));
    }
};

class vp_gpio_padin_00_31 : public vp::reg_32
{
public:
    inline void padin_set(uint32_t value) { this->set_field(value, GPIO_PADIN_00_31_PADIN_BIT, GPIO_PADIN_00_31_PADIN_WIDTH); }
    inline uint32_t padin_get() { return this->get_field(GPIO_PADIN_00_31_PADIN_BIT, GPIO_PADIN_00_31_PADIN_WIDTH); }
    vp_gpio_padin_00_31()
    {
        this->hw_name = "PADIN_00_31";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADIN", 0, 32));
    }
};

class vp_gpio_padout_00_31 : public vp::reg_32
{
public:
    inline void padout_set(uint32_t value) { this->set_field(value, GPIO_PADOUT_00_31_PADOUT_BIT, GPIO_PADOUT_00_31_PADOUT_WIDTH); }
    inline uint32_t padout_get() { return this->get_field(GPIO_PADOUT_00_31_PADOUT_BIT, GPIO_PADOUT_00_31_PADOUT_WIDTH); }
    vp_gpio_padout_00_31()
    {
        this->hw_name = "PADOUT_00_31";
        this->offset = 0xc;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUT", 0, 32));
    }
};

class vp_gpio_padoutset_00_31 : public vp::reg_32
{
public:
    inline void padoutset_set(uint32_t value) { this->set_field(value, GPIO_PADOUTSET_00_31_PADOUTSET_BIT, GPIO_PADOUTSET_00_31_PADOUTSET_WIDTH); }
    inline uint32_t padoutset_get() { return this->get_field(GPIO_PADOUTSET_00_31_PADOUTSET_BIT, GPIO_PADOUTSET_00_31_PADOUTSET_WIDTH); }
    vp_gpio_padoutset_00_31()
    {
        this->hw_name = "PADOUTSET_00_31";
        this->offset = 0x10;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUTSET", 0, 32));
    }
};

class vp_gpio_padoutclr_00_31 : public vp::reg_32
{
public:
    inline void padoutclr_set(uint32_t value) { this->set_field(value, GPIO_PADOUTCLR_00_31_PADOUTCLR_BIT, GPIO_PADOUTCLR_00_31_PADOUTCLR_WIDTH); }
    inline uint32_t padoutclr_get() { return this->get_field(GPIO_PADOUTCLR_00_31_PADOUTCLR_BIT, GPIO_PADOUTCLR_00_31_PADOUTCLR_WIDTH); }
    vp_gpio_padoutclr_00_31()
    {
        this->hw_name = "PADOUTCLR_00_31";
        this->offset = 0x14;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUTCLR", 0, 32));
    }
};

class vp_gpio_inten_00_31 : public vp::reg_32
{
public:
    inline void inten_set(uint32_t value) { this->set_field(value, GPIO_INTEN_00_31_INTEN_BIT, GPIO_INTEN_00_31_INTEN_WIDTH); }
    inline uint32_t inten_get() { return this->get_field(GPIO_INTEN_00_31_INTEN_BIT, GPIO_INTEN_00_31_INTEN_WIDTH); }
    vp_gpio_inten_00_31()
    {
        this->hw_name = "INTEN_00_31";
        this->offset = 0x18;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTEN", 0, 32));
    }
};

class vp_gpio_inttype_00_15 : public vp::reg_32
{
public:
    inline void inttype_set(uint32_t value) { this->set_field(value, GPIO_INTTYPE_00_15_INTTYPE_BIT, GPIO_INTTYPE_00_15_INTTYPE_WIDTH); }
    inline uint32_t inttype_get() { return this->get_field(GPIO_INTTYPE_00_15_INTTYPE_BIT, GPIO_INTTYPE_00_15_INTTYPE_WIDTH); }
    vp_gpio_inttype_00_15()
    {
        this->hw_name = "INTTYPE_00_15";
        this->offset = 0x1c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTTYPE", 0, 32));
    }
};

class vp_gpio_inttype_16_31 : public vp::reg_32
{
public:
    inline void inttype_set(uint32_t value) { this->set_field(value, GPIO_INTTYPE_16_31_INTTYPE_BIT, GPIO_INTTYPE_16_31_INTTYPE_WIDTH); }
    inline uint32_t inttype_get() { return this->get_field(GPIO_INTTYPE_16_31_INTTYPE_BIT, GPIO_INTTYPE_16_31_INTTYPE_WIDTH); }
    vp_gpio_inttype_16_31()
    {
        this->hw_name = "INTTYPE_16_31";
        this->offset = 0x20;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTTYPE", 0, 32));
    }
};

class vp_gpio_intstatus_00_31 : public vp::reg_32
{
public:
    inline void intstatus_set(uint32_t value) { this->set_field(value, GPIO_INTSTATUS_00_31_INTSTATUS_BIT, GPIO_INTSTATUS_00_31_INTSTATUS_WIDTH); }
    inline uint32_t intstatus_get() { return this->get_field(GPIO_INTSTATUS_00_31_INTSTATUS_BIT, GPIO_INTSTATUS_00_31_INTSTATUS_WIDTH); }
    vp_gpio_intstatus_00_31()
    {
        this->hw_name = "INTSTATUS_00_31";
        this->offset = 0x24;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTSTATUS", 0, 32));
    }
};

class vp_gpio_padcfg_00_03 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_00_03_PADCFG_BIT, GPIO_PADCFG_00_03_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_00_03_PADCFG_BIT, GPIO_PADCFG_00_03_PADCFG_WIDTH); }
    vp_gpio_padcfg_00_03()
    {
        this->hw_name = "PADCFG_00_03";
        this->offset = 0x28;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_04_07 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_04_07_PADCFG_BIT, GPIO_PADCFG_04_07_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_04_07_PADCFG_BIT, GPIO_PADCFG_04_07_PADCFG_WIDTH); }
    vp_gpio_padcfg_04_07()
    {
        this->hw_name = "PADCFG_04_07";
        this->offset = 0x2c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_08_11 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_08_11_PADCFG_BIT, GPIO_PADCFG_08_11_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_08_11_PADCFG_BIT, GPIO_PADCFG_08_11_PADCFG_WIDTH); }
    vp_gpio_padcfg_08_11()
    {
        this->hw_name = "PADCFG_08_11";
        this->offset = 0x30;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_12_15 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_12_15_PADCFG_BIT, GPIO_PADCFG_12_15_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_12_15_PADCFG_BIT, GPIO_PADCFG_12_15_PADCFG_WIDTH); }
    vp_gpio_padcfg_12_15()
    {
        this->hw_name = "PADCFG_12_15";
        this->offset = 0x34;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_16_19 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_16_19_PADCFG_BIT, GPIO_PADCFG_16_19_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_16_19_PADCFG_BIT, GPIO_PADCFG_16_19_PADCFG_WIDTH); }
    vp_gpio_padcfg_16_19()
    {
        this->hw_name = "PADCFG_16_19";
        this->offset = 0x38;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_20_23 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_20_23_PADCFG_BIT, GPIO_PADCFG_20_23_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_20_23_PADCFG_BIT, GPIO_PADCFG_20_23_PADCFG_WIDTH); }
    vp_gpio_padcfg_20_23()
    {
        this->hw_name = "PADCFG_20_23";
        this->offset = 0x3c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_24_27 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_24_27_PADCFG_BIT, GPIO_PADCFG_24_27_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_24_27_PADCFG_BIT, GPIO_PADCFG_24_27_PADCFG_WIDTH); }
    vp_gpio_padcfg_24_27()
    {
        this->hw_name = "PADCFG_24_27";
        this->offset = 0x40;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_28_31 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_28_31_PADCFG_BIT, GPIO_PADCFG_28_31_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_28_31_PADCFG_BIT, GPIO_PADCFG_28_31_PADCFG_WIDTH); }
    vp_gpio_padcfg_28_31()
    {
        this->hw_name = "PADCFG_28_31";
        this->offset = 0x44;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_paddir_32_63 : public vp::reg_32
{
public:
    inline void paddir_set(uint32_t value) { this->set_field(value, GPIO_PADDIR_32_63_PADDIR_BIT, GPIO_PADDIR_32_63_PADDIR_WIDTH); }
    inline uint32_t paddir_get() { return this->get_field(GPIO_PADDIR_32_63_PADDIR_BIT, GPIO_PADDIR_32_63_PADDIR_WIDTH); }
    vp_gpio_paddir_32_63()
    {
        this->hw_name = "PADDIR_32_63";
        this->offset = 0x48;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADDIR", 0, 32));
    }
};

class vp_gpio_gpioen_32_63 : public vp::reg_32
{
public:
    inline void gpioen_set(uint32_t value) { this->set_field(value, GPIO_GPIOEN_32_63_GPIOEN_BIT, GPIO_GPIOEN_32_63_GPIOEN_WIDTH); }
    inline uint32_t gpioen_get() { return this->get_field(GPIO_GPIOEN_32_63_GPIOEN_BIT, GPIO_GPIOEN_32_63_GPIOEN_WIDTH); }
    vp_gpio_gpioen_32_63()
    {
        this->hw_name = "GPIOEN_32_63";
        this->offset = 0x4c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("GPIOEN", 0, 32));
    }
};

class vp_gpio_padin_32_63 : public vp::reg_32
{
public:
    inline void padin_set(uint32_t value) { this->set_field(value, GPIO_PADIN_32_63_PADIN_BIT, GPIO_PADIN_32_63_PADIN_WIDTH); }
    inline uint32_t padin_get() { return this->get_field(GPIO_PADIN_32_63_PADIN_BIT, GPIO_PADIN_32_63_PADIN_WIDTH); }
    vp_gpio_padin_32_63()
    {
        this->hw_name = "PADIN_32_63";
        this->offset = 0x50;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADIN", 0, 32));
    }
};

class vp_gpio_padout_32_63 : public vp::reg_32
{
public:
    inline void padout_set(uint32_t value) { this->set_field(value, GPIO_PADOUT_32_63_PADOUT_BIT, GPIO_PADOUT_32_63_PADOUT_WIDTH); }
    inline uint32_t padout_get() { return this->get_field(GPIO_PADOUT_32_63_PADOUT_BIT, GPIO_PADOUT_32_63_PADOUT_WIDTH); }
    vp_gpio_padout_32_63()
    {
        this->hw_name = "PADOUT_32_63";
        this->offset = 0x54;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUT", 0, 32));
    }
};

class vp_gpio_padoutset_32_63 : public vp::reg_32
{
public:
    inline void padoutset_set(uint32_t value) { this->set_field(value, GPIO_PADOUTSET_32_63_PADOUTSET_BIT, GPIO_PADOUTSET_32_63_PADOUTSET_WIDTH); }
    inline uint32_t padoutset_get() { return this->get_field(GPIO_PADOUTSET_32_63_PADOUTSET_BIT, GPIO_PADOUTSET_32_63_PADOUTSET_WIDTH); }
    vp_gpio_padoutset_32_63()
    {
        this->hw_name = "PADOUTSET_32_63";
        this->offset = 0x58;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUTSET", 0, 32));
    }
};

class vp_gpio_padoutclr_32_63 : public vp::reg_32
{
public:
    inline void padoutclr_set(uint32_t value) { this->set_field(value, GPIO_PADOUTCLR_32_63_PADOUTCLR_BIT, GPIO_PADOUTCLR_32_63_PADOUTCLR_WIDTH); }
    inline uint32_t padoutclr_get() { return this->get_field(GPIO_PADOUTCLR_32_63_PADOUTCLR_BIT, GPIO_PADOUTCLR_32_63_PADOUTCLR_WIDTH); }
    vp_gpio_padoutclr_32_63()
    {
        this->hw_name = "PADOUTCLR_32_63";
        this->offset = 0x5c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUTCLR", 0, 32));
    }
};

class vp_gpio_inten_32_63 : public vp::reg_32
{
public:
    inline void inten_set(uint32_t value) { this->set_field(value, GPIO_INTEN_32_63_INTEN_BIT, GPIO_INTEN_32_63_INTEN_WIDTH); }
    inline uint32_t inten_get() { return this->get_field(GPIO_INTEN_32_63_INTEN_BIT, GPIO_INTEN_32_63_INTEN_WIDTH); }
    vp_gpio_inten_32_63()
    {
        this->hw_name = "INTEN_32_63";
        this->offset = 0x60;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTEN", 0, 32));
    }
};

class vp_gpio_inttype_32_47 : public vp::reg_32
{
public:
    inline void inttype_set(uint32_t value) { this->set_field(value, GPIO_INTTYPE_32_47_INTTYPE_BIT, GPIO_INTTYPE_32_47_INTTYPE_WIDTH); }
    inline uint32_t inttype_get() { return this->get_field(GPIO_INTTYPE_32_47_INTTYPE_BIT, GPIO_INTTYPE_32_47_INTTYPE_WIDTH); }
    vp_gpio_inttype_32_47()
    {
        this->hw_name = "INTTYPE_32_47";
        this->offset = 0x64;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTTYPE", 0, 32));
    }
};

class vp_gpio_inttype_48_63 : public vp::reg_32
{
public:
    inline void inttype_set(uint32_t value) { this->set_field(value, GPIO_INTTYPE_48_63_INTTYPE_BIT, GPIO_INTTYPE_48_63_INTTYPE_WIDTH); }
    inline uint32_t inttype_get() { return this->get_field(GPIO_INTTYPE_48_63_INTTYPE_BIT, GPIO_INTTYPE_48_63_INTTYPE_WIDTH); }
    vp_gpio_inttype_48_63()
    {
        this->hw_name = "INTTYPE_48_63";
        this->offset = 0x68;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTTYPE", 0, 32));
    }
};

class vp_gpio_intstatus_32_63 : public vp::reg_32
{
public:
    inline void intstatus_set(uint32_t value) { this->set_field(value, GPIO_INTSTATUS_32_63_INTSTATUS_BIT, GPIO_INTSTATUS_32_63_INTSTATUS_WIDTH); }
    inline uint32_t intstatus_get() { return this->get_field(GPIO_INTSTATUS_32_63_INTSTATUS_BIT, GPIO_INTSTATUS_32_63_INTSTATUS_WIDTH); }
    vp_gpio_intstatus_32_63()
    {
        this->hw_name = "INTSTATUS_32_63";
        this->offset = 0x6c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTSTATUS", 0, 32));
    }
};

class vp_gpio_padcfg_32_35 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_32_35_PADCFG_BIT, GPIO_PADCFG_32_35_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_32_35_PADCFG_BIT, GPIO_PADCFG_32_35_PADCFG_WIDTH); }
    vp_gpio_padcfg_32_35()
    {
        this->hw_name = "PADCFG_32_35";
        this->offset = 0x70;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_36_39 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_36_39_PADCFG_BIT, GPIO_PADCFG_36_39_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_36_39_PADCFG_BIT, GPIO_PADCFG_36_39_PADCFG_WIDTH); }
    vp_gpio_padcfg_36_39()
    {
        this->hw_name = "PADCFG_36_39";
        this->offset = 0x74;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_40_43 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_40_43_PADCFG_BIT, GPIO_PADCFG_40_43_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_40_43_PADCFG_BIT, GPIO_PADCFG_40_43_PADCFG_WIDTH); }
    vp_gpio_padcfg_40_43()
    {
        this->hw_name = "PADCFG_40_43";
        this->offset = 0x78;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_44_47 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_44_47_PADCFG_BIT, GPIO_PADCFG_44_47_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_44_47_PADCFG_BIT, GPIO_PADCFG_44_47_PADCFG_WIDTH); }
    vp_gpio_padcfg_44_47()
    {
        this->hw_name = "PADCFG_44_47";
        this->offset = 0x7c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_48_51 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_48_51_PADCFG_BIT, GPIO_PADCFG_48_51_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_48_51_PADCFG_BIT, GPIO_PADCFG_48_51_PADCFG_WIDTH); }
    vp_gpio_padcfg_48_51()
    {
        this->hw_name = "PADCFG_48_51";
        this->offset = 0x80;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_52_55 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_52_55_PADCFG_BIT, GPIO_PADCFG_52_55_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_52_55_PADCFG_BIT, GPIO_PADCFG_52_55_PADCFG_WIDTH); }
    vp_gpio_padcfg_52_55()
    {
        this->hw_name = "PADCFG_52_55";
        this->offset = 0x84;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_56_59 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_56_59_PADCFG_BIT, GPIO_PADCFG_56_59_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_56_59_PADCFG_BIT, GPIO_PADCFG_56_59_PADCFG_WIDTH); }
    vp_gpio_padcfg_56_59()
    {
        this->hw_name = "PADCFG_56_59";
        this->offset = 0x88;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_60_63 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_60_63_PADCFG_BIT, GPIO_PADCFG_60_63_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_60_63_PADCFG_BIT, GPIO_PADCFG_60_63_PADCFG_WIDTH); }
    vp_gpio_padcfg_60_63()
    {
        this->hw_name = "PADCFG_60_63";
        this->offset = 0x8c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_paddir_64_95 : public vp::reg_32
{
public:
    inline void paddir_set(uint32_t value) { this->set_field(value, GPIO_PADDIR_64_95_PADDIR_BIT, GPIO_PADDIR_64_95_PADDIR_WIDTH); }
    inline uint32_t paddir_get() { return this->get_field(GPIO_PADDIR_64_95_PADDIR_BIT, GPIO_PADDIR_64_95_PADDIR_WIDTH); }
    vp_gpio_paddir_64_95()
    {
        this->hw_name = "PADDIR_64_95";
        this->offset = 0x90;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADDIR", 0, 32));
    }
};

class vp_gpio_gpioen_64_95 : public vp::reg_32
{
public:
    inline void gpioen_set(uint32_t value) { this->set_field(value, GPIO_GPIOEN_64_95_GPIOEN_BIT, GPIO_GPIOEN_64_95_GPIOEN_WIDTH); }
    inline uint32_t gpioen_get() { return this->get_field(GPIO_GPIOEN_64_95_GPIOEN_BIT, GPIO_GPIOEN_64_95_GPIOEN_WIDTH); }
    vp_gpio_gpioen_64_95()
    {
        this->hw_name = "GPIOEN_64_95";
        this->offset = 0x94;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("GPIOEN", 0, 32));
    }
};

class vp_gpio_padin_64_95 : public vp::reg_32
{
public:
    inline void padin_set(uint32_t value) { this->set_field(value, GPIO_PADIN_64_95_PADIN_BIT, GPIO_PADIN_64_95_PADIN_WIDTH); }
    inline uint32_t padin_get() { return this->get_field(GPIO_PADIN_64_95_PADIN_BIT, GPIO_PADIN_64_95_PADIN_WIDTH); }
    vp_gpio_padin_64_95()
    {
        this->hw_name = "PADIN_64_95";
        this->offset = 0x98;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADIN", 0, 32));
    }
};

class vp_gpio_padout_64_95 : public vp::reg_32
{
public:
    inline void padout_set(uint32_t value) { this->set_field(value, GPIO_PADOUT_64_95_PADOUT_BIT, GPIO_PADOUT_64_95_PADOUT_WIDTH); }
    inline uint32_t padout_get() { return this->get_field(GPIO_PADOUT_64_95_PADOUT_BIT, GPIO_PADOUT_64_95_PADOUT_WIDTH); }
    vp_gpio_padout_64_95()
    {
        this->hw_name = "PADOUT_64_95";
        this->offset = 0x9c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUT", 0, 32));
    }
};

class vp_gpio_padoutset_64_95 : public vp::reg_32
{
public:
    inline void padoutset_set(uint32_t value) { this->set_field(value, GPIO_PADOUTSET_64_95_PADOUTSET_BIT, GPIO_PADOUTSET_64_95_PADOUTSET_WIDTH); }
    inline uint32_t padoutset_get() { return this->get_field(GPIO_PADOUTSET_64_95_PADOUTSET_BIT, GPIO_PADOUTSET_64_95_PADOUTSET_WIDTH); }
    vp_gpio_padoutset_64_95()
    {
        this->hw_name = "PADOUTSET_64_95";
        this->offset = 0xa0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUTSET", 0, 32));
    }
};

class vp_gpio_padoutclr_64_95 : public vp::reg_32
{
public:
    inline void padoutclr_set(uint32_t value) { this->set_field(value, GPIO_PADOUTCLR_64_95_PADOUTCLR_BIT, GPIO_PADOUTCLR_64_95_PADOUTCLR_WIDTH); }
    inline uint32_t padoutclr_get() { return this->get_field(GPIO_PADOUTCLR_64_95_PADOUTCLR_BIT, GPIO_PADOUTCLR_64_95_PADOUTCLR_WIDTH); }
    vp_gpio_padoutclr_64_95()
    {
        this->hw_name = "PADOUTCLR_64_95";
        this->offset = 0xa4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADOUTCLR", 0, 32));
    }
};

class vp_gpio_inten_64_95 : public vp::reg_32
{
public:
    inline void inten_set(uint32_t value) { this->set_field(value, GPIO_INTEN_64_95_INTEN_BIT, GPIO_INTEN_64_95_INTEN_WIDTH); }
    inline uint32_t inten_get() { return this->get_field(GPIO_INTEN_64_95_INTEN_BIT, GPIO_INTEN_64_95_INTEN_WIDTH); }
    vp_gpio_inten_64_95()
    {
        this->hw_name = "INTEN_64_95";
        this->offset = 0xa8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTEN", 0, 32));
    }
};

class vp_gpio_inttype_64_79 : public vp::reg_32
{
public:
    inline void inttype_set(uint32_t value) { this->set_field(value, GPIO_INTTYPE_64_79_INTTYPE_BIT, GPIO_INTTYPE_64_79_INTTYPE_WIDTH); }
    inline uint32_t inttype_get() { return this->get_field(GPIO_INTTYPE_64_79_INTTYPE_BIT, GPIO_INTTYPE_64_79_INTTYPE_WIDTH); }
    vp_gpio_inttype_64_79()
    {
        this->hw_name = "INTTYPE_64_79";
        this->offset = 0xac;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTTYPE", 0, 32));
    }
};

class vp_gpio_inttype_80_95 : public vp::reg_32
{
public:
    inline void inttype_set(uint32_t value) { this->set_field(value, GPIO_INTTYPE_80_95_INTTYPE_BIT, GPIO_INTTYPE_80_95_INTTYPE_WIDTH); }
    inline uint32_t inttype_get() { return this->get_field(GPIO_INTTYPE_80_95_INTTYPE_BIT, GPIO_INTTYPE_80_95_INTTYPE_WIDTH); }
    vp_gpio_inttype_80_95()
    {
        this->hw_name = "INTTYPE_80_95";
        this->offset = 0xb0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTTYPE", 0, 32));
    }
};

class vp_gpio_intstatus_64_95 : public vp::reg_32
{
public:
    inline void intstatus_set(uint32_t value) { this->set_field(value, GPIO_INTSTATUS_64_95_INTSTATUS_BIT, GPIO_INTSTATUS_64_95_INTSTATUS_WIDTH); }
    inline uint32_t intstatus_get() { return this->get_field(GPIO_INTSTATUS_64_95_INTSTATUS_BIT, GPIO_INTSTATUS_64_95_INTSTATUS_WIDTH); }
    vp_gpio_intstatus_64_95()
    {
        this->hw_name = "INTSTATUS_64_95";
        this->offset = 0xb4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("INTSTATUS", 0, 32));
    }
};

class vp_gpio_padcfg_64_67 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_64_67_PADCFG_BIT, GPIO_PADCFG_64_67_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_64_67_PADCFG_BIT, GPIO_PADCFG_64_67_PADCFG_WIDTH); }
    vp_gpio_padcfg_64_67()
    {
        this->hw_name = "PADCFG_64_67";
        this->offset = 0xb8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_68_71 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_68_71_PADCFG_BIT, GPIO_PADCFG_68_71_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_68_71_PADCFG_BIT, GPIO_PADCFG_68_71_PADCFG_WIDTH); }
    vp_gpio_padcfg_68_71()
    {
        this->hw_name = "PADCFG_68_71";
        this->offset = 0xbc;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_72_75 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_72_75_PADCFG_BIT, GPIO_PADCFG_72_75_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_72_75_PADCFG_BIT, GPIO_PADCFG_72_75_PADCFG_WIDTH); }
    vp_gpio_padcfg_72_75()
    {
        this->hw_name = "PADCFG_72_75";
        this->offset = 0xc0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_76_79 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_76_79_PADCFG_BIT, GPIO_PADCFG_76_79_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_76_79_PADCFG_BIT, GPIO_PADCFG_76_79_PADCFG_WIDTH); }
    vp_gpio_padcfg_76_79()
    {
        this->hw_name = "PADCFG_76_79";
        this->offset = 0xc4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_80_83 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_80_83_PADCFG_BIT, GPIO_PADCFG_80_83_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_80_83_PADCFG_BIT, GPIO_PADCFG_80_83_PADCFG_WIDTH); }
    vp_gpio_padcfg_80_83()
    {
        this->hw_name = "PADCFG_80_83";
        this->offset = 0xc8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_84_87 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_84_87_PADCFG_BIT, GPIO_PADCFG_84_87_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_84_87_PADCFG_BIT, GPIO_PADCFG_84_87_PADCFG_WIDTH); }
    vp_gpio_padcfg_84_87()
    {
        this->hw_name = "PADCFG_84_87";
        this->offset = 0xcc;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_88_91 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_88_91_PADCFG_BIT, GPIO_PADCFG_88_91_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_88_91_PADCFG_BIT, GPIO_PADCFG_88_91_PADCFG_WIDTH); }
    vp_gpio_padcfg_88_91()
    {
        this->hw_name = "PADCFG_88_91";
        this->offset = 0xd0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};

class vp_gpio_padcfg_92_95 : public vp::reg_32
{
public:
    inline void padcfg_set(uint32_t value) { this->set_field(value, GPIO_PADCFG_92_95_PADCFG_BIT, GPIO_PADCFG_92_95_PADCFG_WIDTH); }
    inline uint32_t padcfg_get() { return this->get_field(GPIO_PADCFG_92_95_PADCFG_BIT, GPIO_PADCFG_92_95_PADCFG_WIDTH); }
    vp_gpio_padcfg_92_95()
    {
        this->hw_name = "PADCFG_92_95";
        this->offset = 0xd4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PADCFG", 0, 4));
    }
};


class vp_regmap_gpio : public vp::regmap
{
public:
    vp_gpio_paddir_00_31 paddir_00_31;
    vp_gpio_gpioen_00_31 gpioen_00_31;
    vp_gpio_padin_00_31 padin_00_31;
    vp_gpio_padout_00_31 padout_00_31;
    vp_gpio_padoutset_00_31 padoutset_00_31;
    vp_gpio_padoutclr_00_31 padoutclr_00_31;
    vp_gpio_inten_00_31 inten_00_31;
    vp_gpio_inttype_00_15 inttype_00_15;
    vp_gpio_inttype_16_31 inttype_16_31;
    vp_gpio_intstatus_00_31 intstatus_00_31;
    vp_gpio_padcfg_00_03 padcfg_00_03;
    vp_gpio_padcfg_04_07 padcfg_04_07;
    vp_gpio_padcfg_08_11 padcfg_08_11;
    vp_gpio_padcfg_12_15 padcfg_12_15;
    vp_gpio_padcfg_16_19 padcfg_16_19;
    vp_gpio_padcfg_20_23 padcfg_20_23;
    vp_gpio_padcfg_24_27 padcfg_24_27;
    vp_gpio_padcfg_28_31 padcfg_28_31;
    vp_gpio_paddir_32_63 paddir_32_63;
    vp_gpio_gpioen_32_63 gpioen_32_63;
    vp_gpio_padin_32_63 padin_32_63;
    vp_gpio_padout_32_63 padout_32_63;
    vp_gpio_padoutset_32_63 padoutset_32_63;
    vp_gpio_padoutclr_32_63 padoutclr_32_63;
    vp_gpio_inten_32_63 inten_32_63;
    vp_gpio_inttype_32_47 inttype_32_47;
    vp_gpio_inttype_48_63 inttype_48_63;
    vp_gpio_intstatus_32_63 intstatus_32_63;
    vp_gpio_padcfg_32_35 padcfg_32_35;
    vp_gpio_padcfg_36_39 padcfg_36_39;
    vp_gpio_padcfg_40_43 padcfg_40_43;
    vp_gpio_padcfg_44_47 padcfg_44_47;
    vp_gpio_padcfg_48_51 padcfg_48_51;
    vp_gpio_padcfg_52_55 padcfg_52_55;
    vp_gpio_padcfg_56_59 padcfg_56_59;
    vp_gpio_padcfg_60_63 padcfg_60_63;
    vp_gpio_paddir_64_95 paddir_64_95;
    vp_gpio_gpioen_64_95 gpioen_64_95;
    vp_gpio_padin_64_95 padin_64_95;
    vp_gpio_padout_64_95 padout_64_95;
    vp_gpio_padoutset_64_95 padoutset_64_95;
    vp_gpio_padoutclr_64_95 padoutclr_64_95;
    vp_gpio_inten_64_95 inten_64_95;
    vp_gpio_inttype_64_79 inttype_64_79;
    vp_gpio_inttype_80_95 inttype_80_95;
    vp_gpio_intstatus_64_95 intstatus_64_95;
    vp_gpio_padcfg_64_67 padcfg_64_67;
    vp_gpio_padcfg_68_71 padcfg_68_71;
    vp_gpio_padcfg_72_75 padcfg_72_75;
    vp_gpio_padcfg_76_79 padcfg_76_79;
    vp_gpio_padcfg_80_83 padcfg_80_83;
    vp_gpio_padcfg_84_87 padcfg_84_87;
    vp_gpio_padcfg_88_91 padcfg_88_91;
    vp_gpio_padcfg_92_95 padcfg_92_95;
    vp_regmap_gpio()
    {
        this->registers.push_back(&this->paddir_00_31);
        this->registers.push_back(&this->gpioen_00_31);
        this->registers.push_back(&this->padin_00_31);
        this->registers.push_back(&this->padout_00_31);
        this->registers.push_back(&this->padoutset_00_31);
        this->registers.push_back(&this->padoutclr_00_31);
        this->registers.push_back(&this->inten_00_31);
        this->registers.push_back(&this->inttype_00_15);
        this->registers.push_back(&this->inttype_16_31);
        this->registers.push_back(&this->intstatus_00_31);
        this->registers.push_back(&this->padcfg_00_03);
        this->registers.push_back(&this->padcfg_04_07);
        this->registers.push_back(&this->padcfg_08_11);
        this->registers.push_back(&this->padcfg_12_15);
        this->registers.push_back(&this->padcfg_16_19);
        this->registers.push_back(&this->padcfg_20_23);
        this->registers.push_back(&this->padcfg_24_27);
        this->registers.push_back(&this->padcfg_28_31);
        this->registers.push_back(&this->paddir_32_63);
        this->registers.push_back(&this->gpioen_32_63);
        this->registers.push_back(&this->padin_32_63);
        this->registers.push_back(&this->padout_32_63);
        this->registers.push_back(&this->padoutset_32_63);
        this->registers.push_back(&this->padoutclr_32_63);
        this->registers.push_back(&this->inten_32_63);
        this->registers.push_back(&this->inttype_32_47);
        this->registers.push_back(&this->inttype_48_63);
        this->registers.push_back(&this->intstatus_32_63);
        this->registers.push_back(&this->padcfg_32_35);
        this->registers.push_back(&this->padcfg_36_39);
        this->registers.push_back(&this->padcfg_40_43);
        this->registers.push_back(&this->padcfg_44_47);
        this->registers.push_back(&this->padcfg_48_51);
        this->registers.push_back(&this->padcfg_52_55);
        this->registers.push_back(&this->padcfg_56_59);
        this->registers.push_back(&this->padcfg_60_63);
        this->registers.push_back(&this->paddir_64_95);
        this->registers.push_back(&this->gpioen_64_95);
        this->registers.push_back(&this->padin_64_95);
        this->registers.push_back(&this->padout_64_95);
        this->registers.push_back(&this->padoutset_64_95);
        this->registers.push_back(&this->padoutclr_64_95);
        this->registers.push_back(&this->inten_64_95);
        this->registers.push_back(&this->inttype_64_79);
        this->registers.push_back(&this->inttype_80_95);
        this->registers.push_back(&this->intstatus_64_95);
        this->registers.push_back(&this->padcfg_64_67);
        this->registers.push_back(&this->padcfg_68_71);
        this->registers.push_back(&this->padcfg_72_75);
        this->registers.push_back(&this->padcfg_76_79);
        this->registers.push_back(&this->padcfg_80_83);
        this->registers.push_back(&this->padcfg_84_87);
        this->registers.push_back(&this->padcfg_88_91);
        this->registers.push_back(&this->padcfg_92_95);
    }
};

#endif

#endif
