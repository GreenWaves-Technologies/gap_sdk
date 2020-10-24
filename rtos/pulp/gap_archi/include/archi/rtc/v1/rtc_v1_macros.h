
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

#ifndef __INCLUDE_ARCHI_RTC_V1_RTC_V1_MACROS_H__
#define __INCLUDE_ARCHI_RTC_V1_RTC_V1_MACROS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define RTC_APB_SR_APB_INT_GET(value)                      (ARCHI_BEXTRACTU((value),2,0))
#define RTC_APB_SR_APB_INT_GETS(value)                     (ARCHI_BEXTRACT((value),2,0))
#define RTC_APB_SR_APB_INT_SET(value,field)                (ARCHI_BINSERT((value),(field),2,0))
#define RTC_APB_SR_APB_INT(val)                            ((val) << 0)

#define RTC_APB_SR_RESERVED_0_GET(value)                   (ARCHI_BEXTRACTU((value),30,2))
#define RTC_APB_SR_RESERVED_0_GETS(value)                  (ARCHI_BEXTRACT((value),30,2))
#define RTC_APB_SR_RESERVED_0_SET(value,field)             (ARCHI_BINSERT((value),(field),30,2))
#define RTC_APB_SR_RESERVED_0(val)                         ((val) << 2)

#define RTC_APB_CR_APB_ADDR_GET(value)                     (ARCHI_BEXTRACTU((value),6,0))
#define RTC_APB_CR_APB_ADDR_GETS(value)                    (ARCHI_BEXTRACT((value),6,0))
#define RTC_APB_CR_APB_ADDR_SET(value,field)               (ARCHI_BINSERT((value),(field),6,0))
#define RTC_APB_CR_APB_ADDR(val)                           ((val) << 0)

#define RTC_APB_CR_RESERVED_0_GET(value)                   (ARCHI_BEXTRACTU((value),10,6))
#define RTC_APB_CR_RESERVED_0_GETS(value)                  (ARCHI_BEXTRACT((value),10,6))
#define RTC_APB_CR_RESERVED_0_SET(value,field)             (ARCHI_BINSERT((value),(field),10,6))
#define RTC_APB_CR_RESERVED_0(val)                         ((val) << 6)

#define RTC_APB_CR_APB_OP_GET(value)                       (ARCHI_BEXTRACTU((value),1,16))
#define RTC_APB_CR_APB_OP_GETS(value)                      (ARCHI_BEXTRACT((value),1,16))
#define RTC_APB_CR_APB_OP_SET(value,field)                 (ARCHI_BINSERT((value),(field),1,16))
#define RTC_APB_CR_APB_OP(val)                             ((val) << 16)

#define RTC_APB_CR_RESERVED_1_GET(value)                   (ARCHI_BEXTRACTU((value),15,17))
#define RTC_APB_CR_RESERVED_1_GETS(value)                  (ARCHI_BEXTRACT((value),15,17))
#define RTC_APB_CR_RESERVED_1_SET(value,field)             (ARCHI_BINSERT((value),(field),15,17))
#define RTC_APB_CR_RESERVED_1(val)                         ((val) << 17)

#define RTC_APB_DR_APB_DATA_GET(value)                     (ARCHI_BEXTRACTU((value),32,0))
#define RTC_APB_DR_APB_DATA_GETS(value)                    (ARCHI_BEXTRACT((value),32,0))
#define RTC_APB_DR_APB_DATA_SET(value,field)               (ARCHI_BINSERT((value),(field),32,0))
#define RTC_APB_DR_APB_DATA(val)                           ((val) << 0)

#define RTC_APB_ICR_MODE_GET(value)                        (ARCHI_BEXTRACTU((value),2,0))
#define RTC_APB_ICR_MODE_GETS(value)                       (ARCHI_BEXTRACT((value),2,0))
#define RTC_APB_ICR_MODE_SET(value,field)                  (ARCHI_BINSERT((value),(field),2,0))
#define RTC_APB_ICR_MODE(val)                              ((val) << 0)

#define RTC_APB_ICR_RESERVED_0_GET(value)                  (ARCHI_BEXTRACTU((value),30,2))
#define RTC_APB_ICR_RESERVED_0_GETS(value)                 (ARCHI_BEXTRACT((value),30,2))
#define RTC_APB_ICR_RESERVED_0_SET(value,field)            (ARCHI_BINSERT((value),(field),30,2))
#define RTC_APB_ICR_RESERVED_0(val)                        ((val) << 2)

#define RTC_APB_IMR_READ_MASK_GET(value)                   (ARCHI_BEXTRACTU((value),1,0))
#define RTC_APB_IMR_READ_MASK_GETS(value)                  (ARCHI_BEXTRACT((value),1,0))
#define RTC_APB_IMR_READ_MASK_SET(value,field)             (ARCHI_BINSERT((value),(field),1,0))
#define RTC_APB_IMR_READ_MASK(val)                         ((val) << 0)

#define RTC_APB_IMR_WRITE_MASK_GET(value)                  (ARCHI_BEXTRACTU((value),1,1))
#define RTC_APB_IMR_WRITE_MASK_GETS(value)                 (ARCHI_BEXTRACT((value),1,1))
#define RTC_APB_IMR_WRITE_MASK_SET(value,field)            (ARCHI_BINSERT((value),(field),1,1))
#define RTC_APB_IMR_WRITE_MASK(val)                        ((val) << 1)

#define RTC_APB_IMR_RESERVED_0_GET(value)                  (ARCHI_BEXTRACTU((value),30,2))
#define RTC_APB_IMR_RESERVED_0_GETS(value)                 (ARCHI_BEXTRACT((value),30,2))
#define RTC_APB_IMR_RESERVED_0_SET(value,field)            (ARCHI_BINSERT((value),(field),30,2))
#define RTC_APB_IMR_RESERVED_0(val)                        ((val) << 2)

#define RTC_APB_IFR_READ_FLAG_GET(value)                   (ARCHI_BEXTRACTU((value),1,0))
#define RTC_APB_IFR_READ_FLAG_GETS(value)                  (ARCHI_BEXTRACT((value),1,0))
#define RTC_APB_IFR_READ_FLAG_SET(value,field)             (ARCHI_BINSERT((value),(field),1,0))
#define RTC_APB_IFR_READ_FLAG(val)                         ((val) << 0)

#define RTC_APB_IFR_WRITE_FLAG_GET(value)                  (ARCHI_BEXTRACTU((value),1,1))
#define RTC_APB_IFR_WRITE_FLAG_GETS(value)                 (ARCHI_BEXTRACT((value),1,1))
#define RTC_APB_IFR_WRITE_FLAG_SET(value,field)            (ARCHI_BINSERT((value),(field),1,1))
#define RTC_APB_IFR_WRITE_FLAG(val)                        ((val) << 1)

#define RTC_APB_IFR_RESERVED_0_GET(value)                  (ARCHI_BEXTRACTU((value),30,2))
#define RTC_APB_IFR_RESERVED_0_GETS(value)                 (ARCHI_BEXTRACT((value),30,2))
#define RTC_APB_IFR_RESERVED_0_SET(value,field)            (ARCHI_BINSERT((value),(field),30,2))
#define RTC_APB_IFR_RESERVED_0(val)                        ((val) << 2)

#endif

#endif
