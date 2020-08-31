#include "omp.h"
#include "pmsis.h"

#define LOOP_ITER (2048)
#define NB_ITER (256)
#define NB_BARRIER_ITER (256)
#define NB_ITER_SINGLE (128)

#define CORE_ID pi_core_id()

#define PRINTF(...)
//#define PRINTF(...) printf(__VA_ARGS__)

static void start_timer()
{
    pi_perf_cl_reset();
    pi_perf_conf(1<<PI_PERF_CYCLES);
    pi_perf_cl_start();
}

static void reset_timer()
{
    pi_perf_cl_reset();
}

static unsigned int get_time()
{
    return pi_perf_cl_read(PI_PERF_CYCLES);
}
static inline unsigned int startTimer() {
    PRINTF("Starting timer\n");
    reset_timer();
    start_timer();
    return 0;
}
static inline unsigned int getTimer(unsigned int start)
{
    PRINTF("Ending timer\n");
    return get_time();
}

void test_barrier(unsigned int nthreads)
{
#pragma omp parallel num_threads(nthreads) shared(nthreads)
    {
        unsigned int start;
        int i;
        float operation_cost = 0;
        if (omp_get_thread_num() == 0) {
            start = startTimer();
        }
        for (i = 0; i < NB_BARRIER_ITER; i++)
        {
#pragma omp barrier
        }
        if (omp_get_thread_num() == 0) {
            unsigned int end = getTimer(start);
            operation_cost = (float) end / NB_BARRIER_ITER;
            printf("BARRIER %d threads: %f cycles\n", nthreads, operation_cost);
        }
    }

}

void test_critical(unsigned int nthreads)
{
#pragma omp parallel num_threads(nthreads)
    {
        int i;
        unsigned int start = startTimer();
        float operation_cost = 0;

        for (i = 0; i < NB_ITER; i++)
        {
#pragma omp critical
            {
                volatile int a = 0;
            }
        }

#pragma omp barrier

        operation_cost = (float) getTimer(start) / NB_ITER;
        if (CORE_ID == 0) {
            printf("CRITICAL %d threads: %.3f cycles\n", nthreads, operation_cost);
        }
    }
}

void test_parallel_loop_static(unsigned int nthreads)
{
    int i;
    int j;

    unsigned int start = startTimer();
    float iteration_cost = 0;

    for (i = 0; i < NB_ITER; i++)
    {
#pragma omp parallel for num_threads(nthreads)
        for (j = 0; j < LOOP_ITER; j++)
        {
            volatile int a = j;
        }
    }

    iteration_cost = ((float) getTimer(start)/(NB_ITER * LOOP_ITER));
    printf("PARALLEL FOR %d threads STATIC %d iter: %.3f cycle(s) per iteration\n", nthreads, LOOP_ITER, iteration_cost);
}

void test_entry()
{
    for (int i = 1; i <= pi_cl_cluster_nb_cores(); i++)
    {
        test_barrier(i);
    }

    printf("\n");
    for (int i = 1; i <= pi_cl_cluster_nb_cores(); i++)
    {
        test_critical(i);
    }

    printf("\n");
    for (int i = 1; i <= pi_cl_cluster_nb_cores(); i++)
    {
        test_parallel_loop_static (i);
    }
}

void launch_test(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
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
    cl_task.entry = test_entry;
    cl_task.arg = NULL;

    pi_cluster_send_task_to_cl(&cluster_dev, &cl_task);

    pi_cluster_close(&cluster_dev);

    printf("Test success !\n");

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** OpenMP Benchmark ***\n\n");
    return pmsis_kickoff((void *) launch_test);
}
