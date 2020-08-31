/* PMSIS includes */
#include "pmsis.h"

#if defined(__PULP_OS__)
#include "pulp.h"

#define FLL_SOC_MIN_FREQUENCY         ( 150000000 )
#define FLL_SOC_MAX_FREQUENCY         ( 250000000 )
#define FLL_CLUSTER_MIN_FREQUENCY     ( 87000000 )
#define FLL_CLUSTER_MAX_FREQUENCY     ( 175000000 )
#define FLL_SOC_FV_SLOPE              ( (FLL_SOC_MAX_FREQUENCY-FLL_SOC_MIN_FREQUENCY)/(DCDC_DEFAULT_NV-DCDC_DEFAULT_LV) )
#define FLL_CLUSTER_FV_SLOPE          ( (FLL_CLUSTER_MAX_FREQUENCY-FLL_CLUSTER_MIN_FREQUENCY)/(DCDC_DEFAULT_NV-DCDC_DEFAULT_LV) )

#define pi_fll_get_frequency(x)       ( rt_freq_get(x) )
#define pi_fll_set_frequency(x, y, z) ( rt_freq_set(x, y) )
#define pi_pmu_voltage_set(x, y)      ( rt_voltage_force(RT_VOLTAGE_DOMAIN_MAIN, y, NULL) )
#else
#define pi_fll_get_frequency(x)       ( pi_fll_get_frequency(x, 1) )
#endif  /* __PULP_OS__ */

#define SOC_MAX_FREQ_AT_V(x)          ( FLL_SOC_MIN_FREQUENCY + (x - DCDC_DEFAULT_LV) * FLL_SOC_FV_SLOPE )
#define CL_MAX_FREQ_AT_V(x)           ( FLL_CLUSTER_MIN_FREQUENCY + (x - DCDC_DEFAULT_LV) * FLL_CLUSTER_FV_SLOPE )

/* Variables used. */
#define FREQUENCY_DELTA  10000000

//#define CLUSTER_PRINTF
#if defined(CLUSTER_PRINTF)
#define CL_PRINTF printf
#else
#define CL_PRINTF(...) ((void) 0)
#endif  /* CLUSTER_PRINTF */

/* Task executed by cluster cores. */
void cluster_hello(void *arg)
{
    uint32_t core_id = pi_core_id();
    uint32_t cluster_id = pi_cluster_id();

    #if defined(CLUSTER_PRINTF)
    CL_PRINTF("[%d %d] Hello World !\n", cluster_id, core_id);
    #else
    for (volatile uint32_t i=0; i<1000; i++);
    #endif  /* CLUSTER_PRINTF */
}

/* Cluster main entry, executed by core 0. */
void master_entry(void *arg)
{
    CL_PRINTF("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_hello, arg);
    CL_PRINTF("Cluster master core exit\n");
}

void test_cluster_frequency(void)
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

    int32_t cur_fc_freq = pi_fll_get_frequency(FLL_SOC);
    int32_t cur_cl_freq = pi_fll_get_frequency(FLL_CLUSTER);

    printf("FC frequency : %ld\nCL frequency : %ld\n", cur_fc_freq, cur_cl_freq);

    /* Prepare cluster task. */
    struct pi_cluster_task task = {0};
    task.entry = master_entry;
    task.arg = NULL;

    for (int32_t voltage = DCDC_DEFAULT_NV; voltage >= DCDC_DEFAULT_LV; voltage -= 100)
    {
        /* Set voltage */
        if (pi_pmu_voltage_set(PI_PMU_DOMAIN_FC, voltage))
        {
            printf("Error changing voltage !\nTest failed...\n");
            pmsis_exit(-2);
        }

        /* Change frequency value for given voltage. */
        for (int32_t fc_freq = SOC_MAX_FREQ_AT_V(voltage);
             fc_freq >= FREQUENCY_DELTA;
             fc_freq -= FREQUENCY_DELTA)
        {
            cur_fc_freq = pi_fll_set_frequency(FLL_SOC, fc_freq, 1);
            if (cur_fc_freq == -1)
            {
                printf("Error changing frequency !\nTest failed...\n");
                pmsis_exit(-3);
            }
            cur_fc_freq = pi_fll_get_frequency(FLL_SOC);
            printf("SoC Frequency : %ld Hz, Voltage : %ld mv\t freq : %ld\n",
                   cur_fc_freq, voltage, fc_freq);

            /* Wait some time. */
            for (volatile uint32_t i = 0; i < 10000; i++);

            for (int32_t cl_freq = CL_MAX_FREQ_AT_V(voltage);
                 cl_freq >= FREQUENCY_DELTA;
                 cl_freq -= FREQUENCY_DELTA)
            {
                cur_cl_freq = pi_fll_set_frequency(FLL_CLUSTER, cl_freq, 1);
                if (cur_cl_freq == -1)
                {
                    printf("Error changing frequency !\nTest failed...\n");
                    pmsis_exit(-4);
                }
                cur_cl_freq = pi_fll_get_frequency(FLL_CLUSTER);
                printf("Cluster Frequency : %ld Hz, freq : %ld\n", cur_cl_freq, cl_freq);

                #if defined(ASYNC)
                pi_task_t wait_task = {0};
                pi_task_block(&wait_task);
                pi_cluster_send_task_to_cl_async(&cluster_dev, &task, &wait_task);
                pi_task_wait_on(&wait_task);
                #else
                pi_cluster_send_task_to_cl(&cluster_dev, &task);
                #endif  /* ASYNC */
            }
        }
    }

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    printf("\nCluster frequency done with %ld error(s) !\n", errors);
    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** PMSIS Cluster Frequency Test ***\n\n");
    return pmsis_kickoff((void *) test_cluster_frequency);
}
