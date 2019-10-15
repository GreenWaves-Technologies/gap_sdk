/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and
 * GreenWaves Technologies
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

#define HEADER_DATA_SIZE 4

void rt_eeprom_conf_init(rt_eeprom_conf_t *conf)
{
}

rt_eeprom_t *rt_eeprom_open(char *dev_name, rt_eeprom_conf_t *conf, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_eeprom_conf_t def_conf;

  if (conf == NULL)
  {
    conf = &def_conf;
    rt_eeprom_conf_init(conf);
  }
  
  rt_i2c_conf_t i2c_conf;
  rt_i2c_conf_init(&i2c_conf);

  i2c_conf.id = conf->id;
  i2c_conf.max_baudrate = 200000;
  i2c_conf.cs = conf->cs;

  rt_eeprom_t *handle = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_eeprom_t));
  if (handle == NULL) goto error;

  handle->i2c_handle = rt_i2c_open(dev_name, &i2c_conf, NULL);
  if (handle->i2c_handle == NULL)
  {
    rt_free(RT_ALLOC_FC_DATA, handle, sizeof(rt_eeprom_t));
    goto error;
  }

  handle->header_data = rt_alloc(RT_ALLOC_PERIPH, HEADER_DATA_SIZE);
  if (handle->i2c_handle == NULL)
  {
    rt_free(RT_ALLOC_PERIPH, handle->header_data, HEADER_DATA_SIZE);
    rt_free(RT_ALLOC_FC_DATA, handle, sizeof(rt_eeprom_t));
    goto error;

  }

  handle->current_event = NULL;
  handle->first_pending = NULL;

  if (event)
    __rt_event_enqueue(event);


  rt_irq_restore(irq);

  return handle;

error:
  rt_warning("[EEPROM] Failed to open eeprom device\n");
  return NULL;
}


void rt_eeprom_close(rt_eeprom_t *handle, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_i2c_close(handle->i2c_handle, NULL);

  rt_free(RT_ALLOC_FC_DATA, handle, sizeof(rt_eeprom_t));

  rt_irq_restore(irq);

}

static void __rt_eeprom_handle_current_copy(rt_eeprom_t *handle)
{
  if (handle->send_header)
  {
    handle->header_data[0] = handle->current_addr >> 8;
    handle->header_data[1] = handle->current_addr & 0xff;

    handle->send_header = 0;

    rt_i2c_write(handle->i2c_handle, handle->header_data, 2, 1, handle->current_event);
  }
  else
  {
    int size = handle->current_size;
    // Max size for read is 256 due to the repeat command used in the i2c driver
    // and 128 for writes due to the page size in the eeprom
    int max_size = handle->is_write ? 128 : 256;
    if (size > max_size)
      size = max_size;

    handle->send_header = 1;

    if (handle->is_write)
    {
      rt_i2c_write_append(handle->i2c_handle, handle->current_data, size, 0, handle->current_event);
    }
    else
    {
      rt_i2c_read(handle->i2c_handle, handle->current_data, size, 0, handle->current_event);
    }

    handle->current_addr += size;
    handle->current_data += size;
    handle->current_size -= size;
  }
}

static void __rt_eeprom_enqueue_transfer(rt_eeprom_t *handle, uint32_t addr, uint8_t *data, int size, int is_write, rt_event_t *event)
{
  int irq = rt_irq_disable();

  int async = event != NULL;
  rt_event_t *call_event = __rt_wait_event_prepare(event);

  if (handle->current_event)
  {
#if 0
    if (handle->first_pending)
      handle->last_pending->next = copy;
    else
      handle->first_pending = copy;

    copy->next = NULL;
    handle->last_pending = copy;
#endif
  }
  else
  {
    handle->current_event = call_event;

    handle->current_addr = addr;
    handle->current_data = data;
    handle->current_size = size;
    handle->is_write = is_write;
    handle->send_header = 1;

    if (async)
    {

    }
    else
    {
      while (1)
      {
        __rt_eeprom_handle_current_copy(handle);

        if (!async)
          __rt_wait_event(handle->current_event);

        if (is_write && handle->send_header)
          rt_time_wait_us(6000);

        if (handle->current_size <= 0)
          break;

        handle->current_event = __rt_wait_event_prepare_blocking();
      }

      handle->current_event = NULL;
    }
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}

void rt_eeprom_write(rt_eeprom_t *handle, uint32_t addr, uint8_t *data, int size, rt_event_t *event)
{
  __rt_eeprom_enqueue_transfer(handle, addr, data, size, 1, event);
}

void rt_eeprom_read(rt_eeprom_t *handle, uint32_t addr, uint8_t *data, int size, rt_event_t *event)
{
  __rt_eeprom_enqueue_transfer(handle, addr, data, size, 0, event);
}



