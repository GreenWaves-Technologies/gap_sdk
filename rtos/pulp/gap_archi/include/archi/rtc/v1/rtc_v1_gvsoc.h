
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __INCLUDE_ARCHI_RTC_V1_RTC_V1_GVSOC_H__
#define __INCLUDE_ARCHI_RTC_V1_RTC_V1_GVSOC_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_rtc_apb_sr : public vp::reg_32
{
public:
  inline void apb_int_set(uint32_t value) { this->set_field(value, RTC_APB_SR_APB_INT_BIT, RTC_APB_SR_APB_INT_WIDTH); }
  inline uint32_t apb_int_get() { return this->get_field(RTC_APB_SR_APB_INT_BIT, RTC_APB_SR_APB_INT_WIDTH); }
  inline void reserved_0_set(uint32_t value) { this->set_field(value, RTC_APB_SR_RESERVED_0_BIT, RTC_APB_SR_RESERVED_0_WIDTH); }
  inline uint32_t reserved_0_get() { return this->get_field(RTC_APB_SR_RESERVED_0_BIT, RTC_APB_SR_RESERVED_0_WIDTH); }
};

class vp_rtc_apb_cr : public vp::reg_32
{
public:
  inline void apb_addr_set(uint32_t value) { this->set_field(value, RTC_APB_CR_APB_ADDR_BIT, RTC_APB_CR_APB_ADDR_WIDTH); }
  inline uint32_t apb_addr_get() { return this->get_field(RTC_APB_CR_APB_ADDR_BIT, RTC_APB_CR_APB_ADDR_WIDTH); }
  inline void reserved_0_set(uint32_t value) { this->set_field(value, RTC_APB_CR_RESERVED_0_BIT, RTC_APB_CR_RESERVED_0_WIDTH); }
  inline uint32_t reserved_0_get() { return this->get_field(RTC_APB_CR_RESERVED_0_BIT, RTC_APB_CR_RESERVED_0_WIDTH); }
  inline void apb_op_set(uint32_t value) { this->set_field(value, RTC_APB_CR_APB_OP_BIT, RTC_APB_CR_APB_OP_WIDTH); }
  inline uint32_t apb_op_get() { return this->get_field(RTC_APB_CR_APB_OP_BIT, RTC_APB_CR_APB_OP_WIDTH); }
  inline void reserved_1_set(uint32_t value) { this->set_field(value, RTC_APB_CR_RESERVED_1_BIT, RTC_APB_CR_RESERVED_1_WIDTH); }
  inline uint32_t reserved_1_get() { return this->get_field(RTC_APB_CR_RESERVED_1_BIT, RTC_APB_CR_RESERVED_1_WIDTH); }
};

class vp_rtc_apb_dr : public vp::reg_32
{
public:
  inline void apb_data_set(uint32_t value) { this->set_field(value, RTC_APB_DR_APB_DATA_BIT, RTC_APB_DR_APB_DATA_WIDTH); }
  inline uint32_t apb_data_get() { return this->get_field(RTC_APB_DR_APB_DATA_BIT, RTC_APB_DR_APB_DATA_WIDTH); }
};

class vp_rtc_reserved : public vp::reg_32
{
public:
};

class vp_rtc_apb_icr : public vp::reg_32
{
public:
  inline void mode_set(uint32_t value) { this->set_field(value, RTC_APB_ICR_MODE_BIT, RTC_APB_ICR_MODE_WIDTH); }
  inline uint32_t mode_get() { return this->get_field(RTC_APB_ICR_MODE_BIT, RTC_APB_ICR_MODE_WIDTH); }
  inline void reserved_0_set(uint32_t value) { this->set_field(value, RTC_APB_ICR_RESERVED_0_BIT, RTC_APB_ICR_RESERVED_0_WIDTH); }
  inline uint32_t reserved_0_get() { return this->get_field(RTC_APB_ICR_RESERVED_0_BIT, RTC_APB_ICR_RESERVED_0_WIDTH); }
};

class vp_rtc_apb_imr : public vp::reg_32
{
public:
  inline void read_mask_set(uint32_t value) { this->set_field(value, RTC_APB_IMR_READ_MASK_BIT, RTC_APB_IMR_READ_MASK_WIDTH); }
  inline uint32_t read_mask_get() { return this->get_field(RTC_APB_IMR_READ_MASK_BIT, RTC_APB_IMR_READ_MASK_WIDTH); }
  inline void write_mask_set(uint32_t value) { this->set_field(value, RTC_APB_IMR_WRITE_MASK_BIT, RTC_APB_IMR_WRITE_MASK_WIDTH); }
  inline uint32_t write_mask_get() { return this->get_field(RTC_APB_IMR_WRITE_MASK_BIT, RTC_APB_IMR_WRITE_MASK_WIDTH); }
  inline void reserved_0_set(uint32_t value) { this->set_field(value, RTC_APB_IMR_RESERVED_0_BIT, RTC_APB_IMR_RESERVED_0_WIDTH); }
  inline uint32_t reserved_0_get() { return this->get_field(RTC_APB_IMR_RESERVED_0_BIT, RTC_APB_IMR_RESERVED_0_WIDTH); }
};

class vp_rtc_apb_ifr : public vp::reg_32
{
public:
  inline void read_flag_set(uint32_t value) { this->set_field(value, RTC_APB_IFR_READ_FLAG_BIT, RTC_APB_IFR_READ_FLAG_WIDTH); }
  inline uint32_t read_flag_get() { return this->get_field(RTC_APB_IFR_READ_FLAG_BIT, RTC_APB_IFR_READ_FLAG_WIDTH); }
  inline void write_flag_set(uint32_t value) { this->set_field(value, RTC_APB_IFR_WRITE_FLAG_BIT, RTC_APB_IFR_WRITE_FLAG_WIDTH); }
  inline uint32_t write_flag_get() { return this->get_field(RTC_APB_IFR_WRITE_FLAG_BIT, RTC_APB_IFR_WRITE_FLAG_WIDTH); }
  inline void reserved_0_set(uint32_t value) { this->set_field(value, RTC_APB_IFR_RESERVED_0_BIT, RTC_APB_IFR_RESERVED_0_WIDTH); }
  inline uint32_t reserved_0_get() { return this->get_field(RTC_APB_IFR_RESERVED_0_BIT, RTC_APB_IFR_RESERVED_0_WIDTH); }
};

#endif

#endif
