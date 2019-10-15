/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __HAL_GVSOC_GVSOC_H__
#define __HAL_GVSOC_GVSOC_H__

#include "archi/gvsoc/gvsoc.h"

#if 0
#define PLP_SEMIHOSTING_STATS_START              20
#define PLP_SEMIHOSTING_STATS_STOP               21
#define PLP_SEMIHOSTING_STATS_DUMP               22

#define PLP_SEMIHOSTING_TRACE_START              30
#define PLP_SEMIHOSTING_TRACE_STOP               31

#define PLP_SEMIHOSTING_PROF_START              40
#define PLP_SEMIHOSTING_PROF_STOP               41
#endif

extern int __rt_semihosting_call(int, ... );

typedef struct {
} gv_vcd_conf_t;

static inline void gv_vcd_configure(int active, gv_vcd_conf_t *conf) {
  __rt_semihosting_call(GV_SEMIHOSTING_VCD_CONFIGURE, active);
}

static inline int gv_vcd_open_trace(char *path) {
  return __rt_semihosting_call(GV_SEMIHOSTING_VCD_OPEN_TRACE, path);
}

static inline void gv_vcd_conf_trace(int trace, int active) {
  __rt_semihosting_call(GV_SEMIHOSTING_VCD_CONF_TRACE, trace, active);
}

static inline void gv_vcd_dump_trace(int trace, unsigned int value) {
  __rt_semihosting_call(GV_SEMIHOSTING_VCD_DUMP_TRACE, trace, 0, 0, value);
}

static inline void gv_vcd_dump_trace_string(int trace, char *str) {
  __rt_semihosting_call(GV_SEMIHOSTING_VCD_DUMP_TRACE_STRING, trace, str);
}


#if 0
static inline void gv_stats_start() {
  __rt_semihosting_call(gv_SEMIHOSTING_STATS_START);
}

static inline void gv_stats_stop() {
  __rt_semihosting_call(gv_SEMIHOSTING_STATS_STOP);
}

static inline void gv_stats_dump() {
  __rt_semihosting_call(gv_SEMIHOSTING_STATS_DUMP);
}



static inline void gv_trace_start() {
  __rt_semihosting_call(gv_SEMIHOSTING_TRACE_START);
}

static inline void gv_trace_stop() {
  __rt_semihosting_call(gv_SEMIHOSTING_TRACE_STOP);
}



static inline void gv_prof_start() {
  __rt_semihosting_call(gv_SEMIHOSTING_PROF_START);
}

static inline void gv_prof_stop() {
  __rt_semihosting_call(gv_SEMIHOSTING_PROF_STOP);
}
#endif



#endif
