/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestNotif0( void *parameters );
void vTestNotif1( void *parameters );
void vTestNotif2( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+2];
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
    printf("\n\n\t *** Notification Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestNotif0,
        "TestNotif0",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL)
    {
        printf("TestNotif0 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestNotif1,
        "TestNotif1",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestNotif1 is NULL !\n");
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

void vTestNotif0( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulValue = 0;
    for(;;)
    {
        printf("|-->%s sending a notification to increment.\n", taskname);
        xTaskNotify( tasks[1], 0, eIncrement );
        printf("|->%s waiting(blocking) for a notification !\n", taskname);
        xTaskNotifyWait( 0x00, portMAX_DELAY, &ulValue, portMAX_DELAY );
        printf("\n|<-%s received notification, it sets bits VAL = %d.\n", taskname, ulValue);
        if( ulValue == 0xff )
        {
            printf("|->%s received a suspend notification, suspending !\n", taskname);
            printf("Test success\n");
            vTaskSuspend( NULL );
        }
        if( ulValue >= 0x1f )
        {
            printf("|->%s sends a suspend notification and suicide !\n", taskname);
            xTaskNotify( tasks[1], 0xff, eSetValueWithOverwrite );
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestNotif1( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t taskNotif = 0, ulValue = 0;
    for(;;)
    {
        printf("\n|->%s waiting(blocking) for a notification !\n", taskname);
        xTaskNotifyWait( 0x00, 0x00, &ulValue, portMAX_DELAY );
        if( ulValue == 0xff )
        {
            printf("|->%s received a suspend notification, suspending !\n", taskname);
            printf("Test success\n");
            vTaskSuspend( NULL );
        }
        if( ulValue >= 0x0f )
        {
            printf("|->%s sends a suspend notification and suicide !\n", taskname);
            xTaskNotify( tasks[0], 0xff, eSetValueWithOverwrite );
            vTaskSuspend( NULL );
        }
        printf("\n|<-%s received notification, it increments VAL = %d.\n", taskname, ulValue);
        printf("|-->%s sending a notification to set bits.\n", taskname);
        taskNotif += 2;
        xTaskNotify( tasks[0], taskNotif, eSetBits );
    }
}
/*-----------------------------------------------------------*/
