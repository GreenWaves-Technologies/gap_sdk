/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#ifndef __PI_GPIO_H__
#define __PI_GPIO_H__

#include "targets/pmsis_targets.h"

#define ARCHI_GPIO_NB_PADCFG_REG       (0x8)

/* Paddir register. */
static inline void hal_gpio_paddir_set(uint32_t value)
{
    hal_write32(&(gpio(0)->paddir), value);
}

static inline uint32_t hal_gpio_paddir_get()
{
    return hal_read32(&(gpio(0)->paddir));
}

/* Padin register. */
static inline void hal_gpio_padin_set(uint32_t value)
{
    hal_write32(&(gpio(0)->padin), value);
}

static inline uint32_t hal_gpio_padin_get()
{
    return hal_read32(&(gpio(0)->padin));
}

/* Padout register. */
static inline void hal_gpio_padout_set(uint32_t value)
{
    hal_write32(&(gpio(0)->padout), value);
}

static inline uint32_t hal_gpio_padout_get()
{
    return hal_read32(&(gpio(0)->padout));
}

/* Inten register. */
static inline void hal_gpio_inten_set(uint32_t value)
{
    hal_write32(&(gpio(0)->inten), value);
}

static inline uint32_t hal_gpio_inten_get()
{
    return hal_read32(&(gpio(0)->inten));
}

/* Inttype register. */
static inline void hal_gpio_inttype_set(uint8_t int_reg, uint32_t value)
{
    hal_write32(&(gpio(0)->inttype[int_reg]), value);
}

static inline uint32_t hal_gpio_inttype_get(uint8_t int_reg)
{
    return hal_read32(&(gpio(0)->inttype[int_reg]));
}

/* Intstatus register. */
static inline void hal_gpio_intstatus_set(uint32_t value)
{
    hal_write32(&(gpio(0)->intstatus), value);
}

static inline uint32_t hal_gpio_intstatus_get()
{
    return hal_read32(&(gpio(0)->intstatus));
}

/* Gpioen register. */
static inline void hal_gpio_gpioen_set(uint32_t value)
{
    hal_write32(&(gpio(0)->gpioen), value);
}

static inline uint32_t hal_gpio_gpioen_get()
{
    return hal_read32(&(gpio(0)->gpioen));
}

/* Padcfg register. */
static inline void hal_gpio_padcfg_set(uint8_t int_reg, uint32_t value)
{
    hal_write32(&(gpio(0)->padcfg[int_reg]), value);
}

static inline uint32_t hal_gpio_padcfg_get(uint8_t int_reg)
{
    return hal_read32(&(gpio(0)->padcfg[int_reg]));
}


/*! Paddir. */
/*
 * Set direction mode of a GPIO pad.
 * 0 = Input | 1 = Output.
 */
static inline void hal_gpio_pin_set_direction(uint8_t gpio_pin, uint8_t dir)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_paddir_get();
    val = ((val & ~(mask << shift)) | (dir << shift));
    hal_gpio_paddir_set(val);
}

static inline void hal_gpio_set_direction(uint32_t gpio_mask, uint8_t dir)
{
    uint32_t val = hal_gpio_paddir_get();
    if (dir)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_paddir_set(val);
}


/*! Padin. */
/* GPIO pad input value. */
static inline uint32_t hal_gpio_pin_get_input_value(uint8_t gpio_pin)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_padin_get();
    val = ((val >> shift) & mask);
    return val;
}

static inline uint32_t hal_gpio_get_input_value()
{
    return hal_gpio_padin_get();
}


/*! Padout. */
/* GPIO pad output value. */
static inline void hal_gpio_pin_set_output_value(uint8_t gpio_pin, uint8_t value)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_padout_get();
    val = ((val & ~(mask << shift)) | (value << shift));
    hal_gpio_padout_set(val);
}

static inline void hal_gpio_set_output_value(uint32_t gpio_mask, uint8_t value)
{
    uint32_t val = hal_gpio_padout_get();
    /*
      Test which one is better ?
      uint32_t set_mask = 0;
      value && (set_mask = gpio_mask);
      val = ((val & ~gpio_mask) | set_mask);
    */
    if (value)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_padout_set(val);
}


/*! Inten. */
/* Enable GPIO pad interruption. */
static inline void hal_gpio_pin_set_irq(uint8_t gpio_pin, uint8_t enable)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_inten_get();
    val = ((val & ~(mask << shift)) | (enable << shift));
    hal_gpio_inten_set(val);
}

static inline void hal_gpio_set_irq(uint32_t gpio_mask, uint8_t enable)
{
    uint32_t val = hal_gpio_inten_get();
    if (enable)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_inten_set(val);
}


/*! Inttype. */
/* GPIO pad interruption type configuration. */
static inline void hal_gpio_pin_set_irq_type(uint8_t gpio_pin, uint8_t irq_type)
{
    uint8_t gpio_reg = (gpio_pin >> 4), gpio_pos = (gpio_pin & 0x0F);
    uint32_t mask = 0x3, shift = (gpio_pos << 1);
    uint32_t val = hal_gpio_inttype_get(gpio_reg);
    val = ((val & ~(mask << shift)) | (irq_type << shift));
    hal_gpio_inttype_set(gpio_reg, val);
}


/*! Intstatus. */
/* GPIO interruption status. */
static inline uint32_t hal_gpio_pin_get_irq_status(uint8_t gpio_pin)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_intstatus_get();
    val = ((val >> shift) & mask);
    return val;
}

static inline uint32_t hal_gpio_get_irq_status()
{
    return hal_gpio_intstatus_get();
}


/*! Gpioen. */
/* Enable a GPIO pad. */
static inline void hal_gpio_pin_enable(uint8_t gpio_pin, uint8_t enable)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_gpioen_get();
    val = ((val & ~(mask << shift)) | (enable << shift));
    hal_gpio_gpioen_set(val);
}

static inline void hal_gpio_enable(uint32_t gpio_mask, uint8_t enable)
{
    uint32_t val = hal_gpio_gpioen_get();
    if (enable)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_gpioen_set(val);
}


/*! Padcfg. */
/* Enable GPIO pad pull. */
static inline void hal_gpio_pin_enable_pull(uint8_t gpio_pin, uint8_t pe)
{
    uint8_t gpio_reg = (gpio_pin >> 2), gpio_pos = (gpio_pin & 0x03);
    uint32_t mask = 0x1, shift = (gpio_pos << 3);
    uint32_t val = hal_gpio_padcfg_get(gpio_reg);
    val = ((val & ~(mask << shift)) | (pe << shift));
    hal_gpio_padcfg_set(gpio_reg, val);
}

/* Set GPIO pad drive strength. */
static inline void hal_gpio_pin_set_drive_strength(uint8_t gpio_pin, uint8_t ds)
{
    uint8_t gpio_reg = (gpio_pin >> 2), gpio_pos = (gpio_pin & 0x03);
    uint32_t mask = 0x2, shift = (gpio_pos << 3);
    uint32_t val = hal_gpio_padcfg_get(gpio_reg);
    val = ((val & ~(mask << shift)) | ((ds << 1) << shift));
    hal_gpio_padcfg_set(gpio_reg, val);
}

/* GPIO pad pull activation & GPIO pad drive strength. */
static inline void hal_gpio_pin_configuration(uint8_t gpio_pin, uint8_t pe, uint8_t ds)
{
    uint8_t gpio_reg = (gpio_pin >> 2), gpio_pos = (gpio_pin & 0x03);
    uint32_t mask = 0x3, shift = (gpio_pos << 3);
    uint32_t val = hal_gpio_padcfg_get(gpio_reg);
    val = ((val & ~(mask << shift)) | (((ds << 1) | pe) << shift));
    hal_gpio_padcfg_set(gpio_reg, val);
}

static inline void hal_gpio_configuration(uint32_t gpio_mask, uint8_t pe, uint8_t ds)
{
    uint32_t mask = gpio_mask, pin = 0;
    while (mask)
    {
        if (mask & 0x1)
        {
            hal_gpio_pin_configuration(pin, pe, ds);
        }
        mask = mask >> 1;
        pin++;
    }
}


#endif  /* __PI_GPIO_H__ */
