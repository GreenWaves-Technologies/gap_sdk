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
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/pad.h"
#include "bsp/gapoc_b_v2.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/display/ili9341.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/ble/nina_b112.h"
#include "bsp/ble/nina_b112/nina_b112_old.h"
#include "bsp/camera/thermeye.h"
#include "bsp/camera/ov5640.h"


static int __bsp_init_pads_done = 0;


static void __gpio_init()
{
    pi_gpio_pin_configure(0, GPIO_USER_LED, PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(0, GPIO_1V8_EN  , PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);

    pi_gpio_pin_write(0, GPIO_USER_LED, 0);
    pi_gpio_pin_write(0, GPIO_1V8_EN, 1);
}

static void __bsp_init_pads()
{
    if (!__bsp_init_pads_done)
    {
        __bsp_init_pads_done = 1;

        // This has been removed because it is messing-up pad configs done by user
        // The pads should now be configured by the drivers themselves
        //uint32_t pads_value[] = {0x00055500, 0x0f450000, 0x003fffff, 0x00000000};
        //pi_pad_init(pads_value);
        __gpio_init();
    }
}

void bsp_hyperram_conf_init(struct pi_hyperram_conf *conf)
{
    conf->ram_start = CONFIG_HYPERRAM_START;
    conf->ram_size = CONFIG_HYPERRAM_SIZE;
    conf->skip_pads_config = 0;
    conf->hyper_itf = CONFIG_HYPERRAM_HYPER_ITF;
    conf->hyper_cs = CONFIG_HYPERRAM_HYPER_CS;
}

int bsp_hyperram_open(struct pi_hyperram_conf *conf)
{
    pi_pad_set_function(CONFIG_HYPERBUS_DATA6_PAD, CONFIG_HYPERRAM_DATA6_PAD_FUNC);
    return 0;
}

void bsp_hyperflash_conf_init(struct pi_hyperflash_conf *conf)
{
    conf->hyper_itf = CONFIG_HYPERFLASH_HYPER_ITF;
    conf->hyper_cs = CONFIG_HYPERFLASH_HYPER_CS;
}

int bsp_hyperflash_open(struct pi_hyperflash_conf *conf)
{
    pi_pad_set_function(CONFIG_HYPERBUS_DATA6_PAD, CONFIG_HYPERRAM_DATA6_PAD_FUNC);
    return 0;
}


void bsp_spiflash_conf_init(struct pi_spiflash_conf *conf)
{
    conf->size = CONFIG_SPIFLASH_SIZE;
    // sector size is in number of KB
    conf->sector_size = CONFIG_SPIFLASH_SECTOR_SIZE;
    conf->spi_itf = CONFIG_SPIFLASH_SPI_ITF;
    conf->spi_cs = CONFIG_SPIFLASH_SPI_CS;
}

int bsp_spiflash_open(struct pi_spiflash_conf *conf)
{
    return 0;
}

void bsp_spiram_conf_init(struct pi_spiram_conf *conf)
{
    conf->ram_start = CONFIG_SPIRAM_START;
    conf->ram_size = CONFIG_SPIRAM_SIZE;
    conf->skip_pads_config = 0;
    conf->spi_itf = CONFIG_SPIRAM_SPI_ITF;
    conf->spi_cs = CONFIG_SPIRAM_SPI_CS;
}

int bsp_spiram_open(struct pi_spiram_conf *conf)
{
    return 0;
}

void bsp_ili9341_conf_init(struct pi_ili9341_conf *conf)
{
    conf->gpio = CONFIG_ILI9341_GPIO;
    conf->spi_itf = CONFIG_ILI9341_SPI_ITF;
    conf->spi_cs = CONFIG_ILI9341_SPI_CS;

}

int bsp_ili9341_open(struct pi_ili9341_conf *conf)
{
    if (!conf->skip_pads_config)
    {
        pi_pad_set_function(CONFIG_ILI9341_GPIO_PAD, CONFIG_ILI9341_GPIO_PAD_FUNC);
    }

    return 0;
}

void bsp_nina_b112_conf_init(struct pi_nina_b112_conf *conf)
{
    conf->uart_itf = (uint8_t) CONFIG_NINA_B112_UART_ID;
}

int bsp_nina_b112_open(struct pi_nina_b112_conf *conf)
{
    //__gpio_init();
    return 0;
}

int bsp_nina_b112_open_old()
{
    pi_pad_set_function(CONFIG_HYPERBUS_DATA6_PAD, CONFIG_UART_RX_PAD_FUNC);
    return 0;
}

void bsp_thermeye_conf_init(struct pi_thermeye_conf *conf)
{
    conf->cpi_id = (uint8_t) CONFIG_THERMEYE_CPI_ID;
    conf->i2c_id = (uint8_t) CONFIG_THERMEYE_I2C_ID;
    conf->pwm_id = (uint8_t) CONFIG_THERMEYE_PWM_ID;
    conf->pwm_channel = (uint8_t) CONFIG_THERMEYE_PWM_CH;
    conf->gpio_power = (pi_gpio_e) CONFIG_THERMEYE_GPIO_POWER;
    conf->gpio_reset = (pi_gpio_e) CONFIG_THERMEYE_GPIO_RESET;
    conf->cpi_format = (pi_cpi_format_e) PI_CPI_FORMAT_BYPASS_LITEND;
}

int bsp_thermeye_open(struct pi_thermeye_conf *conf)
{

    //Set Pad to control GPIO
    //IR Trig
    pi_pad_set_function(PI_PAD_15_B1_RF_PACTRL3, PI_PAD_FUNC1);
    pi_gpio_pin_configure(0,GPIO_IR_TRIG,PI_GPIO_OUTPUT);
    pi_gpio_pin_write(0,GPIO_1V8_EN,1);
    //IR Power ON
    pi_pad_set_function(PI_PAD_17_B40_RF_PACTRL5, PI_PAD_FUNC1);
    pi_gpio_pin_configure(0,GPIO_IR_PWRON, PI_GPIO_OUTPUT);
    pi_gpio_pin_write(0,GPIO_IR_PWRON,1);
    //1V8 enable
    pi_pad_set_function(PI_PAD_16_A44_RF_PACTRL4, PI_PAD_FUNC1);
    pi_gpio_pin_configure(0, GPIO_1V8_EN  , PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
    pi_gpio_pin_write(0,GPIO_1V8_EN,1);
    //PWM IR Clock
    pi_pad_set_function(PI_PAD_33_B12_TIMER0_CH2, PI_PAD_FUNC0);

    //Set camera PAD CPI + I2C
    pi_pad_set_function(PI_PAD_18_A43_CAM_PCLK ,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_19_A37_CAM_HSYNC,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_20_B39_CAM_DATA0,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_21_A42_CAM_DATA1,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_22_B38_CAM_DATA2,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_23_A41_CAM_DATA3,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_24_B37_CAM_DATA4,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_25_A40_CAM_DATA5,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_26_B36_CAM_DATA6,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_27_A38_CAM_DATA7,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_28_A36_CAM_VSYNC,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_29_B34_CAM_SDA  ,PI_PAD_FUNC0);
    pi_pad_set_function(PI_PAD_30_D1_CAM_SCL   ,PI_PAD_FUNC0);

    return 0;
}

void bsp_ov5640_conf_init(struct pi_ov5640_conf *conf)
{
    conf->i2c_itf = CONFIG_OV5640_I2C_ID;
    conf->cpi_itf = CONFIG_OV5640_CPI_ID;
}

int bsp_ov5640_open(struct pi_ov5640_conf *conf)
{
    return 0;
}


void bsp_init()
{
    __bsp_init_pads();
}


void pi_bsp_init_profile(int profile)
{
}



void pi_bsp_init()
{
    pi_bsp_init_profile(PI_BSP_PROFILE_DEFAULT);
}
