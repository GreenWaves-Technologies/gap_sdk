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
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __RT_RT_DEBUG_H__
#define __RT_RT_DEBUG_H__

/// @cond IMPLEM

static inline unsigned int rt_debug_config();

static inline unsigned int rt_debug_config_trace();

static inline int rt_debug_config_warnings();

static inline int rt_debug_config_werror();



#if defined(__RT_USE_IO)

#include <stdio.h>
#include <stdlib.h>

#define rt_msg(fmt, x...)                      \
  do { \
    printf("[\033[35mRT(%d,%d)\033[0m] " fmt, rt_cluster_id(), rt_core_id(), ##x);                                      \
  } while(0)

#define rt_warning(x...)                                       \
  do {                                                          \
    if (rt_debug_config_warnings()) rt_msg("\033[31mWARNING\033[0m: "x);  \
    if (rt_debug_config_werror()) abort();  \
  } while(0)

#define rt_fatal(msg...)    \
  do { \
    rt_msg("\033[31mFATAL\033[0m: "msg);      \
    abort();          \
  } while(0)

#else

#define rt_msg(x...)
#define rt_warning(x...)
#define rt_fatal(x...)

#endif





#if defined(__RT_USE_ASSERT)

#define rt_assert(cond, msg...)           \
  if (!(cond)) {                          \
    rt_msg("\033[31mASSERT\033[0m: "msg); \
    abort();                              \
  }

#else

#define rt_assert(x...)

#endif




#if defined(__RT_USE_TRACE)

#define RT_TRACE_INIT      0
#define RT_TRACE_ALLOC     1
#define RT_TRACE_CONF      2
#define RT_TRACE_UDMA_COPY 3
#define RT_TRACE_DMA_COPY  4
#define RT_TRACE_SCHED     5
#define RT_TRACE_DEPLOY    6
#define RT_TRACE_PMU       7
#define RT_TRACE_CAM       9
#define RT_TRACE_DEV_CTRL  10
#define RT_TRACE_FREQ      11
#define RT_TRACE_FS        12
#define RT_TRACE_UART      13
#define RT_TRACE_SPIM      14
#define RT_TRACE_FLASH     15

#define rt_trace(trace, x...)            \
  do {                                          \
    if (rt_debug_config_trace() & (1<<trace)) rt_msg(x);          \
  } while(0)

#else

#define rt_trace(x...)            \
  do {                            \
  } while(0)

#endif

#define RT_DEBUG_WARNING_BIT   0
#define RT_DEBUG_WERROR_BIT    1


extern RT_FC_GLOBAL_DATA unsigned int __rt_debug_config;
extern RT_FC_GLOBAL_DATA unsigned int __rt_debug_config_trace;

static inline unsigned int rt_debug_config() { return __rt_debug_config; }

static inline unsigned int rt_debug_config_trace() { return __rt_debug_config_trace; }

static inline int rt_debug_config_warnings() {
  return ARCHI_REG_FIELD_GET(rt_debug_config(), RT_DEBUG_WARNING_BIT, 1);
}

static inline int rt_debug_config_werror() {
  return ARCHI_REG_FIELD_GET(rt_debug_config(), RT_DEBUG_WERROR_BIT, 1);
}

/// @endcond


#endif
