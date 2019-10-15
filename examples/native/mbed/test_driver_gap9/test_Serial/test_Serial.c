// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "serial_api.h"
// Ticker
#include "mbed_wait_api.h"

int test_uart(int uart_id, PinName TX, PinName RX)
{
    serial_t pc;
    int _baud = 9600;
    char get = 0;
    char expect = 'c';

    /* SPI pins init, SPI udma channel init */
    serial_init(&pc, TX, RX);

    printf("USB%d_TX  = %d \n", uart_id, TX);
    printf("USB%d_RX  = %d \n", uart_id, RX);

    /* Serial format 8N1 configuration */
    serial_format(&pc, 8, 0, 1);

    /* Serial baudrate 115200 configuration */
    serial_baud(&pc, _baud);

    printf("Please press 'c' to continue...\n");

    /* Ring back test */
    serial_putc(&pc, expect);
    get = serial_getc(&pc);

    /* Serial set break */
    serial_break_set(&pc);
    wait_us(18000000 / _baud);
    /* Serial break clear */
    serial_break_clear(&pc);

    /* Serial stop */
    serial_free(&pc);

    printf("Get char ascii = %d <-> expect char ascii = %d \n", get, expect);

    /* Check read values */
    int error = 0;
    if (get != expect) error = 1;

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os Serial driver test\n");

    int error = 0;
    error = test_uart(0, UART0_TX, UART0_RX);
    if (error) {
        printf("Test failed %d errors\n", error);
        return error;
    }

    error = test_uart(1, UART1_TX, UART1_RX);
    if (error) {
        printf("Test failed %d errors\n", error);
        return error;
    }

    error = test_uart(2, UART2_TX, UART2_RX);

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
