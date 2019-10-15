/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__Wait.html
 */
#include <stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

int main()
{
    if (__is_FC()) {

        printf("Fabric controller code execution for mbed_os delay test\n");

        // Test for osDelay()
        int time = 5;
        printf("Main thread osDelay() waiting start ...\n");

        while (time) {
            printf("Waiting for %dms...\n", time);
            osDelay(time--);
        }

        printf("Main thread osDelay() waiting is end\n\n");


        // Test for osDelayUntil()
        uint32_t tick = osKernelGetTickCount();     // retrieve the number of system ticks
        printf("Main thread osDelayUntil(current_time + 100 ms) waiting start, tick = %d \n", tick);

        tick += 100;                                 // delay 10 ticks periodically
        osDelayUntil(tick);

        printf("Main thread osDelayUntil() waiting is end, tick = %d \n", tick);

        if (time)
        {
            printf("Test failed\n");
            #ifdef JENKINS_TEST_FLAG
            exit(-1);
            #else
            return -1;
            #endif
        }
        else
        {
            printf("Test success\n");
            #ifdef JENKINS_TEST_FLAG
            exit(0);
            #else
            return 0;
            #endif
        }
    }
}
