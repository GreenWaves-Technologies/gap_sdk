/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __DEBUG_BRIDGE_PROXY_HPP__
#define __DEBUG_BRIDGE_PROXY_HPP__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  DEBUG_BRIDGE_JTAG_REQ  = 0,
  DEBUG_BRIDGE_RESET_REQ = 1,
  DEBUG_BRIDGE_CONFIG_REQ = 2
} proxy_req_type_e;

typedef enum {
  DEBUG_BRIDGE_JTAG_TDI  = 0,
  DEBUG_BRIDGE_JTAG_TMS  = 1,
  DEBUG_BRIDGE_JTAG_TRST = 2
} proxy_req_jtag_cycle_e;

typedef struct {
  int32_t type;

  union {
    struct {
      int32_t bits;
      int8_t  tdo;
    } jtag;
    struct {
      int32_t active;
      int32_t duration;
    } reset;
    struct {
      int32_t value;
    } config;
  };

} proxy_req_t;


#ifdef __cplusplus
}
#endif

#endif
