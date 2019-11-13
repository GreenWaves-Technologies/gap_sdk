/*
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou (germain.haugou@gmail.com)
 */
#include <stdarg.h>
#include "gap_bridge.h"
#include "gap_common.h"
#include "tinyprintf.h"

#include "pmsis/rtos/malloc/pmsis_l2_malloc.h"
#include "pmsis/drivers/uart.h"

#ifdef FEATURE_CLUSTER
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"

// Only a fix for gap8, remove for chips with l2 t&s or atomic instructions
extern spinlock_t cluster_printf_spinlock;
#endif

extern uint32_t SystemCoreClock;

struct pi_device *printf_uart_dev;

debug_struct_t HAL_DEBUG_STRUCT_NAME = GAP_DEBUG_STRUCT_INIT;

void printf_uart_init(void)
{
    printf_uart_dev = (struct pi_device *) pmsis_l2_malloc(sizeof(struct pi_device));
    struct pi_uart_conf *config = (struct pi_uart_conf *) pmsis_l2_malloc(sizeof(struct pi_uart_conf));

    pi_uart_conf_init(config);
    config->baudrate_bps = 9600;
    config->enable_tx = true;
    config->enable_rx = true;
    config->src_clock_Hz = SystemCoreClock;

    printf_uart_dev->config = config;

    pi_uart_open(printf_uart_dev);
}

static int _io_lock()
{
    /* user code must know if the cluster is on or off, we'll take care of that later on */
    if (__is_U_Mode()) return 0;
    int irq = 0;

    #ifdef FEATURE_CLUSTER
    if(__is_FC())
    {
        if(pi_cluster_is_on())
        {
            cl_sync_spinlock_take(&cluster_printf_spinlock);
        }
        else
        {
            irq = __disable_irq();
        }
    }
    else
    {
        cl_sync_spinlock_take(&cluster_printf_spinlock);
    }
    #else
    irq = __disable_irq();
    #endif

    return irq;
}

static void _io_unlock(int irq)
{
    if (__is_U_Mode()) return;
    #ifdef FEATURE_CLUSTER
    if(__is_FC())
    {
        if(pi_cluster_is_on())
        {
            cl_sync_spinlock_release(&cluster_printf_spinlock);
        }
        else
        {
            __restore_irq(irq);
        }
    }
    else
    {
        cl_sync_spinlock_release(&cluster_printf_spinlock);
    }
    #else
    __restore_irq(irq);
    #endif
}

int putchar(int c);
/*
__attribute__((always_inline))
static inline char __svcPutChar (char c) {
  SVC_ArgR(0,c);
  SVC_ArgF(putchar);
  SVC_Call0(SVC_In1, SVC_Out1, SVC_CL1);
  return (char) __a0;
}

char osPutChar(char c) {
  if (IsIrqMode() || IsIrqMasked()) {
     return -1 ;
  }
  return __svcPutChar(c);
}
*/


uint8_t temp_byte;
void uart_putc(char c)
{
    if(printf_uart_dev)
    {
        temp_byte = (uint8_t)c;
        pi_uart_write_byte(printf_uart_dev, &temp_byte);
    }
}

static void putc_debug_bridge(char c)
{
    // Iter until we can push the character.
    while (DEBUG_PutcharNoPoll(DEBUG_GetDebugStruct(), c))
    {
        BRIDGE_Delay();
    }

    // If the buffer has been flushed to the bridge, we now need to send him
    // a notification
    if (DEBUG_IsEmpty(DEBUG_GetDebugStruct()))
    {
        BRIDGE_PrintfFlush();
    }
}

static void tfp_putc(void *data, char c) {
    if (__is_U_Mode()) {
        //osPutChar(c);
    } else {
        #ifdef PRINTF_UART
        #ifdef FEATURE_CLUSTER
        if(!__native_is_fc())
        {
            putc_debug_bridge(c);
        }
        else
        #endif
        {
            uart_putc(c);
        }
        #endif

        /* When boot form FLASH, always use internal printf, you can only see printf in UART */
        if (DEBUG_GetDebugStruct()->useInternalPrintf) {
            #ifndef PRINTF_UART
            #ifdef PRINTF_RTL
            /* This is for core internal printf in Simulation */
            if(__cluster_ID() == FC_CLUSTER_ID) {
                FC_STDOUT->PUTC[__core_ID() << 1] = c;
            }
            #ifdef FEATURE_CLUSTER
            else {
                CLUSTER_STDOUT->PUTC[__core_ID() << 1] = c;
            }
            #endif
            #endif
            #endif
        } else {
            #ifndef USE_UART
            /* Only use for JTAG */
            putc_debug_bridge(c);
            #endif
        }
    }
}

int printf(const char *fmt, ...)
{
    #ifndef __DISABLE_PRINTF__
    va_list va;
    va_start(va, fmt);
    /* Only lock the printf if the cluster is up to avoid mixing FC and cluster output */
    int irq = _io_lock();
    tfp_format(NULL, tfp_putc, fmt, va);
    _io_unlock(irq);
    va_end(va);
    #endif

    return 0;
}

int puts(const char *s)
{
    char c;
    int irq = _io_lock();
    do {
        c = *s;
        if (c == 0) {
            tfp_putc(NULL, '\n');
            break;
        }
        tfp_putc(NULL, c);
        s++;
    } while(1);
    _io_unlock(irq);
    return 0;
}

int putchar(int c) {
    tfp_putc(NULL, c);
    return c;
}
