// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"

#ifdef ICACHE_DEBUG
#include "gap_icache_utils.h"
#endif

#include <stdlib.h>

#define BUFFER_SIZE      2048
#define CORE_NUMBER      8

static char L2_IN[BUFFER_SIZE];
static char L2_OUT[BUFFER_SIZE];

static void parallel_operation(char * L1_BUFFER)
{
    int start = __core_ID() * BUFFER_SIZE / 8;
    int end = start - 1 + BUFFER_SIZE / 8;

    printf("Core %d Task %d - %d\n", __core_ID(), start, end);

    for (int i = start; i <= end; i++)
    {
        L1_BUFFER[i] =  (char)(L1_BUFFER[i] * 3);
    }
}

static void cluster_dma_test(void *arg)
{
    char *L1_BUFFER = (char *)arg;

    if(!__core_ID()) {
        // Retrieve the pointer to the allocated memory
        printf("Core %d Entered cluster\n", __core_ID());

        dma_req_t req;
        // copy from L2 to shared L1
        DMAMCHAN_Memcpy_1D((uint32_t)L2_IN, (uint32_t)L1_BUFFER, BUFFER_SIZE, GAP_DMA_L22TCDM, &req);
        DMAMCHAN_WaitRequestEnd(&req);
    }

    // Barrier
    CLUSTER_SynchBarrier();

    // All cores do calculation
    parallel_operation(L1_BUFFER);

    // Barrier
    CLUSTER_SynchBarrier();

    if(!__core_ID()) {
        dma_req_t req;
        // Copy from shared L1 to L2
        DMAMCHAN_Memcpy_1D((uint32_t)L2_OUT, (uint32_t)L1_BUFFER, BUFFER_SIZE, GAP_DMA_TCDM2L2, &req);
        DMAMCHAN_WaitRequestEnd(&req);
    }
}

void Master_Entry(char* L1_BUFFER) {
    CLUSTER_CoresFork(cluster_dma_test, (void *)L1_BUFFER);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Cluster DMA test\n");

    // Initialize the buffer in L2 memory
    for (int i = 0; i < BUFFER_SIZE; i++)
        L2_IN[i] = i;

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    // Allocate a buffer in the shared L1 memory
    char *L1_BUFFER = L1_Malloc(BUFFER_SIZE);

    #ifdef ICACHE_DEBUG
    for (int iter = 0; iter < 3; iter++) {
        /* ICACHE Start counters*/
        ICACHE_Start(iter);

        /* FC send a task to Cluster */
        CLUSTER_SendTask(0, Master_Entry, (void *)L1_BUFFER, 0);

        CLUSTER_Wait(0);

        /* ICACHE Stop counters */
        ICACHE_Stop(iter);
    }
    #else
    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, (void *)L1_BUFFER, 0);
    #endif

    CLUSTER_Wait(0);

    /* Free before cluster is power off */
    L1_MallocFree(L1_BUFFER);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (L2_OUT[i] != (char)(i * 3))
            error++;
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
