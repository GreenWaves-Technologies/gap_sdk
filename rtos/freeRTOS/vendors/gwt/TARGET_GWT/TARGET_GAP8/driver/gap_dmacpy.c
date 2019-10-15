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

#include "gap_dmacpy.h"
#include "assert.h"

/*******************************************************************************
 * Function definition
 ******************************************************************************/

void DMACPY_Init(DMACPY_Type *base)
{
    UDMA_Init((UDMA_Type *) base);
}

void DMACPY_Deinit(DMACPY_Type *base)
{
    UDMA_Deinit((UDMA_Type *) base);
}

status_t DMACPY_BlockingTransfer(DMACPY_Type *base, uint32_t *src_addr, uint32_t *dst_addr,
                                 uint32_t size, dmacpy_sel_t direction)
{
    udma_req_info_t req_info;

    req_info.dataSize    = UDMA_SIZE_SIZE(size);
    req_info.configFlags = UDMA_CFG_EN(1);
    req_info.ctrl        = UDMA_CTRL_DMACPY;
    req_info.repeat.size = 0;

    switch(direction)
    {
    case uDMACPY_L22L2 :
        base->MEM_SEL              = DMACPY_MEM_SEL(1);
        req_info.u.fcTcdm.fc_addr  = 0;
        req_info.isTx              = 0;
        req_info.dataAddr          = UDMA_SADDR_ADDR(dst_addr);
        UDMA_BlockTransfer((UDMA_Type *) base, &req_info, UDMA_NO_WAIT);
        req_info.isTx              = 1;
        req_info.dataAddr          = UDMA_SADDR_ADDR(src_addr);
        return UDMA_BlockTransfer((UDMA_Type *) base, &req_info, UDMA_WAIT_RX);
        break;
    case uDMACPY_FC2L2 :
        req_info.isTx              = 0;
        req_info.dataAddr          = UDMA_SADDR_ADDR(dst_addr);
        req_info.u.fcTcdm.fc_addr  = DMACPY_ADDR(src_addr);
        base->SRC_ADDR             = DMACPY_ADDR(src_addr);
        base->MEM_SEL              = DMACPY_MEM_SEL(0);
        break;
    case uDMACPY_L22FC :
        req_info.isTx              = 1;
        req_info.dataAddr          = UDMA_SADDR_ADDR(src_addr);
        req_info.u.fcTcdm.fc_addr  = DMACPY_ADDR(dst_addr);
        base->DST_ADDR             = DMACPY_ADDR(dst_addr);
        base->MEM_SEL              = DMACPY_MEM_SEL(0);
        break;
    default :
        break;
    }

    return UDMA_BlockTransfer((UDMA_Type *) base, &req_info, UDMA_WAIT);
}

status_t DMACPY_NonBlockingTransfer(DMACPY_Type *base, uint32_t *src_addr, uint32_t *dst_addr,
                                    uint32_t size, dmacpy_sel_t direction, dmacpy_handle_t *handle)
{
    handle->state = uDMACPY_Busy;

    udma_req_t *udma_req = NULL;

    uint32_t status = 0;

    switch(direction)
    {
    case uDMACPY_L22L2 :
        /* Open RX. */
        base->MEM_SEL                   = DMACPY_MEM_SEL(1);
        udma_req                        = UDMA_FindAvailableRequest();
        udma_req->info.dataAddr         = UDMA_SADDR_ADDR(dst_addr);
        udma_req->info.dataSize         = UDMA_SIZE_SIZE(size);
        udma_req->info.configFlags      = UDMA_CFG_EN(1);
        udma_req->info.isTx             = 0;
        udma_req->info.channelId        = UDMA_EVENT_DMACPY_RX;
        udma_req->info.ctrl             = UDMA_CTRL_DUAL_RX;
        udma_req->info.task             = (uint32_t)handle;
        udma_req->info.repeat.size      = 0;
        udma_req->info.u.fcTcdm.fc_addr = 0;
        status = UDMA_SendRequest((UDMA_Type *) base, udma_req);
        /* Open TX. */
        udma_req                        = UDMA_FindAvailableRequest();
        udma_req->info.dataAddr         = UDMA_SADDR_ADDR(src_addr);
        udma_req->info.dataSize         = UDMA_SIZE_SIZE(size);
        udma_req->info.configFlags      = UDMA_CFG_EN(1);
        udma_req->info.isTx             = 1;
        udma_req->info.channelId        = UDMA_EVENT_DMACPY_TX;
        udma_req->info.ctrl             = UDMA_CTRL_DUAL_TX;
        udma_req->info.task             = 0;
        udma_req->info.repeat.size      = 0;
        udma_req->info.u.fcTcdm.fc_addr = 0;
        status = UDMA_SendRequest((UDMA_Type *) base, udma_req);
        break;
    case uDMACPY_FC2L2 :
        udma_req                        = UDMA_FindAvailableRequest();
        udma_req->info.dataAddr         = UDMA_SADDR_ADDR(dst_addr);
        udma_req->info.dataSize         = UDMA_SIZE_SIZE(size);
        udma_req->info.configFlags      = UDMA_CFG_EN(1);
        udma_req->info.isTx             = 0;
        udma_req->info.channelId        = UDMA_EVENT_DMACPY_RX;
        udma_req->info.ctrl             = UDMA_CTRL_NORMAL;//DMACPY;
        udma_req->info.task             = (uint32_t)handle;
        udma_req->info.repeat.size      = 0;
        //udma_req->info.u.fcTcdm.fc_addr = DMACPY_ADDR(src_addr);
        base->SRC_ADDR                  = DMACPY_ADDR(src_addr);
        base->MEM_SEL                   = DMACPY_MEM_SEL(0);
        status = UDMA_SendRequest((UDMA_Type *) base, udma_req);
        break;
    case uDMACPY_L22FC :
        udma_req                        = UDMA_FindAvailableRequest();
        udma_req->info.dataAddr         = UDMA_SADDR_ADDR(src_addr);
        udma_req->info.dataSize         = UDMA_SIZE_SIZE(size);
        udma_req->info.configFlags      = UDMA_CFG_EN(1);
        udma_req->info.isTx             = 1;
        udma_req->info.channelId        = UDMA_EVENT_DMACPY_TX;
        udma_req->info.ctrl             = UDMA_CTRL_NORMAL;//DMACPY;
        udma_req->info.task             = (uint32_t)handle;
        udma_req->info.repeat.size      = 0;
        udma_req->info.u.fcTcdm.fc_addr = DMACPY_ADDR(dst_addr);
        base->DST_ADDR                  = DMACPY_ADDR(dst_addr);
        base->MEM_SEL                   = DMACPY_MEM_SEL(0);
        status = UDMA_SendRequest((UDMA_Type *) base, udma_req);
        break;
    default :
        break;
    }

    return (status ? uStatus_Success : uDMACPY_Error);
}

void DMACPY_TransferCreateHandle(dmacpy_handle_t *handle, dmacpy_callback_t callback, void *userData)
{
    assert(handle);
    handle->state = uDMACPY_Idle;
    handle->callback = callback;
    handle->userData = userData;
}

void DMACPY_IRQHandler(void *arg)
{
    dmacpy_handle_t *handle = (dmacpy_handle_t *) arg;
    handle->state = uDMACPY_Idle;
    if(handle->callback != NULL)
        handle->callback(handle->userData);
}
