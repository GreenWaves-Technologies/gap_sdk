/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __HYPER_PERIPH_H__
#define __HYPER_PERIPH_H__

//#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- HYPERBUS Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** HYPERBUS_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t ext_addr; /**< External Memory address register. */
    volatile uint32_t ext_cfg; /**< External access config register. */
    volatile uint32_t mem_cfg0; /**< Memory access address register. */
    volatile uint32_t mem_cfg1; /**< Memory access address register. */
    volatile uint32_t mem_cfg2; /**< Memory access address register. */
    volatile uint32_t mem_cfg3; /**< Memory access address register. */
    volatile uint32_t mem_cfg4; /**< Memory access address register. */
    volatile uint32_t mem_cfg5; /**< Memory access address register. */
    volatile uint32_t mem_cfg6; /**< Memory access address register. */
    volatile uint32_t mem_cfg7; /**< Memory access address register. */
} hyperbus_t;


/* ----------------------------------------------------------------------------
   -- HYPERBUS Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name EXT_ADDR */
/* Memory access address bitfield. */
#define HYPERBUS_EXT_ADDR_EXT_ADDR_MASK                (0x7fffffff)
#define HYPERBUS_EXT_ADDR_EXT_ADDR_SHIFT               (0)
#define HYPERBUS_EXT_ADDR_EXT_ADDR(val)                (((uint32_t)(((uint32_t)(val)) << HYPERBUS_EXT_ADDR_EXT_ADDR_SHIFT)) & HYPERBUS_EXT_ADDR_EXT_ADDR_MASK)

/* Register access flag bitfield. */
#define HYPERBUS_EXT_ADDR_REG_ACCESS_MASK              (0x80000000)
#define HYPERBUS_EXT_ADDR_REG_ACCESS_SHIFT             (31)
#define HYPERBUS_EXT_ADDR_REG_ACCESS(val)              (((uint32_t)(((uint32_t)(val)) << HYPERBUS_EXT_ADDR_REG_ACCESS_SHIFT)) & HYPERBUS_EXT_ADDR_REG_ACCESS_MASK)

/*! @name MEM_CFG0 - HYPERBUS Memory control Configuration register0 */
#define HYPERBUS_MEM_CFG0_MBR0_MASK                    (0xFFU)
#define HYPERBUS_MEM_CFG0_MBR0_SHIFT                   (0U)
#define HYPERBUS_MEM_CFG0_MBR0(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG0_MBR0_SHIFT)) & HYPERBUS_MEM_CFG0_MBR0_MASK)
#define HYPERBUS_MEM_CFG0_LATENCY0_MASK                (0xF00U)
#define HYPERBUS_MEM_CFG0_LATENCY0_SHIFT               (8U)
#define HYPERBUS_MEM_CFG0_LATENCY0(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG0_LATENCY0_SHIFT)) & HYPERBUS_MEM_CFG0_LATENCY0_MASK)
#define HYPERBUS_MEM_CFG0_WRAP_SIZE0_MASK              (0x3000U)
#define HYPERBUS_MEM_CFG0_WRAP_SIZE0_SHIFT             (12U)
#define HYPERBUS_MEM_CFG0_WRAP_SIZE0(x)                (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG0_WRAP_SIZE0_SHIFT)) & HYPERBUS_MEM_CFG0_WRAP_SIZE0_MASK)

/*! @name MEM_CFG1 - HYPERBUS Memory control Configuration register1 */
#define HYPERBUS_MEM_CFG1_RD_CSHI0_MASK                (0xFU)
#define HYPERBUS_MEM_CFG1_RD_CSHI0_SHIFT               (0U)
#define HYPERBUS_MEM_CFG1_RD_CSHI0(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG1_RD_CSHI0_SHIFT)) & HYPERBUS_MEM_CFG1_RD_CSHI0_MASK)
#define HYPERBUS_MEM_CFG1_RD_CSS0_MASK                 (0xF0U)
#define HYPERBUS_MEM_CFG1_RD_CSS0_SHIFT                (4U)
#define HYPERBUS_MEM_CFG1_RD_CSS0(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG1_RD_CSS0_SHIFT)) & HYPERBUS_MEM_CFG1_RD_CSS0_MASK)
#define HYPERBUS_MEM_CFG1_RD_CSH0_MASK                 (0xF00U)
#define HYPERBUS_MEM_CFG1_RD_CSH0_SHIFT                (8U)
#define HYPERBUS_MEM_CFG1_RD_CSH0(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG1_RD_CSH0_SHIFT)) & HYPERBUS_MEM_CFG1_RD_CSH0_MASK)
#define HYPERBUS_MEM_CFG1_WR_CSHI0_MASK                (0xF000U)
#define HYPERBUS_MEM_CFG1_WR_CSHI0_SHIFT               (12U)
#define HYPERBUS_MEM_CFG1_WR_CSHI0(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG1_WR_CSHI0_SHIFT)) & HYPERBUS_MEM_CFG1_WR_CSHI0_MASK)
#define HYPERBUS_MEM_CFG1_WR_CSS0_MASK                 (0xF0000U)
#define HYPERBUS_MEM_CFG1_WR_CSS0_SHIFT                (16U)
#define HYPERBUS_MEM_CFG1_WR_CSS0(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG1_WR_CSS0_SHIFT)) & HYPERBUS_MEM_CFG1_WR_CSS0_MASK)
#define HYPERBUS_MEM_CFG1_WR_CSH0_MASK                 (0xF00000U)
#define HYPERBUS_MEM_CFG1_WR_CSH0_SHIFT                (20U)
#define HYPERBUS_MEM_CFG1_WR_CSH0(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG1_WR_CSH0_SHIFT)) & HYPERBUS_MEM_CFG1_WR_CSH0_MASK)


/*! @name MEM_CFG2 - HYPERBUS Memory control Configuration register2 */
#define HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0_MASK          (0x1FFU)
#define HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0_SHIFT         (0U)
#define HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0_SHIFT)) & HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0_MASK)
#define HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0_MASK          (0x1FF0000U)
#define HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0_SHIFT         (16U)
#define HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0_SHIFT)) & HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0_MASK)

/*! @name MEM_CFG3 - HYPERBUS Memory control Configuration register3 */
#define HYPERBUS_MEM_CFG3_ACS0_MASK                    (0x1U)
#define HYPERBUS_MEM_CFG3_ACS0_SHIFT                   (0U)
#define HYPERBUS_MEM_CFG3_ACS0(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_ACS0_SHIFT)) & HYPERBUS_MEM_CFG3_ACS0_MASK)
#define HYPERBUS_MEM_CFG3_TCO0_MASK                    (0x2U)
#define HYPERBUS_MEM_CFG3_TCO0_SHIFT                   (1U)
#define HYPERBUS_MEM_CFG3_TCO0(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_TCO0_SHIFT)) & HYPERBUS_MEM_CFG3_TCO0_MASK)
#define HYPERBUS_MEM_CFG3_DT0_MASK                     (0x4U)
#define HYPERBUS_MEM_CFG3_DT0_SHIFT                    (2U)
#define HYPERBUS_MEM_CFG3_DT0(x)                       (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_DT0_SHIFT)) & HYPERBUS_MEM_CFG3_DT0_MASK)
#define HYPERBUS_MEM_CFG3_CRT0_MASK                    (0x8U)
#define HYPERBUS_MEM_CFG3_CRT0_SHIFT                   (3U)
#define HYPERBUS_MEM_CFG3_CRT0(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_CRT0_SHIFT)) & HYPERBUS_MEM_CFG3_CRT0_MASK)
#define HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0_MASK          (0x10U)
#define HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0_SHIFT         (4U)
#define HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0_SHIFT)) & HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0_MASK)
#define HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0_MASK          (0x20U)
#define HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0_SHIFT         (5U)
#define HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0_SHIFT)) & HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0_MASK)
#define HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ_MASK           (0x300U)
#define HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ_SHIFT          (8U)
#define HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ(x)             (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ_SHIFT)) & HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ_MASK)


/*! @name MEM_CFG4 - HYPERBUS Memory control Configuration register4 */
#define HYPERBUS_MEM_CFG4_MBR1_MASK                    (0xFFU)
#define HYPERBUS_MEM_CFG4_MBR1_SHIFT                   (0U)
#define HYPERBUS_MEM_CFG4_MBR1(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG4_MBR1_SHIFT)) & HYPERBUS_MEM_CFG4_MBR1_MASK)
#define HYPERBUS_MEM_CFG4_LATENCY1_MASK                (0xF00U)
#define HYPERBUS_MEM_CFG4_LATENCY1_SHIFT                (8U)
#define HYPERBUS_MEM_CFG4_LATENCY1(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG4_LATENCY1_SHIFT)) & HYPERBUS_MEM_CFG4_LATENCY1_MASK)
#define HYPERBUS_MEM_CFG4_WRAP_SIZE1_MASK              (0x3000U)
#define HYPERBUS_MEM_CFG4_WRAP_SIZE1_SHIFT             (12U)
#define HYPERBUS_MEM_CFG4_WRAP_SIZE1(x)                (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG4_WRAP_SIZE1_SHIFT)) & HYPERBUS_MEM_CFG4_WRAP_SIZE1_MASK)


/*! @name MEM_CFG5 - HYPERBUS Memory control Configuration register5 */
#define HYPERBUS_MEM_CFG5_RD_CSHI1_MASK                (0xFU)
#define HYPERBUS_MEM_CFG5_RD_CSHI1_SHIFT               (0U)
#define HYPERBUS_MEM_CFG5_RD_CSHI1(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG5_RD_CSHI1_SHIFT)) & HYPERBUS_MEM_CFG5_RD_CSHI1_MASK)
#define HYPERBUS_MEM_CFG5_RD_CSS1_MASK                 (0xF0U)
#define HYPERBUS_MEM_CFG5_RD_CSS1_SHIFT                (4U)
#define HYPERBUS_MEM_CFG5_RD_CSS1(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG5_RD_CSS1_SHIFT)) & HYPERBUS_MEM_CFG5_RD_CSS1_MASK)
#define HYPERBUS_MEM_CFG5_RD_CSH1_MASK                 (0xF00U)
#define HYPERBUS_MEM_CFG5_RD_CSH1_SHIFT                (8U)
#define HYPERBUS_MEM_CFG5_RD_CSH1(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG5_RD_CSH1_SHIFT)) & HYPERBUS_MEM_CFG5_RD_CSH1_MASK)
#define HYPERBUS_MEM_CFG5_WR_CSHI1_MASK                (0xF000U)
#define HYPERBUS_MEM_CFG5_WR_CSHI1_SHIFT               (12U)
#define HYPERBUS_MEM_CFG5_WR_CSHI1(x)                  (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG5_WR_CSHI1_SHIFT)) & HYPERBUS_MEM_CFG5_WR_CSHI1_MASK)
#define HYPERBUS_MEM_CFG5_WR_CSS1_MASK                 (0xF0000U)
#define HYPERBUS_MEM_CFG5_WR_CSS1_SHIFT                (16U)
#define HYPERBUS_MEM_CFG5_WR_CSS1(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG5_WR_CSS1_SHIFT)) & HYPERBUS_MEM_CFG5_WR_CSS1_MASK)
#define HYPERBUS_MEM_CFG5_WR_CSH1_MASK                 (0xF00000U)
#define HYPERBUS_MEM_CFG5_WR_CSH1_SHIFT                (20U)
#define HYPERBUS_MEM_CFG5_WR_CSH1(x)                   (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG5_WR_CSH1_SHIFT)) & HYPERBUS_MEM_CFG5_WR_CSH1_MASK)

/*! @name MEM_CFG6 - HYPERBUS Memory control Configuration register6 */
#define HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1_MASK          (0x1FFU)
#define HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1_SHIFT         (0U)
#define HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1_SHIFT)) & HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1_MASK)
#define HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1_MASK          (0x1FF0000U)
#define HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1_SHIFT         (16U)
#define HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1_SHIFT)) & HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1_MASK)


/*! @name MEM_CFG7 - HYPERBUS Memory control Configuration register7 */
#define HYPERBUS_MEM_CFG7_ACS1_MASK                    (0x1U)
#define HYPERBUS_MEM_CFG7_ACS1_SHIFT                   (0U)
#define HYPERBUS_MEM_CFG7_ACS1(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG7_ACS1_SHIFT)) & HYPERBUS_MEM_CFG7_ACS1_MASK)
#define HYPERBUS_MEM_CFG7_TCO1_MASK                    (0x2U)
#define HYPERBUS_MEM_CFG7_TCO1_SHIFT                   (1U)
#define HYPERBUS_MEM_CFG7_TCO1(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG7_TCO1_SHIFT)) & HYPERBUS_MEM_CFG7_TCO1_MASK)
#define HYPERBUS_MEM_CFG7_DT1_MASK                     (0x4U)
#define HYPERBUS_MEM_CFG7_DT1_SHIFT                    (2U)
#define HYPERBUS_MEM_CFG7_DT1(x)                       (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG7_DT1_SHIFT)) & HYPERBUS_MEM_CFG7_DT1_MASK)
#define HYPERBUS_MEM_CFG7_CRT1_MASK                    (0x8U)
#define HYPERBUS_MEM_CFG7_CRT1_SHIFT                   (3U)
#define HYPERBUS_MEM_CFG7_CRT1(x)                      (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG7_CRT1_SHIFT)) & HYPERBUS_MEM_CFG7_CRT1_MASK)
#define HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1_MASK          (0x10U)
#define HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1_SHIFT         (4U)
#define HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1_SHIFT)) & HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1_MASK)
#define HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1_MASK          (0x20U)
#define HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1_SHIFT         (5U)
#define HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1(x)            (((uint32_t)(((uint32_t)(x)) << HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1_SHIFT)) & HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1_MASK)


#endif /* __HYPER_PERIPH_H__ */
