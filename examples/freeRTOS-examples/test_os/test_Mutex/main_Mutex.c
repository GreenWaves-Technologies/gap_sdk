/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestMutex0( void *parameters );
void vTestMutex1( void *parameters );
void vTestMutex2( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+2];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
SemaphoreHandle_t xMutexHandler = NULL;

#define STATIC ( 0 )
#if STATIC == 1
/* xMutexHandler's data structure. */
StaticSemaphore_t xMutexBuffer;
#endif

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Mutex Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestMutex0,
        "TestMutex0",
        configMINIMAL_STACK_SIZE,
        ( void * ) 2000,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL)
    {
        printf("TestMutex0 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask, xTask2;
    TaskHandle_t xHandleDynamic = NULL, xHandleDynamic2 = NULL;

    xTask = xTaskCreate(
        vTestMutex1,
        "TestMutex1",
        configMINIMAL_STACK_SIZE,
        ( void * ) 1000,
        tskIDLE_PRIORITY + 2,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("vTestMutex1 is NULL !\n");
        exit(0);
    }

    xTask2 = xTaskCreate(
        vTestMutex2,
        "MutexCreator",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic2
        );
    if( xTask2 != pdPASS )
    {
        printf("TestMutex2 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleStatic;
    tasks[1] = xHandleDynamic;
    tasks[2] = xHandleDynamic2;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestMutex0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 5;
    for(;;)
    {
        if( xMutexHandler == NULL )
            vTaskSuspend( NULL );

        if( xSemaphoreTake( xMutexHandler, (TickType_t) 50 ) )
        {
            printf("\n%s takes Mutex.\n", taskname);

            for( uint32_t i = 0; i < 10 ; i++ )
                printf("|-->%s : %2d, Priority : %d\n",
                       taskname, i, uxTaskPriorityGet( xTaskGetCurrentTaskHandle() ));

            printf("|<--%s releases Mutex.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Mutex.\n", taskname);
                vSemaphoreDelete( xMutexHandler );
                /* NULL so that task do not continue execution. */
                xMutexHandler = NULL;
                printf("Test success\n");
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xMutexHandler );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestMutex1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 5;
    for(;;)
    {
        if( xMutexHandler == NULL )
            vTaskSuspend( NULL );

        if( xSemaphoreTake( xMutexHandler, (TickType_t) 50 ) )
        {
            printf("\n%s takes Mutex.\n", taskname);

            for( uint32_t i = 0; i < 100 ; i += 10 )
                printf("|-->%s : %2d, Priority : %d\n",
                       taskname, i, uxTaskPriorityGet( xTaskGetCurrentTaskHandle() ));

            printf("|<--%s releases Mutex.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Mutex.\n", taskname);
                vSemaphoreDelete( xMutexHandler );
                /* NULL so that task do not continue execution. */
                xMutexHandler = NULL;
                printf("Test success\n");
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xMutexHandler );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestMutex2( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    #if STATIC == 0
    xMutexHandler = xSemaphoreCreateMutex();
    #else
    xMutexHandler = xSemaphoreCreateMutexStatic( &xMutexBuffer );
    #endif

    if( xMutexHandler == NULL )
        exit(0);
    printf("%s created a Mutex.\n", taskname);
    /* Wake tasks if suspended. */
    if( eTaskGetState( tasks[0] ) == eSuspended )
        vTaskResume( tasks[0] );
    if( eTaskGetState( tasks[1] ) == eSuspended )
        vTaskResume( tasks[1] );
    printf("%s is suiciding.\n", taskname);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
