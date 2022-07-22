
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

#ifndef __ARCHI_FC_ICACHE_CTRL_MACROS__
#define __ARCHI_FC_ICACHE_CTRL_MACROS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include "periph/gap_utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define FC_ICACHE_CTRL_ENABLE_EN_GET(value)                (GAP_BEXTRACTU((value),1,0))
#define FC_ICACHE_CTRL_ENABLE_EN_GETS(value)               (GAP_BEXTRACT((value),1,0))
#define FC_ICACHE_CTRL_ENABLE_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
#define FC_ICACHE_CTRL_ENABLE_EN(val)                      ((val) << 0)

#define FC_ICACHE_CTRL_FLUSH_FL_GET(value)                 (GAP_BEXTRACTU((value),1,0))
#define FC_ICACHE_CTRL_FLUSH_FL_GETS(value)                (GAP_BEXTRACT((value),1,0))
#define FC_ICACHE_CTRL_FLUSH_FL_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
#define FC_ICACHE_CTRL_FLUSH_FL(val)                       ((val) << 0)

#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_GET(value)           (GAP_BEXTRACTU((value),32,0))
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_GETS(value)          (GAP_BEXTRACT((value),32,0))
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR(val)                 ((val) << 0)

#define FC_ICACHE_CTRL_STATUS_STATUS_GET(value)            (GAP_BEXTRACTU((value),1,0))
#define FC_ICACHE_CTRL_STATUS_STATUS_GETS(value)           (GAP_BEXTRACT((value),1,0))
#define FC_ICACHE_CTRL_STATUS_STATUS_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
#define FC_ICACHE_CTRL_STATUS_STATUS(val)                  ((val) << 0)

#endif

#endif
