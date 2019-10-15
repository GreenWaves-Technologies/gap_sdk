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

#ifndef _GAP_UDMA_H_
#define _GAP_UDMA_H_

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include "cmsis.h"
#include "gap_util.h"
#include "gap_soc_eu.h"

/*!
 * @addtogroup udma
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief uDMA transfer blocking or nonblocking hint */
typedef enum _UDMAHint{
    UDMA_NO_WAIT = 0,   /*!< Channel Transfer without wait */
    UDMA_WAIT,          /*!< Channel TX Transfer with wait */
    UDMA_WAIT_RX,       /*!< Channel TX Transfer with wait RX */
} UDMAHint;

/*! @brief uDMA transfer request status */
typedef enum _UDMARequest{
    UDMA_REQ_FREE = 0,          /*!< Request is free */
    UDMA_REQ_IN_SW_QUEUE = 1,   /*!< Request is in software queue */
    UDMA_REQ_IN_HW_QUEUE = 2,   /*!< Request is in hardware queue */
} UDMARequest;

/*! @brief uDMA transfer control */
typedef enum _UDMACtrl{
    UDMA_CTRL_NORMAL    = 0,   /*!< Channel Transfer without wait */
    UDMA_CTRL_HYPERBUS  = 1,   /*!< Channel special Transfer with wait */
    UDMA_CTRL_DMACPY    = 2,   /*!< Channel special Transfer without wait */
    UDMA_CTRL_DUAL_RX   = (1 << 4),  /*!< Channel RX Transfer without wait */
    UDMA_CTRL_DUAL_TX   = (2 << 4),  /*!< Channel TX Transfer without wait */
    UDMA_CTRL_HYPERBUS_CAN_ENQUEUE  = (3 << 4 | UDMA_CTRL_HYPERBUS),  /*!< Channel HYPERBUS enqueue check */
    UDMA_CTRL_HYPERBUS_CAN_NOT_ENQUEUE  = (4 << 4 | UDMA_CTRL_HYPERBUS),  /*!< Channel HYPERBUS enqueue check */
} UDMACtrl;

/*! @brief uDMA transfer channels stucture */
typedef struct _udma_channel
{
  struct _udma_req *first;  /*!< Channel Transfer first request */
  struct _udma_req *last;   /*!< Channel Transfer last request */
  struct _udma_req *previous;   /*!< Channel Transfer last request */
  uint8_t rx_depth;           /*!< RX Channel enqueue depth */
  uint8_t tx_depth;           /*!< TX Channel enqueue depth */
  UDMA_Type *base;          /*!< Channel base pointer */
} udma_channel_t;


/*! @brief UDMA request information structure.*/
typedef struct _udma_req_info
{
    uint32_t dataAddr;          /*!< Transfer data address. */
    volatile uint32_t dataSize; /*!< Transfer data size. */
    uint8_t  isTx;              /*!< Transfer is TX. */
    uint16_t channelId;         /*!< Transfer channel ID. */
    uint32_t task;              /*!< Transfer handler. */
    uint8_t  configFlags;       /*!< Transfer channel configuration flags.*/
    uint8_t ctrl;               /*!< Transfer special control. */

    union {
      struct {
         uint32_t ext_addr;
         uint32_t reg_mem_access;
      } hyperbus;
      struct {
          uint32_t fc_addr;
      } fcTcdm;
    } u;

    struct {
        int size;
        int size_total;
    } repeat;

} udma_req_info_t;

/*! @brief uDMA transfer request stucture */
typedef struct _udma_req {
  struct _udma_req_info info;    /*!< Transfer all information. */
  struct _udma_req *next;        /*!< Transfer next request. */
  udma_channel_t *channel;       /*!< Transfer channel pointer. */
  int pending;                   /*!< Transfer pending flag. */
} udma_req_t;



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
 * @brief Initializes the uDMA.
 *
 * This function ungates the uDMA clock and configures the uDMA peripheral according
 * to the configuration structure.
 *
 * @param base The UDMA channel base pointer.
 * @note .
 */
void UDMA_Init(UDMA_Type *base);


/*!
 * @brief Deinitializes the uDMA.
 *
 * This function closes the peripheral clock.
 *
 * @param base The UDMA channel.
 * @note .
 */
void UDMA_Deinit(UDMA_Type *base);

/* @} */


/*!
 * @name Request Control
 * @{
 */

/*!
 * @brief Get a request from request queue
 *
 * Allocate request number statically, if all requests are in use, wait.
 *
 * @return request pointer in request queue.
 * @note .
 */
udma_req_t * UDMA_FindAvailableRequest();

/* @} */


/*!
 * @name Transfer Control
 * @{
 */

/*!
 * @brief UDMA blocking transfer.
 *
 *
 * @param base The UDMA channel base pointer.
 * @param info pointer to udma_req_info_t structure.
 * @param hint Indicate waiting the udma channel event or not.
 * @return status of status_t.
 * @note .
 */
status_t UDMA_BlockTransfer(UDMA_Type *base, udma_req_info_t *info, UDMAHint hint);


/*!
 * @brief Create a uDMA request from request pool.
 *
 * This function create a UDMA request from the request pool and send this request to queue
 *
 * @param base The UDMA channel base pointer.
 * @param req  The UDMA request.
 * @return status of status_t.
 * @note .
 */
status_t UDMA_SendRequest(UDMA_Type *base, udma_req_t *req);


/*!
 * @brief Abort a uDMA send transfer.
 *
 * This function will abort a UDMA transfer and delete the TX request from request pool
 *
 * @param base The UDMA channel base pointer.
 * @note .
 */
status_t UDMA_AbortSend(UDMA_Type *base);

/*!
 * @brief Abort a uDMA receive transfer.
 *
 * This function will abort a UDMA transfer and delete the RX request from request pool
 *
 * @param base The UDMA channel base pointer.
 * @note .
 */
status_t UDMA_AbortReceive(UDMA_Type *base);

/*!
 * @brief UDMA channel event interrupt handler.
 *
 * This function check the finished channel index, if it has task, then finish the task.
 * then exeute next request.
 *
 * @param index The UDMA channel index.
 * @param abort Indicate the abort of dedicate UDMA channel.
 * @note .
 */
void UDMA_EventHandler(uint32_t index, int abort);


/*!
 * @brief Wait the udma channel transfer end, for blocking transfer.
 *
 * @note .
 */
void UDMA_BlockWait();

/* @} */



/*!
 * @name Busy state
 * @{
 */

/*!
 * @brief Get UDMA TX channel busy state.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline int UDMA_TXBusy(UDMA_Type *base)
{
    return ((base->TX_CFG & UDMA_CFG_EN_MASK) >> UDMA_CFG_EN_SHIFT);
}

/*!
 * @brief Get UDMA RX channel busy state.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline int UDMA_RXBusy(UDMA_Type *base)
{
    return ((base->RX_CFG & UDMA_CFG_EN_MASK) >> UDMA_CFG_EN_SHIFT);
}

/*!
 * @brief Get UDMA TX channel pending state.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline int UDMA_TXPending(UDMA_Type *base)
{
    return ((base->TX_CFG & UDMA_CFG_PENDING_MASK) >> UDMA_CFG_PENDING_SHIFT);
}

/*!
 * @brief Get UDMA RX channel pending state.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline int UDMA_RXPending(UDMA_Type *base)
{
    return ((base->RX_CFG & UDMA_CFG_PENDING_MASK) >> UDMA_CFG_PENDING_SHIFT);
}

/*!
 * @brief Get UDMA TX channel transfer left size which has not been transfered.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline int UDMA_TXRemainBytes(UDMA_Type *base)
{
    return (base->TX_SIZE & UDMA_SIZE_SIZE_MASK);
}

/*!
 * @brief Get UDMA RX channel transfer left size which has not been transfered.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline int UDMA_RXRemainBytes(UDMA_Type *base)
{
    return (base->RX_SIZE & UDMA_SIZE_SIZE_MASK);
}

/*!
 * @brief Get UDMA RX channel clear and stop.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline void UDMA_RXClearAndStop(UDMA_Type *base)
{
    base->RX_CFG |= UDMA_CFG_CLR(1);
}

/*!
 * @brief Get UDMA RX channel clear and stop.
 *
 * @param base The UDMA channel.
 * @note .
 */
static inline void UDMA_TXClearAndStop(UDMA_Type *base)
{
    base->TX_CFG |= UDMA_CFG_CLR(1);
}

/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_UDMA_H_*/
