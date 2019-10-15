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

#ifndef __RT__RT_EEPROM_H__
#define __RT__RT_EEPROM_H__

typedef struct rt_eeprom_s
{
  rt_i2c_t *i2c_handle;
  rt_event_t *current_event;
  rt_periph_copy_t *first_pending;
  rt_periph_copy_t *last_pending;

  uint32_t current_addr;
  uint8_t *current_data;
  int current_size;
  int8_t is_write;
  int8_t send_header;

  uint8_t *header_data;
} rt_eeprom_t;

typedef struct rt_eeprom_conf_s
{
  int8_t cs;
  int8_t id;
} rt_eeprom_conf_t;



void rt_eeprom_conf_init(rt_eeprom_conf_t *conf);



rt_eeprom_t *rt_eeprom_open(char *dev_name, rt_eeprom_conf_t *conf, rt_event_t *event);


void rt_eeprom_write(rt_eeprom_t *handle, uint32_t addr, uint8_t *data, int size, rt_event_t *event);

void rt_eeprom_read(rt_eeprom_t *handle, uint32_t addr, uint8_t *data, int size, rt_event_t *event);

void rt_eeprom_close(rt_eeprom_t *handle, rt_event_t *event);



#endif
