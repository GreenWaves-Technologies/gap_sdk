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

#include "gap8_it.h"
#include "pmsis/implem/hal/hal.h"
#include "printf.h"

/* Ecall Table. */
const void *_ecall_vector[NB_ECALL] = {
    &_isr_default,
    &_procid,
    &_clusterid,
};


/*************
 * Handlers
 *************/

void HardFault_Handler( void )
{
}

void vSetPendSV()
{
    NVIC->MASK_IRQ_OR = (0x1 << PENDSV_IRQN);
    hal_eu_fc_evt_trig_set(PENDSV_IRQN, 0);
}

/*
 * __pending_task_switch is needed for some corner cases.
 * A task is delayed, when coming back, tick count may not match quantum,
 * then task is descheduled. Next times, xTaskGetTickCountFromISR() won't tell
 * if there are any tasks waiting on time delays.
 */
static volatile uint8_t __pending_task_switch = 0;
uint32_t uTaskCheckQuantum(uint32_t schedule)
{
    uint32_t ctx_switch = 0;
    uint32_t quantum = (uint32_t) configPREEMPTION_QUANTUM;
    uint32_t tick_count = xTaskGetTickCountFromISR();
    __pending_task_switch |= schedule;
    if ((tick_count % quantum) == 0)
    {
        ctx_switch = __pending_task_switch;
        __pending_task_switch = 0;
    }
    return ctx_switch;
}

/****************
 * Syscalls.
 ****************/

static inline uint32_t syscall( uint32_t callNum,
				uint32_t arg0,
				uint32_t arg1,
				uint32_t arg2,
				uint32_t arg3
    )
{
    uint32_t ret = 0;
    __asm__ volatile(
	"or a0, zero, %1 \n\t"
	"or a1, zero, %2 \n\t"
	"or a2, zero, %3 \n\t"
	"or a3, zero, %4 \n\t"
	"or a7, zero, %5 \n\t"
	"ecall \n\t"
	:"=r" ( ret )
	:"r" ( arg0 ), "r" ( arg1 ), "r" ( arg2 ), "r" ( arg3 ), "r" ( callNum )
	);

    return ret;
}

uint32_t procid( void )
{
    return syscall( ECALL_PROCID, 0, 0, 0, 0 );
}

uint32_t clusterid( void )
{
    return syscall( ECALL_CLUSTERID, 0, 0, 0, 0 );
}



/*****************
 * Inner syscalls.
 *****************/

/* Return core id. */
uint32_t _procid( void )
{
    uint32_t ulProcid = 0;
    __asm__ volatile( "csrr %0, mhartid" : "=r"( ulProcid ) );
    ulProcid  = ulProcid & 0xf;
    return ( ulProcid );
}

/* Return cluster id. */
uint32_t _clusterid( void )
{
    uint32_t ulCluster = 0;
    __asm__ volatile( "csrr %0, mhartid" : "=r"( ulCluster ) );
    ulCluster  = ulCluster >> 5;
    return ( ulCluster );
}



/****************
 * ISR.
 ****************/

void _isr_default( void )
{
    printf("Default\n");
    __asm__ volatile ( "nop" );
    return;
}
