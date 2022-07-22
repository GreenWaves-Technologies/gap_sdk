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
#include "epeas_cis001_board.h"

#include "bsp/bsp.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/pad.h"
#include "bsp/flash/spiflash.h"
#include "bsp/ram/spiram.h"
#include "bsp/camera/epeas_cis001.h"


static int __bsp_init_pads_done = 0;

static void __gpio_init()
{
    pi_gpio_pin_configure(0, GPIO_CAM_RESET, PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(0, GPIO_CAM_CAPTURE, PI_GPIO_OUTPUT);
} 

static int __bsp_init_pads()
{
    pi_pad_set_function(PAD_CAM_RESET, GPIO_FUNC1);
    pi_pad_set_function(PAD_CAM_CAPTURE, GPIO_FUNC1);

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


void bsp_epeas_cis001_conf_init(struct pi_epeas_cis001_conf *conf)
{
    conf->spi_itf = CONFIG_EPEAS_CIS001_SPI_ITF;
    conf->spi_cs  = CONFIG_EPEAS_CIS001_SPI_CS;
    conf->cpi_itf = CONFIG_EPEAS_CIS001_CPI_ITF;
    conf->pwm_id = (uint8_t) CONFIG_EPEAS_CIS001_PWM_ID;
    conf->pwm_channel = (uint8_t) CONFIG_EPEAS_CIS001_PWM_CH;
    conf->gpio_reset = EPEAS_CIS001_GPIO_RESET;  
    conf->gpio_capture = EPEAS_CIS001_GPIO_CAPTURE;
    conf->skip_pads_config = 0;


}

int bsp_epeas_cis001_open(struct pi_epeas_cis001_conf *conf)
{
    if (!conf->skip_pads_config)
    {
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

        pi_pad_set_function(PI_PAD_8_A4_RF_SPIM1_MISO ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_9_B3_RF_SPIM1_MOSI ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_10_A5_RF_SPIM1_CSN ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_11_B4_RF_SPIM1_SCK ,PI_PAD_FUNC0);

        pi_pad_set_function(PAD_CAM_RESET, PI_PAD_FUNC1);
        pi_gpio_pin_configure(0, GPIO_CAM_RESET, PI_GPIO_OUTPUT);
        pi_gpio_pin_write(0, GPIO_CAM_RESET, 1);

        pi_pad_set_function(PAD_CAM_CAPTURE, PI_PAD_FUNC1);
        pi_gpio_pin_configure(0, GPIO_CAM_CAPTURE, PI_GPIO_OUTPUT);
        pi_gpio_pin_write(0, GPIO_CAM_CAPTURE, 0);

        pi_pad_set_function(PI_PAD_32_A13_TIMER0_CH1 ,PI_PAD_FUNC0);

    }

    return 0;
}

void pi_bsp_init()
{
    __bsp_init_pads();
}
