/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[2] = {NULL};

void vTestNotif0( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulValue = 0;
    for(;;)
    {
        printf("%s sending a notification to increment.\n", taskname);
        xTaskNotify( xHandler[1], 0, eIncrement );
        printf("\n%s waiting(blocking) for a notification !\n", taskname);
        xTaskNotifyWait( 0x00, portMAX_DELAY, &ulValue, portMAX_DELAY );
        printf("%s received notification, it sets bits VAL = %d.\n", taskname, ulValue);
        if( ulValue == 0xFF )
        {
            printf("%s received a suspend notification, suspending !\n", taskname);
            vTaskSuspend( NULL );
        }
        if( ulValue >= 0x1F )
        {
            printf("%s sends a suspend notification and suspending !\n", taskname);
            xTaskNotify( xHandler[1], 0xFF, eSetValueWithOverwrite );
            vTaskSuspend( NULL );
        }
    }
}

void vTestNotif1( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t taskNotif = 0, ulValue = 0;
    for(;;)
    {
        printf("\n%s waiting(blocking) for a notification !\n", taskname);
        xTaskNotifyWait( 0x00, 0x00, &ulValue, portMAX_DELAY );
        if( ulValue == 0xFF )
        {
            printf("%s received a suspend notification, suspending !\n", taskname);
            vTaskSuspend( NULL );
        }
        if( ulValue >= 0x0F )
        {
            printf("%s sends a suspend notification and suspending !\n", taskname);
            xTaskNotify( xHandler[0], 0xFF, eSetValueWithOverwrite );
            vTaskSuspend( NULL );
        }
        printf("%s received notification, it increments VAL = %d.\n", taskname, ulValue);
        printf("%s sending a notification to set bits.\n", taskname);
        taskNotif += 2;
        xTaskNotify( xHandler[0], taskNotif, eSetBits );
    }
}

void test_notification(void)
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestNotif0, "TaskNotif0", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("TaskNotif0 is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestNotif1, "TaskNotif1", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("TaskTimer1 is NULL !\n");
        pmsis_exit(-2);
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) )
    {
        pi_yield();
    }

    printf("Test success !\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Notification Test ***\n\n");
    return pmsis_kickoff((void *) test_notification);
}
