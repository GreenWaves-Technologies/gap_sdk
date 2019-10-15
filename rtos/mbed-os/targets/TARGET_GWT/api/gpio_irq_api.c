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

#include <stddef.h>
#include "cmsis.h"

#include "gpio_irq_api.h"

#if DEVICE_INTERRUPTIN

#include "gpio_api.h"
#include "gap_gpio.h"
#include "gap_port.h"
#include "mbed_error.h"

#define CHANNEL_NUM    32

static uint32_t channel_ids[CHANNEL_NUM] = {0};
static gpio_irq_handler irq_handler;
/* Array of PORT peripheral base address. */
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;

#define IRQ_FALLING_EDGE    (0)
#define IRQ_RAISING_EDGE    (1)
#define IRQ_EITHER_EDGE     (2)
#define IRQ_DISABLED        (0)

static void handle_interrupt_in(PortName port, int ch_base)
{
    uint32_t i;
    uint32_t interrupt_flags;
    GPIO_Type *gpio_base = gpio_addrs[port];

    interrupt_flags = GPIO_GetPinsInterruptFlags(gpio_base);

    for (i = 0; i < 32; i++) {
        if (interrupt_flags & (1 << i)) {
            uint32_t id = channel_ids[ch_base + i];
            if (id == 0) {
                continue;
            }

            gpio_irq_event event = IRQ_NONE;
            int reg_num = i >> 4;
            /* Positon in the target register */
            int pos = i & 0xF;

            switch ((gpio_base->INTCFG[reg_num] >> (pos << 1)) & GPIO_INTCFG_TYPE_BITS_WIDTH_MASK) {
                case IRQ_RAISING_EDGE:
                    event = IRQ_RISE;
                    break;

                case IRQ_FALLING_EDGE:
                    event = IRQ_FALL;
                    break;

                case IRQ_EITHER_EDGE:
                    event = (GPIO_ReadPinInput(gpio_base, i)) ? (IRQ_RISE) : (IRQ_FALL);
                    break;
            }
            if (event != IRQ_NONE) {
                irq_handler(id, event);
            }
        }
    }
    GPIO_ClearPinsInterruptFlags(gpio_base, interrupt_flags);
}

void gpio_irq(void)
{
    handle_interrupt_in(0, 0);
}

int gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id)
{
    if (pin == NC) {
        return -1;
    }

    irq_handler = handler;
    obj->pin = GET_GPIO_NUM(pin);

    GPIO_IRQHandlerBind(gpio_addrs[0], (uint32_t)gpio_irq);
    gpio_irq_enable(obj);

    obj->ch = obj->pin;
    channel_ids[obj->ch] = id;

    return 0;
}

void gpio_irq_free(gpio_irq_t *obj)
{
    channel_ids[obj->ch] = 0;
}

void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable)
{
    GPIO_Type *base = gpio_addrs[0];
    gpio_interrupt_t irq_settings = uGPIO_InterruptDisabled;

    if((base->INTEN & (1 << obj->pin)) == IRQ_DISABLED) {
        if (enable)
            irq_settings = (event == IRQ_RISE) ? (uGPIO_InterruptRisingEdge) : (uGPIO_InterruptFallingEdge);
    } else {
        int reg_num = obj->pin >> 4;
        /* Positon in the target register */
        int pos = obj->pin & 0xF;

        switch ((base->INTCFG[reg_num] >> (pos << 1)) & GPIO_INTCFG_TYPE_BITS_WIDTH_MASK) {
        case IRQ_RAISING_EDGE:
            if (enable) {
                irq_settings = (event == IRQ_RISE) ? (uGPIO_InterruptRisingEdge) : (uGPIO_InterruptEitherEdge);
            } else {
                if (event == IRQ_FALL)
                    irq_settings = uGPIO_InterruptRisingEdge;
            }
            break;

        case IRQ_FALLING_EDGE:
            if (enable) {
                irq_settings = (event == IRQ_FALL) ? (uGPIO_InterruptFallingEdge) : (uGPIO_InterruptEitherEdge);
            } else {
                if (event == IRQ_RISE)
                    irq_settings = uGPIO_InterruptFallingEdge;
            }
            break;

        case IRQ_EITHER_EDGE:
            if (enable) {
                irq_settings = uGPIO_InterruptEitherEdge;
            } else {
                irq_settings = (event == IRQ_RISE) ? (uGPIO_InterruptFallingEdge) : (uGPIO_InterruptRisingEdge);
            }
            break;
        }
    }

    GPIO_SetPinInterruptConfig(base, obj->pin, irq_settings);
}

void gpio_irq_enable(gpio_irq_t *obj)
{
    SOC_EU_SetFCMask(GPIO_EVENT);
}

void gpio_irq_disable(gpio_irq_t *obj)
{
    SOC_EU_ClearFCMask(GPIO_EVENT);
}

#endif
