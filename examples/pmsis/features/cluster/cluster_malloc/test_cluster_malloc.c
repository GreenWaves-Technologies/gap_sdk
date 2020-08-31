/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

/* Task executed by cluster cores. */
/* Cluster main entry, executed by core 0. */
void cluster_malloc(void *arg)
{
    uint32_t coreid = pi_core_id(), clusterid = pi_cluster_id();
    uint32_t *ptr = NULL;

    /* Alloc memory in L1. */
    /* Cluster core allocates in L1 memory. */
    for (uint32_t i=0; i<1000; i++)
    {
        ptr = (uint32_t *) pmsis_l1_malloc((uint32_t) BUFFER_SIZE);
        if (ptr == NULL)
        {
            break;
        }
        printf("[%d, %d] L1_Malloc : %p\n", clusterid, coreid, ptr);
        if ((i % 2) == 0)
        {
            printf("[%d, %d] L1_Malloc free : %p\n", clusterid, coreid, ptr);
            pmsis_l1_malloc_free(ptr, (uint32_t) BUFFER_SIZE);
        }
    }

    /* Alloc memory in L2. */
    /*
      Cluster core requests memory allocation in L2, and wait response for request.
      The request is delegated to FC.
    */
    pi_cl_alloc_req_t alloc_req = {0};
    pi_cl_free_req_t free_req = {0};
    for (uint32_t i=0; i<1000; i++)
    {
        pi_cl_l2_malloc((uint32_t) BUFFER_SIZE, &alloc_req);
        ptr = (uint32_t *) pi_cl_l2_malloc_wait(&alloc_req);
        if (ptr == NULL)
        {
            break;
        }
        printf("[%d, %d] L2_Malloc : %p\n", clusterid, coreid, ptr);
        if ((i % 2) == 0)
        {
            printf("[%d, %d] L2_Malloc free : %p\n", clusterid, coreid, ptr);
            pi_cl_l2_free(ptr, (uint32_t) BUFFER_SIZE, &free_req);
            pi_cl_l2_free_wait(&free_req);
        }
    }
}

void test_cluster_malloc(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task task = {0};
    task.entry = cluster_malloc;
    task.arg = NULL;

    printf("Sending task.\n");
    #if defined(ASYNC)
    pi_task_t wait_task = {0};
    pi_task_block(&wait_task);
    pi_cluster_send_task_to_cl_async(&cluster_dev, &task, &wait_task);
    printf("Wait end of cluster task\n");
    pi_task_wait_on(&wait_task);
    printf("End of cluster task\n");
    #else
    pi_cluster_send_task_to_cl(&cluster_dev, &task);
    #endif  /* ASYNC */

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    /* Verification. */

    printf("\nCluster malloc done with %d error(s) !\n", errors);
    printf("Test %s with %d error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Cluster DMA Test ***\n\n");
    return pmsis_kickoff((void *) test_cluster_malloc);
}
