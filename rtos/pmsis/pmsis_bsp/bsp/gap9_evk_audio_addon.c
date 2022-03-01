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
#include "bsp/boards/gap9_evk/audio_addon.h"


#if defined(CONFIG_FXL6408)
static pi_device_t __pi_bsp_fxl6408;
#if !defined(__FREERTOS__)
static PI_FC_TINY uint8_t __pi_bsp_fxl6408_is_init;
#else
static uint8_t __pi_bsp_fxl6408_is_init;
#endif


void bsp_fxl6408_conf_init(struct pi_fxl6408_conf *conf)
{
    pi_fxl6408_conf_init(conf);
    conf->i2c_itf = CONFIG_FXL6408UMX_I2C_ITF;
    conf->interrupt_pin = PI_PAD_089;
}


static __attribute__((noinline)) int __pi_bsp_fxl6408_do_init()
{
    struct pi_fxl6408_conf conf;
    pi_fxl6408_conf_init(&conf);
    pi_open_from_conf(&__pi_bsp_fxl6408, &conf);
    if (pi_fxl6408_open(&__pi_bsp_fxl6408))
    {
        return - 1;
    }

    __pi_bsp_fxl6408_is_init = 1;

    return 0;
}

static inline __attribute__((always_inline)) int __pi_bsp_fxl6408_check_init()
{
    if (!__pi_bsp_fxl6408_is_init)
    {
        return __pi_bsp_fxl6408_do_init();
    }

    return 0;
}
#endif


#if defined(CONFIG_AK4332)
void bsp_ak4332_conf_init(struct pi_ak4332_conf *conf)
{
    conf->i2c_itf = CONFIG_AK4332_I2C_ITF;
}

int bsp_ak4332_open(struct pi_ak4332_conf *conf)
{
    if (__pi_bsp_fxl6408_check_init())
    {
        return -1;
    }

    pi_fxl6408_gpio_conf_t gpio_conf;
    pi_fxl6408_gpio_conf_init(&gpio_conf);
    gpio_conf.id = CONFIG_FXL6408UMX_AK4332_GPIO;
    gpio_conf.direction = FXL6408_GPIO_DIR_OUTPUT;
    gpio_conf.output_state = FXL6408_GPIO_OUTPUT_STATE_HIGH;

    if (PI_OK != pi_fxl6408_gpio_set(&__pi_bsp_fxl6408, &gpio_conf))
    {
        return -1;
    }

    // Wait at least 1ms after ak4332 power-up
    // TODO took 2x as margin, check we can switch back to 1
    pi_time_wait_us(2000);

    return 0;
}
#endif


#if defined(CONFIG_TLV320)
void bsp_tlv320_conf_init(struct pi_tlv320_conf *conf)
{
    conf->i2c_itf = CONFIG_TLV320_I2C_ITF;
}

int bsp_tlv320_open(struct pi_tlv320_conf *conf)
{
    if (__pi_bsp_fxl6408_check_init())
    {
        return -1;
    }

    pi_fxl6408_gpio_conf_t gpio_conf;
    pi_fxl6408_gpio_conf_init(&gpio_conf);
    gpio_conf.id = CONFIG_FXL6408UMX_TLV320_GPIO;
    gpio_conf.direction = FXL6408_GPIO_DIR_OUTPUT;
    gpio_conf.output_state = FXL6408_GPIO_OUTPUT_STATE_HIGH;

    if (PI_OK != pi_fxl6408_gpio_set(&__pi_bsp_fxl6408, &gpio_conf))
    {
        return -1;
    }

    return 0;
}
#endif


void __bsp_audio_addon_init()
{
#if defined(CONFIG_FXL6408)
    // Initialize this global state here to work well with retentive wake-up
    __pi_bsp_fxl6408_is_init = 0;
#endif

    // Configure padframe for I2C1
    // TODO should use real pad names when available
    pi_pad_set_function(PI_PAD_042, PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_043, PI_PAD_FUNC0);

    // configure gpio for gpio expander interrupt
    pi_pad_set_function(PI_PAD_089, PI_PAD_FUNC1);
}
