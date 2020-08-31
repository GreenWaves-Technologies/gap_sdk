/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PMSIS_IMPLEM_HAL_PMU_PMU_DLC_H__
#define __PMSIS_IMPLEM_HAL_PMU_PMU_DLC_H__

/**
 * addr[9:0] = [CHIP_SEL_ADDR[4:0] | REG_ADDR[4:0]]
 * DLC_PCTRL.addr = addr
 */
#define PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL_MASK     (0x3E0U)
#define PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL_SHIFT    (5U)
#define PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(x)       (((uint32_t)(((uint32_t)(x)) << PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL_SHIFT)) & PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL_MASK)

#define PI_PMU_DLC_PCTRL_ADDR_REG_MASK          (0x1FU)
#define PI_PMU_DLC_PCTRL_ADDR_REG_SHIFT         (0U)
#define PI_PMU_DLC_PCTRL_ADDR_REG(x)            (((uint32_t)(((uint32_t)(x)) << PI_PMU_DLC_PCTRL_ADDR_REG_SHIFT)) & PI_PMU_DLC_PCTRL_ADDR_REG_MASK)

/* CHIP_SEL_ADDR[4:0]*/
#define PICL_ADDR_WIU     ( 0x00 )
#define PICL_ADDR_ICU     ( 0x01 )

/* REG_ADDR[4:0]*/
#define WIU_ISPMR_0 (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x00))
#define WIU_ISPMR_1 (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x01))
#define WIU_IFR_0   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x02))
#define WIU_IFR_1   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x03))
#define WIU_ICR_0   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x04))
#define WIU_ICR_1   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x05))
#define WIU_ICR_2   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x06))
#define WIU_ICR_3   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x07))
#define WIU_ICR_4   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x08))
#define WIU_ICR_5   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x09))
#define WIU_ICR_6   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x0A))
#define WIU_ICR_7   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x0B))
#define WIU_ICR_8   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x0C))
#define WIU_ICR_9   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x0D))
#define WIU_ICR_10  (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x0E))
#define WIU_ICR_11  (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x0F))
#define WIU_ICR_12  (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x10))
#define WIU_ICR_13  (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x11))
#define WIU_ICR_14  (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x12))
#define WIU_ICR_15  (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_WIU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x13))

/* REG_ADDR[4:0]*/
#define ICU_CR      (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_ICU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x00))
#define ICU_MR      (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_ICU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x01))
#define ICU_ISMR    (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_ICU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x02))
#define ICU_DMR_0   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_ICU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x03))
#define ICU_DMA_1   (PI_PMU_DLC_PCTRL_ADDR_CHIP_SEL(PICL_ADDR_ICU) | PI_PMU_DLC_PCTRL_ADDR_REG(0x04))


/*! DLC_PCTRL register. */
static inline void pmu_dlc_dlc_pctrl_set(uint32_t value)
{
    hal_write32(&(pmu_dlc->dlc_pctrl), value);
}

static inline uint32_t pmu_dlc_dlc_pctrl_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_pctrl));
}


/*! DLC_PRDATA register. */
static inline uint32_t pmu_dlc_dlc_prdata_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_prdata));
}


/*! DLC_SR register. */
static inline uint32_t pmu_dlc_dlc_sr_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_sr));
}


/*! DLC_IMR register. */
static inline void pmu_dlc_dlc_imr_set(uint32_t value)
{
    hal_write32(&(pmu_dlc->dlc_imr), value);
}

static inline uint32_t pmu_dlc_dlc_imr_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_imr));
}


/*! DLC_IFR register. */
static inline void pmu_dlc_dlc_ifr_set(uint32_t value)
{
    hal_write32(&(pmu_dlc->dlc_ifr), value);
}

static inline uint32_t pmu_dlc_dlc_ifr_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_ifr));
}


/*! DLC_IOIFR register. */
static inline void pmu_dlc_dlc_ioifr_set(uint32_t value)
{
    hal_write32(&(pmu_dlc->dlc_ioifr), value);
}

static inline uint32_t pmu_dlc_dlc_ioifr_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_ioifr));
}


/*! DLC_IDIFR register. */
static inline void pmu_dlc_dlc_idifr_set(uint32_t value)
{
    hal_write32(&(pmu_dlc->dlc_idifr), value);
}

static inline uint32_t pmu_dlc_dlc_idifr_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_idifr));
}


/*! DLC_IMCIFR register. */
static inline void pmu_dlc_dlc_imcifr_set(uint32_t value)
{
    hal_write32(&(pmu_dlc->dlc_imcifr), value);
}

static inline uint32_t pmu_dlc_dlc_imcifr_get(void)
{
    return hal_read32(&(pmu_dlc->dlc_imcifr));
}


/*! DLC_PCTRL. */
static inline void hal_pmu_dlc_pctrl_set(uint16_t paddr, uint8_t is_read,
                                         uint16_t pwdata)
{
    uint32_t pctrl = (PMU_DLC_DLC_PCTRL_PWDATA(pwdata) |
                      PMU_DLC_DLC_PCTRL_DIR(is_read) |
                      PMU_DLC_DLC_PCTRL_PADDR(paddr) |
                      PMU_DLC_DLC_PCTRL_START(1));
    pmu_dlc_dlc_pctrl_set(pctrl);
}

static inline uint8_t hal_pmu_dlc_pctrl_busy_get(void)
{
    pmu_dlc_dlc_pctrl_t pctrl;
    pctrl.word = pmu_dlc_dlc_pctrl_get();
    return pctrl.field.start;
}


/*! DLC_PRDATA. */
static inline uint32_t hal_pmu_dlc_prdata_get(void)
{
    return pmu_dlc_dlc_prdata_get();
}


/*! DLC_SR. */
static inline uint32_t hal_pmu_dlc_status_get(void)
{
    return pmu_dlc_dlc_sr_get();
}

static inline uint8_t hal_pmu_dlc_picl_busy_get(void)
{
    uint32_t status = pmu_dlc_dlc_sr_get();
    status = ((status & PMU_DLC_DLC_SR_PICL_BUSY_MASK) >> PMU_DLC_DLC_SR_PICL_BUSY_SHIFT);
    return status;
}

static inline uint8_t hal_pmu_dlc_scu_busy_get(void)
{
    uint32_t status = pmu_dlc_dlc_sr_get();
    status = ((status & PMU_DLC_DLC_SR_SCU_BUSY_MASK) >> PMU_DLC_DLC_SR_SCU_BUSY_SHIFT);
    return status;
}


/*! DLC_IMR. */
static inline uint32_t hal_pmu_dlc_imr_mask_get(void)
{
    return pmu_dlc_dlc_imr_get();
}

static inline void hal_pmu_dlc_imr_mask_set(uint32_t mask)
{
    pmu_dlc_dlc_imr_set(mask);
}


/*! DLC_IFR. */
static inline uint32_t hal_pmu_dlc_ifr_get(void)
{
    return pmu_dlc_dlc_ifr_get();
}

static inline void hal_pmu_dlc_ifr_mask_clear(uint32_t mask)
{
    pmu_dlc_dlc_ifr_set(mask);
}



static inline uint32_t hal_pmu_dlc_ioifr_get(void)
{
    return __builtin_pulp_OffsetedRead(0, (uint32_t) &(pmu_dlc->dlc_ioifr));
}

static inline uint32_t hal_pmu_dlc_idifr_get(void)
{
    return __builtin_pulp_OffsetedRead(0, (uint32_t) &(pmu_dlc->dlc_idifr));
}

static inline uint32_t hal_pmu_dlc_imcifr_get(void)
{
    return __builtin_pulp_OffsetedRead(0, (uint32_t) &(pmu_dlc->dlc_imcifr));
}

#endif  /* __PMSIS_IMPLEM_HAL_PMU_PMU_DLC_H__ */
