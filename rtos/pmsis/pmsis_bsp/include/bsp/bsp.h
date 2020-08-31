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

#ifndef __BSP__BSP_H__
#define __BSP__BSP_H__

#if defined(CONFIG_GAPUINO)
#include <bsp/gapuino.h>
#endif

#if defined(CONFIG_GAPOC_A)
#include <bsp/gapoc_a.h>
#endif

#if defined(CONFIG_GAPOC_B)
#if defined(CHIP_VERSION)
#if (CHIP_VERSION == 1)
#include <bsp/gapoc_b.h>
#elif (CHIP_VERSION == 2)
#include <bsp/gapoc_b_v2.h>
#else
#include <bsp/gapoc_b_v2.h>
#endif  /* CHIP_VERSION */
#endif  /* CHIP_VERSION */
#endif  /* CONFIG_GAPOC_B */

#if defined(CONFIG_AI_DECK)
#include <bsp/ai_deck.h>
#endif

#if defined(CONFIG_WOLFE)
#include <bsp/wolfe.h>
#endif

#if defined(CONFIG_VEGA)
#include <bsp/vega.h>
#endif

#if defined(CONFIG_GAP9)
#include <bsp/gap9.h>
#endif

#if defined(CONFIG_GAP9_V2)
#include <bsp/gap9_v2.h>
#endif

/* Include debug helper. */
#include "bsp/debug.h"

#if defined(CONFIG_HIMAX)
#include "bsp/camera/himax.h"
void bsp_himax_conf_init(struct pi_himax_conf *conf);
int bsp_himax_open(struct pi_himax_conf *conf);
#endif

#if defined(CONFIG_OV7670)
#include "bsp/camera/ov7670.h"
void bsp_ov7670_conf_init(struct pi_ov7670_conf *conf);
int bsp_ov7670_open(struct pi_ov7670_conf *conf);
#endif

#if defined(CONFIG_GC0308)
#include "bsp/camera/gc0308.h"
void bsp_gc0308_conf_init(struct pi_gc0308_conf *conf);
int bsp_gc0308_open(struct pi_gc0308_conf *conf);
#endif

#if defined(CONFIG_OV5640)
#include "bsp/camera/ov5640.h"
void bsp_ov5640_conf_init(struct pi_ov5640_conf *conf);
int bsp_ov5640_open(struct pi_ov5640_conf *conf);
#endif

#if defined(CONFIG_MT9V034)
#include "bsp/camera/mt9v034.h"
void bsp_mt9v034_conf_init(struct pi_mt9v034_conf *conf);
int bsp_mt9v034_open(struct pi_mt9v034_conf *conf);
#endif

#if defined(CONFIG_PIXART)
#include "bsp/camera/pixart.h"
void bsp_pixart_conf_init(struct pi_pixart_conf *conf);
int bsp_pixart_open(struct pi_pixart_conf *conf);
#endif

#if defined(CONFIG_ILI9341)
#include "bsp/display/ili9341.h"
void bsp_ili9341_conf_init(struct pi_ili9341_conf *conf);
int bsp_ili9341_open(struct pi_ili9341_conf *conf);

#endif

#if defined(CONFIG_24XX1025)
#include "bsp/eeprom/24xx1025.h"
void bsp_24xx1025_conf_init(struct pi_24xx1025_conf *conf);
int bsp_24xx1025_open(struct pi_24xx1025_conf *conf);
#endif

#if defined(CONFIG_VIRTUAL_EEPROM)
#include "bsp/eeprom/virtual_eeprom.h"
void bsp_virtual_eeprom_conf_init(struct pi_virtual_eeprom_conf *conf);
int bsp_virtual_eeprom_open(struct pi_virtual_eeprom_conf *conf);
#endif


#if defined(CONFIG_HYPERFLASH)
#include "bsp/flash/hyperflash.h"
void bsp_hyperflash_conf_init(struct pi_hyperflash_conf *conf);
int bsp_hyperflash_open(struct pi_hyperflash_conf *conf);
#endif

#if defined(CONFIG_MRAM)
#include "bsp/flash/mram.h"
#endif

#if defined(CONFIG_SPIFLASH)
#include "bsp/flash/spiflash.h"
void bsp_spiflash_conf_init(struct pi_spiflash_conf *conf);
int bsp_spiflash_open(struct pi_spiflash_conf *conf);
#endif

#if defined(CONFIG_HYPERRAM)
#include "bsp/ram/hyperram.h"
void bsp_hyperram_conf_init(struct pi_hyperram_conf *conf);
int bsp_hyperram_open(struct pi_hyperram_conf *conf);
#endif

#if defined(CONFIG_SPIRAM)
#include "bsp/ram/spiram.h"
void bsp_spiram_conf_init(struct pi_spiram_conf *conf);
int bsp_spiram_open(struct pi_spiram_conf *conf);
#endif

#if defined(CONFIG_APS25XXXN)
#include "bsp/ram/aps25xxxn.h"
void bsp_aps25xxxn_conf_init(struct pi_aps25xxxn_conf *conf);
int bsp_aps25xxxn_open(struct pi_aps25xxxn_conf *conf);
#endif

#if defined(CONFIG_NINA_W10)
#include "bsp/transport/nina_w10.h"
void bsp_nina_w10_conf_init(struct pi_nina_w10_conf *conf);
int bsp_nina_w10_open(struct pi_nina_w10_conf *conf);
#endif

#if defined(CONFIG_NINA_B112)
#include "bsp/ble/nina_b112/nina_b112_old.h"
int bsp_nina_b112_open_old();
#include "bsp/ble/nina_b112.h"
void bsp_nina_b112_conf_init(struct pi_nina_b112_conf *conf);
int bsp_nina_b112_open(struct pi_nina_b112_conf *conf);
#endif

#if defined(CONFIG_THERMEYE)
#include "bsp/camera/thermeye.h"
void bsp_thermeye_conf_init(struct pi_thermeye_conf *conf);
int bsp_thermeye_open(struct pi_thermeye_conf *conf);
#endif  /* CONFIG_THERMEYE */

void bsp_init();

void pi_bsp_init();

#define PI_BSP_PROFILE_DEFAULT 0

void pi_bsp_init_profile(int profile);


#endif
