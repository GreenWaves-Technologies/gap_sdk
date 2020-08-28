/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
struct pi_device uart;
char hello[20];
PI_L1 spinlock_t spinlock;
PI_L1 int32_t tas_addr;

/* Task executed by cluster cores. */
void cluster_uart_helloworld(void *arg)
{
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    cl_sync_spinlock_take(&spinlock);
    sprintf(hello, "[%d %d] Hello World!\n", cluster_id, core_id);
    /* Send a request to FC to write through uart and wait for termination of req. */
    pi_cl_uart_req_t req;
    pi_cl_uart_write(&uart, hello, strlen(hello), &req);
    pi_cl_uart_write_wait(&req);
    cl_sync_spinlock_release(&spinlock);
}

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    cl_sync_init_spinlock(&spinlock, &tas_addr);
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_uart_helloworld, arg);
    printf("Cluster master core exit\n");
}

void test_uart_delegate(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    struct pi_uart_conf uart_conf;

    /* Init & open uart. */
    pi_uart_conf_init(&uart_conf);
    uart_conf.enable_tx = 1;
    uart_conf.enable_rx = 0;
    uart_conf.baudrate_bps = 115200;
    pi_open_from_conf(&uart, &uart_conf);
    if (pi_uart_open(&uart))
    {
        printf("Uart open failed !\n");
        pmsis_exit(-1);
    }

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    sprintf(hello, "[%d %d] Hello World!\n", cluster_id, core_id);
    pi_uart_write(&uart, hello, strlen(hello));

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

    pi_uart_close(&uart);

    printf("Test success !\n");

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Cluster Uart Test ***\n\n");
    return pmsis_kickoff((void *) test_uart_delegate);
}

