/* PMSIS includes */
#include "pmsis.h"
#include "omp.h"


#define NUM_THREADS (6)

static int a[NUM_THREADS] = {0};
static uint32_t errors = 0;

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster delegate\n");

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp single
        {
            a[omp_get_thread_num()] = 1;
        }
    }

    int count = 0;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (a[i] == 1)
        {
            count++;
        }
    }
    if (count != 1)
    {
        printf("Incorrect number: %d, expected 1\n", count);
        errors++;
    }
    printf("Cluster master core exit\n");
}

void helloworld(void)
{
    printf("Entering main controller\n");
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

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
        printf("Test failed with %d errors\n", errors);
    }
    else
    {
        printf("Test success !\n");
    }

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    return pmsis_kickoff((void *) helloworld);
}

