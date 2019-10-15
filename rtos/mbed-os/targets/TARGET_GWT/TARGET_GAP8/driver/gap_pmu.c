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
#include <stdlib.h>
#include "gap_pmu.h"
#include "gap_dmacpy.h"


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

uint32_t    PMU_Sleep_Ctrl;
pmu_state_t PMU_State = {0, 0, {0, 0, 0, 0}, {0, 0}};

static uint8_t PMU_RET_State[] = {
  uPMU_SOC_HP,               /* 0: SOC_HP */
  uPMU_SOC_LP,               /* 1: SOC_LP */
  uPMU_SOC_CLUSTER_HP,       /* 2: SOC_CLUSTER_HP */
  uPMU_SOC_CLUSTER_LP        /* 3: SOC_CLUSTER_LP */
};

static uint8_t DCDC_HW_Operating_Point[] = {
  /* 0: REGU_NV  */ (uPMU_DCDC_Nominal   << 3),    /* DCDC Nominal Voltage */
  /* 1: REGU_LV  */ (uPMU_DCDC_Low       << 3),        /* DCDC Low Voltage */
  /* 2: REGU_RET */ (uPMU_DCDC_Retentive << 3)  /* DCDC Retentive Voltage */
  /* 3: REGU_OFF */
};

static uint8_t SystemState_To_SCU_Fast_Seq[7] = {
  [uPMU_SOC_HP]         = 2,
  [uPMU_SOC_LP]         = 3,
  [uPMU_SOC_CLUSTER_HP] = 2,
  [uPMU_SOC_CLUSTER_LP] = 3,
  [uPMU_RETENTIVE]      = 1,
  [uPMU_DEEP_SLEEP]     = 0
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
static void PMU_ChangeDCDC(pmu_dcdc_hw_operatingpoint_t operating_point, uint32_t value)
{
    /* PMU_CTRL->RAR_DCDC |= (value & DCDC_RANGE_MASK) << DCDC_HW_Operating_Point[dcdc_operating_point]; */
    PMU_CTRL->RAR_DCDC = __BIT_INSERT_R(PMU_CTRL->RAR_DCDC,
                                        value,
                                        DCDC_RANGE,
                                        DCDC_HW_Operating_Point[operating_point]);
}

static uint32_t PMU_ChangeRegulatorState(pmu_system_state_t prev_state, pmu_system_state_t curr_state)
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
            EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
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
    #endif

    return uPMU_CHANGE_OK;
}

/*!
 * @brief Flls init.
 *
 * This function initialize the pmu.
 *
 * @note .
 */
static void PMU_FLLSInit()
{
    /* Initial FC SOC domain FLL */
    FLL_Init(uFLL_SOC, READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_SOC_RET(PMU_Sleep_Ctrl));

    #ifdef FEATURE_CLUSTER
    /* Initial Cluster SOC domain FLL if cluster is power on */
    if (PMU_ClusterIsOn())
        FLL_Init(uFLL_CLUSTER, READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET(PMU_Sleep_Ctrl));
    #endif
}

uint32_t fc_data_save_l2_addr = 0x1c000000;
extern uint32_t __fc_data_start;
extern uint32_t __fc_data_size;

static void PMU_RetentiveSaveFCData()
{
    if (((uint32_t)&__fc_data_size) > 0)
    {
        fc_data_save_l2_addr = (uint32_t) malloc ((uint32_t)&__fc_data_size);
        #ifdef GAP8_V2
        DMACPY_Init( DMACPY );

        DMACPY_BlockingTransfer( DMACPY, &__fc_data_start, (uint32_t *)fc_data_save_l2_addr, (uint32_t)&__fc_data_size, uDMACPY_FC2L2 );

        DMACPY_Deinit( DMACPY );
        #else
        memcpy ((char *)fc_data_save_l2_addr, (char *)&__fc_data_start, (size_t)&__fc_data_size);
        #endif
    }
}

static void PMU_RetentiveRestoreFCData()
{
    if (((uint32_t)&__fc_data_size) > 0)
    {
        #ifdef GAP8_V2
        DMACPY_Init( DMACPY );

        DMACPY_BlockingTransfer( DMACPY, (uint32_t *)fc_data_save_l2_addr, &__fc_data_start, (uint32_t)&__fc_data_size, uDMACPY_L22FC );

        DMACPY_Deinit( DMACPY );
        #else
        memcpy ((char *)&__fc_data_start, (char *)fc_data_save_l2_addr, (size_t)&__fc_data_size);
        #endif
        free((void*)fc_data_save_l2_addr);
    }
}

void PMU_Init()
{
    #if !defined( __PLATFORM_FPGA__)
    uint32_t regulator_mode = PMU_CTRL->RAR_DCDC;
    uint32_t Bypass;

    #ifdef FEATURE_CLUSTER
    /* Sync var, has to be init */
    pmu_events_status = 0;
    #endif

    PMU_Sleep_Ctrl  = PMU_CTRL->SLEEP_CTRL;

    PMU_State.State = PMU_RET_State[READ_PMU_CTRL_SLEEP_CTRL_WAKEUP(PMU_Sleep_Ctrl)];

    PMU_State.DCDC_Settings[uPMU_REGU_NV]  = READ_PMU_CTRL_RAR_DCDC_NV(regulator_mode);
    PMU_State.DCDC_Settings[uPMU_REGU_LV]  = READ_PMU_CTRL_RAR_DCDC_LV(regulator_mode);
    PMU_State.DCDC_Settings[uPMU_REGU_RET] = READ_PMU_CTRL_RAR_DCDC_RV(regulator_mode);
    PMU_State.DCDC_Settings[uPMU_REGU_OFF] = 0;

    Bypass = SOC_CTRL->CLUSTER_BYPASS;
    Bypass |= (SOC_CTRL_CLUSTER_BYPASS_BYP_POW(1) |
               SOC_CTRL_CLUSTER_BYPASS_BYP_CLK(1));
    SOC_CTRL->CLUSTER_BYPASS = Bypass;

    PMU_FLLSInit();

    /* Setup interrupt sensibility list, we could remove icu_ok, icu_delayed and icu_mode_changed */
    /* picl_ok and scu_ok are visible icu_ok, icu_delayed, icu_mode_changed and picl_ok are masked */
    SOC_EU_SetFCMask(PMU_EVENT_CLUSTER_POWER_ON);
    SOC_EU_SetFCMask(PMU_EVENT_CLUSTER_CLOCK_GATING);
    SOC_EU_SetFCMask(PMU_DLC_EVENT_BRIDGE_PICL_OK);
    SOC_EU_SetFCMask(PMU_DLC_EVENT_BRIDGE_SCU_OK);

    /* Disable all Maestro interrupts but PICL_OK and SCU_OK */
    PMU_DLC->DLC_IMR = PMU_DLC_IMR_ICU_OK_MASK(1) |
                       PMU_DLC_IMR_ICU_DELAYED_MASK(1) |
                       PMU_DLC_IMR_ICU_MODE_CHANGED_MASK(1);

    /* Clear PICL_OK and SCU_OK interrupts in case they are set */
    PMU_DLC->DLC_IFR = (PMU_DLC_IFR_PICL_OK_FLAG(1) | PMU_DLC_IFR_SCU_OK_FLAG(1));

    if (PMU_WakeupState() == uPMU_RETENTIVE_SLEEP_BOOT)
    {
        PMU_RetentiveRestoreFCData();
    }

    #endif
}

void PMU_DeInit(int retentive, pmu_system_state_t wakeup_state)
{
    PMU_Sleep_Ctrl &= ~(PMU_CTRL_SLEEP_CTRL_BOOT_L2_MASK |
                        PMU_CTRL_SLEEP_CTRL_REBOOT_MASK  |
                        PMU_CTRL_SLEEP_CTRL_WAKEUP_MASK  |
                        PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP_MASK);

    if (retentive) {
        PMU_Sleep_Ctrl |= PMU_CTRL_SLEEP_CTRL_BOOT_L2(uPMU_BOOT_FROM_L2) |
                          PMU_CTRL_SLEEP_CTRL_REBOOT(uPMU_RETENTIVE_SLEEP_BOOT);
    } else {
        PMU_Sleep_Ctrl |= PMU_CTRL_SLEEP_CTRL_BOOT_L2(uPMU_BOOT_FROM_ROM) |
                          PMU_CTRL_SLEEP_CTRL_REBOOT(uPMU_DEEP_SLEEP_BOOT);
    }

    PMU_Sleep_Ctrl |= PMU_CTRL_SLEEP_CTRL_WAKEUP( READ_PMU_REGULATOR_STATE(wakeup_state) )     |
                      PMU_CTRL_SLEEP_CTRL_CLUSTER_WAKEUP(READ_PMU_CLUSTER_STATE(wakeup_state)) |
                      PMU_CTRL_SLEEP_CTRL_CFG_MEM_RET(0xF);

    /* Clear cluster on in case since at wake up it will not be on*/
    PMU_State.State &= ~PMU_CLUSTER_STATE_MASK;

    /* Set Retentive State*/
    PMU_CTRL->SLEEP_CTRL = PMU_Sleep_Ctrl;

    PMU_ChangeRegulatorState(PMU_State.State, (retentive ? uPMU_RETENTIVE : uPMU_DEEP_SLEEP));
}

int PMU_StateSwitch(pmu_switch_state_t state, pmu_switch_mode_t mode) {
    int retentive = 0;

    /* Need retentive or not */
    if (state == uPMU_SWITCH_DEEP_SLEEP)
    {
        retentive = 0;
    }
    else if (state == uPMU_SWITCH_RETENTIVE_SLEEP)
    {
        retentive = 1;
        PMU_RetentiveSaveFCData();
    }

    /* PMU shutdown, change state and set wakeup state */
    if (mode == uPMU_SWITCH_FAST) {
        PMU_DeInit(retentive, uPMU_SOC_HP);
        return 0;
    } else {
        return -1;
    }
}

pmu_wakeup_state_t PMU_WakeupState() {
    int boot_type = READ_PMU_CTRL_SLEEP_CTRL_REBOOT(PMU_Sleep_Ctrl);

    return (pmu_wakeup_state_t)boot_type;
}

int PMU_SetVoltage(uint32_t voltage, uint32_t frequency_check) {
    #if !defined( __PLATFORM_FPGA__)

    uint32_t curr_dcdc_val = PMU_State.DCDC_Settings[READ_PMU_REGULATOR_STATE(PMU_State.State)];
    uint32_t new_dcdc_val  = mV_TO_DCDC(voltage);
    uint32_t status        = PMU_ERROR_NO_ERROR;

    if (curr_dcdc_val == new_dcdc_val)
        return PMU_ERROR_NO_ERROR;

    if ((voltage < DCDC_DEFAULT_LV) || (voltage > DCDC_DEFAULT_NV))
        return PMU_ERROR_VDD_OUT_OF_RANGE;

    if(frequency_check) {
        if (((int)PMU_State.Frequency[uFLL_SOC] > FLL_SoCMaxFreqAtV(voltage)) ||
            ((READ_PMU_CLUSTER_STATE(PMU_State.State) == uPMU_CLUSTER_ON)
             && ((int)PMU_State.Frequency[uFLL_CLUSTER] > FLL_ClusterMaxFreqAtV(voltage))))
            return PMU_ERROR_FREQ_TOO_HIGH;
    }

    pmu_regulator_state_t regulator_state = (READ_PMU_REGULATOR_STATE(PMU_State.State) == uPMU_REGU_NV) ? uPMU_REGU_LV : uPMU_REGU_NV;
    pmu_system_state_t new_state = PMU_POWER_SYSTEM_STATE(regulator_state, READ_PMU_CLUSTER_STATE(PMU_State.State));

    PMU_ChangeDCDC(regulator_state, new_dcdc_val);
    PMU_ChangeRegulatorState(PMU_State.State, new_state);

    PMU_State.State  = new_state;
    PMU_State.DCDC_Settings[READ_PMU_REGULATOR_STATE(PMU_State.State)] = new_dcdc_val;

    return status;
    #else
    return 0;
    #endif
}

int PMU_EnableGPIOWakeUp(int gpio_number, pmu_gpio_wakeup_t type)
{
    /* Clear bits */
    PMU_Sleep_Ctrl &= ~(PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL_MASK | PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE_MASK);

    /* GPIO wake up configure */
    PMU_Sleep_Ctrl |= PMU_CTRL_SLEEP_CTRL_EXT_WAKE_SEL(gpio_number)  |
                      PMU_CTRL_SLEEP_CTRL_EXT_WAKE_TYPE((int)type)   |
                      PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN(1);

    return (gpio_number < GPIO_NUM);
}

void PMU_DisableGPIOWakeUp() {
    /* Clear enable bits */
    PMU_Sleep_Ctrl &= ~PMU_CTRL_SLEEP_CTRL_EXT_WAKE_EN_MASK;
}

#ifdef FEATURE_CLUSTER

int PMU_ClusterIsOn() {
    return (READ_PMU_CLUSTER_STATE(PMU_State.State) == uPMU_CLUSTER_ON);
}

int PMU_ClusterIsOff() {
    return (READ_PMU_CLUSTER_STATE(PMU_State.State) == uPMU_CLUSTER_OFF);
}

void PMU_ClusterPowerOn() {
    #if !defined( __PLATFORM_FPGA__)
    if(READ_PMU_CLUSTER_STATE(PMU_State.State) == uPMU_CLUSTER_OFF) {
        uint32_t Bypass = SOC_CTRL->CLUSTER_BYPASS;

        if (READ_SOC_CTRL_CLUSTER_BYPASS_CLK_GATE(Bypass) == 0) {
            /* Clock gate FLL cluster */
            Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_CLK_GATE_MASK);
            Bypass |= SOC_CTRL_CLUSTER_BYPASS_CLK_GATE(1);
            SOC_CTRL->CLUSTER_BYPASS = Bypass;

            /* Wait for clock gate done event */
            while ((pmu_events_status & PMU_EVENT_CLUSTER_CLOCK_GATE) ==  0)
            {
                EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
            }
            pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_CLOCK_GATE));
        }

        /* Turn on power */
        Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_POW_MASK);
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_POW(1);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        /* Wait for TRC OK event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_ON_OFF) == 0) {
            EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_ON_OFF));

        /* De assert Isolate on cluster */
        SOC_CTRL->CLUSTER_ISO = 0;

        /* De Assert Reset cluster */
        Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_RESET_MASK);
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_RESET(0);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        /* Clock ungate FLL cluster */
        Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_CLK_GATE_MASK);
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_CLK_GATE(0);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        /* Wait for clock gate done event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_CLOCK_GATE) == 0) {
            EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_CLOCK_GATE));

        /* In case the fll is not in retentive mode we need to configure it */
        if (READ_PMU_CTRL_SLEEP_CTRL_CFG_FLL_CLUSTER_RET(PMU_Sleep_Ctrl) == 0)
            FLL_Init(uFLL_CLUSTER, 0);

        /* Wait for cluster fll to lock */
        // while (ClusterFllConverged() == 0) {};

        /* Tell external loader (such as gdb) that the cluster is on so that it can take it into account */
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_BYP_ISO(1);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        PMU_State.State &= ~PMU_CLUSTER_STATE_MASK;
        PMU_State.State |= PMU_CLUSTER_STATE(uPMU_CLUSTER_ON);
    }
    #else
    /* On the FPGA the only thing to manage is the cluster isolation */
    SOC_CTRL->CLUSTER_ISO = 0;
    #endif
}

void PMU_ClusterPowerOff() {
    #if !defined( __PLATFORM_FPGA__)
    uint32_t Bypass = SOC_CTRL->CLUSTER_BYPASS;

    if((READ_PMU_CLUSTER_STATE(PMU_State.State) == uPMU_CLUSTER_OFF) &&
       !READ_SOC_CTRL_CLUSTER_BYPASS_PW_ISO(Bypass))
    {
        /* Clock gate FLL cluster */
        Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_CLK_GATE_MASK);
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_CLK_GATE(1);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        /* Wait for clock gate done event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_CLOCK_GATE) ==  0)
        {
            EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_CLOCK_GATE));

        SOC_CTRL->CLUSTER_ISO = 1;

        /* Turn on power */
        Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_POW_MASK);
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_POW(0);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        /* Assert Reset Cluster, this is needed for proper behaviour of fll cluster when waking up */
        Bypass &= (~SOC_CTRL_CLUSTER_BYPASS_RESET_MASK);
        Bypass |= SOC_CTRL_CLUSTER_BYPASS_RESET(1);
        SOC_CTRL->CLUSTER_BYPASS = Bypass;

        /* Wait for TRC OK event */
        while ((pmu_events_status & PMU_EVENT_CLUSTER_ON_OFF) == 0) {
            EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
        }
        pmu_events_status = pmu_events_status & (~(PMU_EVENT_CLUSTER_ON_OFF));

        PMU_State.State &= ~PMU_CLUSTER_STATE_MASK;
        PMU_State.State |= PMU_CLUSTER_STATE(uPMU_CLUSTER_OFF);
    }
    #else
    /* On the FPGA the only thing to manage is the cluster isolation */
    SOC_CTRL->CLUSTER_ISO = 1;
    #endif
}
#endif

__attribute__((section(".text")))
void PMU_IRQHandler(uint32_t num)
{
    pmu_events_status |= 1 << (num - PMU_EVENT_CLUSTER_POWER_ON);

    EU_SW_EVENTS_DEMUX->TRIGGER_SET[FC_SW_NOTIFY_EVENT] = 0xFFFFFFFF;
}
