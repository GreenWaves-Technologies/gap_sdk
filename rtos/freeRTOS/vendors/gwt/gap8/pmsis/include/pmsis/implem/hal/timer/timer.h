/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#pragma once

#include "pmsis/targets/target.h"

/* TIMER_CFG flags. */
#define TIMER_CFG_MODE_CNT     ( 0 ) /* Timer continues incrementing when cmp value is matched. */
#define TIMER_CFG_MODE_RST     ( 1 ) /* Timer is reset when cmp value is matched. */
#define TIMER_CFG_IRQ_DIS      ( 0 ) /* No IRQ when cmp value is matched. */
#define TIMER_CFG_IRQ_ENA      ( 1 ) /* IRQ triggered when cmp value is matched. */
#define TIMER_CFG_ONE_SHOT_DIS ( 0 ) /* Timer continues incrementing. */
#define TIMER_CFG_ONE_SHOT_ENA ( 1 ) /* Timer is disabled after matching cmp value. */
#define TIMER_CFG_CLK_SRC_FLL  ( 0 ) /* Timer clock source is FLL. */
#define TIMER_CFG_CLK_SRC_REF  ( 1 ) /* Timer clock source is REF_CLK. */


// Timer Low Configuration register.
#define TIMER_UNIT_CFG_LO_OFFSET                 0x0

// Timer High Configuration register.
#define TIMER_UNIT_CFG_HI_OFFSET                 0x4

// Timer Low counter value register.
#define TIMER_UNIT_CNT_LO_OFFSET                 0x8

// Timer High counter value register.
#define TIMER_UNIT_CNT_HI_OFFSET                 0xc

// Timer Low comparator value register.
#define TIMER_UNIT_CMP_LO_OFFSET                 0x10

// Timer High comparator value register.
#define TIMER_UNIT_CMP_HI_OFFSET                 0x14

// Start Timer Low counting register.
#define TIMER_UNIT_START_LO_OFFSET               0x18

// Start Timer High counting register.
#define TIMER_UNIT_START_HI_OFFSET               0x1c

// Reset Timer Low counter register.
#define TIMER_UNIT_RESET_LO_OFFSET               0x20

// Reset Timer High counter register.
#define TIMER_UNIT_RESET_HI_OFFSET               0x24


/* @brief Timer config structure.  */
typedef union
{
    struct
    {
        uint32_t enable:1;      /*!< Enable/Start Timer. */
        uint32_t reset:1;       /*!< Reset Timer counter. */
        uint32_t irq_en:1;      /*!< Enable IRQ when counter matches compare value. */
        uint32_t pad_0:1;       /*!< Padding. */
        uint32_t mode:1;        /*!< Timer counting mode : increment or reset when counter = compare. */
        uint32_t one_shot:1;    /*!< Timer continous mode : enable or disable when counter = compare. */
        uint32_t presc_en:1;    /*!< Enable prescaler. */
        uint32_t clk_source:1;  /*!< Timer's clock source : FLL or Ref32kHz. */
        uint32_t presc_val:8;   /*!< Prescaler value(only Timer_0/Timer_LO : Ftim = Fclk / (1 + val). */
        uint32_t pad_1:15;      /*!< Padding. */
        uint32_t timer64_en:1;  /*!< Enable 64 bit mode timer, using both Timer_0(LO) & TImer_1(HI). */
    } field;                    /*!< Timer config bit fields.  */
    uint32_t word;              /*!< Timer config in 32bits format. */
} timer_cfg_u;


static inline void timer_cfg_set(uint32_t base, uint8_t is_high, uint32_t cfg)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_CFG_LO_OFFSET + (is_high << 2);
    hal_write32((volatile void *) (base + reg_offset), cfg);
}

static inline uint32_t timer_cfg_get(uint32_t base, uint8_t is_high)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_CFG_LO_OFFSET + (is_high << 2);
    return hal_read32((volatile void *) (base + reg_offset));
}

static inline void timer_cnt_set(uint32_t base, uint8_t is_high, uint32_t cnt)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_CNT_LO_OFFSET + (is_high << 2);
    hal_write32((volatile void *) (base + reg_offset), cnt);
}

static inline uint32_t timer_cnt_get(uint32_t base, uint8_t is_high)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_CNT_LO_OFFSET + (is_high << 2);
    return hal_read32((volatile void *) (base + reg_offset));
}

static inline void timer_cmp_set(uint32_t base, uint8_t is_high, uint32_t cmp)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_CMP_LO_OFFSET + (is_high << 2);
    hal_write32((volatile void *) (base + reg_offset), cmp);
}

static inline uint32_t timer_cmp_get(uint32_t base, uint8_t is_high)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_CMP_LO_OFFSET + (is_high << 2);
    return hal_read32((volatile void *) (base + reg_offset));
}

static inline void timer_start_set(uint32_t base, uint8_t is_high, uint32_t start)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_START_LO_OFFSET + (is_high << 2);
    hal_write32((volatile void *) (base + reg_offset), start);
}

static inline void timer_reset_set(uint32_t base, uint8_t is_high, uint32_t reset)
{
    uint32_t reg_offset = (uint32_t) TIMER_UNIT_RESET_LO_OFFSET + (is_high << 2);
    hal_write32((volatile void *) (base + reg_offset), reset);
}


/* Timer config. */
static inline void hal_timer_cfg_set(uint32_t base, uint32_t cfg, uint32_t cmp,
                                     uint32_t cnt, uint8_t is_high)
{
    timer_cmp_set(base, is_high, cmp);
    timer_cnt_set(base, is_high, cnt);
    timer_cfg_set(base, is_high, cfg);
}

/* Timer start. */
static inline void hal_timer_start_set(uint32_t base, uint8_t is_high)
{
    timer_start_set(base, is_high, 1);
}

/* Timer stop. */
static inline void hal_timer_stop_set(uint32_t base, uint8_t is_high)
{
    timer_cfg_u cfg = { .word = timer_cfg_get(base, is_high) };
    cfg.field.enable = 0;
    timer_cfg_set(base, is_high, cfg.word);
}

/* Timer reset. */
static inline void hal_timer_reset_set(uint32_t base, uint8_t is_high)
{
    timer_reset_set(base, is_high, 1);
}

/* Timer current value. */
static inline uint32_t hal_timer_value_get(uint32_t base, uint8_t is_high)
{
    return timer_cnt_get(base, is_high);
}
