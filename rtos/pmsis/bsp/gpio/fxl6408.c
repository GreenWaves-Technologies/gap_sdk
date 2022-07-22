/*
 * Copyright (C) 2022 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use fxl6408 file except in compliance with the License.
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
#include "bsp/gpio/fxl6408.h"

/**************/
/* Structures */
/**************/

typedef enum {
    FXL6408_REG_DEVICE_ID_CTRL      = 0x1,
    FXL6408_REG_IO_DIRECTION        = 0x3,
    FXL6408_REG_OUTPUT_STATE        = 0x5,
    FXL6408_REG_OUTPUT_HIGHZ        = 0x7,
    FXL6408_REG_INPUT_DEFAULT_STATE = 0x9,
    FXL6408_REG_PULL_ENABLE         = 0xB,
    FXL6408_REG_PULL_UP_DOWN        = 0xD,
    FXL6408_REG_INPUT_STATUS        = 0xF,
    FXL6408_REG_INTERRUPT_MASK      = 0x11,
    FXL6408_REG_INTERRUPT_STATUS    = 0x13,
} fxl6408_register_e;

/* values for the IO direction register */
enum {
    __GPIO_MODE_INPUT = 0x0,
    __GPIO_MODE_OUTPUT = 0x1,
};

/* values for the output highz register */
enum {
    __GPIO_HIGHZ_DISABLED = 0x0,
    __GPIO_HIGHZ_ENABLED = 0x1,
};

/* values for the output value register */
enum {
    __GPIO_OUTPUT_LOW = 0x0,
    __GPIO_OUTPUT_HIGH = 0x1,
};

/* values for the interrupt mask register */
enum {
    __GPIO_INTERRUPT_ENABLED = 0x0,
    __GPIO_INTERRUPT_DISABLED = 0x1,
};

/* values for the input default state register */
enum {
    __GPIO_TRIGGER_RISING = 0x0,
    __GPIO_TRIGGER_FALLING = 0x1,
};

/* values for the pull enable register */
enum {
    __GPIO_PULL_DISABLED = 0x0,
    __GPIO_PULL_ENABLED = 0x1,
};

/* values for the pull up down register */
enum {
    __GPIO_PULL_DOWN = 0x0,
    __GPIO_PULL_UP = 0x1,
};

typedef struct {
    pi_device_t i2c;
    pi_device_t gpio_irq;

    /* store device registers value locally to avoid reading and writing back */
    uint8_t gpio_dir;
    uint8_t gpio_value;
    uint8_t gpio_high_z;
    uint8_t gpio_trigger;
    uint8_t gpio_pull_enable;
    uint8_t gpio_pull_updown;
    uint8_t gpio_interrupt_mask;

    /* gpio irq status related */
    pi_task_t gpio_irq_cb;
    pi_task_t i2c_interrupt_status_cb;
    /* this value will be set by the i2c write read async */
    uint8_t i2c_interrupt_status_payload; //1-byte payload
    uint8_t i2c_interrupt_status;
    pi_task_t *irq_tasks[8];
} fxl6408_t;

/********************/
/* Static functions */
/********************/

static int __pi_fxl6408_reg_write(pi_device_t *dev, fxl6408_register_e addr,
        uint8_t value)
{
    uint8_t buffer[2] = { addr, value };
    if (pi_i2c_write(dev, buffer, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP))
    {
        return -1;
    }

    return 0;
}


static uint8_t __pi_fxl6408_reg_read(pi_device_t *dev,
        fxl6408_register_e addr)
{
    uint8_t result;
    pi_i2c_write_read(dev, &addr, &result, 1, 1);
    return result;
}


static int __pi_fxl6408_reset(fxl6408_t *fxl6408)
{
    // To reset the IO expander, just make sure it does not drive gpio outputs
    // Set all GPIO to input
    fxl6408->gpio_dir = 0x00;
    if (__pi_fxl6408_reg_write(&fxl6408->i2c,
                FXL6408_REG_IO_DIRECTION, fxl6408->gpio_dir)) {
        return -1;
    }

    // Set all GPIO output value to 0
    fxl6408->gpio_value = 0x00;
    if (__pi_fxl6408_reg_write(&fxl6408->i2c,
                FXL6408_REG_OUTPUT_STATE, fxl6408->gpio_value)) {
        return -1;
    }

    // Set all GPIO to high-Z
    fxl6408->gpio_high_z = 0xFF;
    if (__pi_fxl6408_reg_write(&fxl6408->i2c,
                FXL6408_REG_OUTPUT_HIGHZ, fxl6408->gpio_high_z)) {
        return -1;
    }

    return 0;
}

static void __attribute__((noinline))
__i2c_interrupt_status_cb(void *arg)
{
    fxl6408_t* fxl6408 = (fxl6408_t *) arg;
    // retrieve the status of the interrupt and
    // schedule corresponding pi_task

    uint8_t irq_status = fxl6408->i2c_interrupt_status;
    for (int i = 0; i < 8; i++) {
        if (irq_status & (1 << i)) {
            /* irq has been triggered */
            if (fxl6408->irq_tasks[i] != NULL) {
                /* schedule the task */
                pi_task_push(fxl6408->irq_tasks[i]);
            }
        }
    }
}

static void __attribute__((noinline))
__gpio_irq_cb(void* arg)
{
    fxl6408_t* fxl6408 = (fxl6408_t *) arg;
    // read the interrupt status (async call) will call another callback.
    pi_task_callback(&fxl6408->i2c_interrupt_status_cb,
            __i2c_interrupt_status_cb, (void*) fxl6408);
    fxl6408->i2c_interrupt_status_payload = FXL6408_REG_INTERRUPT_STATUS;
    pi_i2c_write_read_async(&fxl6408->i2c,
            &fxl6408->i2c_interrupt_status_payload,
            &fxl6408->i2c_interrupt_status,
            1, 1,
            &fxl6408->i2c_interrupt_status_cb);
}

/*****************/
/* API Functions */
/*****************/

int pi_fxl6408_open(pi_device_t *device)
{
    struct pi_fxl6408_conf *conf = (struct pi_fxl6408_conf *) device->config;


    fxl6408_t *fxl6408 = (fxl6408_t *) pmsis_l2_malloc(sizeof(fxl6408_t));
    if (fxl6408 == NULL)
    {
        return -1;
    }

    device->data = (void *) fxl6408;

    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 100000;
    pi_i2c_conf_set_slave_addr(&i2c_conf, 0x86, 0);

    pi_open_from_conf(&fxl6408->i2c, &i2c_conf);
    if (pi_i2c_open(&fxl6408->i2c)) goto error;


    /* Reset the IO expander to a known state in case, in case it was kept on
     * after chip reset */
    if (__pi_fxl6408_reset(fxl6408)) goto error2;


    /* initialize gpio input irq tasks */
    for (int i = 0; i < 8; i++)
    {
        fxl6408->irq_tasks[i] = NULL;
    }

    /* initiliaze gpio irq callback */
    {
        pi_gpio_e gpio_pin = conf->interrupt_pin;
        struct pi_gpio_conf gpio_conf;

        pi_gpio_conf_init(&gpio_conf);
        gpio_conf.port = (gpio_pin & PI_GPIO_NUM_MASK) / 32;

        pi_open_from_conf(&fxl6408->gpio_irq, &gpio_conf);
        if (PI_OK != pi_gpio_open(&fxl6408->gpio_irq)) {
            goto error2;
        }

        pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_FALL;
        pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT
            | PI_GPIO_PULL_DISABLE
            | PI_GPIO_DRIVE_STRENGTH_LOW;

        pi_gpio_pin_configure(&fxl6408->gpio_irq, gpio_pin, cfg_flags);
        pi_gpio_pin_notif_configure(&fxl6408->gpio_irq, gpio_pin, irq_type);

        pi_task_callback(&fxl6408->gpio_irq_cb, __gpio_irq_cb, (void*) fxl6408);

        if (PI_OK != pi_gpio_pin_task_add(&fxl6408->gpio_irq, gpio_pin,
                    &fxl6408->gpio_irq_cb, irq_type))
        {
            goto error3;
        }
    }

    /* clears the reset int else interrupt wont trigger the int pin
     * (value is not important) */
    __pi_fxl6408_reg_read(&fxl6408->i2c, FXL6408_REG_DEVICE_ID_CTRL);
    /* clears the interrupt status (value is not important) */
    __pi_fxl6408_reg_read(&fxl6408->i2c, FXL6408_REG_INTERRUPT_STATUS);

    return 0;

error3:
    //pi_gpio_close(&fxl6408->gpio_irq); //not implemented on pulpos2 ?
error2:
    pi_i2c_close(&fxl6408->i2c);
error:
    pmsis_l2_malloc_free(fxl6408, sizeof(fxl6408_t));
    return -2;
}

int pi_fxl6408_gpio_set(pi_device_t *device, pi_fxl6408_gpio_conf_t *gpio_conf)
{
    if (NULL == device || NULL == device->data || NULL == gpio_conf)
    {
        return PI_ERR_INVALID_ARG;
    }

    fxl6408_t *fxl6408 = (fxl6408_t *)device->data;

    if (gpio_conf->direction == FXL6408_GPIO_DIR_OUTPUT)
    {
        /* pin is in output mode, only set registers that have an effect */
        fxl6408->gpio_dir = __BITINSERT_R(fxl6408->gpio_dir,
                __GPIO_MODE_OUTPUT, 1, gpio_conf->id);
        if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_IO_DIRECTION, fxl6408->gpio_dir))
        {
            return PI_ERR_INVALID_STATE;
        }

        if (gpio_conf->output_state == FXL6408_GPIO_OUTPUT_STATE_DISABLED)
        {
            fxl6408->gpio_high_z = __BITINSERT_R(fxl6408->gpio_high_z,
                    __GPIO_HIGHZ_ENABLED, 1, gpio_conf->id);
        }
        else {
            fxl6408->gpio_high_z = __BITINSERT_R(fxl6408->gpio_high_z,
                    __GPIO_HIGHZ_DISABLED, 1, gpio_conf->id);

            if (gpio_conf->output_state == FXL6408_GPIO_OUTPUT_STATE_LOW)
            {
                fxl6408->gpio_value = __BITINSERT_R(fxl6408->gpio_value,
                        __GPIO_OUTPUT_LOW, 1, gpio_conf->id);
            }
            else
            {
                fxl6408->gpio_value = __BITINSERT_R(fxl6408->gpio_value,
                        __GPIO_OUTPUT_HIGH, 1, gpio_conf->id);
            }

            if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_OUTPUT_STATE, fxl6408->gpio_value))
            {
                return PI_ERR_INVALID_STATE;
            }
        }
        if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_OUTPUT_HIGHZ, fxl6408->gpio_high_z))
        {
            return PI_ERR_INVALID_STATE;
        }
    }
    else
    {
        /* pin is in input mode, only set registers that have an effect */
        fxl6408->gpio_dir = __BITINSERT_R(fxl6408->gpio_dir,
                __GPIO_MODE_INPUT, 1, gpio_conf->id);
        if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_IO_DIRECTION, fxl6408->gpio_dir))
        {
            return PI_ERR_INVALID_STATE;
        }

        // input trigger => (disabled, rising, falling)
        if (gpio_conf->input_trigger == FXL6408_GPIO_INPUT_TRIGGER_DISABLED)
        {
            fxl6408->gpio_interrupt_mask = __BITINSERT_R(fxl6408->gpio_interrupt_mask,
                    __GPIO_INTERRUPT_DISABLED, 1, gpio_conf->id);
        }
        else {
            fxl6408->gpio_interrupt_mask = __BITINSERT_R(fxl6408->gpio_interrupt_mask,
                    __GPIO_INTERRUPT_ENABLED, 1, gpio_conf->id);

            if (gpio_conf->input_trigger == FXL6408_GPIO_INPUT_TRIGGER_FALLING)
            {
                fxl6408->gpio_trigger = __BITINSERT_R(fxl6408->gpio_trigger,
                        __GPIO_TRIGGER_FALLING, 1, gpio_conf->id);
            }
            else
            {
                fxl6408->gpio_trigger = __BITINSERT_R(fxl6408->gpio_trigger,
                        __GPIO_TRIGGER_RISING, 1, gpio_conf->id);
            }

            if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_INPUT_DEFAULT_STATE,
                        fxl6408->gpio_trigger))
            {
                return PI_ERR_INVALID_STATE;
            }


            /* set the irq task */
            if (NULL != gpio_conf->irq_task)
            {
                fxl6408->irq_tasks[gpio_conf->id] = gpio_conf->irq_task;
            }
        }
        if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_INTERRUPT_MASK,
                    fxl6408->gpio_interrupt_mask))
        {
            return PI_ERR_INVALID_STATE;
        }

        // pull state => (disabled, pull-up, pull-down)
        if (gpio_conf->pull_state == FXL6408_GPIO_PULL_STATE_DISABLED)
        {
            fxl6408->gpio_pull_enable = __BITINSERT_R(fxl6408->gpio_pull_enable,
                    __GPIO_PULL_DISABLED, 1, gpio_conf->id);
        }
        else {
            fxl6408->gpio_pull_enable = __BITINSERT_R(fxl6408->gpio_pull_enable,
                    __GPIO_PULL_ENABLED, 1, gpio_conf->id);

            if (gpio_conf->pull_state == FXL6408_GPIO_PULL_STATE_DOWN)
            {
                fxl6408->gpio_pull_updown = __BITINSERT_R(fxl6408->gpio_pull_updown,
                        __GPIO_PULL_DOWN, 1, gpio_conf->id);
            }
            else
            {
                fxl6408->gpio_pull_updown = __BITINSERT_R(fxl6408->gpio_pull_updown,
                        __GPIO_PULL_UP, 1, gpio_conf->id);
            }

            if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_PULL_UP_DOWN,
                        fxl6408->gpio_pull_updown))
            {
                return PI_ERR_INVALID_STATE;
            }
        }
        if (__pi_fxl6408_reg_write(&fxl6408->i2c, FXL6408_REG_PULL_ENABLE,
                    fxl6408->gpio_pull_enable))
        {
            return PI_ERR_INVALID_STATE;
        }
    }

    return PI_OK;
}

int pi_fxl6408_input_status_get(pi_device_t *device, uint8_t *input_status)
{
    if (NULL == device || NULL == device->data || NULL == input_status)
    {
        return PI_ERR_INVALID_ARG;
    }

    fxl6408_t *fxl6408 = (fxl6408_t *)device->data;
    *input_status = __pi_fxl6408_reg_read(&fxl6408->i2c, FXL6408_REG_INPUT_STATUS);

    return PI_OK;
}

int pi_fxl6408_interrupt_status_get(pi_device_t *device, uint8_t *interrupt_status)
{
    if (NULL == device || NULL == device->data || NULL == interrupt_status)
    {
        return PI_ERR_INVALID_ARG;
    }

    fxl6408_t *fxl6408 = (fxl6408_t *)device->data;
    *interrupt_status = __pi_fxl6408_reg_read(&fxl6408->i2c, FXL6408_REG_INTERRUPT_STATUS);

    return PI_OK;
}


void pi_fxl6408_close(pi_device_t *device)
{
    fxl6408_t *fxl6408 = (fxl6408_t *)device->data;
    // Make sure it is not driving anymore any gpio output
    __pi_fxl6408_reset(fxl6408);
    pi_i2c_close(&fxl6408->i2c);
    pmsis_l2_malloc_free(fxl6408, sizeof(fxl6408_t));
}


void pi_fxl6408_conf_init(struct pi_fxl6408_conf *conf)
{
    conf->i2c_itf = 0;
    conf->interrupt_pin = 0;
}

void pi_fxl6408_gpio_conf_init(pi_fxl6408_gpio_conf_t *gpio_conf)
{
    if (NULL == gpio_conf)
    {
        return;
    }

    gpio_conf->id = 0;

    /* set the pin as output high impedance */
    gpio_conf->direction = FXL6408_GPIO_DIR_OUTPUT;
    gpio_conf->output_state = FXL6408_GPIO_OUTPUT_STATE_DISABLED;

    /* no effect */
    gpio_conf->input_trigger = FXL6408_GPIO_INPUT_TRIGGER_DISABLED;
    gpio_conf->pull_state = FXL6408_GPIO_PULL_STATE_DISABLED;

    gpio_conf->irq_task = NULL;
}
