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

#include "gap_udma.h"

/*!
 * @addtogroup mram_driver
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define  MRAM_BASE_ADDRESS      0x1D000000
#define  MRAM_SECTOR_WORD_SIZE  0x200
#define  MRAM_CHIP_WORD_SIZE    0x80000

/*! @brief  MRAM status return codes. */
enum _mram_status
{
    uStatus_MRAM_Busy = MAKE_STATUS(uStatusGroup_MRAM, 0),            /*!< MRAM is busy with current transfer. */
    uStatus_MRAM_Idle = MAKE_STATUS(uStatusGroup_MRAM, 1),            /*!< Bus is Idle. */
    uStatus_MRAM_Error = MAKE_STATUS(uStatusGroup_MRAM, 2)            /*!< Error received during transfer. */
};

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
    uint32_t frequency;     /*!< Frequency configuration of MRAM peripheral. */
} mram_config_t;

/*! @brief MRAM transfer request type definition.*/
#define  mram_req_t udma_req_t

/*! @brief MRAM  handle typedef. */
typedef struct _mram_handle mram_handle_t;

/*! @brief MRAM  transfer callback typedef. */
typedef void (*mram_transfer_callback_t)(MRAM_Type *base,
                                               mram_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*! @brief MRAM  transfer structure. */
typedef struct _mram_transfer
{
    uint8_t *txData;          /*!< Send buffer. */
    uint8_t *rxData;          /*!< Receive buffer. */
    volatile size_t txDataSize; /*!< TX Transfer bytes. */
    volatile size_t rxDataSize; /*!< RX Transfer bytes. */

    uint32_t configFlags; /*!< Transfer transfer configuration flags; is used for slave.*/
} mram_transfer_t;


/*! @brief MRAM  handle structure. */
struct _mram_handle
{
    mram_transfer_t transfer;                    /*!< MRAM  transfer copy. */
    size_t transferSize;                               /*!< Total bytes to be transferred. */
    uint8_t state;                                     /*!< A transfer state maintained during transfer. */
    mram_transfer_callback_t callback; /*!< A callback function called when the transfer is finished. */
    void *userData;                                    /*!< A callback parameter passed to the callback function. */
};

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
void MRAM_Init(MRAM_Type *base, const mram_config_t *Config, uint32_t srcClock_Hz);

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
void MRAM_GetDefaultConfig(mram_config_t *Config);


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
int MRAM_TRIMConfig(MRAM_Type *base, uint32_t address, const char *data, int length);

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
int MRAM_ChipErase(MRAM_Type *base);

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
int MRAM_SectorErase(MRAM_Type *base, uint32_t address, uint32_t sector_size);

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
int MRAM_WordErase(MRAM_Type *base, uint32_t address, uint32_t length);

/*!
 * @brief MRAM TRIM configuration
 *
 *
 * @param base     MRAM base pointer
 * @param ref_line Reference line number
 * @param aref     Signal AREF
 * @param tmen     Signal TMEN
 */
int MRAM_ReferenceLineConfig(MRAM_Type *base, uint32_t ref_line, int aref, int tmen);

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
int MRAM_Program(MRAM_Type *base, uint32_t address, const char *data, int length, int ecc_byp);

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
int MRAM_Read(MRAM_Type *base, uint32_t address, char *data, int length, int ecc_byp);

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
 * @brief Performs a transfer on the MRAM bus.
 *
 * @note The API does not return until the transfer succeeds or fails due
 * to arbitration lost or receiving a NAK.
 *
 * @param base MRAM peripheral base address.
 * @param tx_buffer Udma tx buffer base address.
 * @param tx_length Udma tx buffer size.
 * @param rx_buffer Udma rx buffer base address.
 * @param rx_length Udma rx buffer size.
 */
status_t MRAM_TransferBlocking(MRAM_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length);


/*!
 * @brief Performs a interrupt non-blocking transfer on the MRAM bus.
 *
 * @note Calling the API returns immediately after transfer initiates. When all
 * data is transferred, the callback function is called.
 *
 * @param base MRAM base pointer.
 * @param handle pointer to mram_handle_t structure which stores the transfer state.
 * @param transfer pointer to mram_transfer_t structure.
 * @retval uStatus_Success Successfully start the data transmission.
 * @retval uStatus_MRAM_Busy Previous transmission still not finished.
 * @retval uStatus_MRAM_Error Transfer error, wait signal timeout.
 */
status_t MRAM_TransferNonBlocking(MRAM_Type *base, mram_handle_t *handle, mram_transfer_t *transfer);

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

/*!
 * @brief  interrupt handler.
 *
 * @param base MRAM base pointer.
 * @param handle pointer to mram_handle_t structure.
 */
void MRAM_TransferHandleIRQ(MRAM_Type *base, mram_handle_t *handle);

/*!
 * @brief Initializes the MRAM handle which is used in transactional functions.
 *
 * @param base MRAM base pointer.
 * @param handle pointer to mram_handle_t structure to store the transfer state.
 * @param callback pointer to user callback function.
 * @param userData user parameter passed to the callback function.
 */
void MRAM_TransferCreateHandle(MRAM_Type *base,
                                    mram_handle_t *handle,
                                    mram_transfer_callback_t callback,
                                    void *userData);
/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_MRAM_H_*/
