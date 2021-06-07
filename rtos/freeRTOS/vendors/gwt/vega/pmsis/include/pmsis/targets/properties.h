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

#ifndef __PMSIS_TARGETS_PROPERTIES_H__
#define __PMSIS_TARGETS_PROPERTIES_H__

/* FC. */
/* Cores & cluster ID. */
#define ARCHI_FC_CID                (31)
#define ARCHI_HAS_FC_ITC            (1)

/* Memories. */
/* L2 memory. */
#define ARCHI_HAS_L2_ALIAS          (1)
#define L2_PRIV0_SIZE               (0x00008000) /* 32kB. */
#define L2_PRIV1_SIZE               (0x00008000) /* 32kB. */
#define L2_SHARED_SIZE              (0x00180000) /* 1.5MB. */

/* L2 TS. */
#define ARCHI_HAS_L2_PRIV0_TS       (1)
#define ARCHI_HAS_L2_PRIV1_TS       (1)
#define ARCHI_L2_TS_OFFSET          (1 << 25)

/* ROM memory. */
#define ROM_SIZE                    (0x00002000)

/* SoC peripherals. */
#define FLL_OFFSET                  (0x00000000)
#define GPIO_OFFSET                 (0x00001000)
#define UDMA_OFFSET                 (0x00002000)
#define APB_SOC_CTRL_OFFSET         (0x00004000)
#define ADV_TIMER_OFFSET            (0x00005000)
#define SOC_EU_OFFSET               (0x00006000)
#define PMU_OFFSET                  (0x00007000)
#define RTC_BASE_OFFSET             (0x00008000)
#define FC_ICACHE_OFFSET            (0x00008800)
#define FC_ITC_OFFSET               (0x00009000)
#define I3C0_OFFSET                 (0x0000A000)
#define I3C1_OFFSET                 (0x0000A800)
#define FC_TIMER_OFFSET             (0x0000B000)
#define DPHY_OFFSET                 (0x0000C000)
#define CSI2_OFFSET                 (0x0000D000)
#define MPU_OFFSET                  (0x0000E000)
#define EFUSE_OFFSET                (0x0000F000)
#define DEBUG_OFFSET                (0x00010000)
#define STDOUT_OFFSET               (0x00020000)
#define QUIDDIKEY_OFFSET            (0x00021000)

/* CLOCKS */
#define ARCHI_REF_CLOCK_LOG2        (15)
#define ARCHI_REF_CLOCK             (1 << ARCHI_REF_CLOCK_LOG2) // 32kHz, 32768
#define ARCHI_FLL_REF_CLOCK_LOG2    (ARCHI_REF_CLOCK_LOG2)
//#define ARCHI_REF_CLOCK             (1 << 15) // 32kHz, 32768 // used for RTC
#define ARCHI_FLL_REF_CLOCK         (24576063) // 24Mhz
#define ARCHI_FAST_REF_CLOCK_INIT   (6000000) // 6Mhz

/* FLL */
#if defined( __PLATFORM_FPGA__)
#define ARCHI_FREQ_INIT             (CONFIG_FREQUENCY_FPGA)
#else
#define ARCHI_FREQ_INIT             (50000000)
#endif
#define ARCHI_NB_FLL                (3) // But one struct
#define FLL_ID_PERIPH               (1)
#define FLL_ID_FC                   (0)
#define FLL_ID_CL                   (2)

/* TIMER */
#define ARCHI_NB_TIMER_FC           (2)
#define TIMER_SIZE_LOG2             (11)
#define TIMER_SIZE                  (1 << TIMER_SIZE_LOG2)
#define FC_TIMER(id)                (id)

/* UDMA peripherals. */
#define UDMA_HAS_SPIM               (1)
#define UDMA_HAS_UART               (1)
#define UDMA_HAS_I2C                (1)
#define UDMA_HAS_HYPER              (1)
#define UDMA_HAS_JTAG_FIFO          (1)
#define UDMA_HAS_SDIO               (1)
#define UDMA_HAS_I2S                (1)
#define UDMA_HAS_CPI                (1)
#define UDMA_HAS_CSI2               (1)
#define UDMA_HAS_MRAM               (1)
#define UDMA_HAS_FILTER             (1)

/* Number of UDMA peripherals. */
#define UDMA_NB_SPIM                (4)
#define UDMA_NB_UART                (3)
#define UDMA_NB_I2C                 (1)
#define UDMA_NB_HYPER               (1)
#define UDMA_NB_JTAG_FIFO           (1)
#define UDMA_NB_SDIO                (1)
#define UDMA_NB_I2S                 (1)
#define UDMA_NB_CPI                 (1)
#define UDMA_NB_CSI2                (1)
#define UDMA_NB_MRAM                (1)
#define UDMA_NB_FILTER              (1)

#define ARCHI_UDMA_NB_UART          (UDMA_NB_UART)
#define ARCHI_UDMA_NB_I2S           (UDMA_NB_I2S)

#define UDMA_NB_PERIPH              ((UDMA_HAS_SPIM ? UDMA_NB_SPIM) +                   \
                                     (UDMA_HAS_UART ? UDMA_NB_UART) +                   \
                                     (UDMA_HAS_I2C ? UDMA_NB_I2C) +                     \
                                     (UDMA_HAS_HYPER ? UDMA_NB_HYPER) +                 \
                                     (UDMA_HAS_JTAG_FIFO ? UDMA_NB_JTAG_FIFO) +         \
                                     (UDMA_HAS_SDIO ? UDMA_NB_SDIO) +                   \
                                     (UDMA_HAS_I2S ? UDMA_NB_I2S) +                     \
                                     (UDMA_HAS_CPI ? UDMA_NB_CPI) +                     \
                                     (UDMA_HAS_CSI2 ? UDMA_NB_CSI2) +                   \
                                     (UDMA_HAS_MRAM ? UDMA_NB_MRAM) +                   \
                                     (UDMA_HAS_FILTER ? UDMA_NB_FILTER))

/* Size of each UDMA peripheral. */
#define UDMA_PERIPH_SIZE_LOG2       (7)
#define UDMA_PERIPH_SIZE            (1 << UDMA_PERIPH_SIZE_LOG2)

/* UDMA peripherals ID. */
#define UDMA_SPIM_ID(id)            (0 + (id)) /* 0-3 */
#define UDMA_UART_ID(id)            (UDMA_SPIM_ID(UDMA_NB_SPIM) + (id)) /* 4-6 */
#define UDMA_I2C_ID(id)             (UDMA_UART_ID(UDMA_NB_UART) + (id)) /* 7 */
#define UDMA_HYPER_ID(id)           (UDMA_I2C_ID(UDMA_NB_I2C) + (id)) /* 8 */
#define UDMA_JTAG_FIFO_ID(id)       (UDMA_HYPER_ID(UDMA_NB_HYPER) + (id)) /* 9 */
#define UDMA_SDIO_ID(id)            (UDMA_JTAG_FIFO_ID(UDMA_NB_JTAG_FIFO) + (id)) /* 10 */
#define UDMA_I2S_ID(id)             (UDMA_SDIO_ID(UDMA_NB_SDIO) + (id)) /* 11 */
#define UDMA_CPI_ID(id)             (UDMA_I2S_ID(UDMA_NB_I2S) + (id)) /* 12 */
#define UDMA_CSI2_ID(id)            (UDMA_CPI_ID(UDMA_NB_CPI) + (id)) /* 13 */
#define UDMA_MRAM_ID(id)            (UDMA_CSI2_ID(UDMA_NB_CSI2) + (id)) /* 14 */
#define UDMA_FILTER_ID(id)          (UDMA_MRAM_ID(UDMA_NB_MRAM) + (id)) /* 15 */

/* Pads & GPIO. */
#define ARCHI_NB_PAD                (96)
#define ARCHI_NB_GPIO               (32)
/* GPIO ID */
#define GPIO_ID(id)                 (0 + id)

/* PWM */
#define ARCHI_NB_PWM                (4)
#define ARCHI_NB_CHANNEL_PER_PWM    (4)


/* Cluster. */
/* Cores & cluster. */
#define ARCHI_HAS_CLUSTER           (1)
#define ARCHI_CL_CID(id)            (id)
#define ARCHI_CLUSTER_NB_CORES      (9)
#define ARCHI_CLUSTER_NB_PE         (8) /* Processing elements. */
#define ARCHI_CLUSTER_PE_MASK       (((1 << ARCHI_CLUSTER_NB_PE) - 1))
#define ARCHI_CLUSTER_MASTER_CORE   (8)
#define ARCHI_CLUSTER_SYNC_BARR_ID  (1)
#define ARCHI_CLUSTER_LEGCY_BARR_ID (0)

#define ARCHI_HAS_CLUSTER_CLK_GATE  (1)
#define ARCHI_CLUSTER_SIZE          (0x00400000)

/* Memories. */
/* L1 cluster memory. */
#define ARCHI_HAS_CL_L1_ALIAS       (1)
#define CL_L1_SIZE                  (0x00010000) /* 64kB. */

/* L1 cluster memory TS. */
#define ARCHI_HAS_CL_L1_TS          (1)
#define ARCHI_CL_L1_TS_OFFSET       (1 << 20)


/* Cluster peripherals. */
#define CL_NB_HW_MUTEX              (8)
#define CL_NB_HW_BARRIER            (8)


#define CL_EU_HW_BARRIER_RESERVED       (2)
#define CL_EU_HW_BARRIER_RESERVED_MASK  ((1 << CL_EU_HW_BARRIER_RESERVED) - 1)

#define CL_ALLOC_INIT_BARRIER           (((1 << CL_NB_HW_BARRIER) - 1) & \
                                         ~CL_EU_HW_BARRIER_RESERVED_MASK)


#define CL_CTRL_OFFSET              (0x00000000)
#define CL_TIMER_OFFSET             (0x00000400)
#define CL_GLOB_EU_CORE_OFFSET      (0x00000800)
#define CL_GLOB_EU_BARRIER_OFFSET   (0x00000C00)
#define CL_GLOB_EU_SW_EVENT_OFFSET  (0x00000E00)
#define CL_GLOB_EU_SOC_EVENT_OFFSET (0x00000F00)
#define CL_GLOB_EU_DISPATCH_OFFSET  (0x00000F80)
#define CL_HWCE_OFFSET              (0x00001000)
#define CL_ICACHE_CTRL_OFFSET       (0x00001400)
#define CL_DMA_OFFSET               (0x00001800)
#define CL_DECOMP_OFFSET            (0x00002000)
#define CL_DEMUX_PERIPH_OFFSET      (0x00004000)


/*! Event_Unit Demux offset */
#define CLUSTER_DEMUX_EU_CORE_OFFSET              ( 0x00000000 )
#define CLUSTER_DEMUX_EU_LOOP_OFFSET              ( 0x00000060 )
#define CLUSTER_DEMUX_EU_DISPATCH_OFFSET          ( 0x00000080 )
#define CLUSTER_DEMUX_EU_HW_MUTEX_OFFSET          ( 0x000000C0 )
#define CLUSTER_DEMUX_EU_SW_EVENT_OFFSET          ( 0x00000100 )
#define CLUSTER_DEMUX_EU_HW_BARRIER_OFFSET        ( 0x00000200 )


/*! Event_Unit Core Demux */
#define CL_DEMUX_EU_CORE_EVENT_MASK               ( 0x00 )
#define CL_DEMUX_EU_CORE_EVENT_MASK_AND           ( 0x04 )
#define CL_DEMUX_EU_CORE_EVENT_MASK_OR            ( 0x08 )
#define CL_DEMUX_EU_CORE_IRQ_MASK                 ( 0x0C )
#define CL_DEMUX_EU_CORE_IRQ_MASK_AND             ( 0x10 )
#define CL_DEMUX_EU_CORE_IRQ_MASK_OR              ( 0x14 )
#define CL_DEMUX_EU_CORE_STATUS                   ( 0x18 )
#define CL_DEMUX_EU_CORE_BUFFER                   ( 0x1C )
#define CL_DEMUX_EU_CORE_BUFFER_MASKED            ( 0x20 )
#define CL_DEMUX_EU_CORE_BUFFER_IRQ_MASKED        ( 0x24 )
#define CL_DEMUX_EU_CORE_BUFFER_CLEAR             ( 0x28 )
#define CL_DEMUX_EU_CORE_SW_EVT_MASK              ( 0x2C )
#define CL_DEMUX_EU_CORE_SW_EVT_MASK_AND          ( 0x30 )
#define CL_DEMUX_EU_CORE_SW_EVT_MASK_OR           ( 0x34 )
#define CL_DEMUX_EU_CORE_EVENT_WAIT               ( 0x38 )
#define CL_DEMUX_EU_CORE_EVENT_WAIT_CLEAR         ( 0x3C )
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK             ( 0x40 )
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK_AND         ( 0x44 )
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK_OR          ( 0x48 )

/*! Event_Unit Loop Demux */
#define CL_DEMUX_EU_LOOP_STATE                    ( 0x00 )
#define CL_DEMUX_EU_LOOP_START                    ( 0x04 )
#define CL_DEMUX_EU_LOOP_END                      ( 0x08 )
#define CL_DEMUX_EU_LOOP_INCR                     ( 0x0C )
#define CL_DEMUX_EU_LOOP_CHUNK                    ( 0x10 )
#define CL_DEMUX_EU_LOOP_EPOCH                    ( 0x14 )
#define CL_DEMUX_EU_LOOP_SINGLE                   ( 0x18 )

/*! Event_Unit Dispatch Demux */
#define CL_DEMUX_EU_DISPATCH_FIFO_ACCESS          ( 0x00 )
#define CL_DEMUX_EU_DISPATCH_TEAM_CONFIG          ( 0x04 )

/*! Event_Unit Mutex Demux */
#define CL_DEMUX_EU_HW_MUTEX_MUTEX                ( 0x00 )

/*! Event_Unit SW_Events Demux */
#define CL_DEMUX_EU_SW_EVT_TRIGGER                ( 0x00 )
#define CL_DEMUX_EU_SW_EVT_TRIGGER_WAIT           ( 0x40 )
#define CL_DEMUX_EU_SW_EVT_TRIGGER_WAIT_CLEAR     ( 0x80 )

/*! Event_Unit HW Barrier Demux */
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK       ( 0x00 )
#define CL_DEMUX_EU_HW_BARRIER_STATUS             ( 0x04 )
#define CL_DEMUX_EU_HW_BARRIER_STATUS_SUMMARY     ( 0x08 )
#define CL_DEMUX_EU_HW_BARRIER_TARGET_MASK        ( 0x0C )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER            ( 0x10 )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_SELF       ( 0x14 )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT       ( 0x18 )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT_CLEAR ( 0x1C )

#define CL_DEMUX_EU_HW_BARRIER_SIZE               ( 0x20 )


/* Cluster demux peripherals.*/
#define CL_DEMUX_EU_CORE_OFFSET         (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_CORE_OFFSET)
#define CL_DEMUX_EU_LOOP_OFFSET         (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_LOOP_OFFSET)
#define CL_DEMUX_EU_DISPATCH_OFFSET     (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_DISPATCH_OFFSET)
#define CL_DEMUX_EU_HW_MUTEX_OFFSET     (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_HW_MUTEX_OFFSET)
#define CL_DEMUX_EU_SW_EVENT_OFFSET     (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_SW_EVENT_OFFSET)
#define CL_DEMUX_EU_HW_BARRIER_OFFSET   (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_HW_BARRIER_OFFSET)
#define CL_DEMUX_DMA_OFFSET             (CL_DEMUX_PERIPH_OFFSET + 0x00000400)


#define CL_DMA_ID(id)                   (id)


#define PRINTF_BUFFER_SIZE              ( 128 )


#endif  /* __PMSIS_TARGETS_PROPERTIES_H__ */
