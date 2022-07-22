
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

#ifndef __ARCHI_FC_ICACHE_CTRL_STRUCTS__
#define __ARCHI_FC_ICACHE_CTRL_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int en              :1 ; // FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
  };
  unsigned int raw;
} __attribute__((packed)) fc_icache_ctrl_enable_t;

typedef union {
  struct {
    unsigned int fl              :1 ; // FC instruction cache fully flush command.
  };
  unsigned int raw;
} __attribute__((packed)) fc_icache_ctrl_flush_t;

typedef union {
  struct {
    unsigned int addr            :32; // FC instruction cache selective flush address configuration bitfield.
  };
  unsigned int raw;
} __attribute__((packed)) fc_icache_ctrl_sel_flush_t;

typedef union {
  struct {
    unsigned int status          :1 ; // FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action
  };
  unsigned int raw;
} __attribute__((packed)) fc_icache_ctrl_status_t;

#endif

#endif
