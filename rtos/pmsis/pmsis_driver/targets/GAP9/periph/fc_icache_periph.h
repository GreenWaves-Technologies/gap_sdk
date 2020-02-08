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

#ifndef __FC_ICACHE_PERIPH_H__
#define __FC_ICACHE_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- FC_ICACHE Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** FC_ICACHE_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t enable; /**< FC instruction cache unit enable configuration register. */
    volatile uint32_t flush; /**< FC instruction cache unit flush command register. */
    volatile uint32_t sel_flush; /**< FC instruction cache unit selective flush command register. */
    volatile uint32_t status; /**< FC instruction cache unit status register. */
} fc_icache_t;


/* ----------------------------------------------------------------------------
   -- FC_ICACHE Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name ENABLE */
/* FC instruction cache enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define FC_ICACHE_ENABLE_EN_MASK                                     (0x1)
#define FC_ICACHE_ENABLE_EN_SHIFT                                    (0)
#define FC_ICACHE_ENABLE_EN(val)                                     (((uint32_t)(((uint32_t)(val)) << FC_ICACHE_ENABLE_EN_SHIFT)) & FC_ICACHE_ENABLE_EN_MASK)


/*! @name FLUSH */
/* FC instruction cache fully flush command. */
#define FC_ICACHE_FLUSH_FL_MASK                                      (0x1)
#define FC_ICACHE_FLUSH_FL_SHIFT                                     (0)
#define FC_ICACHE_FLUSH_FL(val)                                      (((uint32_t)(((uint32_t)(val)) << FC_ICACHE_FLUSH_FL_SHIFT)) & FC_ICACHE_FLUSH_FL_MASK)


/*! @name SEL_FLUSH */
/* FC instruction cache selective flush address configuration bitfield. */
#define FC_ICACHE_SEL_FLUSH_ADDR_MASK                                (0xffffffff)
#define FC_ICACHE_SEL_FLUSH_ADDR_SHIFT                               (0)
#define FC_ICACHE_SEL_FLUSH_ADDR(val)                                (((uint32_t)(((uint32_t)(val)) << FC_ICACHE_SEL_FLUSH_ADDR_SHIFT)) & FC_ICACHE_SEL_FLUSH_ADDR_MASK)


/*! @name STATUS */
/* FC instruction cache pending action status flag:
  - 1'b0: no pending caching action
  - 1'b1: pending caching action */
#define FC_ICACHE_STATUS_STATUS_MASK                                 (0x1)
#define FC_ICACHE_STATUS_STATUS_SHIFT                                (0)
#define FC_ICACHE_STATUS_STATUS(val)                                 (((uint32_t)(((uint32_t)(val)) << FC_ICACHE_STATUS_STATUS_SHIFT)) & FC_ICACHE_STATUS_STATUS_MASK)


/*! @name ENABLE */
typedef union
{
    struct
    {
        /* FC instruction cache enable configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t en:1;
    } field;
    uint32_t word;
} fc_icache_enable_t;

/*! @name FLUSH */
typedef union
{
    struct
    {
        /* FC instruction cache fully flush command. */
        uint32_t fl:1;
    } field;
    uint32_t word;
} fc_icache_flush_t;

/*! @name SEL_FLUSH */
typedef union
{
    struct
    {
        /* FC instruction cache selective flush address configuration bitfield. */
        uint32_t addr:32;
    } field;
    uint32_t word;
} fc_icache_sel_flush_t;

/*! @name STATUS */
typedef union
{
    struct
    {
        /* FC instruction cache pending action status flag:
        - 1'b0: no pending caching action
        - 1'b1: pending caching action */
        uint32_t status:1;
    } field;
    uint32_t word;
} fc_icache_status_t;



#endif /* __FC_ICACHE_PERIPH_H__ */
