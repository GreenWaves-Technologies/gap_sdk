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

#ifndef __HAL_GAP_EFUSE_H__
#define __HAL_GAP_EFUSE_H__

#include "hal/pulp.h"

#if PULP_CHIP == CHIP_GAP

#define PLP_EFUSE_PLT_OTHER   0
#define PLP_EFUSE_PLT_FPGA    1
#define PLP_EFUSE_PLT_RTL     2
#define PLP_EFUSE_PLT_VP      3
#define PLP_EFUSE_PLT_CHIP    4


#define PLP_EFUSE_BOOT_OTHER     0
#define PLP_EFUSE_BOOT_SPIS_ALT2 1
#define PLP_EFUSE_BOOT_JTAG      2
#define PLP_EFUSE_BOOT_SPIS_DFLT 3
#define PLP_EFUSE_BOOT_PRELOAD   4
#define PLP_EFUSE_BOOT_HYPER     5
#define PLP_EFUSE_BOOT_SPIM      6
#define PLP_EFUSE_BOOT_SPIM_QPI  7 


#define GAP_EFUSE_INFO_REG          0
#define GAP_EFUSE_INFO2_REG         1
#define GAP_EFUSE_AES_KEY_FIRST_REG 2
#define GAP_EFUSE_AES_KEY_NB_REGS   16
#define GAP_EFUSE_AES_IV_FIRST_REG  18
#define GAP_EFUSE_AES_IV_NB_REGS    8

#define GAP_EFUSE_WAIT_XTAL_DELTA_REG_LSB     26
#define GAP_EFUSE_WAIT_XTAL_DELTA_REG_MSB     27

#define GAP_EFUSE_WAIT_XTAL_MIN_REG        28
#define GAP_EFUSE_WAIT_XTAL_MAX_REG        29

#define GAP_EFUSE_HYPER_RDS_DELAY          30 /* Eric: added a new efuse location for the hyper rds delay, one byte is enough */

#define GAP_EFUSE_FLL_FREQ                 31
#define GAP_EFUSE_FLL_TOLERANCE            32
#define GAP_EFUSE_FLL_ASSERT_CYCLES        33

#define GAP_EFUSE_PERIPH_DIV               34

#define GAP_EFUSE_INFO_PLT_BIT    0
#define GAP_EFUSE_INFO_PLT_WIDTH  3

#define GAP_EFUSE_INFO_BOOT_BIT    3
#define GAP_EFUSE_INFO_BOOT_WIDTH  3

#define GAP_EFUSE_INFO_ENCRYPTED_BIT     6
#define GAP_EFUSE_INFO_ENCRYPTED_WIDTH   1

#define GAP_EFUSE_INFO_WAIT_XTAL_BIT     7
#define GAP_EFUSE_INFO_WAIT_XTAL_WIDTH   1

#define GAP_EFUSE_INFO2_FLL_FREQ_SET_BIT    0
#define GAP_EFUSE_INFO2_FLL_FREQ_SET_WIDTH  1

#define GAP_EFUSE_INFO2_FLL_CONF_BIT    1
#define GAP_EFUSE_INFO2_FLL_CONF_WIDTH  1

#define GAP_EFUSE_INFO2_FLL_BYPASS_LOCK_BIT    2
#define GAP_EFUSE_INFO2_FLL_BYPASS_LOCK_WIDTH  1

#define GAP_EFUSE_INFO2_SPIM_CLKDIV_BIT    3
#define GAP_EFUSE_INFO2_SPIM_CLKDIV_WIDTH  2

static inline unsigned int plp_efuse_info_get() {
  return plp_efuse_readByte(GAP_EFUSE_INFO_REG);
}

static inline unsigned int plp_efuse_info2_get() {
  return plp_efuse_readByte(GAP_EFUSE_INFO2_REG);
}

static inline unsigned int plp_efuse_platform_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO_PLT_BIT, GAP_EFUSE_INFO_PLT_WIDTH);
}

static inline unsigned int plp_efuse_bootmode_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO_BOOT_BIT, GAP_EFUSE_INFO_BOOT_WIDTH);
}

static inline unsigned int plp_efuse_encrypted_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO_ENCRYPTED_BIT, GAP_EFUSE_INFO_ENCRYPTED_WIDTH);
}

static inline unsigned int plp_efuse_aesKey_get(int word) {
  return plp_efuse_readByte(GAP_EFUSE_AES_KEY_FIRST_REG + word);
}

static inline unsigned int plp_efuse_aesIv_get(int word) {
  return plp_efuse_readByte(GAP_EFUSE_AES_IV_FIRST_REG + word);
}

static inline unsigned int plp_efuse_wait_xtal_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO_WAIT_XTAL_BIT, GAP_EFUSE_INFO_WAIT_XTAL_WIDTH);
}

static inline unsigned int plp_efuse_wait_xtal_delta_get() {
  return plp_efuse_readByte(GAP_EFUSE_WAIT_XTAL_DELTA_REG_LSB) | (plp_efuse_readByte(GAP_EFUSE_WAIT_XTAL_DELTA_REG_MSB) << 8);
}

static inline unsigned int plp_efuse_wait_xtal_min_get() {
  return plp_efuse_readByte(GAP_EFUSE_WAIT_XTAL_MIN_REG);
}

static inline unsigned int plp_efuse_wait_xtal_max_get() {
  return plp_efuse_readByte(GAP_EFUSE_WAIT_XTAL_MAX_REG);
}

static inline unsigned int plp_efuse_hyper_rds_delay_get() {  /* Eric: added new function to read hyper rds delay */
  return plp_efuse_readByte(GAP_EFUSE_HYPER_RDS_DELAY);
}

static inline unsigned int plp_efuse_fll_set_freq_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO2_FLL_FREQ_SET_BIT, GAP_EFUSE_INFO2_FLL_FREQ_SET_WIDTH);
}

static inline unsigned int plp_efuse_fll_conf_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO2_FLL_CONF_BIT, GAP_EFUSE_INFO2_FLL_CONF_WIDTH);
}

static inline unsigned int plp_efuse_fll_bypass_lock_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO2_FLL_BYPASS_LOCK_BIT, GAP_EFUSE_INFO2_FLL_BYPASS_LOCK_WIDTH);
}

static inline unsigned int plp_efuse_spim_clkdiv_get(unsigned int infoValue) {
  return ARCHI_REG_FIELD_GET(infoValue, GAP_EFUSE_INFO2_SPIM_CLKDIV_BIT, GAP_EFUSE_INFO2_SPIM_CLKDIV_WIDTH);
}

static inline unsigned int plp_efuse_fll_freq_get() {
  return plp_efuse_readByte(GAP_EFUSE_FLL_FREQ);
}

static inline unsigned int plp_efuse_fll_tolerance_get() {
  return plp_efuse_readByte(GAP_EFUSE_FLL_TOLERANCE);
}

static inline unsigned int plp_efuse_fll_assert_cycles_get() {
  return plp_efuse_readByte(GAP_EFUSE_FLL_ASSERT_CYCLES);
}

#elif PULP_CHIP == CHIP_GAP_REV1

typedef struct {
  union {
    struct {
      unsigned int platform:3;
      unsigned int bootmode:3;
      unsigned int encrypted:1;
      unsigned int wait_xtal:1;
    } __attribute__((packed));
    uint8_t raw;
  } info;
  union {
    struct {
      unsigned int fll_freq_set:1;
      unsigned int fll_conf:1;
      unsigned int fll_bypass_lock:1;
      unsigned int spim_clkdiv:2;
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
      unsigned int flash_type:1;
      unsigned int set_clkdiv:1;
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
    } __attribute__((packed));
    uint8_t raw;
  } info6;
  uint8_t flash_cmd;
  uint8_t flash_cmd2;
  uint8_t flash_cmd3;
  uint8_t flash_cmd4;
  uint8_t flash_wait;
  uint8_t hyperchip_size;
  uint16_t i2c_div;
  uint8_t i2c_cs;
  uint8_t flash_reset_wait;
  uint8_t hyper_latency;
  uint16_t ref_clk_wait_cycles_deep_sleep;
  uint16_t flash_id;
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
#define PLP_EFUSE_BOOT_SPIS      3
#define PLP_EFUSE_BOOT_WAIT      4
#define PLP_EFUSE_BOOT_WAIT_END  5


#define GAP_EFUSE_INFO_REG                           0
#define GAP_EFUSE_INFO2_REG                          1
#define GAP_EFUSE_AES_KEY_FIRST_REG                  2
#define GAP_EFUSE_AES_KEY_NB_REGS                    16
#define GAP_EFUSE_AES_IV_FIRST_REG                   18
#define GAP_EFUSE_AES_IV_NB_REGS                     8
#define GAP_EFUSE_WAIT_XTAL_DELTA_REG_LSB            26
#define GAP_EFUSE_WAIT_XTAL_DELTA_REG_MSB            27
#define GAP_EFUSE_WAIT_XTAL_MIN_REG                  28
#define GAP_EFUSE_WAIT_XTAL_MAX_REG                  29
#define GAP_EFUSE_HYPER_RDS_DELAY                    30
#define GAP_EFUSE_FLL_FREQ                           31
#define GAP_EFUSE_FLL_TOLERANCE                      32
#define GAP_EFUSE_FLL_ASSERT_CYCLES                  33
#define GAP_EFUSE_PERIPH_DIV                         34
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_LSB            35
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_MSB            36
#define GAP_EFUSE_INFO3_REG                          37
#define GAP_EFUSE_INFO4_REG                          38
#define GAP_EFUSE_INFO5_REG                          39
#define GAP_EFUSE_INFO6_REG                          40
#define GAP_EFUSE_FLASH_CMD                          41
#define GAP_EFUSE_FLASH_CMD2                         42
#define GAP_EFUSE_FLASH_CMD3                         43
#define GAP_EFUSE_FLASH_CMD4                         44
#define GAP_EFUSE_FLASH_WAIT                         45
#define GAP_EFUSE_HYPERCHIP_SIZE                     46
#define GAP_EFUSE_I2C_DIV_LSB                        47
#define GAP_EFUSE_I2C_DIV_MSB                        48
#define GAP_EFUSE_I2C_CS                             49
#define GAP_EFUSE_FLASH_RESET_WAIT                   50
#define GAP_EFUSE_HYPER_LATENCY                      51
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_LSB 52
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_MSB 53
#define GAP_EFUSE_FLASH_ID_LSB                       54
#define GAP_EFUSE_FLASH_ID_MSB                       55

static inline unsigned int plp_efuse_readShort(int lsb, int msb) {
  return plp_efuse_readByte(lsb) | (plp_efuse_readByte(msb) << 8);
}

#else

typedef struct {
  union {
    struct {
      unsigned int platform:3;
      unsigned int bootmode:3;
      unsigned int encrypted:1;
      unsigned int wait_xtal:1;
    } __attribute__((packed));
    uint8_t raw;
  } info;
  union {
    struct {
      unsigned int fll_freq_set:1;
      unsigned int fll_conf:1;
      unsigned int fll_bypass_lock:1;
      unsigned int spim_clkdiv:2;
      unsigned int jtag_spis_lock:1;
      unsigned int ref_clk_wait:1;
      unsigned int pad_config:1;
    } __attribute__((packed));
    uint8_t raw;
  } info2;
  uint8_t Key[4][4];
  uint8_t IV[2][4];
  uint16_t wait_xtal_delta;
  uint16_t wait_xtal_min;
  uint16_t wait_xtal_max;
  uint8_t hyper_rds_delay;
  uint8_t padding0;
  uint8_t periph_div;
  uint16_t ref_clk_wait_cycles;
  union {
    struct {
      unsigned int flash_type:1;
      unsigned int set_clkdiv:1;
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
      unsigned int no_init_pads_after_wakeup:1;
      unsigned int set_burst_size:1;
    } __attribute__((packed));
    uint8_t raw;
  } info6;
  uint8_t flash_cmd;
  uint8_t flash_cmd2;
  uint8_t flash_cmd3;
  uint8_t flash_cmd4;
  uint8_t flash_wait;
  uint8_t hyperchip_size;
  uint16_t i2c_div;
  uint8_t i2c_cs;
  uint8_t flash_reset_wait;
  uint8_t hyper_latency;
  uint16_t ref_clk_wait_cycles_deep_sleep;
  uint16_t flash_id;
  uint8_t padding1;
  uint8_t fll_freq;
  uint8_t fll_lock_tolerance;
  uint8_t fll_assert_cycles;
  union {
    struct {
      unsigned int no_udma_memcpy:1;
      unsigned int mode_gpio:1;
      unsigned int partial_init_pads_after_wakeup:1;
    } __attribute__((packed));
    uint8_t raw;
  } info7;
  uint16_t burst_size;
  uint8_t mode_gpio;
  uint8_t mode_pad;
  uint8_t padding[3];
} __attribute__((packed)) efuse_t;

#define PLP_EFUSE_PLT_OTHER   0
#define PLP_EFUSE_PLT_FPGA    1
#define PLP_EFUSE_PLT_RTL     2
#define PLP_EFUSE_PLT_VP      3
#define PLP_EFUSE_PLT_CHIP    4


#define PLP_EFUSE_BOOT_JTAG      0
#define PLP_EFUSE_BOOT_STOP      1
#define PLP_EFUSE_BOOT_FLASH     2
#define PLP_EFUSE_BOOT_SPIS      3
#define PLP_EFUSE_BOOT_WAIT      4
#define PLP_EFUSE_BOOT_WAIT_END  5


#define GAP_EFUSE_INFO_REG                           0
#define GAP_EFUSE_INFO2_REG                          1
#define GAP_EFUSE_AES_KEY_FIRST_REG                  2
#define GAP_EFUSE_AES_KEY_NB_REGS                    16
#define GAP_EFUSE_AES_IV_FIRST_REG                   18
#define GAP_EFUSE_AES_IV_NB_REGS                     8
#define GAP_EFUSE_WAIT_XTAL_DELTA_REG_LSB            26
#define GAP_EFUSE_WAIT_XTAL_DELTA_REG_MSB            27
#define GAP_EFUSE_WAIT_XTAL_MIN_REG_LSB              28
#define GAP_EFUSE_WAIT_XTAL_MIN_REG_MSB              29
#define GAP_EFUSE_WAIT_XTAL_MAX_REG_LSB              30
#define GAP_EFUSE_WAIT_XTAL_MAX_REG_MSB              31
#define GAP_EFUSE_HYPER_RDS_DELAY                    32
#define GAP_EFUSE_PERIPH_DIV                         34
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_LSB            35
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_MSB            36
#define GAP_EFUSE_INFO3_REG                          37
#define GAP_EFUSE_INFO4_REG                          38
#define GAP_EFUSE_INFO5_REG                          39
#define GAP_EFUSE_INFO6_REG                          40
#define GAP_EFUSE_FLASH_CMD                          41
#define GAP_EFUSE_FLASH_CMD2                         42
#define GAP_EFUSE_FLASH_CMD3                         43
#define GAP_EFUSE_FLASH_CMD4                         44
#define GAP_EFUSE_FLASH_WAIT                         45
#define GAP_EFUSE_HYPERCHIP_SIZE                     46
#define GAP_EFUSE_I2C_DIV_LSB                        47
#define GAP_EFUSE_I2C_DIV_MSB                        48
#define GAP_EFUSE_I2C_CS                             49
#define GAP_EFUSE_FLASH_RESET_WAIT                   50
#define GAP_EFUSE_HYPER_LATENCY                      51
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_LSB 52
#define GAP_EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_MSB 53
#define GAP_EFUSE_FLASH_ID_LSB                       54
#define GAP_EFUSE_FLASH_ID_MSB                       55
#define GAP_EFUSE_FLL_FREQ                           57
#define GAP_EFUSE_FLL_TOLERANCE                      58
#define GAP_EFUSE_FLL_ASSERT_CYCLES                  59
#define GAP_EFUSE_INFO7_REG                          60
#define GAP_EFUSE_BURST_SIZE_LSB                     61
#define GAP_EFUSE_BURST_SIZE_MSB                     62
#define GAP_EFUSE_MODE_GPIO                          63
#define GAP_EFUSE_MODE_PAD                           64

static inline unsigned int plp_efuse_readShort(int lsb, int msb) {
  return plp_efuse_readByte(lsb) | (plp_efuse_readByte(msb) << 8);
}

#endif

#endif
