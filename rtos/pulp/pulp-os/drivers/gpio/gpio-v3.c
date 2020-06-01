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

#include "pmsis.h"

#define NB_GPIO_PORT ((ARCHI_NB_GPIO+31)/32)

extern RT_FC_TINY_DATA uint32_t __rt_gpio_status;

typedef struct 
{
  int port;
} pi_gpio_t;

static pi_gpio_t __rt_gpio[NB_GPIO_PORT];



void pi_gpio_conf_init(struct pi_gpio_conf *conf)
{
  conf->port = 0;
}



int pi_gpio_open(struct pi_device *device)
{
  int irq = rt_irq_disable();

  struct pi_gpio_conf *conf = (struct pi_gpio_conf *)device->config;

  if (conf->port >= NB_GPIO_PORT)
    goto error;

  pi_gpio_t *gpio = &__rt_gpio[conf->port];

  device->data = (void *)gpio;

  gpio->port = conf->port;

  rt_irq_restore(irq);

  return 0;

error:
  rt_irq_restore(irq);
  return -1;
}


int pi_gpio_pin_configure(struct pi_device *device, pi_gpio_e pin, pi_gpio_flags_e flags)
{
  if (pin & PI_GPIO_IS_GPIO_MASK)
  {
    pi_pad_e pad = ((pin & ~PI_GPIO_IS_GPIO_MASK)>> PI_GPIO_NUM_SHIFT);
    /* Setup first pad for GPIO. */

    pi_pad_set_function(pad, PI_PAD_FUNC1);
  }
  pin = (pin & PI_GPIO_NUM_MASK);
  return pi_gpio_mask_configure(device, 1<<pin, flags);
}

int pi_gpio_pin_write(struct pi_device *device, uint32_t pin, uint32_t value)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  int irq = rt_irq_disable();
  hal_gpio_set_pin_value(pin, value);
  rt_irq_restore(irq);
  return 0;
}

int pi_gpio_pin_read(struct pi_device *device, uint32_t pin, uint32_t *value)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  *value = (hal_gpio_get_value() >> pin) & 1;
  return 0;
}

int pi_gpio_pin_task_add(struct pi_device *device, uint32_t pin, pi_task_t *task, pi_gpio_notif_e flags)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  return 0;
}

int pi_gpio_pin_task_remove(struct pi_device *device, uint32_t pin)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  return 0;
}

int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask, pi_gpio_flags_e flags)
{
  int irq = rt_irq_disable();
  int is_out = flags & PI_GPIO_OUTPUT;
  hal_gpio_set_dir(mask, is_out);

  if (is_out)
    hal_gpio_en_set(hal_gpio_en_get() & ~mask);
  else
    hal_gpio_en_set(hal_gpio_en_get() | mask);

  rt_irq_restore(irq);

  return 0;
}

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value)
{
  hal_gpio_set_value(mask, value);
  return 0;
}

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value)
{
  *value = hal_gpio_get_value();
  return 0;
}

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask, pi_task_t *task, pi_gpio_notif_e flags)
{
  return 0;
}

int pi_gpio_mask_task_remove(struct pi_device *device, uint32_t mask)
{
  return 0;
}

void pi_gpio_pin_notif_configure(struct pi_device *device, uint32_t pin, pi_gpio_notif_e flags)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  int irq = rt_irq_disable();
  if (flags == PI_GPIO_NOTIF_NONE)
  {
    //hal_gpio_inten_set(hal_gpio_inten_get() & ~(1<<pin));
  }
  else
  {
    //hal_gpio_inten_set(hal_gpio_inten_get() | (1<<pin));
    int inttype = ARCHI_GPIO_INTTYPE_NO(pin);
    int mode =
      flags == PI_GPIO_NOTIF_RISE ? ARCHI_GPIO_INTTYPE_RISE : 
      flags == PI_GPIO_NOTIF_FALL ? ARCHI_GPIO_INTTYPE_FALL :
      ARCHI_GPIO_INTTYPE_RISE_AND_FALL;
    uint32_t prev = hal_gpio_inttype_get(inttype) & ~(((1<<ARCHI_GPIO_INTTYPE_SIZE) - 1) << ARCHI_GPIO_INTTYPE_BIT(pin));
    hal_gpio_inttype_set(inttype, prev | (mode << ARCHI_GPIO_INTTYPE_BIT(pin)));
  }
  rt_irq_restore(irq);
}

void pi_gpio_pin_notif_clear(struct pi_device *device, uint32_t pin)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  int irq = rt_irq_disable();
  __rt_gpio_status &= ~(1<<pin);
  rt_irq_restore(irq);
}

int pi_gpio_pin_notif_get(struct pi_device *device, uint32_t pin)
{
  pin = (pin & PI_GPIO_NUM_MASK);
  return (__rt_gpio_status >> pin) & 1;
}
