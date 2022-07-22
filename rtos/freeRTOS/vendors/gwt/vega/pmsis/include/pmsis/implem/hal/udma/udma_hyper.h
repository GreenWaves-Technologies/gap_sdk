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
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"


/* External address base. */
#define REG_MBR0 0x0            /* Ram. */
#define REG_MBR1 0x01000000     /* Flash. */

/* Hyperbus access type. */
#define MEM_ACCESS               ( 0x0 )
#define REG_ACCESS               ( 0x1 )

/* Base addresses. */
//#define CS0_BASE_ADDR            ( 0x00000000 )
//#define CS1_BASE_ADDR            ( 0x40000000 )

/* OctoSPI device type. */
#define TYPE_OCTOSPI             ( 0x0 )
#define TYPE_HYPERBUS            ( 0x1 )

/* Hyperbus device type. */
#define TYPE_RAM                 ( 0x0 )
#define TYPE_FLASH               ( 0x1 )

/* IRQ enable. */
#define HYPER_IRQ_DISABLE        ( 0x0 )
#define HYPER_IRQ_ENABLE         ( 0x1 )

/* Transfer mode. */
#define HYPER_MODE_AUTO_ENABLE   ( 0x1 )
#define HYPER_MODE_AUTO_DISABLE  ( 0x0 )
#define HYPER_MODE_XIP_ENABLE    ( 0x1 )
#define HYPER_MODE_XIP_DISABLE   ( 0x0 )

/* Burst type. */
#define HYPER_BURST_2D_ENABLE    ( 0x3 ) // enable 2D for both normal and XIP modes

/* 2D transfer mode from L2 to external memory. */
#define HYPER_2D_MODE_1D_1D      ( 0x0 )
#define HYPER_2D_MODE_1D_2D      ( 0x1 )
#define HYPER_2D_MODE_2D_1D      ( 0x2 )
#define HYPER_2D_MODE_2D_2D      ( 0x3 )

/* Device access. */
#define READ  0
#define WRITE 1

/* Hyper udma configuration. */
static inline void hyper_udma_channel_set(uint32_t l2buf, uint32_t size, uint32_t cfg,
                                          udma_channel_e channel)
{
    hal_udma_enqueue_channel((udma_core_cmd_t *) &(hyper(0)->udma), l2buf, size,
                             cfg, channel);
}


/* EXT_ADDR register. */
static inline void hyper_ext_addr_set(uint32_t ext_addr)
{
    GAP_WRITE(&(hyper(0)->ext_addr), 0, ext_addr);
}

static inline uint32_t hyper_ext_addr_get(void)
{
    return GAP_READ(&(hyper(0)->ext_addr), 0);
}

/* TIMING_cfg register. */
static inline void hyper_timing_cfg_set(uint32_t cfg)
{
    GAP_WRITE(&(hyper(0)->timing_cfg), 0, cfg);
}

static inline uint32_t hyper_timing_cfg_get(void)
{
    return GAP_READ(&(hyper(0)->timing_cfg), 0);
}

/* MBA0 register. */
static inline void hyper_mba0_set(uint32_t addr)
{
    GAP_WRITE(&(hyper(0)->mba0), 0, addr);
}

static inline uint32_t hyper_mba0_get(void)
{
    return GAP_READ(&(hyper(0)->mba0), 0);
}

/* MBA1 register. */
static inline void hyper_mba1_set(uint32_t addr)
{
    GAP_WRITE(&(hyper(0)->mba1), 0, addr);
}

static inline uint32_t hyper_mba1_get(void)
{
    return GAP_READ(&(hyper(0)->mba1), 0);
}

/* DEVICE register. */
static inline void hyper_device_set(uint32_t cfg)
{
    GAP_WRITE(&(hyper(0)->device), 0, cfg);
}

static inline uint32_t hyper_device_get(void)
{
    return GAP_READ(&(hyper(0)->device), 0);
}

/* BURST_SIZE register. */
static inline void hyper_burst_size_set(uint32_t size)
{
    GAP_WRITE(&(hyper(0)->burst_size), 0, size);
}

static inline uint32_t hyper_burst_size_get(void)
{
    return GAP_READ(&(hyper(0)->burst_size), 0);
}

/* BURST_ENABLE register. */
static inline void hyper_burst_enable_set(uint32_t enable)
{
    GAP_WRITE(&(hyper(0)->burst_enable), 0, enable);
}

static inline uint32_t hyper_burst_enable_get(void)
{
    return GAP_READ(&(hyper(0)->burst_enable), 0);
}

/* IRQ_EN register. */
static inline void hyper_irq_en_set(uint32_t irq_en)
{
    GAP_WRITE(&(hyper(0)->irq_en), 0, irq_en);
}

static inline uint32_t hyper_irq_en_get(void)
{
    return GAP_READ(&(hyper(0)->irq_en), 0);
}

/* CLK_DIV register. */
static inline void hyper_clk_div_set(uint32_t clk_div)
{
    GAP_WRITE(&(hyper(0)->clk_div), 0, clk_div);
}

static inline uint32_t hyper_clk_div_get(void)
{
    return GAP_READ(&(hyper(0)->clk_div), 0);
}

/* STATUS register. */
static inline uint32_t hyper_status_get(void)
{
    return GAP_READ(&(hyper(0)->status), 0);
}


/* Timing config. */
static inline void hal_udma_hyper_timing_cfg_set(uint8_t latency_0, uint8_t latency_1,
                                                 uint8_t rw_recov, uint8_t rwds_delay,
                                                 uint8_t autocheck, uint16_t cs_max)
{
    hyper_timing_cfg_t timing_cfg = { .word = 0 };
    timing_cfg.field.latency0 = latency_0;
    timing_cfg.field.latency1 = latency_1;
    timing_cfg.field.rw_recovery = rw_recov;
    timing_cfg.field.rwds_delay = rwds_delay;
    timing_cfg.field.additional_latency_autocheck_en = autocheck;
    timing_cfg.field.cs_max = cs_max;
    hyper_timing_cfg_set(timing_cfg.word);
}

/* Device start addr config. */
static inline void hal_udma_hyper_mba0_set(uint32_t addr)
{
    hyper_mba0_set(addr);
}

static inline void hal_udma_hyper_mba1_set(uint32_t addr)
{
    hyper_mba1_set(addr);
}

/* Device config. */
static inline void hal_udma_hyper_device_set(uint8_t dev_type, uint8_t dev_0, uint8_t dev_1)
{
    /* uint32_t device = (HYPER_DEVICE_TYPE(dev_type) | */
    /*                    HYPER_DEVICE_DT0(dev_0) | */
    /*                    HYPER_DEVICE_DT1(dev_1)); */
    hyper_device_t device = { .word = 0 };
    device.field.type = dev_type;
    device.field.dt0 = dev_0;
    device.field.dt1 = dev_1;
    hyper_device_set(device.word);
}

/* Irq en. */
static inline void hal_udma_hyper_irq_enable(void)
{
    hyper_irq_en_set(1);
}

static inline void hal_udma_hyper_irq_disable(void)
{
    hyper_irq_en_set(0);
}

/* Clock div config. */
static inline void hal_udma_hyper_clk_div_set(uint8_t clock_div)
{
    hyper_clk_div_t clk_div = { .word = 0 };
    clk_div.field.data = clock_div;
    clk_div.field.valid = 1;
    hyper_clk_div_set(clk_div.word);
}

/* Status. */
static inline uint32_t hal_udma_hyper_status_get(void)
{
    return hyper_status_get();
}

/* Enqueue transfer on UDMA channels. */
static inline void hal_udma_hyper_enqueue(uint32_t ext_addr, uint32_t l2buf,
                                          uint32_t size, uint32_t cfg,
                                          udma_channel_e channel)
                                          //uint32_t channel)
{
    //cfg |= (UDMA_CORE_CMD_RX_CFG_EN_MASK | UDMA_CORE_CFG_DATASIZE_32);
    /* Set external address. */
    hyper_ext_addr_set(ext_addr);
    /* Enqueue l2 buffer & start transfer. */
    hyper_udma_channel_set(l2buf, size, cfg, channel);
}
