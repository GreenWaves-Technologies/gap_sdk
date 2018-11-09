#include <math.h>
#include <stdlib.h>
// MBED OS
#include "cmsis_os2.h"
// GAP Driver API
#include "gap_common.h"
#include "mbed_wait_api.h"

#define FREQUENCY_DELTA  5000000

int main()
{
    printf("Fabric controller code execution for mbed_os FC frequency test\n");

    for (int voltage = DCDC_DEFAULT_NV; voltage >= DCDC_DEFAULT_LV; voltage -= 100) {

        /* Set voltage */
        if (PMU_SetVoltage(voltage, 1)) {
            printf("Error of changing voltage, check Frequency value!\n");
            printf("Test failed\n");
            return -1;
        }

        for ( int frequency = FLL_SoCMaxFreqAtV(voltage); frequency >= FREQUENCY_DELTA; frequency -= FREQUENCY_DELTA) {

            if (FLL_SetFrequency(uFLL_SOC, frequency, 1) == -1) {
                printf("Error of changing freqency, check Voltage value!\n");
                printf("Test failed\n");
                return -1;
            } else {
                /* Update systick freqency when PMU change freqency */
                osKernelUpdateSysTimerFreq();
            }

            wait(1);
            printf("Frequency = %d , Voltage = %d mv\n", FLL_GetFrequency(uFLL_SOC), voltage);
        }
    }

    printf("Test success\n");

    return 0;
}
