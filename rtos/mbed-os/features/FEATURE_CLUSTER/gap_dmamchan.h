/*
 * Copyright (c) 2017, GreenWaves Technologies, Inc.
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

#ifndef _GAP_DMAMCHAN_H_
#define _GAP_DMAMCHAN_H_

#include <cmsis.h>
#include "gap_util.h"

/*!
 * @addtogroup dmamux
 * @{
 */

#ifdef FEATURE_CLUSTER

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define GAP_DMA_TCDM2L2     0
#define GAP_DMA_L22TCDM     1

#define GAP_DMA_FIX         0
#define GAP_DMA_INC         1

#define GAP_DMA_1D          0
#define GAP_DMA_2D          1

#define GAP_DMA_NO_EVT      0
#define GAP_DMA_WITH_EVT    1

#define GAP_DMA_NO_IRQ      0
#define GAP_DMA_WITH_IRQ    1

#define GAP_DMA_PRIVATE     0
#define GAP_DMA_SHARED      1

/*! @brief DMA request stucture */
typedef struct _dma_req {
  int tid;
} dma_req_t;

/*! @name Driver version */
/*@{*/
/*! @brief DMAMCHAN driver version 1.0.0. */
#define GAP_DMAMCHAN_DRIVER_VERSION (MAKE_VERSION(1, 0, 0))
/*@}*/

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name DMAMCHAN Initialization and de-initialization
 * @{
 */

/*!
 * @brief Initializes the DMAMCHAN peripheral.
 *
 * This function ungates the DMAMCHAN clock.
 *
 * @param base DMAMCHAN peripheral base address.
 *
 */
void DMAMCHAN_Init(DMAMCHAN_Type *base);

/*!
 * @brief Deinitializes the DMAMCHAN peripheral.
 *
 * This function gates the DMAMCHAN clock.
 *
 * @param base DMAMCHAN peripheral base address.
 */
void DMAMCHAN_Deinit(DMAMCHAN_Type *base);

/* @} */

/*!
 * @brief Get the DMAMCHAN channel STATUS.
 *
 * This function get STATUS of the DMAMCHAN channel.
 *
 * @param base DMAMCHAN peripheral base address.
 */
static inline uint32_t DMAMCHAN_GetStatus(DMAMCHAN_Type *base)
{
    return base->STATUS;
}

/*!
 * @brief Free the DMAMCHAN channel TID.
 *
 * This function Free TID of the DMAMCHAN channel.
 *
 * @param base DMAMCHAN peripheral base address.
 */
static inline void DMAMCHAN_FreeTID(DMAMCHAN_Type *base, dma_req_t *req)
{
    base->STATUS = (1 << req->tid);
}


/*!
 * @brief Wait the DMAMCHAN channel End.
 *
 * This function wait the DMAMCHAN channel transfer end.
 *
 * @param base DMAMCHAN peripheral base address.
 */
static inline void DMAMCHAN_WaitRequestEnd(dma_req_t *req)
{
    while(DMAMCHAN->STATUS & (1 << req->tid)) {
        EU_EVT_MaskWaitAndClr(1 << DMA_EVT_IRQn);
    }

    DMAMCHAN_FreeTID(DMAMCHAN, req);
}

/*!
 * @name DMAMCHAN FSM sequence
 * 1. Read TID
 * 2. Set CMD -> Control
 * 3. Set CMD -> TCDM address
 * 4. Set CMD -> L2 address
 *      if (is2D)
 *        goto 5
 *      else
 *        goto 6
 * 5. Set CMD -> 2D
 * 6. IDLE
 * @{
 */
/*!
 * @brief Get the DMAMCHAN channel TID.
 *
 * This function get TID of the DMAMCHAN channel.
 *
 * @param base DMAMCHAN peripheral base address.
 */
static inline uint32_t DMAMCHAN_GetTID(DMAMCHAN_Type *base)
{
    return base->CMD;
}

/*!
 * @brief Set the DMAMCHAN Control CMD.
 *
 * This function set Control CMD of the DMAMCHAN channel.
 *
 * @param base DMAMCHAN peripheral base address.
 */
static inline void DMAMCHAN_SetCMD(DMAMCHAN_Type *base, uint16_t len, uint8_t type, uint8_t inc, uint8_t is2d, uint8_t ele, uint8_t ile, uint8_t ble)
{
    uint32_t cmd = ( (len << DMAMCHAN_CMD_LEN_Pos) | (type << DMAMCHAN_CMD_TYP_Pos) | (inc << DMAMCHAN_CMD_INC_Pos) | (is2d << DMAMCHAN_CMD_2D_Pos) | (ele << DMAMCHAN_CMD_ELE_Pos) | (ile << DMAMCHAN_CMD_ILE_Pos) | (ble << DMAMCHAN_CMD_BLE_Pos) );

    base->CMD = cmd;
}

/*!
 * @brief Set the DMAMCHAN TCDM(L1) address CMD.
 *
 * This function set TCDM(L1) address CMD of the DMAMCHAN channel.
 *
 * @param addr DMAMCHAN TCDM(L1) address address.
 */
static inline void DMAMCHAN_SetTCDMAddress(DMAMCHAN_Type *base, uint32_t addr)
{
    base->CMD = addr;
}

/*!
 * @brief Set the DMAMCHAN L2 address CMD.
 *
 * This function set L2 address CMD of the DMAMCHAN channel.
 *
 * @param base DMAMCHAN peripheral base address.
 * @param addr DMAMCHAN L2 address address.
 */
static inline void DMAMCHAN_SetL2Address(DMAMCHAN_Type *base, uint32_t addr)
{
    base->CMD = addr;
}

/*!
 * @brief Set the DMAMCHAN 2D CMD.
 *
 * This function set 2D CMD of the DMAMCHAN channel.
 *
 * @param base DMAMCHAN peripheral base address.
 * @param stride 2D STRIDE.
 * @param count  2D count.
 */
static inline void DMAMCHAN_Set2D(DMAMCHAN_Type *base, uint32_t stride, uint32_t count)
{
    #if defined(__GAP8__)
    base->CMD = (stride << DMAMCHAN_CMD_2D_STRIDE_Pos | count << DMAMCHAN_CMD_2D_COUNT_Pos);
    #elif defined(__GAP9__)
    base->CMD = count;
    base->CMD = stride;
    #endif
}

/* @} */
/*!
 * @name DMAMCHAN Channel Operation
 * @{
 */

/*!
 * @brief DMA memery 1D transfer between L2 and TCDM(L1).
 *
 * This function transfer the data in bytes between TCDM(L1) and L2, non-bloking
 *
 * @param l2_addr   DMAMCHAN L2 address.
 * @param tcdm_addr DMAMCHAN TCDM (L1) address.
 * @param length    DMAMCHAN transfer length bytes.
 * @param type      DMAMCHAN transfer direction (0 : TCDM(L1)->L2). (1 : L2->TCDM(L1)).
 * @param req       DMAMCHAN user request.
 * @note .
 */
void DMAMCHAN_Memcpy_1D(uint32_t l2_addr, uint32_t tcdm_addr, uint16_t length, uint8_t type, dma_req_t *req);

/*!
 * @brief DMA memery 2D transfer between L2 and TCDM(L1).
 *
 * This function transfer the data in bytes between TCDM(L1) and L2, non-bloking
 *
 * @param l2_addr   DMAMCHAN L2 address.
 * @param tcdm_addr DMAMCHAN TCDM (L1) address.
 * @param length    DMAMCHAN transfer length bytes.
 * @param stride    DMAMCHAN stride value for 2D transfer.
 * @param count     DMAMCHAN count value for 2D transfer.
 * @param type      DMAMCHAN transfer direction (0 : TCDM(L1)->L2). (1 : L2->TCDM(L1)).
 * @param req       DMAMCHAN user request.
 * @note .
 */
void DMAMCHAN_Memcpy_2D(uint32_t l2_addr, uint32_t tcdm_addr, uint16_t length, uint16_t stride, uint16_t count, uint8_t type, dma_req_t *req);
/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* FEATURE_CLUSTER */

/* @} */

#endif /* _GAP_DMAMCHAN_H_ */
