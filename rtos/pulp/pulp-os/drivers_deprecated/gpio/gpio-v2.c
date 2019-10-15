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

#include "rt/rt_api.h"

RT_FC_TINY_DATA uint32_t __rt_gpio_status;
RT_FC_TINY_DATA rt_event_t *__rt_gpio_events[32];
RT_FC_TINY_DATA uint32_t __rt_gpio_active_events;

extern void __rt_gpio_handler();


void rt_gpio_init(uint8_t group, int gpio)
{
}

void rt_gpio_deinit(uint8_t group, int gpio)
{
  rt_gpio_set_sensitivity(group, gpio, RT_GPIO_SENSITIVITY_NONE);
  rt_gpio_set_event(group, gpio, NULL);
}

void rt_gpio_set_sensitivity(uint8_t group, uint8_t gpio, rt_gpio_sensitivity_e sensitivity)
{
  int irq = rt_irq_disable();
  int inten = sensitivity != RT_GPIO_SENSITIVITY_NONE;
  hal_gpio_inten_set((hal_gpio_inten_get() & ~(1<<gpio)) | (inten << gpio));
  if (!inten) return;
  int inttype = ARCHI_GPIO_INTTYPE_NO(gpio);
  int mode =
    sensitivity == RT_GPIO_SENSITIVITY_RISE ? ARCHI_GPIO_INTTYPE_RISE : 
    sensitivity == RT_GPIO_SENSITIVITY_FALL ? ARCHI_GPIO_INTTYPE_FALL :
    ARCHI_GPIO_INTTYPE_RISE_AND_FALL;
  uint32_t prev = hal_gpio_inttype_get(inttype) & ~(((1<<ARCHI_GPIO_INTTYPE_SIZE) - 1) << ARCHI_GPIO_INTTYPE_BIT(gpio));
  hal_gpio_inttype_set(inttype, prev | (mode << ARCHI_GPIO_INTTYPE_BIT(gpio)));
  rt_irq_restore(irq);
}

void rt_gpio_set_event(uint8_t group, uint8_t gpio, rt_event_t *event)
{
  int irq = rt_irq_disable();
  __rt_gpio_events[gpio] = event;
  if (event)
    __rt_gpio_active_events |= 1 << gpio;
  else
    __rt_gpio_active_events &= ~(1 << gpio);
  rt_irq_restore(irq);
}



void rt_gpio_wait(uint8_t group, uint8_t gpio)
{
  int irq = rt_irq_disable();
  while(!(__rt_gpio_status & (1<<gpio)))
  {
    __rt_event_yield(NULL);
  }
  __rt_gpio_status &= ~(1<<gpio);
  rt_irq_restore(irq);
}



void rt_gpio_clear(uint8_t group, uint8_t gpio)
{
  int irq = rt_irq_disable();
  __rt_gpio_status &= ~(1<<gpio);
  rt_irq_restore(irq);
}



int rt_gpio_configure(uint8_t group, uint32_t mask, rt_gpio_conf_e flags)
{
#if PULP_CHIP_FAMILY == CHIP_GAP

  int strength = (flags >> __RT_GPIO_STRENGTH_BIT) & 3;
  int pull = (flags >> __RT_GPIO_PULL_BIT) & 3;

  while (mask)
  {
    int gpio = __FL1(mask);

    int reg_id = ARCHI_GPIO_PADCFG_REG(gpio);
    int group = ARCHI_GPIO_PADCFG_GROUP(gpio);
    gpio_reg_padcfg_t reg = { .raw=gpio_padcfg_get(reg_id) };

    if (strength)
    {
      reg.pin[group].strength = strength >> 1;
    }

    if (pull)
      reg.pin[group].pull = pull >> 1;

    gpio_padcfg_set(reg_id, reg.raw);

    mask &= ~(1<<gpio);
  }
#endif

  return 0;
}



RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_gpio_init()
{
#ifdef ARCHI_FC_EVT_GPIO
  rt_irq_set_handler(ARCHI_FC_EVT_GPIO, __rt_gpio_handler);
  rt_irq_mask_set(1<<ARCHI_FC_EVT_GPIO);
#else
#ifdef SOC_EU_VERSION
  soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_GPIO);
#endif
#endif
  for (int i=0; i<32; i++)
  {
    __rt_gpio_events[i] = NULL;
  }
  
  __rt_gpio_status = 0;
  __rt_gpio_active_events = 0;
}
