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
#include "bsp/gapoc_a.h"
#include "bsp/camera/mt9v034.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash/spiflash.h"
#include "bsp/transport/nina_w10.h"
#include "bsp/display/ili9341.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/ble/nina_b112.h"
#include "bsp/ble/nina_b112/nina_b112_old.h"


static int __bsp_init_pads_done = 0;


static void __gpio_init()
{
    pi_gpio_pin_configure(0, GPIOA0_LED      , PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(0, GPIOA1          , PI_GPIO_INPUT);
    pi_gpio_pin_configure(0, GPIOA4_1V8_EN   , PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(0, GPIOA18         , PI_GPIO_INPUT);
    pi_gpio_pin_configure(0, GPIOA19         , PI_GPIO_INPUT);
    /* pi_gpio_pin_configure(0, GPIOA2_NINA_RST , PI_GPIO_OUTPUT); */
    /* pi_gpio_pin_configure(0, GPIOA21_NINA17  , PI_GPIO_OUTPUT); */

    pi_gpio_pin_write(0, GPIOA0_LED, 0);
    pi_gpio_pin_write(0, GPIOA4_1V8_EN, 1);
    /* pi_gpio_pin_write(0, GPIOA2_NINA_RST, 0); */
    /* pi_gpio_pin_write(0, GPIOA21_NINA17, 1); */
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


static void __bsp_init_spi_pads()
{
    pi_pad_set_function(PI_PAD_40_D2_SPIM0_SDIO0 , PI_PAD_40_D2_SPIM0_SDIO0_FUNC0 );
    pi_pad_set_function(PI_PAD_41_A11_SPIM0_SDIO1, PI_PAD_41_A11_SPIM0_SDIO1_FUNC0);
    pi_pad_set_function(PI_PAD_42_B10_SPIM0_SDIO2, PI_PAD_42_B10_SPIM0_SDIO2_FUNC0);
    pi_pad_set_function(PI_PAD_43_A10_SPIM0_SDIO3, PI_PAD_43_A10_SPIM0_SDIO3_FUNC0);
    pi_pad_set_function(PI_PAD_44_B8_SPIM0_CSN0  , PI_PAD_44_B8_SPIM0_CSN0_FUNC0  );
    pi_pad_set_function(PI_PAD_45_A8_SPIM0_CSN1  , PI_PAD_45_A8_SPIM0_CSN1_FUNC0  );
    pi_pad_set_function(PI_PAD_46_B7_SPIM0_SCK   , PI_PAD_46_B7_SPIM0_SCK_FUNC0   );
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
    __bsp_init_spi_pads();
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
    __bsp_init_pads();
    __bsp_init_spi_pads();
    return 0;
}




void bsp_mt9v034_conf_init(struct pi_mt9v034_conf *conf)
{
    __bsp_init_pads();
    conf->i2c_itf = CONFIG_MT9V034_I2C_ITF;
    conf->cpi_itf = CONFIG_MT9V034_CPI_ITF;
    conf->power_gpio = CONFIG_MT9V034_POWER_GPIO;
    conf->trigger_gpio = CONFIG_MT9V034_TRIGGER_GPIO;
}

int bsp_mt9v034_open(struct pi_mt9v034_conf *conf)
{
    __bsp_init_pads();

    if (!conf->skip_pads_config)
    {
        pi_pad_set_function(CONFIG_MT9V034_TRIGGER_GPIO_PAD, CONFIG_MT9V034_TRIGGER_GPIO_PAD_FUNC);

        pi_pad_set_function(CONFIG_MT9V034_POWER_GPIO_PAD, CONFIG_MT9V034_POWER_GPIO_PAD_FUNC);
    }

    return 0;
}



void bsp_nina_w10_conf_init(struct pi_nina_w10_conf *conf)
{
    conf->spi_itf = CONFIG_NINA_W10_SPI_ITF;
    conf->spi_cs = CONFIG_NINA_W10_SPI_CS;
}

int bsp_nina_w10_open(struct pi_nina_w10_conf *conf)
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
    return 0;
}

int bsp_nina_b112_open_old()
{
    __bsp_init_pads();

    pi_pad_set_function(CONFIG_HYPERBUS_DATA6_PAD, CONFIG_UART_RX_PAD_FUNC);
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
