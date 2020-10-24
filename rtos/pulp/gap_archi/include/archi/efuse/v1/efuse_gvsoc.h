
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

#ifndef __INCLUDE_ARCHI_EFUSE_V1_EFUSE_GVSOC_H__
#define __INCLUDE_ARCHI_EFUSE_V1_EFUSE_GVSOC_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_efuse_cmd : public vp::reg_32
{
public:
};

class vp_efuse_cfg : public vp::reg_32
{
public:
  inline void cnt_target1_set(uint32_t value) { this->set_field(value, EFUSE_CFG_CNT_TARGET1_BIT, EFUSE_CFG_CNT_TARGET1_WIDTH); }
  inline uint32_t cnt_target1_get() { return this->get_field(EFUSE_CFG_CNT_TARGET1_BIT, EFUSE_CFG_CNT_TARGET1_WIDTH); }
  inline void cnt_target2_set(uint32_t value) { this->set_field(value, EFUSE_CFG_CNT_TARGET2_BIT, EFUSE_CFG_CNT_TARGET2_WIDTH); }
  inline uint32_t cnt_target2_get() { return this->get_field(EFUSE_CFG_CNT_TARGET2_BIT, EFUSE_CFG_CNT_TARGET2_WIDTH); }
  inline void cnt_target3_set(uint32_t value) { this->set_field(value, EFUSE_CFG_CNT_TARGET3_BIT, EFUSE_CFG_CNT_TARGET3_WIDTH); }
  inline uint32_t cnt_target3_get() { return this->get_field(EFUSE_CFG_CNT_TARGET3_BIT, EFUSE_CFG_CNT_TARGET3_WIDTH); }
  inline void margin_set(uint32_t value) { this->set_field(value, EFUSE_CFG_MARGIN_BIT, EFUSE_CFG_MARGIN_WIDTH); }
  inline uint32_t margin_get() { return this->get_field(EFUSE_CFG_MARGIN_BIT, EFUSE_CFG_MARGIN_WIDTH); }
};

#endif

#endif
