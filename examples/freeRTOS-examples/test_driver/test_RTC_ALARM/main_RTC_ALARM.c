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
uint32_t flag = 0;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : RTC Alarm ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestDriverRTC,
        "TestDriverRTC",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Test RTC Alarm is NULL !\n");
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

void alarm_irq_handler( void )
{
    printf("RTC Alarm .....\n");
    flag = 1;
}
/*-----------------------------------------------------------*/

void vTestDriverRTC( void *parameters )
{
    printf("Fabric controller code execution for RTC driver test\n");

    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);

    rtc_datetime_t setTime, now;

    /* Year between 2001 - 2099 */
    setTime.year   = 2001;
    setTime.month  = 2;
    setTime.day    = 28;
    setTime.hour   = 23;
    setTime.minute = 59;
    setTime.second = 59;

    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            setTime.year,
            setTime.month,
            setTime.day,
            setTime.hour,
            setTime.minute,
            setTime.second);

    /* Set calendar */
    if (RTC_SetCalendar(RTC_APB, &setTime) != uStatus_Success) {
        printf("Set calendar failed!\n");
        vTaskSuspend( NULL );
    }

    /* Start calendar */
    RTC_StartCalendar(RTC_APB);

    for(int i = 0; i < 10; i++) {
        /* Wait 1 second */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        /* Read calendar */
        RTC_GetCalendar(RTC_APB, &now);

        printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
                now.year,
                now.month,
                now.day,
                now.hour,
                now.minute,
                now.second);
    }

    int repeat_mode = 0;
    setTime.year   = 2001;
    setTime.month  = 3;
    setTime.day    = 1;
    setTime.hour   = 0;
    setTime.minute = 0;
    setTime.second = 15;

    /* Set Alarm 2001/03/01/00:00:15 */
    if (RTC_SetAlarm(RTC_APB, &setTime) != uStatus_Success) {
        printf("Set alarm failed!\n");
        vTaskSuspend( NULL );
    }

    /* Start Alarm */
    RTC_StartAlarm(RTC_APB, repeat_mode);

    /* Binding RTC IRQ */
    RTC_IRQHandlerBind((uint32_t)alarm_irq_handler);

    /* Read RTC Alarm */
    RTC_GetAlarm(RTC_APB, &now);
    printf ("Alarm    Time set %d/%d/%d %d:%d:%d\n",
            now.year,
            now.month,
            now.day,
            now.hour,
            now.minute,
            now.second);

    for(int i = 0; i < 10; i++) {
        /* Wait 1 second */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        /* Read calendar */
        RTC_GetCalendar(RTC_APB, &now);

        printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
                now.year,
                now.month,
                now.day,
                now.hour,
                now.minute,
                now.second);
    }

    int error = 0;
    if (!flag) {
        printf("Test failed\n");
        error = 1;
    } else {
        printf("Test success\n");
    }

    /* Suicide. */
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
