/* PMSIS includes */
#include "pmsis.h"
#include "omp.h"

#define ARRAY_SIZE 512

uint32_t a[ARRAY_SIZE] = {0};
uint32_t b[ARRAY_SIZE] = {0};
uint32_t c[ARRAY_SIZE] = {0};

uint32_t errors = 0;

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");

    int32_t counter = 0;
    #pragma omp parallel
    {
        printf("[%d %d] Fork entry\n", pi_cluster_id(), omp_get_thread_num() );
        #pragma omp barrier
        {
            #pragma omp atomic
            counter ++;
        }
    }
    printf("Core counter: %d\n", counter);

    if (counter != omp_get_num_threads())
    {
        errors = 1;
    }

    printf("Cluster master core exit\n");
}

void helloworld(void)
{
    printf("Entering main controller\n");
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    struct pi_device cluster_dev = {0};
    struct pi_cluster_conf cl_conf = {0};

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task = {0};
    cl_task.entry = cluster_delegate;
    cl_task.arg = NULL;

    pi_cluster_send_task_to_cl(&cluster_dev, &cl_task);

    pi_cluster_close(&cluster_dev);

    if (errors)
    {
        printf("Test failed!\n");
    }
    else
    {
        printf("Test success!\n");
    }

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    return pmsis_kickoff((void *) helloworld);
}

