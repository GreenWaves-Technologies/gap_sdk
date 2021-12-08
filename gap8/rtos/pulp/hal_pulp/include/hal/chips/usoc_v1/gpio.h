/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __HAL_CHIPS_USOC_V1_GPIO_H__
#define __HAL_CHIPS_USOC_V1_GPIO_H__

#include "archi/chips/usoc_v1/gpio.h"

static inline unsigned int hal_gpio_padin_get()
{
  return pulp_read32(ARCHI_GPIO_ADDR + ARCHI_GPIO_PADIN);
}

static inline void hal_gpio_padout_set(unsigned int value)
{
  pulp_write32(ARCHI_GPIO_ADDR + ARCHI_GPIO_PADOUT, value);
}

static inline void hal_gpio_padoutset_set(unsigned int value)
{
  pulp_write32(ARCHI_GPIO_ADDR + ARCHI_GPIO_PADOUTSET, value);
}

static inline void hal_gpio_padoutclr_set(unsigned int value)
{
  pulp_write32(ARCHI_GPIO_ADDR + ARCHI_GPIO_PADOUTCLR, value);
}

static inline unsigned int hal_gpio_padout_get()
{
  return pulp_read32(ARCHI_GPIO_ADDR + ARCHI_GPIO_PADOUT);
}

static inline void hal_gpio_inten_set(unsigned int value)
{
  pulp_write32(ARCHI_GPIO_ADDR + ARCHI_GPIO_INTEN, value);
}

static inline unsigned int hal_gpio_inten_get()
{
  return pulp_read32(ARCHI_GPIO_ADDR + ARCHI_GPIO_INTEN);
}

static inline void hal_gpio_inttype_set(unsigned int id, unsigned int value)
{
  pulp_write32(ARCHI_GPIO_ADDR + ARCHI_GPIO_INTTYPE(id), value);
}

static inline unsigned int hal_gpio_inttype_get(unsigned int id)
{
  return pulp_read32(ARCHI_GPIO_ADDR + ARCHI_GPIO_INTTYPE(id));
}

static inline unsigned int hal_gpio_intstatus_get()
{
  return pulp_read32(ARCHI_GPIO_ADDR + ARCHI_GPIO_INTSTATUS);
}

static inline unsigned int hal_gpio_en_get()
{
  return pulp_read32(ARCHI_GPIO_ADDR + ARCHI_GPIO_EN);
}

static inline void hal_gpio_en_set(unsigned int value)
{
  pulp_write32(ARCHI_GPIO_ADDR + ARCHI_GPIO_EN, value);
}



static inline void hal_gpio_set_value(uint32_t mask, uint8_t value)
{
  if (value)
  {
    hal_gpio_padoutset_set(mask);
  }
  else
  {
    hal_gpio_padoutclr_set(mask);
  }
}

static inline uint32_t hal_gpio_get_value()
{
  return hal_gpio_padin_get();
}

static inline void hal_gpio_set_pin_value(uint32_t pin, uint8_t value)
{
  if (value)
  {
    hal_gpio_padoutset_set(1<<pin);
  }
  else
  {
    hal_gpio_padoutclr_set(1<<pin);
  }
}

#endif