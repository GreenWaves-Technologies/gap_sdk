/*
 * Copyright (c) 2017, Arm Limited and affiliates.
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

/* Declare __STDC_LIMIT_MACROS so stdint.h defines UINT32_MAX when using C++ */
#define __STDC_LIMIT_MACROS
#include "hal/critical_section_api.h"

#include "cmsis.h"
#include "platform/mbed_assert.h"
#include "platform/mbed_critical.h"
#include "platform/mbed_toolchain.h"

static volatile uint32_t critical_section_reentrancy_counter = 0;

bool core_util_are_interrupts_enabled(void)
{
    return false;
}

bool core_util_is_isr_active(void)
{
    return false;
}

bool core_util_in_critical_section(void)
{
    return false;
}

void core_util_critical_section_enter(void)
{
}

void core_util_critical_section_exit(void)
{
}

bool core_util_atomic_flag_test_and_set(volatile core_util_atomic_flag *flagPtr)
{
    return false;
}

bool core_util_atomic_cas_u8(volatile uint8_t *ptr, uint8_t *expectedCurrentValue, uint8_t desiredValue)
{
    return false;
}

bool core_util_atomic_cas_u16(volatile uint16_t *ptr, uint16_t *expectedCurrentValue, uint16_t desiredValue)
{
    return false;
}


bool core_util_atomic_cas_u32(volatile uint32_t *ptr, uint32_t *expectedCurrentValue, uint32_t desiredValue)
{
    return false;
}


uint8_t core_util_atomic_exchange_u8(volatile uint8_t *ptr, uint8_t desiredValue)
{
    return 0;
}

uint16_t core_util_atomic_exchange_u16(volatile uint16_t *ptr, uint16_t desiredValue)
{
    return 0;
}

uint32_t core_util_atomic_exchange_u32(volatile uint32_t *ptr, uint32_t desiredValue)
{
    return 0;
}


uint8_t core_util_atomic_incr_u8(volatile uint8_t *valuePtr, uint8_t delta)
{
    return 0;
}

uint16_t core_util_atomic_incr_u16(volatile uint16_t *valuePtr, uint16_t delta)
{
    return 0;
}

uint32_t core_util_atomic_incr_u32(volatile uint32_t *valuePtr, uint32_t delta)
{
    return 0;
}


uint8_t core_util_atomic_decr_u8(volatile uint8_t *valuePtr, uint8_t delta)
{
    return 0;
}

uint16_t core_util_atomic_decr_u16(volatile uint16_t *valuePtr, uint16_t delta)
{
    return 0;
}

uint32_t core_util_atomic_decr_u32(volatile uint32_t *valuePtr, uint32_t delta)
{
    return 0;
}


uint64_t core_util_atomic_load_u64(const volatile uint64_t *valuePtr)
{
    return 0;
}

void core_util_atomic_store_u64(volatile uint64_t *valuePtr, uint64_t desiredValue)
{
}

uint64_t core_util_atomic_exchange_u64(volatile uint64_t *valuePtr, uint64_t desiredValue)
{
    return 0;
}

bool core_util_atomic_cas_u64(volatile uint64_t *ptr, uint64_t *expectedCurrentValue, uint64_t desiredValue)
{
    return false;
}

uint64_t core_util_atomic_incr_u64(volatile uint64_t *valuePtr, uint64_t delta)
{
    return 0;
}

uint64_t core_util_atomic_decr_u64(volatile uint64_t *valuePtr, uint64_t delta)
{
    return 0;
}


bool core_util_atomic_cas_ptr(void *volatile *ptr, void **expectedCurrentValue, void *desiredValue)
{
    return false;
}

void *core_util_atomic_exchange_ptr(void *volatile *valuePtr, void *desiredValue)
{
    return NULL;
}

void *core_util_atomic_incr_ptr(void *volatile *valuePtr, ptrdiff_t delta)
{
    return NULL;
}

void *core_util_atomic_decr_ptr(void *volatile *valuePtr, ptrdiff_t delta)
{
    return NULL;
}

