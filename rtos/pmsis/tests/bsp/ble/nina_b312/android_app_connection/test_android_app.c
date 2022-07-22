/* HARDWARE SET UP REQUIRED :
RST pin must be connected
as well as RX/TX and 3V3 pins
and flow control ones (CTS/RTS)

For using uart WITHOUT flow control:
CTS pin must be connected to the GND one !

/!\ Require a device to connect /!\
Install CrazyFly app on it,
and use TEST fragment to test transmissions
*/

#include <stdio.h>

#include "pmsis.h"
#include "bsp/gap9_evk.h"
#include "bsp/bsp.h"
#include "bsp/buffer.h"
#include "tx_rx_with_android.h"
#include "setup.h"

#define PRINTF printf


void body(void* parameters)
{
    int* output_map;
    char string_buffer[64];

    PRINTF("Start Ble Notifier Example\n");

    int error;
    error = initHandler();

    if (error)
    {
        PRINTF("Test failed!\n");
        pmsis_exit(-5);
    }
    else
    {
        PRINTF("Test successful.\n");
    }

    closeHandler();
    pmsis_exit(0);
}

int main()
{
    PRINTF("Start BLE Demo\n");
    return pmsis_kickoff((void *) body);
}
