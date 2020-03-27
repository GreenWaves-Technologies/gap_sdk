/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BUFFER_SIZE      ( 256 * ARCHI_CLUSTER_NB_PE )

static uint8_t l2_in[BUFFER_SIZE], l2_out[BUFFER_SIZE];

/* Task executed by cluster cores. */
void cluster_dma(void *arg)
{
    uint8_t *l1_buffer = (uint8_t *) arg;
    uint32_t coreid = pi_core_id(), start = 0, end = 0;

    /* Core 0 of cluster initiates DMA transfer from L2 to L1. */
    if (!coreid)
    {
        printf("Core %d requesting DMA transfer from l2_in to l1_buffer.\n", coreid);
        pi_cl_dma_copy_t copy;
        copy.dir = PI_CL_DMA_DIR_EXT2LOC;
        copy.merge = 0;
        copy.size = (uint16_t) BUFFER_SIZE;
        copy.id = 0;
        copy.ext = (uint32_t) l2_in;
        copy.loc = (uint32_t) l1_buffer;

        pi_cl_dma_memcpy(&copy);
        pi_cl_dma_wait(&copy);
        printf("Core %d : Transfer done.\n", coreid);
    }

    start = (coreid * ((uint32_t) BUFFER_SIZE / pi_cl_cluster_nb_cores()));
    end = (start - 1 + ((uint32_t) BUFFER_SIZE / pi_cl_cluster_nb_cores()));

    /* Barrier synchronisation before starting to compute. */
    pi_cl_team_barrier(0);
    /* Each core computes on specific portion of buffer. */
    for (uint32_t i=start; i<=end; i++)
    {
        l1_buffer[i] = (l1_buffer[i] * 3);
    }

    /* Barrier synchronisation to wait all cores. */
    pi_cl_team_barrier(0);

    /* Core 0 of cluster initiates DMA transfer from L1 to L2. */
    if (!coreid)
    {
        printf("Core %d requesting DMA transfer from l1_buffer to l2_out.\n", coreid);
        pi_cl_dma_copy_t copy;
        copy.dir = PI_CL_DMA_DIR_LOC2EXT;
        copy.merge = 0;
        copy.size = (uint16_t) BUFFER_SIZE;
        copy.id = 0;
        copy.ext = (uint32_t)l2_out;
        copy.loc = (uint32_t)l1_buffer;

        pi_cl_dma_memcpy(&copy);
        pi_cl_dma_wait(&copy);
        printf("Core %d : Transfer done.\n", coreid);
    }
}

/* Cluster main entry, executed by core 0. */
void master_entry(void *arg)
{
    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_dma, arg);
    printf("Cluster master core exit\n");
}

void test_cluster_dma(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;

    /* L2 Array Init. */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        l2_in[i] = i;
        l2_out[i] = 0;
    }

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

    uint8_t *l1_buffer = (uint8_t *) pmsis_l1_malloc((uint32_t) BUFFER_SIZE);
    if (l1_buffer == NULL)
    {
        printf("l1_buffer alloc failed !\n");
        pmsis_exit(-2);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task *task = (struct pi_cluster_task *) pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    if (task == NULL)
    {
        printf("Cluster task alloc failed !\n");
        pmsis_l1_malloc_free(l1_buffer, (uint32_t) BUFFER_SIZE);
        pmsis_exit(-3);
    }
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = master_entry;
    task->arg = l1_buffer;

    printf("Sending task.\n");
    #if defined(ASYNC)
    pi_task_t wait_task = {0};
    pi_task_block(&wait_task);
    pi_cluster_send_task_to_cl_async(&cluster_dev, task, &wait_task);
    printf("Wait end of cluster task\n");
    pi_task_wait_on(&wait_task);
    printf("End of cluster task\n");
    #else
    pi_cluster_send_task_to_cl(&cluster_dev, task);
    #endif  /* ASYNC */
    pmsis_l2_malloc_free(task, sizeof(struct pi_cluster_task));
    pmsis_l1_malloc_free(l1_buffer, (uint32_t) BUFFER_SIZE);

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    /* Verification. */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (l2_out[i] != (uint8_t) (l2_in[i] * 3))
        {
            errors++;
            printf("%d) %x-%x ", i, l2_out[i], (uint8_t) (l2_in[i] * 3));
        }
    }

    printf("\nCluster callback done with %d error(s) !\n", errors);
    printf("Test %s with %d error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Cluster DMA Test ***\n\n");
    return pmsis_kickoff((void *) test_cluster_dma);
}
