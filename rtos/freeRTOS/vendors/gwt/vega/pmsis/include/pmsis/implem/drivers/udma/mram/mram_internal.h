/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#ifndef _GAP_MRAM_H_
#define _GAP_MRAM_H_

#include "pmsis.h"
#include "pmsis/task.h"
//#include "pmsis/drivers/i2c.h"
#include "pmsis/implem/hal/hal.h"

/*!
 * @addtogroup mram_driver
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define  MRAM_SECTOR_WORD_SIZE  0x200
#define  MRAM_CHIP_WORD_SIZE    0x80000

/*! @brief MRAM IRQ enable. */
typedef enum _mram_irq_enable
{
    uMRAM_EraseInterruptEnable   = 0,       /*!< Erase done interrupt. */
    uMRAM_ProgramInterruptEnable = 1,       /*!< Program done interrupt. */
    uMRAM_TRIMInterruptEnable    = 2,       /*!< TRIM configuration interrupt. */
    uMRAM_REFLineInterruptEnable = 3        /*!< Ref line interrupt. */
} mram_irq_enable_t;

typedef enum _mram_mode
{
    uMRAM_Standby        = 0,      /*!< MRAM is in standby mode, CEb is high. */
    uMRAM_Active         = 1 ,     /*!< MRAM is in active mode doing some operations, CEb is low */
    uMRAM_DeepPowerDown  = 2       /*!< MRAM is in deep power down mode, DPD and CEb are high. */
} mram_mode_t;

/*! @brief MRAM  configuration structure.*/
typedef struct _mram_config
{
    uint32_t itf;           /*!< ID of the requested interface. */
    uint32_t frequency;     /*!< Frequency configuration of MRAM peripheral. */
} pi_mram_config_t;

/** MRAM - Register Layout Typedef */
typedef struct {
    udma_core_t       udma;                       /**< MRAM UDMA general register, offset: 0x0 */
    volatile uint32_t TX_DADDR;                        /**< MRAM Destination tx address register, offset: 0x20 */
    volatile uint32_t RX_DADDR;                        /**< MRAM Destination rx address argument register, offset: 0x24 */
    volatile uint32_t STATUS;                          /**< MRAM status register, offset: 0x28 */
    volatile uint32_t MODE;                            /**< MRAM mode register, offset: 0x2C */
    volatile uint32_t ERASE_ADDR;                      /**< MRAM erase adddress register, offset: 0x30 */
    volatile uint32_t ERASE_SIZE;                      /**< MRAM erase size register, offset: 0x34 */
    volatile uint32_t CLK_DIV;                         /**< MRAM clock divider register, offset: 0x38 */
    volatile uint32_t TRIGGER;                         /**< MRAM trigger register, offset: 0x3C */
    volatile uint32_t ISR;                             /**< MRAM Interrupt status register, offset: 0x40 */
    volatile uint32_t IER;                             /**< MRAM Interrupt enable register, offset: 0x44 */
    volatile uint32_t ICR;                             /**< MRAM Interrupt flag clean register, offset: 0x48 */
} MRAM_Type;    

typedef struct {
    MRAM_Type *base;
    volatile pi_task_t *pending_task;
} pi_mram_drv_data_t;

/* ----------------------------------------------------------------------------
   -- MRAM Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup MRAM_Register_Masks MRAM Register Masks
 * @{
 */
/*! @name TX_DADDR - MRAM Destination tx address register */
#define MRAM_TX_DADDR_MASK                     (0xFFFFFU)
#define MRAM_TX_DADDR_SHIFT                    (0U)
#define MRAM_TX_DADDR(x)                       (((uint32_t)(((uint32_t)(x)) /* << MRAM_TX_DADDR_SHIFT */)) & MRAM_TX_DADDR_MASK)

/*! @name RX_DADDR - MRAM Destination rx address register */
#define MRAM_RX_DADDR_MASK                     (0xFFFFFU)
#define MRAM_RX_DADDR_SHIFT                    (0U)
#define MRAM_RX_DADDR(x)                       (((uint32_t)(((uint32_t)(x)) /* << MRAM_RX_DADDR_SHIFT */)) & MRAM_RX_DADDR_MASK)

/*! @name STATUS - MRAM status register */
#define MRAM_STATUS_ERASE_PENDING_MASK         (0x1U)
#define MRAM_STATUS_ERASE_PENDING_SHIFT        (0U)
#define READ_MRAM_STATUS_ERASE_PENDING(x)      (((uint32_t)(((uint32_t)(x)) /* & MRAM_STATUS_ERASE_PENDING_MASK */)) >> MRAM_STATUS_ERASE_PENDING_SHIFT)
#define MRAM_STATUS_TX_BUSY_MASK               (0x2U)
#define MRAM_STATUS_TX_BUSY_SHIFT              (1U)
#define READ_MRAM_STATUS_TX_BUSY(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_TX_BUSY_MASK)) & MRAM_STATUS_TX_BUSY_SHIFT)
#define MRAM_STATUS_RX_BUSY_MASK               (0x4U)
#define MRAM_STATUS_RX_BUSY_SHIFT              (2U)
#define READ_MRAM_STATUS_RX_BUSY(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_RX_BUSY_MASK)) >> MRAM_STATUS_RX_BUSY_SHIFT)
#define MRAM_STATUS_REF_LINE_PENDING_MASK      (0x8U)
#define MRAM_STATUS_REF_LINE_PENDING_SHIFT     (3U)
#define READ_MRAM_STATUS_REF_LINE_PENDING(x)   (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_REF_LINE_PENDING_MASK)) >> MRAM_STATUS_REF_LINE_PENDING_SHIFT)
#define MRAM_STATUS_ECC_ERROR_MASK             (0x30U)
#define MRAM_STATUS_ECC_ERROR_SHIFT            (4U)
#define READ_MRAM_STATUS_ECC_ERROR(x)          (((uint32_t)(((uint32_t)(x)) & MRAM_STATUS_ECC_ERROR_MASK)) >> MRAM_STATUS_ECC_ERROR_SHIFT)

/*! @name ERASE_ADDR - MRAM erase address register */
#define MRAM_ERASE_ADDR_MASK                   (0x7FFFFU)
#define MRAM_ERASE_ADDR_SHIFT                  (0U)
#define MRAM_ERASE_ADDR(x)                     (((uint32_t)(((uint32_t)(x)) /* << MRAM_ERASE_ADDR_SHIFT */)) & MRAM_ERASE_ADDR_MASK)

/*! @name ERASE_SIZE - MRAM erase size register */
#define MRAM_ERASE_SIZE_MASK                   (0x3FFU)
#define MRAM_ERASE_SIZE_SHIFT                  (0U)
#define MRAM_ERASE_SIZE(x)                     (((uint32_t)(((uint32_t)(x)) /* << MRAM_ERASE_SIZE_SHIFT */)) & MRAM_ERASE_SIZE_MASK)

/*! @name CLK_DIV - MRAM clock divider register */
#define MRAM_CLK_DIV_MASK                      (0xFFU)
#define MRAM_CLK_DIV_SHIFT                     (0U)
#define MRAM_CLK_DIV(x)                        (((uint32_t)(((uint32_t)(x)) /* << MRAM_CLK_DIV_SHIFT */)) & MRAM_CLK_DIV_MASK)
#define MRAM_CLK_DIV_EN_MASK                   (0x100U)
#define MRAM_CLK_DIV_EN_SHIFT                  (8U)
#define MRAM_CLK_DIV_EN(x)                     (((uint32_t)(((uint32_t)(x)) << MRAM_CLK_DIV_EN_SHIFT)) & MRAM_CLK_DIV_EN_MASK)

/*! @name TRIGGER - MRAM trigger register */
#define MRAM_TRIGGER_MASK                      (0x1U)
#define MRAM_TRIGGER_SHIFT                     (0U)
#define MRAM_TRIGGER(x)                        (((uint32_t)(((uint32_t)(x)) /* << MRAM_TRIGGER_SHIFT */)) & MRAM_TRIGGER_MASK)

/*! @name ISR - MRAM interrupt status register */
#define MRAM_ISR_ERASE_DONE_MASK               (0x1U)
#define MRAM_ISR_ERASE_DONE_SHIFT              (0U)
#define READ_MRAM_ISR_ERASE_DONE(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_ERASE_DONE_MASK)) >> MRAM_ISR_ERASE_DONE_SHIFT)
#define MRAM_ISR_PROGRAM_DONE_MASK             (0x2U)
#define MRAM_ISR_PROGRAM_DONE_SHIFT            (1U)
#define READ_MRAM_ISR_PROGRAM_DONE(x)          (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_PROGRAM_DONE_MASK)) >> MRAM_ISR_PROGRAM_DONE_SHIFT)
#define MRAM_ISR_TRIM_CFG_DONE_MASK            (0x4U)
#define MRAM_ISR_TRIM_CFG_DONE_SHIFT           (2U)
#define READ_MRAM_ISR_TRIM_CFG_DONE(x)         (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_TRIM_CFG_DONE_MASK)) >> MRAM_ISR_TRIM_CFG_DONE_SHIFT)
#define MRAM_ISR_REF_LINE_DONE_MASK            (0x8U)
#define MRAM_ISR_REF_LINE_DONE_SHIFT           (3U)
#define READ_MRAM_ISR_REF_LINE_DONE(x)         (((uint32_t)(((uint32_t)(x)) & MRAM_ISR_REF_LINE_DONE_MASK)) >> MRAM_ISR_REF_LINE_DONE_SHIFT)

/*! @name IER - MRAM interrupt enable register */
#define MRAM_IFR_ERASE_EN_MASK                 (0x1U)
#define MRAM_IFR_ERASE_EN_SHIFT                (0U)
#define READ_MRAM_IFR_ERASE_EN(x)              (((uint32_t)(((uint32_t)(x)) /* & MRAM_IFR_ERASE_EN_MASK */)) & MRAM_IFR_ERASE_EN_SHIFT)
#define MRAM_IFR_PROGRAM_EN_MASK               (0x2U)
#define MRAM_IFR_PROGRAM_EN_SHIFT              (1U)
#define READ_MRAM_IFR_PROGRAM_EN(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_IFR_PROGRAM_EN_MASK )) & MRAM_IFR_PROGRAM_EN_SHIFT)
#define MRAM_IFR_TRIM_EN_MASK                  (0x4U)
#define MRAM_IFR_TRIM_EN_SHIFT                 (2U)
#define READ_MRAM_IFR_TRIM_EN(x)               (((uint32_t)(((uint32_t)(x)) & MRAM_IFR_TRIM_EN_MASK )) & MRAM_IFR_TRIM_EN_SHIFT)
#define MRAM_IFR_REF_LINE_EN_MASK              (0x8U)
#define MRAM_IFR_REF_LINE_EN_SHIFT             (3U)
#define READ_MRAM_IFR_REF_LINE_EN(x)           (((uint32_t)(((uint32_t)(x)) & MRAM_IFR_REF_LINE_EN_MASK )) & MRAM_IFR_REF_LINE_EN_SHIFT)

/*! @name ICR - MRAM interrupt flag clean register */
#define MRAM_ICR_ERASE_CLR_MASK                 (0x1U)
#define MRAM_ICR_ERASE_CLR_SHIFT                (0U)
#define READ_MRAM_ICR_ERASE_EN(x)              (((uint32_t)(((uint32_t)(x)) /* & MRAM_ICR_ERASE_CLR_MASK */)) & MRAM_ICR_ERASE_CLR_SHIFT)
#define MRAM_ICR_PROGRAM_CLR_MASK               (0x2U)
#define MRAM_ICR_PROGRAM_CLR_SHIFT              (1U)
#define READ_MRAM_ICR_PROGRAM_EN(x)            (((uint32_t)(((uint32_t)(x)) & MRAM_ICR_PROGRAM_CLR_MASK )) & MRAM_ICR_PROGRAM_CLR_SHIFT)
#define MRAM_ICR_TRIM_CLR_MASK                  (0x4U)
#define MRAM_ICR_TRIM_CLR_SHIFT                 (2U)
#define READ_MRAM_ICR_TRIM_EN(x)               (((uint32_t)(((uint32_t)(x)) & MRAM_ICR_TRIM_CLR_MASK )) & MRAM_ICR_TRIM_CLR_SHIFT)
#define MRAM_ICR_REF_LINE_CLR_MASK              (0x8U)
#define MRAM_ICR_REF_LINE_CLR_SHIFT             (3U)
#define READ_MRAM_ICR_REF_LINE_EN(x)           (((uint32_t)(((uint32_t)(x)) & MRAM_ICR_REF_LINE_CLR_MASK )) & MRAM_ICR_REF_LINE_CLR_SHIFT)

#define MRAM_CMD_PWUP                          (0x00U)
#define MRAM_CMD_TRIM_CFG                      (0x01U)
#define MRAM_CMD_NORMAL_TX                     (0x02U)
#define MRAM_CMD_ERASE_CHIP                    (0x04U)
#define MRAM_CMD_ERASE_SECTOR                  (0x08U)
#define MRAM_CMD_ERASE_WORD                    (0x10U)
#define MRAM_CMD_PWDN                          (0x20U)
#define MRAM_CMD_READ_RX                       (0x40U)
#define MRAM_CMD_REF_LINE_P                    (0x80U)
#define MRAM_CMD_REF_LINE_AP                   (0xC0U)

// cfg[7:0] = {porb, retb, rstb, nvr, tmen, aref, dpd, eccbyps}
#define MRAM_MODE(cmd, cfg)                    ((uint32_t)(cmd << 8 | cfg))

/*!
 * @}
 */ /* end of group MRAM_Register_Masks */



/*! @brief MRAM transfer request type definition.*/
#define  mram_req_t udma_req_t

/*! @brief MRAM  handle typedef. */
typedef struct _mram_handle mram_handle_t;

/*! @brief MRAM  transfer structure. */
typedef struct _mram_transfer
{
    uint8_t *txData;          /*!< Send buffer. */
    uint8_t *rxData;          /*!< Receive buffer. */
    volatile size_t txDataSize; /*!< TX Transfer bytes. */
    volatile size_t rxDataSize; /*!< RX Transfer bytes. */

    uint32_t configFlags; /*!< Transfer transfer configuration flags; is used for slave.*/
} mram_transfer_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name Initialization and deinitialization
 * @{
 */
/*!
 * @brief Initializes the MRAM .
 *
 * This function initializes the MRAM configuration. This is an example use case.
 *  @code
 *   mram_config_t  Config;
 *   Config.frequency = 50000000;
 *   MRAM_Init(base, &Config, srcClock_Hz);
 *  @endcode
 *
 * @param base MRAM peripheral address.
 * @param Config Pointer to the structure mram_config_t.
 * @param srcClock_Hz Module source input clock in Hertz.
 */
void MRAM_Init(MRAM_Type *base, const pi_mram_config_t *Config, uint32_t srcClock_Hz);

void pi_mram_open(pi_device_t *device);

/*!
 * @brief De-initializes the MRAM peripheral. Call this API to gate the MRAM clock.
 * The MRAM module can't work unless the MRAM_Init is called.
 * @param base MRAM base pointer
 */
void MRAM_Deinit(MRAM_Type *base);

/*!
 * @brief  Sets the MRAM configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in the MRAM_Configure().
 * Use the initialized structure unchanged in the MRAM_Configure() or modify
 * the structure before calling the MRAM_Configure().
 * This is an example.
 * @code
 * mram_config_t config;
 * MRAM_GetDefaultConfig(&config);
 * @endcode
 * @param Config A pointer to the  configuration structure.
*/
void pi_mram_conf_init(pi_mram_config_t *config);


/* @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Sets the MRAM transfer frequency.
 *
 * @param base MRAM base pointer
 * @param frequency Frequency value in Hz
 * @param srcClock_Hz Source clock
 */
void MRAM_SetFrequency(MRAM_Type *base, uint32_t frequency, uint32_t srcClock_Hz);

/*!
 * @brief Power Up the MRAM.
 *
 * @param base MRAM base pointer
 */
void MRAM_PowerUp(MRAM_Type *base);

/*!
 * @brief MRAM TRIM configuration
 *
 *
 * @param base MRAM base pointer
 * @param address Target address
 * @param data    The pointer to write buffer
 * @param length  The length of write buffer in byte
 */
int pi_mram_trim_config(pi_device_t *device, uint32_t address, const char *data, int length);

    /*!
 * @brief Chip Erase eFlash.
 * The third mode of erase is chip erase in which all the words
 * in the array excluding the NVR’s are erased. Chip
 * contains 512K words. Chip erase erases each of these
 * words one at a time until all 512K words are completed.
 * Minimum 512K erase pulses are required to do chip
 * erase.
 *
 * @param base MRAM base pointer
 */
int pi_mram_chip_erase(pi_device_t *device);

/*!
 * @brief Sector Erase eFlash.
 * @param address Target address
 *
 * The second mode is sector erase in
 * which all the words in a sector are erased. Each sector
 * contains 4 rows of 128 words. Sector erase erases each
 * of these words one at a time until all 512 words are
 * completed. Thus minimum 512 erase pules are required
 * to do sector erase. The pulses are generated internally
 * by a timer after PROGEN is set and automatically
 * terminated by returning a high DONE status signal.

 *
 * @param base MRAM base pointer
 */
int pi_mram_sector_erase(pi_device_t *device, uint32_t address, uint32_t sector_size);

/*!
 * @brief Erase eFlash.
 *
 * The device erases on a 78-bit word basis. User may
 * choose three options to erase. If ECC is bypass, the first
 * mode is word based bit-wise erase by selectively
 * choosing which of the 78 bits in a word are to be erased
 * by inputting “1’s” for the corresponding DIN’s and “0’s”
 * disabling the rest. If ECC is not bypass, the 64 DIN’s
 * going into the ECC encoder must all set to “1’s” to
 * generate the 14 ECC parity bits and erase the entire
 * word.
 *
 * @param address Target address
 * @param length  The length of buffer in words (64bits = 1 word)
 *
 * @param base MRAM base pointer
 */
int pi_mram_word_erase(pi_device_t *device, uint32_t address, uint32_t length);

/*!
 * @brief MRAM TRIM configuration
 *
 *
 * @param base     MRAM base pointer
 * @param ref_line Reference line number
 * @param aref     Signal AREF
 * @param tmen     Signal TMEN
 */
int pi_mram_reference_line_config(pi_device_t *device, uint32_t ref_line, int aref, int tmen);

/*!
 * @brief Write datas into MRAM.
 *
 *
 * @param base MRAM base pointer
 * @param address Target address
 * @param data    The pointer to write buffer
 * @param length  The length of write buffer in byte
 * @param ecc_byp The ECC bypass flag
 */
int pi_mram_program(pi_device_t *device, uint32_t address, const char *data, int length, int ecc_byp);

/*!
 * @brief Read datas from MRAM.
 *
 *
 * @param base MRAM base pointer
 * @param address Target address
 * @param data The pointer to read buffer
 * @param length The length of read buffer in byte
 * @param ecc_byp The ECC bypass flag
 */
int pi_mram_read(pi_device_t *device, uint32_t address, char *data, int length, int ecc_byp);

/*!
 * @brief MRAM in Deep Power Down mode.
 *
 * The device enters the Deep Power Down Mode when
 * both DPD pin and CEb pin are high. Power consumption
 * is further reduced. However a minimum tDPDH
 * recovery time is needed to return the device back to
 * active mode from deep power down mode.
 *
 * @param base MRAM base pointer
 * @param mode Indicate MRAM's mode according to mram_mode_t stucture
 */
int MRAM_DeepPowerDown(MRAM_Type *base, mram_mode_t mode);

/*!
 * @brief MRAM configuration register retention.
 *
 * When VDD and VDDA are powered down after trim
 * configuration, the data can be retained in the
 * configuration registers by keeping VDD_cfg powered on
 * and setting RETb low. When RETb is low, all
 * other logic input control pins are ignored except PORb
 * which is held low. The only current drawn in this mode is
 * from the configuration registers.
 *
 * @param base MRAM base pointer
 */
int MRAM_ConfigurationRegisterRetention(MRAM_Type *base);

/*!
 * @brief MRAM Test mode enable.
 *
 * @param base MRAM base pointer
 * @param enable MRAM test mode enable
 */
int MRAM_TestMode(MRAM_Type *base, int enable);

/*!
 * @brief MRAM Enabel ECCBYPS.
 *
 * @param base MRAM base pointer
 * @param enable MRAM ECC ECCBYPS enable
 */
int MRAM_ECCBypass(MRAM_Type *base, int enable);


/*!
 * @brief Enable MRAM IRQ.
 *
 * This function enables the MRAM IER for the given IRQ mask.
 *
 * @param base           MRAM base pointer.
 * @param irq            IRQ type to enable. This is a logical OR of mram_irq_enable_t members.
 */
static void MRAM_EnableIRQ(MRAM_Type *base, mram_irq_enable_t irq)
{
    base->IER |= (1 << irq);
}

/*!
 * @brief Enable MRAM IRQ.
 *
 * This function disables the MRAM IER for the given IRQ mask.
 *
 * @param base           MRAM base pointer.
 * @param irq            IRQ type to disable. This is a logical OR of mram_irq_enable_t members.
 */
static void MRAM_DisableIRQ(MRAM_Type *base, mram_irq_enable_t irq)
{
    base->IER &= ~(1 << irq);
}
/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_MRAM_H_*/

