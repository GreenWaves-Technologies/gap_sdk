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

#ifndef __DMACPY_PERIPH_H__
#define __DMACPY_PERIPH_H__

#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- DMACPY Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** DMACPY_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t dst_addr; /**< Destination address configuration */
    volatile uint32_t src_addr; /**< Source address configuration */
    volatile uint32_t mem_sel; /**< Memcpy direction configuration */
} dmacpy_t;


/* ----------------------------------------------------------------------------
   -- DMACPY Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name DST_ADDR */
/* TX destination start address configuration bitfield.
  MEMCPY TX transfer copy data from L2 TX_CFG.TX_SADDR address to FC_TCDM or L2 memory DST_ADDR address. */
#define DMACPY_DST_ADDR_DST_ADDR_MASK                                (0x1ffff)
#define DMACPY_DST_ADDR_DST_ADDR_SHIFT                               (0)
#define DMACPY_DST_ADDR_DST_ADDR(val)                                (((uint32_t)(((uint32_t)(val)) << DMACPY_DST_ADDR_DST_ADDR_SHIFT)) & DMACPY_DST_ADDR_DST_ADDR_MASK)


/*! @name SRC_ADDR */
/* RX source start address configuration bitfeld.
  MEMCPY RX transfer copy data from FC_TCDM or L2 memory SRC_ADDR address to L2 RX_CFG.RX_SADDR address. */
#define DMACPY_SRC_ADDR_SRC_ADDR_MASK                                (0x1ffff)
#define DMACPY_SRC_ADDR_SRC_ADDR_SHIFT                               (0)
#define DMACPY_SRC_ADDR_SRC_ADDR(val)                                (((uint32_t)(((uint32_t)(val)) << DMACPY_SRC_ADDR_SRC_ADDR_SHIFT)) & DMACPY_SRC_ADDR_SRC_ADDR_MASK)


/*! @name MEM_SEL */
/* Memcpy direction */
#define DMACPY_MEM_SEL_TYPE_MASK                                     (0x1)
#define DMACPY_MEM_SEL_TYPE_SHIFT                                    (0)
#define DMACPY_MEM_SEL_TYPE(val)                                     (((uint32_t)(((uint32_t)(val)) << DMACPY_MEM_SEL_TYPE_SHIFT)) & DMACPY_MEM_SEL_TYPE_MASK)


/*! @name DST_ADDR */
typedef union
{
    struct
    {
        /* TX destination start address configuration bitfield.
        MEMCPY TX transfer copy data from L2 TX_CFG.TX_SADDR address to FC_TCDM or L2 memory DST_ADDR address. */
        uint32_t dst_addr:17;
    } field;
    uint32_t word;
} dmacpy_dst_addr_t;

/*! @name SRC_ADDR */
typedef union
{
    struct
    {
        /* RX source start address configuration bitfeld.
        MEMCPY RX transfer copy data from FC_TCDM or L2 memory SRC_ADDR address to L2 RX_CFG.RX_SADDR address. */
        uint32_t src_addr:17;
    } field;
    uint32_t word;
} dmacpy_src_addr_t;

/*! @name MEM_SEL */
typedef union
{
    struct
    {
        /* Memcpy direction */
        uint32_t type:1;
    } field;
    uint32_t word;
} dmacpy_mem_sel_t;


/* ----------------------------------------------------------------------------
   -- DMACPY CMD IDs and macros --
   ---------------------------------------------------------------------------- */




#endif /* __DMACPY_PERIPH_H__ */
