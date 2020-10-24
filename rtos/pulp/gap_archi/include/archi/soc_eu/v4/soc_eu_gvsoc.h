
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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_GVSOC_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_GVSOC_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

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
};

class vp_soc_eu_fc_mask_msb : public vp::reg_32
{
public:
  inline void fc_mask_msb_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_MSB_FC_MASK_MSB_BIT, SOC_EU_FC_MASK_MSB_FC_MASK_MSB_WIDTH); }
  inline uint32_t fc_mask_msb_get() { return this->get_field(SOC_EU_FC_MASK_MSB_FC_MASK_MSB_BIT, SOC_EU_FC_MASK_MSB_FC_MASK_MSB_WIDTH); }
};

class vp_soc_eu_fc_mask_lsb : public vp::reg_32
{
public:
  inline void fc_mask_lsb_set(uint32_t value) { this->set_field(value, SOC_EU_FC_MASK_LSB_FC_MASK_LSB_BIT, SOC_EU_FC_MASK_LSB_FC_MASK_LSB_WIDTH); }
  inline uint32_t fc_mask_lsb_get() { return this->get_field(SOC_EU_FC_MASK_LSB_FC_MASK_LSB_BIT, SOC_EU_FC_MASK_LSB_FC_MASK_LSB_WIDTH); }
};

class vp_soc_eu_cl_mask_msb : public vp::reg_32
{
public:
  inline void cl_mask_msb_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_MSB_CL_MASK_MSB_BIT, SOC_EU_CL_MASK_MSB_CL_MASK_MSB_WIDTH); }
  inline uint32_t cl_mask_msb_get() { return this->get_field(SOC_EU_CL_MASK_MSB_CL_MASK_MSB_BIT, SOC_EU_CL_MASK_MSB_CL_MASK_MSB_WIDTH); }
};

class vp_soc_eu_cl_mask_lsb : public vp::reg_32
{
public:
  inline void cl_mask_lsb_set(uint32_t value) { this->set_field(value, SOC_EU_CL_MASK_LSB_CL_MASK_LSB_BIT, SOC_EU_CL_MASK_LSB_CL_MASK_LSB_WIDTH); }
  inline uint32_t cl_mask_lsb_get() { return this->get_field(SOC_EU_CL_MASK_LSB_CL_MASK_LSB_BIT, SOC_EU_CL_MASK_LSB_CL_MASK_LSB_WIDTH); }
};

class vp_soc_eu_pr_mask_msb : public vp::reg_32
{
public:
  inline void pr_mask_msb_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_MSB_PR_MASK_MSB_BIT, SOC_EU_PR_MASK_MSB_PR_MASK_MSB_WIDTH); }
  inline uint32_t pr_mask_msb_get() { return this->get_field(SOC_EU_PR_MASK_MSB_PR_MASK_MSB_BIT, SOC_EU_PR_MASK_MSB_PR_MASK_MSB_WIDTH); }
};

class vp_soc_eu_pr_mask_lsb : public vp::reg_32
{
public:
  inline void pr_mask_lsb_set(uint32_t value) { this->set_field(value, SOC_EU_PR_MASK_LSB_PR_MASK_LSB_BIT, SOC_EU_PR_MASK_LSB_PR_MASK_LSB_WIDTH); }
  inline uint32_t pr_mask_lsb_get() { return this->get_field(SOC_EU_PR_MASK_LSB_PR_MASK_LSB_BIT, SOC_EU_PR_MASK_LSB_PR_MASK_LSB_WIDTH); }
};

class vp_soc_eu_err_msb : public vp::reg_32
{
public:
  inline void err_msb_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_MSB_ERR_MSB_BIT, SOC_EU_ERR_MSB_ERR_MSB_WIDTH); }
  inline uint32_t err_msb_get() { return this->get_field(SOC_EU_ERR_MSB_ERR_MSB_BIT, SOC_EU_ERR_MSB_ERR_MSB_WIDTH); }
};

class vp_soc_eu_err_lsb : public vp::reg_32
{
public:
  inline void err_lsb_set(uint32_t value) { this->set_field(value, SOC_EU_ERR_LSB_ERR_LSB_BIT, SOC_EU_ERR_LSB_ERR_LSB_WIDTH); }
  inline uint32_t err_lsb_get() { return this->get_field(SOC_EU_ERR_LSB_ERR_LSB_BIT, SOC_EU_ERR_LSB_ERR_LSB_WIDTH); }
};

class vp_soc_eu_timer1_sel_hi : public vp::reg_32
{
public:
  inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_HI_EVT_BIT, SOC_EU_TIMER1_SEL_HI_EVT_WIDTH); }
  inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER1_SEL_HI_EVT_BIT, SOC_EU_TIMER1_SEL_HI_EVT_WIDTH); }
  inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_HI_ENA_BIT, SOC_EU_TIMER1_SEL_HI_ENA_WIDTH); }
  inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER1_SEL_HI_ENA_BIT, SOC_EU_TIMER1_SEL_HI_ENA_WIDTH); }
};

class vp_soc_eu_timer1_sel_lo : public vp::reg_32
{
public:
  inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_LO_EVT_BIT, SOC_EU_TIMER1_SEL_LO_EVT_WIDTH); }
  inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER1_SEL_LO_EVT_BIT, SOC_EU_TIMER1_SEL_LO_EVT_WIDTH); }
  inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER1_SEL_LO_ENA_BIT, SOC_EU_TIMER1_SEL_LO_ENA_WIDTH); }
  inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER1_SEL_LO_ENA_BIT, SOC_EU_TIMER1_SEL_LO_ENA_WIDTH); }
};

class vp_soc_eu_timer2_sel_hi : public vp::reg_32
{
public:
  inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_HI_EVT_BIT, SOC_EU_TIMER2_SEL_HI_EVT_WIDTH); }
  inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER2_SEL_HI_EVT_BIT, SOC_EU_TIMER2_SEL_HI_EVT_WIDTH); }
  inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_HI_ENA_BIT, SOC_EU_TIMER2_SEL_HI_ENA_WIDTH); }
  inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER2_SEL_HI_ENA_BIT, SOC_EU_TIMER2_SEL_HI_ENA_WIDTH); }
};

class vp_soc_eu_timer2_sel_lo : public vp::reg_32
{
public:
  inline void evt_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_LO_EVT_BIT, SOC_EU_TIMER2_SEL_LO_EVT_WIDTH); }
  inline uint32_t evt_get() { return this->get_field(SOC_EU_TIMER2_SEL_LO_EVT_BIT, SOC_EU_TIMER2_SEL_LO_EVT_WIDTH); }
  inline void ena_set(uint32_t value) { this->set_field(value, SOC_EU_TIMER2_SEL_LO_ENA_BIT, SOC_EU_TIMER2_SEL_LO_ENA_WIDTH); }
  inline uint32_t ena_get() { return this->get_field(SOC_EU_TIMER2_SEL_LO_ENA_BIT, SOC_EU_TIMER2_SEL_LO_ENA_WIDTH); }
};

#endif

#endif
