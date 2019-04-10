/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestSleep( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define VALUE_INIT     0x1
#define VALUE_SET      0x0

#define PMU_RET_SLEEP  uPMU_SWITCH_RETENTIVE_SLEEP
#define PMU_DEEP_SLEEP uPMU_SWITCH_DEEP_SLEEP

#ifdef DEEP_SLEEP
#define PMU_SLEEP_TYPE PMU_DEEP_SLEEP
#else
#define PMU_SLEEP_TYPE PMU_RET_SLEEP
#endif

#define PMU_WAKE_GPIO  0x0
#define PMU_WAKE_RTC   0x1

#ifdef WAKEUP_GPIO
#define PMU_WAKE_TYPE  PMU_WAKE_GPIO
#else
#define PMU_WAKE_TYPE  PMU_WAKE_RTC
#endif

static uint32_t sleep_value = (uint32_t) VALUE_INIT;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** PMU Sleep Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    xTask = xTaskCreate( vTestSleep, "TestSleep", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler0 );
    if( xTask != pdPASS )
    {
        printf("TestFreq is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandler0;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vWakeByRTC( uint32_t seconds )
{
    uint32_t repeat_en = 0;
    rtc_config_t config;
    RTC_GetDefaultConfig( &config );

    RTC_Init( RTC_APB, &config );

    /* Set & start timer. */
    RTC_SetTimer( RTC_APB, seconds );
    RTC_StartTimer( RTC_APB, repeat_en );
}

void vWakeByGPIO( void )
{
}

void vWakeByFunc( uint32_t wakeup )
{
    if( wakeup == ( uint32_t ) PMU_WAKE_RTC )
    {
        vWakeByRTC( 10 );
    }
    else
    {
        vWakeByGPIO();
    }
}

void vTestSleep( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t errors = 0;

    printf("%s executing PMU Sleep function :\n\n", taskname);

    switch( PMU_WakeupState() )
    {
    case uPMU_COLD_BOOT :
        printf("Cold boot : value = %d\n", sleep_value);
        sleep_value = VALUE_SET;
        printf("Cold boot : set value %d\n", sleep_value);
        vWakeByFunc( (uint32_t) PMU_WAKE_TYPE );

        PMU_StateSwitch(PMU_SLEEP_TYPE, uPMU_SWITCH_FAST);
        break;
    case uPMU_RETENTIVE_SLEEP_BOOT :
        printf("Retentive boot : value = %d\n", sleep_value);
        errors += (sleep_value != (uint32_t) VALUE_SET) ? 1 : 0;
        break;
    case uPMU_DEEP_SLEEP_BOOT :
        printf("Deep Sleep boot : value = %d\n", sleep_value);
        errors += (sleep_value != (uint32_t) VALUE_INIT) ? 1 : 0;
        break;
    default :
        printf("Error : Unkmown boot state !\n");
        errors++;
    }

    printf("Test %s with %d error(s) !\n", ((errors) ? "failed" : "success"), errors);
    vTaskSuspend( NULL );
}
