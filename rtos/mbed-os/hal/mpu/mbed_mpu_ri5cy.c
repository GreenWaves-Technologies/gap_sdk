/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "hal/mpu_api.h"
#include "platform/mbed_assert.h"
#include "platform/mbed_error.h"
#include "cmsis.h"

#if ((!__RISCV_ARCH_GAP__)) && \
    defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U) && \
    !defined(MBED_MPU_CUSTOM)

#if !DEVICE_MPU
#error "Device has ri5cy MPU but it is not enabled. Add 'MPU' to device_has in targets.json"
#endif

#if !defined(MBED_MPU_ROM_END)
#define MBED_MPU_ROM_END             (0x10000000 - 1)
#endif
#define MBED_MPU_RAM_START           (MBED_MPU_ROM_END + 1)

MBED_STATIC_ASSERT(
    MBED_MPU_ROM_END == 0x04000000 - 1 ||
    MBED_MPU_ROM_END == 0x08000000 - 1 ||
    MBED_MPU_ROM_END == 0x0C000000 - 1 ||
    MBED_MPU_ROM_END == 0x10000000 - 1 ||
    MBED_MPU_ROM_END == 0x14000000 - 1 ||
    MBED_MPU_ROM_END == 0x18000000 - 1 ||
    MBED_MPU_ROM_END == 0x1C000000 - 1 ||
    MBED_MPU_ROM_END == 0x20000000 - 1,
    "Unsupported value for MBED_MPU_ROM_END");

void mbed_mpu_init()
{
}

void mbed_mpu_free()
{
}

void mbed_mpu_enable_rom_wn(bool enable)
{
}

void mbed_mpu_enable_ram_xn(bool enable)
{
}

#endif
