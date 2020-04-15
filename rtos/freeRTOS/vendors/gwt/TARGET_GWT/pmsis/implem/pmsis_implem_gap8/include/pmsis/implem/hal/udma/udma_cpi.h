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

#ifndef __PI_HAL_UDMA_CPI_H__
#define __PI_HAL_UDMA_CPI_H__

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* CPI udma configuration. */
static inline void cpi_udma_channel_set(uint32_t device_id, udma_channel_e channel,
                                        uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    udma_enqueue_channel(&(cpi(device_id)->udma), l2buf, size, cfg, channel);
}


/* Global configuration register. */
static inline void cpi_cfg_glob_set(uint32_t device_id, uint32_t cfg_glob)
{
    hal_write32(&(cpi(device_id)->cfg_glob), cfg_glob);
}

static inline uint32_t cpi_cfg_glob_get(uint32_t device_id)
{
    return hal_read32(&(cpi(device_id)->cfg_glob));
}


/* Lower Left configuration register. */
static inline void cpi_cfg_ll_set(uint32_t device_id, uint32_t cfg_ll)
{
    hal_write32(&(cpi(device_id)->cfg_ll), cfg_ll);
}

static inline uint32_t cpi_cfg_ll_get(uint32_t device_id)
{
    return hal_read32(&(cpi(device_id)->cfg_ll));
}


/* Upper Right configuration register. */
static inline void cpi_cfg_ur_set(uint32_t device_id, uint32_t cfg_ur)
{
    hal_write32(&(cpi(device_id)->cfg_ur), cfg_ur);
}

static inline uint32_t cpi_cfg_ur_get(uint32_t device_id)
{
    return hal_read32(&(cpi(device_id)->cfg_ur));
}


/* Size configuration register. */
static inline void cpi_cfg_size_set(uint32_t device_id, uint32_t cfg_size)
{
    hal_write32(&(cpi(device_id)->cfg_size), cfg_size);
}

static inline uint32_t cpi_cfg_size_get(uint32_t device_id)
{
    return hal_read32(&(cpi(device_id)->cfg_size));
}


/* Filter configuration register. */
static inline void cpi_cfg_filter_set(uint32_t device_id, uint32_t cfg_filter)
{
    hal_write32(&(cpi(device_id)->cfg_filter), cfg_filter);
}

static inline uint32_t cpi_cfg_filter_get(uint32_t device_id)
{
    return hal_read32(&(cpi(device_id)->cfg_filter));
}


/*! CFG_GLOB. */
/* Enable framedrop. */
static inline void cpi_framedrop_en_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_glob_set(device_id,
                     (cpi_cfg_glob_get(device_id) & ~CPI_CFG_GLOB_FRAMEDROP_EN_MASK)
                     | CPI_CFG_GLOB_FRAMEDROP_EN(value));
}

/* Set framedrop value. */
static inline void cpi_framedrop_val_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_glob_set(device_id,
                     (cpi_cfg_glob_get(device_id) & ~CPI_CFG_GLOB_FRAMEDROP_VAL_MASK)
                     | CPI_CFG_GLOB_FRAMEDROP_VAL(value));
}

/* Enable Frameslice. */
static inline void cpi_frameslice_en_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_glob_set(device_id,
                     (cpi_cfg_glob_get(device_id) & ~CPI_CFG_GLOB_FRAMESLICE_EN_MASK)
                     | CPI_CFG_GLOB_FRAMESLICE_EN(value));
}

/* Set format value. */
static inline void cpi_format_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_glob_set(device_id,
                     (cpi_cfg_glob_get(device_id) & ~CPI_CFG_GLOB_FORMAT_MASK)
                     | CPI_CFG_GLOB_FORMAT(value));
}

/* Set shift value. */
static inline void cpi_shift_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_glob_set(device_id,
                     (cpi_cfg_glob_get(device_id) & ~CPI_CFG_GLOB_SHIFT_MASK)
                     | CPI_CFG_GLOB_SHIFT(value));
}

/* Enable camera. */
static inline void cpi_glob_en_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_glob_set(device_id,
                     (cpi_cfg_glob_get(device_id) & ~CPI_CFG_GLOB_EN_MASK)
                     | CPI_CFG_GLOB_EN(value));
}


/*! CFG_LL. */
/* Set slice's lower left X coordinate. */
static inline void cpi_frameslice_llx_set(uint32_t device_id, uint16_t value)
{
    cpi_cfg_ll_set(device_id,
                   (cpi_cfg_ll_get(device_id) & ~CPI_CFG_LL_FRAMESLICE_LLX_MASK)
                   | CPI_CFG_LL_FRAMESLICE_LLX(value));
}

/* Set slice's lower left Y coordinate. */
static inline void cpi_frameslice_lly_set(uint32_t device_id, uint16_t value)
{
    cpi_cfg_ll_set(device_id,
                   (cpi_cfg_ll_get(device_id) & ~CPI_CFG_LL_FRAMESLICE_LLY_MASK)
                   | CPI_CFG_LL_FRAMESLICE_LLY(value));
}

static inline void hal_cpi_frameslice_ll_set(uint32_t device_id, uint16_t x,
                                             uint16_t y)
{
    uint32_t xy = CPI_CFG_LL_FRAMESLICE_LLX(x) |
                  CPI_CFG_LL_FRAMESLICE_LLY(y);
    cpi_cfg_ll_set(device_id, xy);
}


/*! CFG_UR. */
/* Set slice's upper right X coordinate. */
static inline void cpi_frameslice_urx_set(uint32_t device_id, uint16_t value)
{
    cpi_cfg_ur_set(device_id,
                   (cpi_cfg_ur_get(device_id) & ~CPI_CFG_UR_FRAMESLICE_URX_MASK)
                   | CPI_CFG_UR_FRAMESLICE_URX(value));
}

/* Set slice's upper right Y coordinate. */
static inline void cpi_frameslice_ury_set(uint32_t device_id, uint16_t value)
{
    cpi_cfg_ur_set(device_id,
                   (cpi_cfg_ur_get(device_id) & ~CPI_CFG_UR_FRAMESLICE_URY_MASK)
                   | CPI_CFG_UR_FRAMESLICE_URY(value));
}

static inline void hal_cpi_frameslice_ur_set(uint32_t device_id, uint16_t x,
                                             uint16_t y)
{
    uint32_t xy = CPI_CFG_UR_FRAMESLICE_URX(x) |
                  CPI_CFG_UR_FRAMESLICE_URY(y);
    cpi_cfg_ur_set(device_id, xy);
}


/*! CFG_SIZE. */
/* Set horizontal resolution. */
static inline void cpi_rowlen_set(uint32_t device_id, uint16_t value)
{
    cpi_cfg_size_set(device_id,
                     (cpi_cfg_size_get(device_id) & ~CPI_CFG_SIZE_ROWLEN_MASK)
                     | CPI_CFG_SIZE_ROWLEN(value));
}


/*! CFG_FILTER. */
/* Set B component multiplier coefficient. */
static inline void cpi_filter_b_coeff_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_filter_set(device_id,
                       (cpi_cfg_filter_get(device_id) & ~CPI_CFG_FILTER_B_COEFF_MASK)
                       | CPI_CFG_FILTER_B_COEFF(value));
}

/* Set G component multiplier coefficient. */
static inline void cpi_filter_g_coeff_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_filter_set(device_id,
                       (cpi_cfg_filter_get(device_id) & ~CPI_CFG_FILTER_G_COEFF_MASK)
                       | CPI_CFG_FILTER_G_COEFF(value));
}

/* Set R component multiplier coefficient. */
static inline void cpi_filter_r_coeff_set(uint32_t device_id, uint8_t value)
{
    cpi_cfg_filter_set(device_id,
                       (cpi_cfg_filter_get(device_id) & ~CPI_CFG_FILTER_R_COEFF_MASK)
                       | CPI_CFG_FILTER_R_COEFF(value));
}

/* Set R, G and B components multiplier coefficients. */
static inline void hal_cpi_filter_rgb_coeff_set(uint32_t device_id, uint8_t r,
                                                uint8_t g, uint8_t b)
{
    uint32_t rgb = CPI_CFG_FILTER_B_COEFF(b) |
                   CPI_CFG_FILTER_G_COEFF(g) |
                   CPI_CFG_FILTER_R_COEFF(r);
    cpi_cfg_filter_set(device_id, rgb);
}


/*! UDMA.  */
/* Enqueue transfer on UDMA channels. */
static inline void hal_cpi_enqueue(uint32_t device_id, uint32_t channel,
                                   uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    /* Enqueue l2 buffer & start transfer. */
    cpi_udma_channel_set(device_id, channel, l2buf, size, cfg);
}

#endif /* __PI_HAL_UDMA_CPI_H__ */
