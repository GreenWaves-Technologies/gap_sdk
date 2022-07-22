/* HARDWARE SET UP REQUIRED :
RST pin must be connected
as well as RX/TX and 3V3 pins
and flow control ones (CTS/RTS)

For using uart WITHOUT flow control:
CTS pin must be connected to the GND one !
*/

#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "pmsis.h"

#include "bsp/gap9_evk.h"
#include "bsp/bsp.h"
#include "bsp/buffer.h"
#include "bleNotifier.h"
#include "setup.h"


#define CLUSTER_STACK_SIZE 2*1024
char person_name[200];


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

#ifdef DEVICE_CONNECTED
    // This requires having a device to connect with Nina.

    pi_ble_ioctl(&ble, PI_NINA_B312_WAIT_FOR_EVENT, (void *) rx_buffer);
    PRINTF("Received Event after reboot: %s\n", rx_buffer);
#endif

    closeHandler();
    pmsis_exit(0);
}

int main()
{
    PRINTF("Start BLE Demo\n");
    return pmsis_kickoff((void *) body);
}
