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

#ifndef __HAL_CHIPS_USOC_V1_APB_SOC_H__
#define __HAL_CHIPS_USOC_V1_APB_SOC_H__
 
 

static inline uint32_t apb_soc_gpio_iface_cfg_get(int gpio)
{
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_GPIO_IFACE_CFG_OFFSET(gpio/APB_SOC_GPIO_IFACE_CFG_NB_GPIO));
}


static inline void apb_soc_gpio_iface_cfg_set(int gpio, uint32_t value)
{
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_GPIO_IFACE_CFG_OFFSET(gpio/APB_SOC_GPIO_IFACE_CFG_NB_GPIO), value);
}

static inline void apb_soc_gpio_iface_cfg_set_dir(int gpio, int dir)
{
  uint32_t cfg = apb_soc_gpio_iface_cfg_get(gpio);

  int bit = (gpio % APB_SOC_GPIO_IFACE_CFG_NB_GPIO) * APB_SOC_GPIO_IFACE_CFG_WIDTH;
  cfg = cfg & ~(1<<bit);
  cfg = cfg | (dir << bit);

  apb_soc_gpio_iface_cfg_set(gpio, cfg);
}


static inline void apb_soc_status_set(unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_CORESTATUS_OFFSET, value | (1<<APB_SOC_STATUS_EOC_BIT));
}


static inline void apb_soc_bootaddr_set(unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_BOOTADDR_OFFSET, value);
}

static inline unsigned int apb_soc_bootaddr_get() {
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_BOOTADDR_OFFSET);
}


static inline unsigned int apb_soc_jtag_reg_read() {
  return pulp_read32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_JTAG_REG_OFFSET);
}

static inline void apb_soc_jtag_reg_write(unsigned int value) {
  pulp_write32(ARCHI_APB_SOC_CTRL_ADDR + APB_SOC_JTAG_REG_OFFSET, value);
}

static inline unsigned int apb_soc_jtag_reg_ext(unsigned int value) {
  return ARCHI_REG_FIELD_GET(value, APB_SOC_JTAG_REG_EXT_BIT, APB_SOC_JTAG_REG_EXT_WIDTH);
}

static inline unsigned int apb_soc_jtag_reg_loc(unsigned int value) {
  return ARCHI_REG_FIELD_GET(value, APB_SOC_JTAG_REG_LOC_BIT, APB_SOC_JTAG_REG_LOC_WIDTH);
}


#endif