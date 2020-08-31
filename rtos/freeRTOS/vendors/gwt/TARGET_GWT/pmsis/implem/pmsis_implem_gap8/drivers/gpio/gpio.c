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

#include "pmsis/chips/gap8/gpio.h"
#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct gpio_itf_data_s
{
    uint32_t device_id;                                   /*!< Referred in pi_gpio_conf as port. */
    uint32_t nb_open;                                     /*!< Number of open. */
    uint32_t irq_status;                                  /*!< GPIO pin mask of IRQ status. */
    uint32_t input_mask;                                  /*!< GPIO mask of configured pins. */
    uint32_t output_mask;                                 /*!< GPIO mask of configured pins. */
    pi_gpio_callback_t *cb;                               /*!< User callbacks. */
    pi_task_t *event_task[ARCHI_GPIO_NB_GPIO_PER_DEVICE]; /*!< Callback associated to each GPIO pins. */
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static struct gpio_itf_data_s *g_gpio_itf_data[ARCHI_GPIO_NB_DEVICE] = {NULL};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

#if defined(TRACE_GPIO)
/* Print gpio info. */
void pi_gpio_print();
#endif  /* TRACE_GPIO */

/* IRQ Handler. */
static void __pi_gpio_handler(void *arg);

static int32_t __pi_gpio_callback_add(struct gpio_itf_data_s *data,
                                      pi_gpio_callback_t *cb);

static int32_t __pi_gpio_callback_remove(struct gpio_itf_data_s *data,
                                         pi_gpio_callback_t *cb);

static void __pi_gpio_callback_fire(struct gpio_itf_data_s *data,
                                    pi_gpio_callback_t *cb);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

#if defined(TRACE_GPIO)
void pi_gpio_print(void)
{
    printf("GpioEn : %x\n", gpio_gpioen_get());
    printf("Paddir | Padin | Padout : %lx %lx %lx\n",
           gpio_paddir_get(), gpio_padin_get(), gpio_padout_get());
    printf("IRQ_EN: %lx | IRQ_Status: %lx | IRQ_Type : %lx %lx\nPadcfg:\n",
           gpio_inten_get(), gpio_intstatus_get(), gpio_inttype_get(0), gpio_inttype_get(1));
    for (uint32_t reg = 0; reg < (uint32_t) ARCHI_GPIO_NB_PADCFG_REG; reg += 4)
    {
        printf("%lx %lx %lx %lx\n",
               gpio_padcfg_get(reg + 0), gpio_padcfg_get(reg + 1),
               gpio_padcfg_get(reg + 2), gpio_padcfg_get(reg + 3));
    }
}
#endif  /* TRACE_GPIO */


static int32_t __pi_gpio_callback_add(struct gpio_itf_data_s *data,
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

static int32_t __pi_gpio_callback_remove(struct gpio_itf_data_s *data,
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

static void __pi_gpio_callback_fire(struct gpio_itf_data_s *data,
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

static void gpio_handler(void *arg)
{
    (void) arg;

    /* Retrieve IRQ status from GPIO. Handle task if needed. */
    struct gpio_itf_data_s *gpio = g_gpio_itf_data[0];
    gpio->irq_status = hal_gpio_irq_status_get();
    //GPIO_TRACE("GPIO IRQ status=%lx\n", gpio->irq_status);

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
                    __pi_irq_handle_end_of_task(task);
                }
            }
            irq_mask = irq_mask >> 1;
            pin++;
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
    struct gpio_itf_data_s *driver_data = g_gpio_itf_data[conf->port];
    if (driver_data == NULL)
    {
        GPIO_TRACE("Device id=%ld opening for first time.\n", conf->port);
        driver_data = (struct gpio_itf_data_s *) pi_l2_malloc(sizeof(struct gpio_itf_data_s));
        if (driver_data == NULL)
        {
            GPIO_TRACE_ERR("Error allocating GPIO driver data.\n");
            return -11;
        }
        driver_data->device_id = conf->port;
        driver_data->nb_open = 1;
        driver_data->irq_status = 0;
        driver_data->input_mask = 0;
        driver_data->output_mask = 0;
        driver_data->cb = NULL;
        for (uint32_t i = 0; i < (uint32_t) ARCHI_GPIO_NB_GPIO_PER_DEVICE; i++)
        {
            driver_data->event_task[i] = NULL;
        }

        /* Set handler. */
        pi_fc_event_handler_set(SOC_EVENT_GPIO, gpio_handler);
        /* Enable SOC events propagation to FC. */
        hal_soc_eu_set_fc_mask(SOC_EVENT_GPIO);
        GPIO_TRACE("GPIO(%ld) enabling SOC events propagation, event = %d.\n",
                   driver_data->device_id, SOC_EVENT_GPIO);
        g_gpio_itf_data[driver_data->device_id] = driver_data;
        device->data = (void *) g_gpio_itf_data[driver_data->device_id];
    }
    else
    {
        driver_data->nb_open++;
        device->data = (void *) g_gpio_itf_data[driver_data->device_id];
        GPIO_TRACE("Device id=%ld already opened, open=%ld.\n",
                   driver_data->device_id, driver_data->nb_open);
    }
    return 0;
}

void pi_gpio_close(struct pi_device *device)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    data->nb_open--;
    GPIO_TRACE("Closing Device id=%ld, open=%ld.\n", device_id, data->nb_open);
    if (data->nb_open == 0)
    {
        /* Clear handler. */
        pi_fc_event_handler_clear(SOC_EVENT_GPIO);
        /* Disable SOC events propagation to FC. */
        hal_soc_eu_clear_fc_mask(SOC_EVENT_GPIO);

        /* Disable input GPIOs. */
        hal_gpio_enable(data->input_mask, 0);

        /* Free allocated structure. */
        pi_l2_free(g_gpio_itf_data[device_id], sizeof(struct gpio_itf_data_s));
    }
    device->data = NULL;
}

int pi_gpio_pin_configure(struct pi_device *device, pi_gpio_e gpio,
                          pi_gpio_flags_e flags)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    if (gpio & PI_GPIO_IS_GPIO_MASK)
    {
        pi_pad_e pad = ((gpio & PI_GPIO_PAD_MASK) >> PI_GPIO_PAD_SHIFT);
        /* Setup first pad for GPIO. */
        pi_pad_set_function(pad, PI_PAD_FUNC1);
    }

    uint32_t gpio_pin = (gpio & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    uint8_t pe = ((flags & PI_GPIO_PULL_ENABLE) >> PI_GPIO_PULL_OFFSET);
    uint8_t ds = ((flags & PI_GPIO_DRIVE_STRENGTH_HIGH) >> PI_GPIO_DRIVE_OFFSET);
    uint8_t dir = ((flags & PI_GPIO_OUTPUT) >> PI_GPIO_MODE_OFFSET);
    if (dir)
    {
        data->input_mask &= ~mask;
        data->output_mask |= mask;
    }
    else
    {
        data->input_mask |= mask;
        data->output_mask &= ~mask;
    }
    GPIO_TRACE("GPIO configured : INPUT=%lx  OUTPUT=%lx\n",
               data->input_mask, data->output_mask);

    hal_gpio_pin_direction_set(gpio_pin, dir);
    hal_gpio_pin_config_set(gpio_pin, pe, ds);
    hal_gpio_pin_enable(gpio_pin, !dir);

    GPIO_TRACE("GPIO[%ld, %d](%lx) configured as %s\n",
               device_id, gpio_pin, gpio, ((dir) ? "output" : "input"));
    return 0;
}

int pi_gpio_pin_write(struct pi_device *device, uint32_t pin, uint32_t value)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    if ((data->output_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as output %lx - %lx!\n",
                       gpio_pin, data->output_mask, mask);
        return -11;
    }
    GPIO_TRACE("Write Pin: %d val: %ld\n", gpio_pin, value);
    hal_gpio_pin_output_value_set(gpio_pin, (uint8_t) (value & 0xff));
    return 0;
}

int pi_gpio_pin_read(struct pi_device *device, uint32_t pin, uint32_t *value)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    if (((data->input_mask & mask) != mask) &&
        ((data->output_mask & mask) != mask))
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as in input or output %lx - %lx!\n",
                       gpio_pin, (data->input_mask | data->output_mask), mask);
        return -11;
    }

    if ((data->input_mask & mask))
    {
        *value = hal_gpio_pin_input_value_get(gpio_pin);
        GPIO_TRACE("Input read Pin: %d val: %ld\n", gpio_pin, *value);
    }
    else
    {
        *value = hal_gpio_pin_output_value_get(gpio_pin);
        GPIO_TRACE("Output read Pin: %d val: %ld\n", gpio_pin, *value);
    }
    return 0;
}

void pi_gpio_pin_notif_configure(struct pi_device *device, uint32_t pin,
                                 pi_gpio_notif_e irq_type)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    if ((data->input_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as input %lx - %lx!\n",
                       gpio_pin, data->input_mask, mask);
        return;
    }

    if (irq_type == PI_GPIO_NOTIF_NONE)
    {
        hal_gpio_pin_irq_set(gpio_pin, 0);
    }
    else
    {
        hal_gpio_pin_irq_set(gpio_pin, 1);
        hal_gpio_pin_irq_type_set(gpio_pin, irq_type);
    }
    GPIO_TRACE("Configure Pin: %d IRQ: %x\n", gpio_pin, irq_type);
}

void pi_gpio_pin_notif_clear(struct pi_device *device, uint32_t pin)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);
    GPIO_TRACE("Clear IRQ Pin: %d\n", gpio_pin);
    data->irq_status &= ~mask;
}

int pi_gpio_pin_notif_get(struct pi_device *device, uint32_t pin)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t status = data->irq_status;
    return ((status >> gpio_pin) & 0x1);
}

int pi_gpio_pin_task_add(struct pi_device *device, uint32_t pin, pi_task_t *task,
                         pi_gpio_notif_e irq_type)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);

    if ((data->input_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error : pin %d is not configured as input %lx - %lx!\n",
                       gpio_pin, data->input_mask, mask);
        return -11;
    }

    data->event_task[gpio_pin] = task;
    if (irq_type == PI_GPIO_NOTIF_NONE)
    {
        hal_gpio_pin_irq_set(gpio_pin, 0);
    }
    else
    {
        hal_gpio_pin_irq_set(gpio_pin, 1);
        hal_gpio_pin_irq_type_set(gpio_pin, irq_type);
    }
    GPIO_TRACE("Configure Pin: %d IRQ: %x\n", gpio_pin, irq_type);
    return 0;
}

int pi_gpio_pin_task_remove(struct pi_device *device, uint32_t pin)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t gpio_pin = (pin & ARCHI_GPIO_NB_GPIO_PER_DEVICE_MASK);
    uint32_t mask = (1 << gpio_pin);

    data->event_task[gpio_pin] = NULL;
    return 0;
}

int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask,
                           pi_gpio_flags_e flags)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint8_t pe = ((flags & PI_GPIO_PULL_ENABLE) >> PI_GPIO_PULL_OFFSET);
    uint8_t ds = ((flags & PI_GPIO_DRIVE_STRENGTH_HIGH) >> PI_GPIO_DRIVE_OFFSET);
    uint8_t dir = ((flags & PI_GPIO_OUTPUT) >> PI_GPIO_MODE_OFFSET);
    if (dir)
    {
        data->input_mask &= ~mask;
        data->output_mask |= mask;
    }
    else
    {
        data->input_mask |= mask;
        data->output_mask &= ~mask;
    }

    hal_gpio_direction_set(mask, dir);
    hal_gpio_config_set(mask, pe, ds);
    hal_gpio_enable(mask, !dir);
    return 0;
}

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    if ((data->output_mask & mask) != mask)
    {
        GPIO_TRACE_ERR("Error some pins are not configured as output %lx - %lx!\n",
                       data->output_mask, mask);
        return -11;
    }
    hal_gpio_output_value_set(mask, value);
    return 0;
}

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    uint32_t device_id = data->device_id;

    uint32_t val = 0;
    if ((data->input_mask & mask))
    {
        *value = hal_gpio_input_value_get(mask);
        GPIO_TRACE("Input read val: %ld\n", *value);
    }
    else
    {
        *value = hal_gpio_output_value_get(mask);
        GPIO_TRACE("Output read val: %ld\n", *value);
    }

    return 0;
}

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask,
                          pi_task_t *task, pi_gpio_notif_e irq_type)
{
    uint32_t gpio_mask = mask;
    uint8_t pin = 0;
    int32_t error = 0;
    while (gpio_mask)
    {
        if (gpio_mask & 0x1)
        {
            error = pi_gpio_pin_task_add(device, pin, task, irq_type);
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
    int32_t error = 0;
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
        gpio_mask = (gpio_mask >> 1);
        pin++;
    }
    return 0;
}

int pi_gpio_callback_add(struct pi_device *device, pi_gpio_callback_t *cb)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    return __pi_gpio_callback_add(data, cb);

}

int pi_gpio_callback_remove(struct pi_device *device, pi_gpio_callback_t *cb)
{
    struct gpio_itf_data_s *data = (struct gpio_itf_data_s *) device->data;
    return __pi_gpio_callback_remove(data, cb);
}
