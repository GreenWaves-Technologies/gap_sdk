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

#ifndef __HAL_FLL_FLL_V1_H__
#define __HAL_FLL_FLL_V1_H__

#include "hal/pulp.h"
#include "archi/fll/fll_v1.h"

typedef enum {
        FLL_CONFIG1=0,
        FLL_CONFIG2=1,
        FLL_INTEGRATOR=2,
} hal_fll_config_e;

#define FLL_SOC_STATUS_REG              	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_SOC_OFFSET+FLL_STATUS_REG_OFFSET)
#define FLL_SOC_CONF_REG1               	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_SOC_OFFSET+FLL_CONF_REG1_OFFSET)
#define FLL_SOC_CONF_REG2               	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_SOC_OFFSET+FLL_CONF_REG2_OFFSET)
#define FLL_SOC_INTEGRATOR_REG          	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_SOC_OFFSET+FLL_INTEGRATOR_REG_OFFSET)

#define FLL_CLUSTER_STATUS_REG          	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_CLUSTER_OFFSET+FLL_STATUS_REG_OFFSET)
#define FLL_CLUSTER_CONF_REG1           	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_CLUSTER_OFFSET+FLL_CONF_REG1_OFFSET)
#define FLL_CLUSTER_CONF_REG2           	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_CLUSTER_OFFSET+FLL_CONF_REG2_OFFSET)
#define FLL_CLUSTER_INTEGRATOR_REG      	(ARCHI_SOC_PERIPHERALS_ADDR+FLL_CLUSTER_OFFSET+FLL_INTEGRATOR_REG_OFFSET)

static inline unsigned int GetFllStatus(int Fll) {
        return (unsigned int) IP_READ((int *) ARCHI_SOC_PERIPHERALS_ADDR, (int) (FLL_SOC_OFFSET+FLL_STATUS_REG_OFFSET));
}

static inline unsigned int GetFllConfiguration(int Fll, hal_fll_config_e Reg) {
        int Offset = (int) (FLL_SOC_OFFSET+FLL_CONF_REG1_OFFSET + Reg*FLL_CONF_OFFSET_REG2REG + Fll*FLL_OFFSET_SOC_CLUSTER);
        return (unsigned int) IP_READ(ARCHI_SOC_PERIPHERALS_ADDR, Offset);
}

static inline void SetFllConfiguration(int Fll, hal_fll_config_e Reg, unsigned int Value) {
        IP_WRITE(ARCHI_SOC_PERIPHERALS_ADDR, FLL_SOC_OFFSET+FLL_CONF_REG1_OFFSET + Reg*FLL_CONF_OFFSET_REG2REG + Fll*FLL_OFFSET_SOC_CLUSTER, Value);
}

static inline unsigned int hal_fll_status_reg_get(int fll)
{
  return IP_READ(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_STATUS_REG_OFFSET + fll*HAL_FLL_AREA_SIZE
  );
}

static inline void hal_fll_conf_reg1_set(int fll, unsigned int value)
{
  IP_WRITE(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_CONF_REG1_OFFSET + fll*HAL_FLL_AREA_SIZE,
    value
  );
}

static inline unsigned int hal_fll_conf_reg1_get(int fll)
{
  return IP_READ(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_CONF_REG1_OFFSET + fll*HAL_FLL_AREA_SIZE
  );
}

static inline void hal_fll_conf_reg2_set(int fll, unsigned int value)
{
  IP_WRITE(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_CONF_REG2_OFFSET + fll*HAL_FLL_AREA_SIZE,
    value
  );
}

static inline unsigned int hal_fll_conf_reg2_get(int fll)
{
  return IP_READ(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_CONF_REG2_OFFSET + fll*HAL_FLL_AREA_SIZE
  );
}

static inline void hal_fll_integrator_set(int fll, unsigned int value)
{
  IP_WRITE(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_INTEGRATOR_REG_OFFSET + fll*HAL_FLL_AREA_SIZE,
    value
  );
}

static inline unsigned int hal_fll_integrator_get(int fll)
{
  return IP_READ(
    ARCHI_SOC_PERIPHERALS_ADDR + FLL_SOC_OFFSET, 
    FLL_INTEGRATOR_REG_OFFSET + fll*HAL_FLL_AREA_SIZE
  );
}


static inline void hal_fll_update_mult_div(int fll, int mult, int div)
{  
  hal_fll_reg1_t reg = { .raw = hal_fll_conf_reg1_get(fll) };
  reg.mult_factor = mult;
  reg.clock_out_divider = div;
  hal_fll_conf_reg1_set(fll, reg.raw);
}

#endif
