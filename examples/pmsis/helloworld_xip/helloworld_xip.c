/* PMSIS includes */
#include "pmsis.h"
#include "pmsis/implem/drivers/xip/xip_internal.h"

/* Variables used. */
struct pi_device uart;

volatile uint32_t g_guinea_pig[4];

/* Task executed by cluster cores. */
void cluster_helloworld(void *arg)
{
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);
}

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_helloworld, arg);
    printf("Cluster master core exit\n");
}

__attribute__((section(".guinea_pig_l2_section"),noinline)) void guinea_pig_l2(void)
{
    g_guinea_pig[0] = 1;
}

__attribute__((section(".guinea_pig_xip_section"),noinline)) void guinea_pig_xip(void)
{
    g_guinea_pig[1] = 1;
}

__attribute__((section(".guinea_pig_xip_long_section"),noinline)) void guinea_pig_xip_long(void)
{
    printf("%s\n",__func__);
    g_guinea_pig[2] = 1;
}

void helloworld(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

#ifdef TEST_CLUSTER
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
#endif
    printf("Going to try aliasing\n");
    
    guinea_pig_l2();
    if(g_guinea_pig[0] !=1)
    {
        printf("l2 access error\n");
        errors++;
    }
    guinea_pig_xip();
    if(g_guinea_pig[1] !=1)
    {
        printf("xip access error\n");
        errors++;
    }
    guinea_pig_xip_long();
    if(g_guinea_pig[2] !=1)
    {
        printf("xip + 2MB access error\n");
        errors++;
    }

    printf("guinea_pig[0] = %x guinea_pig[1] = %x guinea_pig[2] = %x\n", g_guinea_pig[0],
            g_guinea_pig[1], g_guinea_pig[2]);

    if(errors)
    {
        printf("Test failed with %x errors\n",errors);
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

