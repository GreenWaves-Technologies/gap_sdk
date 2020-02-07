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

#define ARCHI_GPIO_NB_PADCFG_REG       ( 0x4 )
#define ARCHI_GPIO_GPIO_PER_CFG_LOG2   ( 3 ) /* Nb gpio is 2^ARCHI_GPIO_GPIO_PER_CFG_LOG2. */
#define ARCHI_GPIO_GPIO_POS_CFG        ( 0x7 )

/* Paddir register. */
static inline void hal_gpio_paddir_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->paddir), value);
}

static inline uint32_t hal_gpio_paddir_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->paddir));
}

/* Gpioen register. */
static inline void hal_gpio_gpioen_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->gpioen), value);
}

static inline uint32_t hal_gpio_gpioen_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->gpioen));
}

/* Padin register. */
static inline void hal_gpio_padin_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->padin), value);
}

static inline uint32_t hal_gpio_padin_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->padin));
}

/* Padout register. */
static inline void hal_gpio_padout_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->padout), value);
}

static inline uint32_t hal_gpio_padout_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->padout));
}

/* Padoutset register. */
static inline void hal_gpio_padoutset_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->padoutset), value);
}

static inline uint32_t hal_gpio_padoutset_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->padoutset));
}

/* Padoutclr register. */
static inline void hal_gpio_padoutclr_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->padoutclr), value);
}

static inline uint32_t hal_gpio_padoutclr_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->padoutclr));
}

/* Inten register. */
static inline void hal_gpio_inten_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->inten), value);
}

static inline uint32_t hal_gpio_inten_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->inten));
}

/* Inttype register. */
static inline void hal_gpio_inttype_set(uint8_t gpio_id, uint8_t int_reg, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->inttype[int_reg]), value);
}

static inline uint32_t hal_gpio_inttype_get(uint8_t gpio_id, uint8_t int_reg)
{
    return hal_read32(&(gpio(gpio_id)->inttype[int_reg]));
}

/* Intstatus register. */
static inline void hal_gpio_intstatus_set(uint8_t gpio_id, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->intstatus), value);
}

static inline uint32_t hal_gpio_intstatus_get(uint8_t gpio_id)
{
    return hal_read32(&(gpio(gpio_id)->intstatus));
}

/* Padcfg register. */
static inline void hal_gpio_padcfg_set(uint8_t gpio_id, uint8_t pad_reg, uint32_t value)
{
    hal_write32(&(gpio(gpio_id)->padcfg[pad_reg]), value);
}

static inline uint32_t hal_gpio_padcfg_get(uint8_t gpio_id, uint8_t pad_reg)
{
    return hal_read32(&(gpio(gpio_id)->padcfg[pad_reg]));
}


/*! Paddir. */
/*
 * Set direction mode of a GPIO pad.
 * 0 = Input
 * 1 = Output.
 */
static inline void hal_gpio_pin_direction_set(uint8_t gpio_id, uint8_t gpio_pin, uint8_t dir)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_paddir_get(gpio_id);
    val = ((val & ~(mask << shift)) | (dir << shift));
    hal_gpio_paddir_set(gpio_id, val);
}

static inline void hal_gpio_direction_set(uint8_t gpio_id, uint32_t gpio_mask, uint8_t dir)
{
    uint32_t val = hal_gpio_paddir_get(gpio_id);
    if (dir)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_paddir_set(gpio_id, val);
}


/*! Padin. */
/* GPIO pad input value. */
static inline uint32_t hal_gpio_pin_input_value_get(uint8_t gpio_id, uint8_t gpio_pin)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_padin_get(gpio_id);
    val = ((val >> shift) & mask);
    return val;
}

static inline uint32_t hal_gpio_input_value_get(uint8_t gpio_id)
{
    return hal_gpio_padin_get(gpio_id);
}


/*! Padout. */
/* GPIO pad output value. */
static inline void hal_gpio_pin_output_value_set(uint8_t gpio_id, uint8_t gpio_pin, uint8_t value)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_padout_get(gpio_id);
    val = ((val & ~(mask << shift)) | (value << shift));
    hal_gpio_padout_set(gpio_id, val);
}

static inline void hal_gpio_output_value_set(uint8_t gpio_id, uint32_t gpio_mask, uint8_t value)
{
    /*
      Test which one is better ?
      uint32_t set_mask = 0;
      value && (set_mask = gpio_mask);
      val = ((val & ~gpio_mask) | set_mask);
    */
    if (value)
    {
        hal_gpio_padoutset_set(gpio_id, gpio_mask);
    }
    else
    {
        hal_gpio_padoutclr_set(gpio_id, gpio_mask);
    }
}


/*! Inten. */
/* Enable GPIO pad interruption. */
static inline void hal_gpio_pin_irq_set(uint8_t gpio_id, uint8_t gpio_pin, uint8_t enable)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_inten_get(gpio_id);
    val = ((val & ~(mask << shift)) | (enable << shift));
    hal_gpio_inten_set(gpio_id, val);
}

static inline void hal_gpio_irq_set(uint8_t gpio_id, uint32_t gpio_mask, uint8_t enable)
{
    uint32_t val = hal_gpio_inten_get(gpio_id);
    if (enable)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_inten_set(gpio_id, val);
}


/*! Inttype. */
/* GPIO pad interruption type configuration. */
static inline void hal_gpio_pin_irq_type_set(uint8_t gpio_id, uint8_t gpio_pin, uint8_t irq_type)
{
    uint8_t gpio_reg = (gpio_pin >> 4), gpio_pos = (gpio_pin & 0x0F);
    uint32_t mask = 0x3, shift = (gpio_pos << 1);
    uint32_t val = hal_gpio_inttype_get(gpio_id, gpio_reg);
    val = ((val & ~(mask << shift)) | (irq_type << shift));
    hal_gpio_inttype_set(gpio_id, gpio_reg, val);
}


/*! Intstatus. */
/* GPIO interruption status. */
static inline uint32_t hal_gpio_pin_irq_status_get(uint8_t gpio_id, uint8_t gpio_pin)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_intstatus_get(gpio_id);
    val = ((val >> shift) & mask);
    return val;
}

static inline uint32_t hal_gpio_irq_status_get(uint8_t gpio_id)
{
    return hal_gpio_intstatus_get(gpio_id);
}


/*! Gpioen. */
/* Enable a GPIO pad. */
static inline void hal_gpio_pin_enable(uint8_t gpio_id, uint8_t gpio_pin, uint8_t enable)
{
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = hal_gpio_gpioen_get(gpio_id);
    val = ((val & ~(mask << shift)) | (enable << shift));
    hal_gpio_gpioen_set(gpio_id, val);
}

static inline void hal_gpio_enable(uint8_t gpio_id, uint32_t gpio_mask, uint8_t enable)
{
    uint32_t val = hal_gpio_gpioen_get(gpio_id);
    if (enable)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    hal_gpio_gpioen_set(gpio_id, val);
}


/*! Padcfg. */
/* Enable GPIO pad pull. */
static inline void hal_gpio_pin_pull_enable(uint8_t gpio_id, uint8_t gpio_pin, uint8_t pe)
{
    uint8_t gpio_reg = (gpio_pin >> 3), gpio_pos = (gpio_pin & 0x07);
    uint32_t mask = 0x1, shift = gpio_pos;
    uint32_t val = hal_gpio_padcfg_get(gpio_id, gpio_reg);
    val = ((val & ~(mask << shift)) | (pe << shift));
    hal_gpio_padcfg_set(gpio_id, gpio_reg, val);
}

/* Set GPIO pad drive strength. */
static inline void hal_gpio_pin_drive_strength_set(uint8_t gpio_id, uint8_t gpio_pin, uint8_t ds)
{
    uint8_t gpio_reg = (gpio_pin >> 3), gpio_pos = (gpio_pin & 0x07);
    uint32_t mask = 0x2, shift = gpio_pos;
    uint32_t val = hal_gpio_padcfg_get(gpio_id, gpio_reg);
    val = ((val & ~(mask << shift)) | ((ds << 1) << shift));
    hal_gpio_padcfg_set(gpio_id, gpio_reg, val);
}

/* GPIO pad pull activation & GPIO pad drive strength. */
static inline void hal_gpio_pin_configuration(uint8_t gpio_id, uint8_t gpio_pin, uint8_t pe, uint8_t ds)
{
    uint8_t gpio_reg = (gpio_pin >> 3), gpio_pos = (gpio_pin & 0x07);
    uint32_t mask = 0x3, shift = gpio_pos;
    uint32_t val = hal_gpio_padcfg_get(gpio_id, gpio_reg);
    val = ((val & ~(mask << shift)) | (((ds << 1) | pe) << shift));
    hal_gpio_padcfg_set(gpio_id, gpio_reg, val);
}

static inline void hal_gpio_configuration(uint8_t gpio_id, uint32_t gpio_mask, uint8_t pe, uint8_t ds)
{
    uint32_t mask = gpio_mask, pin = 0;
    while (mask)
    {
        if (mask & 0x1)
        {
	    hal_gpio_pin_configuration(gpio_id, pin, pe, ds);
        }
        mask = mask >> 1;
        pin++;
    }
}


#endif  /* __PI_GPIO_H__ */
