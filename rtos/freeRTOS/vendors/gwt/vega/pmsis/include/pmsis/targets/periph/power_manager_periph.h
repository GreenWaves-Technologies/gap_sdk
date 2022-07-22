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

#ifndef __POWER_MANAGER_PERIPH_H__
#define __POWER_MANAGER_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- POWER_MANAGER Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** POWER_MANAGER_Type Register Layout Typedef */
typedef struct
{
    volatile uint32_t dlc_pctrl; /**< DLC PICL control register. */
    volatile uint32_t dlc_prdata; /**< DLC PICL data read register */
    volatile uint32_t dlc_sr; /**< DLC Status register */
    volatile uint32_t dlc_imr; /**< DLC Interrupt mask register */
    volatile uint32_t dlc_ifr; /**< DLC Interrupt flag register */
    volatile uint32_t dlc_ioifr; /**< DLC icu_ok interrupt flag register */
    volatile uint32_t dlc_idifr; /**< DLC icu_delayed interrupt flag register */
    volatile uint32_t dlc_imcifr; /**< DLC icu_mode_changed interrupt flag register */
} power_manager_t;


/* ----------------------------------------------------------------------------
   -- POWER_MANAGER Register Offset --
   ---------------------------------------------------------------------------- */

#define POWER_MANAGER_DLC_PCTRL_OFFSET           (0x0) /**< DLC PICL control register. */
#define POWER_MANAGER_DLC_PRDATA_OFFSET          (0x4) /**< DLC PICL data read register */
#define POWER_MANAGER_DLC_SR_OFFSET              (0x8) /**< DLC Status register */
#define POWER_MANAGER_DLC_IMR_OFFSET             (0xc) /**< DLC Interrupt mask register */
#define POWER_MANAGER_DLC_IFR_OFFSET             (0x10) /**< DLC Interrupt flag register */
#define POWER_MANAGER_DLC_IOIFR_OFFSET           (0x14) /**< DLC icu_ok interrupt flag register */
#define POWER_MANAGER_DLC_IDIFR_OFFSET           (0x18) /**< DLC icu_delayed interrupt flag register */
#define POWER_MANAGER_DLC_IMCIFR_OFFSET          (0x1c) /**< DLC icu_mode_changed interrupt flag register */


/* ----------------------------------------------------------------------------
   -- POWER_MANAGER Register Access --
   ---------------------------------------------------------------------------- */

static inline __attribute__((always_inline)) uint32_t power_manager_dlc_pctrl_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_PCTRL_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_pctrl_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_PCTRL_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_prdata_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_PRDATA_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_prdata_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_PRDATA_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_sr_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_SR_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_sr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_SR_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_imr_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_IMR_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_imr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_IMR_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_ifr_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_IFR_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_ifr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_IFR_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_ioifr_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_IOIFR_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_ioifr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_IOIFR_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_idifr_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_IDIFR_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_idifr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_IDIFR_OFFSET, value);
}


static inline __attribute__((always_inline)) uint32_t power_manager_dlc_imcifr_get(uint32_t base)
{
    return GAP_READ(base, POWER_MANAGER_DLC_IMCIFR_OFFSET);
}

static inline __attribute__((always_inline)) void power_manager_dlc_imcifr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, POWER_MANAGER_DLC_IMCIFR_OFFSET, value);
}


/* ----------------------------------------------------------------------------
   -- POWER_MANAGER Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name DLC_PCTRL */
/* Data to write on the PICL bus. */
#define POWER_MANAGER_DLC_PCTRL_PWDATA_MASK                          (0xffff0000)
#define POWER_MANAGER_DLC_PCTRL_PWDATA_SHIFT                         (16)
#define POWER_MANAGER_DLC_PCTRL_PWDATA(val)                          (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_PCTRL_PWDATA_SHIFT)) & POWER_MANAGER_DLC_PCTRL_PWDATA_MASK)

/* Direction of the transfer on the PICL bus.
  - 1'b0: write operation
  - 1'b1: read operation */
#define POWER_MANAGER_DLC_PCTRL_DIR_MASK                             (0x8000)
#define POWER_MANAGER_DLC_PCTRL_DIR_SHIFT                            (15)
#define POWER_MANAGER_DLC_PCTRL_DIR(val)                             (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_PCTRL_DIR_SHIFT)) & POWER_MANAGER_DLC_PCTRL_DIR_MASK)

/* Address of the transfer on the PICL bus. */
#define POWER_MANAGER_DLC_PCTRL_PADDR_MASK                           (0x7ffe)
#define POWER_MANAGER_DLC_PCTRL_PADDR_SHIFT                          (1)
#define POWER_MANAGER_DLC_PCTRL_PADDR(val)                           (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_PCTRL_PADDR_SHIFT)) & POWER_MANAGER_DLC_PCTRL_PADDR_MASK)

/* Start of PICL access sequence. A rising edge of the start bit starts a PICL picl transfer. Start bit remains high until the end of the sequence, which means that no new access can be performed if an access is on going. */
#define POWER_MANAGER_DLC_PCTRL_START_MASK                           (0x1)
#define POWER_MANAGER_DLC_PCTRL_START_SHIFT                          (0)
#define POWER_MANAGER_DLC_PCTRL_START(val)                           (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_PCTRL_START_SHIFT)) & POWER_MANAGER_DLC_PCTRL_START_MASK)


/*! @name DLC_PRDATA */
/* Data read on the PICL bus. This data is valid after a PICL read operation, when the picl_busy bit of the DLC_SR register becomes low. */
#define POWER_MANAGER_DLC_PRDATA_PRDATA_MASK                         (0xff)
#define POWER_MANAGER_DLC_PRDATA_PRDATA_SHIFT                        (0)
#define POWER_MANAGER_DLC_PRDATA_PRDATA(val)                         (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_PRDATA_PRDATA_SHIFT)) & POWER_MANAGER_DLC_PRDATA_PRDATA_MASK)


/*! @name DLC_SR */
/* SCU busy. This bit is set to 1'b1 if when a SCU sequence is on going. This bit is cleared at the end of the sequence. */
#define POWER_MANAGER_DLC_SR_SCU_BUSY_MASK                           (0x2)
#define POWER_MANAGER_DLC_SR_SCU_BUSY_SHIFT                          (1)
#define POWER_MANAGER_DLC_SR_SCU_BUSY(val)                           (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_SR_SCU_BUSY_SHIFT)) & POWER_MANAGER_DLC_SR_SCU_BUSY_MASK)

/* PICL busy. This bit is set to 1'b1 if when a transfer is ongoing on the PICL bus. This bit is cleared at the end of the transfer. */
#define POWER_MANAGER_DLC_SR_PICL_BUSY_MASK                          (0x1)
#define POWER_MANAGER_DLC_SR_PICL_BUSY_SHIFT                         (0)
#define POWER_MANAGER_DLC_SR_PICL_BUSY(val)                          (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_SR_PICL_BUSY_SHIFT)) & POWER_MANAGER_DLC_SR_PICL_BUSY_MASK)


/*! @name DLC_IMR */
/* Mask of scu_ok interrupt.
  - 1'b0: mask cleared
  - 1'b1: mask set */
#define POWER_MANAGER_DLC_IMR_SCU_OK_MASK                            (0x10)
#define POWER_MANAGER_DLC_IMR_SCU_OK_SHIFT                           (4)
#define POWER_MANAGER_DLC_IMR_SCU_OK(val)                            (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IMR_SCU_OK_SHIFT)) & POWER_MANAGER_DLC_IMR_SCU_OK_MASK)

/* Mask of picl_ok interrupt.
  - 1'b0: mask cleared
  - 1'b1: mask set */
#define POWER_MANAGER_DLC_IMR_PICL_OK_MASK                           (0x8)
#define POWER_MANAGER_DLC_IMR_PICL_OK_SHIFT                          (3)
#define POWER_MANAGER_DLC_IMR_PICL_OK(val)                           (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IMR_PICL_OK_SHIFT)) & POWER_MANAGER_DLC_IMR_PICL_OK_MASK)

/* Mask of icu_mode_changed interrupt.
  - 1'b0: mask cleared
  - 1'b1: mask set */
#define POWER_MANAGER_DLC_IMR_ICU_MD_CHG_MASK                        (0x4)
#define POWER_MANAGER_DLC_IMR_ICU_MD_CHG_SHIFT                       (2)
#define POWER_MANAGER_DLC_IMR_ICU_MD_CHG(val)                        (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IMR_ICU_MD_CHG_SHIFT)) & POWER_MANAGER_DLC_IMR_ICU_MD_CHG_MASK)

/* Mask of icu_delayed interrupt.
  - 1'b0: mask cleared
  - 1'b1: mask set */
#define POWER_MANAGER_DLC_IMR_ICU_DLYD_MASK                          (0x2)
#define POWER_MANAGER_DLC_IMR_ICU_DLYD_SHIFT                         (1)
#define POWER_MANAGER_DLC_IMR_ICU_DLYD(val)                          (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IMR_ICU_DLYD_SHIFT)) & POWER_MANAGER_DLC_IMR_ICU_DLYD_MASK)

/* Mask of icu_ok interrupt.
  - 1'b0: mask cleared
  - 1'b1: mask set */
#define POWER_MANAGER_DLC_IMR_ICU_OK_MASK                            (0x1)
#define POWER_MANAGER_DLC_IMR_ICU_OK_SHIFT                           (0)
#define POWER_MANAGER_DLC_IMR_ICU_OK(val)                            (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IMR_ICU_OK_SHIFT)) & POWER_MANAGER_DLC_IMR_ICU_OK_MASK)


/*! @name DLC_IFR */
/* Set to 1'b1 when SCU sequence is finished. Cleared by writing 1 to bit 4 of the DLC_IFR register. */
#define POWER_MANAGER_DLC_IFR_SCU_OK_MASK                            (0x10)
#define POWER_MANAGER_DLC_IFR_SCU_OK_SHIFT                           (4)
#define POWER_MANAGER_DLC_IFR_SCU_OK(val)                            (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IFR_SCU_OK_SHIFT)) & POWER_MANAGER_DLC_IFR_SCU_OK_MASK)

/* Set to 1'b1 when PICL transfer is finish. Cleared by writing 1 to bit 3 of the DLC_IFR register. */
#define POWER_MANAGER_DLC_IFR_PICL_OK_MASK                           (0x8)
#define POWER_MANAGER_DLC_IFR_PICL_OK_SHIFT                          (3)
#define POWER_MANAGER_DLC_IFR_PICL_OK(val)                           (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IFR_PICL_OK_SHIFT)) & POWER_MANAGER_DLC_IFR_PICL_OK_MASK)

/* Set to 1'b1 when at least one of the bit of the DLC_IMCIFR register is set. Cleared by reading the DCL_IMCIFR register. */
#define POWER_MANAGER_DLC_IFR_ICU_MD_CHG_MASK                        (0x4)
#define POWER_MANAGER_DLC_IFR_ICU_MD_CHG_SHIFT                       (2)
#define POWER_MANAGER_DLC_IFR_ICU_MD_CHG(val)                        (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IFR_ICU_MD_CHG_SHIFT)) & POWER_MANAGER_DLC_IFR_ICU_MD_CHG_MASK)

/* Set to 1'b1 when at least one of the bit of the DLC_IDIFR register is set. Cleared by reading the DCL_IDIFR register. */
#define POWER_MANAGER_DLC_IFR_ICU_DLYD_MASK                          (0x2)
#define POWER_MANAGER_DLC_IFR_ICU_DLYD_SHIFT                         (1)
#define POWER_MANAGER_DLC_IFR_ICU_DLYD(val)                          (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IFR_ICU_DLYD_SHIFT)) & POWER_MANAGER_DLC_IFR_ICU_DLYD_MASK)

/* Set to 1'b1 when at least one of the bit of the DLC_IOIFR register is set. Cleared by reading the DCL_IOIFR register. */
#define POWER_MANAGER_DLC_IFR_ICU_OK_MASK                            (0x1)
#define POWER_MANAGER_DLC_IFR_ICU_OK_SHIFT                           (0)
#define POWER_MANAGER_DLC_IFR_ICU_OK(val)                            (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IFR_ICU_OK_SHIFT)) & POWER_MANAGER_DLC_IFR_ICU_OK_MASK)


/*! @name DLC_IOIFR */
/* Flags of the icu_ok interrupts. Bit[x] is set to 1'b1 when a rising edge of the signal i_icu_ok_irq[x] occurs and MSP is requester of the change mode order of the ICU[x]. A read of this register clears the register and the bit icu_ok_flag of the DLC_IFR register. */
#define POWER_MANAGER_DLC_IOIFR_ICU_OK_FLGS_MASK                     (0xfffffffe)
#define POWER_MANAGER_DLC_IOIFR_ICU_OK_FLGS_SHIFT                    (1)
#define POWER_MANAGER_DLC_IOIFR_ICU_OK_FLGS(val)                     (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IOIFR_ICU_OK_FLGS_SHIFT)) & POWER_MANAGER_DLC_IOIFR_ICU_OK_FLGS_MASK)


/*! @name DLC_IDIFR */
/* Flags of the icu_delayed interrupts. Bit[x] is set to 1'b1 when a rising edge of the signal i_icu_delayed_irq[x] occurs and MSP is requester of the change mode order of the ICU[x]. A read this register clears the register and the bit icu_delayed_flag of the DLC_IFR register. */
#define POWER_MANAGER_DLC_IDIFR_ICU_DLYD_FLGS_MASK                   (0xfffffffe)
#define POWER_MANAGER_DLC_IDIFR_ICU_DLYD_FLGS_SHIFT                  (1)
#define POWER_MANAGER_DLC_IDIFR_ICU_DLYD_FLGS(val)                   (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IDIFR_ICU_DLYD_FLGS_SHIFT)) & POWER_MANAGER_DLC_IDIFR_ICU_DLYD_FLGS_MASK)


/*! @name DLC_IMCIFR */
/* Flags of the icu_mode_changed interrupts. Bit [x] is set to 1'b1 when a rising edge of the signal i_icu_mode_changed_irq[x] occurs. A read this register clears the register and the bit icu_mode_changed_flag of the DLC_IFR register. */
#define POWER_MANAGER_DLC_IMCIFR_ICU_MD_CHNG_FLGS_MASK               (0xfffffffe)
#define POWER_MANAGER_DLC_IMCIFR_ICU_MD_CHNG_FLGS_SHIFT              (1)
#define POWER_MANAGER_DLC_IMCIFR_ICU_MD_CHNG_FLGS(val)               (((uint32_t)(((uint32_t)(val)) << POWER_MANAGER_DLC_IMCIFR_ICU_MD_CHNG_FLGS_SHIFT)) & POWER_MANAGER_DLC_IMCIFR_ICU_MD_CHNG_FLGS_MASK)


/*! @name DLC_PCTRL */
typedef union
{
    struct
    {
        uint32_t reserved_0:16;
        /* Data to write on the PICL bus. */
        uint32_t pwdata:16;
        /* Direction of the transfer on the PICL bus.
        - 1'b0: write operation
        - 1'b1: read operation */
        uint32_t dir:1;
        /* Address of the transfer on the PICL bus. */
        uint32_t paddr:14;
        /* Start of PICL access sequence. A rising edge of the start bit starts a PICL picl transfer. Start bit remains high until the end of the sequence, which means that no new access can be performed if an access is on going. */
        uint32_t start:1;
    } field;
    uint32_t word;
} power_manager_dlc_pctrl_t;

/*! @name DLC_PRDATA */
typedef union
{
    struct
    {
        /* Data read on the PICL bus. This data is valid after a PICL read operation, when the picl_busy bit of the DLC_SR register becomes low. */
        uint32_t prdata:8;
    } field;
    uint32_t word;
} power_manager_dlc_prdata_t;

/*! @name DLC_SR */
typedef union
{
    struct
    {
        uint32_t reserved_0:1;
        /* SCU busy. This bit is set to 1'b1 if when a SCU sequence is on going. This bit is cleared at the end of the sequence. */
        uint32_t scu_busy:1;
        /* PICL busy. This bit is set to 1'b1 if when a transfer is ongoing on the PICL bus. This bit is cleared at the end of the transfer. */
        uint32_t picl_busy:1;
    } field;
    uint32_t word;
} power_manager_dlc_sr_t;

/*! @name DLC_IMR */
typedef union
{
    struct
    {
        uint32_t reserved_0:4;
        /* Mask of scu_ok interrupt.
        - 1'b0: mask cleared
        - 1'b1: mask set */
        uint32_t scu_ok:1;
        /* Mask of picl_ok interrupt.
        - 1'b0: mask cleared
        - 1'b1: mask set */
        uint32_t picl_ok:1;
        /* Mask of icu_mode_changed interrupt.
        - 1'b0: mask cleared
        - 1'b1: mask set */
        uint32_t icu_md_chg:1;
        /* Mask of icu_delayed interrupt.
        - 1'b0: mask cleared
        - 1'b1: mask set */
        uint32_t icu_dlyd:1;
        /* Mask of icu_ok interrupt.
        - 1'b0: mask cleared
        - 1'b1: mask set */
        uint32_t icu_ok:1;
    } field;
    uint32_t word;
} power_manager_dlc_imr_t;

/*! @name DLC_IFR */
typedef union
{
    struct
    {
        uint32_t reserved_0:4;
        /* Set to 1'b1 when SCU sequence is finished. Cleared by writing 1 to bit 4 of the DLC_IFR register. */
        uint32_t scu_ok:1;
        /* Set to 1'b1 when PICL transfer is finish. Cleared by writing 1 to bit 3 of the DLC_IFR register. */
        uint32_t picl_ok:1;
        /* Set to 1'b1 when at least one of the bit of the DLC_IMCIFR register is set. Cleared by reading the DCL_IMCIFR register. */
        uint32_t icu_md_chg:1;
        /* Set to 1'b1 when at least one of the bit of the DLC_IDIFR register is set. Cleared by reading the DCL_IDIFR register. */
        uint32_t icu_dlyd:1;
        /* Set to 1'b1 when at least one of the bit of the DLC_IOIFR register is set. Cleared by reading the DCL_IOIFR register. */
        uint32_t icu_ok:1;
    } field;
    uint32_t word;
} power_manager_dlc_ifr_t;

/*! @name DLC_IOIFR */
typedef union
{
    struct
    {
        uint32_t reserved_0:1;
        /* Flags of the icu_ok interrupts. Bit[x] is set to 1'b1 when a rising edge of the signal i_icu_ok_irq[x] occurs and MSP is requester of the change mode order of the ICU[x]. A read of this register clears the register and the bit icu_ok_flag of the DLC_IFR register. */
        uint32_t icu_ok_flgs:31;
    } field;
    uint32_t word;
} power_manager_dlc_ioifr_t;

/*! @name DLC_IDIFR */
typedef union
{
    struct
    {
        uint32_t reserved_0:1;
        /* Flags of the icu_delayed interrupts. Bit[x] is set to 1'b1 when a rising edge of the signal i_icu_delayed_irq[x] occurs and MSP is requester of the change mode order of the ICU[x]. A read this register clears the register and the bit icu_delayed_flag of the DLC_IFR register. */
        uint32_t icu_dlyd_flgs:31;
    } field;
    uint32_t word;
} power_manager_dlc_idifr_t;

/*! @name DLC_IMCIFR */
typedef union
{
    struct
    {
        uint32_t reserved_0:1;
        /* Flags of the icu_mode_changed interrupts. Bit [x] is set to 1'b1 when a rising edge of the signal i_icu_mode_changed_irq[x] occurs. A read this register clears the register and the bit icu_mode_changed_flag of the DLC_IFR register. */
        uint32_t icu_md_chng_flgs:31;
    } field;
    uint32_t word;
} power_manager_dlc_imcifr_t;


/* ----------------------------------------------------------------------------
   -- POWER_MANAGER CMD IDs and macros --
   ---------------------------------------------------------------------------- */




#endif /* __POWER_MANAGER_PERIPH_H__ */
