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

#include "gap_udma.h"
#include "gap_hyperbus.h"
#include <assert.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef void (*func)();

/*! @brief uDMA request queue size */
#define request_queue_num      8

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief uDMA transfer initialization flag */
uint32_t udmaInit = 0;

/*! @brief uDMA transfer channels */
udma_channel_t udma_channels[UDMA_CHANNEL_NUM];

/*! @brief uDMA transfer request pool which control all request */
udma_req_t  udma_requests[request_queue_num];


/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Set channel base adddress.
 *
 */
static void UDMA_SetChannelBase() {
#if DEVICE_LVDS == 1
  udma_channels[0].base = (UDMA_Type *) LVDS;
#else
#if DEVICE_ORCA_ == 1
  udma_channels[0].base = (UDMA_Type *) ORCA;
#endif
#endif
  udma_channels[1].base = (UDMA_Type *) SPIM0;

  udma_channels[2].base = (UDMA_Type *) SPIM1;
  udma_channels[3].base = (UDMA_Type *) HYPERBUS0;
  udma_channels[4].base = (UDMA_Type *) UART;
  udma_channels[5].base = (UDMA_Type *) I2C0;
  udma_channels[6].base = (UDMA_Type *) I2C1;
  udma_channels[7].base = (UDMA_Type *) DMACPY;
  /* Special channel I2S1 use TX for RX */
  udma_channels[8].base = (UDMA_Type *) I2S;
  udma_channels[9].base = (UDMA_Type *) CPI;
}

static uint32_t UDMA_GetInstance(UDMA_Type *base)
{
  uint32_t instance;
  for (instance = 0; instance < UDMA_CHANNEL_NUM; instance++)
  {
    if (udma_channels[instance].base == base)
    {
      break;
    }
  }

  assert(instance < UDMA_CHANNEL_NUM);

  return instance;
}

udma_req_t* UDMA_FindAvailableRequest()
{
    while(1) {
        for (uint32_t i = 0; i < request_queue_num; i++)
        {
            /* Request available */
            if (udma_requests[i].pending == UDMA_REQ_FREE)
                return &udma_requests[i];
        }
    }
}

void UDMA_Init(UDMA_Type *base)
{
    if(udmaInit == 0) {
      /* Initialize every channel queue */
      for (uint32_t instance = 0; instance < UDMA_CHANNEL_NUM; instance++)
      {
          udma_channel_t *channel = &udma_channels[instance];
          channel->first = (void *)0;
          channel->last  = (void *)0;
          channel->previous  = (void *)0;
          channel->rx_depth = 0;
          channel->tx_depth = 0;
      }
      /* Biding channels to individual peripheral */
      UDMA_SetChannelBase();

      /* Finish Initialization  */
      udmaInit++;
    }

    /* Clock gating enable */
    uint32_t index = UDMA_GetInstance(base);
    UDMA_GC->CG |= (1 << index);

    /* Attach event unit for end interrupt */
    if(index == 9) {
        SOC_EU_SetFCMask(UDMA_EVENT_CPI_RX);
    } else {
        SOC_EU_SetFCMask(index << 1);
        SOC_EU_SetFCMask((index << 1) + 1);
    }
}


void UDMA_Deinit(UDMA_Type *base)
{
  uint32_t index = UDMA_GetInstance(base);

  /* Clock gating disable */
  UDMA_GC->CG &= (~(1 << index));

  /*  Detach event unit */
  if(index == 9) {
      SOC_EU_ClearFCMask(UDMA_EVENT_CPI_RX);
  } else {
      SOC_EU_ClearFCMask(index << 1);
      SOC_EU_ClearFCMask((index << 1) + 1);
  }

  if (udmaInit > 0)
    udmaInit--;
}

void UDMA_BlockWait(int32_t index)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    int event = 0;
    int32_t cur_event = 0;
    do
    {
        do
        {
            event = EU_EVT_MaskWaitAndClr(1 << FC_SOC_EVENT_IRQn);
        } while (!(event & (1 << FC_SOC_EVENT_IRQn)));

        /* Pop an event. */
        cur_event = (EU_SOC_EVENTS->CURRENT_EVENT & 0xFF);

        /* Now that we popped the element, we can clear the soc event FIFO event as the FIFO is
           generating an event as soon as the FIFO is not empty */
        EU_CORE_DEMUX->BUFFER_CLEAR = (1 << FC_SOC_EVENT_IRQn);

        /* When there is asynchronous transfer during block transfer, execute the callback */
        if ((cur_event >> 1) != index)
            UDMA_EventHandler(cur_event, 0);
    } while ((cur_event >> 1) != index);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

status_t UDMA_BlockTransfer(UDMA_Type *base, udma_req_info_t *info, UDMAHint hint)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    if (info->isTx) {
        assert(!UDMA_TXBusy(base));
    } else {
        assert(!UDMA_RXBusy(base));
    }

    /*
     * Could not use FC TCDM momery (0x1B000000) as the buffer address for uDMA, please see the detail :
     * https://greenwaves-technologies.com/manuals/BUILD/MBED-OS/html/index.html
     */
    assert(info->dataAddr >= L2_BASE);

    if (info->isTx) {
        base->TX_SADDR = info->dataAddr;
        base->TX_SIZE  = info->dataSize;
        base->TX_CFG   = info->configFlags;
    } else {
        base->RX_SADDR = info->dataAddr;
        base->RX_SIZE  = info->dataSize;
        base->RX_CFG   = info->configFlags;
    }

    uint32_t index = UDMA_GetInstance(base);

    if (hint == UDMA_WAIT) {
        /* Wait TX/RX finished */
        UDMA_BlockWait(index);
    } else if (hint == UDMA_WAIT_RX) {
        /* Wait TX finished */
        UDMA_BlockWait(index);

        /* Wait util previous RX is finished */
        UDMA_BlockWait(index);
    }

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);

    return uStatus_Success;
}

static void UDMA_StartTransfer(UDMA_Type *base, udma_req_info_t *info) {
    /* Send enqueue request to the FIFO. */
    /* Hyperbus ctrl */
    if((info->ctrl & 0x0F) == UDMA_CTRL_HYPERBUS) {
        HYPERBUS_Type *hyperbus_ptr = (HYPERBUS_Type *) base;

        uint32_t ext_addr = info->u.hyperbus.ext_addr;
        uint32_t reg_mem_access = info->u.hyperbus.reg_mem_access;

        hyperbus_ptr->EXT_ADDR = ext_addr;

        /* If RAM register access */
        #if (__HYPERBUS_CSN0_FOR_RAM__ == 1)
        if (ext_addr < uHYPERBUS_Flash_Address) {
            /* hyperbus_crt0_set */
            HYPERBUS_SetCRT0(reg_mem_access);
        }
        #else
        if (ext_addr >= uHYPERBUS_Ram_Address) {
            /* hyperbus_crt1_set */
            HYPERBUS_SetCRT1(reg_mem_access);
        }
        #endif
    }
    else if(info->ctrl == 3) {
        /* For other special IP */
    }

    /*
     * Could not use FC TCDM momery (0x1B000000) as the buffer address for uDMA, please see the detail :
     * https://greenwaves-technologies.com/manuals/BUILD/MBED-OS/html/index.html
     */
    assert(info->dataAddr >= L2_BASE);

    if (info->isTx) {
        base->TX_SADDR = info->dataAddr;
        base->TX_SIZE  = info->dataSize;
        base->TX_CFG   = info->configFlags;
    } else {
        base->RX_SADDR = info->dataAddr;
        base->RX_SIZE  = info->dataSize;
        base->RX_CFG   = info->configFlags;
    }
}

/*!
 * @brief Deal with the transfer request in uDMA.
 *
 *
 * @param base The UDMA channel base pointer.
 * @param req  The request.
 * @return status of status_t.
 * @note .
 */
static status_t UDMA_EnqueueRequest(UDMA_Type *base, udma_req_t *req)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    /* Special calculation for I2S and CPI -- TODO */
    udma_channel_t *channel = &udma_channels[(req->info.channelId >> 1)];

    req->channel   = channel;
    /* Start channel */
    req->info.configFlags |= UDMA_CFG_EN(1);

    req->pending = UDMA_REQ_IN_SW_QUEUE;

    /* Create a request queue - coorperation with FC event handler */
    if (channel->first == (void *)0) {
      channel->first = req;
      channel->last = req;
      channel->first->next = (void *)0;
    } else {
      channel->last->next = req;
      channel->last = req;
      channel->last->next = (void *)0;
    }

    if (req->info.ctrl != UDMA_CTRL_DUAL_TX)
        channel->previous = req;

    /* Enqueue maximum depth is 2 for each channel */
    if(req->info.isTx && channel->tx_depth < 2) {
        channel->tx_depth++;

        /* HyperbusBus special control, if the request can not enqueue
         * it will be in the queue but will not be sent be FIFO
         */
        if (req->info.ctrl != UDMA_CTRL_HYPERBUS_CAN_NOT_ENQUEUE)
        {
            UDMA_StartTransfer(base, &req->info);
            req->pending = UDMA_REQ_IN_HW_QUEUE;
        }
    } else if(!req->info.isTx && channel->rx_depth < 2) {
        channel->rx_depth++;

        /* HyperbusBus special control, if the request can not enqueue
         * it will be in the queue but will not be sent be FIFO
         */
        if (req->info.ctrl != UDMA_CTRL_HYPERBUS_CAN_NOT_ENQUEUE)
        {
            UDMA_StartTransfer(base, &req->info);
            req->pending = UDMA_REQ_IN_HW_QUEUE;
        }
    }

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);

    return 1;
}

status_t UDMA_SendRequest(UDMA_Type *base, udma_req_t *req)
{
    int32_t status;

    /* Get peripheral channel according to channelID */
    udma_channel_t *channel = &udma_channels[(req->info.channelId >> 1)];

    /* HyperbusBus special control to distinguish 4-byte access (data_size = 4N)
     * and Two bytes (data_size = 2 only) request
     * If first request is 4N bytes request, then 2 bytes request can not enqueue
     * it needs to wait
     */
    if (req->info.ctrl == UDMA_CTRL_HYPERBUS && channel->previous) {
        if ((req->info.dataSize & 0x3) || (channel->previous->info.dataSize & 0x3))
            req->info.ctrl = UDMA_CTRL_HYPERBUS_CAN_NOT_ENQUEUE;
        else
            req->info.ctrl = UDMA_CTRL_HYPERBUS_CAN_ENQUEUE;
    }

    /* Enqueue request and send it to FIFO */
    status = UDMA_EnqueueRequest((UDMA_Type *)base, req);

    return status;
}

/*!
 * @brief udma channel repeat.
 *
 * This function repeat a request transfer.
 *
 * @param req udma transfer request.
 * @note .
 */
static inline void UDMA_RepeatTransfer(udma_req_t *req) {

    if((req->info.channelId & 0xFE) == UDMA_EVENT_HYPERBUS_RX) {
        HYPERBUS_Type *hyperbus_ptr = (HYPERBUS_Type *) req->channel->base;
        hyperbus_ptr->EXT_ADDR = req->info.u.hyperbus.ext_addr + req->info.repeat.size;
    }

    UDMA_Type *base = (UDMA_Type *) req->channel->base;

    assert((uint32_t)req->info.repeat.size == req->info.dataSize);

    req->info.dataAddr          += req->info.repeat.size;
    req->info.repeat.size_total -= req->info.repeat.size;

    if(req->info.repeat.size_total <= 0)
        return;

    if (req->info.repeat.size >= req->info.repeat.size_total)
    {
        req->info.dataSize = req->info.repeat.size_total;
        req->info.repeat.size = 0;
    }
    /* Start udma transfer */
    UDMA_StartTransfer(base, &req->info);
}

extern void SAI_IRQHandler_CH0(void *handle);
extern void SAI_IRQHandler_CH1(void *handle);
extern void DMACPY_IRQHandler(void *handle);

__attribute__((section(".text")))
void UDMA_EventHandler(uint32_t index, int abort)
{
    /*
     * Asynchronous transfer control
     * The main idea is to check each request's pending flag.
     */
    udma_channel_t *channel = &udma_channels[index >> 1];
    udma_req_t *first = channel->first;

    /* If abort from a synchronous transfer, just return */
    if (abort || !channel->first)
        return;

    if (first->info.repeat.size) {
        UDMA_RepeatTransfer(first);
        return;
    } else {
        /* Clean pending flag, if it is a dual request.
         * For the first read request, we do not clear it.
         * For the second write request, we clear the two.
         */
        if (first->info.ctrl != UDMA_CTRL_DUAL_RX) {
            first->pending = UDMA_REQ_FREE;

            if (first->info.ctrl == UDMA_CTRL_DUAL_TX)
                channel->previous->pending = UDMA_REQ_FREE;
        }

        /* Channel enqueue depth release one */
        if(first->info.isTx)
            channel->tx_depth--;
        else
            channel->rx_depth--;

        udma_req_t *next = channel->first->next;

        /* Check queue next request
         */
        if (next == (void *)0) {
            /* For hyperbus chanel. If there is no valid request. Initializ the queue */
            if ((index & 0xFE) == UDMA_EVENT_HYPERBUS_RX)
                channel->previous = (void *)0;

            channel->first = (void *)0;
            channel->last  = (void *)0;

        } else {
            /* Clean request next pointer */
            channel->first->next = (void *)0;

            /* First request points to the next request  */
            channel->first = next;

            while(next) {
                if(next->pending == UDMA_REQ_IN_SW_QUEUE) {
                    /* Start request in software queue*/
                    if(next->info.isTx)
                        channel->tx_depth++;
                    else
                        channel->rx_depth++;

                    UDMA_StartTransfer(next->channel->base, &next->info);
                    next->pending = UDMA_REQ_IN_HW_QUEUE;
                    break;
                }
                next = next->next;
            }
        }
    }

    if(!abort) {
        /* Check Task or call back function */
        if (first->info.ctrl != UDMA_CTRL_DUAL_RX) {
            if (first->info.task)
            {
                if(index == UDMA_EVENT_UART_TX || index == UDMA_EVENT_UART_RX)
                    asm volatile ("jal UART_DriverIRQHandler");
                if(index == UDMA_EVENT_SPIM0_TX || index == UDMA_EVENT_SPIM0_RX)
                    asm volatile ("jal SPI0_DriverIRQHandler");
                if(index == UDMA_EVENT_SPIM1_TX || index == UDMA_EVENT_SPIM1_RX)
                    asm volatile ("jal SPI1_DriverIRQHandler");
                if(index == UDMA_EVENT_HYPERBUS_TX || index == UDMA_EVENT_HYPERBUS_RX)
                    asm volatile ("jal HYPERBUS0_DriverIRQHandler");
                if(index == UDMA_EVENT_CPI_RX)
                    asm volatile ("jal CPI_DriverIRQHandler");
                if(index == UDMA_EVENT_SAI_CH0)
                    SAI_IRQHandler_CH0((void *)first->info.task);
                if(index == UDMA_EVENT_SAI_CH1)
                    SAI_IRQHandler_CH1((void *)first->info.task);
                if(index == UDMA_EVENT_DMACPY_RX || index == UDMA_EVENT_DMACPY_TX)
                    DMACPY_IRQHandler((void *)first->info.task);
            }
        }
    }
}

status_t UDMA_AbortSend(UDMA_Type *base) {

    /* Clear TX transfer */
    base->TX_CFG = UDMA_CFG_CLR(1);

    /* Get abort channel */
    uint32_t index = UDMA_GetInstance(base);

    /* Release request pool */
    UDMA_EventHandler((index << 1) + 1, 1);

    return uStatus_Success;
}

status_t UDMA_AbortReceive(UDMA_Type *base) {

    /* Clear RX transfer */
    base->RX_CFG = UDMA_CFG_CLR(1);

    /* Get abort channel */
    uint32_t index = UDMA_GetInstance(base);

    /* Release request pool */
    UDMA_EventHandler((index << 1), 1);

    return uStatus_Success;
}
