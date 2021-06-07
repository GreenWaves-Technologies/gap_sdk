/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2021 ETH Zurich, University of Bologna
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

#ifndef __UDMA_CTRL_PERIPH_H__
#define __UDMA_CTRL_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- UDMA_CTRL Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** UDMA_CTRL_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t cg; /**< uDMA peripherals clock gate configuration */
    volatile uint32_t event; /**< uDMA peripherals external event configuration */
} udma_ctrl_t;


/* ----------------------------------------------------------------------------
   -- UDMA_CTRL Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name CG */
/* Defines uDMA peripherals clock gate configuration for LVDS */
#define UDMA_CTRL_CG_CG_LVDS_MASK                                    (0x1)
#define UDMA_CTRL_CG_CG_LVDS_SHIFT                                   (0)
#define UDMA_CTRL_CG_CG_LVDS(val)                                    (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_LVDS_SHIFT)) & UDMA_CTRL_CG_CG_LVDS_MASK)

/* Defines uDMA peripherals clock gate configuration for SPIM0 */
#define UDMA_CTRL_CG_CG_SPIM0_MASK                                   (0x1)
#define UDMA_CTRL_CG_CG_SPIM0_SHIFT                                  (0)
#define UDMA_CTRL_CG_CG_SPIM0(val)                                   (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_SPIM0_SHIFT)) & UDMA_CTRL_CG_CG_SPIM0_MASK)

/* Defines uDMA peripherals clock gate configuration for SPIM1 */
#define UDMA_CTRL_CG_CG_SPIM1_MASK                                   (0x1)
#define UDMA_CTRL_CG_CG_SPIM1_SHIFT                                  (0)
#define UDMA_CTRL_CG_CG_SPIM1(val)                                   (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_SPIM1_SHIFT)) & UDMA_CTRL_CG_CG_SPIM1_MASK)

/* Defines uDMA peripherals clock gate configuration for SPIM2 */
#define UDMA_CTRL_CG_CG_SPIM2_MASK                                   (0x1)
#define UDMA_CTRL_CG_CG_SPIM2_SHIFT                                  (0)
#define UDMA_CTRL_CG_CG_SPIM2(val)                                   (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_SPIM2_SHIFT)) & UDMA_CTRL_CG_CG_SPIM2_MASK)

/* Defines uDMA peripherals clock gate configuration for UART */
#define UDMA_CTRL_CG_CG_UART_MASK                                    (0x1)
#define UDMA_CTRL_CG_CG_UART_SHIFT                                   (0)
#define UDMA_CTRL_CG_CG_UART(val)                                    (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_UART_SHIFT)) & UDMA_CTRL_CG_CG_UART_MASK)

/* Defines uDMA peripherals clock gate configuration for I2C0 */
#define UDMA_CTRL_CG_CG_I2C0_MASK                                    (0x1)
#define UDMA_CTRL_CG_CG_I2C0_SHIFT                                   (0)
#define UDMA_CTRL_CG_CG_I2C0(val)                                    (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_I2C0_SHIFT)) & UDMA_CTRL_CG_CG_I2C0_MASK)

/* Defines uDMA peripherals clock gate configuration for I2C1 */
#define UDMA_CTRL_CG_CG_I2C1_MASK                                    (0x1)
#define UDMA_CTRL_CG_CG_I2C1_SHIFT                                   (0)
#define UDMA_CTRL_CG_CG_I2C1(val)                                    (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_I2C1_SHIFT)) & UDMA_CTRL_CG_CG_I2C1_MASK)

/* Defines uDMA peripherals clock gate configuration for TCDM */
#define UDMA_CTRL_CG_CG_TCDM_MASK                                    (0x1)
#define UDMA_CTRL_CG_CG_TCDM_SHIFT                                   (0)
#define UDMA_CTRL_CG_CG_TCDM(val)                                    (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_TCDM_SHIFT)) & UDMA_CTRL_CG_CG_TCDM_MASK)

/* Defines uDMA peripherals clock gate configuration for I2S */
#define UDMA_CTRL_CG_CG_I2S_MASK                                     (0x1)
#define UDMA_CTRL_CG_CG_I2S_SHIFT                                    (0)
#define UDMA_CTRL_CG_CG_I2S(val)                                     (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_I2S_SHIFT)) & UDMA_CTRL_CG_CG_I2S_MASK)

/* Defines uDMA peripherals clock gate configuration for CAM */
#define UDMA_CTRL_CG_CG_CAM_MASK                                     (0x1)
#define UDMA_CTRL_CG_CG_CAM_SHIFT                                    (0)
#define UDMA_CTRL_CG_CG_CAM(val)                                     (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_CAM_SHIFT)) & UDMA_CTRL_CG_CG_CAM_MASK)


/*! @name EVENT */
/* ?? */
#define UDMA_CTRL_EVENT_CMP_EVT0_MASK                                (0xff)
#define UDMA_CTRL_EVENT_CMP_EVT0_SHIFT                               (0)
#define UDMA_CTRL_EVENT_CMP_EVT0(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_CMP_EVT0_SHIFT)) & UDMA_CTRL_EVENT_CMP_EVT0_MASK)

/* ?? */
#define UDMA_CTRL_EVENT_CMP_EVT1_MASK                                (0xff00)
#define UDMA_CTRL_EVENT_CMP_EVT1_SHIFT                               (8)
#define UDMA_CTRL_EVENT_CMP_EVT1(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_CMP_EVT1_SHIFT)) & UDMA_CTRL_EVENT_CMP_EVT1_MASK)

/* ?? */
#define UDMA_CTRL_EVENT_CMP_EVT2_MASK                                (0xff0000)
#define UDMA_CTRL_EVENT_CMP_EVT2_SHIFT                               (16)
#define UDMA_CTRL_EVENT_CMP_EVT2(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_CMP_EVT2_SHIFT)) & UDMA_CTRL_EVENT_CMP_EVT2_MASK)

/* ?? */
#define UDMA_CTRL_EVENT_CMP_EVT3_MASK                                (0xff000000)
#define UDMA_CTRL_EVENT_CMP_EVT3_SHIFT                               (24)
#define UDMA_CTRL_EVENT_CMP_EVT3(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_CMP_EVT3_SHIFT)) & UDMA_CTRL_EVENT_CMP_EVT3_MASK)


/*! @name CG */
typedef union
{
    struct
    {
        /* Defines uDMA peripherals clock gate configuration for LVDS */
        uint32_t cg_lvds:1;
        /* Defines uDMA peripherals clock gate configuration for SPIM0 */
        uint32_t cg_spim0:1;
        /* Defines uDMA peripherals clock gate configuration for SPIM1 */
        uint32_t cg_spim1:1;
        /* Defines uDMA peripherals clock gate configuration for SPIM2 */
        uint32_t cg_spim2:1;
        /* Defines uDMA peripherals clock gate configuration for UART */
        uint32_t cg_uart:1;
        /* Defines uDMA peripherals clock gate configuration for I2C0 */
        uint32_t cg_i2c0:1;
        /* Defines uDMA peripherals clock gate configuration for I2C1 */
        uint32_t cg_i2c1:1;
        /* Defines uDMA peripherals clock gate configuration for TCDM */
        uint32_t cg_tcdm:1;
        /* Defines uDMA peripherals clock gate configuration for I2S */
        uint32_t cg_i2s:1;
        /* Defines uDMA peripherals clock gate configuration for CAM */
        uint32_t cg_cam:1;
    } field;
    uint32_t word;
} udma_ctrl_cg_t;

/*! @name EVENT */
typedef union
{
    struct
    {
        /* ?? */
        uint32_t cmp_evt0:8;
        /* ?? */
        uint32_t cmp_evt1:8;
        /* ?? */
        uint32_t cmp_evt2:8;
        /* ?? */
        uint32_t cmp_evt3:8;
    } field;
    uint32_t word;
} udma_ctrl_event_t;


/* ----------------------------------------------------------------------------
   -- UDMA_CTRL CMD IDs and macros --
   ---------------------------------------------------------------------------- */




#endif /* __UDMA_CTRL_PERIPH_H__ */
