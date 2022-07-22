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

#ifndef __CLUSTER_ICACHE_CTRL_PERIPH_H__
#define __CLUSTER_ICACHE_CTRL_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- REGMAP Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** REGMAP_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t enable; /**< Cluster instruction cache unit enable configuration register. */
    volatile uint32_t flush; /**< Cluster instruction cache unit flush command register. */
    volatile uint32_t flush_l1; /**< Cluster level 0 instruction cache unit flush command register. */
    volatile uint32_t flush_sel; /**< Cluster instruction cache unit selective flush command register.
Write : flush cache for selected cores.
R : pending transactions. */
    volatile uint32_t cnts_clear;
    volatile uint32_t cnts_enable;
} cluster_icache_ctrl_t;


/* ----------------------------------------------------------------------------
   -- REGMAP Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name ENABLE */
/* Cluster instruction cache enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_MASK                       (0x1)
#define CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_SHIFT                      (0)
#define CLUSTER_ICACHE_CTRL_ENABLE_ENABLE(val)                       (((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_SHIFT)) & CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_MASK)


/*! @name FLUSH */
/* Cluster instruction cache full flush command. */
#define CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_MASK                         (0x1)
#define CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_SHIFT                        (0)
#define CLUSTER_ICACHE_CTRL_FLUSH_FLUSH(val)                         (((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_SHIFT)) & CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_MASK)


/*! @name FLUSH_L1 */
/* Cluster level 0 instruction cache full flush command.  */
#define CLUSTER_ICACHE_CTRL_FLUSH_L1_FLUSH_L1_MASK                   (0x1)
#define CLUSTER_ICACHE_CTRL_FLUSH_L1_FLUSH_L1_SHIFT                  (0)
#define CLUSTER_ICACHE_CTRL_FLUSH_L1_FLUSH_L1(val)                   (((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_FLUSH_L1_FLUSH_L1_SHIFT)) & CLUSTER_ICACHE_CTRL_FLUSH_L1_FLUSH_L1_MASK)


/*! @name FLUSH_SEL */
/* Cluster instruction cache selective flush address configuration bitfield. */
#define CLUSTER_ICACHE_CTRL_FLUSH_SEL_ADDR_MASK                      (0xffffffff)
#define CLUSTER_ICACHE_CTRL_FLUSH_SEL_ADDR_SHIFT                     (0)
#define CLUSTER_ICACHE_CTRL_FLUSH_SEL_ADDR(val)                      (((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_FLUSH_SEL_ADDR_SHIFT)) & CLUSTER_ICACHE_CTRL_FLUSH_SEL_ADDR_MASK)


/*! @name ENABLE */
typedef union
{
    struct
    {
        /* Cluster instruction cache enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t enable:1;
    } field;
    uint32_t word;
} cluster_icache_ctrl_enable_t;

/*! @name FLUSH */
typedef union
{
    struct
    {
        /* Cluster instruction cache full flush command. */
        uint32_t flush:1;
    } field;
    uint32_t word;
} cluster_icache_ctrl_flush_t;

/*! @name FLUSH_SEL */
typedef union
{
    struct
    {
        /* Cluster instruction cache selective flush address configuration bitfield. */
        uint32_t addr:32;
    } field;
    uint32_t word;
} cluster_icache_ctrl_flush_sel_status_t;

/*! @name FLUSH_L1 */
typedef union
{
    struct
    {
        /* Cluster level 0 instruction cache full flush command.  */
        uint32_t flush_l1:1;
    } field;
    uint32_t word;
} cluster_icache_ctrl_flush_l1_t;


#endif /* __CLUSTER_ICACHE_CTRL_PERIPH_H__ */
