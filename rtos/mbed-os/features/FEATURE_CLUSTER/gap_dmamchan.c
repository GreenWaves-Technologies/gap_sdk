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
#include <assert.h>
#include <stddef.h>
#include "gap_dmamchan.h"
#include "gap_handler_wrapper.h"

#ifdef FEATURE_CLUSTER
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void DMAMCHAN_Init(DMAMCHAN_Type *base)
{
}

void DMAMCHAN_Deinit(DMAMCHAN_Type *base)
{
}

static inline uint32_t DMAMCHAN_SendRequest1D(DMAMCHAN_Type *base, uint32_t tcdm_addr, uint32_t l2_addr, uint16_t length, uint8_t type)
{
    /* FSM 1 */
    uint32_t tid = DMAMCHAN_GetTID(base);
    /* FSM 2 */
    DMAMCHAN_SetCMD(base, length, type, GAP_DMA_INC, GAP_DMA_1D, GAP_DMA_WITH_EVT, GAP_DMA_NO_IRQ, GAP_DMA_SHARED);
    /* FSM 3 */
    DMAMCHAN_SetTCDMAddress(base, tcdm_addr);
    /* FSM 4 */
    DMAMCHAN_SetL2Address(base, l2_addr);

    return tid;
}

static inline uint32_t DMAMCHAN_SendRequest2D(DMAMCHAN_Type *base, uint32_t tcdm_addr, uint32_t l2_addr, uint16_t length, uint16_t stride, uint16_t count, uint8_t type)
{
    /* FSM 1 */
    uint32_t tid = DMAMCHAN_GetTID(base);
    /* FSM 2 */
    DMAMCHAN_SetCMD(base, length, type, GAP_DMA_INC, GAP_DMA_2D, GAP_DMA_WITH_EVT, GAP_DMA_NO_IRQ, GAP_DMA_SHARED);
    /* FSM 3 */
    DMAMCHAN_SetTCDMAddress(base, tcdm_addr);
    /* FSM 4 */
    DMAMCHAN_SetL2Address(base, l2_addr);
    /* FSM 5 */
    DMAMCHAN_Set2D(base, stride, count);

    return tid;
}

void DMAMCHAN_Memcpy_1D(uint32_t l2_addr, uint32_t tcdm_addr, uint16_t length, uint8_t type, dma_req_t *req)
{
    req->tid = DMAMCHAN_SendRequest1D(DMAMCHAN, tcdm_addr, l2_addr, length, type);
}

void DMAMCHAN_Memcpy_2D(uint32_t l2_addr, uint32_t tcdm_addr, uint16_t length, uint16_t stride, uint16_t count, uint8_t type, dma_req_t *req)
{
    req->tid = DMAMCHAN_SendRequest2D(DMAMCHAN, tcdm_addr, l2_addr, length, stride, count, type);
}

#endif /* FEATURE_CLUSTER */
