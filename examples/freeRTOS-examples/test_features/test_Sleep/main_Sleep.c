/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "sleep.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestSleep( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended = 0;

/****************************************************************************/

/* Variables used. */
/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const xGpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const xPort_addrs[] = PORT_BASE_PTRS;

static uint32_t count = 0;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Sleep Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestSleep,
        "TestSleep",
        configMINIMAL_STACK_SIZE * 1,
        0,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestSleep is NULL !\n");
        exit(0);
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
    count = 1;
}
/*-----------------------------------------------------------*/

void setAlaram( void )
{
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
    if (RTC_SetCalendar(RTC_APB, &setTime) != uStatus_Success)
    {
        printf("Set calendar failed!\n");
        vTaskSuspend( NULL );
    }

    /* Start calendar */
    RTC_StartCalendar(RTC_APB);

    setTime.year   = 2001;
    setTime.month  = 3;
    setTime.day    = 1;
    setTime.hour   = 0;
    setTime.minute = 0;
    setTime.second = 05;

    /* Set Alarm 2001/03/01/00:00:15 */
    if (RTC_SetAlarm(RTC_APB, &setTime) != uStatus_Success)
    {
        printf("Set alarm failed!\n");
        vTaskSuspend( NULL );
    }

    /* Start Alarm */
    RTC_StartAlarm(RTC_APB, 0);

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
}
/*-----------------------------------------------------------*/

static void prvGPIO_handler( void )
{
    count++;
    printf("GPIO Button triggered %d time(s).\n", count);
}
/*-----------------------------------------------------------*/

void setGPIO( void )
{
    PinName xButton      = GPIO_A2_A2;
    uint32_t ulPort_nb3  = GET_GPIO_PORT( xButton );
    uint32_t ulGpio_nb3  = GET_GPIO_NUM( xButton );
    uint32_t ulPin_nb3   = GET_GPIO_PIN_NUM( xButton );


    /* Init button GPIO as INPUT. */
    port_pin_config_t xPort_config = { .pullSelect = uPORT_PullUpEnable,
                                       .driveStrength = uPORT_LowDriveStrength,
                                       .mux = uPORT_MuxAlt1};
    PORT_SetPinConfig( xPort_addrs[ulPort_nb3], ulPin_nb3, &xPort_config);

    gpio_pin_config_t xGpio_config = { .pinDirection = uGPIO_DigitalOutput,
                                       .outputLogic = uGPIO_LOW };
    xGpio_config.pinDirection = uGPIO_DigitalInput;
    GPIO_PinInit( xGpio_addrs[ulPort_nb3], ulGpio_nb3, &xGpio_config );

    /* Bind GPIO IRQ handler. */
    GPIO_IRQHandlerBind( xGpio_addrs[ulPort_nb3], ( uint32_t ) prvGPIO_handler );

    /* Enable GPIO IRQ. */
    SOC_EU_SetFCMask( GPIO_EVENT );

    /* GPIO trigger IRQ by falling edge. */
    GPIO_SetPinInterruptConfig( xGpio_addrs[ulPort_nb3], ulGpio_nb3, uGPIO_InterruptFallingEdge );
}
/*-----------------------------------------------------------*/

void vTestSleep( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );

    printf("Hello World %s!\n", taskname);

    setAlaram();
    //setGPIO();

    printf("Before sleep %d ticks.\n", xTaskGetTickCount());

    sleep( FC_SOC_EVENT_IRQn );

    printf("After sleep %d ticks.\n", xTaskGetTickCount());

    if( count )
        vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
