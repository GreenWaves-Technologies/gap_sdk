/****************************************************************************/
/**
 * Leds blinking demo.
 */
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestGPIO( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

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
    printf("\n\n\t *** GPIO Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestGPIO,
        "TestGPIO",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestGPIO is NULL !\n");
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

void vTestGPIO( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint8_t ucLedState1 = 0, ucLedState2 = 0;

    /* Pin information on both leds xLed1 and xLed2 : */
    PinName xLed1        = GPIO_A18;
    uint32_t ulPort_nb1  = GET_GPIO_PORT( xLed1 );
    uint32_t ulGpio_nb1  = GET_GPIO_NUM( xLed1 );
    uint32_t ulPin_nb1   = GET_GPIO_PIN_NUM( xLed1 );
    PinName xLed2        = GPIO_A20;
    uint32_t ulPort_nb2  = GET_GPIO_PORT( xLed2 );
    uint32_t ulGpio_nb2  = GET_GPIO_NUM( xLed2 );
    uint32_t ulPin_nb2   = GET_GPIO_PIN_NUM( xLed2 );

    /* Init both PORT as GPIO/Alter1. */
    port_pin_config_t xPort_config = { .pullSelect = uPORT_PullUpEnable,
                                       .driveStrength = uPORT_LowDriveStrength,
                                       .mux = uPORT_MuxAlt1};
    PORT_SetPinConfig( xPort_addrs[ulPort_nb1], ulPin_nb1, &xPort_config);
    PORT_SetPinConfig( xPort_addrs[ulPort_nb2], ulPin_nb2, &xPort_config);

    /* Init both GPIO as OUTPUT. */
    gpio_pin_config_t xGpio_config = { .pinDirection = uGPIO_DigitalOutput,
                                       .outputLogic = uGPIO_LOW };
    GPIO_PinInit( xGpio_addrs[ulPort_nb1], ulGpio_nb1, &xGpio_config );
    GPIO_PinInit( xGpio_addrs[ulPort_nb2], ulGpio_nb2, &xGpio_config );

    /* Read GPIO state and print values. */
    ucLedState1 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb1], ulGpio_nb1 );
    ucLedState2 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb2], ulGpio_nb2 );
    printf("Led1 = %d\t Led2 = %d\n", ucLedState1, ucLedState2);
    for(;;)
    {
        /* xLed1 On, xLed2 Off.*/
        GPIO_WritePinOutput( xGpio_addrs[ulPort_nb1], ulGpio_nb1, uGPIO_HIGH );
        GPIO_WritePinOutput( xGpio_addrs[ulPort_nb2], ulGpio_nb2, uGPIO_LOW );
        /* Read GPIO state and print values. */
        ucLedState1 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb1], ulGpio_nb1 );
        ucLedState2 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb2], ulGpio_nb2 );
        printf("Led1 = %d\t Led2 = %d\n", ucLedState1, ucLedState2);

        vTaskDelay( 100 / portTICK_PERIOD_MS );

        /* xLed1 On, xLed2 Off.*/
        GPIO_WritePinOutput( xGpio_addrs[ulPort_nb1], ulGpio_nb1, uGPIO_LOW );
        GPIO_WritePinOutput( xGpio_addrs[ulPort_nb2], ulGpio_nb2, uGPIO_HIGH );
        /* Read GPIO state and print values. */
        ucLedState1 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb1], ulGpio_nb1 );
        ucLedState2 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb2], ulGpio_nb2 );
        printf("Led1 = %d\t Led2 = %d\n", ucLedState1, ucLedState2);

        vTaskDelay( 100 / portTICK_PERIOD_MS );

        if( xTaskGetTickCount() >= 5000 )
        {
            if( ucLedState1 != ucLedState2 )
                printf("Test success.\n");
            else
                printf("Test failed.\n");
            printf("%s suspending.\n", taskname);
            vTaskSuspend( NULL );
        }
    }
}
