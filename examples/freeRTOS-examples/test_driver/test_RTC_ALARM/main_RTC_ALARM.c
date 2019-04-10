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
const char *repeat[] = {"NO_RPT", "NO_RPT", "NO_RPT",
                        "RPT_SEC", "RPT_MIN", "RPT_HOUR",
                        "RPT_DAY", "RPT_MON", "RPT_YEAR"};

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : RTC Alarm ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
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

void alarm_irq_handler( void *arg )
{
    printf("RTC Alarm ..... %x\n",*( ( uint32_t *) arg ));
    flag = 1;
}
/*-----------------------------------------------------------*/

void vTestDriverRTC( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    printf("%s executing RTC Alarm test.\n", taskname);

    rtc_config_t config;
    RTC_GetDefaultConfig( &config );

    RTC_Init( RTC_APB, &config );

    rtc_datetime_t setTime, now;

    setTime.year   = 2001;
    setTime.month  = 2;
    setTime.day    = 28;
    setTime.hour   = 23;
    setTime.minute = 59;
    setTime.second = 59;

    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            setTime.year, setTime.month, setTime.day,
            setTime.hour, setTime.minute, setTime.second);

    /* Set calendar. */
    if( RTC_SetCalendar( RTC_APB, &setTime ) != uStatus_Success )
    {
        printf("Set calendar failed!\n");
        vTaskSuspend( NULL );
    }

    /* Start calendar. */
    RTC_StartCalendar( RTC_APB );

    for( uint32_t i = 0; i < 5; i++ )
    {
        /* Wait 1 second. */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        /* Read calendar. */
        RTC_GetCalendar(RTC_APB, &now);

        printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
                now.year, now.month, now.day,
                now.hour, now.minute, now.second);
    }

    uint8_t repeat_mode = RPT_MON;
    setTime.year   = 2001;
    setTime.month  = 3;
    setTime.day    = 1;
    setTime.hour   = 0;
    setTime.minute = 0;
    setTime.second = 15;

    /* Set Alarm 2001/03/01/00:00:15. */
    if( RTC_SetAlarm( RTC_APB, &setTime, repeat_mode ) != uStatus_Success )
    {
        printf("Set alarm failed!\n");
        vTaskSuspend( NULL );
    }

    /* Start Alarm. */
    RTC_StartAlarm( RTC_APB );

    /* Binding RTC IRQ. */
    uint32_t arg = 0xabbadead;
    RTC_CreateHandler( alarm_irq_handler, &arg );

    /* Read RTC Alarm. */
    RTC_GetAlarm( RTC_APB, &now );
    printf ("Alarm    Time set %d/%d/%d %d:%d:%d %s\n",
            now.year, now.month, now.day,
            now.hour, now.minute, now.second, repeat[repeat_mode]);

    uint8_t alarm = 0;
    do
    {
        /* Wait 1 second. */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        /* Read calendar. */
        RTC_GetCalendar( RTC_APB, &now );
        printf("Calendar Time now %d/%d/%d %d:%d:%d\n",
               now.year, now.month, now.day,
               now.hour, now.minute, now.second);

        if( flag )
        {
            alarm++;
            flag = 0;
            RTC_ClearIRQFlags(RTC_APB, uRTC_AlarmFlag);

            if( ( repeat_mode < RPT_SEC ) || ( RPT_YEAR < repeat_mode ) )
            {
                alarm = 5;
            }
            else
            {
                /* Set calendar. */
                setTime.year   = ( repeat_mode == RPT_YEAR ) ? setTime.year   + 1 : setTime.year;
                setTime.month  = ( repeat_mode == RPT_MON  ) ? setTime.month  + 1 : setTime.month;
                setTime.day    = ( repeat_mode == RPT_DAY  ) ? setTime.day    + 1 : setTime.day;
                setTime.hour   = ( repeat_mode == RPT_HOUR ) ? setTime.hour   + 1 : setTime.hour;
                setTime.minute = ( repeat_mode == RPT_MIN  ) ? setTime.minute + 1 : setTime.minute;
                setTime.second = 10;
                if( RPT_SEC < repeat_mode && RTC_SetCalendar( RTC_APB, &setTime ) != uStatus_Success )
                {
                    printf("Set calendar failed! Test failed!\n");
                    exit( -1 );
                }
            }
        }
    } while( alarm < 5 );

    if( !alarm )
        printf("Test failed\n");
    else
        printf("Test success\n");

    /* Suicide. */
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
