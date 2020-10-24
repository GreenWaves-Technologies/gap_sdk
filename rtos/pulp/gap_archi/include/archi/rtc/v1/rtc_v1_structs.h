
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

#ifndef __INCLUDE_ARCHI_RTC_V1_RTC_V1_STRUCTS_H__
#define __INCLUDE_ARCHI_RTC_V1_RTC_V1_STRUCTS_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int apb_int         :2 ; // APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested
    unsigned int reserved_0      :30; // Reserved/Not used.
  };
  unsigned int raw;
} __attribute__((packed)) rtc_apb_sr_t;

typedef union {
  struct {
    unsigned int apb_addr        :6 ; // Indirect access address configuration.
    unsigned int reserved_0      :10; // Reserved/Not used.
    unsigned int apb_op          :1 ; // Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation
    unsigned int reserved_1      :15; // Reserved/Not used.
  };
  unsigned int raw;
} __attribute__((packed)) rtc_apb_cr_t;

typedef union {
  struct {
    unsigned int apb_data        :32; // Indirect access data register.
  };
  unsigned int raw;
} __attribute__((packed)) rtc_apb_dr_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) rtc_reserved_t;

typedef union {
  struct {
    unsigned int mode            :2 ; // APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration
    unsigned int reserved_0      :30; // Reserved/Not used.
  };
  unsigned int raw;
} __attribute__((packed)) rtc_apb_icr_t;

typedef union {
  struct {
    unsigned int read_mask       :1 ; // APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled
    unsigned int write_mask      :1 ; // APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled
    unsigned int reserved_0      :30; // Reserved/Not used.
  };
  unsigned int raw;
} __attribute__((packed)) rtc_apb_imr_t;

typedef union {
  struct {
    unsigned int read_flag       :1 ; // APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available
    unsigned int write_flag      :1 ; // APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done
    unsigned int reserved_0      :30; // Reserved/Not used.
  };
  unsigned int raw;
} __attribute__((packed)) rtc_apb_ifr_t;

#endif

#endif
