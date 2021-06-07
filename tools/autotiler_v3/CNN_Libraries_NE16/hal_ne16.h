/*
 * Copyright (C) 2018-2019 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors:  Francesco Conti <fconti@iis.ee.ethz.ch>
 *           Renzo Andri <andrire@iis.ee.ethz.ch>
 */
#include <stdio.h>

#ifndef __HAL_NE16_H__
#define __HAL_NE16_H__

/* REGISTER MAP */

// Special gvsoc register to enable traces
#define NE16_SPECIAL_TRACE_REG 0x60
#define NE16_L0_JOB_START_END 0
#define NE16_L1_CONFIG 1
#define NE16_L2_ACTIV_INOUT 2
#define NE16_L3_ALL 3

// global address map + event IDs
#define NE16_ADDR_BASE      0x00201000
#define CLUS_CTRL_ADDR_BASE 0x00200000
#define NE16_EVT0           12
#define NE16_EVT1           13

// commands
#define NE16_COMMIT_AND_TRIGGER 0x00
#define NE16_ACQUIRE            0x04
#define NE16_FINISHED           0x08
#define NE16_STATUS             0x0c
#define NE16_RUNNING_JOB        0x10
#define NE16_SOFT_CLEAR         0x14
#define NE16_SWSYNC             0x18
#define NE16_URISCY_IMEM        0x1c

// generic configs
#define NE16_FILTER_MODE_3x3            0x0
#define NE16_FILTER_MODE_3x3_DW         0x1
#define NE16_FILTER_MODE_1x1            0x2
#define NE16_BITS_8BIT                  0x0
#define NE16_BITS_16BIT                 0x1
#define NE16_BITS_32BIT                 0x2

// job configuration
#define NE16_REGISTER_OFFS       0x20
#define NE16_REGISTER_CXT0_OFFS  0x80
#define NE16_REGISTER_CXT1_OFFS  0x120
#define NE16_REG_WEIGHTS_PTR     0x00
#define NE16_REG_INFEAT_PTR      0x04
#define NE16_REG_OUTFEAT_PTR     0x08
#define NE16_REG_SCALE_PTR       0x0c
#define NE16_REG_SCALE_SHIFT_PTR 0x10
#define NE16_REG_SCALE_BIAS_PTR  0x14

#define NE16_REG_INFEAT_D0_STRIDE       0x18
#define NE16_REG_INFEAT_D1_STRIDE       0x1c
#define NE16_REG_INFEAT_D2_STRIDE       0x20
#define NE16_REG_OUTFEAT_D0_STRIDE      0x24
#define NE16_REG_OUTFEAT_D1_STRIDE      0x28
#define NE16_REG_OUTFEAT_D2_STRIDE      0x2c
#define NE16_REG_WEIGHTS_D0_STRIDE      0x30
#define NE16_REG_WEIGHTS_D1_STRIDE      0x34
#define NE16_REG_WEIGHTS_D2_STRIDE      0x38
#define NE16_REG_REM_KO_KI              0x3c
  #define NE16_SHIFT_REM_KI               0
  #define NE16_MASK_REM_KI                0xffff
  #define NE16_SHIFT_REM_KO               16
  #define NE16_MASK_REM_KO                0xffff
#define NE16_REG_REM_HO_WO              0x40
  #define NE16_SHIFT_REM_WO               0
  #define NE16_MASK_REM_WO                0xffff
  #define NE16_SHIFT_REM_HO               16
  #define NE16_MASK_REM_HO                0xffff
#define NE16_REG_REM_HI_WI              0x44
  #define NE16_SHIFT_REM_WI               0
  #define NE16_MASK_REM_WI                0xffff
  #define NE16_SHIFT_REM_HI               16
  #define NE16_MASK_REM_HI                0xffff
#define NE16_REG_NB_KO_KI               0x48
  #define NE16_SHIFT_NB_KI                0
  #define NE16_MASK_NB_KI                 0xffff
  #define NE16_SHIFT_NB_KO                16
  #define NE16_MASK_NB_KO                 0xffff
#define NE16_REG_NB_HO_WO               0x4c
  #define NE16_SHIFT_NB_WO                0
  #define NE16_MASK_NB_WO                 0xffff
  #define NE16_SHIFT_NB_HO                16
  #define NE16_MASK_NB_HO                 0xffff
#define NE16_REG_PADDING                0x50
  #define NE16_SHIFT_PADDING_VALUE        0
  #define NE16_MASK_PADDING_VALUE         0xffff
  #define NE16_SHIFT_PADDING_LEFT         16
  #define NE16_MASK_PADDING_LEFT          0xf
  #define NE16_SHIFT_PADDING_BOTTOM       20
  #define NE16_MASK_PADDING_BOTTOM        0xf
  #define NE16_SHIFT_PADDING_RIGHT        24
  #define NE16_MASK_PADDING_RIGHT         0xf
  #define NE16_SHIFT_PADDING_TOP          28
  #define NE16_MASK_PADDING_TOP           0xf
#define NE16_REG_WEIGHT_OFFSET          0x54
#define NE16_REG_FILTER_MASK            0x58
  #define NE16_SHIFT_FILTER_MASK_LEFT     0
  #define NE16_MASK_FILTER_MASK_LEFT      0xff
  #define NE16_SHIFT_FILTER_MASK_BOTTOM   8
  #define NE16_MASK_FILTER_MASK_BOTTOM    0xff
  #define NE16_SHIFT_FILTER_MASK_RIGHT    16
  #define NE16_MASK_FILTER_MASK_RIGHT     0xff
  #define NE16_SHIFT_FILTER_MASK_TOP      24
  #define NE16_MASK_FILTER_MASK_TOP       0xff
#define NE16_REG_CONFIG                 0x5c
  #define NE16_SHIFT_WBITS_M1             0
  #define NE16_MASK_WBITS_M1              0x7
  #define NE16_SHIFT_MODE16               3
  #define NE16_MASK_MODE16                0x1
  #define NE16_SHIFT_OUTQUANT             4
  #define NE16_MASK_OUTQUANT              0x1
  #define NE16_SHIFT_FILTER_MODE          5
  #define NE16_MASK_FILTER_MODE           0x3
  #define NE16_SHIFT_LINEAR_MODE          7
  #define NE16_MASK_LINEAR_MODE           0x1
  #define NE16_SHIFT_STRIDED_MODE         8
  #define NE16_MASK_STRIDED_MODE          0x1
  #define NE16_SHIFT_NORM_BITS            12
  #define NE16_MASK_NORM_BITS             0x3
  #define NE16_SHIFT_STREAMIN             14
  #define NE16_MASK_STREAMIN              0x1
  #define NE16_SHIFT_WEIGHT_OFFSET_CFG    15
  #define NE16_MASK_WEIGHT_OFFSET_CFG     0x1
  #define NE16_SHIFT_QUANT_RIGHT_SHIFT    16
  #define NE16_MASK_QUANT_RIGHT_SHIFT     0xf
  #define NE16_SHIFT_QUANT_BITS           21
  #define NE16_MASK_QUANT_BITS            0x3
  #define NE16_SHIFT_QUANT_NORECT         23
  #define NE16_MASK_QUANT_NORECT          0x1
  #define NE16_SHIFT_NORM_SHIFT           24
  #define NE16_MASK_NORM_SHIFT            0x1
  #define NE16_SHIFT_NORM_BIAS            25
  #define NE16_MASK_NORM_BIAS             0x1

// cluster controller register offset and bits
#define CLUS_CTRL_HWPE_OFFS              0x18
#define CLUS_CTRL_HWPE_CG_EN_MASK        0x800
#define CLUS_CTRL_HWPE_HCI_PRIO_MASK     0x100
#define CLUS_CTRL_HWPE_HCI_MAXSTALL_MASK 0xff

// others
#define NE16_COMMIT_CMD  1
#define NE16_TRIGGER_CMD 0
#define NE16_SOFT_CLEAR_ALL   0
#define NE16_SOFT_CLEAR_STATE 1

/* LOW-LEVEL HAL */
// For all the following functions we use __builtin_pulp_OffsetedWrite and __builtin_pulp_OffsetedRead
// instead of classic load/store because otherwise the compiler is not able to correctly factorize
// the NE16 base in case several accesses are done, ending up with twice more code
#if defined(__riscv__) && !defined(RV_ISA_RV32)
  #define NE16_WRITE_CMD(offset, value)        __builtin_pulp_OffsetedWrite(value, (int volatile *)(NE16_ADDR_BASE), offset)
  #define NE16_WRITE_CMD_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + offset + be) = value
  // #define NE16_READ_CMD(offset)                (__builtin_pulp_OffsetedRead(*(int volatile *)(NE16_ADDR_BASE), offset))
  #define NE16_READ_CMD(ret, offset)           ret = (*(int volatile *)(NE16_ADDR_BASE + offset))

  #define NE16_WRITE_REG(offset, value)        __builtin_pulp_OffsetedWrite(value, (int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS), offset)
  #define NE16_WRITE_REG_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS + offset + be) = value
  // #define NE16_READ_REG(offset)                (__builtin_pulp_OffsetedRead(*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS), offset))
  #define NE16_READ_REG(ret, offset)           ret = (*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS + offset))

  #define NE16_WRITE_REG_CXT0(offset, value)        __builtin_pulp_OffsetedWrite(value, (int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT0_OFFS), offset)
  #define NE16_WRITE_REG_CXT0_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT0_OFFS + offset + be) = value
  #define NE16_READ_REG_CXT0(offset)                (__builtin_pulp_OffsetedRead(*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT0_OFFS), offset))

  #define NE16_WRITE_REG_CXT1(offset, value)        __builtin_pulp_OffsetedWrite(value, (int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT1_OFFS), offset)
  #define NE16_WRITE_REG_CXT1_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT1_OFFS + offset + be) = value
  #define NE16_READ_REG_CXT1(offset)                (__builtin_pulp_OffsetedRead(*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT1_OFFS), offset))
#else
  #define NE16_WRITE_CMD(offset, value)        *(int volatile *)(NE16_ADDR_BASE + offset) = value
  #define NE16_WRITE_CMD_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + offset + be) = value
  #define NE16_READ_CMD(ret, offset)           ret = (*(int volatile *)(NE16_ADDR_BASE + offset))

  #define NE16_WRITE_REG(offset, value)        *(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS + offset) = value
  #define NE16_WRITE_REG_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS + offset + be) = value
  #define NE16_READ_REG(ret, offset)           ret = (*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_OFFS + offset))

  #define NE16_WRITE_REG_CXT0(offset, value)        __builtin_pulp_OffsetedWrite(value, (int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT0_OFFS), offset)
  #define NE16_WRITE_REG_CXT0_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT0_OFFS + offset + be) = value
  #define NE16_READ_REG_CXT0(offset)                (__builtin_pulp_OffsetedRead(*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT0_OFFS), offset))

  #define NE16_WRITE_REG_CXT1(offset, value)        __builtin_pulp_OffsetedWrite(value, (int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT1_OFFS), offset)
  #define NE16_WRITE_REG_CXT1_BE(offset, value, be) *(char volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT1_OFFS + offset + be) = value
  #define NE16_READ_REG_CXT1(offset)                (__builtin_pulp_OffsetedRead(*(int volatile *)(NE16_ADDR_BASE + NE16_REGISTER_CXT1_OFFS), offset))
#endif

#define NE16_CG_ENABLE()  *(volatile int*) (CLUS_CTRL_ADDR_BASE + CLUS_CTRL_HWPE_OFFS) |=  CLUS_CTRL_HWPE_CG_EN_MASK
#define NE16_CG_DISABLE() *(volatile int*) (CLUS_CTRL_ADDR_BASE + CLUS_CTRL_HWPE_OFFS) &= ~CLUS_CTRL_HWPE_CG_EN_MASK

#define NE16_SETPRIORITY_CORE() *(volatile int*) (CLUS_CTRL_ADDR_BASE + CLUS_CTRL_HWPE_OFFS) &= ~CLUS_CTRL_HWPE_HCI_PRIO_MASK
#define NE16_SETPRIORITY_NE16() *(volatile int*) (CLUS_CTRL_ADDR_BASE + CLUS_CTRL_HWPE_OFFS) |=  CLUS_CTRL_HWPE_HCI_PRIO_MASK

#define NE16_RESET_MAXSTALL()  *(volatile int*) (CLUS_CTRL_ADDR_BASE + CLUS_CTRL_HWPE_OFFS) &= ~CLUS_CTRL_HWPE_HCI_MAXSTALL_MASK
#define NE16_SET_MAXSTALL(val) *(volatile int*) (CLUS_CTRL_ADDR_BASE + CLUS_CTRL_HWPE_OFFS) |=  (val & CLUS_CTRL_HWPE_HCI_MAXSTALL_MASK)

#define NE16_BARRIER_NOSTATUS()      eu_evt_maskWaitAndClr (1 << NE16_EVT0)
#define NE16_BARRIER()               do { eu_evt_maskWaitAndClr (1 << NE16_EVT0); } while((*(int volatile *)(NE16_ADDR_BASE + NE16_STATUS)) != 0)
#define NE16_BUSYWAIT()              do {                                         } while((*(int volatile *)(NE16_ADDR_BASE + NE16_STATUS)) != 0)
#define NE16_BARRIER_ACQUIRE(job_id) job_id = NE16_READ_CMD(job_id, NE16_ACQUIRE); \
                                     while(job_id < 0) { eu_evt_maskWaitAndClr (1 << NE16_EVT0); NE16_READ_CMD(job_id, NE16_ACQUIRE); };

#endif /* __HAL_NE16_H__ */
