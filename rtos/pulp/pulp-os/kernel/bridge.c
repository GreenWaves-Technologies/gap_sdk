/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "hal/debug_bridge/debug_bridge.h"

static RT_FC_TINY_DATA rt_eeprom_t *__rt_bridge_eeprom_handle;
static RT_FC_TINY_DATA rt_flash_t *__rt_bridge_flash_handle;
static RT_FC_TINY_DATA int __rt_bridge_flash_type;
static RT_FC_TINY_DATA unsigned int __rt_bridge_flash_itf;
static RT_FC_TINY_DATA unsigned int __rt_bridge_flash_cs;


static int __rt_bridge_strlen(const char *str)
{
  int result = 0;
  while (*str)
  {
    result++;
    str++;
  }
  return result;
}



// This function should be called everytime the bridge may have new requests
// to handle, so that its pointer to ready requests is updated
static void __rt_bridge_check_bridge_req()
{
  hal_bridge_t *bridge = hal_bridge_get();

  // first_bridge_req is owned by the runtime only when it is NULL, otherwise
  // it is owned by the bridge which is modifying it everytime a request 
  // is handled
  if (bridge->first_bridge_req == 0)
  {
    // We are owning the pointer, check if we can make it point to ready
    // requests
    rt_bridge_req_t *req = (rt_bridge_req_t *)bridge->first_req;

    while (req && req->header.popped)
    {
      req = (rt_bridge_req_t *)req->header.next;
    }

    if (req)
    {
      bridge->first_bridge_req = (uint32_t )req;
    }
  }
}


static void __rt_bridge_post_common(rt_bridge_req_t *req, rt_event_t *event)
{
  hal_bridge_t *bridge = hal_bridge_get();

  req->header.next = 0;
  req->header.popped = 0;

  req->header.size = sizeof(rt_bridge_req_t);
  req->event = event;

  if (bridge->first_req)
    ((hal_bridge_req_t *)bridge->last_req)->next = (uint32_t)req;
  else
    bridge->first_req = (uint32_t)req;

  bridge->last_req = (uint32_t)req;
  req->header.next = 0;

  __rt_bridge_send_notif();

  __rt_bridge_check_bridge_req();
}


// This function can be called by the API layer to commit a new request to 
// the bridge
static void __rt_bridge_post_req(rt_bridge_req_t *req, rt_event_t *event)
{
  req->header.done = 0;
  req->header.bridge_to_target = 0;
  __rt_bridge_post_common(req, event);
}

// This function can be called by the API layer to commit a new request to 
// the bridge
static void __rt_bridge_post_reply(rt_bridge_req_t *req, rt_event_t *event)
{
  req->header.done = 1;
  req->header.bridge_to_target = 1;
  __rt_bridge_post_common(req, event);
}

static void __rt_bridge_efuse_access(int is_write, int index, unsigned int value, unsigned int mask)
{
  //printf("Writing efuse (index: %d, value: 0x%x, mask: 0x%x)\n", index, value, mask);
#ifdef EFUSE_VERSION
  plp_efuse_configTimings (250 << 20 | 50 << 10 | 5);
  plp_efuse_startProgram ();
  plp_efuse_writeByte(index, value & mask);
  plp_efuse_sleep();
#endif
}

static int __rt_bridge_eeprom_access(unsigned int itf, unsigned int cs, int is_write, unsigned int addr, unsigned int buffer, int size)
{
#ifdef ARCHI_UDMA_HAS_I2C
  //printf("Eeprom access (is_write: %d, addr: 0x%x, buffer: 0x%x, size: 0x%x)\n", is_write, addr, buffer, size);

  if (__rt_bridge_eeprom_handle == NULL)
  {
    rt_eeprom_conf_t conf;
    conf.cs = cs;
    conf.id = itf;
    __rt_bridge_eeprom_handle = rt_eeprom_open(NULL, &conf, NULL);
    if (__rt_bridge_eeprom_handle == NULL)
      return -1;
  }

  if (is_write)
    rt_eeprom_write(__rt_bridge_eeprom_handle, addr, (uint8_t *)buffer, size, NULL);
  else
    rt_eeprom_read(__rt_bridge_eeprom_handle, addr, (uint8_t *)buffer, size, NULL);
#endif

  return 0;
}

static int __rt_bridge_check_flash_open(int type, unsigned int itf, unsigned int cs)
{
  if (__rt_bridge_flash_handle == NULL || __rt_bridge_flash_type != type || __rt_bridge_flash_itf != itf || __rt_bridge_flash_cs != cs)
  {
    if (__rt_bridge_flash_handle != NULL)
    {
      rt_flash_close(__rt_bridge_flash_handle, NULL);
    }

    rt_flash_conf_t conf;

    rt_flash_conf_init(&conf);

    conf.id = itf;
    conf.type = type;
    __rt_bridge_flash_type = type;
    __rt_bridge_flash_itf = itf;
    __rt_bridge_flash_cs = cs;

    if (type == RT_FLASH_TYPE_HYPER)
    {
#ifdef PADS_VERSION
      rt_padframe_profile_t *profile_hyper = rt_pad_profile_get("hyper");
      if (profile_hyper == NULL) {
          return 1;
      }
      rt_padframe_set(profile_hyper);
#endif
    }

    __rt_bridge_flash_handle = rt_flash_open(NULL, &conf, NULL);

    if (__rt_bridge_flash_handle == NULL)
      return -1;
  }

  return 0;
}

static int __rt_bridge_flash_access(int type, unsigned int itf, unsigned int cs, int is_write, unsigned int addr, unsigned int buffer, int size)
{
  //printf("Flash access (type: %d, itf: %d, cs: %d, is_write: %d, addr: 0x%x, buffer: 0x%x, size: 0x%x)\n", type, itf, cs, is_write, addr, buffer, size);

  if (__rt_bridge_check_flash_open(type, itf, cs))
    return -1;

  if (is_write)
    __rt_flash_program(__rt_bridge_flash_handle, (uint8_t *)buffer, (void *)addr, size, NULL);
  else
  {
    rt_flash_read(__rt_bridge_flash_handle, (uint8_t *)buffer, (void *)addr, size, NULL);
  }

  return 0;
}



static int __rt_bridge_flash_erase(int type, unsigned int itf, unsigned int cs, void *data, int size)
{
  //printf("Flash erase (type: %d, itf: %d, cs: %d, addr: %p, size: 0x%x)\n", type, itf, cs, data, size);

  if (__rt_bridge_check_flash_open(type, itf, cs))
    return -1;

  __rt_flash_erase(__rt_bridge_flash_handle, data, size, NULL);

  return 0;
}



static int __rt_bridge_flash_erase_chip(int type, unsigned int itf, unsigned int cs)
{
  //printf("Flash erase chip (type: %d, itf: %d, cs: %d)\n", type, itf, cs);

  if (__rt_bridge_check_flash_open(type, itf, cs))
    return -1;

  __rt_flash_erase_chip(__rt_bridge_flash_handle, NULL);

  return 0;
}



static int __rt_bridge_flash_erase_sector(int type, unsigned int itf, unsigned int cs, void *data)
{
  //printf("Flash erase sector (type: %d, itf: %d, cs: %d, addr: %p)\n", type, itf, cs, data);

  if (__rt_bridge_check_flash_open(type, itf, cs))
    return -1;

  __rt_flash_erase_sector(__rt_bridge_flash_handle, data, NULL);

  return 0;
}

static void __rt_bridge_handle_req(void *arg)
{
  rt_event_t *event = (rt_event_t *)arg;
  rt_bridge_req_t *req = &event->implem.bridge_req;

  if (req->header.type == HAL_BRIDGE_TARGET_REQ_EFUSE_ACCESS)
  {
    __rt_bridge_efuse_access(req->header.efuse_access.is_write, req->header.efuse_access.index, req->header.efuse_access.value, req->header.efuse_access.mask);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_BUFFER_ALLOC)
  {
    req->header.buffer_alloc.buffer = (unsigned int)rt_alloc(RT_ALLOC_PERIPH, req->header.buffer_alloc.size);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_BUFFER_FREE)
  {
    rt_free(RT_ALLOC_PERIPH, (void *)req->header.buffer_free.buffer, req->header.buffer_free.size);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_EEPROM_ACCESS)
  {
    req->header.eeprom_access.retval = __rt_bridge_eeprom_access(req->header.eeprom_access.itf, req->header.eeprom_access.cs, req->header.eeprom_access.is_write, req->header.eeprom_access.addr, req->header.eeprom_access.buffer, req->header.eeprom_access.size);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_FLASH_ACCESS)
  {
    req->header.flash_access.retval = __rt_bridge_flash_access(req->header.flash_access.type, req->header.flash_access.itf, req->header.flash_access.cs, req->header.flash_access.is_write, req->header.flash_access.addr, req->header.flash_access.buffer, req->header.flash_access.size);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_FLASH_ERASE_CHIP)
  {
    req->header.flash_erase_chip.retval = __rt_bridge_flash_erase_chip(req->header.flash_erase_chip.type, req->header.flash_erase_chip.itf, req->header.flash_erase_chip.cs);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_FLASH_ERASE_SECTOR)
  {
    req->header.flash_erase_sector.retval = __rt_bridge_flash_erase_sector(req->header.flash_erase_sector.type, req->header.flash_erase_sector.itf, req->header.flash_erase_sector.cs, (void *)req->header.flash_erase_sector.addr);
  }
  else if (req->header.type == HAL_BRIDGE_TARGET_REQ_FLASH_ERASE)
  {
    req->header.flash_erase_sector.retval = __rt_bridge_flash_erase(req->header.flash_erase.type, req->header.flash_erase.itf, req->header.flash_erase.cs, (void *)req->header.flash_erase.addr, req->header.flash_erase.size);
  }


  hal_bridge_reply(&req->header);
  __rt_bridge_post_reply(req, event);
}


int rt_bridge_connect(int wait_bridge, rt_event_t *event)
{
  hal_bridge_t *bridge = hal_bridge_get();

  if (!wait_bridge && !bridge->bridge.connected)
    return -1;

  int irq = rt_irq_disable();

  if (rt_event_alloc(NULL, 2))
    goto error;

  bridge->first_bridge_free_req = 0;

  // We allocated 2 events for bridge to target requests as he will only do one
  // at the same time but he could  try to allocate a new one while we are
  // releasing the last one
  for (int i=0; i<2; i++)
  {
    rt_event_t *bridge_req_event = rt_event_get(NULL, __rt_bridge_handle_req, NULL);
    __rt_event_set_keep(bridge_req_event);
    bridge_req_event->arg[1] = (intptr_t)bridge_req_event;
    rt_bridge_req_t *bridge_req = &bridge_req_event->implem.bridge_req;
    bridge_req->event = bridge_req_event;

    bridge_req->header.next = bridge->first_bridge_free_req;
    bridge->first_bridge_free_req = (uint32_t)bridge_req;
  }

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_connect(&req->header);
  bridge->target.connected = 1;
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
  return 0;

error:
  rt_irq_restore(irq);
  return -1;
}



void rt_bridge_disconnect(rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_disconnect(&req->header);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



int rt_bridge_open(const char* name, int flags, int mode, rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_open(&req->header, __rt_bridge_strlen(name), name, flags, mode);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  int retval = req->header.open.retval;

  rt_irq_restore(irq);

  return retval;
}



int rt_bridge_open_wait(rt_event_t *event)
{
  int irq = rt_irq_disable();
  rt_bridge_req_t *req = &event->implem.bridge_req;

  __rt_wait_event(event);
  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  int retval = req->header.open.retval;

  rt_irq_restore(irq);

  return retval;
}



int rt_bridge_close(int file, rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_close(&req->header, file);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.close.retval;    

  rt_irq_restore(irq);

  return retval;
}



int rt_bridge_close_wait(rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_bridge_req_t *req = &event->implem.bridge_req;
  __rt_wait_event(event);
  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.close.retval;    

  rt_irq_restore(irq);

  return retval;
}



int rt_bridge_read(int file, void* ptr, int len, rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_read(&req->header, file, ptr, len);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.read.retval;    

  rt_irq_restore(irq);

  return retval;
}

int rt_bridge_read_wait(rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_bridge_req_t *req = &event->implem.bridge_req;
  __rt_wait_event(event);
  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.read.retval;    

  rt_irq_restore(irq);

  return retval;
}



int rt_bridge_write(int file, void* ptr, int len, rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_write(&req->header, file, ptr, len);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.write.retval;    

  rt_irq_restore(irq);

  return retval;
}



uint64_t rt_bridge_fb_open(const char* name, int width, int height, rt_fb_format_e format, rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_fb_open(&req->header, __rt_bridge_strlen(name), name, width, height, format);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.fb_open.screen;    

  rt_irq_restore(irq);

  return retval;
}


uint64_t rt_bridge_fb_open_wait(rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_bridge_req_t *req = &event->implem.bridge_req;
  __rt_wait_event(event);
  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  uint64_t retval = req->header.fb_open.screen;    

  rt_irq_restore(irq);

  return retval;
}


void rt_bridge_fb_update(uint64_t fb, unsigned int addr, int posx, int posy, int width, int height, rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();

  rt_event_t *call_event = __rt_wait_event_prepare(event);

  rt_bridge_req_t *req = &call_event->implem.bridge_req;
  // Init request to 0 otherwise the CRC computation on RTL platform
  // is invalid.
  memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
  hal_bridge_fb_update(&req->header, fb, addr, posx, posy, width, height);
  __rt_bridge_post_req(req, call_event);

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



int rt_bridge_write_wait(rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_bridge_req_t *req = &event->implem.bridge_req;
  __rt_wait_event(event);

  // Be careful to not reactivate interrupts before we read
  // the retval as the wait function is putting the event
  // into the free list
  int retval = req->header.write.retval;
  rt_irq_restore(irq);

  return retval;
}



void __rt_bridge_target_status_sync(rt_event_t *event)
{
  int irq = rt_irq_disable();

  hal_bridge_t *bridge = hal_bridge_get();
  if (bridge->bridge.connected)
  {
    rt_event_t *call_event = __rt_wait_event_prepare(event);

    rt_bridge_req_t *req = &call_event->implem.bridge_req;
    // Init request to 0 otherwise the CRC computation on RTL platform
    // is invalid.
    memset((void *)&req->header, 0, sizeof(hal_bridge_req_t));
    hal_bridge_target_status_sync(&req->header);
    __rt_bridge_post_req(req, call_event);

    __rt_wait_event_check(event, call_event);
  }

  rt_irq_restore(irq);
}



// This is called everytime the bridge sends a notification so that we update
// the state of the request queue
void __rt_bridge_handle_notif()
{
  hal_bridge_t *bridge = hal_bridge_get();

  // Go through all the requests and handles the ones which are done
  rt_bridge_req_t *req = (rt_bridge_req_t *)bridge->first_req;

  while (req && req->header.done)
  {
    rt_bridge_req_t *next = (rt_bridge_req_t *)req->header.next;
    bridge->first_req = (uint32_t)next;

    if (req->header.bridge_to_target)
    {
      req->header.next = bridge->first_bridge_free_req;
      bridge->first_bridge_free_req = (unsigned int)req;
    }
    else
    {
      rt_event_enqueue(req->event);
    }

    req = next;
  }

  // Also process bridge to target requests
  if (bridge->target_req)
  {
    rt_bridge_req_t *req = (rt_bridge_req_t *)bridge->target_req;
    bridge->target_req = 0;
    rt_event_t *event = (rt_event_t *)req->event;
    __rt_event_enqueue(event);
  }

  // Then check if we must update the bridge queue
  __rt_bridge_check_bridge_req();
}

static void __rt_bridge_wait()
{
#if defined(ARCHI_FC_CID)
  if (rt_cluster_id() == ARCHI_FC_CID)
  {
#ifdef ITC_VERSION
    hal_itc_wait_for_event_noirq(1<<ARCHI_FC_EVT_CLK_REF);
#else
#ifdef ARCHI_SOC_EVENT_REF_CLK_RISE
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_REF_CLK_RISE);
    __rt_periph_wait_event(ARCHI_SOC_EVENT_REF_CLK_RISE, 1);
    soc_eu_fcEventMask_clearEvent(ARCHI_SOC_EVENT_REF_CLK_RISE);
#endif
#endif
  }
#endif
}

void __rt_bridge_check_connection()
{
  hal_debug_struct_t *debug_struct = hal_debug_struct_get();

  if (!debug_struct->bridge.connected)
  {
    if ((apb_soc_jtag_reg_ext(apb_soc_jtag_reg_read()) >> 1) == 7)
    {
      debug_struct->bridge.connected = 1;

  #if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
#if PULP_CHIP == CHIP_WOLFE
#else
      apb_soc_jtag_reg_write(1<<1);
#endif
  #endif

      while((apb_soc_jtag_reg_ext(apb_soc_jtag_reg_read()) >> 1) == 7)
      {
        __rt_bridge_wait();
      }
    }
  }
}

void __rt_bridge_set_available()
{
  hal_debug_struct_t *debug_struct = hal_debug_struct_get();

  #if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
  if (!debug_struct->bridge.connected)
  {
#if PULP_CHIP == CHIP_WOLFE
#else
    // In case the bridge is not yet connected, there is a handshake to do
    // with him to connect.
    apb_soc_jtag_reg_write(4<<1);
#endif
  }
  else
  {
    // Otherwise just write that we are available
#if PULP_CHIP == CHIP_WOLFE
#else
    apb_soc_jtag_reg_write(1<<1);
#endif
  }
  #endif
}

void __rt_bridge_printf_flush()
{
  hal_debug_struct_t *debug_struct = hal_debug_struct_get();

  __rt_bridge_check_connection();

  if (debug_struct->bridge.connected)
  {
    if (hal_debug_is_busy(hal_debug_struct_get()) || !hal_debug_is_empty(hal_debug_struct_get()))
    {
      // Notify the bridge that he should look for requests
      __rt_bridge_send_notif();

      // We now also have to wait until it is handled so that we can clear
      // the notification
      while(hal_debug_is_busy(hal_debug_struct_get()))
      {
        __rt_bridge_wait();
      }
      __rt_bridge_clear_notif();
    }
  }
}

void __rt_bridge_req_shutdown()
{
#if PULP_CHIP == CHIP_WOLFE
  // JTAG reg does not work well on wolfe
  return;
#endif

  hal_debug_struct_t *debug_struct = hal_debug_struct_get();

  __rt_bridge_check_connection();

  if (debug_struct->bridge.connected)
  {
    // We have to flush pending requests before sending shutdown request
    // otherwise the bridge may never see them.
    __rt_bridge_printf_flush();

  #if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2

    // It can happen that the bridge is still in a state where he haven't
    // seen that we became available. Wait until this is the case.
    while((apb_soc_jtag_reg_ext(apb_soc_jtag_reg_read()) >> 1) == 7)
    {
      __rt_bridge_wait();
    }

    // Send the request for shutdown
    apb_soc_jtag_reg_write(2<<1);

    // And wait until it is acknowledged
    while((apb_soc_jtag_reg_ext(apb_soc_jtag_reg_read()) >> 1) != 7)
    {
      __rt_bridge_wait();
    }

    // Update the status so that the bridge knows that we got the aknowledgement
    apb_soc_jtag_reg_write(0<<1);

    // And wait until it knows it
    while((apb_soc_jtag_reg_ext(apb_soc_jtag_reg_read()) >> 1) == 7)
    {
      __rt_bridge_wait();
    }
  #endif
  }
}

void __rt_bridge_send_notif()
{
  hal_debug_struct_t *debug_struct = hal_debug_struct_get();

  __rt_bridge_check_connection();

  if (debug_struct->bridge.connected)
  {
  #if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 2
#if PULP_CHIP == CHIP_WOLFE
#else
    apb_soc_jtag_reg_write(3<<1);
#endif
  #endif
  }
}

void __rt_bridge_clear_notif()
{
  hal_debug_struct_t *debug_struct = hal_debug_struct_get();

  __rt_bridge_check_connection();

  if (debug_struct->bridge.connected)
  {
    __rt_bridge_set_available();
  }
}


RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_bridge_init()
{
  hal_bridge_t *bridge = hal_bridge_get();
  
  bridge->first_req = 0;
  bridge->first_bridge_req = 0;
  bridge->target_req = 0;

#ifdef ITC_VERSION
  bridge->notif_req_addr = ARCHI_FC_ITC_ADDR + ITC_STATUS_SET_OFFSET;
  bridge->notif_req_value = 1<<RT_BRIDGE_ENQUEUE_EVENT;
#else
#if defined(EU_VERSION) && EU_VERSION >= 3
#if defined(ARCHI_HAS_FC)
  bridge->notif_req_addr = ARCHI_FC_GLOBAL_ADDR + ARCHI_FC_PERIPHERALS_OFFSET + ARCHI_FC_EU_OFFSET + EU_SW_EVENTS_AREA_BASE + EU_CORE_TRIGG_SW_EVENT + (RT_BRIDGE_ENQUEUE_EVENT << 2);
  bridge->notif_req_value = 1;
#endif
#endif
#endif

  __rt_bridge_eeprom_handle = NULL;
  __rt_bridge_flash_handle = NULL;
}
