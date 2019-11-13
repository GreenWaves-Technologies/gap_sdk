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
#include "FreeRTOSConfig.h"

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
  udma_channels[1].base = NULL;//(UDMA_Type *) SPIM0;
  udma_channels[2].base = NULL;//(UDMA_Type *) SPIM1;
  udma_channels[3].base = NULL;//(UDMA_Type *) HYPERBUS0;
  udma_channels[4].base = NULL;//(UDMA_Type *) UART;
  udma_channels[5].base = NULL;//(UDMA_Type *) I2C0;
  udma_channels[6].base = NULL;//(UDMA_Type *) I2C1;
  udma_channels[7].base = (UDMA_Type *) DMACPY;
  /* Special channel I2S1 use TX for RX */
  udma_channels[8].base = (UDMA_Type *) I2S;
  udma_channels[9].base = NULL;//(UDMA_Type *) CPI;
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

  configASSERT(instance < UDMA_CHANNEL_NUM);

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

void UDMA_BlockWait()
{
    /* Disable IRQ */
    int irq_en = NVIC_GetEnableIRQ(FC_SOC_EVENT_IRQN);
    NVIC_DisableIRQ(FC_SOC_EVENT_IRQN);

    int event = 0;
    do {
        event = EU_EVT_MaskWaitAndClr(1 << FC_SOC_EVENT_IRQN);
    } while (!(event & (1 << FC_SOC_EVENT_IRQN)));

    /* Pop a event. */
    EU_SOC_EVENTS->CURRENT_EVENT;

    /* Now that we popped the element, we can clear the soc event FIFO event as the FIFO is
       generating an event as soon as the FIFO is not empty */
    EU_CORE_DEMUX->BUFFER_CLEAR = (1 << FC_SOC_EVENT_IRQN);

    /* Restore IRQ */
    if (irq_en)
        NVIC_EnableIRQ(FC_SOC_EVENT_IRQN);
}

status_t UDMA_BlockTransfer(UDMA_Type *base, udma_req_info_t *info, UDMAHint hint)
{
    /* Disable IRQ */
    int irq_en = NVIC_GetEnableIRQ(FC_SOC_EVENT_IRQN);
    NVIC_DisableIRQ(FC_SOC_EVENT_IRQN);

    if (info->isTx) {
        configASSERT(!UDMA_TXBusy(base));
    } else {
        configASSERT(!UDMA_RXBusy(base));
    }

    if (info->isTx) {
        base->TX_SADDR = info->dataAddr;
        base->TX_SIZE  = info->dataSize;
        base->TX_CFG   = info->configFlags;
    } else {
        base->RX_SADDR = info->dataAddr;
        base->RX_SIZE  = info->dataSize;
        base->RX_CFG   = info->configFlags;
    }


    if (hint == UDMA_WAIT) {
        /* Wait TX/RX finished */
        UDMA_BlockWait();
    } else if (hint == UDMA_WAIT_RX) {
        /* Wait TX finished */
        UDMA_BlockWait();

        /* Wait util previous RX is finished */
        UDMA_BlockWait();
    }

    /* Restore IRQ */
    if (irq_en)
        NVIC_EnableIRQ(FC_SOC_EVENT_IRQN);

    return uStatus_Success;
}

static void UDMA_StartTransfer(UDMA_Type *base, udma_req_info_t *info) {
    /* Send enqueue request to the FIFO. */
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
    /* Disable IRQ */
    int irq_en = NVIC_GetEnableIRQ(FC_SOC_EVENT_IRQN);
    NVIC_DisableIRQ(FC_SOC_EVENT_IRQN);

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

    } else if(!req->info.isTx && channel->rx_depth < 2) {
        channel->rx_depth++;

    }

    /* Restore IRQ */
    if (irq_en)
        NVIC_EnableIRQ(FC_SOC_EVENT_IRQN);

    return 1;
}

status_t UDMA_SendRequest(UDMA_Type *base, udma_req_t *req)
{
    int32_t status;

    /* Get peripheral channel according to channelID */
    udma_channel_t *channel = &udma_channels[(req->info.channelId >> 1)];

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

    UDMA_Type *base = (UDMA_Type *) req->channel->base;

    configASSERT((uint32_t)req->info.repeat.size == req->info.dataSize);

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

extern void I2C_IRQHandler(void *handle);
extern void SAI_IRQHandler(void *handle);
extern void DMACPY_IRQHandler(void *handle);

__attribute__((section(".text")))
void UDMA_EventHandler(uint32_t index, int abort)
{

      // ----------------------------------------
     // --- ALL THIS PART IS PROBABLY USELESS --- 
    // ------ TODO FIXME REMOVE and check -------  
   // ============================================= 

    /*
     * Asynchronous transfer control
     * The main idea is to check each request's pending flag.
     */
    udma_channel_t *channel = &udma_channels[index >> 1];
    udma_req_t *first = channel->first;

    /* If abort from a synchronous transfer, just return */
    if (abort && !channel->first)
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
        //if (first->info.ctrl != UDMA_CTRL_DUAL_RX) {
            if (first->info.task)
            {
                if(index == UDMA_EVENT_UART_TX || index == UDMA_EVENT_UART_RX)
                    //asm volatile ("jal UART_DriverIRQHandler");
                if(index == UDMA_EVENT_SPIM0_TX || index == UDMA_EVENT_SPIM0_RX)
                    asm volatile ("jal SPI0_DriverIRQHandler");
                if(index == UDMA_EVENT_SPIM1_TX || index == UDMA_EVENT_SPIM1_RX)
                    asm volatile ("jal SPI1_DriverIRQHandler");
                if(index == UDMA_EVENT_CPI_RX)
                    asm volatile ("jal CPI_DriverIRQHandler");
                if(index == UDMA_EVENT_SAI_CH0 || index == UDMA_EVENT_SAI_CH1)
                    SAI_IRQHandler((void *)first->info.task);
                if(UDMA_EVENT_I2C0_RX <= index && index <= UDMA_EVENT_I2C1_TX)
                    I2C_IRQHandler((void *)first->info.task);
                if(index == UDMA_EVENT_DMACPY_RX || index == UDMA_EVENT_DMACPY_TX)
                    DMACPY_IRQHandler((void *)first->info.task);
            }
            //}
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
