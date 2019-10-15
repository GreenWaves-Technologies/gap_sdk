/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "greentea-client/test_env.h"
#include "greentea-client/greentea_metrics.h"
#include "platform/mbed_stats.h"
#include <stdint.h>

#define THREAD_BUF_COUNT    16

typedef struct {
    uint32_t entry;
    uint32_t stack_size;
    uint32_t max_stack;
} thread_info_t;

#if defined(MBED_STACK_STATS_ENABLED) && MBED_STACK_STATS_ENABLED

#if !defined(MBED_CONF_RTOS_PRESENT) || !(MBED_CONF_RTOS_PRESENT)
#error "RTOS required for Stack stats"
#endif

#include "rtos/Mutex.h"
#include "rtos/Thread.h"
#include "rtos/Kernel.h"
#include "mbed_stats.h"
#include "cmsis_os2.h"
#include "platform/SingletonPtr.h"
#include "platform/CircularBuffer.h"
using namespace mbed;
using namespace rtos;

// Mutex to protect "buf"
static SingletonPtr<Mutex> mutex;
static char buf[128];
static SingletonPtr<CircularBuffer<thread_info_t, THREAD_BUF_COUNT> > queue;
#endif

#if defined(MBED_CPU_STATS_ENABLED)
static void send_CPU_info(void);
#endif

#if defined(MBED_HEAP_STATS_ENABLED ) && MBED_HEAP_STATS_ENABLED
static void send_heap_info(void);
#endif
#if defined(MBED_STACK_STATS_ENABLED) && MBED_STACK_STATS_ENABLED
static void send_stack_info(void);
static void on_thread_terminate(osThreadId_t id);
static void enqeue_thread_info(osThreadId_t id);
static void deque_and_print_thread_info(void);

// sprintf uses a lot of stack so use these instead
static uint32_t print_hex(char *buf, uint32_t value);
static uint32_t print_dec(char *buf, uint32_t value);
#endif

void greentea_metrics_setup()
{
#if defined(MBED_STACK_STATS_ENABLED) && MBED_STACK_STATS_ENABLED
    Kernel::attach_thread_terminate_hook(on_thread_terminate);
#endif
}

void greentea_metrics_report()
{
#if defined(MBED_HEAP_STATS_ENABLED ) && MBED_HEAP_STATS_ENABLED
    send_heap_info();
#endif
#if defined(MBED_STACK_STATS_ENABLED) && MBED_STACK_STATS_ENABLED
    send_stack_info();
    Kernel::attach_thread_terminate_hook(NULL);
#endif
#if defined(MBED_CPU_STATS_ENABLED)
    send_CPU_info();
#endif
}

#if defined(MBED_CPU_STATS_ENABLED)
static void send_CPU_info()
{
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    greentea_send_kv("__cpu_info        up time", stats.uptime);
    greentea_send_kv("__cpu_info     sleep time", stats.sleep_time);
    greentea_send_kv("__cpu_info deepsleep time", stats.deep_sleep_time);
    greentea_send_kv("__cpu_info  %  sleep/deep", (stats.sleep_time * 100) / stats.uptime, (stats.deep_sleep_time * 100) / stats.uptime);
}
#endif

#if defined(MBED_HEAP_STATS_ENABLED ) && MBED_HEAP_STATS_ENABLED
static void send_heap_info()
{
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    greentea_send_kv("max_heap_usage",heap_stats.max_size);
    greentea_send_kv("reserved_heap",heap_stats.reserved_size);
}
#endif

#if defined(MBED_STACK_STATS_ENABLED) && MBED_STACK_STATS_ENABLED
MBED_UNUSED static void send_stack_info()
{
    mutex->lock();

    // Flush any queued stack entries
    while (!queue->empty()) {
        deque_and_print_thread_info();
    }

    // Print info for all other threads
    uint32_t thread_n = osThreadGetCount();
    osThreadId_t *threads = new (std::nothrow) osThreadId_t[thread_n];
    // Don't fail on lack of memory
    if (!threads) {
        goto end;
    }
    thread_n = osThreadEnumerate(threads, thread_n);

    for(size_t i = 0; i < thread_n; i++) {
        enqeue_thread_info(threads[i]);
        deque_and_print_thread_info();
    }

    delete[] threads;

end:
    mutex->unlock();
}

MBED_UNUSED static void on_thread_terminate(osThreadId_t id)
{
    mutex->lock();

    // There should always be space in the queue
    enqeue_thread_info(id);

    // If queue is full then print out a single entry
    if (queue->full()) {
        deque_and_print_thread_info();
    }

    mutex->unlock();
}

static void enqeue_thread_info(osThreadId_t id)
{
    thread_info_t thread_info = {};

    thread_info.entry = (uint32_t)id;
    thread_info.stack_size = osThreadGetStackSize(id);
    thread_info.max_stack = thread_info.stack_size - osThreadGetStackSpace(id);
    queue->push(thread_info);
}

static void deque_and_print_thread_info()
{
    thread_info_t thread_info;
    bool ret = queue->pop(thread_info);
    MBED_ASSERT(ret);
    uint32_t pos = 0;
    buf[pos++] = '\"';
    pos += print_hex(buf + pos, thread_info.entry);
    buf[pos++] = '\"';
    buf[pos++] = ',';
    pos += print_dec(buf + pos, thread_info.max_stack);
    buf[pos++] = ',';
    pos += print_dec(buf + pos, thread_info.stack_size);
    buf[pos++] = 0;
    greentea_send_kv("__thread_info", buf);
}

static uint32_t print_hex(char *buf, uint32_t value)
{
    uint32_t pos = 0;
    buf[pos] = '0';
    pos++;
    buf[pos] = 'x';
    pos++;
    for (int i = 8; i >= 0; i--) {
        uint32_t val = (value >> (4 * i)) & 0xF;
        if (val <= 9) {
            buf[pos] = '0' + val;
            pos++;
        } else {
            buf[pos] = 'a' + val - 10;
            pos++;
        }
    }
    return pos;
}

static uint32_t print_dec(char *buf, uint32_t value)
{
    uint32_t pos = 0;

    // The value 0 is special case
    if (0 == value) {
        buf[pos] = '0';
        pos++;
        return pos;
    }

    // Write out value in reverse order
    while (value != 0) {
        uint32_t next = value / 10;
        buf[pos] = '0' + (value - next * 10);
        value = next;
        pos++;
    }

    // Reverse order
    for (uint32_t i = 0; i < pos / 2; i++) {
        char temp = buf[i];
        buf[i] = buf[pos - 1 - i];
        buf[pos - 1 - i] = temp;
    }

    return pos;
}

#endif
