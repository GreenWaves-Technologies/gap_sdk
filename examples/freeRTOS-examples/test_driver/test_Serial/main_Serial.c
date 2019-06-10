/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "stdio.h"
#include "gap_common.h"
#include "gap_uart.h"
#include "pinmap.h"
#include "PeripheralPins.h"
#include "serial_api.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestUART( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define BUFFER_SIZE 256
static uint8_t tx_buffer[BUFFER_SIZE];

static UART_Type *const uart_addrs[] = UART_BASE_PTRS;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : UART ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    xTask = xTaskCreate( vTestUART, "TestUART", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler0 );
    if( xTask != pdPASS )
    {
        printf("Task Dynamic is NULL !\n");
        vTaskDelete( xHandler0 );
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

void vTestUART( void *parameters )
{
    ( void ) parameters;
    serial_t pc;
    uint8_t get = 0, expect = 'c';

    #ifndef PRINTF_UART
    uint32_t baudrate = 115200;

    /* Serial pins init. */
    serial_init(&pc, USBTX, USBRX);

    /* Serial format 8N1 configuration : 8 bits, parity disabled, 2 stop bits. */
    serial_format(&pc, 8, 0, 1);

    /* Serial baudrate configuration. */
    serial_baud(&pc, baudrate);
    #endif  /* PRINTF_UART */

    printf("USBTX  = %d \n", USBTX);
    printf("USBRX  = %d \n", USBRX);

    char string[] = "Please press 'c' to continue...\n";
    uint32_t length = strlen( string );
    sprintf( (char *) tx_buffer, string );

    /* Write on UART. */
    #ifndef PRINTF_UART
    UART_TransferSendBlocking( uart_addrs[0], tx_buffer, length );
    #endif
    printf("%s", string);
    /* Read a char. */
    get = serial_getc(&pc);

    #ifndef PRINTF_UART
    /* Free serial. */
    serial_free(&pc);
    #endif  /* PRINTF_UART */

    printf("Get char ascii = %d <-> expect char ascii = %d \n", get, expect);

    /* Check read values */
    uint8_t error = ( get != expect );

    printf("Test %s with %d error(s).\n", (error ? "failed" : "success"), error);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
