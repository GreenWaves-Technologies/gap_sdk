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

/* 
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include "stdio.h"

#if 0
static RT_FC_TINY_DATA unsigned int __rt_current_voltage;



static int __rt_pmu_get_scu_state(unsigned int voltage)
{
  if (voltage > 1100)
    return SCU_SOC_HP;
  else
    return SCU_SOC_LP;
}



static int __rt_pmu_get_regu_state(unsigned int voltage)
{
  if (voltage > 1100)
    return REGU_NV;
  else
    return REGU_LV;
}



static int __rt_pmu_dcdc_operpoint(int state)
{
  if (state == REGU_NV)
    return DCDC_Nominal*8;
  else
    return DCDC_Low*8;
}


static void PMU_ChangeDCDCSetting(RegulatorStateT DCDC_OperPoint, unsigned int Value)
{
  unsigned int *ConfigReg = (unsigned int *) PMU_DCDC_CONFIG_REG;
  *ConfigReg = __builtin_bitinsert_r(*ConfigReg, Value, DCDC_RANGE, __rt_pmu_dcdc_operpoint(DCDC_OperPoint));
}




static void __rt_pmu_change_regu_state(int state)
{
  PMU_Write(PCTRL, ((1<<state)<<16)|(0x3<<1)|(1));
  __rt_periph_wait_event(ARCHI_SOC_EVENT_SCU_OK, 1);

  unsigned int TheIrqs = PMU_Read(DLC_IFR);
  if (TheIrqs & MAESTRO_EVENT_ICU_OK)       PMU_Read(DLC_IOIFR);
  if (TheIrqs & MAESTRO_EVENT_ICU_DELAYED)  PMU_Read(DLC_IDIFR);
  if (TheIrqs & MAESTRO_EVENT_MODE_CHANGED) PMU_Read(DLC_IMCIFR);
  if (TheIrqs & (MAESTRO_EVENT_PICL_OK|MAESTRO_EVENT_SCU_OK)) PMU_Write(DLC_IFR, TheIrqs & (MAESTRO_EVENT_PICL_OK|MAESTRO_EVENT_SCU_OK));
}



void __rt_pmu_voltage_apply(unsigned int new_voltage)
{
  int new_state = __rt_pmu_get_scu_state(new_voltage);

  // First apply the new voltage
  PMU_ChangeDCDCSetting(
    __rt_pmu_get_regu_state(new_voltage),
    mVtoDCDCSetting(new_voltage)
  );

  // And change the PMU state so that it is taken into account.
  // The caller must make sure we go to a different state
  __rt_pmu_change_regu_state(new_state);

  __rt_current_voltage = new_voltage;
}



int rt_voltage_force(rt_voltage_domain_e domain, unsigned int new_voltage, rt_event_t *event)
{
  int irq = rt_irq_disable();

  int current_state = __rt_pmu_get_scu_state(__rt_current_voltage);
  int new_state = __rt_pmu_get_scu_state(new_voltage);

  // Be careful, the PMU can apply the new voltage only if the PMU state is
  // changed. In case it is the same, we have to insert a dummy transition.
  if (current_state == new_state)
  {
    int stub_voltage = current_state == SCU_SOC_HP ? 1100 : 1200;
    __rt_pmu_voltage_apply(stub_voltage);
  }

  __rt_pmu_voltage_apply(new_voltage);

  if (event) __rt_event_enqueue(event);

  rt_irq_restore(irq);

  return 0;
}



void __rt_pmu_cluster_power_down(rt_event_t *event, int *pending)
{
  if (rt_platform() == ARCHI_PLATFORM_FPGA)
  {
    // On the FPGA the only thing to manage is the cluster isolation
    PMU_IsolateCluster(1);
  }
  else
  {
    PMU_BypassT Bypass;
    Bypass.Raw = GetPMUBypass();

    /* Clock gate FLL cluster */
    Bypass.Fields.ClusterClockGate = 1; SetPMUBypass(Bypass.Raw);

    /* Wait for FLL lock */
    __rt_periph_wait_event(ARCHI_SOC_EVENT_ICU_DELAYED, 1);

    /* Isolate Cluster */
    PMU_IsolateCluster(1);
    /* Turn off power */
    Bypass.Fields.ClusterState = 0; SetPMUBypass(Bypass.Raw);

    /* Assert Reset Cluster, this is needed for proper behaviour of fll cluster when waking up */
    Bypass.Fields.ClusterReset = 1; SetPMUBypass(Bypass.Raw);

    /* Wait for TRC OK event */
    __rt_periph_wait_event(ARCHI_SOC_EVENT_CLUSTER_ON_OFF, 1);
  }
}

void __rt_pmu_cluster_power_up(rt_event_t *event, int *pending)
{
  if (rt_platform() == ARCHI_PLATFORM_FPGA)
  {
    // On the FPGA the only thing to manage is the cluster isolation
    PMU_IsolateCluster(0);
  }
  else
  {
    PMU_BypassT Bypass;

    Bypass.Raw = GetPMUBypass();
    
    if (Bypass.Fields.ClusterClockGate == 0)
    {
      /* Clock gate FLL cluster */
      Bypass.Fields.ClusterClockGate = 1; SetPMUBypass(Bypass.Raw);

      /* Wait for clock gate done event */
      __rt_periph_wait_event(ARCHI_SOC_EVENT_ICU_DELAYED, 1);
    }

    /* Turn on power */
    Bypass.Fields.ClusterState = 1; SetPMUBypass(Bypass.Raw);

    /* Wait for TRC OK event */
    __rt_periph_wait_event(ARCHI_SOC_EVENT_CLUSTER_ON_OFF, 1);

    /* De assert Isolate on cluster */
    PMU_IsolateCluster(0);

    /* De Assert Reset cluster */
    Bypass.Fields.ClusterReset = 0; SetPMUBypass(Bypass.Raw);

    /* Clock ungate FLL cluster */
    Bypass.Fields.ClusterClockGate = 0; SetPMUBypass(Bypass.Raw);

    /* Wait for clock gate done event */
      __rt_periph_wait_event(ARCHI_SOC_EVENT_ICU_DELAYED, 1);

    /* Tell external loader (such as gdb) that the cluster is on so that it can take it into account */
    Bypass.Raw |= 1 << APB_SOC_BYPASS_USER0_BIT; SetPMUBypass(Bypass.Raw);
  }
}

static void __attribute__((constructor)) __rt_pmu_init()
{
  if (rt_platform() != ARCHI_PLATFORM_FPGA)
  {
    PMU_BypassT Bypass;
    Bypass.Raw = GetPMUBypass();
    Bypass.Fields.Bypass = 1; 
    Bypass.Fields.BypassClock = 1;
    SetPMUBypass(Bypass.Raw);

    __rt_current_voltage = 1200;

    /* Disable all Maestro interrupts but PICL_OK and SCU_OK */
    PMU_Write(DLC_IMR, 0x7);
    /* Clear PICL_OK and SCU_OK interrupts in case they are sat */
    PMU_Write(DLC_IFR, (MAESTRO_EVENT_PICL_OK|MAESTRO_EVENT_SCU_OK));
    
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_CLUSTER_ON_OFF);
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_MSP);
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_ICU_MODE_CHANGED);
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_ICU_OK);
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_ICU_DELAYED);
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_PICL_OK);
    soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_SCU_OK);
  }
}

#endif
