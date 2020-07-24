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
  uint32_t nb_open;
  uint32_t irq_status;
  uint32_t input_mask;
  uint32_t output_mask;
  pi_gpio_callback_t *cb;
  pi_task_t *event_task[ARCHI_NB_GPIO];
} pi_gpio_t;

static pi_gpio_t __rt_gpio[NB_GPIO_PORT] = {0};

void __pi_gpio_handler(void *arg);


static void __pi_gpio_callback_fire(pi_gpio_t *gpio, pi_gpio_callback_t *cb)
{
  uint32_t irq_mask = gpio->irq_status;
  pi_gpio_callback_t *tmp_cb = cb;
  while (tmp_cb != NULL)
  {
    if (irq_mask & tmp_cb->pin_mask)
    {
      tmp_cb->handler(tmp_cb->args);
    }
    tmp_cb = tmp_cb->next;
  }
}


void __pi_gpio_handler(void *arg)
{
  (void) arg;

  /* Retrieve IRQ status from GPIO. Handle task if needed. */
  pi_gpio_t *gpio = &__rt_gpio[0];
  gpio->irq_status = hal_gpio_intstatus_get();

  if (gpio->cb != NULL)
  {
    __pi_gpio_callback_fire(gpio, gpio->cb);
  }
  else
  {
    uint8_t pin = 0;
    uint32_t irq_mask = 0;
    pi_task_t *task = NULL;
    irq_mask = (gpio->irq_status & gpio->input_mask);
    while (irq_mask)
    {
      if (irq_mask & 0x1)
      {
        task = gpio->event_task[pin];
        if (task != NULL)
        {
          __rt_event_handle_end_of_task(task);
        }
      }
      irq_mask = irq_mask >> 1;
      pin++;
    }
  }
}


void pi_gpio_conf_init(struct pi_gpio_conf *conf)
{
  conf->port = 0;
}



int pi_gpio_open(struct pi_device *device)
{
  struct pi_gpio_conf *conf = (struct pi_gpio_conf *)device->config;

  if (conf->port >= NB_GPIO_PORT)
  {
    return -1;
  }

  pi_gpio_t *gpio = &__rt_gpio[conf->port];
  if (gpio->nb_open == 0)
  {
    gpio->port = conf->port;
    gpio->nb_open = 1;
    gpio->irq_status = 0;
    gpio->input_mask = 0;
    gpio->output_mask = 0;
    gpio->cb = NULL;
    for (int i=0; i<(int) ARCHI_NB_GPIO; i++)
    {
      gpio->event_task[i] = NULL;
    }

    /* Enable SOC event propagation to FC. */
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_GPIO);
  }
  else
  {
    gpio->nb_open++;
  }
  device->data = (void *)gpio;

  return 0;
}

void pi_gpio_close(struct pi_device *device)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  gpio->nb_open--;
  if (gpio->nb_open)
  {
    /* Disable SOC event propagation to FC. */
    soc_eu_fcEventMask_clearEvent(ARCHI_SOC_EVENT_GPIO);

    /* Disable input GPIOs. */
    hal_gpio_en_set(0);
  }
  device->data = NULL;
}

int pi_gpio_pin_configure(struct pi_device *device, pi_gpio_e pin, pi_gpio_flags_e flags)
{
  if (pin & PI_GPIO_IS_GPIO_MASK)
  {
    pi_pad_e pad = ((pin & PI_GPIO_PAD_MASK) >> PI_GPIO_PAD_SHIFT);
    /* Setup first pad for GPIO. */
    pi_pad_set_function(pad, PI_PAD_FUNC1);
  }
  pin = (pin & PI_GPIO_NUM_MASK);
  return pi_gpio_mask_configure(device, 1<<pin, flags);
}

int pi_gpio_pin_write(struct pi_device *device, uint32_t pin, uint32_t value)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);
  if ((gpio->output_mask & mask) != mask)
  {
    return -11;
  }
  hal_gpio_set_pin_value(pin, value);
  return 0;
}

int pi_gpio_pin_read(struct pi_device *device, uint32_t pin, uint32_t *value)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);
  if (((gpio->input_mask & mask) != mask) &&
      ((gpio->output_mask & mask) != mask))
  {
    return -11;
  }

  if ((gpio->input_mask & mask))
  {
    *value = ((hal_gpio_get_value() >> pin) & 0x1);
  }
  else
  {
    *value = ((hal_gpio_paddir_get() >> pin) & 0x1);
  }
  return 0;
}

void pi_gpio_pin_notif_configure(struct pi_device *device, uint32_t pin,
                                 pi_gpio_notif_e flags)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);
  if ((gpio->input_mask & mask) != mask)
  {
    return;
  }

  if (flags == PI_GPIO_NOTIF_NONE)
  {
    hal_gpio_inten_set(hal_gpio_inten_get() & ~(1<<pin));
  }
  else
  {
    hal_gpio_inten_set(hal_gpio_inten_get() | (1<<pin));
    int inttype = ARCHI_GPIO_INTTYPE_NO(pin);
    int mode =
      flags == PI_GPIO_NOTIF_RISE ? ARCHI_GPIO_INTTYPE_RISE :
      flags == PI_GPIO_NOTIF_FALL ? ARCHI_GPIO_INTTYPE_FALL :
      ARCHI_GPIO_INTTYPE_RISE_AND_FALL;
    uint32_t prev = hal_gpio_inttype_get(inttype) & ~(((1<<ARCHI_GPIO_INTTYPE_SIZE) - 1) << ARCHI_GPIO_INTTYPE_BIT(pin));
    hal_gpio_inttype_set(inttype, prev | (mode << ARCHI_GPIO_INTTYPE_BIT(pin)));
  }
}

void pi_gpio_pin_notif_clear(struct pi_device *device, uint32_t pin)
{
  int irq = rt_irq_disable();
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);
  gpio->irq_status &= ~mask;
}

int pi_gpio_pin_notif_get(struct pi_device *device, uint32_t pin)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);
  return ((gpio->irq_status >> pin) & 0x1);
}

int pi_gpio_pin_task_add(struct pi_device *device, uint32_t pin, pi_task_t *task,
                         pi_gpio_notif_e flags)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);

  if ((gpio->input_mask & mask) != mask)
  {
    return -11;
  }

  gpio->event_task[pin] = task;
  if (flags == PI_GPIO_NOTIF_NONE)
  {
    hal_gpio_inten_set(hal_gpio_inten_get() & ~(1<<pin));
  }
  else
  {
    hal_gpio_inten_set(hal_gpio_inten_get() | (1<<pin));
    int inttype = ARCHI_GPIO_INTTYPE_NO(pin);
    int mode =
      flags == PI_GPIO_NOTIF_RISE ? ARCHI_GPIO_INTTYPE_RISE :
      flags == PI_GPIO_NOTIF_FALL ? ARCHI_GPIO_INTTYPE_FALL :
      ARCHI_GPIO_INTTYPE_RISE_AND_FALL;
    uint32_t prev = hal_gpio_inttype_get(inttype) & ~(((1<<ARCHI_GPIO_INTTYPE_SIZE) - 1) << ARCHI_GPIO_INTTYPE_BIT(pin));
    hal_gpio_inttype_set(inttype, prev | (mode << ARCHI_GPIO_INTTYPE_BIT(pin)));
  }
  return 0;
}

int pi_gpio_pin_task_remove(struct pi_device *device, uint32_t pin)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  pin = (pin & PI_GPIO_NUM_MASK);
  uint32_t mask = (1 << pin);

  gpio->event_task[pin] = NULL;
  return 0;
}

int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask, pi_gpio_flags_e flags)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  int is_out = flags & PI_GPIO_OUTPUT;
  hal_gpio_set_dir(mask, is_out);

  if (is_out)
  {
    gpio->input_mask &= ~mask;
    gpio->output_mask |= mask;
    hal_gpio_en_set(hal_gpio_en_get() & ~mask);
  }
  else
  {
    gpio->input_mask |= mask;
    gpio->output_mask &= ~mask;
    hal_gpio_en_set(hal_gpio_en_get() | mask);
  }

  return 0;
}

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  if ((gpio->output_mask & mask) != mask)
  {
    return -11;
  }
  hal_gpio_set_value(mask, value);
  return 0;
}

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  if ((gpio->input_mask & mask))
  {
    *value = (hal_gpio_get_value() & mask);
  }
  else
  {
    *value = (hal_gpio_paddir_get() & mask);
  }
  return 0;
}

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask,
                          pi_task_t *task, pi_gpio_notif_e flags)
{
  uint32_t gpio_mask = mask;
  uint8_t pin = 0;
  int32_t error = 0;
  while (gpio_mask)
  {
    if (gpio_mask & 0x1)
    {
      error = pi_gpio_pin_task_add(device, pin, task, flags);
      if (error)
      {
        return -11;
      }
    }
    gpio_mask = (gpio_mask >> 1);
    pin++;
  }
  return 0;
}

int pi_gpio_mask_task_remove(struct pi_device *device, uint32_t mask)
{
  uint32_t gpio_mask = mask;
  uint8_t pin = 0;
  int32_t error = 0;
  while (gpio_mask)
  {
    if (gpio_mask & 0x1)
    {
      error = pi_gpio_pin_task_remove(device, pin);
      if (error)
      {
        return -11;
      }
    }
    gpio_mask = (gpio_mask >> 1);
    pin++;
  }
  return 0;
}

int pi_gpio_callback_add(struct pi_device *device, pi_gpio_callback_t *cb)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  if (cb == NULL)
  {
    return -11;
  }

  pi_gpio_callback_t *driver_cb = gpio->cb;
  if (driver_cb == NULL)
  {
    gpio->cb = cb;
  }
  else
  {
    while (driver_cb->next != NULL)
    {
      driver_cb = driver_cb->next;
    }
    driver_cb->next = cb;
    cb->prev = driver_cb;
  }
  return 0;
}

int pi_gpio_callback_remove(struct pi_device *device, pi_gpio_callback_t *cb)
{
  pi_gpio_t *gpio = (pi_gpio_t *) device->data;
  if (cb == NULL)
  {
    return -11;
  }

  if (gpio->cb == NULL)
  {
    return -12;
  }

  /* Callback at the head. */
  if (gpio->cb == cb)
  {
    gpio->cb = gpio->cb->next;
    gpio->cb->prev = NULL;
    return 0;
  }

  /* Callback in list. */
  pi_gpio_callback_t *temp_cb = gpio->cb;
  while ((temp_cb != cb) && (temp_cb->next != NULL))
  {
    temp_cb = temp_cb->next;
  }
  if (temp_cb != cb)
  {
    return -13;
  }
  if (temp_cb->next != NULL)
  {
    temp_cb->next->prev = temp_cb->prev;
  }
  if (temp_cb->prev != NULL)
  {
    temp_cb->prev->next = temp_cb->next;
  }
  return 0;
}
