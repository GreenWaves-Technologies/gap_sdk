
/*
 * Copyright (c) 2017 GreenWaves Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of GreenWaves Technologies SAS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "rt/rt_api.h"
#include "hyper_flash.h"

RT_FC_DATA static int isInit = 0;
RT_L2_DATA unsigned int padding;

int hyper_init()
{
  if (!isInit) {
    isInit = 1;

    rt_padframe_profile_t *profile_hyper = rt_pad_profile_get("hyper");
    if (profile_hyper == NULL) {
        printf("[Flasher]: pad config error\n");
        return 1;
    }
    rt_padframe_set(profile_hyper);
    printf ("[Flasher]: pad config done\n");

    hal_hyper_start_ip();

    // Set memory base address with RAM = 16M
    hal_hyper_udma_mbr0_set(REG_MBR0);
    hal_hyper_udma_mbr1_set(REG_MBR1>>24);

    // HyperRAM
    hal_hyper_udma_dt0_set(1);
    // HyperFlash
    hal_hyper_udma_dt1_set(0);

    // When using flash, this bit should set to 0, always memory access
    hal_hyper_udma_crt1_set(MEM_ACCESS);

    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_HYPER_TX(0));
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_HYPER_RX(0));
  }

  return 0;
}

static void __rt_hyperflash_copy(int channel, void *addr, void *hyper_addr, int size, rt_event_t *event, int mbr)
{
    int irq = rt_irq_disable();

    rt_event_t *call_event = __rt_wait_event_prepare(event);
    rt_periph_copy_t *copy = &call_event->copy;

    copy->ctrl = RT_PERIPH_COPY_HYPER << RT_PERIPH_COPY_CTRL_TYPE_BIT;
    copy->u.hyper.hyper_addr = mbr | (unsigned int)hyper_addr;
    // In case the size is bigger than the maximum burst size
    // split the transfer into smaller transfers using the repeat count
    if (size > 512) {
        copy->addr = (unsigned int)addr;
        copy->repeat = 512;
        copy->repeat_size = size;
        size = 512;

    } else {
        copy->repeat = 0;

    }

    rt_periph_copy(copy, channel, (unsigned int)addr, size, UDMA_CHANNEL_CFG_SIZE_16, call_event);

    __rt_wait_event_check(event, call_event);

    rt_irq_restore(irq);
}


/***********************
 * API for HyperFlash  *
 ***********************/
void hyper_flash_write(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)flash;
  __rt_hyperflash_copy(UDMA_CHANNEL_ID(dev->channel) + CHANNEL_TX, (void*) l2Addr, (void *) hyperFlashAddr, 2, event, REG_MBR1);
}

void hyper_flash_burst_write(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, unsigned int size, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)flash;
  __rt_hyperflash_copy(UDMA_CHANNEL_ID(dev->channel) + CHANNEL_TX, (void*) l2Addr, (void *) hyperFlashAddr, size, event, REG_MBR1);
}

void hyper_flash_read(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, unsigned int size, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)flash;
  // Flash read operation always 5 data latency cycles
  unsigned int rest = size % 4;
  unsigned int rest_size = 4;
  unsigned int offset  = size - rest;

  if(rest)
  {
    if(rest > 2)
      rest_size = 4;
    else
      rest_size = 2;

    rt_event_t *call_event = __rt_wait_event_prepare(event);

    __rt_hyperflash_copy(UDMA_CHANNEL_ID(dev->channel) + CHANNEL_RX, &padding, (void *)(hyperFlashAddr+offset), rest_size, call_event, REG_MBR1);

    __rt_wait_event_check(event, call_event);

    if (rest == 1)
    {
      *((unsigned char*)(l2Addr + offset)) = (unsigned char) ((padding) >> 16 & 0xFF);
    }
    else if (rest == 2)
    {
      *((unsigned short*)(l2Addr+ offset)) = (unsigned short) ((padding >> 16) & 0xFFFF);
    }
    else
    {
      *((unsigned short*)(l2Addr+ offset)) = (unsigned short) (padding & 0xFFFF);
      *((unsigned char*)(l2Addr + offset + 2)) = (unsigned char) ((padding >> 16) & 0xFF);
    }

    if(size > 4)
    {
      // Read
      call_event = __rt_wait_event_prepare(event);
      __rt_hyperflash_copy(UDMA_CHANNEL_ID(dev->channel) + CHANNEL_RX, (void *) l2Addr, (void *) hyperFlashAddr, offset, call_event, REG_MBR1);
      __rt_wait_event_check(event, call_event);
    }
  }
  else
  {
    // Read
    rt_event_t *call_event = __rt_wait_event_prepare(event);
    __rt_hyperflash_copy(UDMA_CHANNEL_ID(dev->channel) + CHANNEL_RX, (void *) l2Addr, (void *) hyperFlashAddr, size, call_event, REG_MBR1);
    __rt_wait_event_check(event, call_event);
  }
}

void hyper_close()
{
  isInit = 0;
  hal_hyper_stop_ip();
}

