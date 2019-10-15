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

#include "mbed_assert.h"
#include "gpio_api.h"
#include "pinmap.h"
#include "gap_port.h"
#include "gap_gpio.h"


static GPIO_Type * const gpio_addrs[] = GPIO_BASE_PTRS;

uint32_t gpio_set(PinName pin)
{
    MBED_ASSERT(pin != (PinName)NC);
    uint32_t pin_num = pin & 0xFF;

    pin_function(pin, (int)uPORT_MuxAlt1);
    return 1 << pin_num;
}

void gpio_init(gpio_t *obj, PinName pin)
{
    obj->pin = pin;
    if (pin == (PinName)NC)
        return;

    pin_function(pin, (int)uPORT_MuxAlt1);
}

void gpio_mode(gpio_t *obj, PinMode mode)
{
    pin_mode(obj->pin, mode);
}

void gpio_dir(gpio_t *obj, PinDirection direction)
{
    MBED_ASSERT(obj->pin != (PinName)NC);
    uint32_t port = GET_GPIO_PORT(obj->pin);
    uint32_t pin = GET_GPIO_NUM(obj->pin);
    GPIO_Type *base = gpio_addrs[port];

    /* Clock Gating enable for read */
    base->EN  |= (1U << pin);

#if defined(__GAP8__)
    switch (direction) {
        case PIN_INPUT:
            base->DIR &= ~(1U << pin);
            break;
        case PIN_OUTPUT:
            base->DIR |= (1U << pin);
            break;
    }
#elif defined(__GAP9__)
    switch (direction) {
        case PIN_OUTPUT:
            base->DIR &= ~(1U << pin);
            break;
        case PIN_INPUT:
            base->DIR |= (1U << pin);
            break;
    }
#endif
}

void gpio_write(gpio_t *obj, int value)
{
    MBED_ASSERT(obj->pin != (PinName)NC);
    uint32_t port = GET_GPIO_PORT(obj->pin);
    uint32_t pin = GET_GPIO_NUM(obj->pin);

    GPIO_WritePinOutput(gpio_addrs[port], pin, value);
}

int gpio_read(gpio_t *obj)
{
    MBED_ASSERT(obj->pin != (PinName)NC);
    uint32_t port = GET_GPIO_PORT(obj->pin);
    uint32_t pin = GET_GPIO_NUM(obj->pin);

    return (int)GPIO_ReadPinInput(gpio_addrs[port], pin);
}
