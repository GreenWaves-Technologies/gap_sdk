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

#ifndef _GAP_DMACPY_H_
#define _GAP_DMACPY_H_

#include "GAP8.h"
#include "gap_udma.h"

/*!
 * @addtogroup dmacopy
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

/*! @brief DMAcopy module status. */
typedef enum
{
    uDMACPY_Idle  = 0x0,  /*!< DMAcopy is idle. */
    uDMACPY_Error = 0x1,  /*!< Error during transfer. */
    uDMACPY_Busy  = 0x2   /*!< DMAcopy is busy with a transfer. */
} dmacpy_status_t;

/*! @brief DMAcopy type of transfer. */
typedef enum
{
    uDMACPY_L22L2 = 0x0, /*!< Transfer from L2 to L2. */
    uDMACPY_FC2L2 = 0x1, /*!< Transfer from FC to L2. */
    uDMACPY_L22FC = 0x2  /*!< Transfer from L2 to FC. */
} dmacpy_sel_t;

/*!
 * @brief Completion callback function pointer type.
 *
 * When an asynchronous is completed, the handler calls this callback function.
 *
 * @param userData  Parameter passed to the callback function by the user.
 */
typedef void (*dmacpy_callback_t)(void *useData);

/*!
 * @brief DMAcopy handler structure.
 *
 * This structure holds information to handle events from UDMA upon asynchronous transfers completion.
 * When asynchronous transfers are used, this structure should be filled.
 */
typedef struct _dmacpy_handle_t
{
    dmacpy_status_t state;
    dmacpy_callback_t callback;
    void *userData;
} dmacpy_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name DMAcopy module configuration.
 * @{
 */

/*!
 * @brief Initialize the DMACPY module.
 *
 * This function intializes the DMAcopy module for transfers between FC_TCDM/L2 and L2.
 *
 * @param base         DMACPY base pointer.
 */
void DMACPY_Init(DMACPY_Type *base);

/*!
 * @brief Release the DMACPY module.
 *
 * @param base         DMACPY base pointer.
 */
void DMACPY_Deinit(DMACPY_Type *base);

/* @} */

/*!
 * @name Synchronous operation(blocking function).
 * @{
 */

/*!
 * @brief Blocking transfer.
 *
 * This function initiates a blocking transfer between FC_TCDM/L2 and L2.
 *
 * @param base         DMACPY base pointer.
 * @param src_addr     Source buffer.
 * @param dst_addr     Dest buffer.
 * @param size         Size of data to transfer.
 * @param direction    Direction of the transfer.
 *
 * @return Return uStatus_Success if the read operation is successful, an error otherwise.
 */
status_t DMACPY_BlockingTransfer(DMACPY_Type *base, uint32_t *src_addr, uint32_t *dst_addr,
                                 uint32_t size, dmacpy_sel_t direction);

/* @} */

/*!
 * @name Asynchronous operation(non blocking function).
 * @{
 */

/*!
 * @brief Non blocking transfer.
 *
 * This function is used for non blocking transactions using UDMA.
 * Once the UDMA, called for the transfer operations, is configured, this function returns.
 *
 * @param base         DMACPY base pointer.
 * @param src_addr     Source buffer.
 * @param dst_addr     Dest buffer.
 * @param size         Size of data to transfer.
 * @param direction    Direction of the transfer.
 * @param handle       Pointer to dmacpy_handle_t structure.
 *
 * @return status_t.
 */
status_t DMACPY_NonBlockingTransfer(DMACPY_Type *base, uint32_t *src_addr, uint32_t *dst_addr,
                                    uint32_t size, dmacpy_sel_t direction, dmacpy_handle_t *handle);

/* @} */

/*!
 * @name IRQ Handler.
 * @{
 */

/*!
 * @brief Initialize the DMACPY IRQ Handler.
 *
 * This function creates a IRQ handler for DMACPY non blocking operations.
 * The callback function passed to this function is called when transaction is done.
 *
 * @param handle       Pointer to dmacpy_handle_t structure.
 * @param callback     Callback function.
 * @param userData     Parameter passed to the callback function.
 */
void DMACPY_TransferCreateHandle(dmacpy_handle_t *handle, dmacpy_callback_t callback, void *userData);

/*!
 * @brief DMACPY IRQ Handler.
 *
 * This function is called when a non blocking transfer is completed.
 * When called, the callback function previously defined is executed.
 *
 * @param arg          Callback function.
 */
void DMACPY_IRQHandler(void *arg);

/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_DMACPY_H_*/
