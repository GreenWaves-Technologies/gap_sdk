
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

#ifndef __INCLUDE_ARCHI_EFUSE_V1_EFUSE_REGFIELDS_H__
#define __INCLUDE_ARCHI_EFUSE_V1_EFUSE_REGFIELDS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS FIELDS
//

// Set timer counter short period (access: R/W)
#define EFUSE_CFG_CNT_TARGET1_BIT                                    0
#define EFUSE_CFG_CNT_TARGET1_WIDTH                                  32
#define EFUSE_CFG_CNT_TARGET1_MASK                                   0xffffffff

// Set timer counter medium period (access: R/W)
#define EFUSE_CFG_CNT_TARGET2_BIT                                    6
#define EFUSE_CFG_CNT_TARGET2_WIDTH                                  2
#define EFUSE_CFG_CNT_TARGET2_MASK                                   0xc0

// Set timer counter long period (access: R/W)
#define EFUSE_CFG_CNT_TARGET3_BIT                                    16
#define EFUSE_CFG_CNT_TARGET3_WIDTH                                  2
#define EFUSE_CFG_CNT_TARGET3_MASK                                   0x30000

// Margin (access: R/W)
#define EFUSE_CFG_MARGIN_BIT                                         30
#define EFUSE_CFG_MARGIN_WIDTH                                       32
#define EFUSE_CFG_MARGIN_MASK                                        0x3fffffffc0000000

#endif
