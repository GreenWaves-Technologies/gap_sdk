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

#include "port_api.h"

#if DEVICE_PORTIN || DEVICE_PORTOUT

#include "pinmap.h"
#include "gpio_api.h"

/* Array of GPIO peripheral base address. */
static GPIO_Type *const port_addrs[] = GPIO_BASE_PTRS;

PinName port_pin(PortName port, int pin_n)
{
    if(port == PortA)
        return GPIO_A[pin_n];

    return NC;
}

void port_init(port_t *obj, PortName port, int mask, PinDirection dir)
{
    obj->port = port;
    obj->mask = mask;

    // The function is set per pin: reuse gpio logic
    for (uint32_t i = 0; i < 32; i++) {
        if (obj->mask & (1 << i)) {
            gpio_set(port_pin(obj->port, i));
        }
    }

    port_dir(obj, dir);
}

void port_mode(port_t *obj, PinMode mode)
{
    // The mode is set per pin: reuse pinmap logic
    for (uint32_t i = 0; i < 32; i++) {
        if (obj->mask & (1 << i)) {
            pin_mode(port_pin(obj->port, i), mode);
        }
    }
}

void port_dir(port_t *obj, PinDirection dir)
{
    GPIO_Type *base = port_addrs[obj->port];
    uint32_t direction = base->DIR;

    switch (dir) {
        case PIN_INPUT :
            direction &= ~obj->mask;
            base->EN |= obj->mask;
            break;
        case PIN_OUTPUT:
            direction |= obj->mask;
            break;
    }
    base->DIR = direction;
}

void port_write(port_t *obj, int value)
{
    GPIO_Type *base = port_addrs[obj->port];
    uint32_t input = base->IN & ~obj->mask;

    base->OUT = (input | (uint32_t)(value & obj->mask));
}

int port_read(port_t *obj)
{
    GPIO_Type *base = port_addrs[obj->port];

    return (int)(base->IN & obj->mask);
}

#endif
