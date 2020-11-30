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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_IMPLEM_TRACE_H__
#define __POS_IMPLEM_TRACE_H__

#include <stdlib.h>
#include <stdio.h>

#define POS_LOG_NONE     0   // No trace
#define POS_LOG_FATAL    1   // Fatal condition, the system should terminate execution ASAP
#define POS_LOG_ERROR    2   // Error condition, might require manual intervention
#define POS_LOG_WARNING  3   // Unusual condition, not serious by itself but might indicate problems
#define POS_LOG_INFO     4   // Information on the status of the system
#define POS_LOG_DEBUG    5   // Debug messages
#define POS_LOG_TRACE    6   // Very fine-grained debug and status messages

#define POS_TRACE_INIT      0

#if !defined(__TRACE_LEVEL__) || __TRACE_LEVEL__ == 0

#define POS_MSG(x...) while(0)
#define POS_FATAL(x...) while(0)
#define POS_ERROR(x...) while(0)
#define POS_WARNING(x...) while(0)
#define POS_INFO(x...) while(0)
#define POS_DEBUG(x...) while(0)
#define POS_TRACE(x...) while(0)

#else


#ifdef CONFIG_WERROR
#define WARNING_ABORT() abort()
#else
#define WARNING_ABORT()
#endif

#define POS_MSG(fmt, x...) \
  do { \
    printf("[\033[35mPOS(%d,%d)\033[0m] " fmt, hal_cluster_id(), hal_core_id(), ##x); \
  } while(0)

#define POS_FATAL(fmt, x...) \
  do { \
    printf("[\033[33FATAL:POS(%d,%d)\033[0m] " fmt, hal_cluster_id(), hal_core_id(), ##x); \
    abort(); \
  } while(0)

#define POS_ERROR(fmt, x...) \
  do { \
    printf("[\033[33mERROR:POS(%d,%d)\033[0m] " fmt, hal_cluster_id(), hal_core_id(), ##x); \
    abort(); \
  } while(0)

#define POS_WARNING(fmt, x...) \
  do { \
    printf("[\033[33mWARNING:POS(%d,%d)\033[0m] " fmt, hal_cluster_id(), hal_core_id(), ##x); \
    WARNING_ABORT(); \
  } while(0)

#define POS_INFO(x...) \
    do { \
        if (__TRACE_LEVEL__ >= POS_LOG_INFO) POS_MSG(x); \
    } while(0)

#define POS_DEBUG(x...) \
    do { \
        if (__TRACE_LEVEL__ >= POS_LOG_DEBUG) POS_MSG(x); \
    } while(0)

#define POS_TRACE(level, x...) \
    do { \
        if (level <= __TRACE_LEVEL__) { \
            if (level >= POS_LOG_INFO) POS_MSG(x); \
            else if (level == POS_LOG_WARNING) POS_WARNING(x); \
            else if (level == POS_LOG_ERROR) POS_ERROR(x); \
            else POS_FATAL(x); \
        } \
    } while(0)

#endif




#if !defined(__TRACE_ALL__) && !defined(__TRACE_FREQ__)
#define FREQ_TRACE(x...)
#else
#define FREQ_TRACE(level, x...) POS_TRACE(level, "[FREQ] " x)
#endif


#if !defined(__TRACE_ALL__) && !defined(__TRACE_INIT__)
#define INIT_TRACE(x...)
#else
#define INIT_TRACE(level, x...) POS_TRACE(level, "[INIT] " x)
#endif


#if !defined(__TRACE_ALL__) && !defined(__TRACE_CL__)
#define CL_TRACE(x...)
#else
#define CL_TRACE(level, x...) POS_TRACE(level, "[CL] " x)
#endif


#if !defined(__TRACE_ALL__) && !defined(__TRACE_UART__)
#define UART_TRACE(x...)
#else
#define UART_TRACE(level, x...) POS_TRACE(level, "[UART] " x)
#endif


#if !defined(__TRACE_ALL__) && !defined(__TRACE_ALLOC__)
#define ALLOC_TRACE(x...)
#else
#define ALLOC_TRACE(level, x...) POS_TRACE(level, "[ALLOC] " x)
#endif


#if !defined(__TRACE_ALL__) && !defined(__TRACE_UDMA__)
#define UDMA_TRACE(x...)
#else
#define UDMA_TRACE(level, x...) POS_TRACE(level, "[UDMA] " x)
#endif


#endif