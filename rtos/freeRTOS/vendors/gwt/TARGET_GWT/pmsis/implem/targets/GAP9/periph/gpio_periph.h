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

#ifndef __GPIO_PERIPH_H__
#define __GPIO_PERIPH_H__



/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** GPIO_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t paddir; /**< GPIO pad direction configuration register. */
    volatile uint32_t gpioen; /**< GPIO enable register. */
    volatile uint32_t padin; /**< GPIO pad input value register. */
    volatile uint32_t padout; /**< GPIO pad output value register. */
    volatile uint32_t padoutset; /**< GPIO pad output set register. */
    volatile uint32_t padoutclr; /**< GPIO pad output clear register. */
    volatile uint32_t inten; /**< GPIO pad interrupt enable configuration register. */
    volatile uint32_t inttype[2]; /**< GPIO pad interrupt type configuration register. */
    volatile uint32_t intstatus; /**< GPIO pad interrupt status register. */
    volatile uint32_t padcfg[4]; /**< GPIO pad pin configuration register. */
} gpio_t;


/* ----------------------------------------------------------------------------
   -- GPIO Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name PADDIR */
/* GPIO direction configuration bitfield:
  - bit[i]=1'b0: Input mode for GPIO[i]
  - bit[i]=1'b1: Output mode for GPIO[i] */
#define GPIO_PADDIR_DIR_MASK                                         (0xffffffff)
#define GPIO_PADDIR_DIR_SHIFT                                        (0)
#define GPIO_PADDIR_DIR(val)                                         (((uint32_t)(((uint32_t)(val)) << GPIO_PADDIR_DIR_SHIFT)) & GPIO_PADDIR_DIR_MASK)


/*! @name GPIOEN */
/* GPIO clock enable configuration bitfield:
  - bit[i]=1'b0: disable clock for GPIO[i]
  - bit[i]=1'b1: enable clock for GPIO[i]
  GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled. 
  Clock must be enabled for a GPIO if it's direction is configured in input mode. */
#define GPIO_GPIOEN_GPIOEN_MASK                                      (0xffffffff)
#define GPIO_GPIOEN_GPIOEN_SHIFT                                     (0)
#define GPIO_GPIOEN_GPIOEN(val)                                      (((uint32_t)(((uint32_t)(val)) << GPIO_GPIOEN_GPIOEN_SHIFT)) & GPIO_GPIOEN_GPIOEN_MASK)


/*! @name PADIN */
/* GPIO input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. */
#define GPIO_PADIN_DATA_IN_MASK                                      (0xffffffff)
#define GPIO_PADIN_DATA_IN_SHIFT                                     (0)
#define GPIO_PADIN_DATA_IN(val)                                      (((uint32_t)(((uint32_t)(val)) << GPIO_PADIN_DATA_IN_SHIFT)) & GPIO_PADIN_DATA_IN_MASK)


/*! @name PADOUT */
/* GPIO output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. */
#define GPIO_PADOUT_DATA_OUT_MASK                                    (0xffffffff)
#define GPIO_PADOUT_DATA_OUT_SHIFT                                   (0)
#define GPIO_PADOUT_DATA_OUT(val)                                    (((uint32_t)(((uint32_t)(val)) << GPIO_PADOUT_DATA_OUT_SHIFT)) & GPIO_PADOUT_DATA_OUT_MASK)


/*! @name PADOUTSET */
/* GPIO set bitfield:
  - bit[i]=1'b0: No change for GPIO[i]
  - bit[i]=1'b1: Sets GPIO[i] to 1 */
#define GPIO_PADOUTSET_DATA_SET_MASK                                 (0xffffffff)
#define GPIO_PADOUTSET_DATA_SET_SHIFT                                (0)
#define GPIO_PADOUTSET_DATA_SET(val)                                 (((uint32_t)(((uint32_t)(val)) << GPIO_PADOUTSET_DATA_SET_SHIFT)) & GPIO_PADOUTSET_DATA_SET_MASK)


/*! @name PADOUTCLR */
/* GPIO clear bitfield:
  - bit[i]=1'b0: No change for GPIO[i]
  - bit[i]=1'b1: Clears GPIO[i] */
#define GPIO_PADOUTCLR_DATA_CLEAR_MASK                               (0xffffffff)
#define GPIO_PADOUTCLR_DATA_CLEAR_SHIFT                              (0)
#define GPIO_PADOUTCLR_DATA_CLEAR(val)                               (((uint32_t)(((uint32_t)(val)) << GPIO_PADOUTCLR_DATA_CLEAR_SHIFT)) & GPIO_PADOUTCLR_DATA_CLEAR_MASK)


/*! @name INTEN */
/* GPIO interrupt enable configuration bitfield:
  - bit[i]=1'b0: disable interrupt for GPIO[i]
  - bit[i]=1'b1: enable interrupt for GPIO[i] */
#define GPIO_INTEN_INTEN_MASK                                        (0xffffffff)
#define GPIO_INTEN_INTEN_SHIFT                                       (0)
#define GPIO_INTEN_INTEN(val)                                        (((uint32_t)(((uint32_t)(val)) << GPIO_INTEN_INTEN_SHIFT)) & GPIO_INTEN_INTEN_MASK)


/*! @name INTTYPE */
/* GPIO interrupt type configuration bitfield:
  - bit[2*i+1:2*i]=2'b00: interrupt on falling edge for GPIO[i]
  - bit[2*i+1:2*i]=2'b01: interrupt on rising edge for GPIO[i]
  - bit[2*i+1:2*i]=2'b10: interrupt on rising and falling edge for GPIO[i]
  - bit[2*i+1:2*i]=2'b11: RFU */
#define GPIO_INTTYPE_INTTYPE_MASK                                    (0xffffffff)
#define GPIO_INTTYPE_INTTYPE_SHIFT                                   (0)
#define GPIO_INTTYPE_INTTYPE(val)                                    (((uint32_t)(((uint32_t)(val)) << GPIO_INTTYPE_INTTYPE_SHIFT)) & GPIO_INTTYPE_INTTYPE_MASK)


/*! @name INTSTATUS */
/* GPIO Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. */
#define GPIO_INTSTATUS_INTSTATUS_MASK                                (0xffffffff)
#define GPIO_INTSTATUS_INTSTATUS_SHIFT                               (0)
#define GPIO_INTSTATUS_INTSTATUS(val)                                (((uint32_t)(((uint32_t)(val)) << GPIO_INTSTATUS_INTSTATUS_SHIFT)) & GPIO_INTSTATUS_INTSTATUS_MASK)


/*! @name PADCFG */
/* GPIO[0] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO0_CFG_MASK                                   (0xf)
#define GPIO_PADCFG_GPIO0_CFG_SHIFT                                  (0)
#define GPIO_PADCFG_GPIO0_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO0_CFG_SHIFT)) & GPIO_PADCFG_GPIO0_CFG_MASK)

/* GPIO[1] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO1_CFG_MASK                                   (0xf0)
#define GPIO_PADCFG_GPIO1_CFG_SHIFT                                  (4)
#define GPIO_PADCFG_GPIO1_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO1_CFG_SHIFT)) & GPIO_PADCFG_GPIO1_CFG_MASK)

/* GPIO[2] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO2_CFG_MASK                                   (0xf00)
#define GPIO_PADCFG_GPIO2_CFG_SHIFT                                  (8)
#define GPIO_PADCFG_GPIO2_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO2_CFG_SHIFT)) & GPIO_PADCFG_GPIO2_CFG_MASK)

/* GPIO[3] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO3_CFG_MASK                                   (0xf000)
#define GPIO_PADCFG_GPIO3_CFG_SHIFT                                  (12)
#define GPIO_PADCFG_GPIO3_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO3_CFG_SHIFT)) & GPIO_PADCFG_GPIO3_CFG_MASK)

/* GPIO[4] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO4_CFG_MASK                                   (0xf0000)
#define GPIO_PADCFG_GPIO4_CFG_SHIFT                                  (16)
#define GPIO_PADCFG_GPIO4_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO4_CFG_SHIFT)) & GPIO_PADCFG_GPIO4_CFG_MASK)

/* GPIO[5] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO5_CFG_MASK                                   (0xf00000)
#define GPIO_PADCFG_GPIO5_CFG_SHIFT                                  (20)
#define GPIO_PADCFG_GPIO5_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO5_CFG_SHIFT)) & GPIO_PADCFG_GPIO5_CFG_MASK)

/* GPIO[6] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO6_CFG_MASK                                   (0xf000000)
#define GPIO_PADCFG_GPIO6_CFG_SHIFT                                  (24)
#define GPIO_PADCFG_GPIO6_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO6_CFG_SHIFT)) & GPIO_PADCFG_GPIO6_CFG_MASK)

/* GPIO[7] pull activation and driver strength configuration bitfield:
  - 2'b00: drive strength low - pull disabled
  - 2'b01: drive strength low - pull enabled
  - 2’b10: drive strength high – pull disabled
  - 2’b11: drive strength high – pull enabled */
#define GPIO_PADCFG_GPIO7_CFG_MASK                                   (0xf0000000)
#define GPIO_PADCFG_GPIO7_CFG_SHIFT                                  (28)
#define GPIO_PADCFG_GPIO7_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << GPIO_PADCFG_GPIO7_CFG_SHIFT)) & GPIO_PADCFG_GPIO7_CFG_MASK)


/*! @name PADDIR */
typedef union
{
    struct
    {
        /* GPIO direction configuration bitfield:
        - bit[i]=1'b0: Input mode for GPIO[i]
        - bit[i]=1'b1: Output mode for GPIO[i] */
        uint32_t dir:32;
    } field;
    uint32_t word;
} gpio_paddir_t;

/*! @name GPIOEN */
typedef union
{
    struct
    {
        /* GPIO clock enable configuration bitfield:
        - bit[i]=1'b0: disable clock for GPIO[i]
        - bit[i]=1'b1: enable clock for GPIO[i]
        GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled. 
        Clock must be enabled for a GPIO if it's direction is configured in input mode. */
        uint32_t gpioen:32;
    } field;
    uint32_t word;
} gpio_gpioen_t;

/*! @name PADIN */
typedef union
{
    struct
    {
        /* GPIO input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. */
        uint32_t data_in:32;
    } field;
    uint32_t word;
} gpio_padin_t;

/*! @name PADOUT */
typedef union
{
    struct
    {
        /* GPIO output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. */
        uint32_t data_out:32;
    } field;
    uint32_t word;
} gpio_padout_t;

/*! @name PADOUTSET */
typedef union
{
    struct
    {
        /* GPIO set bitfield:
        - bit[i]=1'b0: No change for GPIO[i]
        - bit[i]=1'b1: Sets GPIO[i] to 1 */
        uint32_t data_set:32;
    } field;
    uint32_t word;
} gpio_padoutset_t;

/*! @name PADOUTCLR */
typedef union
{
    struct
    {
        /* GPIO clear bitfield:
        - bit[i]=1'b0: No change for GPIO[i]
        - bit[i]=1'b1: Clears GPIO[i] */
        uint32_t data_clear:32;
    } field;
    uint32_t word;
} gpio_padoutclr_t;

/*! @name INTEN */
typedef union
{
    struct
    {
        /* GPIO interrupt enable configuration bitfield:
        - bit[i]=1'b0: disable interrupt for GPIO[i]
        - bit[i]=1'b1: enable interrupt for GPIO[i] */
        uint32_t inten:32;
    } field;
    uint32_t word;
} gpio_inten_t;

/*! @name INTTYPE */
typedef union
{
    struct
    {
        /* GPIO interrupt type configuration bitfield:
        - bit[2*i+1:2*i]=2'b00: interrupt on falling edge for GPIO[i]
        - bit[2*i+1:2*i]=2'b01: interrupt on rising edge for GPIO[i]
        - bit[2*i+1:2*i]=2'b10: interrupt on rising and falling edge for GPIO[i]
        - bit[2*i+1:2*i]=2'b11: RFU */
        uint32_t inttype:32;
    } field;
    uint32_t word;
} gpio_inttype_t;

/*! @name INTSTATUS */
typedef union
{
    struct
    {
        /* GPIO Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. */
        uint32_t intstatus:32;
    } field;
    uint32_t word;
} gpio_intstatus_t;

/*! @name PADCFG */
typedef union
{
    struct
    {
        /* GPIO[0] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio0_cfg:4;
        /* GPIO[1] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio1_cfg:4;
        /* GPIO[2] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio2_cfg:4;
        /* GPIO[3] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio3_cfg:4;
        /* GPIO[4] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio4_cfg:4;
        /* GPIO[5] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio5_cfg:4;
        /* GPIO[6] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio6_cfg:4;
        /* GPIO[7] pull activation and driver strength configuration bitfield:
        - 2'b00: drive strength low - pull disabled
        - 2'b01: drive strength low - pull enabled
        - 2’b10: drive strength high – pull disabled
        - 2’b11: drive strength high – pull enabled */
        uint32_t gpio7_cfg:4;
    } field;
    uint32_t word;
} gpio_padcfg_t;


/* ----------------------------------------------------------------------------
   -- GPIO CMD IDs and macros --
   ---------------------------------------------------------------------------- */




#endif /* __GPIO_PERIPH_H__ */
