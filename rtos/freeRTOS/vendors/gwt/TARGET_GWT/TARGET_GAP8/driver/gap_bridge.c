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

#include <stddef.h>
#include "string.h"
#include "gap_bridge.h"
#include "pmsis_hal/pmsis_hal.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
bridge_req_t request;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static inline bridge_t *BRIDGE_Get()
{
    return DEBUG_GetDebugStruct();
}

static inline int BRIDGE_IsConnected(debug_struct_t *bridge) {
    return *(volatile uint32_t *)&bridge->bridge.connected;
}

static inline void BRIDGE_SetConnect(bridge_req_t *req)
{
    req->type = BRIDGE_REQ_CONNECT;
}

static inline void BRIDGE_SetDisconnect(bridge_req_t *req)
{
    req->type = BRIDGE_REQ_DISCONNECT;
}

static inline void BRIDGE_SetOpen(bridge_req_t *req, int name_len, const char* name, int flags, int mode)
{
    req->type = BRIDGE_REQ_OPEN;
    req->open.name_len = name_len;
    req->open.name = (uint32_t)(long)name;
    req->open.flags = flags;
    req->open.mode = mode;
}

static inline void BRIDGE_SetClose(bridge_req_t *req, int file)
{
    req->type = BRIDGE_REQ_CLOSE;
    req->close.file = file;
}

static inline void BRIDGE_SetRead(bridge_req_t *req, int file, void* ptr, int len)
{
    req->type = BRIDGE_REQ_READ;
    req->read.file = file;
    req->read.ptr = (uint32_t)(long)ptr;
    req->read.len = len;
}

static inline void BRIDGE_SetWrite(bridge_req_t *req, int file, void* ptr, int len)
{
    req->type = BRIDGE_REQ_WRITE;
    req->write.file = file;
    req->write.ptr = (uint32_t)(long)ptr;
    req->write.len = len;
}


static inline void BRIDGE_SetFBOpen(bridge_req_t *req, int name_len, const char* name, int width, int height, int format)
{
    req->type = BRIDGE_REQ_FB_OPEN;
    req->fb_open.name_len = name_len;
    req->fb_open.name = (uint32_t)(long)name;
    req->fb_open.width = width;
    req->fb_open.height = height;
    req->fb_open.format = format;
}

static inline void BRIDGE_SetFBUpdate(bridge_req_t *req, uint64_t fb, unsigned int addr, int posx, int posy, int width, int height)
{
    req->type = BRIDGE_REQ_FB_UPDATE;
    req->fb_update.screen = fb;
    req->fb_update.addr = addr;
    req->fb_update.posx = posx;
    req->fb_update.posy = posy;
    req->fb_update.width = width;
    req->fb_update.height = height;
}

static inline void BRIDGE_SetTargetStatusSync(bridge_req_t *req)
{
    req->type = BRIDGE_REQ_TARGET_STATUS_SYNC;
}


// This function should be called everytime the bridge may have new requests
// to handle, so that its pointer to ready requests is updated
static void BRIDGE_CheckBridgeReq()
{
    bridge_t *bridge = BRIDGE_Get();

    // first_bridge_req is owned by the runtime only when it is NULL, otherwise
    // it is owned by the bridge which is modifying it everytime a request
    // is handled
    if (bridge->firstBridgeReq == 0)
    {
        // We are owning the pointer, check if we can make it point to ready
        // requests
        bridge_req_t *req = (bridge_req_t *)bridge->firstReq;

        while (req && req->popped)
        {
            req = (bridge_req_t *)req->next;
        }

        if (req)
        {
            bridge->firstBridgeReq = (uint32_t )req;
        }
    }
}

// This function can be called by the API layer to commit a new request to
// the bridge
static void BRIDGE_PostReq(bridge_req_t *req, void *event)
{
    bridge_t *bridge = BRIDGE_Get();

    req->next = 0;
    req->done = 0;
    req->popped = 0;
    req->size = sizeof(bridge_req_t);
    /* req->event = event; */

    if (bridge->firstReq)
        ((bridge_req_t *)bridge->lastReq)->next = (uint32_t)req;
    else
        bridge->firstReq = (uint32_t)req;

    bridge->lastReq = (uint32_t)req;
    req->next = 0;

    BRIDGE_SendNotif();

    BRIDGE_CheckBridgeReq();
}

static void BRIDGE_HandleNotify(bridge_t* bridge)
{
    // Go through all the requests and handles the ones which are done
    bridge_req_t *req = (bridge_req_t *)bridge->firstReq;

    while (req && req->done)
    {
        bridge_req_t *next = (bridge_req_t *)req->next;
        bridge->firstReq = (uint32_t)next;
    }

    // Then check if we must update the bridge queue
    BRIDGE_CheckBridgeReq();
}

void BRIDGE_Delay()
{
    if (__is_FC()) {
        /* Disable IRQ */
        int irq_en = NVIC_GetEnableIRQ(FC_SOC_EVENT_IRQN);
        NVIC_DisableIRQ(FC_SOC_EVENT_IRQN);

        hal_soc_eu_set_fc_mask(REF32K_CLK_RISE_EVENT);

        int event = 0;
        do {
            event = hal_eu_evt_mask_wait_and_clr(1 << FC_SOC_EVENT_IRQN);
        } while (!(event & (1 << FC_SOC_EVENT_IRQN)));

        EU_CORE_DEMUX->BUFFER_CLEAR = (1 << FC_SOC_EVENT_IRQN);

        hal_soc_eu_clear_fc_mask(REF32K_CLK_RISE_EVENT);
        /* Restore IRQ */
        if (irq_en)
            NVIC_EnableIRQ(FC_SOC_EVENT_IRQN);
    }
}

void BRIDGE_BlockWait() {
    /* Disable IRQ */
    int irq_en = NVIC_GetEnableIRQ(FC_SW_NOTIFY_BRIDGE_EVENT);
    NVIC_DisableIRQ(FC_SW_NOTIFY_BRIDGE_EVENT);

    int event = 0;
    do {
        event = hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_BRIDGE_EVENT);
    } while (!(event & (1 << FC_SW_NOTIFY_BRIDGE_EVENT)));

    EU_CORE_DEMUX->BUFFER_CLEAR = (1 << FC_SW_NOTIFY_BRIDGE_EVENT);

    /* Restore IRQ */
    if (irq_en)
        NVIC_EnableIRQ(FC_SW_NOTIFY_BRIDGE_EVENT);
}

void BRIDGE_Init()
{
    bridge_t* bridge = BRIDGE_Get();

    /* Tell debug bridge which address to trigger software event */
    bridge->notifyReqAddr  = (uint32_t)(&FC_EU_SW_EVENTS->TRIGGER_SET[FC_SW_NOTIFY_BRIDGE_EVENT]);
    bridge->notifyReqValue = 1;

    /* Bind software event handle */
    //NVIC_SetVector(FC_SW_NOTIFY_BRIDGE_EVENT, (uint32_t)__handler_wrapper_light_BRIDGE_IRQHandler);
    /* Activate interrupt handler for soc event */
    NVIC_EnableIRQ(FC_SW_NOTIFY_BRIDGE_EVENT);
}

int BRIDGE_Connect(int wait_bridge, void *event)
{
    bridge_t *bridge = BRIDGE_Get();

    if (!wait_bridge && !BRIDGE_IsConnected(bridge)) {
        return -1;
    }

    request.next = 0;

    BRIDGE_SetConnect(&request);

    bridge->target.connected = 1;

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();

    return 0;
}

void BRIDGE_Disconnect(void *event)
{
    request.next = 0;

    BRIDGE_SetDisconnect(&request);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();
}

int BRIDGE_Open(const char* name, int flags, int mode, void *event)
{
    memset((void *)&request, 0, sizeof(bridge_req_t));

    BRIDGE_SetOpen(&request, strlen(name), name, flags, mode);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();

    return request.open.retval;
}

int BRIDGE_OpenWait(void* event)
{
    BRIDGE_BlockWait();

    return request.open.retval;
}

int BRIDGE_Close(int file, void *event)
{
    memset((void *)&request, 0, sizeof(bridge_req_t));

    BRIDGE_SetClose(&request, file);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();

    return request.close.retval;
}

int BRIDGE_CloseWait(void *event)
{
    BRIDGE_BlockWait();

    return request.close.retval;
}

int BRIDGE_Read(int file, void* ptr, int len, void *event)
{
    memset((void *)&request, 0, sizeof(bridge_req_t));

    BRIDGE_SetRead(&request, file, ptr, len);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();

    return request.read.retval;
}

int BRIDGE_ReadWait(void *event)
{
    BRIDGE_BlockWait();

    return request.read.retval;
}

int BRIDGE_Write(int file, void* ptr, int len, void *event)
{
    memset((void *)&request, 0, sizeof(bridge_req_t));

    BRIDGE_SetWrite(&request, file, ptr, len);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();

    return request.write.retval;
}

int BRIDGE_WriteWait(void *event)
{
    BRIDGE_BlockWait();

    return request.write.retval;
}

uint64_t BRIDGE_FBOpen(const char* name, int width, int height, bridge_fb_format_e format, void *event)
{
    memset((void *)&request, 0, sizeof(bridge_req_t));

    BRIDGE_SetFBOpen(&request, strlen(name), name, width, height, format);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();

    return request.fb_open.screen;
}

uint64_t BRIDGE_FBOpenWait(void *event)
{
    BRIDGE_BlockWait();

    return request.fb_open.screen;
}

void BRIDGE_FBUpdate(uint64_t fb, int addr, int posx, int posy, int width, int height, void *event)
{
    memset((void *)&request, 0, sizeof(bridge_req_t));

    BRIDGE_SetFBUpdate(&request, fb, addr, posx, posy, width, height);

    BRIDGE_PostReq(&request, NULL);

    BRIDGE_BlockWait();
}

void BRIDGE_TargetStatusSync(void *event)
{
    bridge_t *bridge = BRIDGE_Get();

    if ( BRIDGE_IsConnected(bridge) ) {
        memset((void *)&request, 0, sizeof(bridge_req_t));

        BRIDGE_SetTargetStatusSync(&request);

        BRIDGE_PostReq(&request, NULL);

        BRIDGE_BlockWait();
    }
}

void BRIDGE_CheckConnection()
{
    debug_struct_t *debug_struct = DEBUG_GetDebugStruct();

    if (!debug_struct->bridge.connected)
    {
        if (READ_SOC_CTRL_JTAGREG_EXT_BT_MD(soc_ctrl->jtagreg) == 7)
        {
            debug_struct->bridge.connected = 1;

            soc_ctrl->jtagreg = SOC_CTRL_JTAGREG_INT_BT_MD(1);

            while(READ_SOC_CTRL_JTAGREG_EXT_BT_MD(soc_ctrl->jtagreg) == 7)
            {
                BRIDGE_Delay();
            }
        }
    }
}

static void BRIDGE_SetAvailable()
{
    debug_struct_t *debug_struct = DEBUG_GetDebugStruct();

    if (!debug_struct->bridge.connected)
        soc_ctrl->jtagreg = SOC_CTRL_JTAGREG_INT_BT_MD(4);
    else
        soc_ctrl->jtagreg = SOC_CTRL_JTAGREG_INT_BT_MD(1);
}

void BRIDGE_PrintfFlush()
{
    debug_struct_t *debug_struct = DEBUG_GetDebugStruct();

    BRIDGE_CheckConnection();

    if (debug_struct->bridge.connected)
    {
        if (DEBUG_IsBusy(DEBUG_GetDebugStruct()) || !DEBUG_IsEmpty(DEBUG_GetDebugStruct()))
        {
            // Notify the bridge that he should look for requests
            BRIDGE_SendNotif();

            // We now also have to wait until it is handled so that we can clear
            // the notification
            while(DEBUG_IsBusy(DEBUG_GetDebugStruct()))
            {
                BRIDGE_Delay();
            }
            BRIDGE_ClearNotif();
        }
    }
}

void BRIDGE_SendNotif()
{
    debug_struct_t *debug_struct = DEBUG_GetDebugStruct();

    BRIDGE_CheckConnection();

    if (debug_struct->bridge.connected)
    {
        soc_ctrl->jtagreg = SOC_CTRL_JTAGREG_INT_BT_MD(3);
    }
}

void BRIDGE_ClearNotif()
{
    debug_struct_t *debug_struct = DEBUG_GetDebugStruct();

    BRIDGE_CheckConnection();

    if (debug_struct->bridge.connected)
    {
        BRIDGE_SetAvailable();
    }
}

void BRIDGE_ReqShutDown()
{
    debug_struct_t *debug_struct = DEBUG_GetDebugStruct();

    BRIDGE_CheckConnection();

    if (debug_struct->bridge.connected)
    {
        // We have to flush pending requests before sending shutdown request
        // otherwise the bridge may never see them.
        BRIDGE_PrintfFlush();

        // It can happen that the bridge is still in a state where he haven't
        // seen that we became available. Wait until this is the case.
        while(READ_SOC_CTRL_JTAGREG_EXT_BT_MD(soc_ctrl->jtagreg) == 7)
        {
            BRIDGE_Delay();
        }

        // Send the request for shutdown
        soc_ctrl->jtagreg = SOC_CTRL_JTAGREG_INT_BT_MD(2);

        // And wait until it is acknowledged
        while(READ_SOC_CTRL_JTAGREG_EXT_BT_MD(soc_ctrl->jtagreg) != 7)
        {
            BRIDGE_Delay();
        }

        // Update the status so that the bridge knows that we got the aknowledgement
        soc_ctrl->jtagreg = SOC_CTRL_JTAGREG_INT_BT_MD(0);

        // And wait until it knows it
        while(READ_SOC_CTRL_JTAGREG_EXT_BT_MD(soc_ctrl->jtagreg) == 7)
        {
            BRIDGE_Delay();
        }
    }
}

void BRIDGE_IRQHandler(void)
{
}
