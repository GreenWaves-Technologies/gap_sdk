/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
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
        printf("pad config error\n");
        return 1;
    }
    rt_padframe_set(profile_hyper);
    printf ("pad config done\n");

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

/***********************
 * API for HyperFlash  *
 ***********************/
void hyper_flash_write(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)flash;
  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_hyperflash_copy(dev, CHANNEL_TX, (void*)l2Addr, (void *)hyperFlashAddr, 2, call_event);
  __rt_wait_event_check(event, call_event);
}

void hyper_flash_burst_write(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, unsigned int size, rt_event_t *event)
{
  rt_hyperflash_t *dev = (rt_hyperflash_t *)flash;
  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_hyperflash_copy(dev, CHANNEL_TX, (void *)l2Addr, (void *)hyperFlashAddr, size, call_event);
  __rt_wait_event_check(event, call_event);

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

    rt_hyperflash_copy(dev, CHANNEL_RX, &padding, (void *)(hyperFlashAddr+offset), rest_size, call_event);

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
      rt_hyperflash_copy(dev, CHANNEL_RX, (void *)l2Addr, (void *)hyperFlashAddr, offset, call_event);
      __rt_wait_event_check(event, call_event);
    }
  }
  else
  {
    // Read
    rt_event_t *call_event = __rt_wait_event_prepare(event);
    rt_hyperflash_copy(dev, CHANNEL_RX, (void *)l2Addr, (void *)hyperFlashAddr, size, call_event);
    __rt_wait_event_check(event, call_event);
  }
}

void hyper_close()
{
  isInit = 0;
  hal_hyper_stop_ip();
}

