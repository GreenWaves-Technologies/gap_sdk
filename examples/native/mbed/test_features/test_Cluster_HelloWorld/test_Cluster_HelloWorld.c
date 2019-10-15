// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

void Hello(void *arg) {
    printf("Hello World from cluster core %d!\n", __core_ID());
}

void Master_Entry(void *arg) {
    CLUSTER_CoresFork(Hello, arg);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Cluster Power On test\n");

    /* Cluster Start - Power on, 9th core seperation disable */
    CLUSTER_Start(0, CLUSTER_CORES_NUM, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    #ifdef JENKINS_TEST_FLAG
    exit(error);
    #else
    return error;
    #endif
}
