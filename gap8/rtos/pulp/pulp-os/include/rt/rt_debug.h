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

#include "io/stdlib.h"      // abort()
#include "rt/rt_data.h"     // RT_FC_GLOBAL_DATA
#include "rt/rt_utils.h"    // rt_{cluster,core}_id(),

/// @cond IMPLEM

#define RT_LOG_FATAL    0   // Fatal condition, the system should terminate execution ASAP
#define RT_LOG_ERROR    1   // Error condition, might require manual intervention
#define RT_LOG_WARNING  2   // Unusual condition, not serious by itself but might indicate problems
#define RT_LOG_INFO     3   // Information on the status of the system
#define RT_LOG_DEBUG    4   // Debug messages
#define RT_LOG_TRACE    5   // Very fine-grained debug and status messages

static inline unsigned int rt_debug_config();

static inline unsigned int rt_debug_config_trace();

static inline int rt_debug_config_warnings();

static inline int rt_debug_config_werror();

#ifndef __RT_USE_IO
  // Comment the following line out to disable all print-outs by the runtime.
  #define __RT_USE_IO
#endif

#if defined(__RT_USE_IO)

#include <stdio.h>
#include <stdlib.h>

#define RT_LOG_FATALS(x)    (x >= RT_LOG_FATAL)
#define RT_LOG_ERRORS(x)    (x >= RT_LOG_ERROR)
#define RT_LOG_WARNINGS(x)  (x >= RT_LOG_WARNING)
#define RT_LOG_INFOS(x)     (x >= RT_LOG_INFO)
#define RT_LOG_DEBUGS(x)    (x >= RT_LOG_DEBUG)
#define RT_LOG_TRACES(x)    (x >= RT_LOG_TRACE)

#define rt_msg(fmt, x...) \
  do { \
    printf("[\033[35mRT(%d,%d)\033[0m] " fmt, rt_cluster_id(), rt_core_id(), ##x); \
  } while(0)

#define rt_debug(fmt, x...) \
  do { \
    rt_msg("\033[36mDEBUG\033[0m: " fmt, ##x); \
  } while(0)

#define rt_info(fmt, x...) \
  do { \
    rt_msg("\033[34mINFO\033[0m: " fmt, ##x); \
  } while(0)

#define rt_error(fmt, x...) \
  do { \
    rt_msg("\033[31mERROR\033[0m: " fmt, ##x); \
  } while(0)

#define rt_fatal(msg...) \
  do { \
    rt_msg("\033[39mFATAL\033[0m: "msg); \
    abort(); \
  } while(0)

#else

#define rt_msg(x...) while(0)
#define rt_debug(x...) while(0)
#define rt_info(x...) while(0)
#define rt_error(x...) while(0)
#define rt_warning(x...) while(0)
#define rt_fatal(x...) do { abort(); } while(0)

#define RT_LOG_FATALS(x)    0
#define RT_LOG_ERRORS(x)    0
#define RT_LOG_WARNINGS(x)  0
#define RT_LOG_INFOS(x)     0
#define RT_LOG_DEBUGS(x)    0
#define RT_LOG_TRACES(x)    0

#endif


#if defined(__RT_USE_WARNING)

#define rt_warning(fmt, x...) \
  do { \
    if (rt_debug_config_warnings()) rt_msg("\033[33mWARNING\033[0m: " fmt, ##x); \
    if (rt_debug_config_werror()) abort(); \
  } while(0)

#else

#define rt_warning(x...) while(0)

#endif




#if defined(__RT_USE_ASSERT)

#define rt_assert(cond, msg...)           \
  if (!(cond)) {                          \
    rt_msg("\033[39mASSERT\033[0m: "msg); \
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

#define rt_trace(trace, x...) \
  do { \
    if (rt_debug_config_trace() & (1<<trace)) rt_msg(x); \
  } while(0)

#else

#define rt_trace(x...) \
  do { \
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
