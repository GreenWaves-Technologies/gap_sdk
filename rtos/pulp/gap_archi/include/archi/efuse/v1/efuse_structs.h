
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

#ifndef __INCLUDE_ARCHI_EFUSE_V1_EFUSE_STRUCTS_H__
#define __INCLUDE_ARCHI_EFUSE_V1_EFUSE_STRUCTS_H__

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
  };
  unsigned int raw;
} __attribute__((packed)) efuse_cmd_t;

typedef union {
  struct {
    unsigned int cnt_target1     :32; // Set timer counter short period
    unsigned int cnt_target2     :2 ; // Set timer counter medium period
    unsigned int padding0:8 ;
    unsigned int cnt_target3     :2 ; // Set timer counter long period
    unsigned int padding1:12;
    unsigned int margin          :32; // Margin
  };
  unsigned int raw;
} __attribute__((packed)) efuse_cfg_t;

#endif

#endif
