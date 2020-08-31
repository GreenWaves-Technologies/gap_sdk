/* PMSIS includes */
#include "pmsis.h"
#include "omp.h"

#define NB_CORES (8)

static int32_t core_iterations[NB_CORES] = { 0 };
static uint32_t errors = 0;

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");

    #pragma omp parallel num_threads(NB_CORES)
    {
        printf("[%d %d] Fork entry\n", pi_cluster_id(), omp_get_thread_num() );

        #pragma omp for
        for (int i=0; i<64; i++)
        {
            int32_t core_id = omp_get_thread_num();
            if (core_id > NB_CORES)
            {
                errors++;
            }
            else
            {
                core_iterations[core_id]++;
            }
            printf("[%d %d] for entry index %d\n", pi_cluster_id(), omp_get_thread_num(), i );
        }

    }

    for (int i = 0; i < NB_CORES; i++)
    {
        if (core_iterations[i] == 0)
        {
            errors++;
            printf("Core #%d has no iteration\n", i);
        }
    }

    printf("Cluster master core exit\n");
}

void helloworld(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
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

