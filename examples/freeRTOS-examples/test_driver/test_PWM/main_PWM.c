/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_pwm.h"
#include "PinNames.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestPWM( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/
/*
 * Freq = Clk_src / ( TH[31-16] - TH[15-0] )
 * Rcyc = ( ( CH_TH - TH[15-0] ) / ( TH[31-16] - CH_TH ) ) ? 1 : 0
 */

/* Variables used. */
#define TH0     0x88881111
#define TH1     0x66662222
#define TH2     0x55553333
#define TH3     0x77774444

/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const xGpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const xPort_addrs[] = PORT_BASE_PTRS;
static PWM_Type *const xPwm_addrs[] = PWM_BASE_PTRS;
static uint8_t flag = 0;

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
        vTestPWM,
        "TestPWM",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestPWM is NULL !\n");
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
    printf("GPIO Button triggered. Leaving...\n");
    flag++;
}
/*-----------------------------------------------------------*/

static void prvPWM_handler( void *arg )
{
    flag++;
    if( arg != NULL )
        printf("PWM%d IRQ : %x\n", PWM_GetInstance( ( PWM_Type * ) arg ), ( ( PWM_Type * ) arg )->COUNTER);
    else
        printf("Simple PWM IRQ! %d\n", flag);
}
/*-----------------------------------------------------------*/

void vTestPWM( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    PinName xButton     = GPIO_A2_A2;
    uint32_t ulPort_nb  = GET_GPIO_PORT( xButton );
    uint32_t ulGpio_nb  = GET_GPIO_NUM( xButton );
    uint32_t ulPin_nb   = GET_GPIO_PIN_NUM( xButton );
    /* Init PORT - MUX as GPIO/Alter1. */
    port_pin_config_t xPort_config = { .pullSelect = uPORT_PullUpEnable,
                                       .driveStrength = uPORT_LowDriveStrength,
                                       .mux = uPORT_MuxAlt1 };
    PORT_SetPinConfig( xPort_addrs[ulPort_nb], ulPin_nb, &xPort_config);

    /* Init GPIO - INPUT. */
    gpio_pin_config_t xGpio_config = { .pinDirection = uGPIO_DigitalInput,
                                       .outputLogic = uGPIO_LOW };
    GPIO_PinInit( xGpio_addrs[ulPort_nb], ulGpio_nb, &xGpio_config );

    /* Bind GPIO IRQ handler */
    GPIO_IRQHandlerBind( xGpio_addrs[ulPort_nb], ( uint32_t ) prvGPIO_handler );

    /* GPIO IRQ enable */
    SOC_EU_SetFCMask( GPIO_EVENT );

    /* GPIO trigger IRQ by falling edge */
    GPIO_SetPinInterruptConfig( xGpio_addrs[ulPort_nb], ulGpio_nb, uGPIO_InterruptFallingEdge );

    uint32_t timer_cfg = 0;
    pwm_config_t config[4];
    pwm_signal_param_t param = { .chnum = ( pwm_channel_t ) 0,
                                 .dutyCyclePercent = 50 };
    uint32_t th[4] = {TH0, TH1, TH2, TH3 };

    for( uint32_t i = 0; i < 4; i++ )
    {
        /* PWM Configuration. */
        PWM_GetDefaultConfig( &config[i] );
        config[i].evtSel = ( uPWM_EVENT_SEL0 + i );
        param.chnum = ( uPWM_TH_CHANNEL0 + i );

        /* PWM Init. */
        PWM_Init( xPwm_addrs[i], &config[i] );

        /* Default to 20ms: standard for servos, and fine for e.g. brightness control */
        PWM_SetupPwm( xPwm_addrs[i], &param, 1, 10, uPWM_REF_32K );

        /* Set an output event and enable it. */
        PWM_SetOutputEvent( xPwm_addrs[i], param.chnum, config[i].evtSel );

        /* Bind PWM IRQ handler. */
        PWM_IRQHandlerBind( xPwm_addrs[i], ( uint32_t ) prvPWM_handler,( void * ) xPwm_addrs[i] );
    }

    /* Start each timer. */
    for( uint32_t i = 0; i < 4; i++ )
        PWM_StartTimer( xPwm_addrs[i] );

    printf("Press on the button to exit programm.\n");
    printf("\tSleeping for 1s.\n");
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    printf("\tComing back.\n");
    if( flag )
    {
        for( uint32_t i = 0; i < 4; i++ )
        {
            PWM_ClearOutputEvent( xPwm_addrs[i], param.chnum, config[i].evtSel );
            PWM_Deinit( xPwm_addrs[i] );
        }
        printf("Test success. %s suspending.\n", taskname);
    }
    else
        printf("Test failed. %s suspending.\n", taskname);
    vTaskSuspend( NULL );
}
