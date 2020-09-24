/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
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

#ifndef __CLUSTER_CTRL_UNIT_PERIPH_H__
#define __CLUSTER_CTRL_UNIT_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- REGMAP Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** REGMAP_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t eoc; /**< End Of Computation status register. */
    volatile uint32_t reserved_0[1]; /**< Reserved/Unused. */
    volatile uint32_t fetch_en; /**< Cluster cores fetch enable configuration register. */
    volatile uint32_t reserved_1[5]; /**< Reserved/Unused. */
    volatile uint32_t clock_gate; /**< Cluster clock gate configuration register. */
    volatile uint32_t reserved_2[1]; /**< Reserved/Unused. */
    volatile uint32_t dbg_halt_status_resume; /**< Cluster cores debug halt status/resume register. */
    volatile uint32_t reserved_3[3]; /**< Reserved/Unused. */
    volatile uint32_t dbg_halt_mask; /**< Cluster cores debug halt mask configuration register. */
    volatile uint32_t reserved_4[1]; /**< Reserved/Unused. */
    volatile uint32_t boot_addr0; /**< Cluster core 0 boot address configuration register. */
    volatile uint32_t boot_addr1; /**< Cluster core 1 boot address configuration register. */
    volatile uint32_t boot_addr2; /**< Cluster core 2 boot address configuration register. */
    volatile uint32_t boot_addr3; /**< Cluster core 3 boot address configuration register. */
    volatile uint32_t boot_addr4; /**< Cluster core 4 boot address configuration register. */
    volatile uint32_t boot_addr5; /**< Cluster core 5 boot address configuration register. */
    volatile uint32_t boot_addr6; /**< Cluster core 6 boot address configuration register. */
    volatile uint32_t boot_addr7; /**< Cluster core 7 boot address configuration register. */
    volatile uint32_t reserved_5[8]; /**< Reserved/Unused. */
    volatile uint32_t tcdm_arb_policy_ch0; /**< TCDM arbitration policy ch0 for cluster cores configuration register. */
    volatile uint32_t reserved_6[1]; /**< Reserved/Unused. */
    volatile uint32_t tcdm_arb_policy_ch1; /**< TCDM arbitration policy ch1 for DMA/HWCE configuration register. */
    volatile uint32_t reserved_7[13]; /**< Reserved/Unused. */
    volatile uint32_t tcdm_arb_policy_ch0_rep; /**< Read only duplicate of TCDM_ARB_POLICY_CH0 register */
    volatile uint32_t reserved_8[1]; /**< Reserved/Unused. */
    volatile uint32_t tcdm_arb_policy_ch1_rep; /**< Read only duplicate of TCDM_ARB_POLICY_CH1 register */
} cluster_ctrl_unit_t;


/* ----------------------------------------------------------------------------
   -- REGMAP Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name EOC */
/* End of computation status flag bitfield:
  - 1'b0: program execution under going
  - 1'b1: end of computation reached */
#define CLUSTER_CTRL_UNIT_EOC_EOC_MASK                        (0x1)
#define CLUSTER_CTRL_UNIT_EOC_EOC_SHIFT                       (0)
#define CLUSTER_CTRL_UNIT_EOC_EOC(val)                        (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_EOC_EOC_SHIFT)) & CLUSTER_CTRL_UNIT_EOC_EOC_MASK)


/*! @name FETCH_EN */
/* Core 0 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK                 (0x1)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SHIFT                (0)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK)

/* Core 1 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK                 (0x2)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SHIFT                (1)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK)

/* Core 2 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK                 (0x4)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SHIFT                (2)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK)

/* Core 3 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK                 (0x8)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SHIFT                (3)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK)

/* Core 4 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK                 (0x10)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SHIFT                (4)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK)

/* Core 5 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK                 (0x20)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SHIFT                (5)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK)

/* Core 6 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK                 (0x40)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SHIFT                (6)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK)

/* Core 7 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK                 (0x80)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SHIFT                (7)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7(val)                 (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SHIFT)) & CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK)


/*! @name CLOCK_GATE */
/* Cluster clock gate configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK                  (0x1)
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SHIFT                 (0)
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(val)                  (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SHIFT)) & CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK)


/*! @name DBG_HALT_STATUS_RESUME */
/* W : Core 0 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 0 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 0 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_MASK   (0x1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_SHIFT  (0)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_MASK)

/* W : Core 1 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 1 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 1 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_MASK   (0x2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_SHIFT  (1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_MASK)

/* W : Core 2 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 2 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 2 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_MASK   (0x4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_SHIFT  (2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_MASK)

/* W : Core 3 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 3 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 3 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_MASK   (0x8)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_SHIFT  (3)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_MASK)

/* W : Core 4 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 4 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 4 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_MASK   (0x10)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_SHIFT  (4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_MASK)

/* W : Core 5 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 5 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 5 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_MASK   (0x20)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_SHIFT  (5)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_MASK)

/* W : Core 6 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 6 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 6 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_MASK   (0x40)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_SHIFT  (6)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_MASK)

/* W : Core 7 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 7 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 7 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_MASK   (0x80)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_SHIFT  (7)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7(val)   (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_MASK)


/*! @name DBG_HALT_MASK */
/* Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK            (0x1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SHIFT           (0)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK)

/* Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK            (0x2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SHIFT           (1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK)

/* Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK            (0x4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SHIFT           (2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK)

/* Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK            (0x8)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SHIFT           (3)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK)

/* Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK            (0x10)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SHIFT           (4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK)

/* Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK            (0x20)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SHIFT           (5)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK)

/* Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK            (0x40)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SHIFT           (6)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK)

/* Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK            (0x80)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SHIFT           (7)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7(val)            (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SHIFT)) & CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK)


/*! @name BOOT_ADDR0 */
/* Cluster core 0 boot address configuration bitfield. */
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK                  (0xffffffff)
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SHIFT                 (0)
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA(val)                  (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SHIFT)) & CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK)


/*! @name BOOT_ADDR1 */

/*! @name BOOT_ADDR2 */

/*! @name BOOT_ADDR3 */

/*! @name BOOT_ADDR4 */

/*! @name BOOT_ADDR5 */

/*! @name BOOT_ADDR6 */

/*! @name BOOT_ADDR7 */

/*! @name TCDM_ARB_POLICY_CH0 */
/* TCDM arbitration policy for cluster cores configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK        (0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SHIFT       (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL(val)        (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SHIFT)) & CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK)


/*! @name TCDM_ARB_POLICY_CH1 */
/* TCDM arbitration policy for DMA/HWCE configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK        (0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SHIFT       (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL(val)        (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SHIFT)) & CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK)


/*! @name TCDM_ARB_POLICY_CH0_REP */
/* TCDM arbitration policy for cluster cores configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK    (0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SHIFT   (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL(val)    (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SHIFT)) & CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK)


/*! @name TCDM_ARB_POLICY_CH1_REP */
/* TCDM arbitration policy for DMA/HWCE configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK    (0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SHIFT   (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL(val)    (((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SHIFT)) & CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK)


/*! @name EOC */
typedef union
{
    struct
    {
        /* End of computation status flag bitfield:
        - 1'b0: program execution under going
        - 1'b1: end of computation reached */
        uint32_t eoc:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_eoc_t;

/*! @name FETCH_EN */
typedef union
{
    struct
    {
        /* Core 0 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core0:1;
        /* Core 1 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core1:1;
        /* Core 2 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core2:1;
        /* Core 3 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core3:1;
        /* Core 4 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core4:1;
        /* Core 5 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core5:1;
        /* Core 6 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core6:1;
        /* Core 7 fetch enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t core7:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_fetch_en_t;

/*! @name CLOCK_GATE */
typedef union
{
    struct
    {
        /* Cluster clock gate configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t en:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_clock_gate_t;

/*! @name DBG_HALT_STATUS_RESUME */
typedef union
{
    struct
    {
        /* W : Core 0 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 0 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 0 */
        uint32_t core0:1;
        /* W : Core 1 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 1 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 1 */
        uint32_t core1:1;
        /* W : Core 2 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 2 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 2 */
        uint32_t core2:1;
        /* W : Core 3 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 3 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 3 */
        uint32_t core3:1;
        /* W : Core 4 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 4 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 4 */
        uint32_t core4:1;
        /* W : Core 5 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 5 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 5 */
        uint32_t core5:1;
        /* W : Core 6 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 6 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 6 */
        uint32_t core6:1;
        /* W : Core 7 debug halt status flag bitfield:
        - 1'b0: running
        - 1'b1: halted
        R : Core 7 debug resume configuration bitfield:
        - 1'b0: stay halted
        - 1'b1: resume core 7 */
        uint32_t core7:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_dbg_halt_status_resume_t;

/*! @name DBG_HALT_MASK */
typedef union
{
    struct
    {
        /* Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core0:1;
        /* Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core1:1;
        /* Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core2:1;
        /* Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core3:1;
        /* Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core4:1;
        /* Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core5:1;
        /* Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core6:1;
        /* Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. */
        uint32_t core7:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_dbg_halt_mask_t;

/*! @name BOOT_ADDR0 */
typedef union
{
    struct
    {
        /* Cluster core 0 boot address configuration bitfield. */
        uint32_t ba:32;
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr0_t;

/*! @name BOOT_ADDR1 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr1_t;

/*! @name BOOT_ADDR2 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr2_t;

/*! @name BOOT_ADDR3 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr3_t;

/*! @name BOOT_ADDR4 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr4_t;

/*! @name BOOT_ADDR5 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr5_t;

/*! @name BOOT_ADDR6 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr6_t;

/*! @name BOOT_ADDR7 */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} cluster_ctrl_unit_boot_addr7_t;

/*! @name TCDM_ARB_POLICY_CH0 */
typedef union
{
    struct
    {
        /* TCDM arbitration policy for cluster cores configuration bitfield:
        - 1'b0: fair round robin
        - 1'b1: fixed order */
        uint32_t pol:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_tcdm_arb_policy_ch0_t;

/*! @name TCDM_ARB_POLICY_CH1 */
typedef union
{
    struct
    {
        /* TCDM arbitration policy for DMA/HWCE configuration bitfield:
        - 1'b0: fair round robin
        - 1'b1: fixed order */
        uint32_t pol:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_tcdm_arb_policy_ch1_t;

/*! @name TCDM_ARB_POLICY_CH0_REP */
typedef union
{
    struct
    {
        /* TCDM arbitration policy for cluster cores configuration bitfield:
        - 1'b0: fair round robin
        - 1'b1: fixed order */
        uint32_t pol:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_t;

/*! @name TCDM_ARB_POLICY_CH1_REP */
typedef union
{
    struct
    {
        /* TCDM arbitration policy for DMA/HWCE configuration bitfield:
        - 1'b0: fair round robin
        - 1'b1: fixed order */
        uint32_t pol:1;
    } field;
    uint32_t word;
} cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_t;



#endif /* __CLUSTER_CTRL_UNIT_PERIPH_H__ */
