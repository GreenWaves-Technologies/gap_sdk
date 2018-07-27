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
        configMINIMAL_STACK_SIZE,
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
    uint32_t repeat_mode = 0, alarmSec = 0;
    rtc_datetime_t setTime, setAlarm, now;

    /* Year between 2001 - 2099 */
    setTime.year   = 2001;
    setTime.month  = 2;
    setTime.day    = 28;
    setTime.hour   = 23;
    setTime.minute = 59;
    setTime.second = 59;

    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            setTime.year, setTime.month, setTime.day,
            setTime.hour, setTime.minute, setTime.second
        );

    /* Configure and init RTC with default values. */
    RTC_GetDefaultConfig( &config );
    RTC_Init( RTC_APB, &config );

    if( RTC_SetCalendar( RTC_APB, &setTime ) != uStatus_Success )
    {
        printf("Set calendar failed! Test failed !\n");
        vTaskSuspend( NULL );
    }

    RTC_StartCalendar( RTC_APB );

    setAlarm.year   = 2001;
    setAlarm.month  = 3;
    setAlarm.day    = 1;
    setAlarm.hour   = 0;
    setAlarm.minute = 0;
    setAlarm.second = 5;

    if( RTC_SetAlarm( RTC_APB, &setAlarm ) != uStatus_Success )
    {
        printf("Set alarm failed! Test failed !\n");
        vTaskSuspend( NULL );
    }

    RTC_StartAlarm( RTC_APB, repeat_mode );

    RTC_IRQHandlerBind( ( uint32_t ) alarm_irq_handler );

    RTC_GetAlarm( RTC_APB, &now );
    printf ("Alarm    Time set %d/%d/%d %d:%d:%d\n",
            now.year, now.month, now.day,
            now.hour, now.minute, now.second
        );

    alarmSec = RTC_ConvertDatetimeToSeconds( &setAlarm ) - RTC_ConvertDatetimeToSeconds( &setTime );
    printf("Alarm in %d seconds.\n", alarmSec);

    while( !flag )
    {
        if( ( xTaskGetTickCount()/1000 ) > alarmSec )
        {
            printf("Test failed !\n");
            vTaskSuspend( NULL );
        }
        EU_EVT_MaskWaitAndClr( 1 << FC_SW_NOTIF_EVENT );
    }

    /* Read calendar */
    RTC_GetCalendar( RTC_APB, &now );
    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            now.year, now.month, now.day,
            now.hour, now.minute, now.second);

    printf("Test success !\n");

    /* Suicide. */
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
