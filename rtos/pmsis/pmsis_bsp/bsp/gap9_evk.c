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
#include "bsp/camera/himax.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/eeprom/24xx1025.h"
#include "bsp/eeprom/virtual_eeprom.h"

static int __bsp_init_pads_done = 0;

static void __bsp_init_pads()
{
  if (!__bsp_init_pads_done)
  {
    __bsp_init_pads_done = 1;
  }
}

void bsp_aps25xxxn_conf_init(struct pi_aps25xxxn_conf *conf)
{
    conf->ram_start = CONFIG_APS25XXXN_START;
    conf->ram_size = CONFIG_APS25XXXN_SIZE;
    conf->spi_itf = CONFIG_APS25XXXN_SPI_ITF;
    conf->spi_cs = CONFIG_APS25XXXN_SPI_CS;
}

int bsp_aps25xxxn_open(struct pi_aps25xxxn_conf *conf)
{
    return 0;
}


int bsp_24xx1025_open(struct pi_24xx1025_conf *conf)
{
    pi_pad_set_function(CONFIG_24XX1025_I2C_SCL_PAD, CONFIG_24XX1025_I2C_SCL_PADFUN);
    pi_pad_set_function(CONFIG_24XX1025_I2C_SDA_PAD, CONFIG_24XX1025_I2C_SDA_PADFUN);

#ifdef CONFIG_24XX1025_I2C_SCL_PADMUX_GROUP
    pi_pad_set_mux_group(CONFIG_24XX1025_I2C_SCL_PAD, CONFIG_24XX1025_I2C_SCL_PADMUX_GROUP);
#endif

#ifdef CONFIG_24XX1025_I2C_SDA_PADMUX_GROUP
    pi_pad_set_mux_group(CONFIG_24XX1025_I2C_SDA_PAD, CONFIG_24XX1025_I2C_SDA_PADMUX_GROUP);
#endif

    return 0;
}

void bsp_24xx1025_conf_init(struct pi_24xx1025_conf *conf)
{
  conf->i2c_addr = CONFIG_24XX1025_I2C_ADDR;
  conf->i2c_itf = CONFIG_24XX1025_I2C_ITF;
}

void bsp_virtual_eeprom_conf_init(struct pi_virtual_eeprom_conf *conf)
{
  conf->i2c_addr = CONFIG_VIRTUAL_EEPROM_I2C_ADDR;
  conf->i2c_itf = CONFIG_VIRTUAL_EEPROM_I2C_ITF;
}


void bsp_mx25u51245g_conf_init(struct pi_mx25u51245g_conf *conf)
{
    conf->spi_itf = CONFIG_MX25U51245G_SPI_ITF;
    conf->spi_cs = CONFIG_MX25U51245G_SPI_CS;
    conf->baudrate = 200000000;
}

int bsp_mx25u51245g_open(struct pi_mx25u51245g_conf *conf)
{
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



// This function is automatically called by the OS during init
void pi_bsp_init()
{
    // Set the pads alternate so that we have by default flash/ram and uart
    // working.
    // Flash and ram are on hyperbus0, pads 0 to 12 included.
    // Uart is on pad 44 and 45
#ifdef __FREERTOS__
    // TODO freertos is setting everything to 0 by default, keep it for now to not break everything
    uint32_t pad_values[] = { 0, 0, 0, 0, 0, 0 };
#else
    uint32_t pad_values[] = { 0x54000000, 0x55555555, 0x50555555, 0x55555555, 0x55555555, 0x55555555 };
#endif
    pi_pad_init(pad_values);

    // Since pad 44 and 45 are for i2c, we need to configure the pad mux
    pi_pad_set_mux_group(PI_PAD_044, PI_PAD_MUX_GROUP_UART1_RX);
    pi_pad_set_mux_group(PI_PAD_045, PI_PAD_MUX_GROUP_UART1_TX);

    pi_bsp_init_profile(PI_BSP_PROFILE_DEFAULT);

#if defined(CONFIG_GAP9_EVK_AUDIO_ADDON)
    __bsp_audio_addon_init();
#endif
}
