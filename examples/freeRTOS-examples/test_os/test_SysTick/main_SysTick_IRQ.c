/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestSysTick( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+1];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE * 2 ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** SysTick Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestSysTick,
        "TestSysTick0",
        configMINIMAL_STACK_SIZE * 2,
        ( void * ) 20,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("TestSysTick0 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestSysTick,
        "TestSysTick1",
        configMINIMAL_STACK_SIZE * 2,
        ( void * ) 30,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestSysTick1 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleStatic;
    tasks[1] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestSysTick( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0;
    for(;;)
    {
        loop += 1;
        printf("\nHello world %s %d\n", taskname, loop);
        if( loop == ( uint32_t ) parameters )
        {
            printf("|->%s suicide with loop = %d.\n", taskname, loop);
            printf("Test success\n");
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/
