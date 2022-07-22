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
#include "bsp/gapoc_b.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/display/ili9341.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/ble/nina_b112.h"
#include "bsp/ble/nina_b112/nina_b112_old.h"
#include "bsp/camera/pixart.h"

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
        uint32_t pads_value[] = {0x00055500, 0x0f450000, 0x003fffff, 0x00000000};
        pi_pad_init(pads_value);
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
    __bsp_init_pads();
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
    __bsp_init_pads();

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
    __bsp_init_pads();

    pi_pad_set_function(CONFIG_HYPERBUS_DATA6_PAD, CONFIG_UART_RX_PAD_FUNC);
    return 0;
}

void bsp_pixart_conf_init(struct pi_pixart_conf *conf)
{
    conf->cpi_itf = CONFIG_PIXART_CPI_ITF;
    conf->spi_itf = CONFIG_PIXART_SPI_ITF;
    conf->spi_cs = CONFIG_PIXART_SPI_CS;
    conf->pwm_id = CONFIG_PIXART_PWM_ID;
    conf->pwm_channel = CONFIG_PIXART_PWM_CH;
    conf->gpio_ctl.gpio_power_4V = CONFIG_PIXART_GPIO_PWR_4V;
    conf->gpio_ctl.gpio_power_2V5 = CONFIG_PIXART_GPIO_PWR_2V5;
    conf->gpio_ctl.gpio_reset = CONFIG_PIXART_GPIO_RESET;
}

int bsp_pixart_open(struct pi_pixart_conf *conf)
{
    __bsp_init_pads();
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
