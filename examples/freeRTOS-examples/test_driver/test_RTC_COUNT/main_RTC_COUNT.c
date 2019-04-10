/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestRTCCount( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define TIMER_SECONDS 5

uint32_t flag = 0;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : RTC Count down ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestRTCCount,
        "TestRTCCount",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestRTCCount is NULL !\n");
        vTaskDelete( xHandleDynamic );
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void timer_irq_handler( void *arg )
{
    printf("Count down arrive ..... %x\n",*( ( uint32_t *) arg ));
    flag = 1;
}
/*-----------------------------------------------------------*/

void vTestRTCCount( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    printf("%s executing RTC Count test.\n", taskname);

    rtc_config_t config;
    uint32_t repeat_nb = 3;
    uint32_t repeat_en = ( repeat_nb != 0 );

    /* Configure and init RTC with default values. */
    RTC_GetDefaultConfig( &config );

    RTC_Init( RTC_APB, &config );

    /* Set Timer. */
    RTC_SetTimer( RTC_APB, TIMER_SECONDS );

    /* Binding RTC IRQ. */
    uint32_t arg = 0xabbadead;
    RTC_CreateHandler( timer_irq_handler, &arg );

    printf("Start Timer value now = %d, repeat %d time(s).\n", TIMER_SECONDS, repeat_nb);

    /* Start Timer */
    RTC_StartTimer( RTC_APB, repeat_en );

    uint32_t timer_now = 0;
    uint8_t timer = 0;
    do
    {
        timer_now = RTC_GetTimer( RTC_APB );
        printf("Timer value now = %d\n", timer_now);

        /* Wait 1 second. */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        if( flag )
        {
            timer++;
            flag = 0;
            RTC_ClearIRQFlags(RTC_APB, uRTC_TimerFlag);
        }
    } while( timer < repeat_nb );

    if( !timer )
        printf("Test failed\n");
    else
        printf("Test success\n");

    /* Suicide. */
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
