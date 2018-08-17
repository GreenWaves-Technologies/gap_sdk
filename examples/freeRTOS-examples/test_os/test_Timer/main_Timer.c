/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestTimer0( void *parameters );
void vTestTimer1( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+2];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE * 2 ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
#define NUM_TIMERS ( 2 )

TimerHandle_t xTimers[ NUM_TIMERS ];

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Timer Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestTimer0,
        "Timer0",
        configMINIMAL_STACK_SIZE * 2,
        ( void * ) 20,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("Timer0 is NULL !\n");
        vTaskDelete( xHandleStatic );
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask, xTask2;
    TaskHandle_t xHandleDynamic = NULL, xHandleDynamic2 = NULL;

    xTask = xTaskCreate(
        vTestTimer0,
        "Timer1",
        configMINIMAL_STACK_SIZE * 2,
        ( void * ) 30,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Timer1 is NULL !\n");
        vTaskDelete( xHandleDynamic );
    }

    xTask2 = xTaskCreate(
        vTestTimer1,
        "TimerCreator",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic2
        );
    if( xTask2 != pdPASS )
    {
        printf("TestTimer21 is NULL !\n");
        vTaskDelete( xHandleDynamic2 );
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

void vTimerCallback0( TimerHandle_t xTimer )
{
    char *taskname = pcTaskGetName( NULL );
    printf("%s : One shot Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
}
/*-----------------------------------------------------------*/

void vTimerCallback1( TimerHandle_t xTimer )
{
    char *taskname = pcTaskGetName( NULL );
    printf("%s : Periodic Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
}
/*-----------------------------------------------------------*/

void vTestTimer0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0;
    for(;;)
    {
        printf("%s : %d.\n", taskname, loop);
        loop++;
        if( loop >= ( uint32_t ) parameters )
        {
            xTaskNotify( tasks[2], ( uint32_t ) parameters, eSetBits );
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestTimer1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulValue = 0x0;

    printf("%s creating Timers.\n", taskname);
    xTimers[0] = xTimerCreate(
        "Timer0",
        pdMS_TO_TICKS( 210 ),
        pdFALSE,
        0,
        vTimerCallback0
        );
    if( xTimers[0] == NULL )
        exit(0);

    xTimers[1] = xTimerCreate(
        "Timer0",
        pdMS_TO_TICKS( 50 ),
        pdTRUE,
        0,
        vTimerCallback1
        );
    if( xTimers[1] == NULL )
        exit(0);

    xTimerStart( xTimers[0], portMAX_DELAY );
    xTimerStart( xTimers[1], portMAX_DELAY );

    printf("%s created and started timers.\n", taskname);
    xTaskNotifyWait( 0x00, 0x00, &ulValue, portMAX_DELAY );
    printf("%s deletes timers and suicides.\n", taskname);
    xTimerStop( xTimers[0], portMAX_DELAY );
    xTimerStop( xTimers[1], portMAX_DELAY );
    xTimerDelete( xTimers[0], portMAX_DELAY );
    xTimerDelete( xTimers[1], portMAX_DELAY );
    printf("Test success\n");
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
