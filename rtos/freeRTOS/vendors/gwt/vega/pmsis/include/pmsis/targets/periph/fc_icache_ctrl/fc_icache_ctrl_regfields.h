
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

#ifndef __ARCHI_FC_ICACHE_CTRL_REGFIELD__
#define __ARCHI_FC_ICACHE_CTRL_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
#define FC_ICACHE_CTRL_ENABLE_EN_BIT                                 0
#define FC_ICACHE_CTRL_ENABLE_EN_WIDTH                               1
#define FC_ICACHE_CTRL_ENABLE_EN_MASK                                0x1
#define FC_ICACHE_CTRL_ENABLE_EN_RESET                               0x0

// FC instruction cache fully flush command. (access: W)
#define FC_ICACHE_CTRL_FLUSH_FL_BIT                                  0
#define FC_ICACHE_CTRL_FLUSH_FL_WIDTH                                1
#define FC_ICACHE_CTRL_FLUSH_FL_MASK                                 0x1
#define FC_ICACHE_CTRL_FLUSH_FL_RESET                                0x0

// FC instruction cache selective flush address configuration bitfield. (access: W)
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT                            0
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH                          32
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_MASK                           0xffffffff
#define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_RESET                          0x0

// FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action (access: R)
#define FC_ICACHE_CTRL_STATUS_STATUS_BIT                             0
#define FC_ICACHE_CTRL_STATUS_STATUS_WIDTH                           1
#define FC_ICACHE_CTRL_STATUS_STATUS_MASK                            0x1
#define FC_ICACHE_CTRL_STATUS_STATUS_RESET                           0x0

#endif
