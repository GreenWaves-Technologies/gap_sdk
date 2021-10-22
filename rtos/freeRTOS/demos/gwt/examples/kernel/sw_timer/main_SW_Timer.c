/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[3] = {NULL};

#define TIMERS ( 2 )
TimerHandle_t xTimers[ TIMERS ];

void vTimerCallback0( TimerHandle_t xTimer )
{
    char *taskname = pcTaskGetName( NULL );
    printf("%s : One shot Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
}

void vTimerCallback1( TimerHandle_t xTimer )
{
    char *taskname = pcTaskGetName( NULL );
    printf("%s : Periodic Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
}

void vTestTimer( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0;
    for(;;)
    {
        printf("%s : %d.\n", taskname, loop);
        loop++;
        if( loop >= ( uint32_t ) parameters )
        {
            xTaskNotify( xHandler[2], ( uint32_t ) parameters, eSetBits );
            printf("%s suspending\n", taskname);
            vTaskSuspend( NULL );
        }
    }
}

void vTimerCreator( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulValue = 0, wait_val = 0;

    printf("%s creating Timers.\n", taskname);
    xTimers[0] = xTimerCreate( "Timer0", pdMS_TO_TICKS( 110 ), pdFALSE,
                               0, vTimerCallback0 );
    if( xTimers[0] == NULL )
    {
        exit(0);
    }
    xTimers[1] = xTimerCreate( "Timer1", pdMS_TO_TICKS( 50 ), pdTRUE,
                               0, vTimerCallback1 );
    if( xTimers[1] == NULL )
    {
        exit(0);
    }
    xTimerStart( xTimers[0], portMAX_DELAY );
    xTimerStart( xTimers[1], portMAX_DELAY );

    printf("%s created and started timers.\n", taskname);
    while ( wait_val != 150 )
    {
        xTaskNotifyWait( 0xFFFFFFFF, 0x00, &ulValue, portMAX_DELAY );
        wait_val += ulValue;
    }
    printf("%s deleting timers and suspending.\n", taskname);
    xTimerStop( xTimers[0], portMAX_DELAY );
    xTimerStop( xTimers[1], portMAX_DELAY );
    xTimerDelete( xTimers[0], portMAX_DELAY );
    xTimerDelete( xTimers[1], portMAX_DELAY );

    vTaskSuspend( NULL );
}

void test_sw_timer( void )
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestTimer, "TaskTimer0", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 50, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("TaskTimer0 is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestTimer, "TaskTimer1", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 100, tskIDLE_PRIORITY + 1, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("TaskTimer1 is NULL !\n");
        pmsis_exit(-2);
    }

    xTask = xTaskCreate( vTimerCreator, "TimerCreator", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 2, &xHandler[2] );
    if( xTask != pdPASS )
    {
        printf("TimerCreator is NULL !\n");
        pmsis_exit(-3);
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) ||
           ( eTaskGetState(xHandler[2]) != eSuspended ) )
    {
        pi_yield();
    }

    printf("Test success !\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** SW Timer Test ***\n\n");
    return pmsis_kickoff((void *) test_sw_timer);
}
