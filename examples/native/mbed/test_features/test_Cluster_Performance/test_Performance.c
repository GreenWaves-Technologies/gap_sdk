#include <stdio.h>
#include <stdlib.h>

#include "gap_common.h"
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "cmsis.h"
#include "gap_performance.h"

#define CORE_NUMBER   8

void Hello() {
    printf("Hello World from cluster core %d!\n", (int)__core_ID());
}

void Master_Entry(void* arg){

    performance_t perf;

    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK);
    CLUSTER_CoresFork((void *) Hello, NULL);
    PERFORMANCE_Stop(&perf);

    printf("Total CYCLES (include core sleep cycles): %d\n" , (int)PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT));

}

int main()
{

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    printf("Test success\n");

    return 0;
}
