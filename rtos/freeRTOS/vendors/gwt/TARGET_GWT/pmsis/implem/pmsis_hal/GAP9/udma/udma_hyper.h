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

#ifndef __PI_HAL_UDMA_HYPER_H__
#define __PI_HAL_UDMA_HYPER_H__

#include "targets/pmsis_targets.h"
#include "pmsis_hal/GAP9/udma/udma_core.h"
#include "pmsis_hal/GAP9/udma/udma_ctrl.h"

/* External address base. */
#define REG_MBR0 0x0            /* Ram. */
#define REG_MBR1 0x10000000     /* Flash. */

/* Hyper access type. */
#define MEM_ACCESS 0x0
#define REG_ACCESS 0x1

/* OctoSPI device type. */
#define TYPE_OCTOSPI  0x0
#define TYPE_HYPERBUS 0x1

/* Hyper device type, used internally. */
#define RAM   0x0
#define FLASH 0x1

/* Device access. */
#define READ  0x0
#define WRITE 0x1

/* Hyper udma configuration. */
static inline void hyper_udma_channel_set(uint8_t hyper_id, udma_channel_e channel,
                                          uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    udma_enqueue_channel(&(hyperbus(hyper_id)->udma), l2buf, size, cfg, channel);
}

/* External memory configuration register. */
static inline void hyper_ext_addr_set(uint8_t hyper_id, uint32_t addr)
{
    hal_write32(&(hyperbus(hyper_id)->ext_addr), addr);
}

static inline uint32_t hyper_ext_addr_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->ext_addr));
}

/* Timing configuration register. */
static inline void hyper_timing_cfg_set(uint8_t hyper_id, uint32_t cfg)
{
    hal_write32(&(hyperbus(hyper_id)->timing_cfg), cfg);
}

static inline uint32_t hyper_timing_cfg_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->timing_cfg));
}

/* MBA0(Memory Base Address) configuration register. */
static inline void hyper_mba0_set(uint8_t hyper_id, uint32_t base)
{
    hal_write32(&(hyperbus(hyper_id)->mba0), base);
}

static inline uint32_t hyper_mba0_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->mba0));
}

/* MBA1(Memory Base Address) configuration register. */
static inline void hyper_mba1_set(uint8_t hyper_id, uint32_t base)
{
    hal_write32(&(hyperbus(hyper_id)->mba1), base);
}

static inline uint32_t hyper_mba1_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->mba1));
}

/* Device configuration register. */
static inline void hyper_device_set(uint8_t hyper_id, uint32_t cfg)
{
    hal_write32(&(hyperbus(hyper_id)->device), cfg);
}

static inline uint32_t hyper_device_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->device));
}

/* OSPI command register. */
static inline void hyper_ospi_cmd_set(uint8_t hyper_id, uint32_t cmd)
{
    hal_write32(&(hyperbus(hyper_id)->ospi_cmd), cmd);
}

static inline uint32_t hyper_ospi_cmd_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->ospi_cmd));
}

/* OSPI address register. */
static inline void hyper_ospi_addr_set(uint8_t hyper_id, uint32_t addr)
{
    hal_write32(&(hyperbus(hyper_id)->ospi_addr), addr);
}

static inline uint32_t hyper_ospi_addr_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->ospi_addr));
}

/* OSPI configuration register. */
static inline void hyper_ospi_cfg_set(uint8_t hyper_id, uint32_t cfg)
{
    hal_write32(&(hyperbus(hyper_id)->ospi_cfg), cfg);
}

static inline uint32_t hyper_ospi_cfg_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->ospi_cfg));
}

/* Burst size configuration register. */
static inline void hyper_burst_size_set(uint8_t hyper_id, uint32_t size)
{
    hal_write32(&(hyperbus(hyper_id)->burst_size), size);
}

static inline uint32_t hyper_burst_size_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->burst_size));
}

/* Burst enable configuration register. */
static inline void hyper_burst_enable_set(uint8_t hyper_id, uint32_t enable)
{
    hal_write32(&(hyperbus(hyper_id)->burst_enable), enable);
}

static inline uint32_t hyper_burst_enable_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->burst_enable));
}

/* Irq enable register. */
static inline void hyper_irq_en_set(uint8_t hyper_id, uint32_t enable)
{
    hal_write32(&(hyperbus(hyper_id)->irq_en), enable);
}

static inline uint32_t hyper_irq_en_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->irq_en));
}

/* Clock div configuration register. */
static inline void hyper_clk_div_set(uint8_t hyper_id, uint32_t clk_div)
{
    hal_write32(&(hyperbus(hyper_id)->clk_div), clk_div);
}

static inline uint32_t hyper_clk_div_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->clk_div));
}

/* Status register. */
static inline uint32_t hyper_status_get(uint8_t hyper_id)
{
    return hal_read32(&(hyperbus(hyper_id)->status));
}


/*! EXT_ADDR */
/*
 * Set external address(address in Ram or Flash).
 * For Ram, to accces registers, reg_access should be set.
 */
static inline void hal_hyper_ext_addr_set(uint8_t hyper_id, uint32_t ext_addr, uint8_t reg_access)
{
    hyper_ext_addr_set(hyper_id, HYPERBUS_EXT_ADDR_REG_ACCESS(reg_access) + ext_addr);
}


/*! TIMING_CFG */
/* Latency0 setting, for device on CS0. */
static inline void hal_hyper_timing_latency0_set(uint8_t hyper_id, uint8_t latency)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    value |= HYPERBUS_TIMING_CFG_LATENCY0(latency);
    hyper_timing_cfg_set(hyper_id, value);
}

/* Latency1 setting, for device on CS1. */
static inline void hal_hyper_timing_latency1_set(uint8_t hyper_id, uint8_t latency)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_LATENCY1_MASK;
    value |= HYPERBUS_TIMING_CFG_LATENCY1(latency);
    hyper_timing_cfg_set(hyper_id, value);
}

/* RW recovery setting, Read-Write recovery time. */
static inline void hal_hyper_timing_rw_delay_set(uint8_t hyper_id, uint8_t delay)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_RW_RECOVERY_MASK;
    value |= HYPERBUS_TIMING_CFG_RW_RECOVERY(delay);
    hyper_timing_cfg_set(hyper_id, value);
}

/* RWDS setting. */
static inline void hal_hyper_timing_rwds_set(uint8_t hyper_id, uint8_t delay)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_RWDS_DELAY_MASK;
    value |= HYPERBUS_TIMING_CFG_RWDS_DELAY(delay);
    hyper_timing_cfg_set(hyper_id, value);
}

/* Enable additional latency check. */
static inline void hal_hyper_timing_latency_check_enable(uint8_t hyper_id)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK;
    value |= HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(1);
    hyper_timing_cfg_set(hyper_id, value);
}

/* Disable additional latency check. */
static inline void hal_hyper_timing_latency_check_disable(uint8_t hyper_id)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK;
    hyper_timing_cfg_set(hyper_id, value);
}

/* CS Max setting, time for HyperRam to valid data transfer. */
static inline void hal_hyper_timing_cs_max_set(uint8_t hyper_id, uint16_t delay)
{
    uint32_t value = hyper_timing_cfg_get(hyper_id);
    value &= ~HYPERBUS_TIMING_CFG_CS_MAX_MASK;
    value |= HYPERBUS_TIMING_CFG_CS_MAX(delay);
    hyper_timing_cfg_set(hyper_id, value);
}


/*! MBA0. */
/*  */


/* Enqueue transfer on UDMA channels. */
static inline void hal_hyper_enqueue(uint32_t device_id, uint32_t channel, uint32_t ext_addr,
                                     uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    cfg |= UDMA_CFG_DATA_SIZE(32 >> 4);
    /* Set external address. */
    hyper_ext_addr_set(ext_addr);
    /* Enqueue l2 buffer & start transfer. */
    hyper_udma_channel_set(device_id, channel, l2buf, size, cfg);
}

#endif /* __PI_HAL_UDMA_HYPER_H__ */
