// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE CLUSTER_CORES_NUM * 100

int a[BUFFER_SIZE];

void Hello(void *arg) {
    if (__core_ID()) {
        a[(int)(arg + __core_ID())] *= 3;
    }
    else {
        CLUSTER_SCBC->ICACHE_ENABLE_PREFETCH = 0x1FF;
        for (volatile int i = 0 ; i < (rand() & 0xF); i++);
        a[(int)(arg + __core_ID())] *= 3;
        CLUSTER_SCBC->ICACHE_ENABLE_PREFETCH = 0x0;
        for (volatile int i = 0 ; i < (rand() & 0xF); i++);
    }
}

void Master_Entry(void *arg) {
    for (int i = 0; i < BUFFER_SIZE; i += CLUSTER_CORES_NUM)
        CLUSTER_CoresFork(Hello, (void*)i);
}

int main()
{
    printf("mbed_os Cluster Icache controller test\n");

    // Use current time as seed for random generator
    srand(time(0));

    for (int i = 0; i < BUFFER_SIZE; i++)
        a[i] = i;

    /* Cluster Start - Power on, 9th core seperation disable */
    CLUSTER_Start(0, CLUSTER_CORES_NUM, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (a[i] != 3*i) error++;
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    #ifdef JENKINS_TEST_FLAG
    exit(error);
    #else
    return error;
    #endif
}
