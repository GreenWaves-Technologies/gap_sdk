/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "mbed_assert.h"
#include "pinmap.h"
#include "mbed_error.h"
#include "gap_port.h"

/* Array of PORT peripheral base address. */
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;

void pin_function(PinName pin, int function)
{
    MBED_ASSERT(pin != (PinName)NC);
    int pin_num = (pin & 0xFF) - GAP_PIN_OFFSET;

    if (0<= pin_num && pin_num < GAP_PORT_PIN_NUM )
        PORT_SetPinMux(port_addrs[GET_GPIO_PORT(pin)], pin_num, (port_mux_t)function);
}

void pin_mode(PinName pin, PinMode mode)
{
    MBED_ASSERT(pin != (PinName)NC);

    uint32_t is_gpio = GET_IS_GPIO(pin);
    uint32_t instance = GET_GPIO_PORT(pin);

    if(is_gpio) {
        uint32_t gpio_num = GET_GPIO_NUM(pin);

        int reg_num = gpio_num >> 2;
        int pos = (gpio_num & 0x3) << 3;

        GPIO_Type *base = gpio_addrs[instance];
        switch (mode) {
        case PullNone:
            /* Write 0 to the PullUp and PullDown Enable bits */
#if defined(__GAP8__)
            base->PADCFG[reg_num] &= ~(1U << pos);
            break;
        case PullUp:
            /* Write 1 to the PullUp Enable bits */
            base->PADCFG[reg_num] |= (1U << pos);
            break;
#elif defined(__GAP9__)
            base->PADCFG[reg_num] &= ~(3U << pos);
            break;
        case PullDown:
            /* Write 1 to the PD Enable bits */
            base->PADCFG[reg_num] |= (1U << pos);
            break;
        case PullUp:
            /* Write 1 to the PU Enable bits */
            base->PADCFG[reg_num] |= (2U << pos);
            break;
#endif
        default:
            break;
        }
    } else {
        int pin_num = (pin & 0xFF) - GAP_PIN_OFFSET;

        if (0 <= pin_num && pin_num < GAP_PORT_PIN_NUM ) {
            int reg_num = pin_num >> 2;
            int pos = (pin_num & 0x3) << 3;

            PORT_Type *base = port_addrs[instance];

            switch (mode) {
            case PullNone:
                /* Write 0 to the PullUp and PullDown Enable bits */
#if defined(__GAP8__)
                base->PADCFG[reg_num] &= ~(1U << pos);
                break;
            case PullUp:
                /* Write 1 to the PullUp Enable bits */
                base->PADCFG[reg_num] |= (1U << pos);
                break;
#elif defined(__GAP9__)
                base->PADCFG[reg_num] &= ~(3U << pos);
                break;
            case PullDown:
                /* Write 1 to the PD Enable bits */
                base->PADCFG[reg_num] |= (1U << pos);
                break;
            case PullUp:
                /* Write 1 to the PU Enable bits */
                base->PADCFG[reg_num] |= (2U << pos);
                break;
#endif
            default:
                break;
            }
        }
    }
}
