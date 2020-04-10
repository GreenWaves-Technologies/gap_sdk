/******************************************************************************************
 *
 * Copyright (c) 2018 , GreenWaves Technologies, Inc.
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
 *******************************************************************************************/


/****************************************************************************/
/* FreeRTOS Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

/* GAP8 handler includes. */
#include "gap8_it.h"
#include "stdio.h"
#include "stdlib.h"
#include "printf.h"

/****************************************************************************/

/* Utilities to control tasks. */
#define NBCORES ( 8 )
#define NBTASKS ( 1 )

extern TaskHandle_t tasks[];
extern uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#ifdef NOSTDMALLOC
extern HeapRegion_t xHeapRegions[];
#endif

/****************************************************************************/

#if configUSE_IDLE_HOOK == 1
void vApplicationIdleHook( void );
#endif //configUSE_IDLE_HOOK

#if configSUPPORT_DYNAMIC_ALLOCATION == 1
#if configUSE_MALLOC_FAILED_HOOK == 1
void vApplicationMallocFailedHook( void );
#endif //configUSE_MALLOC_FAILED_HOOK
#endif //configSUPPORT_DYNAMIC_ALLOCATION

#if ( configCHECK_FOR_STACK_OVERFLOW == 1 ) || ( configCHECK_FOR_STACK_OVERFLOW == 2 )
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
#endif // configCHECK_FOR_STACK_OVERFLOW

#if configSUPPORT_STATIC_ALLOCATION == 1
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
				    StackType_t **ppxIdleTaskStackBuffer,
				    uint32_t *pulIdleTaskStackSize );
#endif //configSUPPORT_STATIC_ALLOCATION

#if configUSE_TIMERS == 1
#if configUSE_DAEMON_TASK_STARTUP_HOOK == 1
void vApplicationDaemonTaskStartupHook( void );
#endif //configUSE_DAEMON_TASK_STARTUP_HOOK

#if configSUPPORT_STATIC_ALLOCATION == 1
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
				     StackType_t **ppxTimerTaskStackBuffer,
				     uint32_t *pulTimerTaskStackSize );
#endif //configSUPPORT_STATIC_ALLOCATION

#endif //configUSE_TIMERS

#ifndef NDEBUG
void vPrvAssertFailed( const char *filename, uint32_t line, const char *expr );
#endif
/****************************************************************************/
