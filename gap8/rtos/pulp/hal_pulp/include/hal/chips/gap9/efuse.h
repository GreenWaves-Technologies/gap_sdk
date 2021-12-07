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

#ifndef __HAL_VEGA_EFUSE_H__
#define __HAL_VEGA_EFUSE_H__

#include "hal/pulp.h"

typedef struct {
  union {
    struct {
      unsigned int platform:3;
      unsigned int bootmode:8;
      unsigned int encrypted:1;
      unsigned int wait_xtal:1;
      unsigned int icache_enabled:1;
    } __attribute__((packed));
    uint32_t raw;
  } info;
  union {
    struct {
      unsigned int fll_freq_set:1;
      unsigned int fll_conf:1;
      unsigned int fll_lock:1;
      unsigned int clkdiv:2;
      unsigned int jtag_spis_lock:1;
      unsigned int ref_clk_wait:1;
      unsigned int pad_config:1;
    } __attribute__((packed));
    uint8_t raw;
  } info2;
  uint8_t Key[4][4];
  uint8_t IV[2][4];
  uint16_t wait_xtal_delta;
  uint8_t wait_xtal_min;
  uint8_t wait_xtal_max;
  uint8_t hyper_rds_delay;
  uint8_t fll_freq;
  uint8_t fll_lock_tolerance;
  uint8_t fll_assert_cycles;
  uint8_t periph_div;
  uint16_t ref_clk_wait_cycles;
  union {
    struct {
      unsigned int flash_type:2;
      unsigned int flash_reset:1;
      unsigned int flash_wait:1;
      unsigned int flash_wakeup:1;
      unsigned int flash_init:1;
      unsigned int flash_reset_wait:1;
      unsigned int ref_clk_wait_deep_sleep:1;
    } __attribute__((packed));
    uint8_t raw;
  } info3;
  union {
    struct {
      unsigned int flash_cmd:1;
      unsigned int flash_cmd_ds:1;
      unsigned int flash_cmd2:1;
      unsigned int flash_cmd2_ds:1;
      unsigned int flash_cmd3:1;
      unsigned int flash_cmd3_ds:1;
      unsigned int flash_cmd4:1;
      unsigned int flash_cmd4_ds:1;
    } __attribute__((packed));
    uint8_t raw;
  } info4;
  union {
    struct {
      unsigned int flash_cs:1;
      unsigned int flash_itf:2;
      unsigned int flash_pad:2;
      unsigned int hyperchip_size:1;
      unsigned int hyper_delay:1;
      unsigned int hyper_latency:1;
    } __attribute__((packed));
    uint8_t raw;
  } info5;
  union {
    struct {
      unsigned int i2c_loader:1;
      unsigned int i2c_itf:1;
      unsigned int i2c_pad_config:2;
      unsigned int signature:1;
      unsigned int flash_id:1;
      unsigned int mram_trim:1;
      unsigned int set_clkdiv:1;
    } __attribute__((packed));
    uint8_t raw;
  } info6;
  uint8_t flash_cmd;
  uint8_t flash_cmd2;
  uint8_t flash_cmd3;
  uint8_t flash_cmd4;
  uint8_t flash_wait;
  uint32_t hyperchip_size;
  uint16_t i2c_div;
  uint8_t i2c_cs;
  uint8_t flash_reset_wait;
  uint8_t hyper_latency;
  uint16_t ref_clk_wait_cycles_deep_sleep;
  uint16_t flash_id;
  uint16_t mram_trim_size;
  uint8_t padding[8];
} __attribute__((packed)) efuse_t;

#define PLP_EFUSE_PLT_OTHER   0
#define PLP_EFUSE_PLT_FPGA    1
#define PLP_EFUSE_PLT_RTL     2
#define PLP_EFUSE_PLT_VP      3
#define PLP_EFUSE_PLT_CHIP    4


#define PLP_EFUSE_BOOT_JTAG      0
#define PLP_EFUSE_BOOT_STOP      1
#define PLP_EFUSE_BOOT_FLASH     2
#define PLP_EFUSE_BOOT_WAIT      4
#define PLP_EFUSE_BOOT_WAIT_END  5


#define EFUSE_INFO_REG                           0
#define EFUSE_INFO2_REG                          1
#define EFUSE_AES_KEY_FIRST_REG                  2
#define EFUSE_AES_KEY_NB_REGS                    16
#define EFUSE_AES_IV_FIRST_REG                   18
#define EFUSE_AES_IV_NB_REGS                     8
#define EFUSE_WAIT_XTAL_DELTA_REG_LSB            26
#define EFUSE_WAIT_XTAL_DELTA_REG_MSB            27
#define EFUSE_WAIT_XTAL_MIN_REG                  28
#define EFUSE_WAIT_XTAL_MAX_REG                  29
#define EFUSE_HYPER_RDS_DELAY                    30
#define EFUSE_FLL_FREQ                           31
#define EFUSE_FLL_TOLERANCE                      32
#define EFUSE_FLL_ASSERT_CYCLES                  33
#define EFUSE_PERIPH_DIV                         34
#define EFUSE_REF_CLK_WAIT_CYCLES_LSB            35
#define EFUSE_REF_CLK_WAIT_CYCLES_MSB            36
#define EFUSE_INFO3_REG                          37
#define EFUSE_INFO4_REG                          38
#define EFUSE_INFO5_REG                          39
#define EFUSE_INFO6_REG                          40
#define EFUSE_FLASH_CMD                          41
#define EFUSE_FLASH_CMD2                         42
#define EFUSE_FLASH_CMD3                         43
#define EFUSE_FLASH_CMD4                         44
#define EFUSE_FLASH_WAIT                         45
#define EFUSE_HYPERCHIP_SIZE                     46
#define EFUSE_I2C_DIV_LSB                        47
#define EFUSE_I2C_DIV_MSB                        48
#define EFUSE_I2C_CS                             49
#define EFUSE_FLASH_RESET_WAIT                   50
#define EFUSE_HYPER_LATENCY                      51
#define EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_LSB 52
#define EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_MSB 53
#define EFUSE_FLASH_ID_LSB                       54
#define EFUSE_FLASH_ID_MSB                       55
#define EFUSE_MRAM_TRIM_SIZE                     56
#define EFUSE_MRAM_TRIM_START                    57
// DONT PUT ANYTHING AFTER THIS EFUSE AS IT IS A VARIABLE SIZE ARRAY

static inline unsigned int plp_efuse_readShort(int lsb, int msb) {
  return plp_efuse_readByte(lsb) | (plp_efuse_readByte(msb) << 8);
}

#endif
