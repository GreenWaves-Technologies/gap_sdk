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
#include "bsp/wolfe.h"
#include "bsp/camera/himax.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"


static int __bsp_init_pads_done = 0;

static void __bsp_init_pads()
{
  if (!__bsp_init_pads_done)
  {
    __bsp_init_pads_done = 1;
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


void bsp_himax_conf_init(struct pi_himax_conf *conf)
{
  conf->i2c_itf = CONFIG_HIMAX_I2C_ITF;
  conf->cpi_itf = CONFIG_HIMAX_CPI_ITF;
}

int bsp_himax_open(struct pi_himax_conf *conf)
{
  __bsp_init_pads();
  return 0;
}



void bsp_init()
{
}



void pi_bsp_init_profile(int profile)
{
}



void pi_bsp_init()
{
  pi_bsp_init_profile(PI_BSP_PROFILE_DEFAULT);
}