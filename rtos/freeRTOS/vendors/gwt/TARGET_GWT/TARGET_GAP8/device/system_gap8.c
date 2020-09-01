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

#include "system_gap8.h"

/* PMSIS includes. */
#include "pmsis.h"

#include "../driver/semihost.h"

/* FC & L2 heaps. */
extern char __heapfcram_start;
extern char __heapfcram_size;
extern char __heapl2ram_start;
extern char __heapl2ram_size;

volatile uint32_t SystemCoreClock = (uint32_t) DEFAULT_SYSTEM_CLOCK;

static volatile uint32_t tick_rate = 0;

void system_init(void)
{
    /* Disable all IRQs first. */
    NVIC->MASK_IRQ_AND = 0xFFFFFFFF;

    /* Deactivate all soc events. */
    SOCEU->FC_MASK_MSB = 0xFFFFFFFF;
    SOCEU->FC_MASK_LSB = 0xFFFFFFFF;

    /* FLush FC Icache and then enable it. */
    SCBC->ICACHE_ENABLE = 0xFFFFFFFF;

    /* Setup FC_SOC events handler. */
    pi_fc_event_handler_init(FC_SOC_EVENT_IRQN);

    /* PMU Init */
    __pi_pmu_init();

    /* Enable IRQ. */
    __enable_irq();

    /* Initialize malloc functions. */
    pi_malloc_init((void*) &__heapfcram_start, (uint32_t) &__heapfcram_size,
                   (void*) &__heapl2ram_start, (uint32_t) &__heapl2ram_size);

    #if defined(PRINTF_UART)
    printf_uart_init(0);
    #endif  /* PRINTF_UART */
}

void system_setup_systick(uint32_t tick_rate_hz)
{
    tick_rate = tick_rate_hz;
    timer_cfg_u cfg = {0};
    cfg.field.enable = 1;
    cfg.field.reset = 1;
    cfg.field.irq_en = 1;
    cfg.field.mode = 1;
    uint32_t cmp_val = ((SystemCoreClock / tick_rate) - 1);
    pi_timer_init(SYS_TIMER, cfg, cmp_val);
    NVIC_EnableIRQ(SYSTICK_IRQN);
}

void system_core_clock_update(void)
{
    //system_setup_systick(tick_rate);
    SystemCoreClock = pi_fll_get_frequency(FLL_SOC, 0);
    pi_timer_stop(SYS_TIMER);
    /* Systick timer configuration. */
    timer_cfg_u cfg = {0};
    cfg.field.enable = 1;
    cfg.field.reset = 1;
    cfg.field.irq_en = 1;
    cfg.field.mode = 1;
    uint32_t cmp_val = ((SystemCoreClock / tick_rate) - 1);
    pi_timer_init(SYS_TIMER, cfg, cmp_val);
}

uint32_t system_core_clock_get(void)
{
    return SystemCoreClock;
}

void system_exit(int32_t code)
{
    if (pi_is_fc())
    {
        /* Flush pending output. */
        system_exit_printf_flush();

        #if !defined(__PLATFORM_GVSOC__)
        /* Notify debug tools about the termination. */
        semihost_exit(code == 0 ? SEMIHOST_EXIT_SUCCESS : SEMIHOST_EXIT_ERROR);
        #endif  /* __PLATFORM_GVSOC__ */

        /* Write return value to APB device */
        soc_ctrl_corestatus_set(code);
    }
    /* In case the platform does not support exit or this core is not allowed to exit the platform ... */
    hal_eu_evt_mask_clr(0xffffffff);
    hal_eu_evt_wait();
}
