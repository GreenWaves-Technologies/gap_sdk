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

#ifndef __FC_ITC_PERIPH_H__
#define __FC_ITC_PERIPH_H__



/* ----------------------------------------------------------------------------
   -- FC_ITC Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** FC_ITC_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t mask; /**< FC_ITC Mask status register. */
    volatile uint32_t mask_set; /**< FC_ITC Mask set register. */
    volatile uint32_t mask_clear; /**< FC_ITC Mask clear register. */
    volatile uint32_t irq; /**< FC_ITC interrupt status register. */
    volatile uint32_t irq_set; /**< FC_ITC interrupt enable register. */
    volatile uint32_t irq_clear; /**< FC_ITC interrupt clear register. */
    volatile uint32_t ack; /**< FC_ITC interrupt ack status register. */
    volatile uint32_t ack_set; /**< FC_ITC interrupt ack set register. */
    volatile uint32_t ack_clear; /**< FC_ITC interrupt ack clear register. */
    volatile uint32_t fifo; /**< FC_ITC event number fifo register. */
} fc_itc_t;


/* ----------------------------------------------------------------------------
   -- FC_ITC Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name MASK */
/* Mask IRQ register:
  - if bit is set, IRQ is enabled.
  - if bit is not set, IRQ is disabled. */
#define FC_ITC_MASK_VALUE_MASK                                       (0xffffffff)
#define FC_ITC_MASK_VALUE_SHIFT                                      (0)
#define FC_ITC_MASK_VALUE(val)                                       (((uint32_t)(((uint32_t)(val)) << FC_ITC_MASK_VALUE_SHIFT)) & FC_ITC_MASK_VALUE_MASK)


/*! @name MASK_SET */
/* Set IRQ mask register: set N bits to enable IRQ. */
#define FC_ITC_MASK_SET_VALUE_MASK                                   (0xffffffff)
#define FC_ITC_MASK_SET_VALUE_SHIFT                                  (0)
#define FC_ITC_MASK_SET_VALUE(val)                                   (((uint32_t)(((uint32_t)(val)) << FC_ITC_MASK_SET_VALUE_SHIFT)) & FC_ITC_MASK_SET_VALUE_MASK)


/*! @name MASK_CLEAR */
/* Clear IRQ mask register: set N bits to disable IRQ. */
#define FC_ITC_MASK_CLEAR_VALUE_MASK                                 (0xffffffff)
#define FC_ITC_MASK_CLEAR_VALUE_SHIFT                                (0)
#define FC_ITC_MASK_CLEAR_VALUE(val)                                 (((uint32_t)(((uint32_t)(val)) << FC_ITC_MASK_CLEAR_VALUE_SHIFT)) & FC_ITC_MASK_CLEAR_VALUE_MASK)


/*! @name IRQ */
/* Active IRQ register:
  - if bit is set, IRQ is active.
  - if bit is not set, no IRQ. */
#define FC_ITC_IRQ_VALUE_MASK                                        (0xffffffff)
#define FC_ITC_IRQ_VALUE_SHIFT                                       (0)
#define FC_ITC_IRQ_VALUE(val)                                        (((uint32_t)(((uint32_t)(val)) << FC_ITC_IRQ_VALUE_SHIFT)) & FC_ITC_IRQ_VALUE_MASK)


/*! @name IRQ_SET */
/* Set active IRQ register: set N bits to active IRQ. */
#define FC_ITC_IRQ_SET_VALUE_MASK                                    (0xffffffff)
#define FC_ITC_IRQ_SET_VALUE_SHIFT                                   (0)
#define FC_ITC_IRQ_SET_VALUE(val)                                    (((uint32_t)(((uint32_t)(val)) << FC_ITC_IRQ_SET_VALUE_SHIFT)) & FC_ITC_IRQ_SET_VALUE_MASK)


/*! @name IRQ_CLEAR */
/* Clear active IRQ register: set N bits to remove IRQ. */
#define FC_ITC_IRQ_CLEAR_VALUE_MASK                                  (0xffffffff)
#define FC_ITC_IRQ_CLEAR_VALUE_SHIFT                                 (0)
#define FC_ITC_IRQ_CLEAR_VALUE(val)                                  (((uint32_t)(((uint32_t)(val)) << FC_ITC_IRQ_CLEAR_VALUE_SHIFT)) & FC_ITC_IRQ_CLEAR_VALUE_MASK)


/*! @name ACK */
/* - */
#define FC_ITC_ACK_VALUE_MASK                                        (0xffffffff)
#define FC_ITC_ACK_VALUE_SHIFT                                       (0)
#define FC_ITC_ACK_VALUE(val)                                        (((uint32_t)(((uint32_t)(val)) << FC_ITC_ACK_VALUE_SHIFT)) & FC_ITC_ACK_VALUE_MASK)


/*! @name ACK_SET */
/* - */
#define FC_ITC_ACK_SET_VALUE_MASK                                    (0xffffffff)
#define FC_ITC_ACK_SET_VALUE_SHIFT                                   (0)
#define FC_ITC_ACK_SET_VALUE(val)                                    (((uint32_t)(((uint32_t)(val)) << FC_ITC_ACK_SET_VALUE_SHIFT)) & FC_ITC_ACK_SET_VALUE_MASK)


/*! @name ACK_CLEAR */
/* - */
#define FC_ITC_ACK_CLEAR_VALUE_MASK                                  (0xffffffff)
#define FC_ITC_ACK_CLEAR_VALUE_SHIFT                                 (0)
#define FC_ITC_ACK_CLEAR_VALUE(val)                                  (((uint32_t)(((uint32_t)(val)) << FC_ITC_ACK_CLEAR_VALUE_SHIFT)) & FC_ITC_ACK_CLEAR_VALUE_MASK)


/*! @name FIFO */
/* Fifo containing last 8 received events.
  A read access to this fifo pops the oldest event from the fifo. */
#define FC_ITC_FIFO_CURRENT_EVENT_MASK                               (0xffffffff)
#define FC_ITC_FIFO_CURRENT_EVENT_SHIFT                              (0)
#define FC_ITC_FIFO_CURRENT_EVENT(val)                               (((uint32_t)(((uint32_t)(val)) << FC_ITC_FIFO_CURRENT_EVENT_SHIFT)) & FC_ITC_FIFO_CURRENT_EVENT_MASK)


/*! @name MASK */
typedef union
{
    struct
    {
        /* Mask IRQ register:
        - if bit is set, IRQ is enabled.
        - if bit is not set, IRQ is disabled. */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_mask_t;

/*! @name MASK_SET */
typedef union
{
    struct
    {
        /* Set IRQ mask register: set N bits to enable IRQ. */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_mask_set_t;

/*! @name MASK_CLEAR */
typedef union
{
    struct
    {
        /* Clear IRQ mask register: set N bits to disable IRQ. */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_mask_clear_t;

/*! @name IRQ */
typedef union
{
    struct
    {
        /* Active IRQ register:
        - if bit is set, IRQ is active.
        - if bit is not set, no IRQ. */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_irq_t;

/*! @name IRQ_SET */
typedef union
{
    struct
    {
        /* Set active IRQ register: set N bits to active IRQ. */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_irq_set_t;

/*! @name IRQ_CLEAR */
typedef union
{
    struct
    {
        /* Clear active IRQ register: set N bits to remove IRQ. */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_irq_clear_t;

/*! @name ACK */
typedef union
{
    struct
    {
        /* - */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_ack_t;

/*! @name ACK_SET */
typedef union
{
    struct
    {
        /* - */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_ack_set_t;

/*! @name ACK_CLEAR */
typedef union
{
    struct
    {
        /* - */
        uint32_t value:32;
    } field;
    uint32_t word;
} fc_itc_ack_clear_t;

/*! @name FIFO */
typedef union
{
    struct
    {
        /* Fifo containing last 8 received events.
        A read access to this fifo pops the oldest event from the fifo. */
        uint32_t current_event:32;
    } field;
    uint32_t word;
} fc_itc_fifo_t;


#endif /* __FC_ITC_PERIPH_H__ */
