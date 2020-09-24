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

#ifndef __RTC_PERIPH_H__
#define __RTC_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- RTC Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** RTC_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t apb_sr; /**< RTC APB status register. */
    volatile uint32_t apb_cr; /**< RTC APB control register. */
    volatile uint32_t apb_dr; /**< RTC APB data register. */
    volatile uint32_t reserved; /**< Reserved/Not used. */
    volatile uint32_t apb_icr; /**< RTC APB interruption control register. */
    volatile uint32_t apb_imr; /**< RTC APB interruption mask register. */
    volatile uint32_t apb_ifr; /**< RTC APB interruption flag register. */
} rtc_t;


/* ----------------------------------------------------------------------------
   -- RTC Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name APB_SR */
/* APB interrupt status bitfield :
  - 1'b0: No interruption has been requested
  - 1'b1: Interruption requested */
#define RTC_APB_SR_APB_INT_MASK                                      (0x3)
#define RTC_APB_SR_APB_INT_SHIFT                                     (0)
#define RTC_APB_SR_APB_INT(val)                                      (((uint32_t)(((uint32_t)(val)) << RTC_APB_SR_APB_INT_SHIFT)) & RTC_APB_SR_APB_INT_MASK)


/*! @name APB_CR */
/* Indirect access address configuration. */
#define RTC_APB_CR_APB_ADDR_MASK                                     (0x3f)
#define RTC_APB_CR_APB_ADDR_SHIFT                                    (0)
#define RTC_APB_CR_APB_ADDR(val)                                     (((uint32_t)(((uint32_t)(val)) << RTC_APB_CR_APB_ADDR_SHIFT)) & RTC_APB_CR_APB_ADDR_MASK)

/* Indirect access operation configuration bitfield :
  - 1'b0: APB read operation
  - 1'b1: APB write operation */
#define RTC_APB_CR_APB_OP_MASK                                       (0x10000)
#define RTC_APB_CR_APB_OP_SHIFT                                      (16)
#define RTC_APB_CR_APB_OP(val)                                       (((uint32_t)(((uint32_t)(val)) << RTC_APB_CR_APB_OP_SHIFT)) & RTC_APB_CR_APB_OP_MASK)


/*! @name APB_DR */
/* Indirect access data register. */
#define RTC_APB_DR_APB_DATA_MASK                                     (0xffffffff)
#define RTC_APB_DR_APB_DATA_SHIFT                                    (0)
#define RTC_APB_DR_APB_DATA(val)                                     (((uint32_t)(((uint32_t)(val)) << RTC_APB_DR_APB_DATA_SHIFT)) & RTC_APB_DR_APB_DATA_MASK)


/*! @name RESERVED */

/*! @name APB_ICR */
/* APB interrupt signal mode configuration bitfield :
  - 2'b00: APB interrupt is a high level signal
  - 2'b01: APB interrupt is a low level signal
  - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration
  - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration */
#define RTC_APB_ICR_MODE_MASK                                        (0x3)
#define RTC_APB_ICR_MODE_SHIFT                                       (0)
#define RTC_APB_ICR_MODE(val)                                        (((uint32_t)(((uint32_t)(val)) << RTC_APB_ICR_MODE_SHIFT)) & RTC_APB_ICR_MODE_MASK)


/*! @name APB_IMR */
/* APB read operation interruption mask bitfield :
  - 1'b0: enabled
  - 1'b1: disabled */
#define RTC_APB_IMR_READ_MASK_MASK                                   (0x1)
#define RTC_APB_IMR_READ_MASK_SHIFT                                  (0)
#define RTC_APB_IMR_READ_MASK(val)                                   (((uint32_t)(((uint32_t)(val)) << RTC_APB_IMR_READ_MASK_SHIFT)) & RTC_APB_IMR_READ_MASK_MASK)

/* APB write operation interruption mask bitfield :
  - 1'b0: enabled
  - 1'b1: disabled */
#define RTC_APB_IMR_WRITE_MASK_MASK                                  (0x2)
#define RTC_APB_IMR_WRITE_MASK_SHIFT                                 (1)
#define RTC_APB_IMR_WRITE_MASK(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_APB_IMR_WRITE_MASK_SHIFT)) & RTC_APB_IMR_WRITE_MASK_MASK)


/*! @name APB_IFR */
/* APB read operation status flag bitfield :
  - 1'b0: nothing
  - 1'b1: read operation done and requested indirect data is available */
#define RTC_APB_IFR_READ_FLAG_MASK                                   (0x1)
#define RTC_APB_IFR_READ_FLAG_SHIFT                                  (0)
#define RTC_APB_IFR_READ_FLAG(val)                                   (((uint32_t)(((uint32_t)(val)) << RTC_APB_IFR_READ_FLAG_SHIFT)) & RTC_APB_IFR_READ_FLAG_MASK)

/* APB write operation status flag bitfield :
  - 1'b0: nothing
  - 1'b1: write operation done */
#define RTC_APB_IFR_WRITE_FLAG_MASK                                  (0x2)
#define RTC_APB_IFR_WRITE_FLAG_SHIFT                                 (1)
#define RTC_APB_IFR_WRITE_FLAG(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_APB_IFR_WRITE_FLAG_SHIFT)) & RTC_APB_IFR_WRITE_FLAG_MASK)


/*! @name APB_SR */
typedef union
{
    struct
    {
        /* APB interrupt status bitfield :
        - 1'b0: No interruption has been requested
        - 1'b1: Interruption requested */
        uint32_t apb_int:2;
    } field;
    uint32_t word;
} rtc_apb_sr_t;

/*! @name APB_CR */
typedef union
{
    struct
    {
        /* Indirect access address configuration. */
        uint32_t apb_addr:6;
        uint32_t reserved_0:10;
        /* Indirect access operation configuration bitfield :
        - 1'b0: APB read operation
        - 1'b1: APB write operation */
        uint32_t apb_op:1;
    } field;
    uint32_t word;
} rtc_apb_cr_t;

/*! @name APB_DR */
typedef union
{
    struct
    {
        /* Indirect access data register. */
        uint32_t apb_data:32;
    } field;
    uint32_t word;
} rtc_apb_dr_t;

/*! @name RESERVED */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} rtc_reserved_t;

/*! @name APB_ICR */
typedef union
{
    struct
    {
        /* APB interrupt signal mode configuration bitfield :
        - 2'b00: APB interrupt is a high level signal
        - 2'b01: APB interrupt is a low level signal
        - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration
        - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration */
        uint32_t mode:2;
    } field;
    uint32_t word;
} rtc_apb_icr_t;

/*! @name APB_IMR */
typedef union
{
    struct
    {
        /* APB read operation interruption mask bitfield :
        - 1'b0: enabled
        - 1'b1: disabled */
        uint32_t read_mask:1;
        /* APB write operation interruption mask bitfield :
        - 1'b0: enabled
        - 1'b1: disabled */
        uint32_t write_mask:1;
    } field;
    uint32_t word;
} rtc_apb_imr_t;

/*! @name APB_IFR */
typedef union
{
    struct
    {
        /* APB read operation status flag bitfield :
        - 1'b0: nothing
        - 1'b1: read operation done and requested indirect data is available */
        uint32_t read_flag:1;
        /* APB write operation status flag bitfield :
        - 1'b0: nothing
        - 1'b1: write operation done */
        uint32_t write_flag:1;
    } field;
    uint32_t word;
} rtc_apb_ifr_t;


/* ----------------------------------------------------------------------------
   -- RTC Peripheral Internal Access Layer --
   ---------------------------------------------------------------------------- */

/** RTC Internal Register Offset */
#define RTC_SR_OFFSET                  (0x0) /**< RTC status register. */
#define RTC_CR_OFFSET                  (0x1) /**< RTC control register. */
#define RTC_CCR_OFFSET                 (0x2) /**< RTC clock control register. */
#define RTC_ICR_OFFSET                 (0x8) /**< RTC interrupt control register. */
#define RTC_IMR_OFFSET                 (0x9) /**< RTC interrupt mask register. */
#define RTC_IFR_OFFSET                 (0xa) /**< RTC interrupt flag register. */
#define CALENDAR_CONTROL_OFFSET        (0x10) /**< RTC calendar control register. */
#define CALENDAR_TIME_OFFSET           (0x12) /**< RTC calendar time register. */
#define CALENDAR_DATE_OFFSET           (0x13) /**< RTC calendar date register. */
#define ALARM_CONTROL_OFFSET           (0x18) /**< RTC alarm control register. */
#define ALARM1_TIME_OFFSET             (0x1a) /**< RTC alarm 1 time register. */
#define ALARM1_DATE_OFFSET             (0x1b) /**< RTC alarm 1 date register. */
#define COUNTDOWN_CONTROL_OFFSET       (0x20) /**< RTC countdown control register. */
#define COUNTDOWN1_INIT_OFFSET         (0x21) /**< RTC countdown 1 initialisation register. */
#define COUNTDOWN1_TIMER_OFFSET        (0x22) /**< RTC countdown 1 timer register. */
#define CKIN_DIV1_OFFSET               (0x28) /**< RTC CKIN divider 1 register. */
#define CKREF_CONF_OFFSET              (0x2a) /**< RTC reference clock configuration register. */
#define RTC_TEST_REG_A_OFFSET          (0x30) /**< RTC test register. */

/* ----------------------------------------------------------------------------
   -- RTC Internal Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name RTC_SR */
/* RTC interrupt status bitfield :
  - 1'b0: No interruption has been requested
  - 1'b1: Interruption requested */
#define RTC_RTC_SR_IRQ_MASK                                          (0x1)
#define RTC_RTC_SR_IRQ_SHIFT                                         (0)
#define RTC_RTC_SR_IRQ(val)                                          (((uint32_t)(((uint32_t)(val)) << RTC_RTC_SR_IRQ_SHIFT)) & RTC_RTC_SR_IRQ_MASK)


/*! @name RTC_CR */
/* RTC standby configuration bitfield :
  - 1'b0: RTC is in active mode
  - 1'b1: RTC is in standby mode */
#define RTC_RTC_CR_SB_MASK                                           (0x1)
#define RTC_RTC_CR_SB_SHIFT                                          (0)
#define RTC_RTC_CR_SB(val)                                           (((uint32_t)(((uint32_t)(val)) << RTC_RTC_CR_SB_SHIFT)) & RTC_RTC_CR_SB_MASK)

/* Calibration process activation bitfield :
  - 1'b0: disabled
  - 1'b1: enabled
  This bitfield is automatically updated to 0 after calibration operation. */
#define RTC_RTC_CR_CAL_EN_MASK                                       (0x10)
#define RTC_RTC_CR_CAL_EN_SHIFT                                      (4)
#define RTC_RTC_CR_CAL_EN(val)                                       (((uint32_t)(((uint32_t)(val)) << RTC_RTC_CR_CAL_EN_SHIFT)) & RTC_RTC_CR_CAL_EN_MASK)

/* Soft reset command bitfield :
  - 1'b0: No reset
  - 1'b1: Reset the calendar, alarm, countdown and clock generation features and associated registers (CALENDAR_*, ALARM*, COUNTDOWN*, CKIN_DIV* and CKREF_CONF)
  
  This bitfield is automatically updated to 1'b1 after event in signle mode. */
#define RTC_RTC_CR_SOFT_RST_MASK                                     (0x100)
#define RTC_RTC_CR_SOFT_RST_SHIFT                                    (8)
#define RTC_RTC_CR_SOFT_RST(val)                                     (((uint32_t)(((uint32_t)(val)) << RTC_RTC_CR_SOFT_RST_SHIFT)) & RTC_RTC_CR_SOFT_RST_MASK)


/*! @name RTC_CCR */
/* CLOUT clock generation standby configuration bitfield :
  - 1'b0: generated
  - 1'b1: not generated */
#define RTC_RTC_CCR_CKOUT_SB_MASK                                    (0x1)
#define RTC_RTC_CCR_CKOUT_SB_SHIFT                                   (0)
#define RTC_RTC_CCR_CKOUT_SB(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_RTC_CCR_CKOUT_SB_SHIFT)) & RTC_RTC_CCR_CKOUT_SB_MASK)

/* CKOUT autocalibrated clock generation configuration bitfield :
  - 1'b0: not auto calibrated
  - 1'b1: auto calibrated */
#define RTC_RTC_CCR_DIV1_AUTOCAL_MASK                                (0x1000)
#define RTC_RTC_CCR_DIV1_AUTOCAL_SHIFT                               (12)
#define RTC_RTC_CCR_DIV1_AUTOCAL(val)                                (((uint32_t)(((uint32_t)(val)) << RTC_RTC_CCR_DIV1_AUTOCAL_SHIFT)) & RTC_RTC_CCR_DIV1_AUTOCAL_MASK)

/* Compensation value according to CKREF configuration bitfield. */
#define RTC_RTC_CCR_DIV1_COMP_MASK                                   (0x1f0000)
#define RTC_RTC_CCR_DIV1_COMP_SHIFT                                  (16)
#define RTC_RTC_CCR_DIV1_COMP(val)                                   (((uint32_t)(((uint32_t)(val)) << RTC_RTC_CCR_DIV1_COMP_SHIFT)) & RTC_RTC_CCR_DIV1_COMP_MASK)


/*! @name RTC_ICR */
/* RTC interrupt signal mode configuration bitfield :
  - 2'b00: RTC interrupt is a high level signal
  - 2'b01: RTC interrupt is a low level signal
  - 2'b10: RTC interrupt is a high level pulse of 1 PCLK duration
  - 2'b11: RTC interrupt is a low level pulse of 1 PCLK duration */
#define RTC_RTC_ICR_MODE_MASK                                        (0x3)
#define RTC_RTC_ICR_MODE_SHIFT                                       (0)
#define RTC_RTC_ICR_MODE(val)                                        (((uint32_t)(((uint32_t)(val)) << RTC_RTC_ICR_MODE_SHIFT)) & RTC_RTC_ICR_MODE_MASK)


/*! @name RTC_IMR */
/* Alarm 1 interrupt mask configuration bitfield :
  - 1'b0: enabled
  - 1'b1: disabled */
#define RTC_RTC_IMR_ALARM_MASK_MASK                                  (0x1)
#define RTC_RTC_IMR_ALARM_MASK_SHIFT                                 (0)
#define RTC_RTC_IMR_ALARM_MASK(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_RTC_IMR_ALARM_MASK_SHIFT)) & RTC_RTC_IMR_ALARM_MASK_MASK)

/* Timer 1 interrupt mask configuration bitfield :
  - 1'b0: enabled
  - 1'b1: disabled */
#define RTC_RTC_IMR_TIMER_MASK_MASK                                  (0x10)
#define RTC_RTC_IMR_TIMER_MASK_SHIFT                                 (4)
#define RTC_RTC_IMR_TIMER_MASK(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_RTC_IMR_TIMER_MASK_SHIFT)) & RTC_RTC_IMR_TIMER_MASK_MASK)

/* Calibration interrupt mask configuration bitfield :
  - 1'b0: enabled
  - 1'b1: disabled */
#define RTC_RTC_IMR_CAL_MASK_MASK                                    (0x1000)
#define RTC_RTC_IMR_CAL_MASK_SHIFT                                   (12)
#define RTC_RTC_IMR_CAL_MASK(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_RTC_IMR_CAL_MASK_SHIFT)) & RTC_RTC_IMR_CAL_MASK_MASK)


/*! @name RTC_IFR */
/* Alarm 1 interrupt status flag bitfield :
  - 1'b0: nothing
  - 1'b1: calendar reaches the date and time set in alarm 1 */
#define RTC_RTC_IFR_ALARM_FLAG_MASK                                  (0x1)
#define RTC_RTC_IFR_ALARM_FLAG_SHIFT                                 (0)
#define RTC_RTC_IFR_ALARM_FLAG(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_RTC_IFR_ALARM_FLAG_SHIFT)) & RTC_RTC_IFR_ALARM_FLAG_MASK)

/* Timer 1 interrupt status flag bitfield :
  - 1'b0: nothing
  - 1'b1: countdown timer 1 reaches value 0 */
#define RTC_RTC_IFR_TIMER_FLAG_MASK                                  (0x10)
#define RTC_RTC_IFR_TIMER_FLAG_SHIFT                                 (4)
#define RTC_RTC_IFR_TIMER_FLAG(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_RTC_IFR_TIMER_FLAG_SHIFT)) & RTC_RTC_IFR_TIMER_FLAG_MASK)

/* Calibration interrupt status flag bitfield :
  - 1'b0: nothing
  - 1'b1: calibration process ends */
#define RTC_RTC_IFR_CAL_FLAG_MASK                                    (0x1000)
#define RTC_RTC_IFR_CAL_FLAG_SHIFT                                   (12)
#define RTC_RTC_IFR_CAL_FLAG(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_RTC_IFR_CAL_FLAG_SHIFT)) & RTC_RTC_IFR_CAL_FLAG_MASK)


/*! @name CALENDAR_CONTROL */
/* Calendar standby configuration bitfield:
  - 1'b0: active
  - 1'b1: disabled */
#define RTC_CALENDAR_CONTROL_SB_MASK                                 (0x1)
#define RTC_CALENDAR_CONTROL_SB_SHIFT                                (0)
#define RTC_CALENDAR_CONTROL_SB(val)                                 (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_CONTROL_SB_SHIFT)) & RTC_CALENDAR_CONTROL_SB_MASK)


/*! @name CALENDAR_TIME */
/* Calendar time seconds digit 0 bitfield. */
#define RTC_CALENDAR_TIME_SEC0_MASK                                  (0xf)
#define RTC_CALENDAR_TIME_SEC0_SHIFT                                 (0)
#define RTC_CALENDAR_TIME_SEC0(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_TIME_SEC0_SHIFT)) & RTC_CALENDAR_TIME_SEC0_MASK)

/* Calendar time seconds digit 1 bitfield. */
#define RTC_CALENDAR_TIME_SEC1_MASK                                  (0xf0)
#define RTC_CALENDAR_TIME_SEC1_SHIFT                                 (4)
#define RTC_CALENDAR_TIME_SEC1(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_TIME_SEC1_SHIFT)) & RTC_CALENDAR_TIME_SEC1_MASK)

/* Calendar time minutes digit 0 bitfield. */
#define RTC_CALENDAR_TIME_MIN0_MASK                                  (0xf00)
#define RTC_CALENDAR_TIME_MIN0_SHIFT                                 (8)
#define RTC_CALENDAR_TIME_MIN0(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_TIME_MIN0_SHIFT)) & RTC_CALENDAR_TIME_MIN0_MASK)

/* Calendar time minutes digit 1 bitfield. */
#define RTC_CALENDAR_TIME_MIN1_MASK                                  (0xf000)
#define RTC_CALENDAR_TIME_MIN1_SHIFT                                 (12)
#define RTC_CALENDAR_TIME_MIN1(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_TIME_MIN1_SHIFT)) & RTC_CALENDAR_TIME_MIN1_MASK)

/* Calendar time hours digit 0 bitfield. */
#define RTC_CALENDAR_TIME_HOU0_MASK                                  (0xf0000)
#define RTC_CALENDAR_TIME_HOU0_SHIFT                                 (16)
#define RTC_CALENDAR_TIME_HOU0(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_TIME_HOU0_SHIFT)) & RTC_CALENDAR_TIME_HOU0_MASK)

/* Calendar time hours digit 1 bitfield. */
#define RTC_CALENDAR_TIME_HOU1_MASK                                  (0xf00000)
#define RTC_CALENDAR_TIME_HOU1_SHIFT                                 (20)
#define RTC_CALENDAR_TIME_HOU1(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_TIME_HOU1_SHIFT)) & RTC_CALENDAR_TIME_HOU1_MASK)


/*! @name CALENDAR_DATE */
/* Calendar date days digit 0 bitfield. */
#define RTC_CALENDAR_DATE_DAY0_MASK                                  (0xf)
#define RTC_CALENDAR_DATE_DAY0_SHIFT                                 (0)
#define RTC_CALENDAR_DATE_DAY0(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_DATE_DAY0_SHIFT)) & RTC_CALENDAR_DATE_DAY0_MASK)

/* Calendar date days digit 1 bitfield. */
#define RTC_CALENDAR_DATE_DAY1_MASK                                  (0xf0)
#define RTC_CALENDAR_DATE_DAY1_SHIFT                                 (4)
#define RTC_CALENDAR_DATE_DAY1(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_DATE_DAY1_SHIFT)) & RTC_CALENDAR_DATE_DAY1_MASK)

/* Calendar date months digit 0 bitfield. */
#define RTC_CALENDAR_DATE_MON0_MASK                                  (0xf00)
#define RTC_CALENDAR_DATE_MON0_SHIFT                                 (8)
#define RTC_CALENDAR_DATE_MON0(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_DATE_MON0_SHIFT)) & RTC_CALENDAR_DATE_MON0_MASK)

/* Calendar date months digit 1 bitfield. */
#define RTC_CALENDAR_DATE_MON1_MASK                                  (0xf000)
#define RTC_CALENDAR_DATE_MON1_SHIFT                                 (12)
#define RTC_CALENDAR_DATE_MON1(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_DATE_MON1_SHIFT)) & RTC_CALENDAR_DATE_MON1_MASK)

/* Calendar date years digit 0 bitfield. */
#define RTC_CALENDAR_DATE_YEA0_MASK                                  (0xf0000)
#define RTC_CALENDAR_DATE_YEA0_SHIFT                                 (16)
#define RTC_CALENDAR_DATE_YEA0(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_DATE_YEA0_SHIFT)) & RTC_CALENDAR_DATE_YEA0_MASK)

/* Calendar date years digit 1 bitfield. */
#define RTC_CALENDAR_DATE_YEA1_MASK                                  (0xf00000)
#define RTC_CALENDAR_DATE_YEA1_SHIFT                                 (20)
#define RTC_CALENDAR_DATE_YEA1(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_CALENDAR_DATE_YEA1_SHIFT)) & RTC_CALENDAR_DATE_YEA1_MASK)


/*! @name ALARM_CONTROL */
/* Alarm 1 standby configuration bitfield :
  - 1'b0: Alarm is active
  - 1'b1: Alarm is inactive
  
  This bitfield is automatically updated to 1'b1 after event in signle mode. */
#define RTC_ALARM_CONTROL_SB_MASK                                    (0x1)
#define RTC_ALARM_CONTROL_SB_SHIFT                                   (0)
#define RTC_ALARM_CONTROL_SB(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM_CONTROL_SB_SHIFT)) & RTC_ALARM_CONTROL_SB_MASK)

/* Alarm 1 mode configuration bitfield :
  - 1'b0: single
  - 1'b1: repeat */
#define RTC_ALARM_CONTROL_MODE_MASK                                  (0x10)
#define RTC_ALARM_CONTROL_MODE_SHIFT                                 (4)
#define RTC_ALARM_CONTROL_MODE(val)                                  (((uint32_t)(((uint32_t)(val)) << RTC_ALARM_CONTROL_MODE_SHIFT)) & RTC_ALARM_CONTROL_MODE_MASK)

/* Alarm 1 repeat configuration bitfield :
  - 4'b0011: every second
  - 4'b0100: every minute
  - 4'b0101: every hour
  - 4'b0110: every day
  - 4'b0111: every month
  - 4'b1000: every year */
#define RTC_ALARM_CONTROL_CFG_MASK                                   (0xf0000)
#define RTC_ALARM_CONTROL_CFG_SHIFT                                  (16)
#define RTC_ALARM_CONTROL_CFG(val)                                   (((uint32_t)(((uint32_t)(val)) << RTC_ALARM_CONTROL_CFG_SHIFT)) & RTC_ALARM_CONTROL_CFG_MASK)


/*! @name ALARM1_TIME */
/* Alarm 1 time seconds digit 0 bitfield. */
#define RTC_ALARM1_TIME_SEC0_MASK                                    (0xf)
#define RTC_ALARM1_TIME_SEC0_SHIFT                                   (0)
#define RTC_ALARM1_TIME_SEC0(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_TIME_SEC0_SHIFT)) & RTC_ALARM1_TIME_SEC0_MASK)

/* Alarm 1 time seconds digit 1 bitfield. */
#define RTC_ALARM1_TIME_SEC1_MASK                                    (0xf0)
#define RTC_ALARM1_TIME_SEC1_SHIFT                                   (4)
#define RTC_ALARM1_TIME_SEC1(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_TIME_SEC1_SHIFT)) & RTC_ALARM1_TIME_SEC1_MASK)

/* Alarm 1 time minutes digit 0 bitfield. */
#define RTC_ALARM1_TIME_MIN0_MASK                                    (0xf00)
#define RTC_ALARM1_TIME_MIN0_SHIFT                                   (8)
#define RTC_ALARM1_TIME_MIN0(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_TIME_MIN0_SHIFT)) & RTC_ALARM1_TIME_MIN0_MASK)

/* Alarm 1 time minutes digit 1 bitfield. */
#define RTC_ALARM1_TIME_MIN1_MASK                                    (0xf000)
#define RTC_ALARM1_TIME_MIN1_SHIFT                                   (12)
#define RTC_ALARM1_TIME_MIN1(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_TIME_MIN1_SHIFT)) & RTC_ALARM1_TIME_MIN1_MASK)

/* Alarm 1 time hours digit 0 bitfield. */
#define RTC_ALARM1_TIME_HOU0_MASK                                    (0xf0000)
#define RTC_ALARM1_TIME_HOU0_SHIFT                                   (16)
#define RTC_ALARM1_TIME_HOU0(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_TIME_HOU0_SHIFT)) & RTC_ALARM1_TIME_HOU0_MASK)

/* Alarm 1 time hours digit 1 bitfield. */
#define RTC_ALARM1_TIME_HOU1_MASK                                    (0xf00000)
#define RTC_ALARM1_TIME_HOU1_SHIFT                                   (20)
#define RTC_ALARM1_TIME_HOU1(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_TIME_HOU1_SHIFT)) & RTC_ALARM1_TIME_HOU1_MASK)


/*! @name ALARM1_DATE */
/* Alarm 1 date days digit 0 bitfield. */
#define RTC_ALARM1_DATE_DAY0_MASK                                    (0xf)
#define RTC_ALARM1_DATE_DAY0_SHIFT                                   (0)
#define RTC_ALARM1_DATE_DAY0(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_DATE_DAY0_SHIFT)) & RTC_ALARM1_DATE_DAY0_MASK)

/* Alarm 1 date days digit 1 bitfield. */
#define RTC_ALARM1_DATE_DAY1_MASK                                    (0xf0)
#define RTC_ALARM1_DATE_DAY1_SHIFT                                   (4)
#define RTC_ALARM1_DATE_DAY1(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_DATE_DAY1_SHIFT)) & RTC_ALARM1_DATE_DAY1_MASK)

/* Alarm 1 date months digit 0 bitfield. */
#define RTC_ALARM1_DATE_MON0_MASK                                    (0xf00)
#define RTC_ALARM1_DATE_MON0_SHIFT                                   (8)
#define RTC_ALARM1_DATE_MON0(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_DATE_MON0_SHIFT)) & RTC_ALARM1_DATE_MON0_MASK)

/* Alarm 1 date months digit 1 bitfield. */
#define RTC_ALARM1_DATE_MON1_MASK                                    (0xf000)
#define RTC_ALARM1_DATE_MON1_SHIFT                                   (12)
#define RTC_ALARM1_DATE_MON1(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_DATE_MON1_SHIFT)) & RTC_ALARM1_DATE_MON1_MASK)

/* Alarm 1 date years digit 0 bitfield. */
#define RTC_ALARM1_DATE_YEA0_MASK                                    (0xf0000)
#define RTC_ALARM1_DATE_YEA0_SHIFT                                   (16)
#define RTC_ALARM1_DATE_YEA0(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_DATE_YEA0_SHIFT)) & RTC_ALARM1_DATE_YEA0_MASK)

/* Alarm 1 date years digit 1 bitfield. */
#define RTC_ALARM1_DATE_YEA1_MASK                                    (0xf00000)
#define RTC_ALARM1_DATE_YEA1_SHIFT                                   (20)
#define RTC_ALARM1_DATE_YEA1(val)                                    (((uint32_t)(((uint32_t)(val)) << RTC_ALARM1_DATE_YEA1_SHIFT)) & RTC_ALARM1_DATE_YEA1_MASK)


/*! @name COUNTDOWN_CONTROL */
/* Countdown 1 standby configuration bitfield :
  - 1'b0: Countdown is active
  - 1'b1: Countdown is inactive
  
  This bitfield is automatically updated to 1'b1 after event in signle mode. */
#define RTC_COUNTDOWN_CONTROL_SB_MASK                                (0x1)
#define RTC_COUNTDOWN_CONTROL_SB_SHIFT                               (0)
#define RTC_COUNTDOWN_CONTROL_SB(val)                                (((uint32_t)(((uint32_t)(val)) << RTC_COUNTDOWN_CONTROL_SB_SHIFT)) & RTC_COUNTDOWN_CONTROL_SB_MASK)

/* Countdown 1 mode configuration bitfield :
  - 1'b0: single
  - 1'b1: repeat */
#define RTC_COUNTDOWN_CONTROL_MODE_MASK                              (0x10)
#define RTC_COUNTDOWN_CONTROL_MODE_SHIFT                             (4)
#define RTC_COUNTDOWN_CONTROL_MODE(val)                              (((uint32_t)(((uint32_t)(val)) << RTC_COUNTDOWN_CONTROL_MODE_SHIFT)) & RTC_COUNTDOWN_CONTROL_MODE_MASK)


/*! @name COUNTDOWN1_INIT */
/* Countdown 1 initial value. */
#define RTC_COUNTDOWN1_INIT_VAL_MASK                                 (0xffffffff)
#define RTC_COUNTDOWN1_INIT_VAL_SHIFT                                (0)
#define RTC_COUNTDOWN1_INIT_VAL(val)                                 (((uint32_t)(((uint32_t)(val)) << RTC_COUNTDOWN1_INIT_VAL_SHIFT)) & RTC_COUNTDOWN1_INIT_VAL_MASK)


/*! @name COUNTDOWN1_TIMER */
/* Countdown 1 current value. */
#define RTC_COUNTDOWN1_TIMER_VAL_MASK                                (0xffffffff)
#define RTC_COUNTDOWN1_TIMER_VAL_SHIFT                               (0)
#define RTC_COUNTDOWN1_TIMER_VAL(val)                                (((uint32_t)(((uint32_t)(val)) << RTC_COUNTDOWN1_TIMER_VAL_SHIFT)) & RTC_COUNTDOWN1_TIMER_VAL_MASK)


/*! @name CKIN_DIV1 */
/* Countdown 1 CKIN clock divider 1 value bitfield. */
#define RTC_CKIN_DIV1_VAL_MASK                                       (0xffff)
#define RTC_CKIN_DIV1_VAL_SHIFT                                      (0)
#define RTC_CKIN_DIV1_VAL(val)                                       (((uint32_t)(((uint32_t)(val)) << RTC_CKIN_DIV1_VAL_SHIFT)) & RTC_CKIN_DIV1_VAL_MASK)


/*! @name CKREF_CONF */
/* Countdown 1 reference clock configuration value bitfield. */
#define RTC_CKREF_CONF_VAL_MASK                                      (0x3fffff)
#define RTC_CKREF_CONF_VAL_SHIFT                                     (0)
#define RTC_CKREF_CONF_VAL(val)                                      (((uint32_t)(((uint32_t)(val)) << RTC_CKREF_CONF_VAL_SHIFT)) & RTC_CKREF_CONF_VAL_MASK)


/*! @name RTC_TEST_REG_A */

/*! @name RTC_SR */
typedef union
{
    struct
    {
        /* RTC interrupt status bitfield :
        - 1'b0: No interruption has been requested
        - 1'b1: Interruption requested */
        uint32_t irq:1;
    } field;
    uint32_t word;
} rtc_rtc_sr_t;

/*! @name RTC_CR */
typedef union
{
    struct
    {
        /* RTC standby configuration bitfield :
        - 1'b0: RTC is in active mode
        - 1'b1: RTC is in standby mode */
        uint32_t sb:1;
        uint32_t reserved_0:3;
        /* Calibration process activation bitfield :
        - 1'b0: disabled
        - 1'b1: enabled
        This bitfield is automatically updated to 0 after calibration operation. */
        uint32_t cal_en:1;
        uint32_t reserved_1:3;
        /* Soft reset command bitfield :
        - 1'b0: No reset
        - 1'b1: Reset the calendar, alarm, countdown and clock generation features and associated registers (CALENDAR_*, ALARM*, COUNTDOWN*, CKIN_DIV* and CKREF_CONF)
        
        This bitfield is automatically updated to 1'b1 after event in signle mode. */
        uint32_t soft_rst:1;
    } field;
    uint32_t word;
} rtc_rtc_cr_t;

/*! @name RTC_CCR */
typedef union
{
    struct
    {
        /* CLOUT clock generation standby configuration bitfield :
        - 1'b0: generated
        - 1'b1: not generated */
        uint32_t ckout_sb:1;
        uint32_t reserved_0:11;
        /* CKOUT autocalibrated clock generation configuration bitfield :
        - 1'b0: not auto calibrated
        - 1'b1: auto calibrated */
        uint32_t div1_autocal:1;
        uint32_t reserved_1:3;
        /* Compensation value according to CKREF configuration bitfield. */
        uint32_t div1_comp:5;
    } field;
    uint32_t word;
} rtc_rtc_ccr_t;

/*! @name RTC_ICR */
typedef union
{
    struct
    {
        /* RTC interrupt signal mode configuration bitfield :
        - 2'b00: RTC interrupt is a high level signal
        - 2'b01: RTC interrupt is a low level signal
        - 2'b10: RTC interrupt is a high level pulse of 1 PCLK duration
        - 2'b11: RTC interrupt is a low level pulse of 1 PCLK duration */
        uint32_t mode:2;
    } field;
    uint32_t word;
} rtc_rtc_icr_t;

/*! @name RTC_IMR */
typedef union
{
    struct
    {
        /* Alarm 1 interrupt mask configuration bitfield :
        - 1'b0: enabled
        - 1'b1: disabled */
        uint32_t alarm_mask:1;
        uint32_t reserved_0:3;
        /* Timer 1 interrupt mask configuration bitfield :
        - 1'b0: enabled
        - 1'b1: disabled */
        uint32_t timer_mask:1;
        uint32_t reserved_1:7;
        /* Calibration interrupt mask configuration bitfield :
        - 1'b0: enabled
        - 1'b1: disabled */
        uint32_t cal_mask:1;
    } field;
    uint32_t word;
} rtc_rtc_imr_t;

/*! @name RTC_IFR */
typedef union
{
    struct
    {
        /* Alarm 1 interrupt status flag bitfield :
        - 1'b0: nothing
        - 1'b1: calendar reaches the date and time set in alarm 1 */
        uint32_t alarm_flag:1;
        uint32_t reserved_0:3;
        /* Timer 1 interrupt status flag bitfield :
        - 1'b0: nothing
        - 1'b1: countdown timer 1 reaches value 0 */
        uint32_t timer_flag:1;
        uint32_t reserved_1:7;
        /* Calibration interrupt status flag bitfield :
        - 1'b0: nothing
        - 1'b1: calibration process ends */
        uint32_t cal_flag:1;
    } field;
    uint32_t word;
} rtc_rtc_ifr_t;

/*! @name CALENDAR_CONTROL */
typedef union
{
    struct
    {
        /* Calendar standby configuration bitfield:
        - 1'b0: active
        - 1'b1: disabled */
        uint32_t sb:1;
    } field;
    uint32_t word;
} rtc_calendar_control_t;

/*! @name CALENDAR_TIME */
typedef union
{
    struct
    {
        /* Calendar time seconds digit 0 bitfield. */
        uint32_t sec0:4;
        /* Calendar time seconds digit 1 bitfield. */
        uint32_t sec1:4;
        /* Calendar time minutes digit 0 bitfield. */
        uint32_t min0:4;
        /* Calendar time minutes digit 1 bitfield. */
        uint32_t min1:4;
        /* Calendar time hours digit 0 bitfield. */
        uint32_t hou0:4;
        /* Calendar time hours digit 1 bitfield. */
        uint32_t hou1:4;
    } field;
    uint32_t word;
} rtc_calendar_time_t;

/*! @name CALENDAR_DATE */
typedef union
{
    struct
    {
        /* Calendar date days digit 0 bitfield. */
        uint32_t day0:4;
        /* Calendar date days digit 1 bitfield. */
        uint32_t day1:4;
        /* Calendar date months digit 0 bitfield. */
        uint32_t mon0:4;
        /* Calendar date months digit 1 bitfield. */
        uint32_t mon1:4;
        /* Calendar date years digit 0 bitfield. */
        uint32_t yea0:4;
        /* Calendar date years digit 1 bitfield. */
        uint32_t yea1:4;
    } field;
    uint32_t word;
} rtc_calendar_date_t;

/*! @name ALARM_CONTROL */
typedef union
{
    struct
    {
        /* Alarm 1 standby configuration bitfield :
        - 1'b0: Alarm is active
        - 1'b1: Alarm is inactive
        
        This bitfield is automatically updated to 1'b1 after event in signle mode. */
        uint32_t sb:1;
        uint32_t reserved_0:3;
        /* Alarm 1 mode configuration bitfield :
        - 1'b0: single
        - 1'b1: repeat */
        uint32_t mode:1;
        uint32_t reserved_1:11;
        /* Alarm 1 repeat configuration bitfield :
        - 4'b0011: every second
        - 4'b0100: every minute
        - 4'b0101: every hour
        - 4'b0110: every day
        - 4'b0111: every month
        - 4'b1000: every year */
        uint32_t cfg:4;
    } field;
    uint32_t word;
} rtc_alarm_control_t;

/*! @name ALARM1_TIME */
typedef union
{
    struct
    {
        /* Alarm 1 time seconds digit 0 bitfield. */
        uint32_t sec0:4;
        /* Alarm 1 time seconds digit 1 bitfield. */
        uint32_t sec1:4;
        /* Alarm 1 time minutes digit 0 bitfield. */
        uint32_t min0:4;
        /* Alarm 1 time minutes digit 1 bitfield. */
        uint32_t min1:4;
        /* Alarm 1 time hours digit 0 bitfield. */
        uint32_t hou0:4;
        /* Alarm 1 time hours digit 1 bitfield. */
        uint32_t hou1:4;
    } field;
    uint32_t word;
} rtc_alarm1_time_t;

/*! @name ALARM1_DATE */
typedef union
{
    struct
    {
        /* Alarm 1 date days digit 0 bitfield. */
        uint32_t day0:4;
        /* Alarm 1 date days digit 1 bitfield. */
        uint32_t day1:4;
        /* Alarm 1 date months digit 0 bitfield. */
        uint32_t mon0:4;
        /* Alarm 1 date months digit 1 bitfield. */
        uint32_t mon1:4;
        /* Alarm 1 date years digit 0 bitfield. */
        uint32_t yea0:4;
        /* Alarm 1 date years digit 1 bitfield. */
        uint32_t yea1:4;
    } field;
    uint32_t word;
} rtc_alarm1_date_t;

/*! @name COUNTDOWN_CONTROL */
typedef union
{
    struct
    {
        /* Countdown 1 standby configuration bitfield :
        - 1'b0: Countdown is active
        - 1'b1: Countdown is inactive
        
        This bitfield is automatically updated to 1'b1 after event in signle mode. */
        uint32_t sb:1;
        uint32_t reserved_0:3;
        /* Countdown 1 mode configuration bitfield :
        - 1'b0: single
        - 1'b1: repeat */
        uint32_t mode:1;
    } field;
    uint32_t word;
} rtc_countdown_control_t;

/*! @name COUNTDOWN1_INIT */
typedef union
{
    struct
    {
        /* Countdown 1 initial value. */
        uint32_t val:32;
    } field;
    uint32_t word;
} rtc_countdown1_init_t;

/*! @name COUNTDOWN1_TIMER */
typedef union
{
    struct
    {
        /* Countdown 1 current value. */
        uint32_t val:32;
    } field;
    uint32_t word;
} rtc_countdown1_timer_t;

/*! @name CKIN_DIV1 */
typedef union
{
    struct
    {
        /* Countdown 1 CKIN clock divider 1 value bitfield. */
        uint32_t val:16;
    } field;
    uint32_t word;
} rtc_ckin_div1_t;

/*! @name CKREF_CONF */
typedef union
{
    struct
    {
        /* Countdown 1 reference clock configuration value bitfield. */
        uint32_t val:22;
    } field;
    uint32_t word;
} rtc_ckref_conf_t;

/*! @name RTC_TEST_REG_A */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} rtc_rtc_test_reg_a_t;


#endif /* __RTC_PERIPH_H__ */
