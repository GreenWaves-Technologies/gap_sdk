#include <stdlib.h>
// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"

#define CORE_NUMBER      8

void HelloWorld(void *arg)
{
    uint32_t coreid = 0, args = (uint32_t) arg;
    uint8_t *p = NULL;

    /* Allocate in FC. */
    for(uint32_t i=0; i<1000; i++)
    {
        p = (uint8_t *) L1_Malloc(2048);
        if(p == NULL)
            break;
        printf("Core %d L1_Malloc : %p\n", __core_ID(), p);

        if(p != NULL)
            L1_MallocFree(p);
    }

    /* Allocate in L2. */
    for(uint32_t i=0; i<1000; i++)
    {
        p = (uint8_t *) malloc(2048*8);
        if(p == NULL)
            break;
        printf("Core %d L2_Malloc from Cluster : %p\n", __core_ID(), p);
    }
    if(p != NULL)
        free(p);
}

void Master_Entry(void *arg)
{
    L1_MallocInit();
    CLUSTER_CoresFork(HelloWorld, arg);
}

int main()
{
    printf("Fabric controller code execution for mbed_os malloc test\n");

    uint8_t *p = NULL;
    FC_MallocInit();

    /* Allocate in FC. */
    for(uint32_t i=0; i<1000; i++)
    {
        p = (uint8_t *) FC_Malloc(2048);
        if(p == NULL)
            break;
        printf("FC_Malloc : %p\n", p);

        if(p != NULL)
            FC_MallocFree(p);
    }

    /* Power On Computing Cluster. */
    CLUSTER_Start(0, 1, 0);

    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Wait for tasks to end and shut down Computing Cluster. */
    CLUSTER_Wait(0);
    CLUSTER_Stop(0);

    return 0;
}
