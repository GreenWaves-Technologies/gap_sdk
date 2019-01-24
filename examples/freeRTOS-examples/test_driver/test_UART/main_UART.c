/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_uart.h"
#include "pinmap.h"
#include "PeripheralPins.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestUART( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
static UART_Type *const uart_addrs[] = UART_BASE_PTRS;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : RTC ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestUART,
        "TestUART",
        configMINIMAL_STACK_SIZE * 2,
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

static void serial_init( PinName tx, PinName rx )
{
    uart_config_t config;

    UART_GetDefaultConfig( &config );

    UART_Init( uart_addrs[0], &config, SystemCoreClock );

    pinmap_pinout(tx, PinMap_UART_TX);
    pinmap_pinout(rx, PinMap_UART_RX);

    if( tx != NC )
    {
        UART_EnableTx( uart_addrs[0], true );
        pin_mode( tx, PullUp );
    }
    if( rx != NC )
    {
        UART_EnableRx( uart_addrs[0], true );
        pin_mode( rx, PullUp );
    }
}
/*-----------------------------------------------------------*/

static void serial_format( uart_bit_length_t data_bits, uart_parity_mode_t parity, uart_stop_bit_count_t stop_bits )
{
    /* Set bit count and parity mode. */
    uart_addrs[0]->SETUP |= ~( UART_SETUP_STOP_BITS_MASK |
                      UART_SETUP_BIT_LENGTH_MASK |
                      UART_SETUP_PARITY_ENA_MASK );
    uart_addrs[0]->SETUP |= ( UART_SETUP_PARITY_ENA( parity ) |
                     UART_SETUP_STOP_BITS( stop_bits ) |
                     UART_SETUP_BIT_LENGTH( data_bits ) );
}
/*-----------------------------------------------------------*/

static void serial_break_set()
{
    uint16_t pre_div = ( uart_addrs[0]->SETUP & UART_SETUP_CLKDIV_MASK ) >> UART_SETUP_CLKDIV_SHIFT;

    /* Use a low half baudrate to send '0' to supply a break, about 2 frame time */
    while( UART_TXBusy( uart_addrs[0] ) );
    UART_SetBaudRate( uart_addrs[0], ( SystemCoreClock / pre_div ) >> 1, SystemCoreClock );
    UART_WriteByte( uart_addrs[0], 0 );
}
/*-----------------------------------------------------------*/

static void serial_break_clear()
{
    uint16_t pre_div = ( uart_addrs[0]->SETUP & UART_SETUP_CLKDIV_MASK ) >> UART_SETUP_CLKDIV_SHIFT;

    /* Wait TX end */
    while( UART_TXBusy( uart_addrs[0] ) );
    UART_SetBaudRate( uart_addrs[0], ( SystemCoreClock / pre_div ) << 1, SystemCoreClock );
}
/*-----------------------------------------------------------*/

void vTestUART( void *parameters )
{
    ( void ) parameters;
    uint32_t baudrate = 115200;
    uint8_t get = 0, expect = 'c';

    /* Serial pins init */
    serial_init( USBTX, USBRX );

    printf("USBTX  = %d \n", USBTX);
    printf("USBRX  = %d \n", USBRX);

    /* Serial format 8N1 configuration : 8 bits, parity disabled, 2 stop bits. */
    serial_format( uUART_8bits, uUART_ParityDisabled, uUART_TwoStopBit );

    /* Serial baudrate 115200 configuration */
    UART_SetBaudRate( uart_addrs[0], baudrate, SystemCoreClock );

    printf("Please press 'c' to continue...\n");

    /* Write a char. */
    UART_WriteByte( uart_addrs[0], expect );
    /* Read a char. */
    get = UART_ReadByte( uart_addrs[0] );

    /* Serial set break */
    serial_break_set();
    vTaskDelay( 18000 / baudrate );
    /* Serial break clear */
    serial_break_clear();

    /* Free serial. */
    UART_Deinit( uart_addrs[0] );

    printf("Get char ascii = %d <-> expect char ascii = %d \n", get, expect);

    /* Check read values */
    uint8_t error = ( get != expect );

    if( error )
        printf("Test failed.\n");
    else
        printf("Test success.\n");
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
