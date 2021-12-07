
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

#ifndef __ARCHI_APB_SOC_CTRL_MACROS__
#define __ARCHI_APB_SOC_CTRL_MACROS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define APB_SOC_CTRL_INFO_NB_CL_GET(value)                 (GAP_BEXTRACTU((value),16,0))
#define APB_SOC_CTRL_INFO_NB_CL_GETS(value)                (GAP_BEXTRACT((value),16,0))
#define APB_SOC_CTRL_INFO_NB_CL_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
#define APB_SOC_CTRL_INFO_NB_CL(val)                       ((val) << 0)

#define APB_SOC_CTRL_INFO_NB_CORES_GET(value)              (GAP_BEXTRACTU((value),16,16))
#define APB_SOC_CTRL_INFO_NB_CORES_GETS(value)             (GAP_BEXTRACT((value),16,16))
#define APB_SOC_CTRL_INFO_NB_CORES_SET(value,field)        (GAP_BINSERT((value),(field),16,16))
#define APB_SOC_CTRL_INFO_NB_CORES(val)                    ((val) << 16)

#define APB_SOC_CTRL_FC_BOOT_ADDR_GET(value)               (GAP_BEXTRACTU((value),32,0))
#define APB_SOC_CTRL_FC_BOOT_ADDR_GETS(value)              (GAP_BEXTRACT((value),32,0))
#define APB_SOC_CTRL_FC_BOOT_ADDR_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
#define APB_SOC_CTRL_FC_BOOT_ADDR(val)                     ((val) << 0)

#define APB_SOC_CTRL_FC_FETCH_FC_FE_GET(value)             (GAP_BEXTRACTU((value),1,0))
#define APB_SOC_CTRL_FC_FETCH_FC_FE_GETS(value)            (GAP_BEXTRACT((value),1,0))
#define APB_SOC_CTRL_FC_FETCH_FC_FE_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
#define APB_SOC_CTRL_FC_FETCH_FC_FE(val)                   ((val) << 0)

#define APB_SOC_CTRL_CORESTATUS_STATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
#define APB_SOC_CTRL_CORESTATUS_STATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
#define APB_SOC_CTRL_CORESTATUS_STATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
#define APB_SOC_CTRL_CORESTATUS_STATUS(val)                ((val) << 0)

#define APB_SOC_CTRL_CORESTATUS_EOC_GET(value)             (GAP_BEXTRACTU((value),1,31))
#define APB_SOC_CTRL_CORESTATUS_EOC_GETS(value)            (GAP_BEXTRACT((value),1,31))
#define APB_SOC_CTRL_CORESTATUS_EOC_SET(value,field)       (GAP_BINSERT((value),(field),1,31))
#define APB_SOC_CTRL_CORESTATUS_EOC(val)                   ((val) << 31)

#endif

#endif
