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
} tlv320_t;


static int __pi_tlv320_reg_write(pi_device_t *dev, uint8_t addr, uint8_t value)
{
    uint8_t buffer[2] = { addr, value };
    if (pi_i2c_write(dev, buffer, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP))
    {
        return -1;
    }
    return 0;
}


static uint8_t __pi_tlv320_reg_read(pi_device_t *dev, uint8_t addr)
{
    uint8_t result;
    pi_i2c_write_read(dev, &addr, &result, 1, 1);
    return result;
}


int pi_tlv320_open(struct pi_device *device)
{
    struct pi_tlv320_conf *conf = (struct pi_tlv320_conf *)device->config;

    tlv320_t *tlv320 = (tlv320_t *)pmsis_l2_malloc(sizeof(tlv320_t));
    if (tlv320 == NULL)
    {
        return -1;
    }

    device->data = (void *)tlv320;

    if (bsp_tlv320_open(conf))
    {
        goto error;
    }

    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 100000;
    pi_i2c_conf_set_slave_addr(&i2c_conf, 0x98, 0);

    pi_open_from_conf(&tlv320->i2c, &i2c_conf);
    if (pi_i2c_open(&tlv320->i2c)) goto error;

    //tlv320 write i2c config (and read for debug)
    
    uint8_t expected , read;

    // Select page 0 (configuration register page)
    expected = 0x00;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x00, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x00);
    printf("Read page 0x%x \n",read);
    if (read == expected)
      printf("Page ok \n");
    else printf("Page failed \n",read);
     
    // Wake-up device by I2C write into P0_R2 using internal AREG
    __pi_tlv320_reg_write(&tlv320->i2c, 0x02, 0x81);
    expected = 0x81;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x02, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x02);
    if (read == expected)
      printf("Wake up ok \n");
    else printf("Wake up failed \n",read);

    //wait at least 1ms to complete init
    pi_time_wait_us(2000);
    
    // Enable Input Ch-1,2,3,4 by I2C write into P0_R115
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x73, 0xF0);
    //only channel 2
    expected = 0x40;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x73, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x73);
    if (read == expected)
      printf("Enable channel 2 ok \n");
    else printf("Enable channel 2 failed \n",read);
    
    // Enable ASI Output Ch-1,2,3,4 slot by I2C write into P0_R116
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x74, 0xF0);
    //only channel 2
    expected = 0x40;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x74, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x74);
    if (read == expected)
      printf("Enable ASI out channel 2 ok \n");
    else printf("Enable ASI out channel 2 failed \n",read);
    
    // Power-up ADC, MICBIAS and PLL by I2C write into P0_R117
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x75, 0xe0);
    //power up only ADC and PLL
    expected = 0x60;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x75, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x75);
    if (read == expected)
      printf("Power up ADC and PLL  ok \n");
    else printf("Power Up ADC and PLL failed \n",read);
    

    // TDM 32bits mode
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x7, 0x30);
    expected = 0x30;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x7, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x7);
    if (read == expected)
      printf("TDM 32 bit ok \n");
    else printf("TDM 32 bit failed \n",read);

    // TX OFFSET. set to 1 to match our ws_delay of 1, but not sure it is the right value -- seems to work
    expected = 0x1;
    __pi_tlv320_reg_write(&tlv320->i2c, 0x8, expected);
    read = __pi_tlv320_reg_read(&tlv320->i2c, 0x8);
    if (read == expected)
      printf("Tx offset 1  ok \n");
    else printf("Tx offset 1  failed \n",read);



    //configure AC single - ended
    //channel1
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x3C, 0x20);
    //channel2
      __pi_tlv320_reg_write(&tlv320->i2c, 0x41, 0x20);
    //channel3
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x46, 0x20);
    //channel4
    //__pi_tlv320_reg_write(&tlv320->i2c, 0x4B, 0x20);

    return 0;

error:
    pmsis_l2_malloc_free(tlv320, sizeof(tlv320_t));
    return -2;
}


void pi_tlv320_close(struct pi_device *device)
{
  tlv320_t *tlv320 = (tlv320_t *)device->data;
  pmsis_l2_malloc_free(tlv320, sizeof(tlv320_t));
}


void pi_tlv320_conf_init(struct pi_tlv320_conf *conf)
{
  bsp_tlv320_conf_init(conf);
}

