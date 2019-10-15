// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#define CORE_NUMBER      8
#define FREQUENCY_DELTA  5000000

void Hello(void *arg) {
    for (volatile int i = 0; i < 10000; i++);
}

void Master_Entry(void *arg) {
    CLUSTER_CoresFork(Hello, arg);
}

int check_cluster_frequency() {

    /* Frequency descend from MAX to MIN*/
    for (int voltage = DCDC_DEFAULT_NV; voltage >= DCDC_DEFAULT_LV; voltage -= 100) {

        /* Set voltage */
        if (PMU_SetVoltage(voltage, 1)) {
            printf("Error of changing voltage, check Frequency value!\n");
            printf("Test failed\n");
            return -1;
        }

        /* Frequency descend from MAX to MIN*/
        for ( int frequency = FLL_ClusterMaxFreqAtV(voltage); frequency >= FREQUENCY_DELTA; frequency -= FREQUENCY_DELTA) {

            if (FLL_SetFrequency(uFLL_CLUSTER, frequency, 1) == -1) {
                printf("Error of changing freqency, check Voltage value!\n");
                printf("Test failed\n");
                return -1;
            }

            for(int j = 0; j < 5; j++) {
                /* FC send a task to Cluster */
                CLUSTER_SendTask(0, Master_Entry, 0, 0);
            }
            CLUSTER_Wait(0);
            printf("Cluster Frequency = %d : %d Hz , Voltage = %d mv\n", frequency, FLL_GetFrequency(uFLL_CLUSTER), voltage);
        }
    }

    return 0;
}

int check_fc_frequency() {

    /* Frequency descend from MAX to MIN*/
    for (int voltage = DCDC_DEFAULT_NV; voltage >= DCDC_DEFAULT_LV; voltage -= 100) {

        /* Set voltage */
        if (PMU_SetVoltage(voltage, 1)) {
            printf("Error of changing voltage, check Frequency value!\n");
            printf("Test failed\n");
            return -1;
        }

        /* Frequency descend from MAX to MIN*/
        for ( int frequency = FLL_SoCMaxFreqAtV(voltage); frequency >= FREQUENCY_DELTA; frequency -= FREQUENCY_DELTA) {

            if (FLL_SetFrequency(uFLL_SOC, frequency, 1) == -1) {
                printf("Error of changing freqency, check Voltage value!\n");
                printf("Test failed\n");
                return -1;
            }

            printf("FC Frequency      = %d Hz , Voltage = %d mv\n", FLL_GetFrequency(uFLL_SOC), voltage);

            for (volatile int j = 0; j < 100000; j++);
        }
    }

    return 0;
}


int main()
{
    printf("Fabric controller code execution for mbed_os Cluster and FC Frequency Change test\n");

    printf("Default FC Frequency      = %d Hz\n", FLL_GetFrequency(uFLL_SOC));

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    printf("Default Cluster Frequency = %d Hz\n", FLL_GetFrequency(uFLL_CLUSTER));

    check_cluster_frequency();

    check_fc_frequency();

    /* cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
