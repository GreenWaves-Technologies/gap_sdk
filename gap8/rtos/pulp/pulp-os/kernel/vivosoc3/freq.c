/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#include "rt/rt_api.h"

static inline unsigned int __rt_fll_check_ref_fast(void)
{
  if (rt_platform() != ARCHI_PLATFORM_RTL) {
    return hal_fll_check_ref_fast();
  } else {
    return 0;
  }
}

__rt_freq_domain_settings_t __rt_freq_domain_settings[RT_FREQ_NB_DOMAIN];

void __rt_freq_init(void)
{
  // flls
  __rt_flls_constructor();  // init fll set constructor

  __rt_fll_set_init(HAL_FLL_CL);  // init fll set register

  // take care here!!
  if(__rt_fll_check_ref_fast()) hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_16M); // fast reference up and running
  else hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_32K); // take slow reference because fast is not running properly

  __rt_fll_set_init(HAL_FLL_SOC);  // init fll set register

  hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_FLL_SOC);  // switch back to fll_soc clock for soc_clk

  __rt_fll_set_init(HAL_FLL_PER);  // init fll set register

  // freq domains
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_freq =            __rt_fll_set_freq_get(HAL_FLL_SOC, 0);
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_config =          FREQ_DOMAIN_CLK_TREE_FLL_SOC;
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_fll_on =          1;  // soc fll is on from beginning
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_alt_fll_on =      0; 
  hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_FLL_SOC);

  __rt_freq_domain_settings[RT_FREQ_DOMAIN_FC].__rt_freq_domain_freq =            __rt_fll_set_freq_get(HAL_FLL_SOC, 0);
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_FC].__rt_freq_domain_config =          FREQ_DOMAIN_CLK_TREE_FLL_SOC;
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_FC].__rt_freq_domain_fll_on =          1;  // soc fll is on from beginning
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_FC].__rt_freq_domain_alt_fll_on =      -1; // there is no alternative fll available
  hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_FLL_SOC);

  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_freq =        __rt_fll_set_freq_get(HAL_FLL_SOC, 0);
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_config =      FREQ_DOMAIN_CLK_TREE_FLL_SOC;
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_fll_on =      1;  // soc fll is on from beginning
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_alt_fll_on =  0; 
  hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_FLL_SOC);

  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PM].__rt_freq_domain_freq =            __rt_fll_set_freq_get(HAL_FLL_SOC, 0);
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PM].__rt_freq_domain_config =          FREQ_DOMAIN_CLK_TREE_FLL_SOC;
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PM].__rt_freq_domain_fll_on =          1;  // soc fll is on from beginning
  __rt_freq_domain_settings[RT_FREQ_DOMAIN_PM].__rt_freq_domain_alt_fll_on =      -1; // there is no alternative fll available
  hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_FLL_SOC);
}

int rt_freq_config_set(rt_freq_domain_e domain, hal_freq_domain_clk_tree_config_e config)
{
  int irq = rt_irq_disable();
  int error = -1;

  // checks and storing of configs
  switch(config)
  {
    case FREQ_DOMAIN_CLK_TREE_32K:  // can always be done
    {
      __rt_freq_domain_settings[domain].__rt_freq_domain_config = FREQ_DOMAIN_CLK_TREE_32K;
      __rt_freq_domain_settings[domain].__rt_freq_domain_freq = 32768;
      break;
    }
    case FREQ_DOMAIN_CLK_TREE_16M:  // can only be done if 16M clk is running
    {
      if(__rt_fll_check_ref_fast())
      {
        __rt_freq_domain_settings[domain].__rt_freq_domain_config = FREQ_DOMAIN_CLK_TREE_16M;
        __rt_freq_domain_settings[domain].__rt_freq_domain_freq = 16384000;
      }
      else goto error;
      break;
    }
    case FREQ_DOMAIN_CLK_TREE_FLL_SOC:  // can only be done if soc fll is running (should always be the case)
    {
      if(__rt_freq_domain_settings[domain].__rt_freq_domain_fll_on == 1)
      {
        __rt_freq_domain_settings[domain].__rt_freq_domain_config = FREQ_DOMAIN_CLK_TREE_FLL_SOC;
        __rt_freq_domain_settings[domain].__rt_freq_domain_freq = __rt_fll_active_freq_get(HAL_FLL_SOC);       
      }
      else goto error;
      break;
    }
    case FREQ_DOMAIN_CLK_TREE_FLL_ALT:  // can only be done if alternate fll is running
    {
      if(__rt_freq_domain_settings[domain].__rt_freq_domain_alt_fll_on == 1)
      {
        if(domain == RT_FREQ_DOMAIN_CL) __rt_freq_domain_settings[domain].__rt_freq_domain_freq = __rt_fll_active_freq_get(HAL_FLL_CL); 
        else if(domain == RT_FREQ_DOMAIN_PERIPH) __rt_freq_domain_settings[domain].__rt_freq_domain_freq = __rt_fll_active_freq_get(HAL_FLL_PER); 
        else goto error; // something went wrong
        __rt_freq_domain_settings[domain].__rt_freq_domain_config = FREQ_DOMAIN_CLK_TREE_FLL_ALT;
      }
      else goto error;     
      break;
    }
    default: goto error;
  }

  // change clk tree config
  switch(domain)
  {
    case RT_FREQ_DOMAIN_FC:
    {
      hal_freq_soc_clk_tree_config_set(__rt_freq_domain_settings[domain].__rt_freq_domain_config);
      break;
    }
    case RT_FREQ_DOMAIN_CL:
    {
      hal_freq_cl_clk_tree_config_set(__rt_freq_domain_settings[domain].__rt_freq_domain_config);
      break;
    }
    case RT_FREQ_DOMAIN_PERIPH:
    {
      hal_freq_per_clk_tree_config_set(__rt_freq_domain_settings[domain].__rt_freq_domain_config);
      break;
    }
    case RT_FREQ_DOMAIN_PM:
    {
      hal_freq_pm_clk_tree_config_set(__rt_freq_domain_settings[domain].__rt_freq_domain_config);
      break;
    }
    default: goto error;
  }

  error = 0;

error:
  rt_irq_restore(irq);  
  return error;
}

unsigned int rt_freq_fll_freq_get(hal_fll_e fll, hal_fll_reg_set_e set)
{
  int irq = rt_irq_disable();
  int error = 0;

  // checks
  if(fll > HAL_FLL_PER) goto error;
  if(set > HAL_FLL_REG_SET_1) goto error;

  // get frequency
  error = __rt_fll_set_freq_get(fll, set);

error:
  rt_irq_restore(irq);  
  return error;  
}

unsigned int rt_freq_fll_freq_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int freq)
{
  int irq = rt_irq_disable();
  int error = 0;

  // checks
  if(fll > HAL_FLL_PER) goto error;
  if(set > HAL_FLL_REG_SET_1) goto error;  

  // set frequency
  error = __rt_fll_set_freq_set(fll, set, freq);

error:
  rt_irq_restore(irq);  
  return error;    
}

int rt_freq_fll_status_get(hal_fll_e fll)
{
  int irq = rt_irq_disable();
  int error = -1;

  // checks
  if(fll > HAL_FLL_PER) goto error;

  if((0x1 & hal_fll_status_get(fll)) == 0) goto error; // fll is off
  else error = (int) __rt_fll_set_active_get(fll); 

error:
  rt_irq_restore(irq);  
  return error;  
}

int rt_freq_fll_status_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned char enable)
{

  int irq = rt_irq_disable();
  int error = -1;
  unsigned int freq;

  // checks
  if(set > HAL_FLL_REG_SET_1) goto error;

  // enable (has to be done first in order to enable and set set within this function)
  if(enable)
  {
    freq = __rt_fll_enable(fll);
    if(fll == HAL_FLL_CL) __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_alt_fll_on = 1;
    else if(fll == HAL_FLL_PER) __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_alt_fll_on = 1;
  }
  else
  {
    if(fll == HAL_FLL_CL)
    {
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_ALT) goto error; // cannot disable fll as it is used as clk source
      else
      {
        __rt_fll_disable(fll);
        __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_alt_fll_on = 0;
      }
    }
    else if(fll == HAL_FLL_PER)
    {
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_ALT) goto error; // cannot disable fll as it is used as clk source
      else
      {
        __rt_fll_disable(fll);
        __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_alt_fll_on = 0;
      }
    }
    else if(fll == HAL_FLL_SOC) goto error; // soc fll cannot be turned off at the moment
  }

  // set
  switch(fll)
  {
    case HAL_FLL_CL: 
    {
      freq = __rt_fll_set_active_set(fll, set);
      // update internal settings when cl fll is choosen as clk source for cl clk
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_ALT) __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_freq = freq;
      break;
    }
    case HAL_FLL_SOC:
    {
      // take care here!!
      if(__rt_fll_check_ref_fast()) hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_16M); // fast reference up and running
      else hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_32K); // take slow reference because fast is not running properly

      freq = __rt_fll_set_active_set(fll, set);

      hal_freq_soc_clk_tree_config_set(FREQ_DOMAIN_CLK_TREE_FLL_SOC);  // switch back to fll_soc clock for soc_clk

      // update internal settings for all clk domains which have soc fll as clk source
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_SOC) __rt_freq_domain_settings[RT_FREQ_DOMAIN_CL].__rt_freq_domain_freq = freq; 
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_FC].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_SOC) __rt_freq_domain_settings[RT_FREQ_DOMAIN_FC].__rt_freq_domain_freq = freq; 
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_SOC) __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_freq = freq; 
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_PM].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_SOC) __rt_freq_domain_settings[RT_FREQ_DOMAIN_PM].__rt_freq_domain_freq = freq; 
      break;
    }
    case HAL_FLL_PER:
    {
      freq = __rt_fll_set_active_set(fll, set);
      // update internal settings when per fll is choosen as clk source for per clk
      if(__rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_config == FREQ_DOMAIN_CLK_TREE_FLL_ALT) __rt_freq_domain_settings[RT_FREQ_DOMAIN_PERIPH].__rt_freq_domain_freq = freq;
      break;
    }
    default: goto error;
  }

  error = 0;

error:
  rt_irq_restore(irq);  
  return error;  
}

