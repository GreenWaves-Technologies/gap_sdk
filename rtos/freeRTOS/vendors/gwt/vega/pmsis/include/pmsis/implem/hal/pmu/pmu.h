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

#pragma once

#include "pmsis/targets/target.h"

/* Boot mode. */
#define COLD_BOOT                  ( 0 )   /* SoC cold boot, from Flash usually */
#define DEEP_SLEEP_BOOT            ( 1 )   /* Reboot from deep sleep state, state has been lost, somehow equivalent to COLD_BOOT */
#define RETENTIVE_BOOT             ( 2 )   /* Reboot from Retentive state, state has been retained, should bypass flash reload */

/* Domain ID. */
#define PI_PMU_SWU_ID              ( 0 )
#define PI_PMU_MRAM_ID             ( 1 )
#define PI_PMU_CSI_ID              ( 2 )
#define PI_PMU_CLUSTER_ID          ( 3 )
#define PI_PMU_CHIP_ID             ( 4 )

/* PMU state. */
#define PI_PMU_STATE_OFF           ( 0 )
#define PI_PMU_STATE_ON            ( 1 )

/* Retentive mode flags. */
#define PI_PMU_FLAGS_NO_RET        ( 0 )
#define PI_PMU_FLAGS_RET           ( 1 )
#define PI_PMU_FLAGS_PADS_OFF      ( 0 )
#define PI_PMU_FLAGS_PADS_ON       ( 2 )

/* Maestro internal events */
#define PI_PMU_MAESTRO_EVENT_ICU_OK              ( 1 << 0 )
#define PI_PMU_MAESTRO_EVENT_ICU_DELAYED         ( 1 << 1 )
#define PI_PMU_MAESTRO_EVENT_MODE_CHANGED        ( 1 << 2 )
#define PI_PMU_MAESTRO_EVENT_PICL_OK             ( 1 << 3 )
#define PI_PMU_MAESTRO_EVENT_SCU_OK              ( 1 << 4 )

/**
 * PICL_CTRL = DATA[31:16] | DIR | PICL_ADDR[14:1] | START
 *
 * DIR = READ(1) | WRITE(0)
 * PICL_ADDR = CHIP_SEL_ADDR | REG_ADDR
 * * CHIP_SEL_ADDR[6:0] PICL block selection offset, MSB-part
 * * REG_ADDR[4:0] PICL sub-block register offset, LSB-part
 * START = 1
 */
#define PI_PMU_PICL_DIR_WRITE              ( 0 )
#define PI_PMU_PICL_DIR_READ               ( 1 )

#define PI_PMU_PICL_ADDR_CHIP_SEL_OFFSET   ( 5 )
#define PI_PMU_PICL_ADDR_REG_OFFSET        ( 0 )

#define PI_PMU_PICL_ADDR(island, addr)     (((island) << PI_PMU_PICL_ADDR_CHIP_SEL_OFFSET) | \
                                            ((addr) << PI_PMU_PICL_ADDR_REG_OFFSET))
//#define PI_PMU_DLC_PACK(data, dir, paddr)  (((state) << 16) | (dir) | ((paddr) << 1) | 0x1);

#define PMU_WRITE(offset, value) IP_WRITE(pmu, offset, value)
#define PMU_READ(offset) IP_READ(pmu, offset)


static inline void maestro_picl_write(uint32_t island, uint32_t addr,
                                      uint32_t data)
{
    uint32_t base = (uint32_t) pmu;
    uint32_t paddr = PI_PMU_PICL_ADDR(island, addr);
    maestro_dlc_pctrl_t pctrl_val = { .raw = 0 };
    pctrl_val.pwdata = data;
    pctrl_val.dir    = PI_PMU_PICL_DIR_WRITE;
    pctrl_val.paddr  = paddr;
    pctrl_val.start  = 1;
    maestro_dlc_pctrl_set(base, pctrl_val.raw);
}


static inline void hal_pmu_maestro_sequence_trigger(uint32_t seq)
{
    // Compute the right register ID / bit shift as each WIU IFR register is 8 bits wide
    uint32_t reg_id = MAESTRO_WIU_IFR_0_OFFSET + (seq >> 3);
    uint32_t seq_id = seq & 0x7;
    maestro_picl_write(MAESTRO_WIU_OFFSET, reg_id, (1 << seq_id));
}


/*! MAESTRO_DLC_IMR */
/* Enable IRQ. */
static inline void hal_pmu_irq_mask_set(uint32_t mask)
{
    uint32_t base = (uint32_t) pmu;
    uint32_t irq_mask = maestro_dlc_imr_get(base);
    irq_mask |= mask;
    maestro_dlc_imr_set(base, mask);
}

/* Disable IRQ. */
static inline void hal_pmu_irq_mask_clear(uint32_t mask)
{
    uint32_t base = (uint32_t) pmu;
    uint32_t irq_mask = maestro_dlc_imr_get(base);
    irq_mask &= ~mask;
    maestro_dlc_imr_set(base, mask);
}


/*! MAESTRO_DLC_IFR */
/* Acknowledge IRQ. */
static inline void hal_pmu_irq_flag_clear(uint32_t flag)
{
    uint32_t base = (uint32_t) pmu;
    maestro_dlc_ifr_set(base, flag);
}
