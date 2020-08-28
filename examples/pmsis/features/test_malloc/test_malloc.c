/* PMSIS includes */
#include "pmsis.h"

/* Demo utlities includes. */
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
static uint32_t hyper_buff;
static struct pi_device ram;
static struct pi_hyperram_conf conf;

void test_malloc_cluster(void *arg)
{
    uint32_t coreid = pi_core_id(), clusterid = pi_cluster_id(), args = (uint32_t) arg;
    uint32_t *p = NULL;

    /* Allocate in FC. */
    for (uint32_t i=0; i<1000; i++)
    {
        p = (uint32_t *) pmsis_l1_malloc(2048);
        if (p == NULL)
        {
            break;
        }
        printf("[%ld, %ld] L1_Malloc : %p\n", clusterid, coreid, p);
    }

    /* Allocate in L2. */
    for (uint32_t i=0; i<1000; i++)
    {
        p = (uint32_t *) pmsis_l2_malloc(2048*8);
        if (p == NULL)
        {
            break;
        }
        printf("[%ld, %ld] L2_Malloc from Cluster : %p\n", clusterid, coreid, p);
    }

    /* Allocate in Ram. */
    pi_cl_ram_alloc_req_t alloc_req;
    for (uint32_t i=0; i<1000; i++)
    {
        pi_cl_ram_alloc(&ram, 2048*20, &alloc_req);
        pi_cl_ram_alloc_wait(&alloc_req, p);
        if ((*p == 0) || alloc_req.error)
        {
            break;
        }
        printf("[%ld, %ld] Hyperram_Malloc : %lx\n", clusterid, coreid, *p);
    }
}

void master_entry(void *arg)
{
    pi_cl_team_fork(1, test_malloc_cluster, arg);
}

void test_malloc(void)
{
    printf("Entering main controller\n");
    uint32_t coreid = pi_core_id(), clusterid = pi_cluster_id();
    uint32_t *p = NULL;

    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-1);
    }

    #if defined(PMSIS_DRIVERS)
    /* Allocate in FC. */
    for (uint32_t i=0; i<1000; i++)
    {
        p = (uint32_t *) pmsis_fc_tcdm_malloc(2048);
        if (p == NULL)
        {
            break;
        }
        printf("[%d, %d] FC_Malloc : %p\n", clusterid, coreid, p);
    }
    #endif  /* PMSIS_DRIVERS */

    struct pi_device *cluster_dev = (struct pi_device *) pmsis_l2_malloc(sizeof(struct pi_device));
    if (cluster_dev == NULL)
    {
        printf("Cluster dev alloc failed !\n");
        pmsis_exit(-2);
    }

    struct pi_cluster_conf *conf = (struct pi_cluster_conf *) pmsis_l2_malloc(sizeof(struct pi_cluster_conf));
    if (cluster_dev == NULL)
    {
        printf("Cluster conf alloc failed !\n");
        pmsis_l2_malloc_free(cluster_dev, sizeof(struct pi_device));
        pmsis_exit(-3);
    }
    pi_cluster_conf_init(conf);
    conf->id = 0;
    pi_open_from_conf(cluster_dev, conf);
    if (pi_cluster_open(cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_l2_malloc_free(cluster_dev, sizeof(struct pi_device));
        pmsis_l2_malloc_free(conf, sizeof(struct pi_cluster_conf));
        pmsis_exit(-4);
    }

    struct pi_cluster_task *task = (struct pi_cluster_task *) pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    if (task == NULL)
    {
        printf("Cluster task alloc failed !\n");
        pmsis_l2_malloc_free(cluster_dev, sizeof(struct pi_device));
        pmsis_l2_malloc_free(conf, sizeof(struct pi_cluster_conf));
        pmsis_exit(-5);
    }
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = master_entry;
    task->arg = NULL;

    /* Sending task to cluster. */
    pi_cluster_send_task_to_cl(cluster_dev, task);
    pmsis_l2_malloc_free(task, sizeof(struct pi_cluster_task));
    /* shut down cluster after task completion. */
    pi_cluster_close(cluster_dev);
    pmsis_l2_malloc_free(conf, sizeof(struct pi_cluster_conf));
    pmsis_l2_malloc_free(cluster_dev, sizeof(struct pi_device));

    pi_ram_close(&ram);

    printf("Test success !\n");
    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Malloc Test ***\n\n");
    return pmsis_kickoff((void *) test_malloc);
}
