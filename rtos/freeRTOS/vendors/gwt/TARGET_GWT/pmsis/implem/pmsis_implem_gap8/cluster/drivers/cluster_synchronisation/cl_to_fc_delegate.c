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

#include "pmsis.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/implem/hal/hal.h"
#include "pmsis/implem/drivers/pmsis_it.h"

volatile uint32_t g_task;

extern struct cluster_driver_data *__per_cluster_data[];
void cluster_cl_event_handler(void);

HANDLER_WRAPPER_LIGHT(cluster_cl_event_handler);

/**
 * @ingroup groupCluster
 */

/**
 * @defgroup FC_Delegate
 * Delegation of task to FC by cluster API
 */

/**
 * @addtogroup FC_Delegate
 * @{
 */

/**@{*/

void cl_notify_fc_event_handler(void)
{
    volatile struct cluster_driver_data *data = __per_cluster_data[0];
    pmsis_event_push(data->event_kernel, (pi_task_t*)g_task);
    hal_compiler_barrier();
    g_task = 0xdeadbeefUL;
    hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT,0);
}


/** \brief send an opaque task structure for FC
 *
 * FC will execute task according to the opaque structure
 * (callback, driver access...)
 * \param        opaque argument for the fc
 * Side effect: Uses cluster mutex
 */
void pi_cl_send_task_to_fc(pi_task_t *task)
{
    hal_eu_mutex_lock(0);
    while(g_task != 0xdeadbeefUL)
    {
        hal_compiler_barrier();
        hal_eu_evt_mask_wait_and_clr(1<<FC_NOTIFY_CLUSTER_EVENT);
        hal_compiler_barrier();
    }
    g_task = (uint32_t)task;
    hal_eu_fc_evt_trig_set(CLUSTER_TO_FC_NOTIFY_IRQN, 0);
    hal_eu_mutex_unlock(0);
}

void mc_fc_delegate_init(void *arg)
{
    g_task = 0xdeadbeefUL;
    /* Activate interrupt handler for FC when cluster want to push a task to FC */
    NVIC_EnableIRQ(CLUSTER_TO_FC_NOTIFY_IRQN);
    return;
}

void cl_wait_task(uint8_t *done)
{
    while ((*(volatile uint8_t *) done) == 0)
    {
        hal_eu_evt_mask_wait_and_clr(1 << FC_NOTIFY_CLUSTER_EVENT);
    }
}

void cl_notify_task_done(uint8_t *done, uint8_t cluster_id)
{
    (*(volatile uint8_t *) done) = 1;
    hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT, 0);
}
