
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

#ifndef __INCLUDE_ARCHI_EFUSE_V1_EFUSE_ACCESSORS_H__
#define __INCLUDE_ARCHI_EFUSE_V1_EFUSE_ACCESSORS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t efuse_cmd_get(uint32_t base) { return ARCHI_READ(base, EFUSE_CMD_OFFSET); }
static inline void efuse_cmd_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, EFUSE_CMD_OFFSET, value); }

static inline uint32_t efuse_cfg_get(uint32_t base) { return ARCHI_READ(base, EFUSE_CFG_OFFSET); }
static inline void efuse_cfg_set(uint32_t base, uint32_t value) { ARCHI_WRITE(base, EFUSE_CFG_OFFSET, value); }

#endif

#endif
