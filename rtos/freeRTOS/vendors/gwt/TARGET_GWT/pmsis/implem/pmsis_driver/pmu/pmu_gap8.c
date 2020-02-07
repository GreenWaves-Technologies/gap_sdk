/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __GAP8__
#include "pmsis_driver/pmu/pmsis_pmu.h"
#include "pmsis_driver/pmsis_fc_event.h"
#include "pmsis_hal/pmsis_hal.h"
//#include "pmsis_hal/gap_eu/pmsis_eu.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief For PMU events
  bit0 : MSP
  bit1 : ICU_MODE_CHANGED
  bit2 : ICU_OK
  bit3 : ICU_DELAYED
  bit4 : PICL_OK
  bit5 : SCU_OK
*/
uint32_t pmu_events_status;

uint32_t    pmu_sleep_ctrl;
pmu_state_t pmu_state = {0, 0, {0, 0, 0, 0}, {0, 0}};

static uint8_t PMU_RET_State[] = {
  PMU_SOC_HP,               /* 0: SOC_HP */
  PMU_SOC_LP,               /* 1: SOC_LP */
  PMU_SOC_CLUSTER_HP,       /* 2: SOC_CLUSTER_HP */
  PMU_SOC_CLUSTER_LP        /* 3: SOC_CLUSTER_LP */
};

static uint8_t DCDC_HW_Operating_Point[] = {
  /* 0: REGU_NV  */ (PMU_DCDC_Nominal   << 3),    /* DCDC Nominal Voltage */
  /* 1: REGU_LV  */ (PMU_DCDC_Low       << 3),        /* DCDC Low Voltage */
  /* 2: REGU_RET */ (PMU_DCDC_Retentive << 3)  /* DCDC Retentive Voltage */
  /* 3: REGU_OFF */
};

static uint8_t SystemState_To_SCU_Fast_Seq[7] = {
  [PMU_SOC_HP]         = 2,
  [PMU_SOC_LP]         = 3,
  [PMU_SOC_CLUSTER_HP] = 2,
  [PMU_SOC_CLUSTER_LP] = 3,
  [PMU_RETENTIVE]      = 1,
  [PMU_DEEP_SLEEP]     = 0
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/
static void pi_pmu_change_dcdc(pmu_dcdc_hw_operatingpoint_t operating_point, uint32_t value)
{
    /* PMU_CTRL->RAR_DCDC |= (value & DCDC_RANGE_MASK) << DCDC_HW_Operating_Point[dcdc_operating_point]; */
    PMU_CTRL->RAR_DCDC = __BIT_INSERT_R(PMU_CTRL->RAR_DCDC,
                                        value,
                                        DCDC_RANGE,
                                        DCDC_HW_Operating_Point[operating_point]);
}

static uint32_t pi_pmu_change_regulator_state(pmu_system_state_t prev_state, pmu_system_state_t curr_state)
{
    #if !defined( __PLATFORM_FPGA__)
    uint32_t scu_new_state = SystemState_To_SCU_Fast_Seq[curr_state];

    /* If Regulator state is changing apply the change now */
    if (READ_PMU_REGULATOR_STATE(curr_state) != READ_PMU_REGULATOR_STATE(prev_state)) {

        /* Maestro driven change state */
        PMU_DLC->PCTRL = PMU_DLC_PCTRL_PWDATA(1 << scu_new_state) |
                         PMU_DLC_PCTRL_PADDR(WIU_IFR_1)           |
                         PMU_DLC_PCTRL_START(1);

        /* Wait for clock gate done event */
        while ((pmu_events_status & PMU_EVENT_SCU_OK) == 0) {
            hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_SCU_OK));

        uint32_t pmu_irq_flags = PMU_DLC->DLC_IFR;
        if (pmu_irq_flags & PMU_MAESTRO_EVENT_ICU_OK)
            __builtin_pulp_OffsetedRead(0, (uint32_t)&PMU_DLC->DLC_IOIFR);
        if (pmu_irq_flags & PMU_MAESTRO_EVENT_ICU_DELAYED)
            __builtin_pulp_OffsetedRead(0, (uint32_t)&PMU_DLC->DLC_IDIFR);
        if (pmu_irq_flags & PMU_MAESTRO_EVENT_MODE_CHANGED)
            __builtin_pulp_OffsetedRead(0, (uint32_t)&PMU_DLC->DLC_IMCIFR);
        if (pmu_irq_flags & (PMU_MAESTRO_EVENT_PICL_OK | PMU_MAESTRO_EVENT_SCU_OK))
            PMU_DLC->DLC_IFR = pmu_irq_flags & (PMU_MAESTRO_EVENT_PICL_OK | PMU_MAESTRO_EVENT_SCU_OK);
    }
    return PMU_CHANGE_OK;
    #endif
}

/*!
 * @brief Flls init.
 *
 * This function initialize the pmu.
 *
 * @note .
 */
static void pi_pmu_flls_init()
{
    /* Initial FC SOC domain FLL */
    pi_fll_init(FLL_SOC, READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET(pmu_sleep_ctrl));

    #ifdef FEATURE_CLUSTER
    /* Initial Cluster SOC domain FLL if cluster is power on */
    if (pi_pmu_cluster_is_on())
        pi_fll_init(FLL_CLUSTER, READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET(pmu_sleep_ctrl));
    #endif
}

void pi_pmu_init()
{
    #if !defined( __PLATFORM_FPGA__)
    uint32_t regulator_mode = PMU_CTRL->RAR_DCDC;
    uint32_t Bypass;

    #ifdef FEATURE_CLUSTER
    /* Sync var, has to be init */
    pmu_events_status = 0;
    #endif

    pmu_sleep_ctrl  = PMU_CTRL->SLEEP_CTRL;

    pmu_state.State = PMU_RET_State[READ_PMU_CTRL_SLEEP_CTRL_WAKEUP(pmu_sleep_ctrl)];

    pmu_state.DCDC_Settings[PMU_REGU_NV]  = READ_PMU_CTRL_RAR_DCDC_NV(regulator_mode);
    pmu_state.DCDC_Settings[PMU_REGU_LV]  = READ_PMU_CTRL_RAR_DCDC_LV(regulator_mode);
    pmu_state.DCDC_Settings[PMU_REGU_RET] = READ_PMU_CTRL_RAR_DCDC_RV(regulator_mode);
    pmu_state.DCDC_Settings[PMU_REGU_OFF] = 0;

    Bypass = soc_ctrl->cl_bypass;
    Bypass |= (SOC_CTRL_CL_BYPASS_BYP_POW(1) | SOC_CTRL_CL_BYPASS_BYP_CLK(1));
    soc_ctrl->cl_bypass = Bypass;

    pi_pmu_flls_init();


    pi_fc_event_handler_set(PMU_EVENT_CLUSTER_POWER_ON, pi_pmu_irq_handler);
    pi_fc_event_handler_set(PMU_EVENT_CLUSTER_CLOCK_GATING, pi_pmu_irq_handler);
    pi_fc_event_handler_set(PMU_DLC_EVENT_BRIDGE_PICL_OK, pi_pmu_irq_handler);
    pi_fc_event_handler_set(PMU_DLC_EVENT_BRIDGE_SCU_OK, pi_pmu_irq_handler);

    /* Setup interrupt sensibility list, we could remove icu_ok, icu_delayed and icu_mode_changed */
    /* picl_ok and scu_ok are visible icu_ok, icu_delayed, icu_mode_changed and picl_ok are masked */
    hal_soc_eu_set_fc_mask(PMU_EVENT_CLUSTER_POWER_ON);
    hal_soc_eu_set_fc_mask(PMU_EVENT_CLUSTER_CLOCK_GATING);
    hal_soc_eu_set_fc_mask(PMU_DLC_EVENT_BRIDGE_PICL_OK);
    hal_soc_eu_set_fc_mask(PMU_DLC_EVENT_BRIDGE_SCU_OK);



    /* Disable all Maestro interrupts but PICL_OK and SCU_OK */
    PMU_DLC->DLC_IMR = PMU_DLC_IMR_ICU_OK_MASK(1) |
                       PMU_DLC_IMR_ICU_DELAYED_MASK(1) |
                       PMU_DLC_IMR_ICU_MODE_CHANGED_MASK(1);

    /* Clear PICL_OK and SCU_OK interrupts in case they are set */
    PMU_DLC->DLC_IFR = (PMU_DLC_IFR_PICL_OK_FLAG(1) | PMU_DLC_IFR_SCU_OK_FLAG(1));
    #endif
}

void pi_pmu_deinit(int retentive, pmu_system_state_t wakeup_state)
{
    pmu_sleep_ctrl &= ~(PMU_CTRL_SLEEP_CTRL_BOOT_L2_MASK |
                        PMU_CTRL_SLEEP_CTRL_REBOOT_MASK  |
                        PMU_CTRL_SLEEP_CTRL_WAKEUP_MASK  |
                        PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_MASK);

    if (retentive) {
        pmu_sleep_ctrl |= PMU_CTRL_SLEEP_CTRL_BOOT_L2(PMU_BOOT_FROM_L2) |
                          PMU_CTRL_SLEEP_CTRL_REBOOT(PMU_RETENTIVE_SLEEP_BOOT);
    } else {
        pmu_sleep_ctrl |= PMU_CTRL_SLEEP_CTRL_BOOT_L2(PMU_BOOT_FROM_ROM) |
                          PMU_CTRL_SLEEP_CTRL_REBOOT(PMU_DEEP_SLEEP_BOOT);
    }

    pmu_sleep_ctrl |= PMU_CTRL_SLEEP_CTRL_WAKEUP( READ_PMU_REGULATOR_STATE(wakeup_state) )     |
                      PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP(READ_PMU_CLUSTER_STATE(wakeup_state)) |
                      PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET(0xF);

    /* Clear cluster on in case since at wake up it will not be on*/
    pmu_state.State &= ~PMU_CLUSTER_STATE_MASK;

    /* Set Retentive State*/
    PMU_CTRL->SLEEP_CTRL = pmu_sleep_ctrl;

    pi_pmu_change_regulator_state(pmu_state.State, (retentive ? PMU_RETENTIVE : PMU_DEEP_SLEEP));
}

int pi_pmu_state_switch(pmu_switch_state_t state, pmu_switch_mode_t mode) {
    int retentive = 0;

    /* Need retentive or not */
    if (state == PMU_SWITCH_DEEP_SLEEP)
        retentive = 0;
    else if (state == PMU_SWITCH_RETENTIVE_SLEEP)
        retentive = 1;

    /* PMU shutdown, change state and set wakeup state */
    if (mode == PMU_SWITCH_FAST) {
        pi_pmu_deinit(retentive, PMU_SOC_HP);
        return 0;
    } else {
        return -1;
    }
}

pmu_wakeup_state_t pi_pmu_wakeup_state() {
    int boot_type = READ_PMU_CTRL_SLEEP_CTRL_REBOOT(pmu_sleep_ctrl);

    return (pmu_wakeup_state_t)boot_type;
}

int pi_pmu_set_voltage(uint32_t voltage, uint32_t frequency_check) {
    #if !defined( __PLATFORM_FPGA__)

    uint32_t curr_dcdc_val = pmu_state.DCDC_Settings[READ_PMU_REGULATOR_STATE(pmu_state.State)];
    uint32_t new_dcdc_val  = mV_TO_DCDC(voltage);
    uint32_t status        = PMU_ERROR_NO_ERROR;

    if (curr_dcdc_val == new_dcdc_val)
        return PMU_ERROR_NO_ERROR;

    if ((voltage < DCDC_DEFAULT_LV) || (voltage > DCDC_DEFAULT_NV))
        return PMU_ERROR_VDD_OUT_OF_RANGE;

    if(frequency_check) {
        if (((int)pmu_state.frequency[FLL_SOC] > pi_fll_soc_max_freq_at_V(voltage)) ||
            ((READ_PMU_CLUSTER_STATE(pmu_state.State) == PMU_CLUSTER_ON)
             && ((int)pmu_state.frequency[FLL_CLUSTER] > pi_fll_cluster_max_freq_at_V(voltage))))
            return PMU_ERROR_FREQ_TOO_HIGH;
    }

    pmu_regulator_state_t regulator_state = (READ_PMU_REGULATOR_STATE(pmu_state.State) == PMU_REGU_NV) ? PMU_REGU_LV : PMU_REGU_NV;
    pmu_system_state_t new_state = PMU_POWER_SYSTEM_STATE(regulator_state, READ_PMU_CLUSTER_STATE(pmu_state.State));

    pi_pmu_change_dcdc(regulator_state, new_dcdc_val);
    pi_pmu_change_regulator_state(pmu_state.State, new_state);

    pmu_state.State  = new_state;
    pmu_state.DCDC_Settings[READ_PMU_REGULATOR_STATE(pmu_state.State)] = new_dcdc_val;

    return status;
    #else
    return 0;
    #endif
}

int pi_pmu_enable_gpio_wakeup(int gpio_number, pmu_gpio_wakeup_t type)
{
    /* Clear bits */
    pmu_sleep_ctrl &= ~(PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_MASK | PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_MASK);

    /* GPIO wake up configure */
    pmu_sleep_ctrl |= PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL(gpio_number)  |
                      PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE((int)type)   |
                      PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN(1);

    return (gpio_number < GPIO_NUM);
}

void po_pmu_disable_gpio_wakeup() {
    /* Clear enable bits */
    pmu_sleep_ctrl &= ~PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_MASK;
}

#ifdef FEATURE_CLUSTER

int pi_pmu_cluster_is_on() {
    return (READ_PMU_CLUSTER_STATE(pmu_state.State) == PMU_CLUSTER_ON);
}

int pi_pmu_cluster_is_off() {
    return (READ_PMU_CLUSTER_STATE(pmu_state.State) == PMU_CLUSTER_OFF);
}

void pi_pmu_cluster_poweron() {
    #if !defined( __PLATFORM_FPGA__)
    if(READ_PMU_CLUSTER_STATE(pmu_state.State) == PMU_CLUSTER_OFF) {
        uint32_t Bypass = soc_ctrl->cl_bypass;

        if (READ_SOC_CTRL_CL_BYPASS_CG(Bypass) == 0) {
            /* Clock gate FLL cluster */
            Bypass &= (~SOC_CTRL_CL_BYPASS_CG_MASK);
            Bypass |= SOC_CTRL_CL_BYPASS_CG(1);
            soc_ctrl->cl_bypass = Bypass;

            /* Wait for clock gate done event */
            while ((pmu_events_status & PMU_EVENT_CLUSTER_CLOCK_GATE) ==  0)
            {
                hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
            }
            pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_CLOCK_GATE));
        }

        /* Turn on power */
        Bypass &= (~SOC_CTRL_CL_BYPASS_CL_STATE_MASK);
        Bypass |= SOC_CTRL_CL_BYPASS_CL_STATE(1);
        soc_ctrl->cl_bypass = Bypass;

        /* Wait for TRC OK event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_ON_OFF) == 0) {
            hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_ON_OFF));

        /* De assert Isolate on cluster */
        soc_ctrl->cl_isolate = 0;

        /* De Assert Reset cluster */
        Bypass &= (~SOC_CTRL_CL_BYPASS_RST_MASK);
        Bypass |= SOC_CTRL_CL_BYPASS_RST(0);
        soc_ctrl->cl_bypass = Bypass;

        /* Clock ungate FLL cluster */
        Bypass &= (~SOC_CTRL_CL_BYPASS_CG_MASK);
        Bypass |= SOC_CTRL_CL_BYPASS_CG(0);
        soc_ctrl->cl_bypass = Bypass;

        /* Wait for clock gate done event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_CLOCK_GATE) == 0) {
            hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_CLOCK_GATE));

        /* In case the fll is not in retentive mode we need to configure it */
        if (READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET(pmu_sleep_ctrl) == 0)
            pi_fll_init(FLL_CLUSTER, 0);

        /* Wait for cluster fll to lock */
        // while (ClusterFllConverged() == 0) {};

        /* Tell external loader (such as gdb) that the cluster is on so that it can take it into account */
        Bypass |= SOC_CTRL_CL_BYPASS_BYP_ISO(1);
        soc_ctrl->cl_bypass = Bypass;

        pmu_state.State &= ~PMU_CLUSTER_STATE_MASK;
        pmu_state.State |= PMU_CLUSTER_STATE(PMU_CLUSTER_ON);
    }
    #else
    /* On the FPGA the only thing to manage is the cluster isolation */
    soc_ctrl->cl_isolate = 0;
    #endif
}

void pi_pmu_cluster_poweroff() {
    #if !defined( __PLATFORM_FPGA__)
    uint32_t Bypass = soc_ctrl->cl_bypass;

    if((READ_PMU_CLUSTER_STATE(pmu_state.State) == PMU_CLUSTER_OFF) &&
       !READ_SOC_CTRL_CL_BYPASS_PWISO(Bypass))
    {
        /* Clock gate FLL cluster */
        Bypass &= (~SOC_CTRL_CL_BYPASS_CG_MASK);
        Bypass |= SOC_CTRL_CL_BYPASS_CG(1);
        soc_ctrl->cl_bypass = Bypass;

        /* Wait for clock gate done event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_CLOCK_GATE) ==  0)
        {
            hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_CLOCK_GATE));

        soc_ctrl->cl_isolate = 1;

        /* Turn off power */
        Bypass &= (~SOC_CTRL_CL_BYPASS_CL_STATE_MASK);
        Bypass |= SOC_CTRL_CL_BYPASS_CL_STATE(0);
        soc_ctrl->cl_bypass = Bypass;

        /* Assert Reset Cluster, this is needed for proper behaviour of fll cluster when waking up */
        Bypass &= (~SOC_CTRL_CL_BYPASS_RST_MASK);
        Bypass |= SOC_CTRL_CL_BYPASS_RST(1);
        soc_ctrl->cl_bypass = Bypass;

        /* Wait for TRC OK event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_ON_OFF) == 0) {
            hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_ON_OFF));

        pmu_state.State &= ~PMU_CLUSTER_STATE_MASK;
        pmu_state.State |= PMU_CLUSTER_STATE(PMU_CLUSTER_OFF);
    }
    #else
    /* On the FPGA the only thing to manage is the cluster isolation */
    soc_ctrl->cl_isolate = 1;
    #endif
}
#endif

__attribute__((section(".text")))
void pi_pmu_irq_handler(void *arg)
{
    uint32_t num = (uint32_t) arg;
    pmu_events_status |= 1 << (num - PMU_EVENT_CLUSTER_POWER_ON);

    EU_SW_EVENTS_DEMUX->TRIGGER_SET[FC_SW_NOTIFY_EVENT] = 0xFFFFFFFF;
}
#endif
