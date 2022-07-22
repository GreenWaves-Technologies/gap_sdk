/* PMSIS includes */
#include "pmsis.h"
#include "bsp/bsp.h"

/* Variables used. */
#define NO_PRINTF

#if defined(NO_PRINTF)
#define PRINTF(...)      ( (void) 0 )
#else
#define PRINTF           ( printf )
#endif  /* NO_PRINTF */

#define SW_RESET_VAL     ( 0x55660011 )

PI_L2 static uint32_t flash_val = (uint32_t) SW_RESET_VAL;
PI_L2 static uint32_t reset_val = 0;

/* Task executed by cluster cores. */
void cluster_helloworld(void *arg)
{
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);
}

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    /* Task dispatch to cluster cores. */
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_helloworld, arg);
}

void test_sw_reset(void)
{
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
        PRINTF("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task;

    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, NULL));

    pi_cluster_close(&cluster_dev);

    struct pi_device flash;
    struct pi_hyperflash_conf conf = {0};
    struct pi_flash_info flash_info;
    uint32_t hyper_buff;
    /* Init & open flash. */
    pi_hyperflash_conf_init(&conf);
    pi_open_from_conf(&flash, &conf);
    if (pi_flash_open(&flash))
    {
        PRINTF("Error flash open !\n");
        pmsis_exit(-2);
    }

    /* Retrieve info on flash. */
    pi_flash_ioctl(&flash, PI_FLASH_IOCTL_INFO, (void *) &flash_info);
    hyper_buff = ((flash_info.flash_start + flash_info.sector_size - 1) &
                  ~(flash_info.sector_size - 1)) + 128;

    pi_flash_read(&flash, hyper_buff, &reset_val, sizeof(uint32_t));
    PRINTF("Reset val=%lx, write in flash val=%lx\n", reset_val, flash_val);
    if (reset_val != flash_val)
    {
        pi_flash_erase(&flash, hyper_buff, sizeof(uint32_t));
        pi_flash_program(&flash, hyper_buff, &flash_val, sizeof(uint32_t));
        PRINTF("SW Reset\n");
        pi_os_reboot();
    }

    if (reset_val == flash_val)
    {
        PRINTF("Test success !\n");
        printf("TOK\n");
    }
    else
    {
        PRINTF("Test failed !\n");
        printf("TKO\n");
    }
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    return pmsis_kickoff((void *) test_sw_reset);
}

