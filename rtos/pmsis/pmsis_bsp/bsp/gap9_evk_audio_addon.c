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
#include "bsp/gap9_v2.h"
#include "bsp/boards/gap9_evk/audio_addon.h"

static pi_device_t __bsp_fxl6408_i2c;
static PI_FC_TINY uint8_t __bsp_fxl6408_is_init;
static PI_FC_TINY uint32_t __bsp_fxl6408_output_state;  // Keep as 32bits to lower code footprint


void bsp_ak4332_conf_init(struct pi_ak4332_conf *conf)
{
    conf->i2c_itf = CONFIG_AK4332_I2C_ITF;
    conf->i2s_itf = CONFIG_AK4332_I2S_ITF;
}


int bsp_ak4332_open(struct pi_ak4332_conf *conf)
{
    return 0;
}


static void __bsp_fxl6408_write_reg(uint8_t addr, uint8_t value)
{
    uint8_t buffer[2] = { addr, value };
    pi_i2c_write(&__bsp_fxl6408_i2c, buffer, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP);
}


static uint8_t __bsp_fxl6408_read_reg(uint8_t addr)
{
    uint8_t result;
    pi_i2c_write_read(&__bsp_fxl6408_i2c, &addr, &result, 1, 1);
    return result;
}


static __attribute__((noinline)) int __bsp_fxl6408_do_init()
{
    struct pi_i2c_conf conf;
    pi_i2c_conf_init(&conf);
    conf.itf = CONFIG_FXL6408UMX_I2C_ITF;
    pi_i2c_conf_set_slave_addr(&conf, CONFIG_FXL6408UMX_I2C_ADDR, 0);

    pi_open_from_conf(&__bsp_fxl6408_i2c, &conf);
    if (pi_i2c_open(&__bsp_fxl6408_i2c))
    {
        return - 1;
    }

    // Configure GPIO direction (output) for dac and adc
    __bsp_fxl6408_write_reg(0x03, (1 << CONFIG_FXL6408UMX_AK4332_GPIO) | (1 << CONFIG_FXL6408UMX_TLV320_GPIO));

    __bsp_fxl6408_output_state = 0;
    __bsp_fxl6408_is_init = 1;

    return 0;
}


static inline __attribute__((always_inline)) int __bsp_fxl6408_check_init()
{
    if (!__bsp_fxl6408_is_init)
    {
        return __bsp_fxl6408_do_init();
    }

    return 0;
}


static void __bsp_fxl6408_gpio_output_state(unsigned int gpio, int state)
{
    __bsp_fxl6408_output_state = __BITINSERT_R(__bsp_fxl6408_output_state, state, 1, gpio);

    __bsp_fxl6408_write_reg(0x05, __bsp_fxl6408_output_state);
}


void __bsp_audio_addon_init()
{
    // Initialize this global state here to work well with retentive wake-up
    __bsp_fxl6408_is_init = 0;

    // Configure padframe for I2C1
    // TODO should use real pad names when available
    pi_pad_set_function(PI_PAD_042, 0);
    pi_pad_set_function(PI_PAD_043, 0);
}


uint8_t pi_bsp_fxl6408_read_id()
{
    if (__bsp_fxl6408_check_init()) return 0;
    
    return __bsp_fxl6408_read_reg(0x01);
}


int pi_bsp_ak4332_power_ctrl(int power_enable)
{
    if (__bsp_fxl6408_check_init()) return -1;

    __bsp_fxl6408_gpio_output_state(CONFIG_FXL6408UMX_AK4332_GPIO, power_enable);

    return 0;
}


int pi_bsp_tlv320_power_ctrl(int power_enable)
{
    if (__bsp_fxl6408_check_init()) return -1;

    __bsp_fxl6408_gpio_output_state(CONFIG_FXL6408UMX_TLV320_GPIO, power_enable);

    return 0;
}
