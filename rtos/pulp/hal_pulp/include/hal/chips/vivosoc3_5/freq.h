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

#ifndef __HAL_CHIPS_VIVOSOC3_5_FREQ_H__
#define __HAL_CHIPS_VIVOSOC3_5_FREQ_H__

#include "hal/pulp.h"

typedef enum {
  FREQ_DOMAIN_CLK_TREE_32K =      0x01,	// available for all domains
  FREQ_DOMAIN_CLK_TREE_16M =      0x05,	// available for all domains
  FREQ_DOMAIN_CLK_TREE_FLL_SOC =  0x00, // available for all domains
  FREQ_DOMAIN_CLK_TREE_FLL_ALT =  0x02, // available for cl domain (cl fll) and periph domain (per fll)
} hal_freq_domain_clk_tree_config_e;

static inline void hal_freq_soc_clk_tree_config_set(hal_freq_domain_clk_tree_config_e config)
{
  if(config < FREQ_DOMAIN_CLK_TREE_FLL_ALT) IP_WRITE(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_SOC_OFFSET, config);
}

static inline hal_freq_domain_clk_tree_config_e hal_freq_soc_clk_tree_config_get(void)
{
  return IP_READ(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_SOC_OFFSET);
}

static inline void hal_freq_cl_clk_tree_config_set(hal_freq_domain_clk_tree_config_e config)
{
  IP_WRITE(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_CL_OFFSET, config);
}

static inline hal_freq_domain_clk_tree_config_e hal_freq_cl_clk_tree_config_get(void)
{
  return IP_READ(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_CL_OFFSET);
}

static inline void hal_freq_per_clk_tree_config_set(hal_freq_domain_clk_tree_config_e config)
{
  IP_WRITE(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_PER_OFFSET, config);
}

static inline hal_freq_domain_clk_tree_config_e hal_freq_per_clk_tree_config_get(void)
{
  return IP_READ(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_PER_OFFSET);
}

static inline void hal_freq_pm_clk_tree_config_set(hal_freq_domain_clk_tree_config_e config)
{
  if(config < FREQ_DOMAIN_CLK_TREE_FLL_ALT) IP_WRITE(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_PM_OFFSET, config);
}

static inline hal_freq_domain_clk_tree_config_e hal_freq_pm_clk_tree_config_get(void)
{
  return IP_READ(ARCHI_APB_SOC_CTRL_ADDR, FREQ_CLK_TREE_CONF_PM_OFFSET);
}

#endif