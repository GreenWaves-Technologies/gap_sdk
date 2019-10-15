/*
** ###################################################################
**     Processors:          GAP8
**
**     Compilers:           GNU C Compiler
**
**     Reference manual:    riscv-spec-v2.1, January 2017
**     Version:             rev. 2.9, 2017-07-19
**
**     Abstract:
**         CMSIS Peripheral Access Layer for GAP8
**
**     Copyright (c) 2015 - 2018 GreenWave Technologies, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of GreenWaves Technologies, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 http://greenwaves-technologies.com
**     mail:                 jie.chen@greenwaves-technologies.com
**
**     Revisions:
**     - rev. 1.0 (2017-10-11)
**         Initial version.
** ###################################################################
*/

/*!
 * @file system_GAP8.c
 * @version 1.0
 * @date 2017-10-10
 * @brief Device specific configuration file for GAP8 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include <stdlib.h>
#include "system_GAP8.h"
#include "gap_device_registers.h"
#include "../driver/gap_common.h"
#include "../driver/gap_debug.h"

#ifdef _OPENMP
extern void CLUSTER_Start(int cid, int nbCores, int control_icache_seperation);
extern uint32_t cluster_ret_value;
#endif

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */
/* handler wrapper  */
Handler_Wrapper(FC_EventHandler);

void SystemInit (void) {

    /* Deactivate all soc events as they are active by default */
    SOCEU->FC_MASK_MSB = 0xFFFFFFFF;
    SOCEU->FC_MASK_LSB = 0xFFFFFFFF;

    /* PMU Init */
    PMU_Init();

    /* FC Icache Enable*/
    SCBC->ICACHE_ENABLE = 0xFFFFFFFF;

    /* Here we bind same Handler in M and U mode vector table, TODO, security problem */
    /* If we need to protect the access to peripheral IRQ, we need do as SysTick_Handler */
    /* by using ecall form U mode to M mode */
    NVIC_SetVector(FC_SOC_EVENT_IRQn, (uint32_t)__handler_wrapper_FC_EventHandler);

    /* Activate interrupt handler for soc event */
    NVIC_EnableIRQ(FC_SOC_EVENT_IRQn);

    /* Initialize malloc functions */
    FC_MallocInit();

    __enable_irq();

    #ifdef _OPENMP
    CLUSTER_Start(0, CLUSTER_CORES_NUM, 0);
    /* Cluster Stack Init wait */
    EU_EVT_MaskWaitAndClr(1 << CLUSTER_NOTIFY_FC_EVENT);
    /* Cluster End wait */
    EU_EVT_MaskWaitAndClr(1 << CLUSTER_NOTIFY_FC_EVENT);
    exit(cluster_ret_value);
    #endif
}

void SystemCoreClockUpdate () {
    SystemCoreClock = FLL_GetFrequency(uFLL_SOC);

    /* Need to update clock divider for each peripherals */
    uart_is_init     = 0;
}

void Boot_Deinit()
{
}

void Platform_Exit(int code)
{
    /* Flush the pending messages to the debug tools
       Notify debug tools about the termination */
    BRIDGE_PrintfFlush();
    DEBUG_Exit(DEBUG_GetDebugStruct(), code);
    BRIDGE_SendNotif();

    /* Write return value to APB device */
    SOC_CTRL->CORE_STATUS = SOC_CTRL_CORE_STATUS_EOC(1) | code;

    /* In case the platform does not support exit or this core is not allowed to exit the platform ... */
    EU_EVT_MaskClr(0xffffffff);
    EU_EVT_Wait();
}
