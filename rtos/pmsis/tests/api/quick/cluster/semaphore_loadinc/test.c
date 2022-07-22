
#include "pmsis.h"
#include <string.h>

#define NB_ITER 1

#define TIMER0_PERIOD 73
#define TIMER1_PERIOD 201
#define PULP_TIMER0_EVENT 10
#define PULP_TIMER1_EVENT 11

#define MAX_TOKENS (1<<12)
#if TEST_DURATION > 50
#define SEM_LAST_VAL ((1<<12) - 16)

#elif TEST_DURATION >= 50
#define SEM_LAST_VAL ((1<<10) - 16)

#elif TEST_DURATION >= 25
#define SEM_LAST_VAL ((1<<8) - 16)

#else
#define SEM_LAST_VAL ((1<<6) - 16)

#endif

static int check_hw_sem_2cores(int max_delay);
static int check_hw_sem_9cores(int max_delay);
static int check_hw_sem_2cores_all_sems(int max_delay);
static int check_hw_sem_9cores_3sems(int max_delay);



static void __attribute__((interrupt)) irqHandler0()
{
}

static void __attribute__((interrupt)) irqHandler1()
{
}

static char sem_val_owner[ARCHI_EU_NB_HW_SEMAPHORE][MAX_TOKENS];
static int sem_val_count[ARCHI_CLUSTER_NB_CORES];
static int global_res;


static inline void timers_start()
{
#ifdef WITH_IRQ
    #if defined(__FREERTOS__)
    pi_timer_start(CL_TIMER_0);
    pi_timer_start(CL_TIMER_1);
    #else
    timer_conf_set(timer_base_cl(0, 0, 0),
      TIMER_CFG_LO_ENABLE(1) |
      TIMER_CFG_LO_RESET(1) |
      TIMER_CFG_LO_IRQEN(1) |
      TIMER_CFG_LO_MODE(1)
    );

    timer_conf_set(timer_base_cl(0, 0, 1),
      TIMER_CFG_LO_ENABLE(1) |
      TIMER_CFG_LO_RESET(1) |
      TIMER_CFG_LO_IRQEN(1) |
      TIMER_CFG_LO_MODE(1)
    );
    #endif
#endif
}

static inline void timers_stop()
{
#ifdef WITH_IRQ
    #if defined(__FREERTOS__)
    pi_timer_stop(CL_TIMER_0);
    pi_timer_stop(CL_TIMER_1);
    #else
    timer_conf_set(timer_base_cl(0, 0, 0),
      TIMER_CFG_LO_ENABLE(0)
    );

    timer_conf_set(timer_base_cl(0, 0, 1),
      TIMER_CFG_LO_ENABLE(0)
    );
    #endif
#endif
}


static int launch_tests(int max_delay)
{
    int errors = 0;

#if TEST_DURATION >= 75
    errors += check_hw_sem_2cores(max_delay);
#endif

#if TEST_DURATION >= 50
    errors += check_hw_sem_9cores(max_delay);
#endif

#if TEST_DURATION > 75
    errors += check_hw_sem_2cores_all_sems(max_delay);
#endif

#if TEST_DURATION >= 10
    errors += check_hw_sem_9cores_3sems(max_delay);
#endif

    return errors;
}


static int cluster_entry()
{
    int errors = 0;

    for (int i=0; i<ARCHI_EU_NB_HW_SEMAPHORE; i++)
    {
        for (int j=0; j<32; j++)
        {
            sem_val_owner[i][j] = -1;
        }
    }

    if (pi_core_id() == 0)
    {
#ifdef WITH_IRQ
        printf("Launching with interrupt\n");
#else
        printf("Launching without interrupt\n");
#endif
    }

#ifdef WITH_IRQ
    #if defined(__FREERTOS__)
    if (pi_core_id() & 1)
    {
        hal_cl_eu_irq_mask_set(1<<PULP_TIMER0_EVENT);
    }
    else
    {
        hal_cl_eu_irq_mask_set(1<<PULP_TIMER1_EVENT);
    }

    timer_cfg_u timer_cfg;
    pi_timer_conf_init(&timer_cfg);
    timer_cfg.enable = 0;
    pi_timer_init(CL_TIMER_0, timer_cfg, (uint32_t) TIMER0_PERIOD);
    pi_timer_init(CL_TIMER_1, timer_cfg, (uint32_t) TIMER1_PERIOD);

    pi_irq_enable();

    #else
    if (pi_core_id() & 1)
        eu_irq_maskSet(1<<PULP_TIMER0_EVENT);
    else
        eu_irq_maskSet(1<<PULP_TIMER1_EVENT);

    // We set setup timers in order to generate interrupts on the cores while they
    // are accessing the dispatch
    // We use 2 timers with different periods to randomize the delay between 2 interrupts
    // And 2 have 2 sets of cores
    timer_cmp_set(timer_base_cl(0, 0, 0), TIMER0_PERIOD);
    timer_cmp_set(timer_base_cl(0, 0, 1), TIMER1_PERIOD);

    hal_irq_enable();
    #endif
#endif

    errors += launch_tests(0);
#if TEST_DURATION >= 50
    errors += launch_tests(10);
#if TEST_DURATION >= 75
    errors += launch_tests(30);
#endif
#endif

    return errors;
}


static inline void exec_cluster_pe_stub(void *arg)
{
  int *desc = arg;
  int (*entry)() = (int (*)())desc[0];
  int status = entry();
  pi_cl_team_critical_enter();
  desc[1] += status;
  pi_cl_team_critical_exit();
}

static inline void exec_cluster_stub(void *arg)
{
  pi_cl_team_fork_cc(0, exec_cluster_pe_stub, arg);
}


static inline void exec_cluster(void)
{
  int desc[2] = { (int)cluster_entry, 0 };

  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task cluster_task;

  #ifdef WITH_IRQ
  pi_cl_irq_handler_fast_set(PULP_TIMER0_EVENT, irqHandler0);
  pi_cl_irq_handler_fast_set(PULP_TIMER1_EVENT, irqHandler1);
  #endif

  pi_cluster_conf_init(&conf);
  conf.cc_stack_size = 0x800;

  pi_open_from_conf(&cluster_dev, &conf);

  pi_cluster_open(&cluster_dev);

  pi_cluster_task(&cluster_task, exec_cluster_stub, desc);
  pi_cluster_task_stacks(&cluster_task, NULL, 0x800);
  pi_cluster_send_task(&cluster_dev, &cluster_task);

  pi_cluster_close(&cluster_dev);

  pmsis_exit(desc[1]);
}

int main()
{
    return pmsis_kickoff((void *) exec_cluster);
}


typedef struct {
    int nb_sems;
    int nb_cores;
    uint32_t teams[ARCHI_CLUSTER_NB_CORES];
    int cores_sem[ARCHI_CLUSTER_NB_CORES];
    int first_core[ARCHI_EU_NB_HW_SEMAPHORE];
    int allocated_sems[ARCHI_CLUSTER_NB_CORES];
} sem_test_t;


static sem_test_t sem_test;


static int loop_on_sem(int sem, int delay, int max_val)
{
    int errors = 0;

    int current_delay = 0;
    int delay_inc = delay / 5;

    int core_id = pi_core_id();
    int sem_id = pi_cl_sem_id(sem);

    while (1)
    {
        current_delay += delay_inc;
        if (current_delay > delay)
            current_delay = 0;

        // First allocate  small sems, number vary between 1 and nb_alloc
        // Also keep track that we are the owner of each allocated bit to check if
        // a bit is allocated twice
        char *val_owner = sem_val_owner[sem_id];

        int val = pi_cl_sem_load_inc(sem);
        if (__builtin_expect(val < max_val, 1))
        {
            val_owner[val] = core_id;
            sem_val_count[core_id]++;
        }
        else
        {
            break;
        }


        // Randomize a bit
        for(volatile int i=0; i<current_delay; i++);
    }

    return errors;
}



static void sem_test_init(sem_test_t *test)
{
    if (pi_core_id() == 0)
    {
        test->nb_sems = 0;
        for (int i=0; i<ARCHI_CLUSTER_NB_CORES; i++)
        {
            test->cores_sem[i] = -1;
        }
        test->nb_cores = 0;
    }
}


static int sem_test_team(sem_test_t *test, int nb_cores)
{
    if (pi_core_id() == 0)
    {
        uint32_t sem = pi_cl_sem_alloc();
        if (sem == 0)
            return -1;
        int sem_id = pi_cl_sem_id(sem);

        test->allocated_sems[test->nb_sems++] = sem;

        for (int i=0; i<nb_cores; i++)
        {
            int core = test->nb_cores++;
            test->cores_sem[core] = sem;
            if (i == 0)
            {
                test->first_core[sem_id] = core;
            }
        }
    }

    return 0;
}


static int sem_test_deinit(sem_test_t *test)
{
    if (pi_core_id() == 0)
    {
        for (int i=0; i<test->nb_sems; i++)
        {
            pi_cl_sem_free(test->allocated_sems[i]);
        }
    }
}


static int sem_test_exec(sem_test_t *test, int delay, int nb_iter, int max_val)
{
    int errors = 0;

    global_res = 0;

    pi_cl_team_barrier_cc();

    for (int i=0; i<nb_iter; i++)
    {
        int sem = test->cores_sem[pi_core_id()];
        int sem_id = pi_cl_sem_id(sem);

        if (sem != -1 && pi_core_id() == test->first_core[sem_id])
        {
            pi_cl_sem_set(sem, 0);
            memset(sem_val_owner[sem_id], -1, MAX_TOKENS);
            memset(sem_val_count, 0, sizeof(sem_val_count));
        }

        pi_cl_team_barrier_cc();

        timers_start();

        if (sem != -1)
        {
            errors += loop_on_sem(sem, delay, max_val);
        }

        pi_cl_team_barrier_cc();

        timers_stop();

        if (sem != -1 && pi_core_id() == test->first_core[sem_id])
        {
            char *val_owner = sem_val_owner[sem_id];
            int val_count[ARCHI_CLUSTER_NB_CORES];
            int total_count = 0;
            for (int j=0; j<ARCHI_CLUSTER_NB_CORES; j++)
            {
                val_count[j] = 0;
            }
            for (int j=0; j<MAX_TOKENS; j++)
            {
                if (val_owner[j] != -1 && test->cores_sem[val_owner[j]] == sem)
                {
                    val_count[val_owner[j]]++;
                    total_count++;
                }
            }
            for (int j=0; j<ARCHI_CLUSTER_NB_CORES; j++)
            {
                if (test->cores_sem[j] == sem)
                {
                    errors += val_count[j] != sem_val_count[j];
                }
            }

            errors += pi_cl_sem_get(sem) < max_val;
            errors += total_count < max_val;
        }

        pi_cl_team_barrier_cc();
    }

    pi_cl_team_critical_enter();
    global_res += errors;
    pi_cl_team_critical_exit();

    pi_cl_team_barrier_cc();


    return global_res;
}



static int check_hw_sem_2cores(int max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW sem test (2 cores, max_delay: %d)\n", max_delay);

    sem_test_init(&sem_test);

    // sem=0, 2 cores
    if (sem_test_team(&sem_test, 2))
        return -1;

    // nb_tokens=200, delay=max_delay, nb_alloc=32
    int res = sem_test_exec(&sem_test, max_delay, NB_ITER, SEM_LAST_VAL);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_9cores(int max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW sem test (9 cores, max_delay: %d)\n", max_delay);

    sem_test_init(&sem_test);

    // sem=0, 9 cores
    if (sem_test_team(&sem_test, 9))
        return -1;

    // nb_tokens=200, delay=max_delay, nb_alloc=32
    int res = sem_test_exec(&sem_test, max_delay, NB_ITER, SEM_LAST_VAL);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_2cores_all_sems(int max_delay)
{
    int res = 0;

    if (pi_core_id() == 0)
        printf("Starting HW sem test (2 cores, all sems, max_delay: %d)\n", max_delay);

    for (int i=0; i<ARCHI_EU_NB_HW_SEMAPHORE; i++)
    {
        sem_test_init(&sem_test);

        // sem=0, 2 cores
        if (sem_test_team(&sem_test, 2))
            return -1;

        // nb_tokens=10, delay=max_delay, nb_alloc=32
        res += sem_test_exec(&sem_test, max_delay, NB_ITER, SEM_LAST_VAL/ARCHI_EU_NB_HW_SEMAPHORE);

        sem_test_deinit(&sem_test);

    }

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_9cores_3sems(int max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW sem test (9 cores, 3 sems, max_delay: %d)\n", max_delay);

    sem_test_init(&sem_test);

    // sem=0, 3 cores
    if (sem_test_team(&sem_test, 3))
        return -1;

    // sem=1, 3 cores
    if (sem_test_team(&sem_test, 3))
        return -1;

    // sem=2, 3 cores
    if (sem_test_team(&sem_test, 3))
        return -1;

    // nb_tokens=200, delay=max_delay, nb_alloc=32
    int res = sem_test_exec(&sem_test, max_delay, NB_ITER, SEM_LAST_VAL);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}
