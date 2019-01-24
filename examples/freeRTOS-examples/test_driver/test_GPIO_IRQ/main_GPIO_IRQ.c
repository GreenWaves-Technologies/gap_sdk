/****************************************************************************/
/**
 * Leds blinking demo using a button to switch on and off leds.
 */
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestGPIO_IRQ( void *parameters );

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
    printf("\n\n\t *** GPIO IRQ Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestGPIO_IRQ,
        "TestGPIO_IRQ",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestGPIO_IRQ is NULL !\n");
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

static void prvGPIO_handler( void )
{
    count++;
    printf("GPIO Button triggered %d time(s).\n", count);
}
/*-----------------------------------------------------------*/

void vTestGPIO_IRQ( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulPrevCount = count;
    uint8_t ucLedState1 = 0, ucLedState2 = 0;

    /* Pin information on both leds xLed1 and xLed2, and xButton : */
    PinName xLed1        = GPIO_A18;
    uint32_t ulPort_nb1  = GET_GPIO_PORT( xLed1 );
    uint32_t ulGpio_nb1  = GET_GPIO_NUM( xLed1 );
    uint32_t ulPin_nb1   = GET_GPIO_PIN_NUM( xLed1 );
    PinName xLed2        = GPIO_A20;
    uint32_t ulPort_nb2  = GET_GPIO_PORT( xLed2 );
    uint32_t ulGpio_nb2  = GET_GPIO_NUM( xLed2 );
    uint32_t ulPin_nb2   = GET_GPIO_PIN_NUM( xLed2 );
    PinName xButton      = GPIO_A2_A2;
    uint32_t ulPort_nb3  = GET_GPIO_PORT( xButton );
    uint32_t ulGpio_nb3  = GET_GPIO_NUM( xButton );
    uint32_t ulPin_nb3   = GET_GPIO_PIN_NUM( xButton );

    /* Init all three PORT as GPIO/Alter1. */
    port_pin_config_t xPort_config = { .pullSelect = uPORT_PullUpEnable,
                                       .driveStrength = uPORT_LowDriveStrength,
                                       .mux = uPORT_MuxAlt1};
    PORT_SetPinConfig( xPort_addrs[ulPort_nb1], ulPin_nb1, &xPort_config);
    PORT_SetPinConfig( xPort_addrs[ulPort_nb2], ulPin_nb2, &xPort_config);
    PORT_SetPinConfig( xPort_addrs[ulPort_nb3], ulPin_nb3, &xPort_config);

    /* Init both led GPIO as OUTPUT. */
    gpio_pin_config_t xGpio_config = { .pinDirection = uGPIO_DigitalOutput,
                                       .outputLogic = uGPIO_LOW };
    GPIO_PinInit( xGpio_addrs[ulPort_nb1], ulGpio_nb1, &xGpio_config );
    GPIO_PinInit( xGpio_addrs[ulPort_nb2], ulGpio_nb2, &xGpio_config );
    /* Init button GPIO as INPUT. */
    xGpio_config.pinDirection = uGPIO_DigitalInput;
    GPIO_PinInit( xGpio_addrs[ulPort_nb3], ulGpio_nb3, &xGpio_config );

    /* Bind GPIO IRQ handler. */
    GPIO_IRQHandlerBind( xGpio_addrs[ulPort_nb3], ( uint32_t ) prvGPIO_handler );

    /* Enable GPIO IRQ. */
    SOC_EU_SetFCMask( GPIO_EVENT );

    /* GPIO trigger IRQ by falling edge. */
    GPIO_SetPinInterruptConfig( xGpio_addrs[ulPort_nb3], ulGpio_nb3, uGPIO_InterruptFallingEdge );

    printf("Press button to switch on and off leds.\n");
    /* Read GPIO state and print values. */
    ucLedState1 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb1], ulGpio_nb1 );
    ucLedState2 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb2], ulGpio_nb2 );
    printf("Led1 = %d\t Led2 = %d\n", ucLedState1, ucLedState2);
    for(;;)
    {
        if( ulPrevCount != count )
        {
            ulPrevCount = count;
            if( count % 2 )
            {
                GPIO_WritePinOutput( xGpio_addrs[ulPort_nb1], ulGpio_nb1, uGPIO_HIGH );
                GPIO_WritePinOutput( xGpio_addrs[ulPort_nb2], ulGpio_nb2, uGPIO_LOW );
            }
            else
            {
                GPIO_WritePinOutput( xGpio_addrs[ulPort_nb1], ulGpio_nb1, uGPIO_LOW );
                GPIO_WritePinOutput( xGpio_addrs[ulPort_nb2], ulGpio_nb2, uGPIO_HIGH );
            }
            /* Read GPIO state and print values. */
            ucLedState1 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb1], ulGpio_nb1 );
            ucLedState2 = GPIO_ReadPinInput( xGpio_addrs[ulPort_nb2], ulGpio_nb2 );
            printf("Led1 = %d\t Led2 = %d\n", ucLedState1, ucLedState2);
        }
        if( count >= 10 )
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
