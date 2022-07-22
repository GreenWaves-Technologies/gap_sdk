/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#include "pmsis/chips/vega/gpio.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * GPIO split.
 * Port[0]=00-31
 * Port[1]=32-63
 */
typedef struct gpio_data_s
{
    uint32_t nb_open;                                     /*!< Number of open. */
    uint32_t irq_status;                                  /*!< GPIO pin mask of IRQ status. */
    uint32_t input_mask;                                  /*!< GPIO mask of pins configured for IN. */
    uint32_t output_mask;                                 /*!< GPIO mask of pins configured for OUT. */
    pi_gpio_callback_t *cb;                               /*!< Fast callbacks. */
    pi_task_t *event_task[ARCHI_GPIO_NB_GPIO_PER_DEVICE]; /*!< Callback associated to each GPIO puns. */
    uint8_t device_id;                                    /*!< Referred in pi_gpio_conf as port. */
} gpio_data_t;

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static gpio_data_t *g_gpio_data[ARCHI_GPIO_NB_DEVICE] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

#if defined(TRACE_GPIO)
/* Print gpio info. */
void pi_gpio_print(void);
#endif  /* TRACE_GPIO */

/* IRQ handler. */
extern void __pi_gpio_irq_handler(void);
void __pi_gpio_handler(void *arg);

static int32_t __pi_gpio_callback_add(struct gpio_data_s *data,
                                      pi_gpio_callback_t *cb);

static int32_t __pi_gpio_callback_remove(struct gpio_data_s *data,
                                         pi_gpio_callback_t *cb);

static void __pi_gpio_callback_fire(struct gpio_data_s *data,
                                    pi_gpio_callback_t *cb);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

#if defined(TRACE_GPIO)
void pi_gpio_print(void)
{
    gpio_data_t *gpio = NULL;
    uint8_t pin_00 = 0, pin_31 = 0;
    uint32_t device_id = 0;
    for (uint32_t i = 0; i < (uint32_t) ARCHI_GPIO_NB_DEVICE; i++)
    {
        gpio = g_gpio_data[i];
        //if (1)
        if (gpio != NULL)
        {
            pin_00 = (uint32_t) ARCHI_GPIO_NB_GPIO_PER_DEVICE * i;
            pin_31 = pin_00 + 0x1f;
            //device_id = gpio->device_id;
            device_id = i;
            printf("GPIO %d - %d\n", pin_00, pin_31);
            printf("GPIOEN: %lx\n", hal_gpio_enable_get(device_id));
            printf("Paddir | Padin | Padout: %lx %lx %lx\n",
                   hal_gpio_direction_get(device_id),
                   hal_gpio_input_value_get(device_id),
                   hal_gpio_output_value_get(device_id));
            printf("IRQ_EN: %lx | IRQ_STATUS: %lx | IRQ_TYPE: %lx %lx\nPadcfg: \n",
                   hal_gpio_irq_get(device_id),
                   hal_gpio_irq_status_get(device_id),
                   hal_gpio_irq_type_get(device_id, 0),
                   hal_gpio_irq_type_get(device_id, 1));
            for (uint32_t reg = 0; reg < (uint32_t) ARCHI_GPIO_NB_PADCFG_REG; reg+=4)
            {
                printf("%lx %lx %lx %lx\n",
                       hal_gpio_config_get(device_id, (reg + 0)),
                       hal_gpio_config_get(device_id, (reg + 1)),
                       hal_gpio_config_get(device_id, (reg + 2)),
                       hal_gpio_config_get(device_id, (reg + 3)));
            }
        }
    }
}
#endif  /* TRACE_GPIO */


static int32_t __pi_gpio_callback_add(struct gpio_data_s *data,
                                      pi_gpio_callback_t *cb)
{
    if (cb == NULL)
    {
        GPIO_TRACE_ERR("Error : callback is NULL !\n");
        return -11;
    }

    GPIO_TRACE("Attaching callback %lx, gpio_mask=%lx\n", cb, cb->pin_mask);

    pi_gpio_callback_t *driver_cb = data->cb;
    if (driver_cb == NULL)
    {
        data->cb = cb;
        GPIO_TRACE("Attach callback %lx to head.\n", cb);
    }
    else
    {
        while (driver_cb->next != NULL)
        {
            driver_cb = driver_cb->next;
        }
        driver_cb->next = cb;
        cb->prev = driver_cb;
        GPIO_TRACE("Attach callback %lx to tail.\n", cb);
    }
    return 0;
}

static int32_t __pi_gpio_callback_remove(struct gpio_data_s *data,
                                         pi_gpio_callback_t *cb)
{
    GPIO_TRACE("Removing callback=%lx, head=%lx.\n", cb, data->cb);
    if (cb == NULL)
    {
        GPIO_TRACE_ERR("Error : callback is NULL !\n");
        return -11;
    }

    if (data->cb == NULL)
    {
        GPIO_TRACE_ERR("Error : callback list is empty !\n");
        return -12;
    }

    /* Callback at the head. */
    if (data->cb == cb)
    {
        data->cb = data->cb->next;
        data->cb->prev = NULL;
        return 0;
    }

    /* Callback in list. */
    pi_gpio_callback_t *temp_cb = data->cb;
    while ((temp_cb != cb) && (temp_cb->next != NULL))
    {
        temp_cb = temp_cb->next;
    }
    if (temp_cb != cb)
    {
        GPIO_TRACE_ERR("Error : callback=%lx not found in list.\n", cb);
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
    GPIO_TRACE("Remove callback %lx.\n", cb);
    return 0;
}

static void __pi_gpio_callback_fire(struct gpio_data_s *data,
                                    pi_gpio_callback_t *cb)
{
    uint32_t irq_mask = data->irq_status;
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

/* IRQ Handler. */
void __pi_gpio_handler(void *arg)
{
    (void) arg;

    gpio_data_t *gpio = NULL;
    uint32_t irq_mask[ARCHI_GPIO_NB_DEVICE] = {0};
    /* Retrieve IRQ status from GPIO. Handle task if needed. */
    for (uint32_t i = 0; i < (uint32_t) ARCHI_GPIO_NB_DEVICE; i++)
    {
        gpio = g_gpio_data[i];
        if (gpio != NULL)
        {
            gpio->irq_status = hal_gpio_irq_status_get(gpio->device_id);
            irq_mask[i] = gpio->irq_status & gpio->input_mask;
        }
    }
    pi_task_t *task = NULL;
    uint8_t pin = 0;
    for (uint32_t i = 0; i < (uint32_t) ARCHI_GPIO_NB_DEVICE; i++)
    {
        gpio = g_gpio_data[i];
        pin = 0;
        if ((gpio->cb != NULL) && irq_mask[i])
        {
            __pi_gpio_callback_fire(gpio, gpio->cb);
        }
        else
        {
            while (irq_mask[i])
            {
                if (irq_mask[i] & 0x1)
                {
                    task = gpio->event_task[pin];
                    if (task != NULL)
                    {
                        __pi_irq_handle_end_of_task(task);
                    }
                }
                irq_mask[i] = irq_mask[i] >> 1;
                pin++;
            }
        }
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
    struct pi_gpio_conf *conf = (struct pi_gpio_conf *) device->config;
    gpio_data_t *driver_data = g_gpio_data[conf->port];
    if (driver_data == NULL)
    {
        GPIO_TRACE("Device id=%ld opening for first time.\n", conf->port);
        driver_data = pi_fc_l1_malloc(sizeof(struct gpio_data_s));
        if (driver_data == NULL)
        {
            GPIO_TRACE_ERR("Error allocating GPIO driver data\n");
            return -11;
        }
        driver_data->device_id = conf->port;
        driver_data->nb_open = 0;
        driver_data->irq_status = 0;
        driver_data->input_mask = 0;
        driver_data->output_mask = 0;
        driver_data->cb = NULL;
        for (uint32_t i = 0; i < (uint32_t) ARCHI_GPIO_NB_GPIO_PER_DEVICE; i++)
        {
            driver_data->event_task[i] = NULL;
        }

        /* Enable GPIO IRQ and set IRQ handler. */
        #if 0
        /* Set handler. */
        pi_fc_event_handler_set(SOC_EVENT_GPIO, __pi_gpio_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_GPIO);
        GPIO_TRACE("GPIO(%ld) enabling SOC events propagation, event = %d.\n",
                   driver_data->device_id, SOC_EVENT_GPIO);
        #else
        pi_irq_mask_enable(FC_IRQ_GPIO);
        //pi_irq_handler_fast_set(FC_IRQ_GPIO, __pi_gpio_irq_handler);
        GPIO_TRACE("GPIO(%ld) enabling GPIO IRQ = %d.\n", driver_data->device_id, FC_IRQ_GPIO);
        #endif

        g_gpio_data[driver_data->device_id] = driver_data;
        device->data = (void *) g_gpio_data[driver_data->device_id];
    }
    driver_data->nb_open++;
    device->data = (void *) g_gpio_data[driver_data->device_id];
    GPIO_TRACE("GPIO device id=%d open=%ld.\n", driver_data->device_id, driver_data->nb_open);

    return 0;
}

void pi_gpio_close(struct pi_device *device)
{
    struct gpio_data_s *data = (struct gpio_data_s *) device->data;
    uint8_t device_id = data->device_id;

    data->nb_open--;
    GPIO_TRACE("Closing device id=%ld, open=%ld.\n", device_id, data->nb_open);
    if (data->nb_open == 0)
    {
        /* Disable GPIO IRQ. */
        pi_irq_mask_disable(FC_IRQ_GPIO);

        /* Disable input GPIOs. */
        hal_gpio_enable(device_id, data->input_mask, 0);

        /* Free allocated structure. */
        pi_fc_l1_free(g_gpio_data[device_id], sizeof(struct gpio_data_s));
    }
    device->data = NULL;
}


int pi_gpio_pin_configure(struct pi_device *device, pi_gpio_e gpio,
                          pi_gpio_flags_e flags)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(gpio) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return -11;
    }
    if (gpio & PI_GPIO_IS_GPIO_MASK)
    {
        pi_pad_e pad = ((gpio & PI_GPIO_PAD_MASK) >> PI_GPIO_PAD_SHIFT);
        /* Setup first pad for GPIO. */
        GPIO_TRACE("Configuring PAD_%d as GPIO.\n", pad);
        pi_pad_set_function(pad, PI_PAD_FUNC1);
    }

    uint32_t gpio_pin = (gpio & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    uint8_t pe = (flags & PI_GPIO_PULL_ENABLE) >> PI_GPIO_PULL_OFFSET;
    uint8_t ds = (flags & PI_GPIO_DRIVE_STRENGTH_HIGH) >> PI_GPIO_DRIVE_OFFSET;
    uint8_t dir = (flags & PI_GPIO_OUTPUT) >> PI_GPIO_MODE_OFFSET;
    if (dir)
    {
        device_data->input_mask &= ~mask;
        device_data->output_mask |= mask;
    }
    else
    {
        device_data->input_mask |= mask;
        device_data->output_mask &= ~mask;
    }

    hal_gpio_pin_direction_set(device_id, gpio_pin, dir);
    hal_gpio_pin_config_set(device_id, gpio_pin, pe, ds);
    hal_gpio_pin_enable(device_id, gpio_pin, !dir);

    GPIO_TRACE("GPIO[%ld, %d](%lx) configured as %s\n",
               device_id, gpio_pin, gpio, ((dir) ? "output" : "input"));
    return 0;
}

int pi_gpio_pin_write(struct pi_device *device, uint32_t pin, uint32_t value)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return -11;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    if ((device_data->output_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as output %lx - %lx!\n",
                       gpio_pin, device_data->output_mask, mask);
        return -12;
    }
    GPIO_TRACE("Write Pin: %d val: %ld\n", gpio_pin, value);
    hal_gpio_pin_output_value_set(device_id, gpio_pin, (uint8_t) value);
    return 0;
}

int pi_gpio_pin_read(struct pi_device *device, uint32_t pin, uint32_t *value)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return -11;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    #if 0
    if (((device_data->input_mask & mask) != mask) &&
        ((device_data->output_mask & mask) != mask))
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as input or output %lx - %lx!\n",
                       gpio_pin, (device_data->input_mask | device_data->output_mask), mask);
        return -12;
    }
    #endif
    if ((device_data->input_mask & mask))
    {
        *value = hal_gpio_pin_input_value_get(device_id, gpio_pin);
        GPIO_TRACE("Input read Pin: %d val: %ld\n", gpio_pin, *value);
    }
    /* else */
    /* { */
    /*     *value = hal_gpio_pin_output_value_get(device_id, gpio_pin); */
    /*     GPIO_TRACE("Output read Pin: %d val: %ld\n", gpio_pin, *value); */
    /* } */
    return 0;
}

void pi_gpio_pin_notif_configure(struct pi_device *device, uint32_t pin,
                                 pi_gpio_notif_e flags)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    if ((device_data->input_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as input %lx - %lx!\n",
                       gpio_pin, device_data->input_mask, mask);
        return;
    }

    if (flags == PI_GPIO_NOTIF_NONE)
    {
        hal_gpio_pin_irq_set(device_id, gpio_pin, 0);
    }
    else
    {
        hal_gpio_pin_irq_set(device_id, gpio_pin, 1);
        hal_gpio_pin_irq_type_set(device_id, gpio_pin, flags);
    }
    GPIO_TRACE("Configure Pin: %d IRQ: %x\n", gpio_pin, flags);
}

void pi_gpio_pin_notif_clear(struct pi_device *device, uint32_t pin)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    GPIO_TRACE("Clear IRQ Pin: %d\n", gpio_pin);
    device_data->irq_status &= ~mask;
}

int pi_gpio_pin_notif_get(struct pi_device *device, uint32_t pin)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return -11;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t status = device_data->irq_status;
    return ((status >> gpio_pin) & 0x1);
}

int pi_gpio_pin_task_add(struct pi_device *device, uint32_t pin, pi_task_t *task,
                         pi_gpio_notif_e flags)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return -11;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    if ((device_data->input_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as input %lx - %lx!\n",
                       gpio_pin, device_data->input_mask, mask);
        return -12;
    }

    device_data->event_task[gpio_pin] = task;
    /* Configure GPIO pin. */
    if (flags == PI_GPIO_NOTIF_NONE)
    {
        hal_gpio_pin_irq_set(device_id, gpio_pin, 0);
    }
    else
    {
        hal_gpio_pin_irq_set(device_id, gpio_pin, 1);
        hal_gpio_pin_irq_type_set(device_id, gpio_pin, flags);
    }
    GPIO_TRACE("Configure Pin: %d IRQ: %x\n", gpio_pin, flags);
    return 0;
}

int pi_gpio_pin_task_remove(struct pi_device *device, uint32_t pin)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if (ARCHI_GPIO_DEVICE_ID(pin) != device_id)
    {
        GPIO_TRACE_ERR("Error mismatch device and GPIO pin !\n");
        return -11;
    }
    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    device_data->event_task[gpio_pin] = NULL;
    return 0;
}

int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask,
                           pi_gpio_flags_e flags)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    uint8_t pe = (flags & PI_GPIO_PULL_ENABLE) >> PI_GPIO_PULL_OFFSET;
    uint8_t ds = (flags & PI_GPIO_DRIVE_STRENGTH_HIGH) >> PI_GPIO_DRIVE_OFFSET;
    uint8_t dir = (flags & PI_GPIO_OUTPUT) >> PI_GPIO_MODE_OFFSET;
    if (dir)
    {
        device_data->input_mask &= ~mask;
        device_data->output_mask |= mask;
    }
    else
    {
        device_data->input_mask |= mask;
        device_data->output_mask &= ~mask;
    }

    hal_gpio_direction_set(device_id, mask, dir);
    hal_gpio_config_set(device_id, mask, pe, ds);
    hal_gpio_enable(device_id, mask, !dir);
    return 0;
}

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if ((device_data->output_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error some pins are not configured as output %lx - %lx!\n",
                       device_data->output_mask, mask);
        return -11;
    }
    hal_gpio_output_value_set(device_id, mask, value);
    return 0;
}

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value)
{
    gpio_data_t *device_data = (gpio_data_t *) device->data;
    uint32_t device_id = device_data->device_id;
    if ((device_data->input_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error some pins are not configured %lx - %lx!\n",
                       device_data->input_mask, mask);
        return -11;
    }
    uint32_t val = hal_gpio_input_value_get(device_id);
    *value = val & mask;
    return 0;
}

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask,
                          pi_task_t *task, pi_gpio_notif_e flags)
{
    uint32_t gpio_mask = mask;
    uint8_t pin = 0;
    int error = 0;
    while (gpio_mask)
    {
        if (gpio_mask & 0x1)
        {
            error = pi_gpio_pin_task_add(device, pin, task, flags);
            if (error)
            {
                GPIO_TRACE_ERR("Error configuring pin %d\n", pin);
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
    int error = 0;
    while (gpio_mask)
    {
        if (gpio_mask & 0x1)
        {
            error = pi_gpio_pin_task_remove(device, pin);
            if (error)
            {
                GPIO_TRACE_ERR("Error configuring pin %d\n", pin);
                return -11;
            }
        }
        gpio_mask = gpio_mask >> 1;
        pin++;
    }
    return 0;
}

int pi_gpio_callback_add(struct pi_device *device, pi_gpio_callback_t *cb)
{
    struct gpio_data_s *data = (struct gpio_data_s *) device->data;
    return __pi_gpio_callback_add(data, cb);

}

int pi_gpio_callback_remove(struct pi_device *device, pi_gpio_callback_t *cb)
{
    struct gpio_data_s *data = (struct gpio_data_s *) device->data;
    return __pi_gpio_callback_remove(data, cb);
}
