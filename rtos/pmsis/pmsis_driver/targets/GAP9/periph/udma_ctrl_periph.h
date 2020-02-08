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
/* uDMA peripherals clock gate configuration. Each bit clock gates one uDMA peripheral(SPIM, UART, I2C,…). */
#define UDMA_CTRL_CG_CG_MASK                                         (0xffffffff)
#define UDMA_CTRL_CG_CG_SHIFT                                        (0)
#define UDMA_CTRL_CG_CG(val)                                         (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_CG_CG_SHIFT)) & UDMA_CTRL_CG_CG_MASK)


/*! @name EVENT */
/* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
#define UDMA_CTRL_EVENT_EVENT_0_MASK                                 (0xff)
#define UDMA_CTRL_EVENT_EVENT_0_SHIFT                                (0)
#define UDMA_CTRL_EVENT_EVENT_0(val)                                 (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_EVENT_0_SHIFT)) & UDMA_CTRL_EVENT_EVENT_0_MASK)

/* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
#define UDMA_CTRL_EVENT_EVENT_1_MASK                                 (0xff00)
#define UDMA_CTRL_EVENT_EVENT_1_SHIFT                                (8)
#define UDMA_CTRL_EVENT_EVENT_1(val)                                 (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_EVENT_1_SHIFT)) & UDMA_CTRL_EVENT_EVENT_1_MASK)

/* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
#define UDMA_CTRL_EVENT_EVENT_2_MASK                                 (0xff0000)
#define UDMA_CTRL_EVENT_EVENT_2_SHIFT                                (16)
#define UDMA_CTRL_EVENT_EVENT_2(val)                                 (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_EVENT_2_SHIFT)) & UDMA_CTRL_EVENT_EVENT_2_MASK)

/* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
#define UDMA_CTRL_EVENT_EVENT_3_MASK                                 (0xff000000)
#define UDMA_CTRL_EVENT_EVENT_3_SHIFT                                (24)
#define UDMA_CTRL_EVENT_EVENT_3(val)                                 (((uint32_t)(((uint32_t)(val)) << UDMA_CTRL_EVENT_EVENT_3_SHIFT)) & UDMA_CTRL_EVENT_EVENT_3_MASK)


/*! @name CG */
typedef union
{
    struct
    {
        /* uDMA peripherals clock gate configuration. Each bit clock gates one uDMA peripheral(SPIM, UART, I2C,…). */
        uint32_t cg:32;
    } field;
    uint32_t word;
} udma_ctrl_cg_t;

/*! @name EVENT */
typedef union
{
    struct
    {
        /* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
        uint32_t event_0:8;
        /* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
        uint32_t event_1:8;
        /* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
        uint32_t event_2:8;
        /* uDMA event propagation configuration. Set which SoC event should be propagated to uDMA. */
        uint32_t event_3:8;
    } field;
    uint32_t word;
} udma_ctrl_event_t;


#endif /* __UDMA_CTRL_PERIPH_H__ */
