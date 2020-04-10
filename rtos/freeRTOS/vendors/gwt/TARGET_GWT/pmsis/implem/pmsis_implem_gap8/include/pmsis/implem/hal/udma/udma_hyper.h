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

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* External address base. */
#define REG_MBR0 0x0            /* Ram. */
#define REG_MBR1 0x01000000     /* Flash. */

/* Hyper access type. */
#define REG_ACCESS 0x1
#define MEM_ACCESS 0x0

/* Hyper device type, used internally. */
#define RAM 0
#define FLASH 1

/* Device access. */
#define READ 0
#define WRITE 1

/* Hyper udma configuration. */
static inline void hyper_udma_channel_set(uint32_t device_id, udma_channel_e channel,
                                          uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    udma_enqueue_channel(&(hyperbus(0)->udma), l2buf, size, cfg, channel);
}


/* External memory configuration. */
static inline void hyper_ext_addr_set(uint32_t ext_addr)
{
    hal_write32(&(hyperbus(0)->ext_addr), ext_addr);
}

static inline uint32_t hyper_ext_addr_get()
{
    return hal_read32(&(hyperbus(0)->ext_addr));
}


/* Memory control configuration 0 register. */
static inline void hyper_mem_cfg0_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg0), cfg);
}

static inline uint32_t hyper_mem_cfg0_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg0));
}


/* Memory control configuration 1 register. */
static inline void hyper_mem_cfg1_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg1), cfg);
}

static inline uint32_t hyper_mem_cfg1_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg1));
}


/* Memory control configuration 2 register. */
static inline void hyper_mem_cfg2_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg2), cfg);
}

static inline uint32_t hyper_mem_cfg2_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg2));
}


/* Memory control configuration 3 register. */
static inline void hyper_mem_cfg3_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg3), cfg);
}

static inline uint32_t hyper_mem_cfg3_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg3));
}


/* Memory control configuration 4 register. */
static inline void hyper_mem_cfg4_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg4), cfg);
}

static inline uint32_t hyper_mem_cfg4_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg4));
}


/* Memory control configuration 5 register. */
static inline void hyper_mem_cfg5_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg5), cfg);
}

static inline uint32_t hyper_mem_cfg5_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg5));
}


/* Memory control configuration 6 register. */
static inline void hyper_mem_cfg6_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg6), cfg);
}

static inline uint32_t hyper_mem_cfg6_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg6));
}


/* Memory control configuration 7 register. */
static inline void hyper_mem_cfg7_set(uint32_t cfg)
{
    hal_write32(&(hyperbus(0)->mem_cfg7), cfg);
}

static inline uint32_t hyper_mem_cfg7_get()
{
    return hal_read32(&(hyperbus(0)->mem_cfg7));
}


/*! MEM_CFG0. */
/* Set MBR0. */
static inline void hyper_mbr0_set(uint8_t mbr)
{
    hyper_mem_cfg0_set((hyper_mem_cfg0_get() & ~HYPERBUS_MEM_CFG0_MBR0_MASK)
                       | HYPERBUS_MEM_CFG0_MBR0(mbr));
}

/* Set LATENCY0. */
static inline void hyper_latency0_set(uint8_t latency)
{
    hyper_mem_cfg0_set((hyper_mem_cfg0_get() & ~HYPERBUS_MEM_CFG0_LATENCY0_MASK)
                       | HYPERBUS_MEM_CFG0_LATENCY0(latency));
}

/* Set WRAP_SIZE0. */
static inline void hyper_wrap_size0_set(uint8_t wrap_size)
{
    hyper_mem_cfg0_set((hyper_mem_cfg0_get() & ~HYPERBUS_MEM_CFG0_WRAP_SIZE0_MASK)
                       | HYPERBUS_MEM_CFG0_WRAP_SIZE0(wrap_size));
}

/*! MEM_CFG1. */
/* Set Read Chip Select High between operations. */
static inline void hyper_rd_cshi0_set(uint8_t value)
{
    hyper_mem_cfg1_set((hyper_mem_cfg1_get() & ~HYPERBUS_MEM_CFG1_RD_CSHI0_MASK)
                       | HYPERBUS_MEM_CFG1_RD_CSHI0(value));
}

/* Set Read Chip Select Setup. */
static inline void hyper_rd_css0_set(uint8_t value)
{
    hyper_mem_cfg1_set((hyper_mem_cfg1_get() & ~HYPERBUS_MEM_CFG1_RD_CSS0_MASK)
                       | HYPERBUS_MEM_CFG1_RD_CSS0(value));
}

/* Set Read Chip Select Hold. */
static inline void hyper_rd_csh0_set(uint8_t value)
{
    hyper_mem_cfg1_set((hyper_mem_cfg1_get() & ~HYPERBUS_MEM_CFG1_RD_CSH0_MASK)
                       | HYPERBUS_MEM_CFG1_RD_CSH0(value));
}

/* Set Write Chip Select High between operations. */
static inline void hyper_wr_cshi0_set(uint8_t value)
{
    hyper_mem_cfg1_set((hyper_mem_cfg1_get() & ~HYPERBUS_MEM_CFG1_WR_CSHI0_MASK)
                       | HYPERBUS_MEM_CFG1_WR_CSHI0(value));
}

/* Set Write Chip Select Setup. */
static inline void hyper_wr_css0_set(uint8_t value)
{
    hyper_mem_cfg1_set((hyper_mem_cfg1_get() & ~HYPERBUS_MEM_CFG1_WR_CSS0_MASK)
                       | HYPERBUS_MEM_CFG1_WR_CSS0(value));
}

/* Set Write Chip Select Hold. */
static inline void hyper_wr_csh0_set(uint8_t value)
{
    hyper_mem_cfg1_set((hyper_mem_cfg1_get() & ~HYPERBUS_MEM_CFG1_WR_CSH0_MASK)
                       | HYPERBUS_MEM_CFG1_WR_CSH0(value));
}

/*! MEM_CFG2. */
/* Set Read Maximum Length0. */
static inline void hyper_rd_max_length0_set(uint16_t max_len)
{
    hyper_mem_cfg2_set((hyper_mem_cfg2_get() & ~HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0_MASK)
                       | HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0(max_len));
}

/* Set Write Maximum Length0. */
static inline void hyper_wr_max_length0_set(uint16_t max_len)
{
    hyper_mem_cfg2_set((hyper_mem_cfg2_get() & ~HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0_MASK)
                       | HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0(max_len));
}

/*! MEM_CFG3. */
/* Set ACS0. */
static inline void hyper_acs0_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_ACS0_MASK)
                       | HYPERBUS_MEM_CFG3_ACS0(value));
}

/* Set TCO0. */
static inline void hyper_tco0_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_TCO0_MASK)
                       | HYPERBUS_MEM_CFG3_TCO0(value));
}

/* Set DT0. */
static inline void hyper_dt0_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_DT0_MASK)
                       | HYPERBUS_MEM_CFG3_DT0(value));
}

/* Set CRT0. */
static inline void hyper_crt0_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_CRT0_MASK)
                       | HYPERBUS_MEM_CFG3_CRT0(value));
}

/* Set RD_MAX_LEN0. */
static inline void hyper_rd_max_length0_en_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0_MASK)
                       | HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0(value));
}

/* Set WR_MAX_LEN0. */
static inline void hyper_wr_max_length0_en_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0_MASK)
                       | HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0(value));
}

/* Set RDS_DELAY_ADJ. */
static inline void hyper_rds_delay_adj_set(uint8_t value)
{
    hyper_mem_cfg3_set((hyper_mem_cfg3_get() & ~HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ_MASK)
                       | HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ(value));
}

/*! MEM_CFG4. */
/* Set MBR1. */
static inline void hyper_mbr1_set(uint8_t mbr)
{
    hyper_mem_cfg4_set((hyper_mem_cfg4_get() & ~HYPERBUS_MEM_CFG4_MBR1_MASK)
                       | HYPERBUS_MEM_CFG4_MBR1(mbr));
}

/* Set LATENCY1. */
static inline void hyper_latency1_set(uint8_t latency)
{
    hyper_mem_cfg4_set((hyper_mem_cfg4_get() & ~HYPERBUS_MEM_CFG4_LATENCY1_MASK)
                       | HYPERBUS_MEM_CFG4_LATENCY1(latency));
}

/* Set WRAP_SIZE1. */
static inline void hyper_wrap_size1_set(uint8_t wrap_size)
{
    hyper_mem_cfg4_set((hyper_mem_cfg4_get() & ~HYPERBUS_MEM_CFG4_WRAP_SIZE1_MASK)
                       | HYPERBUS_MEM_CFG4_WRAP_SIZE1(wrap_size));
}

/*! MEM_CFG5. */
/* Set Read Chip Select High between operations. */
static inline void hyper_rd_cshi1_set(uint8_t value)
{
    hyper_mem_cfg5_set((hyper_mem_cfg5_get() & ~HYPERBUS_MEM_CFG5_RD_CSHI1_MASK)
                       | HYPERBUS_MEM_CFG5_RD_CSHI1(value));
}

/* Set Read Chip Select Setup. */
static inline void hyper_rd_css1_set(uint8_t value)
{
    hyper_mem_cfg5_set((hyper_mem_cfg5_get() & ~HYPERBUS_MEM_CFG5_RD_CSS1_MASK)
                       | HYPERBUS_MEM_CFG5_RD_CSS1(value));
}

/* Set Read Chip Select Hold. */
static inline void hyper_rd_csh1_set(uint8_t value)
{
    hyper_mem_cfg5_set((hyper_mem_cfg5_get() & ~HYPERBUS_MEM_CFG5_RD_CSH1_MASK)
                       | HYPERBUS_MEM_CFG5_RD_CSH1(value));
}

/* Set Write Chip Select High between operations. */
static inline void hyper_wr_cshi1_set(uint8_t value)
{
    hyper_mem_cfg5_set((hyper_mem_cfg5_get() & ~HYPERBUS_MEM_CFG5_WR_CSHI1_MASK)
                       | HYPERBUS_MEM_CFG5_WR_CSHI1(value));
}

/* Set Write Chip Select Setup. */
static inline void hyper_wr_css1_set(uint8_t value)
{
    hyper_mem_cfg5_set((hyper_mem_cfg5_get() & ~HYPERBUS_MEM_CFG5_WR_CSS1_MASK)
                       | HYPERBUS_MEM_CFG5_WR_CSS1(value));
}

/* Set Write Chip Select Hold. */
static inline void hyper_wr_csh1_set(uint8_t value)
{
    hyper_mem_cfg5_set((hyper_mem_cfg5_get() & ~HYPERBUS_MEM_CFG5_WR_CSH1_MASK)
                       | HYPERBUS_MEM_CFG5_WR_CSH1(value));
}

/*! MEM_CFG6. */
/* Set Read Maximum Length0. */
static inline void hyper_rd_max_length1_set(uint16_t max_len)
{
    hyper_mem_cfg6_set((hyper_mem_cfg6_get() & ~HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1_MASK)
                       | HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1(max_len));
}

/* Set Write Maximum Length1. */
static inline void hyper_wr_max_length1_set(uint16_t max_len)
{
    hyper_mem_cfg6_set((hyper_mem_cfg6_get() & ~HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1_MASK)
                       | HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1(max_len));
}

/*! MEM_CFG7. */
/* Set ACS1. */
static inline void hyper_acs1_set(uint8_t value)
{
    hyper_mem_cfg7_set((hyper_mem_cfg7_get() & ~HYPERBUS_MEM_CFG7_ACS1_MASK)
                       | HYPERBUS_MEM_CFG7_ACS1(value));
}

/* Set TCO1. */
static inline void hyper_tco1_set(uint8_t value)
{
    hyper_mem_cfg7_set((hyper_mem_cfg7_get() & ~HYPERBUS_MEM_CFG7_TCO1_MASK)
                       | HYPERBUS_MEM_CFG7_TCO1(value));
}

/* Set DT1. */
static inline void hyper_dt1_set(uint8_t value)
{
    hyper_mem_cfg7_set((hyper_mem_cfg7_get() & ~HYPERBUS_MEM_CFG7_DT1_MASK)
                       | HYPERBUS_MEM_CFG7_DT1(value));
}

/* Set CRT1. */
static inline void hyper_crt1_set(uint8_t value)
{
    hyper_mem_cfg7_set((hyper_mem_cfg7_get() & ~HYPERBUS_MEM_CFG7_CRT1_MASK)
                       | HYPERBUS_MEM_CFG7_CRT1(value));
}

/* Set RD_MAX_LEN1. */
static inline void hyper_rd_max_length1_en_set(uint8_t value)
{
    hyper_mem_cfg7_set((hyper_mem_cfg7_get() & ~HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1_MASK)
                       | HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1(value));
}

/* Set WR_MAX_LEN1. */
static inline void hyper_wr_max_length1_en_set(uint8_t value)
{
    hyper_mem_cfg7_set((hyper_mem_cfg7_get() & ~HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1_MASK)
                       | HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1(value));
}

/* Length setting. */
static inline void hyper_max_length_set(uint16_t max_length, uint8_t max_length_en,
                                        uint8_t read, uint8_t cs)
{
    if (!cs)
    {
        if (read == (uint8_t) READ)
        {
            hyper_rd_max_length0_set(max_length);
            hyper_rd_max_length0_en_set(max_length_en);
        }
        else
        {
            hyper_wr_max_length0_set(max_length);
            hyper_wr_max_length0_en_set(max_length_en);
        }
    }
    else
    {
        if (read == (uint8_t) READ)
        {
            hyper_rd_max_length1_set(max_length);
            hyper_rd_max_length1_en_set(max_length_en);
        }
        else
        {
            hyper_wr_max_length1_set(max_length);
            hyper_wr_max_length1_en_set(max_length_en);
        }
    }
}

/* Timimg setting. */
static inline void hyper_timing_set(uint32_t cshi, uint32_t css, uint32_t csh,
                                    uint32_t latency, uint8_t read, uint8_t cs)
{
    if (!cs)
    {
        if (read == (uint8_t) READ)
        {
            hyper_rd_cshi0_set(cshi);
            hyper_rd_css0_set(css);
            hyper_rd_csh0_set(csh);
        }
        else
        {
            hyper_wr_cshi0_set(cshi);
            hyper_wr_css0_set(css);
            hyper_wr_csh0_set(csh);
        }
        hyper_latency0_set(latency);
    }
    else
    {
        if (read == (uint8_t) READ)
        {
            hyper_rd_cshi1_set(cshi);
            hyper_rd_css1_set(css);
            hyper_rd_csh1_set(csh);
        }
        else
        {
            hyper_wr_cshi1_set(cshi);
            hyper_wr_css1_set(css);
            hyper_wr_csh1_set(csh);
        }
        hyper_latency1_set(latency);
    }
}


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
