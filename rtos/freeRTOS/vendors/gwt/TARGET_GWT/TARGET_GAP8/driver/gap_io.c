/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#include <stdarg.h>
#include "pmsis.h"
#include "gap_common.h"

debug_struct_t HAL_DEBUG_STRUCT_NAME = GAP_DEBUG_STRUCT_INIT;

/*
 * This should be used in case of printf via uart before scheduler has started.
 * Output will be on terminal instead of uart. After scheduler has started, output
 * will be via uart.
 */
uint8_t g_freertos_scheduler_started = 0;

#if defined(FEATURE_CLUSTER)
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"

/* Only a fix for gap8, remove for chips with l2 t&s or atomic instructions. */
extern spinlock_t cluster_printf_spinlock;
#endif  /* FEATURE_CLUSTER */

#if defined(PRINTF_UART)
static uint8_t g_temp_byte;
struct pi_device g_printf_uart_dev = {0};

void printf_uart_init(void)
{
    struct pi_uart_conf config = {0};
    /* Init & open uart. */
    pi_uart_conf_init(&config);
    config.baudrate_bps = 9600;
    config.enable_tx = true;
    config.enable_rx = true;
    config.src_clock_Hz = system_core_clock_get();
    pi_open_from_conf(&g_printf_uart_dev, &config);

    if (pi_uart_open(&g_printf_uart_dev))
    {
        pmsis_exit(-117);
    }
}

static void __uart_putc(char c)
{
    g_temp_byte = (uint8_t) c;
    #if defined(FEATURE_CLUSTER)
    if (!__native_is_fc())
    {
        pi_cl_uart_req_t req;
        pi_cl_uart_write_byte(&g_printf_uart_dev, &g_temp_byte, &req);
        pi_cl_uart_write_wait(&req);
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        pi_uart_write_byte(&g_printf_uart_dev, &g_temp_byte);
    }
}
#elif defined(PRINTF_SEMIHOST)
#include "semihost.h"

#define SEMIHOST_BUFFER_SIZE       ( 128 )
static uint8_t g_semihost_buffer[SEMIHOST_BUFFER_SIZE];
static uint32_t g_semihost_buffer_index = 0;

struct semihost_putc_req_s
{
    uint8_t c;                  /*!< Character to send. */
    uint8_t done;               /*!< Variable to check completion. */
    uint8_t cid;                /*!< Cluster ID. */
    pi_task_t cb;               /*!< Callback function. */
};

static void __semihost_putc_exec(char c)
{
    g_semihost_buffer[g_semihost_buffer_index++] = c;
    if ((g_semihost_buffer_index == ((uint32_t) SEMIHOST_BUFFER_SIZE - 1)) ||
        (c == '\n'))
    {
        g_semihost_buffer[g_semihost_buffer_index] = '\0';
        semihost_write0((const char *) g_semihost_buffer);
        g_semihost_buffer_index = 0;
    }
}

static void __semihost_putc_cluster_req(void *arg)
{
    struct semihost_putc_req_s *req = (struct semihost_putc_req_s *) arg;
    __semihost_putc_exec(req->c);
    cl_notify_task_done(&(req->done), req->cid);
}

static void __semihost_putc(char c)
{
    #if defined(FEATURE_CLUSTER)
    if (!__native_is_fc())
    {
        struct semihost_putc_req_s req = {0};
        req.c = c;
        req.cid = __native_cluster_id();
        req.done = 0;
        pi_task_callback(&(req.cb), (void *) __semihost_putc_cluster_req, &req);
        pi_cl_send_task_to_fc(&(req.cb));
        cl_wait_task(&(req.done));
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __semihost_putc_exec(c);
    }
}
#else
#if defined(PRINTF_RTL)
static void __stdout_putc(char c)
{
    #if defined(FEATURE_CLUSTER)
    if (!__native_is_fc())
    {
        CLUSTER_STDOUT->PUTC[__core_ID() << 1] = c;
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        FC_STDOUT->PUTC[__core_ID() << 1] = c;
    }
}
#endif  /* PRINTF_RTL */
#endif  /* PRINTF_UART */

static void __debug_bridge_putc(char c)
{
    /* Iter until we can push the character. */
    while (DEBUG_PutcharNoPoll(DEBUG_GetDebugStruct(), c))
    {
        BRIDGE_Delay();
    }

    /*
     * If the buffer has been flushed to the bridge, we now need to send him
     * a notification.
     */
    if (DEBUG_IsEmpty(DEBUG_GetDebugStruct()))
    {
        BRIDGE_PrintfFlush();
    }
}

static uint32_t __io_lock()
{
    uint32_t irq = 0;
    #if defined(FEATURE_CLUSTER)
    if (__native_is_fc())
    {
        if (pi_cluster_is_on())
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
    #endif  /* FEATURE_CLUSTER */

    return irq;
}

static void __io_unlock(uint32_t irq)
{
    #if defined(FEATURE_CLUSTER)
    if (__native_is_fc())
    {
        if (pi_cluster_is_on())
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
    #endif  /* FEATURE_CLUSTER */
}

static void tfp_putc(void *data, char c)
{
    #if !defined(__DISABLE_PRINTF__)
    #if defined(PRINTF_UART)
    if (g_freertos_scheduler_started)
    {
        __uart_putc(c);
    }
    else
    {
        __debug_bridge_putc(c);
    }
    #elif defined(PRINTF_SEMIHOST)
    __semihost_putc(c);
    #else
    #if defined(PRINTF_RTL)  /* PRINTF_SEMIHOST */
    if (DEBUG_GetDebugStruct()->useInternalPrintf)
    {
        /* This is for core internal printf in Simulation */
        __stdout_putc(c);
    }
    else
    #endif  /* PRINTF_RTL */
    {
        /* Only use for JTAG */
        __debug_bridge_putc(c);
    }
    #endif /* PRINTF_UART */
    #endif /* __DISABLE_PRINTF__ */
}

void _putchar(char character)
{
    tfp_putc(NULL, character);
}

int puts(const char *s)
{
    char c;
    uint32_t irq = __io_lock();
    do
    {
        c = *s;
        if (c == 0)
        {
            tfp_putc(NULL, '\n');
            break;
        }
        tfp_putc(NULL, c);
        s++;
    } while (1);
    __io_unlock(irq);
    return 0;
}
