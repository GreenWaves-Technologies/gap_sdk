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
#include "bsp/gapuino.h"
#include "bsp/camera/himax.h"
#include "bsp/display/ili9341.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"


static int __bsp_init_pads_done = 0;

static void __bsp_init_pads()
{
  if (!__bsp_init_pads_done)
  {
    __bsp_init_pads_done = 1;
    uint32_t pads_value[] = {0x00055500, 0x0f000000, 0x003fffff, 0x00000000};
    pi_pad_init(pads_value);
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
  __bsp_init_pads();
  return 0;
}



void bsp_hyperflash_conf_init(struct pi_hyperflash_conf *conf)
{
  conf->hyper_itf = CONFIG_HYPERFLASH_HYPER_ITF;
  conf->hyper_cs = CONFIG_HYPERFLASH_HYPER_CS;
}

int bsp_hyperflash_open(struct pi_hyperflash_conf *conf)
{
  __bsp_init_pads();
  return 0;
}



void bsp_himax_conf_init(struct pi_himax_conf *conf)
{
  __bsp_init_pads();
  conf->i2c_itf = CONFIG_HIMAX_I2C_ITF;
  conf->cpi_itf = CONFIG_HIMAX_CPI_ITF;
}

int bsp_himax_open(struct pi_himax_conf *conf)
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




void pi_bsp_init_profile(int profile)
{
    __bsp_init_pads();

    if (profile == PI_BSP_PROFILE_DEFAULT)
    {
        /* Special for I2S1, use alternative pad for SDI signal. */
        pi_pad_set_function(PI_PAD_35_B13_I2S1_SCK, PI_PAD_35_B13_I2S1_SDI_FUNC3);
        pi_pad_set_function(PI_PAD_37_B14_I2S1_SDI, PI_PAD_37_B14_HYPER_CK_FUNC3);

#ifndef __ZEPHYR__
        pi_i2s_setup(PI_I2S_SETUP_SINGLE_CLOCK);
#endif
    }
}



void pi_bsp_init()
{
  pi_bsp_init_profile(PI_BSP_PROFILE_DEFAULT);
}