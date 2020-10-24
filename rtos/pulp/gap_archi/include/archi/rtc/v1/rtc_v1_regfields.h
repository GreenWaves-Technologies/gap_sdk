
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

#ifndef __INCLUDE_ARCHI_RTC_V1_RTC_V1_REGFIELDS_H__
#define __INCLUDE_ARCHI_RTC_V1_RTC_V1_REGFIELDS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS FIELDS
//

// APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested (access: R)
#define RTC_APB_SR_APB_INT_BIT                                       0
#define RTC_APB_SR_APB_INT_WIDTH                                     2
#define RTC_APB_SR_APB_INT_MASK                                      0x3

// Reserved/Not used. (access: R/W)
#define RTC_APB_SR_RESERVED_0_BIT                                    2
#define RTC_APB_SR_RESERVED_0_WIDTH                                  30
#define RTC_APB_SR_RESERVED_0_MASK                                   0xfffffffc

// Indirect access address configuration. (access: R/W)
#define RTC_APB_CR_APB_ADDR_BIT                                      0
#define RTC_APB_CR_APB_ADDR_WIDTH                                    6
#define RTC_APB_CR_APB_ADDR_MASK                                     0x3f

// Reserved/Not used. (access: R/W)
#define RTC_APB_CR_RESERVED_0_BIT                                    6
#define RTC_APB_CR_RESERVED_0_WIDTH                                  10
#define RTC_APB_CR_RESERVED_0_MASK                                   0xffc0

// Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation (access: R/W)
#define RTC_APB_CR_APB_OP_BIT                                        16
#define RTC_APB_CR_APB_OP_WIDTH                                      1
#define RTC_APB_CR_APB_OP_MASK                                       0x10000

// Reserved/Not used. (access: R/W)
#define RTC_APB_CR_RESERVED_1_BIT                                    17
#define RTC_APB_CR_RESERVED_1_WIDTH                                  15
#define RTC_APB_CR_RESERVED_1_MASK                                   0xfffe0000

// Indirect access data register. (access: R/W)
#define RTC_APB_DR_APB_DATA_BIT                                      0
#define RTC_APB_DR_APB_DATA_WIDTH                                    32
#define RTC_APB_DR_APB_DATA_MASK                                     0xffffffff

// APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration (access: R/W)
#define RTC_APB_ICR_MODE_BIT                                         0
#define RTC_APB_ICR_MODE_WIDTH                                       2
#define RTC_APB_ICR_MODE_MASK                                        0x3

// Reserved/Not used. (access: R/W)
#define RTC_APB_ICR_RESERVED_0_BIT                                   2
#define RTC_APB_ICR_RESERVED_0_WIDTH                                 30
#define RTC_APB_ICR_RESERVED_0_MASK                                  0xfffffffc

// APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled (access: R/W)
#define RTC_APB_IMR_READ_MASK_BIT                                    0
#define RTC_APB_IMR_READ_MASK_WIDTH                                  1
#define RTC_APB_IMR_READ_MASK_MASK                                   0x1

// APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled (access: R/W)
#define RTC_APB_IMR_WRITE_MASK_BIT                                   1
#define RTC_APB_IMR_WRITE_MASK_WIDTH                                 1
#define RTC_APB_IMR_WRITE_MASK_MASK                                  0x2

// Reserved/Not used. (access: R/W)
#define RTC_APB_IMR_RESERVED_0_BIT                                   2
#define RTC_APB_IMR_RESERVED_0_WIDTH                                 30
#define RTC_APB_IMR_RESERVED_0_MASK                                  0xfffffffc

// APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available (access: R/W)
#define RTC_APB_IFR_READ_FLAG_BIT                                    0
#define RTC_APB_IFR_READ_FLAG_WIDTH                                  1
#define RTC_APB_IFR_READ_FLAG_MASK                                   0x1

// APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done (access: R/W)
#define RTC_APB_IFR_WRITE_FLAG_BIT                                   1
#define RTC_APB_IFR_WRITE_FLAG_WIDTH                                 1
#define RTC_APB_IFR_WRITE_FLAG_MASK                                  0x2

// Reserved/Not used. (access: R/W)
#define RTC_APB_IFR_RESERVED_0_BIT                                   2
#define RTC_APB_IFR_RESERVED_0_WIDTH                                 30
#define RTC_APB_IFR_RESERVED_0_MASK                                  0xfffffffc

#endif
