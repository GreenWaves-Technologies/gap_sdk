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

#ifndef __PMSIS_IMPLEM_DEBUG_H__
#define __PMSIS_IMPLEM_DEBUG_H__

/* Define those flags in Makefile or command line to enable traces/logs. */

#if defined(TRACE_CPI)
#define CPI_TRACE(...)             PI_LOG_DBG(__func__, __VA_ARGS__)
#define CPI_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define CPI_TRACE(...)             ((void) 0)
#define CPI_TRACE_ERR(...)         ((void) 0)
#endif  /* TRACE_CPI */

#if defined(TRACE_DMACPY)
#define DMACPY_TRACE(...)          PI_LOG_DBG(__func__, __VA_ARGS__)
#define DMACPY_TRACE_ERR(...)      PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define DMACPY_TRACE(...)          ((void) 0)
#define DMACPY_TRACE_ERR(...)      ((void) 0)
#endif  /* TRACE_DMACPY */

#if defined(TRACE_GPIO)
#define GPIO_TRACE(...)            PI_LOG_DBG(__func__, __VA_ARGS__)
#define GPIO_TRACE_ERR(...)        PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define GPIO_TRACE(...)            ((void) 0)
#define GPIO_TRACE_ERR(...)        ((void) 0)
#endif  /* TRACE_GPIO */

#if defined(TRACE_HYPERBUS)
#define HYPER_TRACE(...)           PI_LOG_DBG(__func__, __VA_ARGS__)
#define HYPER_TRACE_ERR(...)       PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define HYPER_TRACE(...)           ((void) 0)
#define HYPER_TRACE_ERR(...)       ((void) 0)
#endif  /* TRACE_HYPERBUS */

#if defined(TRACE_I2C)
#define I2C_TRACE(...)             PI_LOG_DBG(__func__, __VA_ARGS__)
#define I2C_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define I2C_TRACE(...)             ((void) 0)
#define I2C_TRACE_ERR(...)         ((void) 0)
#endif  /* TRACE_I2C */

#if defined(TRACE_I2S)
#define I2S_TRACE(...)             PI_LOG_DBG(__func__, __VA_ARGS__)
#define I2S_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define I2S_TRACE(...)             ((void) 0)
#define I2S_TRACE_ERR(...)         ((void) 0)
#endif  /* TRACE_I2S */

#if defined(TRACE_PAD)
#define PAD_TRACE(...)             PI_LOG_DBG(__func__, __VA_ARGS__)
#define PAD_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define PAD_TRACE(...)             ((void) 0)
#define PAD_TRACE_ERR(...)         ((void) 0)
#endif  /* TRACE_PAD */

#if defined(TRACE_PMU)
#define PMU_TRACE(...)             PI_LOG_DBG(__func__, __VA_ARGS__)
#define PMU_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define PMU_TRACE(...)             ((void) 0)
#define PMU_TRACE_ERR(...)         ((void) 0)
#endif  /* TRACE_PMU */

#if defined(TRACE_PWM)
#define PWM_TRACE(...)             PI_LOG_DBG(__func__, __VA_ARGS__)
#define PWM_TRACE_ERR(...)         PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define PWM_TRACE(...)             ((void) 0)
#define PWM_TRACE_ERR(...)         ((void) 0)
#endif  /* TRACE_PWM */

#if defined(TRACE_UART)
#define UART_TRACE(...)            PI_LOG_DBG(__func__, __VA_ARGS__)
#define UART_TRACE_ERR(...)        PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define UART_TRACE(...)            ((void) 0)
#define UART_TRACE_ERR(...)        ((void) 0)
#endif  /* TRACE_UART */



#endif  /* __PMSIS_IMPLEM_DEBUG_H__ */
