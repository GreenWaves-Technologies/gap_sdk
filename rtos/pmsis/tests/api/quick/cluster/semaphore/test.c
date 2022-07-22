
#include "pmsis.h"
#include <string.h>

#define NB_ITER 32

#define TIMER0_PERIOD 115
#define TIMER1_PERIOD 73
#define PULP_TIMER0_EVENT 10
#define PULP_TIMER1_EVENT 11


static int check_hw_sem_1producer_1consumer(int producer_max_delay, int consumer_max_delay);
static int check_hw_sem_1producer_8consumer(int producer_max_delay, int consumer_max_delay);
static int check_hw_sem_4producer_5consumer(int producer_max_delay, int consumer_max_delay);
static int check_hw_sem_1producer_2consumer_3sem(int producer_max_delay, int consumer_max_delay);
static int check_hw_sem_4producer_5consumer_all_sem(int producer_max_delay, int consumer_max_delay);



static void __attribute__((interrupt)) irqHandler0()
{
}

static void __attribute__((interrupt)) irqHandler1()
{
}


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


static int launch_tests(int producer_max_delay, int consumer_max_delay)
{
    int errors = 0;

#if TEST_DURATION >= 75
    errors += check_hw_sem_1producer_1consumer(producer_max_delay, consumer_max_delay);
#endif

#if TEST_DURATION >= 75
    errors += check_hw_sem_1producer_8consumer(producer_max_delay, consumer_max_delay);
#endif

#if TEST_DURATION >= 25
    errors += check_hw_sem_4producer_5consumer(producer_max_delay, consumer_max_delay);
#endif

    errors += check_hw_sem_1producer_2consumer_3sem(producer_max_delay, consumer_max_delay);

#if TEST_DURATION > 75
    errors += check_hw_sem_4producer_5consumer_all_sem(producer_max_delay, consumer_max_delay);
#endif

    return errors;
}


static int cluster_entry()
{
    int errors = 0;

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

    errors += launch_tests(0, 0);

#if TEST_DURATION >= 50
    errors += launch_tests(10, 10);
#if TEST_DURATION >= 75
    errors += launch_tests(5, 20);
#if TEST_DURATION > 75
    errors += launch_tests(20, 5);
#endif
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
    uint32_t cores_sem[ARCHI_CLUSTER_NB_CORES];
    int cores_is_producer[ARCHI_CLUSTER_NB_CORES];
    int allocated_sems[ARCHI_CLUSTER_NB_CORES];
} sem_test_t;


static sem_test_t sem_test;

static sem_test_t sem_test_all[ARCHI_EU_NB_HW_SEMAPHORE];


static void loop_on_sem_producer(uint32_t sem, int delay, int nb_iter, int max_inc)
{
    int inc = 1;
    int current_delay = 0;
    int delay_inc = delay / 5;

    while (nb_iter)
    {
        current_delay += delay_inc;
        if (current_delay > delay)
            current_delay = 0;

        for(volatile int i=0; i<current_delay; i++);

        if (nb_iter < inc)
        {
            inc = nb_iter;
        }

        pi_cl_sem_inc(sem, inc);

        nb_iter -= inc;

        // Make the increment vary between 1 and max_inc
        inc++;
        if (inc > max_inc)
        {
            inc = 1;
        }
    }
}


static void loop_on_sem_consumer(uint32_t sem, int delay, int nb_iter)
{
    int current_delay = 0;
    int delay_inc = delay / 5;

    for (int i=0; i<nb_iter; i++)
    {
        current_delay += delay_inc;
        if (current_delay > delay)
            current_delay = 0;

        for(volatile int i=0; i<current_delay; i++);

#ifdef WITH_IRQ
        pi_cl_sem_dec_cc(sem);
#else
        pi_cl_sem_dec(sem);
#endif
    }
}



static void sem_test_init(sem_test_t *test)
{
  if (pi_core_id() == 0)
  {
      test->nb_sems = 0;
      for (int i=0; i<ARCHI_CLUSTER_NB_CORES; i++)
      {
          test->cores_sem[i] = 0;
      }
      test->nb_cores = 0;
  }
}


static int sem_test_team(sem_test_t *test, int nb_producer, int nb_consumer)
{
    if (pi_core_id() == 0)
    {
        uint32_t sem = pi_cl_sem_alloc();
        if (sem == 0)
            return -1;

        test->allocated_sems[test->nb_sems++] = sem;

        for (int i=0; i<nb_producer; i++)
        {
            int core = test->nb_cores++;
            test->cores_sem[core] = sem;
            test->cores_is_producer[core] = 1;
        }
        for (int i=0; i<nb_consumer; i++)
        {
            int core = test->nb_cores++;
            test->cores_sem[core] = sem;
            test->cores_is_producer[core] = 0;
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



static int sem_test_exec(sem_test_t *test, int nb_producer_tokens, int max_producer_inc, int producer_delay, int nb_consumer_tokens, int consumer_delay, int nb_iter)
{
    int errors = 0;

    for (int i=0; i<nb_iter; i++)
    {
        pi_cl_team_barrier_cc();


        timers_start();

        int sem = test->cores_sem[pi_core_id()];
        if (sem != 0)
        {
            if (test->cores_is_producer[pi_core_id()])
            {
                loop_on_sem_producer(sem, producer_delay, nb_producer_tokens, max_producer_inc);
            }
            else
            {
                loop_on_sem_consumer(sem, consumer_delay, nb_consumer_tokens);
            }
        }

        pi_cl_team_barrier_cc();

        timers_stop();

        if (sem != 0)
        {
            errors += pi_cl_sem_get(sem) != 0;
        }
    }

    return errors;
}



static int check_hw_sem_1producer_1consumer(int producer_max_delay, int consumer_max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW semaphore test (1 producer, 1 consumer, producer_max_delay: %d, consumer_max_delay: %d)\n", producer_max_delay, consumer_max_delay);

    sem_test_init(&sem_test);

    // sem=0, nb_producer=1, nb_consumer=1
    if (sem_test_team(&sem_test, 1, 1))
        return -1;

    // nb_producer_tokens=NB_TOKENS, max_producer_inc=4, producer_delay=0, nb_consumer_tokens=NB_TOKENS, consumer_delay=0
    int res = sem_test_exec(&sem_test, 200, 4, producer_max_delay, 200, consumer_max_delay, NB_ITER);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_1producer_8consumer(int producer_max_delay, int consumer_max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW semaphore test (1 producer, 8 consumer, producer_max_delay: %d, consumer_max_delay: %d)\n", producer_max_delay, consumer_max_delay);

    sem_test_init(&sem_test);

    // sem=0, nb_producer=1, nb_consumer=8
    if (sem_test_team(&sem_test, 1, 8))
        return -1;

    // nb_producer_tokens=NB_TOKENS, max_producer_inc=12, producer_delay=0, nb_consumer_tokens=NB_TOKENS, consumer_delay=0
    int res = sem_test_exec(&sem_test, 20*8, 12, producer_max_delay, 20, consumer_max_delay, NB_ITER);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_4producer_5consumer(int producer_max_delay, int consumer_max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW semaphore test (4 producer, 5 consumer, producer_max_delay: %d, consumer_max_delay: %d)\n", producer_max_delay, consumer_max_delay);

    sem_test_init(&sem_test);

    // sem=0, nb_producer=4, nb_consumer=5
    if (sem_test_team(&sem_test, 4, 5))
        return -1;

    // nb_producer_tokens=NB_TOKENS, max_producer_inc=12, producer_delay=0, nb_consumer_tokens=NB_TOKENS, consumer_delay=0
    int res = sem_test_exec(&sem_test, 12*5, 12, producer_max_delay, 12*4, consumer_max_delay, NB_ITER);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_1producer_2consumer_3sem(int producer_max_delay, int consumer_max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW semaphore test (1 producer, 2 consumer, 3sem, producer_max_delay: %d, consumer_max_delay: %d)\n", producer_max_delay, consumer_max_delay);

    sem_test_init(&sem_test);

    // sem=0, nb_producer=1, nb_consumer=2
    if (sem_test_team(&sem_test, 1, 2))
        return -1;

    if (sem_test_team(&sem_test, 1, 2))
        return -1;

    if (sem_test_team(&sem_test, 1, 2))
        return -1;

    // nb_producer_tokens=NB_TOKENS, max_producer_inc=12, producer_delay=0, nb_consumer_tokens=NB_TOKENS, consumer_delay=0
    int res = sem_test_exec(&sem_test, 100*2, 12, producer_max_delay, 100, consumer_max_delay, NB_ITER);

    sem_test_deinit(&sem_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_sem_4producer_5consumer_all_sem(int producer_max_delay, int consumer_max_delay)
{
  int res = 0;

    if (pi_core_id() == 0)
        printf("Starting HW semaphore test (all sem, 4 producer, 5 consumer, producer_max_delay: %d, consumer_max_delay: %d)\n", producer_max_delay, consumer_max_delay);

    for (int i=0; i<ARCHI_EU_NB_HW_SEMAPHORE; i++)
    {
        sem_test_init(&sem_test_all[i]);

        // sem=0, nb_producer=4, nb_consumer=5
        if (sem_test_team(&sem_test_all[i], 4, 5))
            continue;

        // nb_producer_tokens=NB_TOKENS, max_producer_inc=12, producer_delay=0, nb_consumer_tokens=NB_TOKENS, consumer_delay=0
        res += sem_test_exec(&sem_test_all[i], 12*5, 12, producer_max_delay, 12*4, consumer_max_delay, 1);

    }

    for (int i=0; i<ARCHI_EU_NB_HW_SEMAPHORE; i++)
    {
        sem_test_deinit(&sem_test_all[i]);
    }

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}
