/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __HAL_CHIPS_GAP_APB_SOC_H__
#define __HAL_CHIPS_GAP_APB_SOC_H__

#include "archi/pulp.h"

#define APB_SOC_PADFUN_OFFSET(g)   (APB_SOC_SAFE_PADFUN0_OFFSET+(g)*4) //sets the mux for pins  g*16+0 (bits [1:0]) to g*16+15 (bits [31:30])
#define APB_SOC_PADFUN_NO(pad)     ((pad) >> 4)
#define APB_SOC_PADFUN_PAD(padfun) ((padfun)*16)
#define APB_SOC_PADFUN_SIZE        2
#define ARCHI_APB_SOC_PADFUN_NB          3
#define APB_SOC_PADFUN_BIT(pad)    (((pad) & 0xF) << 1)

#define APB_SOC_PADCFG_OFFSET(g)   (APB_SOC_SAFE_PADCFG0_OFFSET+(g)*4) //sets config for pin  g*4+0(bits [7:0]) to pin  g*4+3(bits [31:24])
#define APB_SOC_PADCFG_NO(pad)     ((pad) >> 2)
#define APB_SOC_PADCFG_PAD(padfun) ((padfun)*4)
#define APB_SOC_PADCFG_SIZE        8
#define APB_SOC_PADCFG_BIT(pad)    (((pad) & 0x3) << 3)

#define APB_SOC_PADCFG_OFFSET(g)   (APB_SOC_SAFE_PADCFG0_OFFSET+(g)*4) //sets config for pin  g*4+0(bits [7:0]) to pin  g*4+3(bits [31:24])

#define APB_SOC_INFO_CORES_OFFSET    (APB_SOC_INFO_OFFSET + 2)

#define APB_SOC_STATUS_EOC_BIT  31

#define APB_SOC_BYPASS_USER1_BIT       15
#define APB_SOC_BYPASS_USER0_BIT       14

static inline void apb_soc_status_set(unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_CORESTATUS_OFFSET, value | (1<<APB_SOC_STATUS_EOC_BIT));
}

static inline unsigned int apb_soc_nbCores() {
  return pulp_read16(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_INFO_CORES_OFFSET);
}

static inline unsigned int apb_soc_nbClusters() {
  return pulp_read16(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_INFO_OFFSET);
}



static inline unsigned int hal_apb_soc_padfun_get(unsigned int id) {
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_PADFUN_OFFSET(id));
}

static inline void hal_apb_soc_padfun_set(unsigned int id, unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_PADFUN_OFFSET(id), value);
}

static inline unsigned int hal_apb_soc_padcfg_get(unsigned int id) {
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_PADCFG_OFFSET(id));
}

static inline void hal_apb_soc_padcfg_set(unsigned int id, unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_PADCFG_OFFSET(id), value);
}


static inline void hal_apb_soc_pad_set_function(unsigned int pad, unsigned int func) {
  unsigned int padfunId = APB_SOC_PADFUN_NO(pad);
  unsigned int padBit = APB_SOC_PADFUN_BIT(pad);
  unsigned int oldval = hal_apb_soc_padfun_get(padfunId) & ~(((1<<APB_SOC_PADFUN_SIZE)-1) << padBit);
  unsigned int newVal = oldval | (func << padBit);
  hal_apb_soc_padfun_set(padfunId, newVal);
}

static inline void hal_apb_soc_pad_set_config(unsigned int pad, unsigned int cfg) {
  unsigned int padcfgId = APB_SOC_PADCFG_NO(pad);
  unsigned int padBit = APB_SOC_PADCFG_BIT(pad);
  unsigned int oldval = hal_apb_soc_padcfg_get(padcfgId) & ~(((1<<APB_SOC_PADCFG_SIZE)-1) << padBit);
  unsigned int newVal = oldval | (cfg << padBit);
  hal_apb_soc_padcfg_set(padcfgId, newVal);
}




static inline unsigned int apb_soc_jtag_reg_read() {
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_JTAGREG_OFFSET);
}

static inline void apb_soc_jtag_reg_write(unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_JTAGREG_OFFSET, value);
}

static inline unsigned int apb_soc_jtag_reg_ext(unsigned int value) {
  return ARCHI_REG_FIELD_GET(value, APB_SOC_JTAGREG_EXT_SYNC_BIT, APB_SOC_JTAGREG_EXT_SYNC_WIDTH + APB_SOC_JTAGREG_EXT_BT_MD_WIDTH);
}

static inline unsigned int apb_soc_jtag_reg_loc(unsigned int value) {
  return ARCHI_REG_FIELD_GET(value, APB_SOC_JTAGREG_INT_SYNC_BIT, APB_SOC_JTAGREG_INT_SYNC_WIDTH + APB_SOC_JTAGREG_INT_BT_MD_WIDTH);
}

static inline void apb_soc_padsleep_set(unsigned int value)
{
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_SAFE_PADSLEEP_OFFSET, value);  
}

static inline unsigned int apb_soc_sleep_control_get()
{
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_SAFE_PMU_SLEEPCTRL_OFFSET);  
}

#endif
