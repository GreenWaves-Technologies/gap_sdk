/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#include "pmsis.h"
#include "pmsis/implem/drivers/pmu/pmu_internal.h"
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Boot from after wake up. */
#define PI_PMU_BOOT_DEV_ROM ( 0 )
#define PI_PMU_BOOT_DEV_L2  ( 1 )

/* Power state after wake up. */
#define PI_PMU_BOOT_POW_STATE_SOC_LV ( 0 )
#define PI_PMU_BOOT_POW_STATE_SOC_HV ( 1 )

/* FLL retentive. */
#define PI_PMU_FLL_RET_OFF ( 0 )
#define PI_PMU_FLL_RET_ON  ( 1 )

/* L2 memory retentive. */
#define PI_PMU_L2_MEM_RET_OFF ( 0 )
#define PI_PMU_L2_MEM_RET_ON  ( 1 )


#define PI_PMU_MV_TO_DCDC(mV)           ( (PI_PMU_DCDC_LOW_DCDC_VALUE + (((mV) - PI_PMU_DCDC_LOW_MV_VALUE) / PI_PMU_DCDC_STEP_MV)) )
#define PI_PMU_DCDC_TO_MV(Dc)           ( ((((Dc) - PI_PMU_DCDC_LOW_DCDC_VALUE) * PI_PMU_DCDC_STEP_MV) + PI_PMU_DCDC_LOW_MV_VALUE) )

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/* 2 domains : FC + CL. */
static struct pmu_data_s g_pmu_data = {0};
/**
 * WIU_IFR1[0] = SCU_4 SoC-sleep, CL-sleep
 * WIU_IFR1[1] = SCU_5 SoC-ret, CL-sleep
 * WIU_IFR1[2] = SCU_0 SoC-NV, CL-sleep
 * WIU_IFR1[3] = SCU_1 SoC-LV, CL-sleep
 * WIU_IFR1[4] = SCU_2 SoC-NV, CL-ON
 * WIU_IFR1[5] = SCU_3 SoC-LV, CL-ON
 */
typedef enum
{
    PI_PMU_SCU_SEQ_DEEP_SLEEP     = 0, /* SCU seq 4 */
    PI_PMU_SCU_SEQ_RET_DEEP_SLEEP = 1, /* SCU seq 5 */
    PI_PMU_SCU_SEQ_HP_CLUSTER_OFF = 2, /* SCU seq 0 */
    PI_PMU_SCU_SEQ_LP_CLUSTER_OFF = 3, /* SCU seq 1 */
    PI_PMU_SCU_SEQ_HP_CLUSTER_ON  = 4, /* SCU seq 2 */
    PI_PMU_SCU_SEQ_LP_CLUSTER_ON  = 5  /* SCU seq 3 */
} pi_pmu_scu_seq_e;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static void __pi_pmu_handler(void *arg);

static void __pi_pmu_task_fifo_enqueue(pi_task_t *task);

static pi_task_t *__pi_pmu_task_fifo_pop();

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_pmu_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    PMU_TRACE("Event=%ld\n", event);

    pi_task_t *task = g_pmu_data.fifo_head;
    if ((task != NULL) && (task->data[0] == event))
    {
        __pi_pmu_task_fifo_pop();
        pi_task_release(task);
    }
}


static void __pi_pmu_task_fifo_enqueue(pi_task_t *task)
{
    uint32_t irq = __disable_irq();
    task->next = NULL;
    if (g_pmu_data.fifo_head == NULL)
    {
        g_pmu_data.fifo_head = task;
    }
    else
    {
        g_pmu_data.fifo_tail->next = task;
    }
    g_pmu_data.fifo_tail = task;
    __restore_irq(irq);
}

static pi_task_t *__pi_pmu_task_fifo_pop()
{
    //uint32_t irq = __disable_irq();
    struct pi_task *task_to_return = NULL;
    if (g_pmu_data.fifo_head != NULL)
    {
        task_to_return = g_pmu_data.fifo_head;
        g_pmu_data.fifo_head = g_pmu_data.fifo_head->next;
    }
    //__restore_irq(irq);
    return task_to_return;
}

static void __pi_pmu_regulator_state_change(pi_pmu_scu_seq_e scu_seq)
{
    #if !defined(__PLATFORM_FPGA__)
    PMU_TRACE("Changing PMU state : current state SoC=%x, cluster=%x, regulator=%lx\n",
              g_pmu_data.pmu_state, g_pmu_data.cluster_state, g_pmu_data.dcdc_regulator.word);

    uint16_t paddr = (uint16_t) WIU_IFR_1;
    uint16_t pwdata = (1 << scu_seq);
    pi_task_t task = {0};

    PMU_TRACE("Sending seq=%x : data=%x, addr=%x -> %lx\n",
              scu_seq, pwdata, paddr, PMU_DLC_DLC_PCTRL_PWDATA(pwdata) |
              PMU_DLC_DLC_PCTRL_DIR(0) |
              PMU_DLC_DLC_PCTRL_PADDR(paddr) |
              PMU_DLC_DLC_PCTRL_START(1));
    pi_task_block(&task);
    task.data[0] = (uint32_t) SOC_EVENT_PMU_DLC_BRIDGE_SCU;
    __pi_pmu_task_fifo_enqueue(&task);
    hal_pmu_dlc_pctrl_set(paddr, 0, pwdata);
    pi_task_wait_on(&task);

    uint32_t pmu_dlc_ifr = hal_pmu_dlc_ifr_get();
    if (pmu_dlc_ifr & PI_PMU_MAESTRO_EVENT_ICU_OK)
    {
        hal_pmu_dlc_ioifr_get();
    }
    if (pmu_dlc_ifr & PI_PMU_MAESTRO_EVENT_ICU_DELAYED)
    {
        hal_pmu_dlc_idifr_get();
    }
    if (pmu_dlc_ifr & PI_PMU_MAESTRO_EVENT_MODE_CHANGED)
    {
        hal_pmu_dlc_imcifr_get();
    }
    if (pmu_dlc_ifr & (PI_PMU_MAESTRO_EVENT_SCU_OK | PI_PMU_MAESTRO_EVENT_PICL_OK))
    {
        hal_pmu_dlc_ifr_mask_clear((PI_PMU_MAESTRO_EVENT_SCU_OK | PI_PMU_MAESTRO_EVENT_PICL_OK));
    }

    /* Update states. */
    if (scu_seq >= PI_PMU_SCU_SEQ_LP_CLUSTER_ON)
    {
        g_pmu_data.cluster_state = PI_PMU_DOMAIN_STATE_ON;
        g_pmu_data.pmu_state = (scu_seq & 0x1);
    }
    else
    {
        g_pmu_data.cluster_state = PI_PMU_DOMAIN_STATE_OFF;
        g_pmu_data.pmu_state = (scu_seq & 0x1);
    }
    PMU_TRACE("Changing PMU state : new state SoC=%x, cluster=%x\n",
              g_pmu_data.pmu_state, g_pmu_data.cluster_state);
    #endif  /* __PLATFORM_FPGA__ */
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_pmu_init(void)
{
    g_pmu_data.fifo_head = NULL;
    g_pmu_data.fifo_tail = NULL;
    g_pmu_data.dcdc_regulator.word = 0;
    g_pmu_data.sleepctrl.word = 0;
    g_pmu_data.sleepcfg.word = 0;
    g_pmu_data.pmu_state = 0;
    g_pmu_data.cluster_state = 0;
    g_pmu_data.sleep_state = 0;
    g_pmu_data.cur_voltage = 0;

    #if !defined(__PLATFORM_FPGA__)
    g_pmu_data.dcdc_regulator.word = hal_soc_ctrl_pmu_rar_get();
    g_pmu_data.sleepctrl.word = hal_soc_ctrl_pmu_sleepctrl_get();
    g_pmu_data.pmu_state = g_pmu_data.sleepctrl.field.wakestate;
    g_pmu_data.cluster_state = g_pmu_data.sleepctrl.field.cl_wake;
    g_pmu_data.cur_voltage = (uint32_t) PI_PMU_DCDC_TO_MV(g_pmu_data.dcdc_regulator.field.nv_volt);
    PMU_TRACE("PMU init state : RAR=%lx, SLEEPCTRL=%lx, PowerState=%d, Cluster=%d, voltage=%ld\n",
              g_pmu_data.dcdc_regulator.word, g_pmu_data.sleepctrl.word,
              g_pmu_data.pmu_state, g_pmu_data.cluster_state, g_pmu_data.cur_voltage);

    /* Bypass cluster. */
    soc_ctrl_cl_bypass_t cl_bypass;
    cl_bypass.word = hal_soc_ctrl_cl_bypass_get();
    cl_bypass.field.byp_pow = 1;
    cl_bypass.field.byp_clk = 1;
    hal_soc_ctrl_cl_bypass_mask_set(cl_bypass.word);

    /* TODO : Init out of PMU? */
    /* Init FLLs. */
    pi_fll_init(FLL_SOC, 0);

    /* Set handlers. */
    pi_fc_event_handler_set(SOC_EVENT_PMU_CLUSTER_POWER, __pi_pmu_handler);
    pi_fc_event_handler_set(SOC_EVENT_PMU_CLUSTER_CG, __pi_pmu_handler);
    pi_fc_event_handler_set(SOC_EVENT_PMU_DLC_BRIDGE_PICL, __pi_pmu_handler);
    pi_fc_event_handler_set(SOC_EVENT_PMU_DLC_BRIDGE_SCU, __pi_pmu_handler);

    /* Enable SoC events propagation from PMU_DLC/Maestro. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_PMU_CLUSTER_POWER);
    hal_soc_eu_set_fc_mask(SOC_EVENT_PMU_CLUSTER_CG);
    hal_soc_eu_set_fc_mask(SOC_EVENT_PMU_DLC_BRIDGE_PICL);
    hal_soc_eu_set_fc_mask(SOC_EVENT_PMU_DLC_BRIDGE_SCU);

    /* Disable all Maestro interrupts but PICL_OK and SCU_OK. */
    uint32_t imr_mask = (PMU_DLC_DLC_IMR_ICU_OK_MASK |
                         PMU_DLC_DLC_IMR_ICU_DLYD_MASK |
                         PMU_DLC_DLC_IMR_ICU_MD_CHG_MASK);
    hal_pmu_dlc_imr_mask_set(imr_mask);

    /* Clear PICL_OK and SCU_OK flags if set. */
    uint32_t ifr_mask = (PMU_DLC_DLC_IFR_PICL_OK_MASK | PMU_DLC_DLC_IFR_SCU_OK_MASK);
    hal_pmu_dlc_ifr_mask_clear(ifr_mask);
    #endif  /* __PLATFORM_FPGA__ */
    PMU_TRACE("PMU opened and init.\n");
}

int __pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage)
{
    if ((voltage < 800) || (1300 < voltage))
    {
        PMU_TRACE_ERR("Power domain id=%d, Voltage setting is out of boundaries "
                      "800mV < %ld < 1300mV.\n",
                      domain, voltage);
        return -11;
    }
    /* Check freq. */
    pi_pmu_state_e next_state = (voltage > 1100) ? PI_PMU_STATE_HV : PI_PMU_STATE_LV;
    soc_ctrl_safe_pmu_rar_t regulator = {0};
    regulator.word = g_pmu_data.dcdc_regulator.word;
    uint8_t cur_dcdc = 0;
    uint8_t new_dcdc = PI_PMU_MV_TO_DCDC(voltage);
    pi_pmu_scu_seq_e scu_seq = PI_PMU_SCU_SEQ_HP_CLUSTER_OFF;
    if (g_pmu_data.pmu_state == PI_PMU_STATE_HV)
    {
        cur_dcdc = (regulator.word & SOC_CTRL_SAFE_PMU_RAR_LV_VOLT_MASK) >> SOC_CTRL_SAFE_PMU_RAR_LV_VOLT_SHIFT;
        next_state = PI_PMU_STATE_LV;
        scu_seq = PI_PMU_SCU_SEQ_LP_CLUSTER_OFF;
        regulator.word &= ~SOC_CTRL_SAFE_PMU_RAR_LV_VOLT_MASK;
        regulator.word |= SOC_CTRL_SAFE_PMU_RAR_LV_VOLT(new_dcdc);
    }
    else
    {
        cur_dcdc = (regulator.word & SOC_CTRL_SAFE_PMU_RAR_NV_VOLT_MASK) >> SOC_CTRL_SAFE_PMU_RAR_NV_VOLT_SHIFT;
        next_state = PI_PMU_STATE_HV;
        scu_seq = PI_PMU_SCU_SEQ_HP_CLUSTER_OFF;
        regulator.word &= ~SOC_CTRL_SAFE_PMU_RAR_NV_VOLT_MASK;
        regulator.word |= SOC_CTRL_SAFE_PMU_RAR_NV_VOLT(new_dcdc);
    }
    if (new_dcdc == cur_dcdc)
    {
        PMU_TRACE("Voltage already set\n");
        return 0;
    }
    PMU_TRACE("Setting voltage=%ld, old dcdc=%lx, new=%lx\n", voltage, cur_dcdc, new_dcdc);

    /* Set voltage. */
    hal_soc_ctrl_pmu_regulator_set(regulator.word);
    g_pmu_data.dcdc_regulator.word = hal_soc_ctrl_pmu_rar_get();
    PMU_TRACE("New regulator value = %lx\n", g_pmu_data.dcdc_regulator.word);

    /* Change PMU state to change voltage. */
    uint8_t scu_seq_int = (uint8_t) scu_seq + (g_pmu_data.cluster_state << 1);
    PMU_TRACE("Calling regulator with sequence=%x\n", scu_seq_int);
    __pi_pmu_regulator_state_change((pi_pmu_scu_seq_e) scu_seq_int);

    /* Update current voltage. */
    g_pmu_data.cur_voltage = voltage;
    return 0;
}

int __pi_pmu_state_get(pi_pmu_domain_e domain)
{
    int status = -11;
    switch (domain)
    {
    case PI_PMU_DOMAIN_FC :
        status = PI_PMU_DOMAIN_STATE_ON; /* This domain has to be on. */
        break;

    case PI_PMU_DOMAIN_CL :
        status = g_pmu_data.cluster_state;
        break;

    default :
        PMU_TRACE_ERR("Unknown power domain on GAP8 !\n");
        break;
    }
    return status;
}

int __pi_pmu_boot_state_get(pi_pmu_domain_e domain)
{
    return g_pmu_data.sleepctrl.field.bttype;
}

int __pi_pmu_sleep_mode_set(pi_pmu_domain_e domain,
                            struct pi_pmu_sleep_conf_s *conf)
{
    uint32_t irq = disable_irq();
    if (domain != PI_PMU_DOMAIN_FC)
    {
        PMU_TRACE_ERR("Power domain id=%d, trying sleep mode.\n", domain);
        restore_irq(irq);
        return -11;
    }
    g_pmu_data.sleep_state = conf->sleep_mode;

    soc_ctrl_safe_pmu_sleepctrl_t sleepctrl = g_pmu_data.sleepctrl;
    if (conf->sleep_mode == PI_PMU_MODE_DEEP_SLEEP)
    {
        sleepctrl.field.bttype = PI_PMU_BOOT_DEEP_SLEEP;
        sleepctrl.field.btdev  = PI_PMU_BOOT_DEV_ROM;
        sleepctrl.field.l2_r3  = PI_PMU_L2_MEM_RET_OFF;
        sleepctrl.field.l2_r2  = PI_PMU_L2_MEM_RET_OFF;
        sleepctrl.field.l2_r1  = PI_PMU_L2_MEM_RET_OFF;
        sleepctrl.field.l2_r0  = PI_PMU_L2_MEM_RET_OFF;
    }
    else
    {
        sleepctrl.field.bttype = PI_PMU_BOOT_RET_DEEP_SLEEP;
        sleepctrl.field.btdev  = PI_PMU_BOOT_DEV_L2;
        sleepctrl.field.l2_r3  = PI_PMU_L2_MEM_RET_ON;
        sleepctrl.field.l2_r2  = PI_PMU_L2_MEM_RET_ON;
        sleepctrl.field.l2_r1  = PI_PMU_L2_MEM_RET_ON;
        sleepctrl.field.l2_r0  = PI_PMU_L2_MEM_RET_ON;
    }
    sleepctrl.field.wakestate    = conf->power_state;
    sleepctrl.field.extint       = conf->wakeup;
    sleepctrl.field.extwake_en   = 1;
    sleepctrl.field.extwake_type = conf->gpio_notif;
    sleepctrl.field.extwake_src  = conf->gpio_pin;
    sleepctrl.field.cl_wake      = PI_PMU_DOMAIN_STATE_OFF;
    sleepctrl.field.cl_fll       = PI_PMU_FLL_RET_OFF;
    sleepctrl.field.soc_fll      = PI_PMU_FLL_RET_ON;
    g_pmu_data.sleepcfg = sleepctrl;
    PMU_TRACE("Configuring sleep mode : sleepcfg=%lx:\n", g_pmu_data.sleepcfg);
    PMU_TRACE("cl_wake=%x, boottype=%x, ext_int=%x, boot_dev=%x, wakestate=%x, "
              "ext_wake_en=%x, ext_wake_type=%x, ext_wake_src=%x, cl_fll=%x, "
              "soc_fll=%x, l2_r3=%x, l2_r2=%x, l2_r1=%x, l2_r0=%x\n",
              sleepctrl.field.cl_wake, sleepctrl.field.bttype, sleepctrl.field.extint,
              sleepctrl.field.btdev, sleepctrl.field.wakestate, sleepctrl.field.extwake_en,
              sleepctrl.field.extwake_type, sleepctrl.field.extwake_src, sleepctrl.field.cl_fll,
              sleepctrl.field.soc_fll, sleepctrl.field.l2_r3, sleepctrl.field.l2_r2,
              sleepctrl.field.l2_r1, sleepctrl.field.l2_r0);

    restore_irq(irq);

    return 0;
}

int __pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain)
{
    soc_ctrl_safe_pmu_sleepctrl_t sleepcfg = g_pmu_data.sleepcfg;
    hal_soc_ctrl_pmu_sleepctrl_mask_set(sleepcfg.word);
    hal_soc_ctrl_pmu_sleepctrl_get();
    PMU_TRACE("Set sleep mode : sleepcfg=%lx:\n", hal_soc_ctrl_pmu_sleepctrl_get());
    pi_pmu_scu_seq_e scu_seq = PI_PMU_SCU_SEQ_DEEP_SLEEP;
    if (g_pmu_data.sleep_state == PI_PMU_MODE_RET_DEEP_SLEEP)
    {
        scu_seq = PI_PMU_SCU_SEQ_RET_DEEP_SLEEP;
    }
    __pi_pmu_regulator_state_change(scu_seq);
    return 0;
}

#if defined(FEATURE_CLUSTER)
void __pi_pmu_cluster_power_on(void)
{
    if (g_pmu_data.cluster_state == PI_PMU_DOMAIN_STATE_OFF)
    {
        #if !defined(__PLATFORM_FPGA__)
        PMU_TRACE("Power on cluster\n");
        pi_task_t task = {0};
        soc_ctrl_cl_bypass_t bypass;
        bypass.word = hal_soc_ctrl_cl_bypass_get();
        if (bypass.field.cg == 0)
        {
            bypass.field.cg = 1;
            pi_task_block(&task);
            task.data[0] = (uint32_t) SOC_EVENT_PMU_CLUSTER_CG;
            __pi_pmu_task_fifo_enqueue(&task);
            hal_soc_ctrl_cl_bypass_mask_set(bypass.word);
            pi_task_wait_on(&task);
        }
        /* Turn on power. */
        bypass.field.cl_state = 1;
        pi_task_block(&task);
        task.data[0] = (uint32_t) SOC_EVENT_PMU_CLUSTER_POWER;
        __pi_pmu_task_fifo_enqueue(&task);
        hal_soc_ctrl_cl_bypass_mask_set(bypass.word);
        pi_task_wait_on(&task);

        /* Do not isolate cluster. */
        hal_soc_ctrl_cl_isolate_set(0);

        /* Reset cluster. */
        bypass.field.rst = 0;
        hal_soc_ctrl_cl_bypass_mask_set(bypass.word);

        /* Disable cluster CG. */
        bypass.field.cg = 0;
        pi_task_block(&task);
        task.data[0] = (uint32_t) SOC_EVENT_PMU_CLUSTER_CG;
        __pi_pmu_task_fifo_enqueue(&task);
        hal_soc_ctrl_cl_bypass_mask_set(bypass.word);
        pi_task_wait_on(&task);

        /* Set cluster FLL if not retentive. */
        if (g_pmu_data.sleepctrl.field.cl_fll == PI_PMU_FLL_RET_OFF)
        {
            pi_fll_init(FLL_CLUSTER, 0);
        }

        /* Tell external loader (such as gdb) that the cluster is on so that it can take it into account. */
        bypass.field.byp_iso = 1;   /* bit14 = SOC_CTRL_CL_BYPASS_BYP_ISO. */
        hal_soc_ctrl_cl_bypass_mask_set(bypass.word);

        g_pmu_data.cluster_state = PI_PMU_DOMAIN_STATE_ON;
        #else
        /* Do not isolate cluster. */
        hal_soc_ctrl_cl_isolate_set(0);
        #endif  /* __PLATFORM_FPGA__ */
    }
}

void __pi_pmu_cluster_power_off(void)
{
    if (g_pmu_data.cluster_state == PI_PMU_DOMAIN_STATE_ON)
    {
        #if !defined(__PLATFORM_FPGA__)
        PMU_TRACE("Power off cluster\n");
        pi_task_t task = {0};
        soc_ctrl_cl_bypass_t bypass;
        bypass.word = hal_soc_ctrl_cl_bypass_get();

        if (bypass.field.pwiso == 0)
        {
            /* Clock gate cluster.  */
            bypass.field.cg = 1;
            pi_task_block(&task);
            task.data[0] = (uint32_t) SOC_EVENT_PMU_CLUSTER_CG;
            __pi_pmu_task_fifo_enqueue(&task);
            hal_soc_ctrl_cl_bypass_mask_set(bypass.word);
            pi_task_wait_on(&task);

            /* Isolate cluster. */
            hal_soc_ctrl_cl_isolate_set(1);

            /* Turn off power. */
            pi_task_block(&task);
            task.data[0] = (uint32_t) SOC_EVENT_PMU_CLUSTER_POWER;
            bypass.field.cl_state = 0;
            __pi_pmu_task_fifo_enqueue(&task);
            hal_soc_ctrl_cl_bypass_mask_set(bypass.word);
            pi_task_wait_on(&task);

            /* Reset cluster. */
            bypass.field.rst = 1;
            hal_soc_ctrl_cl_bypass_mask_set(bypass.word);

            g_pmu_data.cluster_state = PI_PMU_DOMAIN_STATE_OFF;
        }
        #else
        /* Isolate cluster. */
        hal_soc_ctrl_cl_isolate_set(1);
        #endif  /* __PLATFORM_FPGA__ */
    }
}
#endif  /* FEATURE_CLUSTER */
