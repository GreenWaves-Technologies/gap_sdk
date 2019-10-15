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
#ifndef _GAP_UART_H_
#define _GAP_UART_H_

#include "gap_udma.h"

/*!
 * @addtogroup uart_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART transfer state. */
enum _uart_tansfer_states
{
    uUART_TxIdle,         /* TX idle. */
    uUART_TxBusy,         /* TX busy. */
    uUART_RxIdle,         /* RX idle. */
    uUART_RxBusy,         /* RX busy. */
    uUART_RxParityError   /* Rx parity error */
};

/*! @name Driver version */
/*@{*/
/*! @brief UART driver version 1.0.0. */
#define GAP_UART_DRIVER_VERSION (MAKE_VERSION(1, 0, 0))
/*@}*/

/*! @brief Error codes for the UART driver. */
enum _uart_status
{
    uStatus_UART_TxBusy = MAKE_STATUS(uStatusGroup_UART, 0),              /*!< Transmitter is busy. */
    uStatus_UART_RxBusy = MAKE_STATUS(uStatusGroup_UART, 1),              /*!< Receiver is busy. */
    uStatus_UART_TxIdle = MAKE_STATUS(uStatusGroup_UART, 2),              /*!< UART transmitter is idle. */
    uStatus_UART_RxIdle = MAKE_STATUS(uStatusGroup_UART, 3),              /*!< UART receiver is idle. */
    uStatus_UART_Error = MAKE_STATUS(uStatusGroup_UART, 4),               /*!< Error happens on UART. */
    uStatus_UART_ParityError = MAKE_STATUS(uStatusGroup_UART, 5),        /*!< UART parity error. */
    uStatus_UART_BaudrateNotSupport =
        MAKE_STATUS(uStatusGroup_UART, 6), /*!< Baudrate is not support in current clock source */
};

/*! @brief UART parity mode. */
typedef enum _uart_parity_mode
{
    uUART_ParityDisabled = 0x0U, /*!< Parity disabled */
    uUART_ParityEnable = 0x1U,    /*!< Parity enabled */
} uart_parity_mode_t;

/*! @brief UART bit length. */
typedef enum _uart_bit_length
{
    uUART_5bits = 0x0U, /*!< 5 bits length */
    uUART_6bits = 0x1U, /*!< 6 bits length */
    uUART_7bits = 0x2U, /*!< 7 bits length */
    uUART_8bits = 0x3U, /*!< 8 bits length */
} uart_bit_length_t;

/*! @brief UART stop bit count. */
typedef enum _uart_stop_bit_count
{
    uUART_OneStopBit = 0U, /*!< One stop bit */
    uUART_TwoStopBit = 1U, /*!< Two stop bits */
} uart_stop_bit_count_t;

/*!
 * @brief UART interrupt configuration structure, default settings all disabled.
 *
 * This structure contains the settings for all of the UART interrupt configurations.
 */
enum _uart_interrupt_enable
{
    uUART_RxEndInterruptEnable = 0,   /*!< RX active edge interrupt. */
    uUART_TxEndInterruptEnable = 1    /*!< TX active edge interrupt. */
};

/*! @brief UART configuration structure. */
typedef struct _uart_config
{
    uint32_t baudRate_Bps;         /*!< UART baud rate  */
    uart_parity_mode_t parityMode; /*!< Parity mode, disabled (default), even, odd */
    uart_stop_bit_count_t stopBitCount; /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */
    bool enableTx; /*!< Enable TX */
    bool enableRx; /*!< Enable RX */
} uart_config_t;

/*! @brief UART transfer structure. */
#define  uart_transfer_t udma_req_info_t

/*! @brief UART request structure. */
#define  uart_req_t udma_req_t

/* Forward declaration of the handle typedef. */
typedef struct _uart_handle uart_handle_t;

/*! @brief UART transfer callback function. */
typedef void (*uart_transfer_callback_t)(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);

/*! @brief UART handle structure. */
struct _uart_handle
{
    uint8_t *volatile txData;   /*!< Address of remaining data to send. */
    volatile size_t txDataSize; /*!< Size of the remaining data to send. */
    size_t txDataSizeAll;       /*!< Size of the data to send out. */
    uint8_t *volatile rxData;   /*!< Address of remaining data to receive. */
    volatile size_t rxDataSize; /*!< Size of the remaining data to receive. */
    size_t rxDataSizeAll;       /*!< Size of the data to receive. */

    uint8_t *rxRingBuffer;              /*!< Start address of the receiver ring buffer. */
    size_t rxRingBufferSize;            /*!< Size of the ring buffer. */
    volatile uint16_t rxRingBufferHead; /*!< Index for the driver to store received data into ring buffer. */
    volatile uint16_t rxRingBufferTail; /*!< Index for the user to get data from the ring buffer. */

    uart_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                    /*!< UART callback function parameter.*/

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state */
};

/* Typedef for interrupt handler. */
typedef void (*uart_isr_t)(UART_Type *base, uart_handle_t *handle);

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern uint8_t uart_is_init;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes a UART instance with a user configuration structure and peripheral clock.
 *
 * This function configures the UART module with the user-defined settings. The user can configure the configuration
 * structure and also get the default configuration by using the UART_GetDefaultConfig() function.
 * The example below shows how to use this API to configure UART.
 * @code
 *  uart_config_t uartConfig;
 *  uartConfig.baudRate_Bps = 115200U;
 *  uartConfig.parityMode = uUART_ParityDisabled;
 *  uartConfig.stopBitCount = uUART_OneStopBit;
 *  UART_Init(UART1, &uartConfig, 20000000U);
 * @endcode
 *
 * @param base UART peripheral base address.
 * @param config Pointer to the user-defined configuration structure.
 * @param srcClock_Hz UART clock source frequency in HZ.
 * @retval uStatus_UART_BaudrateNotSupport Baudrate is not support in current clock source.
 * @retval uStatus_Success Status UART initialize succeed
 */
status_t UART_Init(UART_Type *base, const uart_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief Deinitializes a UART instance.
 *
 * This function waits for TX complete, disables TX and RX, and disables the UART clock.
 *
 * @param base UART peripheral base address.
 */
void UART_Deinit(UART_Type *base);

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the UART configuration structure to a default value. The default
 * values are as follows.
 *   uartConfig->baudRate_Bps = 115200U;
 *   uartConfig->parityMode = uUART_ParityDisabled;
 *   uartConfig->stopBitCount = uUART_OneStopBit;
 *   uartConfig->enableTx = false;
 *   uartConfig->enableRx = false;
 *
 * @param config Pointer to configuration structure.
 */
void UART_GetDefaultConfig(uart_config_t *config);

/*!
 * @brief Sets the UART instance baud rate.
 *
 * This function configures the UART module baud rate. This function is used to update
 * the UART module baud rate after the UART module is initialized by the UART_Init.
 * @code
 *  UART_SetBaudRate(UART1, 115200U, 20000000U);
 * @endcode
 *
 * @param base UART peripheral base address.
 * @param baudRate_Bps UART baudrate to be set.
 * @param srcClock_Hz UART clock source freqency in Hz.
 * @retval uStatus_UART_BaudrateNotSupport Baudrate is not support in the current clock source.
 * @retval uStatus_Success Set baudrate succeeded.
 */
status_t UART_SetBaudRate(UART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz);

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets UART status flags.
 *
 * @param base UART peripheral base address.
 * @return UART status flags.
 */
uint32_t UART_GetStatusFlags(UART_Type *base);

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables UART interrupts according to the provided mask.
 *
 * This function enables the UART interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref _uart_interrupt_enable.
 * For example, to enable TX empty interrupt and RX full interrupt, do the following.
 * @code
 *     UART_EnableInterrupts(UART1,uUART_TxEndInterruptEnable | uUART_RXEndInterruptEnable);
 * @endcode
 *
 * @param base UART peripheral base address.
 * @param mask The interrupts to enable. Logical OR of @ref _uart_interrupt_enable.
 */
void UART_EnableInterrupts(UART_Type *base, uint32_t mask);

/*!
 * @brief Disables the UART interrupts according to the provided mask.
 *
 * This function disables the UART interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref _uart_interrupt_enable.
 * For example, to disable TX empty interrupt and RX full interrupt do the following.
 * @code
 *     UART_DisableInterrupts(UART1,uUART_TxEndInterruptEnable | uUART_RXEndInterruptEnable);
 * @endcode
 *
 * @param base UART peripheral base address.
 * @param mask The interrupts to disable. Logical OR of @ref _uart_interrupt_enable.
 */
void UART_DisableInterrupts(UART_Type *base, uint32_t mask);

/*!
 * @brief Gets the enabled UART interrupts.
 *
 * This function gets the enabled UART interrupts. The enabled interrupts are returned
 * as the logical OR value of the enumerators @ref _uart_interrupt_enable. To check
 * a specific interrupts enable status, compare the return value with enumerators
 * in @ref _uart_interrupt_enable.
 * For example, to check whether TX empty interrupt is enabled, do the following.
 * @code
 *     uint32_t enabledInterrupts = UART_GetEnabledInterrupts(UART1);
 *
 *     if (uUART_TxEndInterruptEnable & enabledInterrupts)
 *     {
 *         ...
 *     }
 * @endcode
 *
 * @param base UART peripheral base address.
 * @return UART interrupt flags which are logical OR of the enumerators in @ref _uart_interrupt_enable.
 */
uint32_t UART_GetEnabledInterrupts(UART_Type *base);

/* @} */


/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the UART handle.
 *
 * This function initializes the UART handle which can be used for other UART
 * transactional APIs. Usually, for a specified UART instance,
 * call this API once to get the initialized handle.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 */
void UART_TransferCreateHandle(UART_Type *base,
                               uart_handle_t *handle,
                               uart_transfer_callback_t callback,
                               void *userData);


/*!
 * @brief UART IRQ handle function.
 *
 * This function handles the UART transmit and receive IRQ request.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferHandleIRQ(UART_Type *base, uart_handle_t *handle);


/*!
 * @brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt-driven data sending. The user can get the remainBytes to find out
 * how many bytes are not sent out.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferAbortSend(UART_Type *base, uart_handle_t *handle);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to know
 * how many bytes are not received yet.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 */
void UART_TransferAbortReceive(UART_Type *base, uart_handle_t *handle);

/* @} */

/*!
 * @brief Enables or disables the UART transmitter.
 *
 * This function enables or disables the UART transmitter.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableTx(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->SETUP |= UART_SETUP_TX_ENA(1);
    }
    else
    {
        base->SETUP &= ~UART_SETUP_TX_ENA(1);
    }
}

/*!
 * @brief Enables or disables the UART receiver.
 *
 * This function enables or disables the UART receiver.
 *
 * @param base UART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void UART_EnableRx(UART_Type *base, bool enable)
{
    if (enable)
    {
        base->SETUP |= UART_SETUP_RX_ENA(1);
    }
    else
    {
        base->SETUP &= ~UART_SETUP_RX_ENA(1);
    }
}

/*!
 * @brief Writes to the TX register.
 *
 * This function writes data to the TX register directly. The upper layer must ensure
 * that the TX register is empty or TX FIFO has empty room before calling this function.
 *
 * @param base UART peripheral base address.
 * @param data The byte to write.
 */
void UART_WriteByte(UART_Type *base, uint8_t data);


/*!
 * @brief Reads the RX register directly.
 *
 * This function reads data from the RX register directly. The upper layer must
 * ensure that the RX register is full or that the TX FIFO has data before calling this function.
 *
 * @param base UART peripheral base address.
 * @return The byte read from UART data register.
 */
uint8_t UART_ReadByte(UART_Type *base);


/*!
 * @brief Transmits a buffer of data using event wait.
 *
 * This function sends data using an event wait method. This is a blocking function, which
 *  wait for all data to be written to the TX register.
 *
 *
 * @param base UART peripheral base address.
 * @param tx   The buffer address.
 * @param tx_length   The buffer length.
 * @retval uStatus_Success Successfully start the data transmission.
 * @retval uStatus_UART_TxBusy Previous transmission still not finished; data not all written to TX register yet.
 * @retval uStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferSendBlocking(UART_Type *base, const uint8_t *tx, size_t tx_length);


/*!
 * @brief Receives a buffer of data using event wait.
 *
 * This function receives data using an event wait method. This is a blocking function, which
 *  wait for all data to be received.
 *
 * @param base UART peripheral base address.
 * @param rx     The buffer address.
 * @param rx_length   The buffer length.
 * @retval uStatus_Success Successfully queue the transfer into transmit queue.
 * @retval uStatus_UART_RxBusy Previous receive request is not finished.
 * @retval uStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferReceiveBlocking(UART_Type *base, const uint8_t *rx, size_t rx_length);

/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function sends data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data to be written to the TX register. When
 * all data is written to the TX register in the ISR, the UART driver calls the callback
 * function and passes the @ref uStatus_UART_TxIdle as status parameter.
 *
 * @note The uStatus_UART_TxIdle is passed to the upper layer when all data is written
 * to the TX register.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param tx   The buffer address.
 * @param tx_length   The buffer length.
 * @retval uStatus_Success Successfully start the data transmission.
 * @retval uStatus_UART_TxBusy Previous transmission still not finished; data not all written to TX register yet.
 * @retval uStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferSendNonBlocking(UART_Type *base, uart_handle_t *handle, const uint8_t *tx, size_t tx_length);


/*!
 * @brief Receives a buffer of data using an interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function, which
 *  returns without waiting for all data to be received.
 *
 * @param base UART peripheral base address.
 * @param handle UART handle pointer.
 * @param rx     The buffer address.
 * @param rx_length   The buffer length.
 * @retval uStatus_Success Successfully queue the transfer into transmit queue.
 * @retval uStatus_UART_RxBusy Previous receive request is not finished.
 * @retval uStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferReceiveNonBlocking(UART_Type *base,
                                         uart_handle_t *handle,
                                         const uint8_t *rx,
                                         size_t rx_length);

/*!
 * @brief Get the TX busy state.
 *
 * This function test the STATUS register tx busy bit
 * Clear parity error bit when STATUS register is red
 *
 * @param base UART peripheral base address.
 */
static inline  uint32_t UART_TXBusy(UART_Type *base) {
    /* Fix, Need to use UDMA register cfg_rx_en bit to determine busy or not */
    return UDMA_TXBusy((UDMA_Type *)base);
}

/*!
 * @brief Get the RX busy state.
 *
 * This function test the STATUS register Rx busy bit
 * Clear parity error bit when STATUS register is red
 *
 * @param base UART peripheral base address.
 */
static inline uint32_t UART_RXBusy(UART_Type *base) {
    /* Fix, Need to use UDMA register cfg_rx_en bit to determine busy or not */
    return UDMA_RXBusy((UDMA_Type *)base);
}

/*!
 * @brief Get the TX UDMA channel Reamin bytes.
 *
 * @param base UART peripheral base address.
 */
static inline  uint32_t UART_TXRemainBytes(UART_Type *base) {
    /* Fix, Need to use UDMA register cfg_rx_en bit to determine busy or not */
    return UDMA_TXRemainBytes((UDMA_Type *)base);
}

/*!
 * @brief Get the RX UDMA channel Remain bytes.
 *
 * @param base UART peripheral base address.
 */
static inline uint32_t UART_RXRemainBytes(UART_Type *base) {
    /* Fix, Need to use UDMA register cfg_rx_en bit to determine busy or not */
    return UDMA_RXRemainBytes((UDMA_Type *)base);
}

/*!
 * @brief Get the RX parity error state.
 *
 * This function test the STATUS register RX parity error bit
 * Clear parity error bit when STATUS register is red
 *
 * @param base UART peripheral base address.
 */
static inline  uint32_t UART_RXParityError(UART_Type *base) {
    return (base->STATUS & UART_STATUS_RX_PE_MASK);
}

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _GAP_UART_H_ */
