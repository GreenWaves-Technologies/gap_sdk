
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

#ifndef __INCLUDE_ARCHI_EFUSE_V1_EFUSE_MACROS_H__
#define __INCLUDE_ARCHI_EFUSE_V1_EFUSE_MACROS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define EFUSE_CFG_CNT_TARGET1_GET(value)                   (ARCHI_BEXTRACTU((value),32,0))
#define EFUSE_CFG_CNT_TARGET1_GETS(value)                  (ARCHI_BEXTRACT((value),32,0))
#define EFUSE_CFG_CNT_TARGET1_SET(value,field)             (ARCHI_BINSERT((value),(field),32,0))
#define EFUSE_CFG_CNT_TARGET1(val)                         ((val) << 0)

#define EFUSE_CFG_CNT_TARGET2_GET(value)                   (ARCHI_BEXTRACTU((value),2,6))
#define EFUSE_CFG_CNT_TARGET2_GETS(value)                  (ARCHI_BEXTRACT((value),2,6))
#define EFUSE_CFG_CNT_TARGET2_SET(value,field)             (ARCHI_BINSERT((value),(field),2,6))
#define EFUSE_CFG_CNT_TARGET2(val)                         ((val) << 6)

#define EFUSE_CFG_CNT_TARGET3_GET(value)                   (ARCHI_BEXTRACTU((value),2,16))
#define EFUSE_CFG_CNT_TARGET3_GETS(value)                  (ARCHI_BEXTRACT((value),2,16))
#define EFUSE_CFG_CNT_TARGET3_SET(value,field)             (ARCHI_BINSERT((value),(field),2,16))
#define EFUSE_CFG_CNT_TARGET3(val)                         ((val) << 16)

#define EFUSE_CFG_MARGIN_GET(value)                        (ARCHI_BEXTRACTU((value),32,30))
#define EFUSE_CFG_MARGIN_GETS(value)                       (ARCHI_BEXTRACT((value),32,30))
#define EFUSE_CFG_MARGIN_SET(value,field)                  (ARCHI_BINSERT((value),(field),32,30))
#define EFUSE_CFG_MARGIN(val)                              ((val) << 30)

#endif

#endif
