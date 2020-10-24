
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

#ifndef __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_MACROS_H__
#define __INCLUDE_ARCHI_SOC_EU_V4_SOC_EU_MACROS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define SOC_EU_SW_EVENT_EVENT_GET(value)                   (ARCHI_BEXTRACTU((value),8,0))
#define SOC_EU_SW_EVENT_EVENT_GETS(value)                  (ARCHI_BEXTRACT((value),8,0))
#define SOC_EU_SW_EVENT_EVENT_SET(value,field)             (ARCHI_BINSERT((value),(field),8,0))
#define SOC_EU_SW_EVENT_EVENT(val)                         ((val) << 0)

#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_GET(value)          (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_GETS(value)         (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_SET(value,field)    (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_FC_MASK_MSB_FC_MASK_MSB(val)                ((val) << 0)

#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_GET(value)          (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_GETS(value)         (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_SET(value,field)    (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_FC_MASK_LSB_FC_MASK_LSB(val)                ((val) << 0)

#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_GET(value)          (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_GETS(value)         (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_SET(value,field)    (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_CL_MASK_MSB_CL_MASK_MSB(val)                ((val) << 0)

#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_GET(value)          (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_GETS(value)         (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_SET(value,field)    (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_CL_MASK_LSB_CL_MASK_LSB(val)                ((val) << 0)

#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_GET(value)          (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_GETS(value)         (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_SET(value,field)    (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_PR_MASK_MSB_PR_MASK_MSB(val)                ((val) << 0)

#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_GET(value)          (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_GETS(value)         (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_SET(value,field)    (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_PR_MASK_LSB_PR_MASK_LSB(val)                ((val) << 0)

#define SOC_EU_ERR_MSB_ERR_MSB_GET(value)                  (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_ERR_MSB_ERR_MSB_GETS(value)                 (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_ERR_MSB_ERR_MSB_SET(value,field)            (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_ERR_MSB_ERR_MSB(val)                        ((val) << 0)

#define SOC_EU_ERR_LSB_ERR_LSB_GET(value)                  (ARCHI_BEXTRACTU((value),32,0))
#define SOC_EU_ERR_LSB_ERR_LSB_GETS(value)                 (ARCHI_BEXTRACT((value),32,0))
#define SOC_EU_ERR_LSB_ERR_LSB_SET(value,field)            (ARCHI_BINSERT((value),(field),32,0))
#define SOC_EU_ERR_LSB_ERR_LSB(val)                        ((val) << 0)

#define SOC_EU_TIMER1_SEL_HI_EVT_GET(value)                (ARCHI_BEXTRACTU((value),8,0))
#define SOC_EU_TIMER1_SEL_HI_EVT_GETS(value)               (ARCHI_BEXTRACT((value),8,0))
#define SOC_EU_TIMER1_SEL_HI_EVT_SET(value,field)          (ARCHI_BINSERT((value),(field),8,0))
#define SOC_EU_TIMER1_SEL_HI_EVT(val)                      ((val) << 0)

#define SOC_EU_TIMER1_SEL_HI_ENA_GET(value)                (ARCHI_BEXTRACTU((value),1,31))
#define SOC_EU_TIMER1_SEL_HI_ENA_GETS(value)               (ARCHI_BEXTRACT((value),1,31))
#define SOC_EU_TIMER1_SEL_HI_ENA_SET(value,field)          (ARCHI_BINSERT((value),(field),1,31))
#define SOC_EU_TIMER1_SEL_HI_ENA(val)                      ((val) << 31)

#define SOC_EU_TIMER1_SEL_LO_EVT_GET(value)                (ARCHI_BEXTRACTU((value),8,0))
#define SOC_EU_TIMER1_SEL_LO_EVT_GETS(value)               (ARCHI_BEXTRACT((value),8,0))
#define SOC_EU_TIMER1_SEL_LO_EVT_SET(value,field)          (ARCHI_BINSERT((value),(field),8,0))
#define SOC_EU_TIMER1_SEL_LO_EVT(val)                      ((val) << 0)

#define SOC_EU_TIMER1_SEL_LO_ENA_GET(value)                (ARCHI_BEXTRACTU((value),1,31))
#define SOC_EU_TIMER1_SEL_LO_ENA_GETS(value)               (ARCHI_BEXTRACT((value),1,31))
#define SOC_EU_TIMER1_SEL_LO_ENA_SET(value,field)          (ARCHI_BINSERT((value),(field),1,31))
#define SOC_EU_TIMER1_SEL_LO_ENA(val)                      ((val) << 31)

#define SOC_EU_TIMER2_SEL_HI_EVT_GET(value)                (ARCHI_BEXTRACTU((value),8,0))
#define SOC_EU_TIMER2_SEL_HI_EVT_GETS(value)               (ARCHI_BEXTRACT((value),8,0))
#define SOC_EU_TIMER2_SEL_HI_EVT_SET(value,field)          (ARCHI_BINSERT((value),(field),8,0))
#define SOC_EU_TIMER2_SEL_HI_EVT(val)                      ((val) << 0)

#define SOC_EU_TIMER2_SEL_HI_ENA_GET(value)                (ARCHI_BEXTRACTU((value),1,31))
#define SOC_EU_TIMER2_SEL_HI_ENA_GETS(value)               (ARCHI_BEXTRACT((value),1,31))
#define SOC_EU_TIMER2_SEL_HI_ENA_SET(value,field)          (ARCHI_BINSERT((value),(field),1,31))
#define SOC_EU_TIMER2_SEL_HI_ENA(val)                      ((val) << 31)

#define SOC_EU_TIMER2_SEL_LO_EVT_GET(value)                (ARCHI_BEXTRACTU((value),8,0))
#define SOC_EU_TIMER2_SEL_LO_EVT_GETS(value)               (ARCHI_BEXTRACT((value),8,0))
#define SOC_EU_TIMER2_SEL_LO_EVT_SET(value,field)          (ARCHI_BINSERT((value),(field),8,0))
#define SOC_EU_TIMER2_SEL_LO_EVT(val)                      ((val) << 0)

#define SOC_EU_TIMER2_SEL_LO_ENA_GET(value)                (ARCHI_BEXTRACTU((value),1,31))
#define SOC_EU_TIMER2_SEL_LO_ENA_GETS(value)               (ARCHI_BEXTRACT((value),1,31))
#define SOC_EU_TIMER2_SEL_LO_ENA_SET(value,field)          (ARCHI_BINSERT((value),(field),1,31))
#define SOC_EU_TIMER2_SEL_LO_ENA(val)                      ((val) << 31)

#endif

#endif
