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

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static struct pmu_data_s g_pmu_data = {0};
static volatile uint32_t pmu_req = 0;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_pmu_handler(void *arg);

static void __pi_pmu_task_fifo_enqueue(pi_task_t *task);

static pi_task_t *__pi_pmu_task_fifo_pop();

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

void __pi_pmu_handler(void *arg)
{
    //printf("PMU HANDLER\n");
    uint32_t event = (uint32_t) arg;
    pmu_req = 0;
    hal_pmu_irq_flag_clear(PI_PMU_MAESTRO_EVENT_SCU_OK);
    pi_task_t *task = g_pmu_data.fifo_head;
    if ((task != NULL) && (task->data[0] == event))
    {
        __pi_pmu_task_fifo_pop();
        __pi_irq_handle_end_of_task(task);
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
    struct pi_task *task_to_return = NULL;
    if (g_pmu_data.fifo_head != NULL)
    {
        task_to_return = g_pmu_data.fifo_head;
        g_pmu_data.fifo_head = g_pmu_data.fifo_head->next;
    }
    return task_to_return;
}

void __pi_pmu_wait_end_of_sequence(uint8_t domain_id)
{
    if (domain_id == PI_PMU_CLUSTER_ID)
    {
        hal_itc_wait_for_event_no_irq(FC_IRQ_DLC_BRIDGE_SCU_OK_IRQ);
        /* Clear IRQ flags. */
        hal_pmu_irq_flag_clear(PI_PMU_MAESTRO_EVENT_SCU_OK);
    }
    else
    {
        hal_itc_wait_for_event_no_irq(FC_IRQ_DLC_BRIDGE_SCU_OK_IRQ);
        /* Clear IRQ flags. */
        hal_pmu_irq_flag_clear(PI_PMU_MAESTRO_EVENT_SCU_OK);
        //pmu_req = 1;
        //while(pmu_req)
        //{
        //    pi_time_wait_us(10);
        //}
    }
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_pmu_init(void)
{
    g_pmu_data.fifo_head = NULL;
    g_pmu_data.fifo_tail = NULL;
    g_pmu_data.cluster_state = 0;

    #if !defined(__PLATFORM_FPGA__)
    /* Enable PMU IRQ. */
    hal_itc_mask_set(FC_IRQ_DLC_BRIDGE_SCU_OK_IRQ);

    uint32_t mask = 0x7;      /* Cf. HAL for value def. */
    //uint32_t mask = (POWER_MANAGER_DLCPD_IMR_ICU_OK_M(1) |
      //               POWER_MANAGER_DLCPD_IMR_ICU_DLY_M(1) |
        //             POWER_MANAGER_DLCPD_IMR_ICU_DEN_M(1) |
          //           POWER_MANAGER_DLCPD_IMR_ICU_UPD_M(1) |
            //         POWER_MANAGER_DLCPD_IMR_SCU_FL_M(1));
    hal_pmu_irq_mask_set(mask);

    //hal_pmu_irq_flag_clear(PI_PMU_MAESTRO_EVENT_SCU_OK);
    #endif  /* __PLATFORM_FPGA__ */
    PMU_TRACE("PMU opened and init.\n");
}

int32_t __pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage)
{
    return 0;
}

int32_t __pi_pmu_state_get(pi_pmu_domain_e domain)
{
    int32_t status = -11;
    switch (domain)
    {
    case PI_PMU_DOMAIN_FC :
        status = PI_PMU_DOMAIN_STATE_ON; /* This domain has to be on. */
        break;

    case PI_PMU_DOMAIN_CL :
        status = g_pmu_data.cluster_state;
        break;

    default :
        PMU_TRACE_ERR("Unknown power domain on VEGA !\n");
        break;
    }
    return status;
}

int32_t __pi_pmu_boot_state_get(pi_pmu_domain_e domain)
{
    return 0;
}

int32_t __pi_pmu_sleep_mode_set(pi_pmu_domain_e domain,
                                struct pi_pmu_sleep_conf_s *conf)
{
    return 0;
}

int32_t __pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain)
{
    return 0;
}

void __pi_pmu_state_apply(uint8_t domain_id, uint8_t state, uint8_t flags)
{
    uint32_t sequence = 0, ret = 0;

    switch (domain_id)
    {
    case PI_PMU_CHIP_ID :
        ret = (flags & PI_PMU_FLAGS_NO_RET) != 0;
        if (flags & PI_PMU_FLAGS_PADS_ON)
        {
            // For soc, 14 is deep sleep with pads on, 15 retentive deep sleep with pads on
            sequence = 14 + ret;
        }
        else
        {
            // For soc, 4 is deep sleep, 5 retentive deep sleep, and 6 and 7 the same
            // with smart wakeup on.
            sequence = 4 + ret;
        }
        break;

    default :
        //case PI_PMU_CLUSTER_ID :
        sequence = domain_id * 2 + 6 + state;
        break;
    }
    hal_pmu_maestro_sequence_trigger(sequence);
}

#if defined(FEATURE_CLUSTER)
void __pi_pmu_cluster_power_on(void)
{
    if (g_pmu_data.cluster_state == PI_PMU_DOMAIN_STATE_OFF)
    {
        #if !defined(__PLATFORM_FPGA__)
        PMU_TRACE("Power on cluster\n");
        pi_task_t task = {0};

        __pi_pmu_state_apply(PI_PMU_CLUSTER_ID, PI_PMU_STATE_ON, 0);
        __pi_pmu_wait_end_of_sequence(PI_PMU_CLUSTER_ID);

        pi_fll_init(FLL_ID_CL, (uint32_t) ARCHI_FREQ_INIT);

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

        /* Isolate cluster. */
        hal_soc_ctrl_cl_isolate_set(1);

        __pi_pmu_state_apply(PI_PMU_CLUSTER_ID, PI_PMU_STATE_OFF, 0);
        __pi_pmu_wait_end_of_sequence(PI_PMU_CLUSTER_ID);

        g_pmu_data.cluster_state = PI_PMU_DOMAIN_STATE_OFF;
        #else
        /* Isolate cluster. */
        hal_soc_ctrl_cl_isolate_set(1);
        #endif  /* __PLATFORM_FPGA__ */
    }
}
#endif  /* FEATURE_CLUSTER */
