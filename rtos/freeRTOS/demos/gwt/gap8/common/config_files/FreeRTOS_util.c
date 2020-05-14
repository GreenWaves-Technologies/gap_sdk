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

/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"
#include "pmsis/implem/hal/hal.h"
#include "pmsis.h"

/****************************************************************************/

/* Variables used. */
#ifdef NOSTDMALLOC
/*
 * Using heap_5 for memory management : need to define memory regions to
 * allocate memory.
 */
extern uint8_t __heapl2ram_start;
extern uint8_t __heapl2ram_size;

HeapRegion_t xHeapRegions[] =
{
    { ( uint8_t * ) &__heapl2ram_start, ( uint32_t ) &__heapl2ram_size },
    { NULL, 0 }
};
#endif

/****************************************************************************/

#if configUSE_IDLE_HOOK == 1
void vApplicationIdleHook( void )
{
    asm volatile("wfi");
}
#endif //configUSE_IDLE_HOOK
/*-----------------------------------------------------------*/

#if configSUPPORT_DYNAMIC_ALLOCATION == 1
#if configUSE_MALLOC_FAILED_HOOK == 1
void vApplicationMallocFailedHook( void )
{
    /*
      vApplicationMallocFailedHook() will only be called if
      configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
      function that will get called if a call to pvPortMalloc() fails.
      pvPortMalloc() is called internally by the kernel whenever a task, queue,
      timer or semaphore is created.  It is also called by various parts of the
      demo application.  If heap_1.c or heap_2.c are used, then the size of the
      heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
      FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
      to query the size of free heap space that remains (although it does not
      provide information on how the remaining heap might be fragmented).
    */
    printf("Memory Allocation failed! Exiting !\n");
    exit(-4322);
    /*
    taskDISABLE_INTERRUPTS();
    for( ;; );
    */
}
#endif //configUSE_MALLOC_FAILED_HOOK
#endif //configSUPPORT_DYNAMIC_ALLOCATION
/*-----------------------------------------------------------*/

#if ( configCHECK_FOR_STACK_OVERFLOW == 1 ) || ( configCHECK_FOR_STACK_OVERFLOW == 2 )
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /*
      Run time stack overflow checking is performed if
      configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
      function is called if a stack overflow is detected.
    */
    printf("Stack Overflow : %s ! Exiting !\n", pcTaskName);
    exit(-4321);
    /*
    taskDISABLE_INTERRUPTS();
    for( ;; );
    */
}
#endif //configCHECK_FOR_STACK_OVERFLOW
/*-----------------------------------------------------------*/

#if configSUPPORT_STATIC_ALLOCATION == 1
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
				    StackType_t **ppxIdleTaskStackBuffer,
				    uint32_t *pulIdleTaskStackSize )
{
    /*
      If the buffers to be provided to the Idle task are declared inside this
      function then they must be declared static - otherwise they will be allocated on
      the stack and so not exists after this function exits.
    */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /*
      Pass out a pointer to the StaticTask_t structure in which the Idle task's
      state will be stored.
    */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /*
      Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
      Note that, as the array is necessarily of type StackType_t,
      configMINIMAL_STACK_SIZE is specified in words, not bytes.
    */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif //configSUPPORT_STATIC_ALLOCATION
/*-----------------------------------------------------------*/


#if configUSE_TIMERS == 1

#if configUSE_DAEMON_TASK_STARTUP_HOOK == 1
void vApplicationDaemonTaskStartupHook( void )
{
    /*
      This function is executed once when schedular starts.
     */
}
#endif //configUSE_DAEMON_TASK_STARTUP_HOOK
/*-----------------------------------------------------------*/

#if configSUPPORT_STATIC_ALLOCATION == 1
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
				     StackType_t **ppxTimerTaskStackBuffer,
				     uint32_t *pulTimerTaskStackSize )
{
    /*
      If the buffers to be provided to the Timer task are declared inside this
      function then they must be declared static - otherwise they will be allocated on
      the stack and so not exists after this function exits.
    */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /*
      Pass out a pointer to the StaticTask_t structure in which the Timer
      task's state will be stored.
    */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /*
      Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
      Note that, as the array is necessarily of type StackType_t,
      configTIMER_TASK_STACK_DEPTH is specified in words, not bytes.
    */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif //configSUPPORT_STATIC_ALLOCATION

#endif //configUSE_TIMERS

#ifndef NDEBUG
void vPrvAssertFailed( const char *filename, uint32_t line, const char *expr )
{
    printf("Assertion failed in %s at line %d : %s\n", filename, line, expr);
}
#endif
/****************************************************************************/
