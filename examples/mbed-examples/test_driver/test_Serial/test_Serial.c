// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "serial_api.h"
// Ticker
#include "mbed_wait_api.h"


int main()
{
    printf("Fabric controller code execution for mbed_os Serial driver test\n");

    serial_t pc;
    int _baud = 115200;
    char get = 0;
    char expect = 'c';

    /* SPI pins init, SPI udma channel init */
    serial_init(&pc, USBTX, USBRX);

    printf("USBTX  = %d \n", USBTX);
    printf("USBRX  = %d \n", USBRX);

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

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
