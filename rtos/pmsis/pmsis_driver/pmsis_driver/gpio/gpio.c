/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct
{
    uint8_t port;
    pi_task_t *cb;
} pi_gpio_t;

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static pi_gpio_t __global_gpio[ARCHI_NB_GPIO];

static pi_task_t *__global_gpio_task[ARCHI_NB_GPIO];

/* TODO : Add handler. */
static uint32_t __gpio_irq_status = 0x0;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Print gpio info. */
void pi_gpio_print();

/* Handler. */
static void gpio_handler(void *arg);

/* End of task handler. */
static void __pi_gpio_handle_end_of_task(struct pi_task *task);

/*******************************************************************************
 * Inner functions.
 ******************************************************************************/

void pi_gpio_print()
{
    DEBUG_PRINTF("GpioEn : %x ", hal_gpio_gpioen_get());
    DEBUG_PRINTF("Paddir|Padin|Padout : %x %x %x  ",
           hal_gpio_paddir_get(), hal_gpio_padin_get(), hal_gpio_padout_get());
    DEBUG_PRINTF("Padcfg :\n");
    for (uint32_t i=0; i<(uint32_t) ARCHI_GPIO_NB_PADCFG_REG; i++)
    {
        DEBUG_PRINTF("%x %x %x %x\n",
                     hal_gpio_padcfg_get(i + 0), hal_gpio_padcfg_get(i + 1),
                     hal_gpio_padcfg_get(i + 2), hal_gpio_padcfg_get(i + 3));
    }
}

static void __pi_gpio_handle_end_of_task(struct pi_task *task)
{
    if (task->id == PI_TASK_NONE_ID)
    {
        pi_task_release(task);
    }
    else
    {
        pmsis_event_push(pmsis_event_get_default_scheduler(), task);
    }
}

static void gpio_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;

    /* Retrieve IRQ status from GPIO. Handle task if needed. */
    uint32_t irq_mask = hal_gpio_get_irq_status(), pin = 0;;
    struct pi_task *task = NULL;
    while (irq_mask)
    {
        if (irq_mask & 0x1)
        {
            task = __global_gpio_task[pin];
            if (task != NULL)
            {
                __pi_gpio_handle_end_of_task(task);
            }
        }
        irq_mask = irq_mask >> 1;
        pin++;
    }
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_gpio_conf_init(struct pi_gpio_conf *conf)
{
    conf->device = PI_DEVICE_GPIO_TYPE;
    conf->port = 0;
}

int pi_gpio_open(struct pi_device *device)
{
    #if 0
    struct pi_gpio_conf *conf = (struct pi_gpio_conf *) device->config;
    if (conf->port >= ((uint32_t) ARCHI_NB_GPIO))
    {
        return -1;
    }
    uint8_t port = conf->port;
    __global_gpio[port].port = port;
    __global_gpio[port].cb = NULL;
    device->data = (void *) &__global_gpio[port];
    __gpio_irq_status &= ~(0x1 << port);
    #endif
    for (uint32_t i = 0; i < (uint32_t) ARCHI_NB_GPIO; i++)
    {
        __global_gpio_task[i] = NULL;
    }

    /* Set handlers. */
    pi_fc_event_handler_set(SOC_EVENT_GPIO, gpio_handler);
    /* Enable SOC events propagation to FC. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_GPIO);

    return 0;
}

/* TODO : Add flags to set irq type, irq enable, drive strength, pull enable. */
int pi_gpio_pin_configure(struct pi_device *device, pi_gpio_e gpio, pi_gpio_flags_e flags)
{
    pi_pad_e pad = (gpio >> PI_GPIO_NUM_SHIFT);
    uint32_t pin = (gpio & PI_GPIO_NUM_MASK);

    /* Setup first pad for GPIO. */
    pi_pad_set_function(pad, PI_PAD_FUNC1);

    uint8_t pe = (flags & PI_GPIO_PULL_ENABLE) >> PI_GPIO_PULL_OFFSET;
    uint8_t ds = (flags & PI_GPIO_DRIVE_STRENGTH_HIGH) >> PI_GPIO_DRIVE_OFFSET;
    uint8_t dir = (flags & PI_GPIO_OUTPUT) >> PI_GPIO_MODE_OFFSET;

    hal_gpio_pin_set_direction(pin, dir);
    hal_gpio_pin_configuration(pin, pe, ds);
    hal_gpio_pin_enable(pin, !dir);
    #ifdef DEBUG
    pi_gpio_print();
    #endif
    return 0;
}

int pi_gpio_pin_write(struct pi_device *device, uint32_t pin, uint32_t value)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    #ifdef DEBUG
    DEBUG_PRINTF("Pin: %d val: %d\n", pin, value);
    #endif
    hal_gpio_pin_set_output_value(pin, (uint8_t) (value & 0xff));
    return 0;
}

int pi_gpio_pin_read(struct pi_device *device, uint32_t pin, uint32_t *value)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    *value = hal_gpio_get_input_value(pin);
    return *value;
}

void pi_gpio_pin_notif_configure(struct pi_device *device, uint32_t pin, pi_gpio_notif_e irq_type)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    if (irq_type == PI_GPIO_NOTIF_NONE)
    {
        hal_gpio_pin_set_irq(pin, 0);
    }
    else
    {
        hal_gpio_pin_set_irq(pin, 1);
        hal_gpio_pin_set_irq_type(pin, irq_type);
    }
}

void pi_gpio_pin_notif_clear(struct pi_device *device, uint32_t pin)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    __gpio_irq_status &= ~(0x1 << pin);
}

int pi_gpio_pin_notif_get(struct pi_device *device, uint32_t pin)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    __gpio_irq_status = hal_gpio_get_irq_status();
    return ((__gpio_irq_status >> pin) & 0x1);
}

/* TODO : Add flags to set irq type, irq enable, drive strength, pull enable. */
int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask, pi_gpio_flags_e flags)
{
    uint8_t pe = (flags & PI_GPIO_PULL_ENABLE) >> PI_GPIO_PULL_OFFSET;
    uint8_t ds = (flags & PI_GPIO_DRIVE_STRENGTH_HIGH) >> PI_GPIO_DRIVE_OFFSET;
    uint8_t dir = (flags & PI_GPIO_OUTPUT) >> PI_GPIO_MODE_OFFSET;

    hal_gpio_set_direction(mask, dir);
    hal_gpio_configuration(mask, pe, ds);
    hal_gpio_enable(mask, !dir);
    return 0;
}

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value)
{
    hal_gpio_set_output_value(mask, value);
    return 0;
}

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value)
{
    *value = hal_gpio_get_input_value();
    return *value;
}

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask, pi_task_t *task, pi_gpio_notif_e irq_type)
{
    uint32_t gpio_mask = mask, pin = 0;
    while (gpio_mask)
    {
        if (gpio_mask & 0x1)
        {
            pi_gpio_pin_task_add(device, pin, task, irq_type);
        }
        gpio_mask = gpio_mask >> 1;
        pin++;
    }
    return 0;
}

int pi_gpio_mask_task_remove(struct pi_device *device, uint32_t mask)
{
    uint32_t gpio_mask = mask, pin = 0;
    while (gpio_mask)
    {
        if (gpio_mask & 0x1)
        {
            pi_gpio_pin_task_remove(device, pin);
        }
        gpio_mask = gpio_mask >> 1;
        pin++;
    }
    return 0;
}

int pi_gpio_pin_task_add(struct pi_device *device, uint32_t pin,
                             pi_task_t *task, pi_gpio_notif_e irq_type)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    __global_gpio_task[pin] = task;
    hal_gpio_pin_set_irq_type(pin, irq_type);
    return 0;
}

int pi_gpio_pin_task_remove(struct pi_device *device, uint32_t pin)
{
    pin = (pin & PI_GPIO_NUM_MASK);
    __global_gpio_task[pin] = NULL;
    hal_gpio_pin_set_irq_type(pin, PI_GPIO_NOTIF_NONE);
    return 0;
}

