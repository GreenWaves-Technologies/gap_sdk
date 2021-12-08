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

#define GAP_EFUSE_HYPER_RDS_DELAY          30	/* Eric: added a new efuse location for the hyper rds delay, one byte is enough */

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

static inline unsigned int plp_efuse_hyper_rds_delay_get() {	/* Eric: added new function to read hyper rds delay */
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

#endif
