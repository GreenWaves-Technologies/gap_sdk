#include "pmsis.h"
#include "stdio.h"

#define NB_ITER 200
#define NB_HIGH_PRIO_TASKS 10
#define NB_LOW_PRIO_TASKS 3
#define STACK_SIZE 1024

static int nb_fork;
static int nb_callback_exec;
static PI_CL_L1 int current_iter[NB_LOW_PRIO_TASKS];
static int nb_yield;
static pi_cluster_task_t low_prio_cluster_task[NB_LOW_PRIO_TASKS], high_prio_cluster_task;
static pi_task_t low_prio_task[NB_LOW_PRIO_TASKS], high_prio_task, end_task;
static PI_CL_L1 int low_prio_errors = 0;

static void pe_entry(void *arg)
{
    int *var = (int *)arg;
    pi_cl_team_critical_enter();
    *var |= 1 << (pi_core_id());
    pi_cl_team_critical_exit();
}

static int check_fork(int nb_cores)
{
    int var = 0;
    nb_fork++;
    pi_cl_team_fork(nb_cores, pe_entry, (void *)&var);
    return (1<<pi_cl_team_nb_cores()) - 1 != var;
}

static void low_prio_cluster_entry(void *arg)
{
    int *current_iter = (int *)arg;

    if (pi_cl_cluster_nb_pe_cores() != NB_PE)
        low_prio_errors += 1;

    while ((*current_iter) < NB_ITER)
    {
        for (int i=0; i<=pi_cl_cluster_nb_pe_cores(); i++)
        {
            low_prio_errors += check_fork(i);
            if (i != 0)
                low_prio_errors += check_fork(0);
        }
        (*current_iter)++;

        if (pi_cl_task_yield())
        {
            nb_yield++;
            return;
        }
    }
}

static void high_prio_cluster_entry(void *arg)
{
}

static void low_prio_callback(void *arg)
{
    if (arg)
    {
        pi_task_push((pi_task_t *)arg);
    }
}


static int test_task_sync()
{
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    int high_prio_errors = 0, errors = 0;
    int stacks_size = STACK_SIZE * pi_cl_cluster_nb_pe_cores();

    pi_cluster_conf_init(&conf);
    conf.id = 0;
    conf.scratch_size = stacks_size + 0x8000;

    pi_open_from_conf(&cluster_dev, &conf);

    pi_cluster_open(&cluster_dev);

    for (int j=0; j<1; j++)
    {
        nb_fork = 0;

        pi_task_block(&end_task);

        for (int i=0; i<NB_LOW_PRIO_TASKS; i++)
        {
            // Init the task. We give it a stack from stack area to share it with other tasks
            pi_cluster_task(&low_prio_cluster_task[i], &low_prio_cluster_entry, (void *)&current_iter[i]);
            void *stacks = pi_cl_l1_scratch_alloc(&cluster_dev, &low_prio_cluster_task[i], stacks_size);
            pi_cluster_task_stacks(&low_prio_cluster_task[i], stacks, STACK_SIZE);

            // Enqueue task
            pi_cluster_enqueue_task_async(&cluster_dev,
                &low_prio_cluster_task[i],
                pi_task_callback(&low_prio_task[i], low_prio_callback, i==NB_LOW_PRIO_TASKS-1 ? (void *)&end_task : NULL)
            );
        }

        // Init the prio task only once since we send it in sequence.
        // Also give it a stack from scratch area to share it with low prio tasks.
        // We give it priority 1 so that low prio tasks are preempted
        pi_cluster_task(&high_prio_cluster_task, &high_prio_cluster_entry, (void *)&high_prio_errors);
        void *stacks = pi_cl_l1_scratch_alloc(&cluster_dev, &high_prio_cluster_task, stacks_size);
        pi_cluster_task_stacks(&high_prio_cluster_task, stacks, STACK_SIZE);
        pi_cluster_task_priority(&high_prio_cluster_task, 1);

        for (int i=0; i<NB_HIGH_PRIO_TASKS; i++)
        {
            // Send a prio task from time to time to let low prio tasks execute a bit
            pi_time_wait_us(200);
            pi_cluster_enqueue_task(&cluster_dev, &high_prio_cluster_task);
        }

        pi_task_wait_on(&end_task);

        // Check that low prio tasks has always been preempted by high prio tasks
        if (nb_yield != NB_HIGH_PRIO_TASKS)
        {
            errors++;
        }

        // Check that the task were correctly resumed by checking the total number of forks
        if (nb_fork != 17*NB_ITER*NB_LOW_PRIO_TASKS)
            errors++;

    }

    pi_cluster_close(&cluster_dev);

    return errors + high_prio_errors + low_prio_errors;
}


int test_entry()
{
    int errors = 0;

    printf("Starting test\n");

    errors += test_task_sync();

    if (errors)
        printf("Test failure\n");
    else
        printf("Test success\n");

    return errors;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
return pmsis_kickoff((void *)test_kickoff);
}
