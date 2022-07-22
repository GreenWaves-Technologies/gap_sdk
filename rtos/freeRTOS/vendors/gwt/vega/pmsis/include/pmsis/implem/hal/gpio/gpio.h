/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_HAL_GPIO_GPIO_H__
#define __PMSIS_IMPLEM_HAL_GPIO_GPIO_H__

#include "pmsis/targets/target.h"

#define ARCHI_GPIO_NB_DEVICE               ( 0x2 )
#define ARCHI_GPIO_DEVICE_SIZE             ( 0x38 )
//#define ARCHI_GPIO_NB_GPIO_PER_DEVICE      ( ARCHI_NB_GPIO / ARCHI_GPIO_NB_DEVICE )
#define ARCHI_GPIO_NB_GPIO_PER_DEVICE      ( 0x20 )
#define ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK ( ARCHI_GPIO_NB_GPIO_PER_DEVICE - 1 )
#define ARCHI_GPIO_DEVICE_ID(gpio)         ( ((gpio) & 0x60) >> 5 )
#define ARCHI_GPIO_DEVICE_OFFSET(id)       ( (id) * ARCHI_GPIO_DEVICE_SIZE )

#define ARCHI_GPIO_NB_PADCFG_REG           ( 0x4 )
#define ARCHI_GPIO_NB_PADCFG_REG_LOG2      ( 0x2 )
#if 1
#define ARCHI_GPIO_NB_GPIO_PER_PADCFG      ( ARCHI_GPIO_NB_GPIO_PER_DEVICE >> ARCHI_GPIO_NB_PADCFG_REG_LOG2 )
#define ARCHI_GPIO_PADCFG_REG_OFFSET(gpio) ( (gpio) / ARCHI_GPIO_NB_GPIO_PER_PADCFG )
#define ARCHI_GPIO_PADCFG_POS_OFFSET(gpio) ( (gpio) % ARCHI_GPIO_NB_GPIO_PER_PADCFG )
#else
//#define ARCHI_GPIO_DEVICE_OFFSET(gpio)     ( (gpio) / ARCHI_GPIO_NB_DEVICE )
#define ARCHI_GPIO_POS_DEVICE(gpio)        ( (gpio) & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK )

#define ARCHI_GPIO_NB_GPIO_PER_PADCFG      ( ARCHI_GPIO_NB_GPIO_PER_DEVICE >> ARCHI_GPIO_NB_PADCFG_REG_LOG2 )
#define ARCHI_GPIO_PADCFG_REG_OFFSET(gpio) ( ARCHI_GPIO_POS_DEVICE(gpio) / ARCHI_GPIO_NB_GPIO_PER_PADCFG )
#define ARCHI_GPIO_PADCFG_POS_OFFSET(gpio) ( ARCHI_GPIO_POS_DEVICE(gpio) % ARCHI_GPIO_NB_GPIO_PER_PADCFG )
#endif

/** PADDIR Register. */
static inline uint32_t gpio_paddir_get(uint32_t base, uint32_t dev_offset)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADDIR_00_31_OFFSET + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_paddir_set(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADDIR_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** GPIOEN Register. */
static inline uint32_t gpio_gpioen_get(uint32_t base, uint32_t dev_offset)
{
    uint32_t reg_offset = (uint32_t) GPIO_GPIOEN_00_31_OFFSET + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_gpioen_set(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_GPIOEN_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** PADIN Register. */
static inline uint32_t gpio_padin_get(uint32_t base, uint32_t dev_offset)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADIN_00_31_OFFSET + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_padin_set(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADIN_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** PADOUT Register. */
static inline uint32_t gpio_padout_get(uint32_t base, uint32_t dev_offset)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADOUT_00_31_OFFSET + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_padout_set(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADOUTSET_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}

static inline void gpio_padout_clear(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADOUTCLR_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** INTEN Register. */
static inline uint32_t gpio_inten_get(uint32_t base, uint32_t dev_offset)
{
    uint32_t reg_offset = (uint32_t) GPIO_INTEN_00_31_OFFSET + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_inten_set(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_INTEN_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** INTTYPE Register. */
/* TODO : Check inttype offset. */
static inline uint32_t gpio_inttype_get(uint32_t base, uint32_t dev_offset,
                                        uint32_t reg_id)
{
    uint32_t reg_offset = (uint32_t) GPIO_INTTYPE_00_15_OFFSET + (reg_id << 2) + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_inttype_set(uint32_t base, uint32_t dev_offset,
                                    uint32_t reg_id, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_INTTYPE_00_15_OFFSET + (reg_id << 2) + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** INTSTATUS Register. */
static inline uint32_t gpio_intstatus_get(uint32_t base, uint32_t dev_offset)
{
    uint32_t reg_offset = (uint32_t) GPIO_INTSTATUS_00_31_OFFSET + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_intstatus_set(uint32_t base, uint32_t dev_offset, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_INTSTATUS_00_31_OFFSET + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}


/** PADCFG Register. */
static inline uint32_t gpio_padcfg_get(uint32_t base, uint32_t dev_offset,
                                       uint32_t reg_id)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADCFG_00_07_OFFSET + (reg_id << 2) + dev_offset;
    return GAP_READ(base, reg_offset);
}

static inline void gpio_padcfg_set(uint32_t base, uint32_t dev_offset,
                                   uint32_t reg_id, uint32_t value)
{
    uint32_t reg_offset = (uint32_t) GPIO_PADCFG_00_07_OFFSET + (reg_id << 2) + dev_offset;
    GAP_WRITE(base, reg_offset, value);
}



/*! Paddir. */
/**
 * Set direction mode of a GPIO pad.
 * 0 = Input
 * 1 = Output.
 */
static inline void hal_gpio_pin_direction_set(uint32_t device_id, uint8_t gpio_pin,
                                              uint8_t dir)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = gpio_paddir_get(base, dev_offset);
    val &= ~(mask << shift);
    val |= (dir << shift);
    gpio_paddir_set(base, dev_offset, val);
}

static inline void hal_gpio_direction_set(uint32_t device_id, uint32_t gpio_mask,
                                          uint8_t dir)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t val = gpio_paddir_get(base, dev_offset);
    if (dir)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    gpio_paddir_set(base, dev_offset, val);
}

static inline uint32_t hal_gpio_direction_get(uint32_t device_id)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_paddir_get(base, dev_offset);
}


/*! Gpioen. */
/* Enable a GPIO pad. */
static inline void hal_gpio_pin_enable(uint32_t device_id, uint8_t gpio_pin,
                                       uint8_t enable)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = gpio_gpioen_get(base, dev_offset);
    val &= ~(mask << shift);
    val |= (enable << shift);
    gpio_gpioen_set(base, dev_offset, val);
}

static inline void hal_gpio_enable(uint32_t device_id, uint32_t gpio_mask,
                                   uint8_t enable)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t val = gpio_gpioen_get(base, dev_offset);
    if (enable)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    gpio_gpioen_set(base, dev_offset, val);
}

static inline uint32_t hal_gpio_enable_get(uint32_t device_id)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_gpioen_get(base, dev_offset);
}


/*! Padin. */
/* GPIO pad input value. */
static inline uint32_t hal_gpio_pin_input_value_get(uint32_t device_id,
                                                    uint8_t gpio_pin)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = gpio_padin_get(base, dev_offset);
    return ((val >> shift) & mask);
}

static inline uint32_t hal_gpio_input_value_get(uint32_t device_id)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_padin_get(base, dev_offset);
}


/*! Padout. */
/* GPIO pad output value. */
static inline void hal_gpio_output_value_set(uint32_t device_id, uint32_t gpio_mask,
                                             uint8_t value)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    if (value)
    {
        gpio_padout_set(base, dev_offset, gpio_mask);
    }
    else
    {
        gpio_padout_clear(base, dev_offset, gpio_mask);
    }
}

static inline void hal_gpio_pin_output_value_set(uint32_t device_id, uint8_t gpio_pin,
                                                 uint8_t value)
{
    hal_gpio_output_value_set(device_id, (1 << gpio_pin), value);
}

static inline uint32_t hal_gpio_output_value_get(uint32_t device_id)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_padout_get(base, dev_offset);
}


/*! Inten. */
/* Enable GPIO pad interruption. */
static inline void hal_gpio_pin_irq_set(uint32_t device_id, uint8_t gpio_pin,
                                        uint8_t enable)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = gpio_inten_get(base, dev_offset);
    val &= ~(mask << shift);
    val |= (enable << shift);
    gpio_inten_set(base, dev_offset, val);
}

static inline void hal_gpio_irq_set(uint32_t device_id, uint32_t gpio_mask,
                                    uint8_t enable)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t val = gpio_inten_get(base, dev_offset);
    if (enable)
    {
        val |= gpio_mask;
    }
    else
    {
        val &= ~gpio_mask;
    }
    gpio_inten_set(base, dev_offset, val);
}

static inline uint32_t hal_gpio_irq_get(uint32_t device_id)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_inten_get(base, dev_offset);
}


/*! Inttype. */
/* GPIO pad interruption type configuration. */
static inline void hal_gpio_pin_irq_type_set(uint32_t device_id, uint8_t gpio_pin,
                                             uint8_t irq_type)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t reg_offset = (gpio_pin >> 4);
    uint32_t mask = 0x3, shift = (gpio_pin << 1);
    uint32_t val = gpio_inttype_get(base, dev_offset, reg_offset);
    val &= ~(mask << shift);
    val |= (irq_type << shift);
    gpio_inttype_set(base, dev_offset, reg_offset, val);
}

static inline uint32_t hal_gpio_irq_type_get(uint32_t device_id,
                                             uint32_t reg_offset)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_inttype_get(base, dev_offset, reg_offset);
}


/*! Intstatus. */
/* GPIO interruption status. */
static inline uint32_t hal_gpio_pin_irq_status_get(uint32_t device_id,
                                                   uint8_t gpio_pin)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t mask = 0x1, shift = gpio_pin;
    uint32_t val = gpio_intstatus_get(base, dev_offset);
    return ((val >> shift) & mask);
}

static inline uint32_t hal_gpio_irq_status_get(uint32_t device_id)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_intstatus_get(base, dev_offset);
}


/*! Padcfg. */
/* Enable GPIO pad pull. */
static inline void hal_gpio_pin_pull_enable(uint32_t device_id, uint8_t gpio_pin,
                                            uint8_t pe)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t reg_offset = ARCHI_GPIO_PADCFG_REG_OFFSET(gpio_pin);
    uint32_t pin_offset = ARCHI_GPIO_PADCFG_POS_OFFSET(gpio_pin);
    uint32_t mask = 0x1, shift = (pin_offset << ARCHI_GPIO_NB_PADCFG_REG_LOG2);
    uint32_t val = gpio_padcfg_get(base, dev_offset, reg_offset);
    val &= ~(mask << shift);
    val |= (pe << shift);
    gpio_padcfg_set(base, dev_offset, reg_offset, val);
}

/* Set GPIO pad drive strength. */
static inline void hal_gpio_pin_drive_strength_set(uint32_t device_id, uint8_t gpio_pin,
                                                   uint8_t ds)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t reg_offset = ARCHI_GPIO_PADCFG_REG_OFFSET(gpio_pin);
    uint32_t pin_offset = ARCHI_GPIO_PADCFG_POS_OFFSET(gpio_pin);
    uint32_t mask = 0x2, shift = (pin_offset << ARCHI_GPIO_NB_PADCFG_REG_LOG2);
    uint32_t val = gpio_padcfg_get(base, dev_offset, reg_offset);
    val &= ~(mask << shift);
    val |= ((ds << 1) << shift);
    gpio_padcfg_set(base, dev_offset, reg_offset, val);
}

/* GPIO pad pull activation & GPIO pad drive strength. */
static inline void hal_gpio_pin_config_set(uint32_t device_id, uint8_t gpio_pin,
                                           uint8_t pe, uint8_t ds)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    uint32_t reg_offset = ARCHI_GPIO_PADCFG_REG_OFFSET(gpio_pin);
    uint32_t pin_offset = ARCHI_GPIO_PADCFG_POS_OFFSET(gpio_pin);
    uint32_t mask = 0x3, shift = (pin_offset << ARCHI_GPIO_NB_PADCFG_REG_LOG2);
    uint32_t val = gpio_padcfg_get(base, dev_offset, reg_offset);
    val &= ~(mask << shift);
    val |= (((ds << 1) | pe) << shift);
    gpio_padcfg_set(base, dev_offset, reg_offset, val);
}

static inline void hal_gpio_config_set(uint32_t device_id, uint32_t gpio_mask,
                                       uint8_t pe, uint8_t ds)
{
    uint32_t mask = gpio_mask;
    uint8_t gpio_pin = 0;
    while (mask)
    {
        if (mask & 0x1)
        {
	    hal_gpio_pin_config_set(device_id, gpio_pin, pe, ds);
        }
        mask = mask >> 1;
        gpio_pin++;
    }
}

static inline uint32_t hal_gpio_config_get(uint32_t device_id, uint32_t gpio_reg)
{
    uint32_t base = (uint32_t) gpio(0);
    uint32_t dev_offset = (uint32_t) ARCHI_GPIO_DEVICE_OFFSET(device_id);
    return gpio_padcfg_get(base, dev_offset, gpio_reg);
}

#endif  /* __PMSIS_IMPLEM_HAL_GPIO_GPIO_H__ */
