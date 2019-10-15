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

#include "gap_cluster_fc_delegate.h"
#include "gap_common.h"

#ifdef FEATURE_CLUSTER

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* handler wrapper  */
Handler_Wrapper(CLUSTER_CL2FC_Handler);

/*******************************************************************************
 * Variables
 ******************************************************************************/
fc_call_t fc_task;

/*******************************************************************************
 * Code
 ******************************************************************************/
void CLUSTER_FC_Delegate_Init(){
    /* Initialize shared tasks */
    memset(&fc_task, 0, sizeof(fc_task));

    /* Activate interrupt handler for FC when cluster want to push a task to FC */
    NVIC_SetVector(CLUSTER_NOTIFY_FC_IRQn, (uint32_t)__handler_wrapper_CLUSTER_CL2FC_Handler);
    NVIC_EnableIRQ(CLUSTER_NOTIFY_FC_IRQn);
}

static void hyperbus_block_transfer(const int addr, const void *tx, size_t tx_length, void *rx, size_t rx_length) {
    hyperbus_transfer_t xfer;

    if (!hyperbus_is_init) {

        /* Set hyperbus pin conection - lowest software level */
        PORT_SetPinMux(PORTA, HYPERBUS_CLK  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_CLKN - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_RWDS - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_CSN0 - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_CSN1 - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ0  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ1  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ2  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ3  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ4  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ5  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ6  - GAP_PIN_OFFSET, uPORT_MuxAlt3);
        PORT_SetPinMux(PORTA, HYPERBUS_DQ7  - GAP_PIN_OFFSET, uPORT_MuxAlt3);

        hyperbus_config_t  config;

        HYPERBUS_GetDefaultConfig(&config);

        HYPERBUS_Init(HYPERBUS0, &config, SystemCoreClock);

        #if defined(__GAP8__)
        /* Config memory maximum transfer data length for TX and RX*/
        HYPERBUS_SetMaxLength(HYPERBUS0, 1, 0x1ff, 0, uHYPERBUS_Ram);
        HYPERBUS_SetMaxLength(HYPERBUS0, 1, 0x1ff, 1, uHYPERBUS_Ram);

        /* Config memory access timing for TX and RX*/
        HYPERBUS_SetTiming(HYPERBUS0, 4, 4, 4, 0, 0, uHYPERBUS_Ram);
        HYPERBUS_SetTiming(HYPERBUS0, 4, 4, 4, 0, 1, uHYPERBUS_Ram);
        #endif
    }

    /*Start transfer*/
    xfer.txData = (uint16_t *)tx;
    xfer.txDataSize = tx_length;
    xfer.rxData = (uint16_t *)rx;
    xfer.rxDataSize = rx_length;
    xfer.configFlags = 32;
    xfer.addr = addr;
    xfer.device = uHYPERBUS_Ram;
    xfer.reg_access = uHYPERBUS_Mem_Access;

    HYPERBUS_TransferBlocking(HYPERBUS0, &xfer);
}

void CLUSTER_FC_Delegate(){

    switch((uint32_t)fc_task.id) {

#if defined(__GAP8__)
    case UDMA_EVENT_UART_TX :
#elif defined(__GAP9__)
    case UDMA_EVENT_UART0_TX :
#endif
    {
        uart_putc((char)((uint32_t)fc_task.arg[0] & 0xFF));
        break;
    }
    case UDMA_EVENT_HYPERBUS_RX :
    {
        hyperbus_block_transfer(fc_task.arg[0], 0, 0, (void *)fc_task.arg[1], fc_task.arg[2]);
        break;
    }
    case UDMA_EVENT_HYPERBUS_TX :
    {
        hyperbus_block_transfer(fc_task.arg[0], (const void *)fc_task.arg[1], fc_task.arg[2], 0, 0);
        break;
    }
    default :
        break;
    }

    fc_task.id = 0;
    EU_CLUSTER_EVT_TrigSet(FC_NOTIFY_CLUSTER_EVENT, 0);
}

void CLUSTER_CL2FC_SendTask(uint32_t cid, fc_call_t *task)
{
    // First wait until the slot to post events is free
    while(fc_task.id != 0)
    {
        EU_EVT_MaskWaitAndClr(1 << FC_NOTIFY_CLUSTER_EVENT);
    }

    fc_task.id       = task->id;
    fc_task.arg[0]   = task->arg[0];
    fc_task.arg[1]   = task->arg[1];
    fc_task.arg[2]   = task->arg[2];
    fc_task.arg[3]   = task->arg[3];

    /* Cluster Notify FC to send a task back to FC by HW IRQ */
    #if defined(__GAP8__)
    EU_FC_EVT_TrigSet(CLUSTER_NOTIFY_FC_IRQn, 0);
    #elif defined(__GAP9__)
    ITC_SetIRQ(CLUSTER_NOTIFY_FC_IRQn);
    #endif
}


void CLUSTER_CL2FC_Handler() {
    EU_FC_EVT_TrigSet(CLUSTER_NOTIFY_FC_EVENT, 0);
}

#endif
