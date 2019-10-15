/* mbed Microcontroller Library
 * Copyright (c) 2006-2016 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "platform/mbed_mem_trace.h"
#include "platform/mbed_critical.h"
#include "platform/SingletonPtr.h"
#include "platform/PlatformMutex.h"

/******************************************************************************
 * Internal variables, functions and helpers
 *****************************************************************************/

/* The callback function that will be called after a traced memory operations finishes. */
static mbed_mem_trace_cb_t mem_trace_cb;
static mbed_mem_trace_cb_t mem_trace_cb_reserve;
/* 'trace_lock_count' guards "trace inside trace" situations (for example, the implementation
 * of realloc() might call malloc() internally, and since malloc() is also traced, this could
 * result in two calls to the callback function instead of one. */
static uint8_t trace_lock_count;
static SingletonPtr<PlatformMutex> mem_trace_mutex;

#define TRACE_FIRST_LOCK() (trace_lock_count < 2)


/******************************************************************************
 * Public interface
 *****************************************************************************/

void mbed_mem_trace_set_callback(mbed_mem_trace_cb_t cb)
{
    mem_trace_cb = cb;
}

void mbed_mem_trace_disable()
{
    mbed_mem_trace_lock();
    if (mem_trace_cb) {
        mem_trace_cb_reserve = mem_trace_cb;
        mem_trace_cb = 0;
    }
    mbed_mem_trace_unlock();
}
void mbed_mem_trace_enable()
{
    mbed_mem_trace_lock();
    if (!mem_trace_cb && mem_trace_cb_reserve) {
        mem_trace_cb = mem_trace_cb_reserve;
    }
    mbed_mem_trace_unlock();
}

void mbed_mem_trace_lock()
{
    mem_trace_mutex->lock();
    trace_lock_count++;
}

void mbed_mem_trace_unlock()
{
    trace_lock_count--;
    mem_trace_mutex->unlock();
}

void *mbed_mem_trace_malloc(void *res, size_t size, void *caller)
{
    if (mem_trace_cb) {
        if (TRACE_FIRST_LOCK()) {
            mem_trace_cb(MBED_MEM_TRACE_MALLOC, res, caller, size);
        }
    }
    return res;
}

void *mbed_mem_trace_realloc(void *res, void *ptr, size_t size, void *caller)
{
    if (mem_trace_cb) {
        if (TRACE_FIRST_LOCK()) {
            mem_trace_cb(MBED_MEM_TRACE_REALLOC, res, caller, ptr, size);
        }
    }
    return res;
}

void *mbed_mem_trace_calloc(void *res, size_t num, size_t size, void *caller)
{
    if (mem_trace_cb) {
        if (TRACE_FIRST_LOCK()) {
            mem_trace_cb(MBED_MEM_TRACE_CALLOC, res, caller, num, size);
        }
    }
    return res;
}

void mbed_mem_trace_free(void *ptr, void *caller)
{
    if (mem_trace_cb) {
        if (TRACE_FIRST_LOCK()) {
            mem_trace_cb(MBED_MEM_TRACE_FREE, NULL, caller, ptr);
        }
    }
}

void mbed_mem_trace_default_callback(uint8_t op, void *res, void *caller, ...)
{
    va_list va;
    size_t temp_s1, temp_s2;
    void *temp_ptr;

    va_start(va, caller);
    switch (op) {
        case MBED_MEM_TRACE_MALLOC:
            temp_s1 = va_arg(va, size_t);
            printf(MBED_MEM_DEFAULT_TRACER_PREFIX "m:%p;%p-%u\n", res, caller, temp_s1);
            break;

        case MBED_MEM_TRACE_REALLOC:
            temp_ptr = va_arg(va, void *);
            temp_s1 = va_arg(va, size_t);
            printf(MBED_MEM_DEFAULT_TRACER_PREFIX "r:%p;%p-%p;%u\n", res, caller, temp_ptr, temp_s1);
            break;

        case MBED_MEM_TRACE_CALLOC:
            temp_s1 = va_arg(va, size_t);
            temp_s2 = va_arg(va, size_t);
            printf(MBED_MEM_DEFAULT_TRACER_PREFIX "c:%p;%p-%u;%u\n", res, caller, temp_s1, temp_s2);
            break;

        case MBED_MEM_TRACE_FREE:
            temp_ptr = va_arg(va, void *);
            printf(MBED_MEM_DEFAULT_TRACER_PREFIX "f:%p;%p-%p\n", res, caller, temp_ptr);
            break;

        default:
            printf("?\n");
    }
    va_end(va);
}

