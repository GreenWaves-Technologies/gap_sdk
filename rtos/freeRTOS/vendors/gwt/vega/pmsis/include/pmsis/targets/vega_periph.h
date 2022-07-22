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

#ifndef __PMSIS_TARGETS_VEGA_PERIPH_H__
#define __PMSIS_TARGETS_VEGA_PERIPH_H__

/* ----------------------------------------------------------------------------
   -- VEGA SoC Configuration
   ---------------------------------------------------------------------------- */

#define __MPU_PRESENT                  1 /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               0 /**< Number of priority bits implemented in the NVIC */
#define __FPU_PRESENT                  0 /**< Defines if an FPU is present or not */

/* Include memory map and events for Vega. */
#include "pmsis/targets/memory_map.h"
#include "pmsis/targets/events.h"

#include "cores/TARGET_RISCV_32/core_gap.h"               /* Core Peripheral Access Layer */
#include "cores/TARGET_RISCV_32/core_gap_memory_define.h" /* Core Memory regions definitions */
#include "cores/TARGET_RISCV_32/core_utils.h"


/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
   -- FLL_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/fll_ctrl/fll_ctrl.h"
#define fll(id) ((fll_ctrl_t *) (FLL_ADDR + (id << FLL_AREA_SIZE_LOG2)))


/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */
//#include "periph/gpio_periph.h"
#include "periph/gpio/gpio.h"
#define gpio(id) ((gpio_t *) GPIO_ADDR)


/* ----------------------------------------------------------------------------
   -- UDMA Global Configuration Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_core_periph.h"
#include "periph/udma_core_cmd_periph.h"
#include "periph/udma_ctrl_periph.h"
#define udma_ctrl ((udma_ctrl_t *) UDMA_CTRL_ADDR)

/* ----------------------------------------------------------------------------
   -- SPIM Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/spi_periph.h"

/* ----------------------------------------------------------------------------
   -- HYPERBUS Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_hyper_periph.h"
#define hyper(id) ((hyper_t *) UDMA_HYPER(id))

/* ----------------------------------------------------------------------------
   -- UART Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_uart_periph.h"
#define uart(id) ((uart_t *) UDMA_UART(id))

/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_i2c_periph.h"
#define i2c(id) ((i2c_t *) UDMA_I2C(id))

/* ----------------------------------------------------------------------------
   -- I2S Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/udma_i2s_periph.h"
#include "periph/udma_i2s_v2.h"
#define i2s(id) ((i2s_t *) UDMA_I2S(id))

/* ----------------------------------------------------------------------------
   -- CPI Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/cpi_periph.h"
#define cpi(id) ((cpi_t *) UDMA_CPI(id))


/* ----------------------------------------------------------------------------
   -- SOC_CTRL Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/apb_soc_ctrl.h"
#include "periph/soc_ctrl_periph.h"
#define apb_soc_ctrl ((soc_ctrl_t *) APB_SOC_CTRL_ADDR)


/* ----------------------------------------------------------------------------
   -- PWM Peripheral
   ---------------------------------------------------------------------------- */
#include "periph/pwm_periph.h"
#include "periph/pwm_ctrl_periph.h"
#define pwm(id) ((pwm_t *) (ADV_TIMER_ADDR + (id << 6)))
#define pwm_ctrl ((pwm_ctrl_t *) (ADV_TIMER_ADDR + 0x100))


/* ----------------------------------------------------------------------------
   -- SOC_EU Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/soc_eu/soc_eu.h"
#define soc_eu ((soc_eu_t *) SOC_EU_ADDR)


/* ----------------------------------------------------------------------------
   -- PMU Peripheral Access Layer
   ---------------------------------------------------------------------------- */
//#include "periph/power_manager_periph.h"
#include "periph/maestro_v3.h"
#define pmu ((maestro_maestro_t *) PMU_ADDR)


/* ----------------------------------------------------------------------------
   -- RTC_APB Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/rtc_periph.h"
#define rtc(id) ((rtc_t *) RTC_ADDR)


/* ----------------------------------------------------------------------------
   -- FC_ICACHE_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/fc_icache_ctrl/fc_icache_ctrl.h"
#define fc_icache ((fc_icache_ctrl_t *) FC_ICACHE_ADDR)


/* ----------------------------------------------------------------------------
   -- FC_ITC Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/fc_itc_periph.h"
#define fc_itc ((fc_itc_t *) FC_ITC_ADDR)


/* ----------------------------------------------------------------------------
   -- FC_TIMER  Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#define fc_timer(id) ((timer_periph_t *) FC_TIMER_ADDR(id))


/* ----------------------------------------------------------------------------
   -- I3C  Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/i3c_v2.h"
#define i3c(id) ((i3c_i3c_t *) I3C_ADDR(id))


/* ----------------------------------------------------------------------------
   -- EFUSE CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/efuse_periph.h"

/* EFUSE_CTRL - Peripheral instance base addresses */
/** Peripheral EFUSE_CTRL base address */
#define EFUSE_CTRL_BASE                               (SOC_PERI_BASE + 0x09000u)
/** Peripheral EFUSE_CTRL base pointer */
#define efuse_ctrl                                    ((efuse_ctrl_t *)EFUSE_CTRL_BASE)
/** Array initializer of EFUSE_CTRL base addresses */
#define EFUSE_CTRL_BASE_ADDRS                         { EFUSE_CTRL_BASE }
/** Array initializer of EFUSE_CTRL base pointers */
#define EFUSE_CTRL_BASE_PTRS                          { EFUSE_CTRL }


/* EFUSE_REGS - Peripheral instance base addresses */
/** Peripheral EFUSE_REGS base address */
#define EFUSE_REGS_BASE                                (SOC_PERI_BASE + 0x09200u)
/** Peripheral EFUSE_REGS base pointer */
#define efuse_regs                                     ((efuse_regs_t *)EFUSE_REGS_BASE)
#define efuse_regs_array                               ((int32_t*)EFUSE_REGS_BASE)
/** Array initializer of EFUSE_REGS base addresses */
#define EFUSE_REGS_BASE_ADDRS                          { EFUSE_REGS_BASE }
/** Array initializer of EFUSE_REGS base pointers */
#define EFUSE_REGS_BASE_PTRS                           { EFUSE_REGS }


/* ----------------------------------------------------------------------------
   -- EFUSE REG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/** EFUSE_REGS - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t INFO;                    /**< EFUSE INFO register, offset: 0x000 */
  __IO  uint32_t INFO2;                   /**< EFUSE_INFO2 register, offset: 0x004 */
  __IO  uint32_t AES_KEY[16];             /**< EFUSE_AES_KEY registers, offset: 0x008 */
  __IO  uint32_t AES_IV[8];               /**< EFUSE_AES_IV registers, offset: 0x048 */
  __IO  uint32_t WAIT_XTAL_DELTA_LSB;     /**< EFUSE_WAIT_XTAL_DELTA_LSB register, offset: 0x068 */
  __IO  uint32_t WAIT_XTAL_DELTA_MSB;     /**< EFUSE_WAIT_XTAL_DELTA_MSB register, offset: 0x06C */
  __IO  uint32_t WAIT_XTAL_MIN;           /**< EFUSE_WAIT_XTAL_MIN registers, offset: 0x070 */
  __IO  uint32_t WAIT_XTAL_MAX;           /**< EFUSE_WAIT_XTAL_MAX registers, offset: 0x074 */
  __IO  uint32_t HYPER_RDS_DELAY;         /**< EFUSE_WAIT_XTAL_MAX registers, offset: 0x078 */
  __IO  uint32_t FLL_FREQ;                /**< EFUSE_FLL_FREQ registers, offset: 0x07C */
  __IO  uint32_t FLL_TOLERANCE;           /**< EFUSE_FLL_TOLERANCE registers, offset: 0x080 */
  __IO  uint32_t FLL_ASSERT_CYCLE;        /**< EFUSE_FLL_ASSERT_CYCLE registers, offset: 0x084 */
  __IO  uint32_t _reserved[6];            /**< EFUSE_reserved registers, offset: 0x088 */
  __IO  uint32_t USER_REG[88];            /**< EFUSE_USER_REG, offset: 0x0A0 */
} EFUSE_REGS_Type;

/* ----------------------------------------------------------------------------
   -- EFUSE_REGS Register Masks
   ---------------------------------------------------------------------------- */

/*! @name INFO - EFUSE information register */
#define EFUSE_INFO_PLT_MASK                           (0x07U)
#define EFUSE_INFO_PLT_SHIFT                          (0U)
#define EFUSE_INFO_PLT(x)                             (((uint32_t)(((uint32_t)(x)) /* << EFUSE_INFO_PLT_SHIFT */)) & EFUSE_INFO_PLT_MASK)

#define EFUSE_INFO_BOOT_MASK                          (0x38U)
#define EFUSE_INFO_BOOT_SHIFT                         (3U)
#define EFUSE_INFO_BOOT(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_BOOT_SHIFT)) & EFUSE_INFO_BOOT_MASK)

#define EFUSE_INFO_ENCRYPTED_MASK                     (0x40U)
#define EFUSE_INFO_ENCRYPTED_SHIFT                    (6U)
#define EFUSE_INFO_ENCRYPTED(x)                       (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_ENCRYPTED_SHIFT)) & EFUSE_INFO_ENCRYPTED_MASK)

#define EFUSE_INFO_WAIT_XTAL_MASK                     (0x80U)
#define EFUSE_INFO_WAIT_XTAL_SHIFT                    (7U)
#define EFUSE_INFO_WAIT_XTAL(x)                       (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_WAIT_XTAL_SHIFT)) & EFUSE_INFO_WAIT_XTAL_MASK)



/* EFUSE_REGS - Peripheral instance base addresses */
/** Peripheral EFUSE_REGS base address */
#define EFUSE_REGS_BASE                                (SOC_PERI_BASE + 0x09200u)
/** Peripheral EFUSE_REGS base pointer */
#define EFUSE_REGS                                     ((EFUSE_REGS_Type *)EFUSE_REGS_BASE)
/** Array initializer of EFUSE_REGS base addresses */
#define EFUSE_REGS_BASE_ADDRS                          { EFUSE_REGS_BASE }
/** Array initializer of EFUSE_REGS base pointers */
#define EFUSE_REGS_BASE_PTRS                           { EFUSE_REGS }

/*!
 * @}
 */ /* end of group EFUSE_REGS_Peripheral_Access_Layer */



#if defined(FEATURE_CLUSTER)

/* ----------------------------------------------------------------------------
   -- CL_CTRL_UNIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_ctrl_unit/cluster_ctrl_unit.h"
#define cl_ctrl(id)         ((cluster_ctrl_unit_t *) CL_CTRL_ADDR)
#define cl_glob_ctrl(cid)   ((cluster_ctrl_unit_t *) CL_CTRL_GLOB_ADDR(cid))

/* ----------------------------------------------------------------------------
   -- CL_TIMER Peripheral Access Layer
   ---------------------------------------------------------------------------- */
//#include "archi/timer_unit/timer_unit.h"
#define cl_timer(id) ((timer_periph_t *) CL_TIMER_ADDR)

/* ----------------------------------------------------------------------------
   -- CL_ICACHE_CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_icache_ctrl/cluster_icache_ctrl.h"
#define cl_icache(id)       ((cluster_icache_ctrl_t *) CL_ICACHE_ADDR)
#define cl_glob_icache(cid) ((cluster_icache_ctrl_t *) CL_GLOB_ICACHE_ADDR(cid))

/* ----------------------------------------------------------------------------
   -- CL_DEMUX_EVENT_UNIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_event_unit/cluster_event_unit.h"
#define cl_demux_eu_core(id)     ((cluster_eu_core_demux_t *) CL_DEMUX_EU_CORE_ADDR)
#define cl_demux_eu_loop(id)     ((cluster_eu_loop_demux_t *) CL_DEMUX_EU_LOOP_ADDR)
#define cl_demux_eu_dispatch(id) ((cluster_eu_dispatch_demux_t *) CL_DEMUX_EU_DISPATCH_ADDR)
#define cl_demux_eu_mutex(id)    ((cluster_eu_mutex_demux_t *) CL_DEMUX_EU_HW_MUTEX_ADDR)
#define cl_demux_eu_sw_evt(id)   ((cluster_eu_sw_evt_demux_t *) CL_DEMUX_EU_SW_EVENT_ADDR)
#define cl_demux_eu_barrier(id)  ((cluster_eu_barrier_demux_t *) (CL_DEMUX_EU_HW_BARRIER_ADDR + (id * sizeof(cluster_eu_barrier_demux_t))))

/* ----------------------------------------------------------------------------
   -- CL_EVENT_UNIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cluster_event_unit/cluster_event_unit.h"
#define cl_glob_eu_core(cid)     ((cluster_eu_core_demux_t *) CL_GLOB_EU_CORE_ADDR(cid))
#define cl_glob_eu_dispatch(cid) ((cluster_eu_dispatch_demux_t *) CL_GLOB_EU_DISPATCH_ADDR(cid))
#define cl_glob_eu_sw_evt(cid)   ((cluster_eu_sw_evt_demux_t *) CL_GLOB_EU_SW_EVENT_ADDR(cid))

/* ----------------------------------------------------------------------------
   -- CL_DMA Peripheral Access Layer
   ---------------------------------------------------------------------------- */
#include "periph/cluster/cl_dma/cl_dma.h"
#define cl_dma(id) ((cl_dma_t *) CL_DMA_ADDR)


/* ----------------------------------------------------------------------------
   -- HWCE Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/** HWCE - Registers Layout Typedef */
typedef struct {
    __IO  uint32_t HWCE_TRIGGER_REG;              /**< HWCE Trigger register, offset: 0x00 */
    __IO  uint32_t HWCE_ACQUIRE_REG;              /**< HWCE Acquire register, offset: 0x04 */
    __IO  uint32_t HWCE_FINISHED_REG;             /**< HWCE Finished register, offset: 0x08 */
    __IO  uint32_t HWCE_STATUS_REG;               /**< HWCE Status register, offset: 0x0C */
    __IO  uint32_t HWCE_RUNNING_JOB_REG;          /**< HWCE Running Job register, offset: 0x10 */
    __IO  uint32_t HWCE_SOFT_CLEAR_REG;           /**< HWCE Soft_Clear register, offset: 0x14 */
    __IO  uint32_t _reserved0[2];                 /**< HWCE Non used registers, offser: 0x18 */
    __IO  uint32_t HWCE_GEN_CONFIG0_REG;          /**< HWCE Gen_Config0 register, offset: 0x20 */
    __IO  uint32_t HWCE_GEN_CONFIG1_REG;          /**< HWCE Gen_Config1 register, offset: 0x24 */
    __IO  uint32_t _reserved1[6];                 /**< HWCE unused registers, offset: 0x28 */
    __IO  uint32_t HWCE_Y_TRANS_SIZE_REG;         /**< HWCE Y_Trans_Size register, offset: 0x40 */
    __IO  uint32_t HWCE_Y_LINE_STRIDE_LENGTH_REG; /**< HWCE Y_Line_Stride_Length register, offset: 0x44 */
    __IO  uint32_t HWCE_Y_FEAT_STRIDE_LENGTH_REG; /**< HWCE Y_Feat_Stride_Length register, offset: 0x48 */
    __IO  uint32_t HWCE_Y_OUT_3_REG;              /**< HWCE Y_Out_3 register, offset: 0x4C */
    __IO  uint32_t HWCE_Y_OUT_2_REG;              /**< HWCE Y_Out_2 register, offset: 0x50 */
    __IO  uint32_t HWCE_Y_OUT_1_REG;              /**< HWCE Y_Out_1 register, offset: 0x54 */
    __IO  uint32_t HWCE_Y_OUT_0_REG;              /**< HWCE Y_Out_0 register, offset: 0x58 */
    __IO  uint32_t HWCE_Y_IN_3_REG;               /**< HWCE Y_In_3 register, offset: 0x5C */
    __IO  uint32_t HWCE_Y_IN_2_REG;               /**< HWCE Y_In_2 register, offset: 0x60 */
    __IO  uint32_t HWCE_Y_IN_1_REG;               /**< HWCE Y_In_1 register, offset: 0x64 */
    __IO  uint32_t HWCE_Y_IN_0_REG;               /**< HWCE Y_In_0 register, offset: 0x68 */
    __IO  uint32_t HWCE_X_TRANS_SIZE_REG;         /**< HWCE X_Trans_Size register, offset: 0x6C */
    __IO  uint32_t HWCE_X_LINE_STRIDE_LENGTH_REG; /**< HWCE X_Line_Stride_Length register, offset: 0x70 */
    __IO  uint32_t HWCE_X_FEAT_STRIDE_LENGTH_REG; /**< HWCE X_Feat_Stride_Length register, offset: 0x74 */
    __IO  uint32_t HWCE_X_IN_REG;                 /**< HWCE X_In register, offset: 0x78 */
    __IO  uint32_t HWCE_W_REG;                    /**< HWCE W register, offset: 0x7C */
    __IO  uint32_t HWCE_JOB_CONFIG0_REG;          /**< HWCE Job_Config0 register, offset: 0x80 */
    __IO  uint32_t HWCE_JOB_CONFIG1_REG;          /**< HWCE Job_Config1 register, offset: 0x84 */
} HWCE_Type;


/* ----------------------------------------------------------------------------
   -- HWCE Register Masks
   ---------------------------------------------------------------------------- */

/*! @name INFO - HWCE information register */
/* Internal registers */
#define HWCE_TRIGGER              ( 0x00 )
#define HWCE_ACQUIRE              ( 0x04 )
#define HWCE_FINISHED             ( 0x08 )
#define HWCE_STATUS               ( 0x0C )
#define HWCE_RUNNING_JOB          ( 0x10 )
#define HWCE_SOFT_CLEAR           ( 0x14 )
#define HWCE_GEN_CONFIG0          ( 0x20 )
#define HWCE_GEN_CONFIG1          ( 0x24 )

/* Configuration registers */
#define HWCE_Y_TRANS_SIZE         ( 0x40 )
#define HWCE_Y_LINE_STRIDE_LENGTH ( 0x44 )
#define HWCE_Y_FEAT_STRIDE_LENGTH ( 0x48 )
#define HWCE_Y_OUT_3_BASE_ADDR    ( 0x4C )
#define HWCE_Y_OUT_2_BASE_ADDR    ( 0x50 )
#define HWCE_Y_OUT_1_BASE_ADDR    ( 0x54 )
#define HWCE_Y_OUT_0_BASE_ADDR    ( 0x58 )
#define HWCE_Y_IN_3_BASE_ADDR     ( 0x5C )
#define HWCE_Y_IN_2_BASE_ADDR     ( 0x60 )
#define HWCE_Y_IN_1_BASE_ADDR     ( 0x64 )
#define HWCE_Y_IN_0_BASE_ADDR     ( 0x68 )
#define HWCE_X_TRANS_SIZE         ( 0x6C )
#define HWCE_X_LINE_STRIDE_LENGTH ( 0x70 )
#define HWCE_X_FEAT_STRIDE_LENGTH ( 0x74 )
#define HWCE_X_IN_BASE_ADDR       ( 0x78 )
#define HWCE_W_BASE_ADDR          ( 0x7C )
#define HWCE_JOB_CONFIG0          ( 0x80 )
#define HWCE_JOB_CONFIG1          ( 0x84 )

#define HWCE_NB_IO_REGS           ( 18 )

#define HWCE_ACQUIRE_CONTEXT_COPY ( -3 )
#define HWCE_ACQUIRE_LOCKED       ( -2 )
#define HWCE_ACQUIRE_QUEUE_FULL   ( -1 )
#define HWCE_ACQUIRE_READY        ( 0 )

#define HWCE_GEN_CONFIG0_WSTRIDE(x)           ((x) >> 16)
#define HWCE_GEN_CONFIG0_NCP(x)               (((x) >> 13) & 0x1)
#define HWCE_GEN_CONFIG0_CONV(x)              (((x) >> 11) & 0x3)
#define HWCE_GEN_CONFIG0_VECT(x)              (((x) >> 9) & 0x3)
#define HWCE_GEN_CONFIG0_UNS(x)               (((x) >> 8) & 1)
#define HWCE_GEN_CONFIG0_NY(x)                (((x) >> 7) & 1)
#define HWCE_GEN_CONFIG0_NF(x)                (((x) >> 6) & 1)
#define HWCE_GEN_CONFIG0_QF(x)                ((x) & 0x3f)

#define HWCE_GEN_CONFIG0_CONV_5x5 ( 0 )
#define HWCE_GEN_CONFIG0_CONV_3x3 ( 1 )
#define HWCE_GEN_CONFIG0_CONV_4x7 ( 2 )

#define HWCE_GEN_CONFIG0_VECT_1   ( 0 )
#define HWCE_GEN_CONFIG0_VECT_2   ( 1 )
#define HWCE_GEN_CONFIG0_VECT_4   ( 2 )

#define HWCE_GEN_CONFIG1_PIXSHIFTR(x)         (((x) >> 16) & 0x1F)
#define HWCE_GEN_CONFIG1_PIXMODE(x)           (((x) >> 8) & 0x3)
#define HWCE_GEN_CONFIG1_PIXSHIFTL(x)         (((x) >> 0) & 0x1F)

#define HWCE_JOB_CONFIG0_NOYCONST(x)          ((x) >> 16)
#define HWCE_JOB_CONFIG0_LBUFLEN(x)           ((x) & 0x3ff)

#define HWCE_JOB_CONFIG1_LO(x)                (((x) >> 24) & 0x1)
#define HWCE_JOB_CONFIG1_WIF(x)               (((x) >> 16) & 0x3F)
#define HWCE_JOB_CONFIG1_WOF(x)               (((x) >> 8) & 0x1F)
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK(x) (((x) >> 0) & 0xF)

#define HWCE_JOB_STRIDE(x)                    ((x) >> 16)
#define HWCE_JOB_LENGTH(x)                    ((x) & 0xffff)


/* HWCE - Peripheral instance base addresses */
/** Peripheral HWCE base address */
#define HWCE_BASE                                (CORE_PERI_BASE + 0x00001000)
/** Peripheral HWCE base pointer */
#define HWCE                                     ((HWCE_Type *) HWCE_BASE)
/** Array initializer of HWCE base addresses */
#define HWCE_BASE_ADDRS                          { HWCE_BASE }
/** Array initializer of HWCE base pointers */
#define HWCE_BASE_PTRS                           { HWCE }

#endif  /* FEATURE_CLUSTER */


/* ----------------------------------------------------------------------------
   -- STDOUT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/** STDOUT - Registers Layout Typedef */
typedef struct
{
    volatile uint32_t putc[16];
} stdout_t;

/* ----------------------------------------------------------------------------
   -- STDOUT Register Offset --
   ---------------------------------------------------------------------------- */

#define STDOUT_PUTC_OFFSET         0x0


#define _stdout(cid)    ((stdout_t *) (STDOUT_ADDR + (cid << 7)))


#endif  /* __PMSIS_TARGETS_VEGA_PERIPH_H__ */
