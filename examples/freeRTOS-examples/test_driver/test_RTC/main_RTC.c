/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestDriverRTC( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define COUNT_SECONDS 10

uint32_t flag = 0;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : RTC ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestDriverRTC,
        "TestDriverRTC",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Task Dynamic is NULL !\n");
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

void timer_irq_handler( void )
{
    printf("Count down arrive .....\n");
    flag = 1;
}
/*-----------------------------------------------------------*/

void vTestDriverRTC( void *parameters )
{
    printf("Fabric controller code execution for RTC driver test\n");

    rtc_config_t config;
    uint32_t repeat_en = 0;

    /* Configure and init RTC with default values. */
    RTC_GetDefaultConfig( &config );

    RTC_Init( RTC_APB, &config );

    /* Set Timer */
    RTC_SetCountDown( RTC_APB, COUNT_SECONDS );

    /* Binding RTC IRQ */
    RTC_IRQHandlerBind( ( uint32_t ) timer_irq_handler );

    printf("Start CountDown value now = %d\n", COUNT_SECONDS);

    /* Start Timer */
    RTC_StartCountDown( RTC_APB, repeat_en );

    /* Wait several seconds */
    vTaskDelay( pdMS_TO_TICKS( 2000 ) );

    /* Read Timer */
    uint32_t countdown_now = RTC_GetCountDown( RTC_APB );
    printf("After 2 seconds, CountDown value now = %d\n", countdown_now);

    vTaskDelay( pdMS_TO_TICKS( 10000 ) );

    if ( !flag && (countdown_now != ( COUNT_SECONDS - 2 ) ) )
        printf("Test failed\n");
    else
        printf("Test success\n");

    /* Suicide. */
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
