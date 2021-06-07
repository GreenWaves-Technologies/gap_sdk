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

/// @cond IMPLEM

#define SEMIHOSTING_GV_TRACE_SETUP   0x100


/* riscv semihosting standard: 
 * IN: a0 holds syscall number
 * IN: a1 holds pointer to arg struct
 * OUT: a0 holds return value (if exists)
 */
static inline long
gvsoc_semihost(long n, long _a1)
{
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
 * @}
 */


#endif