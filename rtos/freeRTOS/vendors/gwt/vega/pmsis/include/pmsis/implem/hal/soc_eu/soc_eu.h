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


/* Number of SOC events. */
/* 256 events, 32/regs. */
#define SOC_EVENTS_NB_LOG2                   ( 0x8 )
#define SOC_EVENTS_NB                        ( 0x1 << SOC_EVENTS_NB_LOG2 )
#define SOC_EVENTS_NB_REGS_LOG2              ( 0x3 )
#define SOC_EVENTS_NB_REGS                   ( 0x1 << SOC_EVENTS_NB_REGS_LOG2 )
#define SOC_EVENTS_NB_EVENTS_PER_REG_LOG2    ( SOC_EVENTS_NB_LOG2 - SOC_EVENTS_NB_REGS_LOG2 )
#define SOC_EVENTS_NB_EVENTS_PER_REG         ( 0x1 << SOC_EVENTS_NB_EVENTS_PER_REG_LOG2 )
#define SOC_EVENTS_NB_EVENTS_PER_REG_MASK    ( SOC_EVENTS_NB_EVENTS_PER_REG - 1 )

#define SOC_EVENTS_EVENT_REG_OFFSET_GET(evt) ( (evt) >> SOC_EVENTS_NB_EVENTS_PER_REG_LOG2 )
#define SOC_EVENTS_EVENT_POS_OFFSET_GET(evt) ( (evt) & SOC_EVENTS_NB_EVENTS_PER_REG_MASK )


/** FC|PR|CL_MASK. */
static inline void soc_eu_mask_set(uint32_t reg_offset, uint32_t reg_id,
                                   uint32_t mask)
{
    uint32_t base = (uint32_t) soc_eu;
    uint32_t offset = reg_offset + (reg_id << 2);
    GAP_WRITE(base, offset, mask);
}

static inline uint32_t soc_eu_mask_get(uint32_t reg_offset, uint32_t reg_id)
{
    uint32_t base = (uint32_t) soc_eu;
    uint32_t offset = reg_offset + (reg_id << 2);
    return GAP_READ(base, offset);
}

static inline void soc_eu_mask_event_set(uint32_t reg_offset, uint32_t event_num)
{
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t mask = soc_eu_mask_get(reg_offset, reg_id);
    mask = __BITCLR_R(mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, mask);
}

static inline void soc_eu_mask_event_clear(uint32_t reg_offset, uint32_t event_num)
{
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t mask = soc_eu_mask_get(reg_offset, reg_id);
    mask = __BITSET_R(mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, mask);
}

/** TIMER_SEL. */
static inline void soc_eu_timer_sel_set(uint32_t reg_id, uint32_t mask)
{
    uint32_t base = (uint32_t) soc_eu;
    uint32_t offset = (uint32_t) SOC_EU_TIMER1_SEL_HI_OFFSET + reg_id;
    GAP_WRITE(base, offset, mask);
}


/* SW event trigger. */
static inline uint32_t hal_soc_eu_event_get(void)
{
    uint32_t base = (uint32_t) soc_eu;
    return soc_eu_sw_event_get(base);
}

static inline void hal_soc_eu_event_mask_set(uint32_t mask)
{
    uint32_t base = (uint32_t) soc_eu;
    soc_eu_sw_event_set(base, mask);
}


/* Set FC_MASK. */
static inline void hal_soc_eu_fc_mask_reset(void)
{
    uint32_t reg_offset = (uint32_t) SOC_EU_FC_MASK_0_OFFSET;
    uint32_t reset_mask = 0xFFFFFFFF;
    for (uint32_t offset=0; offset<(uint32_t) SOC_EVENTS_NB_REGS; offset++)
    {
        soc_eu_mask_set(reg_offset, offset, reset_mask);
    }
}

static inline void hal_soc_eu_fc_mask_set(uint32_t event_num)
{
    #if 0
    uint32_t reg_offset = (uint32_t) SOC_EU_FC_MASK_0_OFFSET;
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t fc_mask = soc_eu_mask_get(reg_offset, reg_id);
    fc_mask = __BITCLR_R(fc_mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, fc_mask);
    #else
    uint32_t reg_offset = (uint32_t) SOC_EU_FC_MASK_0_OFFSET;
    soc_eu_mask_event_set(reg_offset, event_num);
    #endif
}

static inline void hal_soc_eu_fc_mask_clear(uint32_t event_num)
{
    #if 0
    uint32_t reg_offset = (uint32_t) SOC_EU_FC_MASK_0_OFFSET;
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t fc_mask = soc_eu_mask_get(reg_offset, reg_id);
    fc_mask = __BITSET_R(fc_mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, fc_mask);
    #else
    uint32_t reg_offset = (uint32_t) SOC_EU_FC_MASK_0_OFFSET;
    soc_eu_mask_event_clear(reg_offset, event_num);
    #endif
}


/* Set CL_MASK. */
static inline void hal_soc_eu_cl_mask_reset(void)
{
    uint32_t reg_offset = (uint32_t) SOC_EU_CL_MASK_0_OFFSET;
    uint32_t reset_mask = 0xFFFFFFFF;
    for (uint32_t offset=0; offset<(uint32_t) SOC_EVENTS_NB_REGS; offset++)
    {
        soc_eu_mask_set(reg_offset, offset, reset_mask);
    }
}

static inline void hal_soc_eu_cl_mask_set(uint32_t event_num)
{
    #if 0
    uint32_t reg_offset = (uint32_t) SOC_EU_CL_MASK_0_OFFSET;
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t cl_mask = soc_eu_mask_get(reg_offset, reg_id);
    cl_mask = __BITCLR_R(cl_mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, cl_mask);
    #else
    uint32_t reg_offset = (uint32_t) SOC_EU_CL_MASK_0_OFFSET;
    soc_eu_mask_event_set(reg_offset, event_num);
    #endif
}

static inline void hal_soc_eu_cl_mask_clear(uint32_t event_num)
{
    #if 0
    uint32_t reg_offset = (uint32_t) SOC_EU_CL_MASK_0_OFFSET;
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t cl_mask = soc_eu_mask_get(reg_offset, reg_id);
    cl_mask = __BITSET_R(cl_mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, cl_mask);
    #else
    uint32_t reg_offset = (uint32_t) SOC_EU_CL_MASK_0_OFFSET;
    soc_eu_mask_event_clear(reg_offset, event_num);
    #endif
}


/* Set PR_MASK. */
static inline void hal_soc_eu_pr_mask_reset(void)
{
    uint32_t reg_offset = (uint32_t) SOC_EU_PR_MASK_0_OFFSET;
    uint32_t reset_mask = 0xFFFFFFFF;
    for (uint32_t offset=0; offset<(uint32_t) SOC_EVENTS_NB_REGS; offset++)
    {
        soc_eu_mask_set(reg_offset, offset, reset_mask);
    }
}

static inline void hal_soc_eu_pr_mask_set(uint32_t event_num)
{
    #if 0
    uint32_t reg_offset = (uint32_t) SOC_EU_PR_MASK_0_OFFSET;
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t pr_mask = soc_eu_mask_get(reg_offset, reg_id);
    pr_mask = __BITCLR_R(pr_mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, pr_mask);
    #else
    uint32_t reg_offset = (uint32_t) SOC_EU_PR_MASK_0_OFFSET;
    soc_eu_mask_event_set(reg_offset, event_num);
    #endif
}

static inline void hal_soc_eu_pr_mask_clear(uint32_t event_num)
{
    #if 0
    uint32_t reg_offset = (uint32_t) SOC_EU_PR_MASK_0_OFFSET;
    uint32_t reg_id = SOC_EVENTS_EVENT_REG_OFFSET_GET(event_num);
    uint32_t reg_pos = SOC_EVENTS_EVENT_POS_OFFSET_GET(event_num);
    uint32_t pr_mask = soc_eu_mask_get(reg_offset, reg_id);
    pr_mask = __BITSET_R(pr_mask, 1, reg_pos);
    soc_eu_mask_set(reg_offset, reg_id, pr_mask);
    #else
    uint32_t reg_offset = (uint32_t) SOC_EU_PR_MASK_0_OFFSET;
    soc_eu_mask_event_clear(reg_offset, event_num);
    #endif
}


/* Set Timer propagtion event. */
static inline void hal_soc_eu_timer_event_set(uint32_t timer_id, uint8_t is_lo,
                                              uint32_t event)
{
    uint32_t reg_id = (timer_id << 3) + (is_lo << 2);
    uint32_t mask = SOC_EU_TIMER1_SEL_HI_EVT(event);
    mask = SOC_EU_TIMER1_SEL_HI_ENA_SET(mask, 1);
    soc_eu_timer_sel_set(reg_id, mask);
}


/* Compat. */
#define hal_soc_eu_set_fc_mask   hal_soc_eu_fc_mask_set
#define hal_soc_eu_clear_fc_mask hal_soc_eu_fc_mask_clear
#define hal_soc_eu_set_cl_mask   hal_soc_eu_cl_mask_set
#define hal_soc_eu_clear_cl_mask hal_soc_eu_cl_mask_clear
#define hal_soc_eu_set_pr_mask   hal_soc_eu_pr_mask_set
#define hal_soc_eu_clear_pr_mask hal_soc_eu_pr_mask_clear
