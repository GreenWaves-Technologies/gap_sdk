/*
 * Copyright (C) 2018 GreenWaves Technologies
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

/*
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "bsp/bsp.h"


typedef struct
{
    struct pi_device i2c;
} ak4332_t;


static int __pi_ak4332_reg_write(pi_device_t *dev, uint8_t addr, uint8_t value)
{
    uint8_t buffer[2] = { addr, value };
    if (pi_i2c_write(dev, buffer, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP))
    {
        return -1;
    }
    return 0;
}


static uint8_t __pi_ak4332_reg_read(pi_device_t *dev, uint8_t addr)
{
    uint8_t result;
    pi_i2c_write_read(dev, &addr, &result, 1, 1);
    return result;
}


int pi_ak4332_open(struct pi_device *device)
{
    struct pi_ak4332_conf *conf = (struct pi_ak4332_conf *)device->config;

    ak4332_t *ak4332 = (ak4332_t *)pmsis_l2_malloc(sizeof(ak4332_t));
    if (ak4332 == NULL)
    {
        return -1;
    }

    device->data = (void *)ak4332;

    if (bsp_ak4332_open(conf))
    {
        goto error;
    }

    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 100000;
    pi_i2c_conf_set_slave_addr(&i2c_conf, 0x20, 0);

    pi_open_from_conf(&ak4332->i2c, &i2c_conf);
    if (pi_i2c_open(&ak4332->i2c)) goto error;

    // DAC initial settings
    __pi_ak4332_reg_write(&ak4332->i2c, 0x26, 0x02);

    __pi_ak4332_reg_write(&ak4332->i2c, 0x27, 0xC0);

    // Select left channel
    __pi_ak4332_reg_write(&ak4332->i2c, 0x07, 0x1);

    // Set 32bits samples
    __pi_ak4332_reg_write(&ak4332->i2c, 0x15, 0b110);

    // Set 48KHz sampling rate
    // and CM to 512
    __pi_ak4332_reg_write(&ak4332->i2c, 0x5, 0x2A);

    //Set HP Gain to 0
    __pi_ak4332_reg_write(&ak4332->i2c, 0x0d, 0b101);
    //Set DAC volume to max
    __pi_ak4332_reg_write(&ak4332->i2c, 0x0b, 0x1F);

    // Configure PLL to take BLCK as input clock
    __pi_ak4332_reg_write(&ak4332->i2c, 0x0E, 0x1);

    // Configure DAC to take PLL as input clock
    __pi_ak4332_reg_write(&ak4332->i2c, 0x13, 0x1);


    int pld = 3;
    __pi_ak4332_reg_write(&ak4332->i2c, 0x0F, pld >> 8);
    __pi_ak4332_reg_write(&ak4332->i2c, 0x10, pld & 0xff);
    int plm = 31;
    __pi_ak4332_reg_write(&ak4332->i2c, 0x11, plm >> 8);
    __pi_ak4332_reg_write(&ak4332->i2c, 0x12, plm & 0xff);

    // set volume to max
    //__pi_ak4332_reg_write(&ak4332->i2c, 0x0b, 0x1f);
    //__pi_ak4332_reg_write(&ak4332->i2c, 0x0d, 0x7);

    // Power-up PLL
    __pi_ak4332_reg_write(&ak4332->i2c, 0x00, 0x1);

    pi_time_wait_us(20000);

    // Power-up PMTIM
    __pi_ak4332_reg_write(&ak4332->i2c, 0x00, 0x3);

    // Power-up charge pump for both channels
    __pi_ak4332_reg_write(&ak4332->i2c, 0x01, 0x1);

    pi_time_wait_us(65000);

    // Power-up LDO1
    __pi_ak4332_reg_write(&ak4332->i2c, 0x01, 0x31);

    pi_time_wait_us(5000);

    // Power up charge pump 2
    __pi_ak4332_reg_write(&ak4332->i2c, 0x01, 0x33);

    // Power-up DAC
    __pi_ak4332_reg_write(&ak4332->i2c, 0x02, 0x1);

    // Power-up Amplifier
    __pi_ak4332_reg_write(&ak4332->i2c, 0x03, 0x1);

    return 0;

error:
    pmsis_l2_malloc_free(ak4332, sizeof(ak4332_t));
    return -2;
}


int pi_ak4332_set_dac_volume(pi_device_t *device, uint8_t volume)
{
    ak4332_t *ak4332 = (ak4332_t *)device->data;
    if (volume > 0x1F)
    {
        return -1;
    }

    return __pi_ak4332_reg_write(&ak4332->i2c, 0x0b, volume);
}

int pi_ak4332_set_hp_volume(pi_device_t *device, uint8_t volume)
{
    ak4332_t *ak4332 = (ak4332_t *)device->data;
    if (volume > 0x7)
    {
        return -1;
    }

    return __pi_ak4332_reg_write(&ak4332->i2c, 0x0d, volume);
}


void pi_ak4332_close(struct pi_device *device)
{
  ak4332_t *ak4332 = (ak4332_t *)device->data;
  pmsis_l2_malloc_free(ak4332, sizeof(ak4332_t));
}


void pi_ak4332_conf_init(struct pi_ak4332_conf *conf)
{
  bsp_ak4332_conf_init(conf);
}

