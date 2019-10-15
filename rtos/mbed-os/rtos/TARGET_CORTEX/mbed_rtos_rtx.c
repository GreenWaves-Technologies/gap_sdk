/* mbed Microcontroller Library
 * Copyright (c) 2018-2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
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

#include "cmsis.h"
#include "mbed_rtx.h"
#include "mbed_rtos_storage.h"
#include "cmsis_os2.h"
#include "mbed_toolchain.h"
#include "mbed_error.h"
#include "mbed_critical.h"
#include "mbed_boot.h"

#if defined(TARGET_MBED_SPM)
#include "spm_init.h"
#include "spm_api.h"
#endif
#if defined(TARGET_TFM) && defined(COMPONENT_NSPE)
#include "TARGET_TFM/interface/include/tfm_ns_lock.h"
#endif

#if defined(COMPONENT_NSPE) && defined(COMPONENT_SPM_MAILBOX)

MBED_ALIGN(8) char psa_spm_dispatcher_th_stack[0x100];
mbed_rtos_storage_thread_t psa_spm_dispatcher_th_tcb;
const osThreadAttr_t psa_spm_dispatcher_th_attr = {
    .name       = "SPM_DISP",
    .priority   = osPriorityNormal,
    .stack_mem  = psa_spm_dispatcher_th_stack,
    .stack_size = sizeof(psa_spm_dispatcher_th_stack),
    .cb_mem     = &psa_spm_dispatcher_th_tcb,
    .cb_size    = sizeof(psa_spm_dispatcher_th_tcb)
};

#endif // defined(COMPONENT_NSPE) && defined(COMPONENT_SPM_MAILBOX)

osThreadAttr_t _main_thread_attr;

#ifndef MBED_CONF_APP_MAIN_STACK_SIZE
#define MBED_CONF_APP_MAIN_STACK_SIZE MBED_CONF_RTOS_MAIN_THREAD_STACK_SIZE
#endif
MBED_ALIGN(8) char _main_stack[MBED_CONF_APP_MAIN_STACK_SIZE];
mbed_rtos_storage_thread_t _main_obj __attribute__((section(".bss.os.thread.cb")));

osMutexId_t               singleton_mutex_id;
mbed_rtos_storage_mutex_t singleton_mutex_obj;
osMutexAttr_t             singleton_mutex_attr;

void mbed_rtos_init()
{
    osKernelInitialize();
}

MBED_NORETURN void mbed_rtos_start()
{
    singleton_mutex_attr.name = "singleton_mutex";
    singleton_mutex_attr.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
    singleton_mutex_attr.cb_size = sizeof(singleton_mutex_obj);
    singleton_mutex_attr.cb_mem = &singleton_mutex_obj;

    _main_thread_attr.stack_mem = _main_stack;
    _main_thread_attr.stack_size = sizeof(_main_stack);
    _main_thread_attr.cb_size = sizeof(_main_obj);
    _main_thread_attr.cb_mem = &_main_obj;
    _main_thread_attr.priority = osPriorityNormal;
    _main_thread_attr.name = "main";

    /* Allow non-secure main thread to call secure functions */
#if defined(DOMAIN_NS) && (DOMAIN_NS == 1U)
    _main_thread_attr.tz_module = 1U;
#endif

#if defined(COMPONENT_SPM_MAILBOX)
    spm_ipc_mailbox_init();
#endif // defined(COMPONENT_SPM_MAILBOX)

#if defined(TARGET_MBED_SPM)

#if defined(COMPONENT_SPE)
    // At this point, the mailbox is already initialized
    psa_spm_init();
    spm_hal_start_nspe();
#endif // defined(COMPONENT_SPE)

#if defined(COMPONENT_NSPE) && defined(COMPONENT_SPM_MAILBOX)
    osThreadId_t spm_result = osThreadNew((osThreadFunc_t)psa_spm_mailbox_dispatcher, NULL, &psa_spm_dispatcher_th_attr);
    if ((void *)spm_result == NULL) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_INITIALIZATION_FAILED), "Dispatcher thread not created", &psa_spm_dispatcher_th_attr);
    }
#endif // defined(COMPONENT_NSPE) && defined(COMPONENT_SPM_MAILBOX)
#endif // defined(TARGET_MBED_SPM)

#if defined(TARGET_TFM) && defined(COMPONENT_NSPE)
    tfm_ns_lock_init();
#endif // defined(TARGET_TFM) && defined(COMPONENT_NSPE)

    singleton_mutex_id = osMutexNew(&singleton_mutex_attr);
    osThreadId_t result = osThreadNew((osThreadFunc_t)mbed_start, NULL, &_main_thread_attr);
    if ((void *)result == NULL) {
        MBED_ERROR1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_INITIALIZATION_FAILED), "Pre main thread not created", &_main_thread_attr);
    }

    osKernelStart();
    MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_INITIALIZATION_FAILED), "Failed to start RTOS");
    while (1); // Code should never get here
}
