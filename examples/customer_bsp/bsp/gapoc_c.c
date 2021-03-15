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
#include "gapoc_c.h"

#include "bsp/bsp.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/pad.h"
#include "bsp/flash/spiflash.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/camera/hm0360.h"
#include "bsp/camera/gc0308.h"

static int __bsp_init_pads_done = 0;

static void __gpio_init()
{
    // FIXME: once inited, nothing works
    pi_gpio_pin_configure(0, GAPA9_WKUPIN    , PI_GPIO_INPUT);
    pi_gpio_pin_configure(0, GAP_B11_PBUTTON , PI_GPIO_INPUT);

    pi_gpio_pin_configure(0, GPIO_1V8MEM_EN  , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, GPIO_1V8MEM_EN, 0);
    pi_gpio_pin_configure(0, GAPB15_LED2     , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, GAPB15_LED2, 0);
    pi_gpio_pin_configure(0, GPIO_ESP32_EN   , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, GPIO_ESP32_EN, 0);
    pi_gpio_pin_configure(0, GPIO_CAM_RESET  , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, GPIO_CAM_RESET, 0);
    pi_gpio_pin_configure(0, CAM_GPIO_A      , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, CAM_GPIO_A, 0);
    pi_gpio_pin_configure(0, CAM_GPIO_B      , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, CAM_GPIO_B, 0);
    pi_gpio_pin_configure(0, CAM_GPIO_C      , PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, CAM_GPIO_C, 0);
                                               
} 

static int __bsp_init_pads(){
    pi_pad_set_function(PI_PAD_12_A3_RF_PACTRL0,  GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_47_A9_SPIS0_CSN,   GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_31_B11_TIMER0_CH0, GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_48_B15_SPIS0_MISO, GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_37_B14_I2S1_SDI,   GPIO_FUNC1);
    /*
     * Moved to bsp open
    pi_pad_set_function(PI_PAD_16_A44_RF_PACTRL4, GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_13_B2_RF_PACTRL1,  GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_15_B1_RF_PACTRL3,  GPIO_FUNC1);
    pi_pad_set_function(PI_PAD_14_A2_RF_PACTRL2,  GPIO_FUNC1);
     */
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
    pi_gpio_pin_write(NULL, GPIO_1V8MEM_EN, 1);
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
    __bsp_init_spi_pads();
    return 0;
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
    // FIXME: I don't think this is necessary any more on GAP8_revC
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
    // FIXME: I don't think this is necessary any more on GAP8_revC
    pi_pad_set_function(CONFIG_HYPERBUS_DATA6_PAD, CONFIG_HYPERRAM_DATA6_PAD_FUNC);
    return 0;
}

void bsp_hm0360_conf_init(struct pi_hm0360_conf *conf)
{
    conf->i2c_itf = CONFIG_HM0360_I2C_ITF;
    conf->cpi_itf = CONFIG_HM0360_CPI_ITF;
    conf->gpio_xshdown = GPIO_CAM_RESET;  
    conf->gpio_xsleep = CAM_GPIO_A;
    conf->gpio_cxt_sel = CAM_GPIO_B;
    conf->gpio_fae = CAM_GPIO_C;
    conf->skip_pads_config = 0;
}

int bsp_hm0360_open(struct pi_hm0360_conf *conf)
{
    if (!conf->skip_pads_config)
    {
        pi_pad_set_function(PI_PAD_16_A44_RF_PACTRL4, GPIO_FUNC1);
        pi_pad_set_function(PI_PAD_13_B2_RF_PACTRL1,  GPIO_FUNC1);
        pi_pad_set_function(PI_PAD_15_B1_RF_PACTRL3,  GPIO_FUNC1);
        pi_pad_set_function(PI_PAD_14_A2_RF_PACTRL2,  GPIO_FUNC1);
        conf->skip_pads_config = 1;
    }
    return 0;
}

void bsp_gc0308_conf_init(struct pi_gc0308_conf *conf)
{
    //__bsp_init_pads();
    conf->i2c_itf    = CONFIG_GC0308_I2C_ITF;
    conf->cpi_itf    = CONFIG_GC0308_CPI_ITF;
    conf->reset_gpio = GC0308_GPIO_RST;
    conf->pwdn_gpio  = GC0308_GPIO_PWDN;
    conf->mclk_gpio  = GC0308_GPIO_MCLK;
}

int bsp_gc0308_open(struct pi_gc0308_conf *conf)
{
    //__bsp_init_pads();
    if (!conf->skip_pads_config)
    {
        //Set camera PAD CPI + I2C
        pi_pad_set_function(PI_PAD_18_A43_CAM_PCLK ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_20_B39_CAM_DATA0,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_21_A42_CAM_DATA1,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_22_B38_CAM_DATA2,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_23_A41_CAM_DATA3,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_24_B37_CAM_DATA4,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_25_A40_CAM_DATA5,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_26_B36_CAM_DATA6,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_27_A38_CAM_DATA7,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_28_A36_CAM_VSYNC,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_19_A37_CAM_HSYNC,PI_PAD_FUNC0);

        pi_pad_set_function(PI_PAD_30_D1_CAM_SCL_FUNC0 ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_29_B34_CAM_SDA_FUNC0 ,PI_PAD_FUNC0);

        //pi_pad_set_function(PI_PAD_53_B22_I2C0_SDA ,PI_PAD_FUNC0);
        //pi_pad_set_function(PI_PAD_54_A25_I2C0_SCL ,PI_PAD_FUNC0);

        //CAM Power DOWN //Camera ON with this signal at 0
        pi_pad_set_function(GC0308_PAD_PWDN, PI_PAD_FUNC1);
        //Set as GPIO output and set it to 0 to be sure it is not turned on here
        pi_gpio_pin_configure(0,conf->pwdn_gpio, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
        //keep the camera off
        pi_gpio_pin_write(0,conf->pwdn_gpio,0);
        
        //CAM Reset //Asserted the reset with this pin at 0
        pi_pad_set_function(GC0308_PAD_RST, PI_PAD_FUNC1);
        //Set as GPIO output and set it to 0
        pi_gpio_pin_configure(0,conf->reset_gpio, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
        pi_gpio_pin_write(0,conf->reset_gpio,1);

        //CAM Master Clock //Master clock enabled at 1
        pi_pad_set_function(GC0308_PAD_MCLK, PI_PAD_FUNC1);
        pi_gpio_pin_configure(0,GC0308_GPIO_MCLK, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
        //Keep the master clock disabled
        pi_gpio_pin_write(0,GC0308_GPIO_MCLK,1);

    }
    return 0;
}

void pi_bsp_init()
{
    __bsp_init_pads();
    //__gpio_init();
}
