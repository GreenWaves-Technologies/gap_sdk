// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#define CORE_NUMBER      8

/* L2 Buffer */
#define BUFFER_SIZE      16
uint16_t l2_buffer[BUFFER_SIZE];

static void hyperbus_block_write(uint32_t addr, const void* buf, uint32_t size) {
    /* */
    fc_call_t task;
    task.id     = UDMA_EVENT_HYPERBUS_TX;
    task.arg[0] = 0;
    task.arg[1] = (uint32_t)buf;
    task.arg[2] = size;

    CLUSTER_CL2FC_SendTask(0, &task);
}

static void hyperbus_block_read(uint32_t addr, void* buf, uint32_t size) {
    /* */
    fc_call_t task;
    task.id     = UDMA_EVENT_HYPERBUS_RX;
    task.arg[0] = 0;
    task.arg[1] = (uint32_t)buf;
    task.arg[2] = size;

    CLUSTER_CL2FC_SendTask(0, &task);
}

void Hello(void *arg) {
    printf("Hello World from cluster core %d!\n", __core_ID());
}

void Master_Entry(void *arg) {

    /* Cluster call FC to write with hyperbus */
    hyperbus_block_write(0, (const void*)l2_buffer, BUFFER_SIZE);

    /* Cluster call FC to read with hyperbus */
    hyperbus_block_read(0, (void*)l2_buffer, BUFFER_SIZE);

    CLUSTER_CoresFork(Hello, arg);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Cluster Power On test\n");

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
