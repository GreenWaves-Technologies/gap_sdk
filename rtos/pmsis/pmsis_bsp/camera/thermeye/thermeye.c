/*
 * Copyright (C) 2019 GreenWaves Technologies
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
#include "bsp/bsp.h"
#include "bsp/camera/thermeye.h"
#include "thermeye_defines.h"

typedef struct
{
    uint16_t addr;
    uint8_t value;
} __attribute__((packed)) i2c_req_t;

typedef struct
{
    struct pi_thermeye_conf conf;
    struct pi_device cpi_device;
    struct pi_device i2c_device;
    struct pi_device pwm_device;
    struct pi_device gpio_device;
    i2c_req_t i2c_req;
} thermeye_t;


static uint8_t __pi_thermeye_reg_read(thermeye_t *thermeye, uint16_t addr)
{
    /*
     * addr = 0x_X1_X2_X3_X4.
     * 16 bits address : MSB(8) | LSB(8).
     * => req.addr = X3_X4_X1_X2
     */
    thermeye->i2c_req.addr = ((addr  << 8) | (addr >> 8));
    pi_i2c_write(&(thermeye->i2c_device), (uint8_t *) &(thermeye->i2c_req.addr), 2, PI_I2C_XFER_NO_STOP);
    pi_i2c_read(&(thermeye->i2c_device), (uint8_t *) &(thermeye->i2c_req.value), 1, PI_I2C_XFER_STOP);
    return thermeye->i2c_req.value;
}

static void __pi_thermeye_reg_write(thermeye_t *thermeye, uint16_t addr, uint8_t value)
{
    thermeye->i2c_req.addr = ((addr  << 8) | (addr >> 8));
    thermeye->i2c_req.value = value;
    pi_i2c_write(&(thermeye->i2c_device), (uint8_t *) &(thermeye->i2c_req), 3, PI_I2C_XFER_STOP);
}

static void __pi_thermeye_start(thermeye_t *thermeye)
{
    //printf("Sensor On\n");
    /* Start Master Clock. Need GPIO, pin E12 on sensor. */
    /* MC frequency 14 * I2C SCL frequency. Set when opened. */
    {
        /* Set NRST. GPIO_IR_NRST, pin M5 on sensor. */
        pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_reset, 0);

        /* Enable power to sensor. */
        pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_power, 1);
        pi_time_wait_us(1500);

        /* Start sensor MC -> PWM. */
        /* No need to start again and again. */
        //pi_pwm_timer_start(&(thermeye->pwm_device));

        /* Release NRST. */
        pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_reset, 1);

        /* Wait some time to stabilise power on. */
        pi_time_wait_us(1500);
        hal_compiler_barrier();
    }

    /* Read read-only register using I2C. */
    {
        uint8_t value[3] = {0};
        do
        {
            value[0] = __pi_thermeye_reg_read(thermeye, (uint16_t) READ_ONLY_A);
            value[1] = __pi_thermeye_reg_read(thermeye, (uint16_t) READ_ONLY_B);
            value[2] = __pi_thermeye_reg_read(thermeye, (uint16_t) READ_ONLY_C);
        } while ((value[0] != 0x55) || (value[1] != 0xC6) || (value[2] != 0xC5));
        //printf("Read_only access : %x %x %x\n", value[0], value[1], value[2]);
    }

    /* Set sensor in digital mode. */
    hal_compiler_barrier();
    __pi_thermeye_reg_write(thermeye, (uint16_t) INT_CAPA_LD, 0x01);
    __pi_thermeye_reg_write(thermeye, (uint16_t) EXT_CAPA_LD, 0x02);
    __pi_thermeye_reg_write(thermeye, (uint16_t) ADC_BIAS, 0x00);
    __pi_thermeye_reg_write(thermeye, (uint16_t) EXT_POLAR, 0x08);
    __pi_thermeye_reg_write(thermeye, (uint16_t) DACGFID, 0xBD);
    __pi_thermeye_reg_write(thermeye, (uint16_t) DACGSK_A, 0x01);
    __pi_thermeye_reg_write(thermeye, (uint16_t) DACGSK_B, 0x30);
    __pi_thermeye_reg_write(thermeye, (uint16_t) VTEMP_ADC, 0x60);
    __pi_thermeye_reg_write(thermeye, (uint16_t) DACVBUS, 0x00);
    __pi_thermeye_reg_write(thermeye, (uint16_t) INTEGRATION, 0x25);
    __pi_thermeye_reg_write(thermeye, (uint16_t) GAIN_IMAGE, 0x53);
    /* Set Configs. */
    __pi_thermeye_reg_write(thermeye, (uint16_t) CONFIG, 0x01);
    __pi_thermeye_reg_write(thermeye, (uint16_t) CONFIG, 0x41);
    __pi_thermeye_reg_write(thermeye, (uint16_t) 0x005D, ((1 << 2) | (1 << 0)));
    //__pi_thermeye_reg_write(thermeye, (uint16_t) CONFIG, 0x01);

    hal_compiler_barrier();
    /* Wait capa preload. Here 19ms. */
    pi_time_wait_us(19000);

    /* Disable external capa preload. */
    __pi_thermeye_reg_write(thermeye, (uint16_t) EXT_CAPA_LD, 0x00);
}

static uint32_t __pi_thermeye_trigger_snapshot(thermeye_t *thermeye)
{
    uint32_t feedback = 0;
    /* Switch on led during image capture. */
    //pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);

    __pi_thermeye_reg_write(thermeye, (uint16_t) CONFIG, 0x45);
    //feedback = __pi_thermeye_reg_read(thermeye, (uint16_t) CONFIG);
    return feedback;
}

static void __pi_thermeye_stop(thermeye_t *thermeye)
{
    /* Switch off led. */
    //pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);

    /* Reset NRST. GPIO_IR_NRST, pin M5 on sensor. */
    pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_reset, 0);
    pi_time_wait_us(2);

    /* Disable power to sensor. */
    pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_power, 0);
    //printf("Sensor Off\n");
}

static void __pi_thermeye_on(thermeye_t *thermeye)
{
    /* Enable power to sensor. */
    pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_power, 1);
    pi_time_wait_us(100);
}

static void __pi_thermeye_off(thermeye_t *thermeye)
{
    /* Disable power to sensor. */
    pi_gpio_pin_write(&(thermeye->gpio_device), thermeye->conf.gpio_power, 0);
    pi_time_wait_us(100);
}

static int32_t __pi_thermeye_open(struct pi_device *device)
{
    struct pi_thermeye_conf *conf = (struct pi_thermeye_conf *) device->config;

    /* Setup board. Init default padframes. */
    bsp_thermeye_open(conf);

    /* Switch on user led while config. */
    //pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);

    thermeye_t *thermeye = (thermeye_t *) pi_l2_malloc(sizeof(thermeye_t));
    if (thermeye == NULL)
    {
        return -1;
    }

    memcpy(&(thermeye->conf), conf, sizeof(struct pi_thermeye_conf));
    device->data = (void *) thermeye;

    //printf("ThermEye: opening CPI(%d), I2C(%d), PWM(%d, %d), GPIO_Power %x, GPIO_Reset %x\n",
    //       conf->cpi_id, conf->i2c_id, conf->pwm_id, conf->pwm_channel,
    //       conf->gpio_power, conf->gpio_reset);

    /* PWM open. */
    #define PWM_FQCY_KHz    (700)
    struct pi_pwm_conf pwm_conf;
    pi_pwm_conf_init(&pwm_conf);
    pwm_conf.pwm_id = conf->pwm_id;
    pwm_conf.ch_id = conf->pwm_channel;
    pwm_conf.timer_conf &= ~PI_PWM_CLKSEL_REFCLK_32K;
    pwm_conf.timer_conf |= PI_PWM_CLKSEL_FLL;
    pi_open_from_conf(&(thermeye->pwm_device), &pwm_conf);
    if (pi_pwm_open(&(thermeye->pwm_device)))
    {
        printf("PWM open failed !\n");
        pi_l2_free(thermeye, sizeof(struct pi_thermeye_conf));
        return -2;
    }
    pi_pwm_duty_cycle_set(&(thermeye->pwm_device), PWM_FQCY_KHz * 1000, 50);

    pi_pwm_timer_start(&(thermeye->pwm_device));

    /* I2C open. */
    /* F_pwm = 14 * F_i2c, at least. */
    #define  I2C0_FQCY_KHZ   (PWM_FQCY_KHz  / 16)
    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.cs = ((uint8_t) I2CAD) << 1;
    i2c_conf.itf = conf->i2c_id;
    i2c_conf.max_baudrate = I2C0_FQCY_KHZ * 1000;
    pi_open_from_conf(&(thermeye->i2c_device), &i2c_conf);
    if (pi_i2c_open(&(thermeye->i2c_device)))
    {
        printf("I2C open failed !\n");
        pi_pwm_close(&(thermeye->i2c_device));
        pi_l2_free(thermeye, sizeof(struct pi_thermeye_conf));
        return -3;
    }

    //printf("PWM: %d kHz I2C: %d kHz\n", PWM_FQCY_KHz, I2C0_FQCY_KHZ);

    /* CPI open. */
    struct pi_cpi_conf cpi_conf = {0};
    pi_cpi_conf_init(&cpi_conf);
    cpi_conf.itf = conf->cpi_id;
    pi_open_from_conf(&(thermeye->cpi_device), &cpi_conf);
    if (pi_cpi_open(&(thermeye->cpi_device)))
    {
        printf("CPI_open failed !\n");
        pi_pwm_close(&(thermeye->pwm_device));
        pi_i2c_close(&(thermeye->i2c_device));
        pi_l2_free(thermeye, sizeof(struct pi_thermeye_conf));
        return -4;
    }
    pi_cpi_set_format(&(thermeye->cpi_device), conf->cpi_format);

    /* GPIO open. */
    struct pi_gpio_conf gpio_conf;
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&(thermeye->gpio_device), &gpio_conf);
    if (pi_gpio_open(&(thermeye->gpio_device)))
    {
        printf("GPIO_open failed !\n");
        pi_pwm_close(&(thermeye->pwm_device));
        pi_i2c_close(&(thermeye->i2c_device));
        pi_cpi_close(&(thermeye->cpi_device));
        pi_l2_free(thermeye, sizeof(struct pi_thermeye_conf));
        return -5;
    }

    /* GPIO_IR_NRST, pin M5 on sensor. */
    pi_gpio_pin_configure(&(thermeye->gpio_device), conf->gpio_reset, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);

    /* Power to sensor. */
    pi_gpio_pin_configure(&(thermeye->gpio_device), conf->gpio_power, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);

    /* Init done, switch off led. */
    //pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
    return 0;
}

static void __pi_thermeye_close(struct pi_device *device)
{
    thermeye_t *thermeye = (thermeye_t *) device->data;

    /* Close PWM. */
    pi_pwm_close(&(thermeye->pwm_device));

    /* Close I2C. */
    pi_i2c_close(&(thermeye->i2c_device));

    /* Close CPI. */
    pi_cpi_close(&(thermeye->cpi_device));

    /* Free thermeye_t struct. */
    pi_l2_free(thermeye, sizeof(thermeye_t));
}

static int32_t __pi_thermeye_control(struct pi_device *device, pi_camera_cmd_e cmd,
                                     void *arg)
{
    thermeye_t *thermeye = (thermeye_t *) device->data;
    switch (cmd)
    {
    case PI_CAMERA_CMD_ON:
        __pi_thermeye_on(thermeye);
        break;

    case PI_CAMERA_CMD_OFF:
        __pi_thermeye_off(thermeye);
        break;

    case PI_CAMERA_CMD_START:
        pi_cpi_control_start(&(thermeye->cpi_device));
        __pi_thermeye_start(thermeye);
        __pi_thermeye_trigger_snapshot(thermeye);
        break;

    case PI_CAMERA_CMD_STOP:
        pi_cpi_control_stop(&(thermeye->cpi_device));
        __pi_thermeye_stop(thermeye);
        break;

    default:
        break;
    }
    return 0;
}

static void __pi_thermeye_capture_async(struct pi_device *device, void *buffer,
                                        uint32_t bufferlen, pi_task_t *task)
{
    thermeye_t *thermeye = (thermeye_t *) device->data;
    pi_cpi_capture_async(&(thermeye->cpi_device), buffer, bufferlen, task);
    //__pi_thermeye_trigger_snapshot(thermeye);
}

static int32_t __pi_thermeye_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    thermeye_t *thermeye = (thermeye_t *) device->data;
    *value = __pi_thermeye_reg_read(thermeye, (uint16_t) addr);
    return 0;
}

static int32_t __pi_thermeye_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    thermeye_t *thermeye = (thermeye_t *) device->data;
    __pi_thermeye_reg_write(thermeye, (uint16_t) addr, *value);
    return 0;
}


static pi_camera_api_t thermeye_api =
{
    .open = &__pi_thermeye_open,
    .close = &__pi_thermeye_close,
    .control = &__pi_thermeye_control,
    .capture_async = &__pi_thermeye_capture_async,
    .reg_get = &__pi_thermeye_reg_get,
    .reg_set = &__pi_thermeye_reg_set
};


void pi_thermeye_conf_init(struct pi_thermeye_conf *conf)
{
    conf->camera.api = &thermeye_api;
    bsp_thermeye_conf_init(conf);
    conf->skip_pads_config = 0;
}
