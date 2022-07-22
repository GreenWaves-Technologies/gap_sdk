/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PMSIS__PLATFORMS__GVSOC__H__
#define __PMSIS__PLATFORMS__GVSOC__H__

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond IMPLEM

#define SEMIHOSTING_GV_TRACE_SETUP       0x100
#define SEMIHOSTING_GV_PCER_CONF         0x101
#define SEMIHOSTING_GV_PCER_RESET        0x102
#define SEMIHOSTING_GV_PCER_START        0x103
#define SEMIHOSTING_GV_PCER_STOP         0x104
#define SEMIHOSTING_GV_PCER_READ         0x105
#define SEMIHOSTING_GV_PCER_DUMP         0x106
#define SEMIHOSTING_GV_VCD_CONFIGURE     0x107
#define SEMIHOSTING_GV_OPEN_TRACE        0x108
#define SEMIHOSTING_GV_CONF_TRACE        0x109
#define SEMIHOSTING_GV_DUMP_TRACE        0x10A
#define SEMIHOSTING_GV_DUMP_TRACE_STRING 0x10B
#define SEMIHOSTING_GV_RELEASE_TRACE     0x10C
#define SEMIHOSTING_GV_STOP              0x10D



/* riscv semihosting standard: 
 * IN: a0 holds syscall number
 * IN: a1 holds pointer to arg struct
 * OUT: a0 holds return value (if exists)
 */
static inline long
gvsoc_semihost(long n, long _a1)
{
#ifdef __PLATFORM_GVSOC__
  register long a0 asm("a0") = n;
  register long a1 asm("a1") = _a1;

  // riscv magic values for semihosting
  asm volatile (
          ".option norvc;\t\n"
		  "slli    zero,zero,0x1f\t\n"
		  "ebreak\t\n"
		  "srai    zero,zero,0x7\t\n"
          ".option rvc;\t\n"
		: "+r"(a0) 
		: "r"(a1)
		);

#if 0
  if (a0 < 0)
    return __syscall_error (a0);
  else
    return a0;
#else
  return a0;
#endif
#else
    return 0;
#endif
}

/// @endcond




/**        
 * @ingroup groupPlatform
 */

/**
 * @defgroup GVSOC
 *
 *
 */

/**
 * @addtogroup GVSOC
 * @{
 */

/**        
 * @defgroup ENGINE Engine control
 *
 */

/**
 * @}
 */

/**
 * @addtogroup ENGINE
 * @{
 */

/**@{*/

/** \brief Stop execution.
 *
 * This function can be called to stop the GVSOC engine. In case a proxy is
 * connected, it will be notified that the engine has been stopped.
 */
static inline void gv_stop()
{
    gvsoc_semihost(SEMIHOSTING_GV_STOP, 0);

}

//!@}

/**
 * @addtogroup GVSOC
 * @{
 */

/**        
 * @defgroup TRACES Trace control
 *
 */

/**
 * @}
 */

/**        
 * @addtogroup TRACES
 * @{        
 */

/**@{*/

/** \brief Enable GVSOC traces.
 *
 * This function can be called to dynamically enable GVSOC traces from the
 * simulated SW. A regular expression can be passed to specify what should be
 * enabled, as with option --trace on the command-line.
 *
 * \param path A regular expression specifying the path of the traces to enable.
 */
static inline void gv_trace_enable(char *path)
{
    uint32_t len = strlen(path);
    volatile uint32_t args[2] = {(uint32_t)path, 1};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_TRACE_SETUP, (long)args);

}


/** \brief Disable GVSOC traces.
 *
 * This function can be called to dynamically disable GVSOC traces from the
 * simulated SW. A regular expression can be passed to specify what should be
 * disabled, as with option --trace on the command-line.
 *
 * \param path A regular expression specifying the path of the traces to disable.
 */
static inline void gv_trace_disable(char *path)
{
    uint32_t len = strlen(path);
    volatile uint32_t args[2] = {(uint32_t)path, 0};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_TRACE_SETUP, (long)args);

}

//!@}

/**        
 * @addtogroup PERF
 * @{        
 */

/**@{*/

/** \brief Configure performance counters.
 *
 * This function can be called to dynamically enable or disable performance counters.
 * Note that this is done through semi-hosting, and can also be done through a CSR instruction.
 * 
 *
 * \param events A bitfields specifiying which counter should be enabled. There is one bit
 *               per counter, bit 0 is for counter 0.
 */
static inline void gv_pcer_conf(unsigned int events)
{
    volatile uint32_t args[1] = {events};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_PCER_CONF, (long)args);
}


/** \brief Reset all performance counters.
 *
 * This function can be called to dynamically set all performance counters to 0.
 * Note that this is done through semi-hosting, and can also be done through a CSR instruction.
 * 
 */
static inline void gv_pcer_reset()
{
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_PCER_RESET, 0);
}


/** \brief Enable performance counting.
 *
 * This function can be called to dynamically enabled performance counting, which means
 * all enabled performance counters, will register the events they are monitoring.
 * Note that this is done through semi-hosting, and can also be done through a CSR instruction.
 * 
 */
static inline void gv_pcer_start()
{
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_PCER_START, 0);
}


/** \brief Enable performance counting.
 *
 * This function can be called to dynamically enabled performance counting, which means
 * all enabled performance counters, will register the events they are monitoring.
 * Note that this is done through semi-hosting, and can also be done through a CSR instruction.
 * 
 */
static inline void gv_pcer_stop()
{
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_PCER_STOP, 0);
}


/** \brief Read a perfomance counter.
 *
 * This function can be called to get the current value of a performance counter.
 * Note that this is done through semi-hosting, and can also be done through a CSR instruction.
 * 
 * \param pcer The performance counter index to be read.
 * \return The performance counter value.
 */
static inline unsigned int gv_pcer_read(int pcer)
{
    volatile uint32_t args[1] = {pcer};
    __asm__ __volatile__ ("" : : : "memory");
    return gvsoc_semihost(SEMIHOSTING_GV_PCER_READ, (long)args);
}


/** \brief Dump performance counters to a file.
 *
 * This function can be called to the current value of all performance counters to a file.
 * 
 * \param path The path of the file where the performance counters should be dumped.
 * \param mode The string specifying the mode used to open the file (directly passed to fopen)
 * \return 0 if the operation was successful, otherwise the error code.
 */
static inline int gv_pcer_dump(const char *path, const char *mode)
{
    volatile uint32_t args[2] = {(uint32_t)path, (uint32_t)mode};
    __asm__ __volatile__ ("" : : : "memory");
    return gvsoc_semihost(SEMIHOSTING_GV_PCER_DUMP, (long)args);
}


/** \brief Start performance counting .
 *
 * This function can be called to do all the required steps to start the performance counters
 * (configure all events, reset and start).
 */
static inline void gv_pcer_dump_start()
{
    gv_pcer_conf(0xffffffff);
    gv_pcer_reset();
    gv_pcer_start();
}


/** \brief Stop performance counting and dump to file.
 *
 * This function can be called to do all the required steps to stop the performance counters
 * and dump their values to a file (pcer.log).
 */
static inline void gv_pcer_dump_end()
{
    gv_pcer_stop();
    gv_pcer_dump("pcer.log", "w");
}


//!@}

/**
 * @}
 */


/**        
 * @addtogroup VCD
 * @{        
 */

/**@{*/

typedef struct {
} gv_vcd_conf_t;


/** \brief Configure VCD traces.
 *
 * This function can be called to modify global VCD configuration.
 * 
 * \param pcer 1 if VCD tracing must be enabled, 0 if it must be disabled.
 * \param conf VCD configuration, should be NULL for now.
 */
static inline void gv_vcd_configure(int active, gv_vcd_conf_t *conf) {
    volatile uint32_t args[1] = {(uint32_t)active};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_VCD_CONFIGURE, (long)args);
}


/** \brief Open a VCD trace.
 *
 * This function can be called to open a VCD trace from its full path and get a
 * descriptor which can be used for other API functions.
 * The trace must have been created either by a model or by user traces.
 * 
 * \param path The full path of the trace.
 * \return The trace descriptor which can be used to change the value of the trace.
 */
static inline int gv_vcd_open_trace(char *path) {
    volatile uint32_t args[1] = {(uint32_t)path};
    __asm__ __volatile__ ("" : : : "memory");
    return gvsoc_semihost(SEMIHOSTING_GV_OPEN_TRACE, (long)args);
}

// static inline void gv_vcd_conf_trace(int trace, int active) {
//     volatile uint32_t args[2] = {(uint32_t)trace, (uint32_t)active};
//     __asm__ __volatile__ ("" : : : "memory");
//     gvsoc_semihost(SEMIHOSTING_GV_CONF_TRACE, (long)args);
// }


/** \brief Change VCD trace value.
 *
 * This function can be called to give the new value of the specified trace.
 * 
 * \param trace The trace descriptor returned when the trace was opened.
 * \param value The new trace value.
 */
static inline void gv_vcd_dump_trace(int trace, unsigned int value) {
    volatile uint32_t args[2] = {(uint32_t)trace, (uint32_t)value};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_DUMP_TRACE, (long)args);
}


/** \brief Release a VCD trace.
 *
 * This function can be called to give the special z value (high impedance).
 * 
 * \param trace The trace descriptor returned when the trace was opened.
 */
static inline void gv_vcd_release_trace(int trace) {
    volatile uint32_t args[1] = {(uint32_t)trace};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_RELEASE_TRACE, (long)args);
}


/** \brief Change VCD trace value for a string trace.
 *
 * This function can be called to give the new string value of the specified trace.
 * 
 * \param trace The trace descriptor returned when the trace was opened.
 * \param str The new trace string.
 */
static inline void gv_vcd_dump_trace_string(int trace, char *str) {
    volatile uint32_t args[2] = {(uint32_t)trace, (uint32_t)str};
    __asm__ __volatile__ ("" : : : "memory");
    gvsoc_semihost(SEMIHOSTING_GV_DUMP_TRACE_STRING, (long)args);
}

//!@}

/**
 * @}
 */


#ifdef __cplusplus
}
#endif
#endif
