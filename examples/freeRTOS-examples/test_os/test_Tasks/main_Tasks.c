/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTaskTestStatic( void *parameters );
void vTaskTestDynamic( void *parameters );
void vTaskTestDynamic2( void *parameters );
void vTaskTestCleaner( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+3];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
uint8_t taskStatic = 0, taskDynamic = 0, taskDynamic2 = 0, resumeStatic = 0;

/*
 * configUSE_PREEMPTION may not be set, to use cooperative mode.
 * if configUSE_PREEMPTION is set, configUSE_TIME_SLICING must also be set.
 */
/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Tasks Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTaskTestStatic,
        "Thread0",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("Thread0 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask, xTask2, xTaskCleaner;
    TaskHandle_t xHandleDynamic = NULL, xHandleDynamic2 = NULL, xHandleCleaner = NULL;

    xTask = xTaskCreate(
        vTaskTestDynamic,
        "Thread1",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Thread1 is NULL !\n");
        exit(0);
    }

    xTask2 = xTaskCreate(
        vTaskTestDynamic2,
        "Thread2",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic2
        );
    if( xTask2 != pdPASS )
    {
        printf("Thread2 is NULL !\n");
        exit(0);
    }

    xTaskCleaner = xTaskCreate(
        vTaskTestCleaner,
        "ThreadCleaner",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleCleaner
        );
    if( xTaskCleaner != pdPASS )
    {
        printf("Thread Cleaner is NULL !\n");
        exit(0);
    }

    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleStatic;
    tasks[1] = xHandleDynamic;
    tasks[2] = xHandleDynamic2;
    tasks[3] = xHandleCleaner;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTaskTestStatic( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t pid = 0, cid = 0;
    int32_t loop = 0;
    for(;;)
    {
        loop -= 1;
        pid = procid();
        cid = clusterid();
        printf("%s : %d\t TICK = %d\t ProcID = %d\t ClusterID = %d\n",
               taskname, loop, xTaskGetTickCount(), pid, cid);
        taskStatic = 1;
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
}
/*-----------------------------------------------------------*/

void vTaskTestDynamic( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0;
    for(;;)
    {
        loop += 1;
        printf("%s : %d\t TICK = %d\n", taskname, loop, xTaskGetTickCount());
        if( loop >= 15 )
        {
            printf("|->%s resuming %s and suicide.\n",
                   taskname, pcTaskGetName( tasks[0] ));
            vTaskResume( tasks[0] );
            taskDynamic = 1;
            vTaskSuspend( NULL );
        }
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
}
/*-----------------------------------------------------------*/

void vTaskTestDynamic2( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0, delay = 10;
    for(;;)
    {
        loop += 5;
        printf("%s : %d\t TICK = %d\n", taskname, loop, xTaskGetTickCount());
        if( loop == 15 )
        {
            printf("%s delaying at Tick = %d for %d ms !\n\n",
                   taskname, xTaskGetTickCount(), delay);
            vTaskDelay( pdMS_TO_TICKS( delay ) );
            printf("%s is coming back at Tick = %d!\n",
                   taskname, xTaskGetTickCount());
        }
        if( loop >= 100 )
        {
            printf("|->%s suspending.\n", taskname);
            taskDynamic2 = 1;
            vTaskSuspend( NULL );
        }
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
}
/*-----------------------------------------------------------*/

void vTaskTestCleaner( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    for(;;)
    {
        printf("%s \t TICK = %d\n", taskname, xTaskGetTickCount());

        if( taskStatic )
        {
            printf("Suspending %s\n\n", pcTaskGetName( tasks[0] ));
            vTaskSuspend( tasks[0] );
            taskStatic = 0;
        }

        if( taskDynamic && taskDynamic2 )
        {
            printf("|->%s self deleting and deleting task %s\n",
                   taskname, pcTaskGetName( tasks[0] ));
            vTaskSuspend( tasks[0] );
            printf("Test success\n");
            vTaskDelete( NULL );
        }
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
}
/*-----------------------------------------------------------*/
