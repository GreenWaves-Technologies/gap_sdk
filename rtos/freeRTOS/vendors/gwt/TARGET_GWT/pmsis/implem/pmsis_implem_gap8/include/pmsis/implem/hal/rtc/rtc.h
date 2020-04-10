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

#ifndef __PI_HAL_RTC_H__
#define __PI_HAL_RTC_H__

#include "pmsis/targets/target.h"

/* APB Status Register. */
static inline void rtc_apb_sr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_sr), value);
}

static inline uint32_t rtc_apb_sr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_sr));
}

/* APB Control Register. */
static inline void rtc_apb_cr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_cr), value);
}

static inline uint32_t rtc_apb_cr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_cr));
}

/* APB Data Register. */
static inline void rtc_apb_dr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_dr), value);
}

static inline uint32_t rtc_apb_dr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_dr));
}

/* APB IRQ Control Register. */
static inline void rtc_apb_icr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_icr), value);
}

static inline uint32_t rtc_apb_icr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_icr));
}

/* APB IRQ Mask Register. */
static inline void rtc_apb_imr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_imr), value);
}

static inline uint32_t rtc_apb_imr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_imr));
}

/* APB IRQ Flag Register. */
static inline void rtc_apb_ifr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_ifr), value);
}

static inline uint32_t rtc_apb_ifr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_ifr));
}


/*! APB Status Register. */
static inline uint32_t hal_rtc_status_get(uint8_t rtc_id)
{
    return rtc_apb_sr_get(rtc_id);
}

/*! APB Control Register. */
static inline void hal_rtc_request_set(uint8_t rtc_id, uint8_t addr, uint8_t write)
{
    rtc_apb_cr_set(rtc_id, RTC_APB_CR_APB_ADDR(addr) | RTC_APB_CR_APB_OP(write));
}


/*! APB Data Register. */
static inline void hal_rtc_data_set(uint8_t rtc_id, uint32_t data)
{
    rtc_apb_dr_set(rtc_id, data);
}

static inline uint32_t hal_rtc_data_get(uint8_t rtc_id)
{
    return rtc_apb_dr_get(rtc_id);
}


/*! APB IRQ Flag Register. */
static inline void hal_rtc_irq_flag_read_reset(uint8_t rtc_id)
{
    rtc_apb_ifr_set(rtc_id, (rtc_apb_ifr_get(rtc_id)
                             & ~RTC_APB_IFR_READ_FLAG_MASK)
                             | RTC_APB_IFR_READ_FLAG(1));
}

static inline void hal_rtc_irq_flag_write_reset(uint8_t rtc_id)
{
    rtc_apb_ifr_set(rtc_id, (rtc_apb_ifr_get(rtc_id)
                             & ~RTC_APB_IFR_WRITE_FLAG_MASK)
                             | RTC_APB_IFR_WRITE_FLAG(1));
}

#endif  /* __PI_HAL_RTC_H__ */
