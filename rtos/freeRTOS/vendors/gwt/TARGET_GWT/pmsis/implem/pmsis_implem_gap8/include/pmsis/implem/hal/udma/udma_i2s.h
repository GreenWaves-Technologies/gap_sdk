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

#ifndef __PI_HAL_UDMA_I2S_H__
#define __PI_HAL_UDMA_I2S_H__

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* I2S udma configuration. */
static inline void i2s_udma_channel_set(uint32_t device_id, udma_channel_e channel,
                                        uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    udma_enqueue_channel(&(i2s(device_id)->udma), l2buf, size, cfg, channel);
    //printf("I2S: %x %x %x %x %d\n", &(i2s(device_id)->udma), l2buf, size, cfg, channel);
}

static inline void i2s_udma_clear(uint32_t device_id, udma_channel_e channel)
{
    udma_channel_clear(&(i2s(device_id)->udma), channel);
}

/* EXT_CLK. */
static inline void i2s_ext_clk_set(uint32_t device_id, uint32_t ext_clk)
{
    hal_write32(&(i2s(device_id)->ext_clk), ext_clk);
}

static inline uint32_t i2s_ext_clk_get(uint32_t device_id)
{
    return hal_read32(&(i2s(device_id)->ext_clk));
}

/* CFG_CLKGEN0. */
static inline void i2s_cfg_clkgen0_set(uint32_t device_id, uint32_t cfg_clkgen0)
{
    hal_write32(&(i2s(device_id)->cfg_clkgen0), cfg_clkgen0);
}

static inline uint32_t i2s_cfg_clkgen0_get(uint32_t device_id)
{
    return hal_read32(&(i2s(device_id)->cfg_clkgen0));
}

/* CFG_CLKGEN1. */
static inline void i2s_cfg_clkgen1_set(uint32_t device_id, uint32_t cfg_clkgen1)
{
    hal_write32(&(i2s(device_id)->cfg_clkgen1), cfg_clkgen1);
}

static inline uint32_t i2s_cfg_clkgen1_get(uint32_t device_id)
{
    return hal_read32(&(i2s(device_id)->cfg_clkgen1));
}

/* CHMODE. */
static inline void i2s_chmode_set(uint32_t device_id, uint32_t chmode)
{
    hal_write32(&(i2s(device_id)->chmode), chmode);
}

static inline uint32_t i2s_chmode_get(uint32_t device_id)
{
    return hal_read32(&(i2s(device_id)->chmode));
}

/* FILT_CH0. */
static inline void i2s_filt_ch0_set(uint32_t device_id, uint32_t filt_ch0)
{
    hal_write32(&(i2s(device_id)->filt_ch0), filt_ch0);
}

static inline uint32_t i2s_filt_ch0_get(uint32_t device_id)
{
    return hal_read32(&(i2s(device_id)->filt_ch0));
}

/* FILT_CH1. */
static inline void i2s_filt_ch1_set(uint32_t device_id, uint32_t filt_ch1)
{
    hal_write32(&(i2s(device_id)->filt_ch1), filt_ch1);
}

static inline uint32_t i2s_filt_ch1_get(uint32_t device_id)
{
    return hal_read32(&(i2s(device_id)->filt_ch1));
}


/*! EXT_CLK. */
/* Set external clock word length. */
static inline void hal_i2s_ext_clk_set(uint32_t device_id, uint16_t clk_len)
{
    i2s_ext_clk_set(device_id, clk_len);
}


/*! CFG_CLKGEN0. */
/* CH0 : Set clock generator word length. */
static inline void hal_i2s_cfg_clkgen0_word_len_set(uint32_t device_id, uint16_t word_len)
{
    i2s_cfg_clkgen0_set(device_id,
                        (i2s_cfg_clkgen0_get(device_id) & ~I2S_CFG_CLKGEN0_BITS_WORD_MASK)
                        | I2S_CFG_CLKGEN0_BITS_WORD(word_len));
}

/* CH0 : Enable clock generator. */
static inline void hal_i2s_cfg_clkgen0_clock_enable(uint32_t device_id, uint8_t enable)
{
    i2s_cfg_clkgen0_set(device_id,
                        (i2s_cfg_clkgen0_get(device_id) & ~I2S_CFG_CLKGEN0_CLK_EN_MASK)
                        | I2S_CFG_CLKGEN0_CLK_EN(enable));
}

/* CH0 : Set clock divider. */
static inline void hal_i2s_cfg_clkgen0_clk_div_set(uint32_t device_id, uint16_t clk_div)
{
    i2s_cfg_clkgen0_set(device_id,
                        (i2s_cfg_clkgen0_get(device_id) & ~I2S_CFG_CLKGEN0_CLK_DIV_MASK)
                        | I2S_CFG_CLKGEN0_CLK_DIV(clk_div));
}

/* CH0 : Configure clkgen0. */
static inline void hal_i2s_cfg_clkgen0_set(uint32_t device_id, uint16_t word_len,
                                           uint8_t enable, uint16_t clk_div)
{
    i2s_cfg_clkgen0_set(device_id, (I2S_CFG_CLKGEN0_BITS_WORD(word_len)
                                    | I2S_CFG_CLKGEN0_CLK_EN(enable)
                                    | I2S_CFG_CLKGEN0_CLK_DIV(clk_div)));
}


/*! CFG_CLKGEN1. */
/* CH1 : Set clock generator word length. */
static inline void hal_i2s_cfg_clkgen1_word_len_set(uint32_t device_id, uint16_t word_len)
{
    i2s_cfg_clkgen1_set(device_id,
                        (i2s_cfg_clkgen1_get(device_id) & ~I2S_CFG_CLKGEN1_BITS_WORD_MASK)
                        | I2S_CFG_CLKGEN1_BITS_WORD(word_len));
}

/* CH1 : Enable clock generator. */
static inline void hal_i2s_cfg_clkgen1_clock_enable(uint32_t device_id, uint8_t enable)
{
    i2s_cfg_clkgen1_set(device_id,
                        (i2s_cfg_clkgen1_get(device_id) & ~I2S_CFG_CLKGEN1_CLK_EN_MASK)
                        | I2S_CFG_CLKGEN1_CLK_EN(enable));
}

/* CH1 : Set clock divider. */
static inline void hal_i2s_cfg_clkgen1_clk_div_set(uint32_t device_id, uint16_t clk_div)
{
    i2s_cfg_clkgen1_set(device_id,
                        (i2s_cfg_clkgen1_get(device_id) & ~I2S_CFG_CLKGEN1_CLK_DIV_MASK)
                        | I2S_CFG_CLKGEN1_CLK_DIV(clk_div));
}

/* CH1 : Configure clkgen1. */
static inline void hal_i2s_cfg_clkgen1_set(uint32_t device_id, uint16_t word_len,
                                           uint8_t enable, uint16_t clk_div)
{
    i2s_cfg_clkgen1_set(device_id, (I2S_CFG_CLKGEN1_BITS_WORD(word_len)
                                    | I2S_CFG_CLKGEN1_CLK_EN(enable)
                                    | I2S_CFG_CLKGEN1_CLK_DIV(clk_div)));
}


/*! CHMODE. */
/* CH0 : Set LSB. */
static inline void hal_i2s_chmode_ch0_lsb_set(uint32_t device_id, uint8_t lsb)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH0_LSB_FIRST_MASK)
                   | I2S_CHMODE_CH0_LSB_FIRST(lsb));
}

/* CH0 : Enable PDM filter. */
static inline void hal_i2s_chmode_ch0_pdm_filter_enable(uint32_t device_id, uint8_t enable)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH0_PDM_USEFILTER_MASK)
                   | I2S_CHMODE_CH0_PDM_USEFILTER(enable));
}

/* CH0 : Enable PDM demodulation. */
static inline void hal_i2s_chmode_ch0_pdm_enable(uint32_t device_id, uint8_t enable)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH0_PDM_EN_MASK)
                   | I2S_CHMODE_CH0_PDM_EN(enable));
}

/* CH0 : Enable DDR. */
static inline void hal_i2s_chmode_ch0_ddr_enable(uint32_t device_id, uint8_t enable)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH0_USEDDR_MASK)
                   | I2S_CHMODE_CH0_USEDDR(enable));
}

/* CH0 : Configure clock/WS mode. */
static inline void hal_i2s_chmode_ch0_mode_set(uint32_t device_id, uint8_t mode)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH0_MODE_MASK)
                   | I2S_CHMODE_CH0_MODE(mode));
}

/* CH0 : Channel configuration. */
static inline void hal_i2s_chmode_ch0_set(uint32_t device_id, uint8_t lsb,
                                          uint8_t pdm_filter_ena, uint8_t pdm_ena,
                                          uint8_t ddr_ena, uint8_t ch_mode)
{
    uint32_t ch0_mask = I2S_CHMODE_CH0_LSB_FIRST_MASK |
                        I2S_CHMODE_CH0_PDM_USEFILTER_MASK |
                        I2S_CHMODE_CH0_PDM_EN_MASK |
                        I2S_CHMODE_CH0_USEDDR_MASK |
                        I2S_CHMODE_CH0_MODE_MASK;
    uint32_t ch0_setting = I2S_CHMODE_CH0_LSB_FIRST(lsb) |
                           I2S_CHMODE_CH0_PDM_USEFILTER(pdm_filter_ena) |
                           I2S_CHMODE_CH0_PDM_EN(pdm_ena) |
                           I2S_CHMODE_CH0_USEDDR(ddr_ena) |
                           I2S_CHMODE_CH0_MODE(ch_mode);
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~ch0_mask) | ch0_setting);
}

/* CH1 : Set LSB. */
static inline void hal_i2s_chmode_ch1_lsb_set(uint32_t device_id, uint8_t lsb)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH1_LSB_FIRST_MASK)
                   | I2S_CHMODE_CH1_LSB_FIRST(lsb));
}

/* CH1 : Enable PDM filter. */
static inline void hal_i2s_chmode_ch1_pdm_filter_enable(uint32_t device_id, uint8_t enable)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH1_PDM_USEFILTER_MASK)
                   | I2S_CHMODE_CH1_PDM_USEFILTER(enable));
}

/* CH1 : Enable PDM demodulation. */
static inline void hal_i2s_chmode_ch1_pdm_enable(uint32_t device_id, uint8_t enable)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH1_PDM_EN_MASK)
                   | I2S_CHMODE_CH1_PDM_EN(enable));
}

/* CH1 : Enable DDR. */
static inline void hal_i2s_chmode_ch1_ddr_enable(uint32_t device_id, uint8_t enable)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH1_USEDDR_MASK)
                   | I2S_CHMODE_CH1_USEDDR(enable));
}

/* CH1 : Configure clock/WS mode. */
static inline void hal_i2s_chmode_ch1_mode_set(uint32_t device_id, uint8_t mode)
{
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~I2S_CHMODE_CH1_MODE_MASK)
                   | I2S_CHMODE_CH1_MODE(mode));
}

/* CH1 : Channel configuration. */
static inline void hal_i2s_chmode_ch1_set(uint32_t device_id, uint8_t lsb,
                                          uint8_t pdm_filter_ena, uint8_t pdm_ena,
                                          uint8_t ddr_ena, uint8_t ch_mode)
{
    uint32_t ch1_mask = I2S_CHMODE_CH1_LSB_FIRST_MASK |
                        I2S_CHMODE_CH1_PDM_USEFILTER_MASK |
                        I2S_CHMODE_CH1_PDM_EN_MASK |
                        I2S_CHMODE_CH1_USEDDR_MASK |
                        I2S_CHMODE_CH1_MODE_MASK;
    uint32_t ch1_setting = I2S_CHMODE_CH1_LSB_FIRST(lsb) |
                           I2S_CHMODE_CH1_PDM_USEFILTER(pdm_filter_ena) |
                           I2S_CHMODE_CH1_PDM_EN(pdm_ena) |
                           I2S_CHMODE_CH1_USEDDR(ddr_ena) |
                           I2S_CHMODE_CH1_MODE(ch_mode);
    i2s_chmode_set(device_id,
                   (i2s_chmode_get(device_id) & ~ch1_mask) | ch1_setting);
}


/*! FILT_CH0. */
/* CH0 : Set decimation value. */
static inline void hal_i2s_filt_ch0_decimation_set(uint32_t device_id, uint16_t value)
{
    i2s_filt_ch0_set(device_id,
                     (i2s_filt_ch0_get(device_id) & ~I2S_FILT_CH0_DECIMATION_MASK)
                     | I2S_FILT_CH0_DECIMATION(value));
}

/* CH0 : Set filter normalisation right shift value. */
static inline void hal_i2s_filt_ch0_shift_set(uint32_t device_id, uint8_t value)
{
    i2s_filt_ch0_set(device_id,
                     (i2s_filt_ch0_get(device_id) & ~I2S_FILT_CH0_SHIFT_MASK)
                     | I2S_FILT_CH0_SHIFT(value));
}

/* CH0 : Set decimation and filter normalisation right shift value. */
static inline void hal_i2s_filt_ch0_set(uint32_t device_id, uint16_t decimation,
                                        uint8_t shift)
{
    i2s_filt_ch0_set(device_id,
                     (I2S_FILT_CH0_DECIMATION(decimation) | I2S_FILT_CH0_SHIFT(shift)));
}


/*! FILT_CH1. */
/* CH1 : Set decimation value. */
static inline void hal_i2s_filt_ch1_decimation_set(uint32_t device_id, uint16_t value)
{
    i2s_filt_ch1_set(device_id,
                     (i2s_filt_ch1_get(device_id) & ~I2S_FILT_CH1_DECIMATION_MASK)
                     | I2S_FILT_CH1_DECIMATION(value));
}

/* CH1 : Set filter normalisation right shift value. */
static inline void hal_i2s_filt_ch1_shift_set(uint32_t device_id, uint8_t value)
{
    i2s_filt_ch1_set(device_id,
                     (i2s_filt_ch1_get(device_id) & ~I2S_FILT_CH1_SHIFT_MASK)
                     | I2S_FILT_CH1_SHIFT(value));
}

/* CH1 : Set decimation and filter normalisation right shift value. */
static inline void hal_i2s_filt_ch1_set(uint32_t device_id, uint16_t decimation,
                                        uint8_t shift)
{
    i2s_filt_ch1_set(device_id,
                     (I2S_FILT_CH1_DECIMATION(decimation) | I2S_FILT_CH1_SHIFT(shift)));
}


/*! UDMA. */
static inline void hal_i2s_enqueue(uint32_t device_id, udma_channel_e channel,
                                   uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    cfg |= UDMA_CORE_RX_CFG_EN(1);
    i2s_udma_channel_set(device_id, channel, l2buf, size, cfg);
}

static inline void hal_i2s_clear(uint32_t device_id, udma_channel_e channel)
{
    i2s_udma_clear(device_id, channel);
}

#endif  /* __PI_HAL_UDMA_I2S_H__ */
