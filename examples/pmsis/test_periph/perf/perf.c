/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
struct pi_device uart;
char hello[20];
PI_L1 spinlock_t spinlock;
PI_L1 int32_t tas_addr;
PI_L2 uint32_t perf_values[ARCHI_CLUSTER_NB_PE] = {0};

/* Task executed by cluster cores. */
void cluster_helloworld(void *arg)
{
    pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
    pi_perf_start();

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    cl_sync_spinlock_take(&spinlock);
    sprintf(hello, "[%d %d] Hello World!\n", cluster_id, core_id);
    printf(hello);
    cl_sync_spinlock_release(&spinlock);

    pi_perf_stop();
    perf_values[core_id] = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
}

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    cl_sync_init_spinlock(&spinlock, &tas_addr);
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_helloworld, arg);
    printf("Cluster master core exit\n");
}

void helloworld(void)
{
    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_ACTIVE_CYCLES);
    pi_perf_start();
    printf("Entering main controller\n");
    uint32_t errors = 0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    sprintf(hello, "[%d %d] Hello World!\n", cluster_id, core_id);
    printf(hello);

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
        pmsis_exit(-2);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task = {0};
    cl_task.entry = cluster_delegate;
    cl_task.arg = NULL;

    pi_cluster_send_task_to_cl(&cluster_dev, &cl_task);

    pi_cluster_close(&cluster_dev);

    pi_perf_stop();
    uint32_t cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    uint32_t tim_cycles = pi_perf_read(PI_PERF_CYCLES);
    printf("Perf : %d cycles Timer : %d cycles\n", cycles, tim_cycles);
    for (uint32_t i = 0; i < (uint32_t) ARCHI_CLUSTER_NB_PE; i++)
    {
        printf("[%d %d] Perf : %d cycles\n", 0, i, perf_values[i]);
    }

    printf("Test success !\n");

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    return pmsis_kickoff((void *) helloworld);
}

