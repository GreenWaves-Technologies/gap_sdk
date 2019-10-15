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
#include "rtx_core_c.h"
#include "tinyprintf.h"

#ifdef FEATURE_CLUSTER
#include "gap_cluster_fc_delegate.h"
#endif

extern void tfp_format(void *putp, putcf putf, const char *fmt, va_list va);
extern void exit(int code);

debug_struct_t HAL_DEBUG_STRUCT_NAME = GAP_DEBUG_STRUCT_INIT;

static int _io_lock() {
    /* user code must know if the cluster is on or off, we'll take care of that later on */
    if (__is_U_Mode()) return 0;
    int irq = 0;

    #ifdef FEATURE_CLUSTER
    if(!__is_FC()) {
        int needLock = 1;
        if (!cluster_is_on)
            needLock = 0;

        if (needLock) {
            irq = __disable_irq();
            EU_MutexLock(0);
        }
    }
    #endif

    return irq;
}

static void _io_unlock(int irq) {
    if (__is_U_Mode()) return;

    #ifdef FEATURE_CLUSTER
    if(!__is_FC()) {
        int needLock = 1;
        if (!cluster_is_on)
            needLock = 0;

        if (needLock) {
            EU_MutexUnlock(0);
            __restore_irq(irq);
        }
    }
    #endif
}

int putchar(int c);

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

static inline void uart_flush_printf(void *buffer, uint32_t size)
{
    UART_TransferSendBlocking(UART,  (uint8_t *) buffer, size);
    HAL_DEBUG_STRUCT_NAME.putcharCurrent = 0;
}

void uart_putc(char c)
{
    if (!uart_is_init)
    {
        uart_config_t config;

        UART_GetDefaultConfig(&config);
        config.baudRate_Bps = 9600;
        config.enableTx = true;
        config.enableRx = true;

        UART_Init(UART, &config, SystemCoreClock);
    }

    /* Enqueue in debug struct buffer before flushing when buffer is full. */
    HAL_DEBUG_STRUCT_NAME.putcharBuffer[HAL_DEBUG_STRUCT_NAME.putcharCurrent] = c;
    HAL_DEBUG_STRUCT_NAME.putcharCurrent++;
    if ((HAL_DEBUG_STRUCT_NAME.putcharCurrent == PRINTF_BUF_SIZE) || (c == '\n'))
    {
        uart_flush_printf(HAL_DEBUG_STRUCT_NAME.putcharBuffer, HAL_DEBUG_STRUCT_NAME.putcharCurrent);
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

static void tfp_putc(void *data, char c)
{
    if (__is_U_Mode())
    {
        osPutChar(c);
    }
    else
    {
        #ifdef PRINTF_UART
        /* When boot form FLASH, always use internal printf, you can only see printf in UART */
        #ifdef FEATURE_CLUSTER
        if (!__is_FC())
        {
            fc_call_t task;
            task.id     = UDMA_EVENT_UART_TX;
            task.arg[0] = c;
            CLUSTER_CL2FC_SendTask(0, &task);
        }
        else
        #endif  /* FEATURE_CLUSTER */
        {
            uart_putc(c);
        }
        #else  /* PRINTF_UART */
        if (DEBUG_GetDebugStruct()->useInternalPrintf)
        {
            #ifdef PRINTF_RTL
            /* This is for core internal printf in Simulation */
            #ifdef FEATURE_CLUSTER
            if (!__is_FC())
            {
                CLUSTER_STDOUT->PUTC[__core_ID() << 1] = c;
            }
            else
            #endif  /* FEATURE_CLUSTER */
            {
                FC_STDOUT->PUTC[__core_ID() << 1] = c;
            }
            #endif  /* PRINTF_RTL */
        }
        else
        {
            /* Only use for JTAG */
            putc_debug_bridge(c);
        }
        #endif  /* PRINTF_UART */
    }
}

int GAP_EXPORT printf(const char *fmt, ...) {
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

void abort() {
    exit(-1);
}

int GAP_EXPORT puts(const char *s) {
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
