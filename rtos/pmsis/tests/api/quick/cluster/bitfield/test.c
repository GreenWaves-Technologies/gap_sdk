
#include "pmsis.h"
#include <string.h>

#if TEST_DURATION >= 75
#define NB_ITER 4
#elif TEST_DURATION >= 50
#define NB_ITER 2
#else
#define NB_ITER 1
#endif

#define TIMER0_PERIOD 64
#define TIMER1_PERIOD 201
#define PULP_TIMER0_EVENT 10
#define PULP_TIMER1_EVENT 11


static int check_hw_bitfield_2cores(int max_delay);
static int check_hw_bitfield_9cores(int max_delay);
static int check_hw_bitfield_2cores_all_bitfields(int max_delay);
static int check_hw_bitfield_9cores_3bitfields(int max_delay);


static void __attribute__((interrupt)) irqHandler0()
{
}

static void __attribute__((interrupt)) irqHandler1()
{
}

static char bitfield_bit_owner[ARCHI_EU_NB_HW_BITFIELD][32];
static int bitfield_bit_used[ARCHI_EU_NB_HW_BITFIELD];
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
    errors += check_hw_bitfield_2cores(max_delay);
#endif

#if TEST_DURATION >= 50
    errors += check_hw_bitfield_9cores(max_delay);
#endif

#if TEST_DURATION > 75
    errors += check_hw_bitfield_2cores_all_bitfields(max_delay);
#endif

#if TEST_DURATION >= 10
    errors += check_hw_bitfield_9cores_3bitfields(max_delay);
#endif

    return errors;
}


static int cluster_entry()
{
    int errors = 0;

    for (int i=0; i<ARCHI_EU_NB_HW_BITFIELD; i++)
    {
        for (int j=0; j<32; j++)
        {
            bitfield_bit_owner[i][j] = -1;
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
#if TEST_DURATION >= 70
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
    int nb_bitfields;
    int nb_cores;
    uint32_t teams[ARCHI_CLUSTER_NB_CORES];
    int cores_bitfield[ARCHI_CLUSTER_NB_CORES];
    int allocated_bitfields[ARCHI_CLUSTER_NB_CORES];
} bitfield_test_t;


static bitfield_test_t bitfield_test;


static int loop_on_bitfield(int bitfield, int delay, int nb_iter, int min_alloc, int max_alloc)
{
    int errors = 0;

    int bitfield_id = pi_cl_bitfield_id(bitfield);
    int current_delay = 0;
    int delay_inc = delay / 5;

    int bitfields[max_alloc];
    int current_nb_alloc = max_alloc;

    int core_id = pi_core_id();

    while (nb_iter)
    {
        current_delay += delay_inc;
        if (current_delay > delay)
            current_delay = 0;

        if (nb_iter < current_nb_alloc)
        {
            current_nb_alloc = nb_iter;
        }

        // First allocate some bitfields, number vary between 1 and nb_alloc
        // Also keep track that we are the owner of each allocated bit to check if
        // a bit is allocated twice
        char *bit_owner = bitfield_bit_owner[bitfield_id];
        for (int i=0; i<current_nb_alloc; i++)
        {
            int bit = pi_cl_bitfield_bit_alloc(bitfield);
            if (__builtin_expect(bit != 32, 1))
            {
                bit_owner[bit] = core_id;
            }
            bitfields[i] = bit;
        }


        // Randomize a bit
        for(volatile int i=0; i<current_delay; i++);

        // For each allocated bit, check that we are still the owner and frees it
        for (int i=0; i<current_nb_alloc; i++)
        {
            int bit = bitfields[i];
            if (bit != 32)
            {
                nb_iter--;
                errors += bitfield_bit_owner[bitfield_id][bit] != core_id;
                bitfield_bit_owner[bitfield_id][bit] = -1;
                pi_cl_team_critical_enter();
                bitfield_bit_used[bitfield_id] |= 1<<bit;
                pi_cl_team_critical_exit();
                hal_compiler_barrier();
                pi_cl_bitfield_bits_set(bitfield, 1<<bit);
            }
        }

        current_nb_alloc--;
        if (current_nb_alloc <= min_alloc)
        {
            current_nb_alloc = max_alloc;
        }

    }

    return errors;
}



static void bitfield_test_init(bitfield_test_t *test)
{
  if (pi_core_id() == 0)
  {
      test->nb_bitfields = 0;
      for (int i=0; i<ARCHI_CLUSTER_NB_CORES; i++)
      {
          test->cores_bitfield[i] = -1;
      }
      test->nb_cores = 0;
  }
}


static int bitfield_test_team(bitfield_test_t *test, int nb_cores)
{
    if (pi_core_id() == 0)
    {
        uint32_t bitfield = pi_cl_bitfield_alloc();
        if (bitfield == 0)
            return -1;

        test->allocated_bitfields[test->nb_bitfields++] = bitfield;

        for (int i=0; i<nb_cores; i++)
        {
            int core = test->nb_cores++;
            test->cores_bitfield[core] = bitfield;
        }
    }

    return 0;
}


static int bitfield_test_deinit(bitfield_test_t *test)
{
    if (pi_core_id() == 0)
    {
        for (int i=0; i<test->nb_bitfields; i++)
        {
            pi_cl_bitfield_free(test->allocated_bitfields[i]);
        }
    }
}


static int bitfield_test_exec(bitfield_test_t *test, int nb_tokens, int delay, int nb_iter, int min_alloc, int max_alloc)
{
    int errors = 0;

    global_res = 0;

    pi_cl_team_barrier_cc();

    for (int i=0; i<nb_iter; i++)
    {
        int bitfield = test->cores_bitfield[pi_core_id()];
        int bitfield_id = pi_cl_bitfield_id(bitfield);

        if (bitfield != -1)
        {
            pi_cl_bitfield_value_set(bitfield, 0xffffffff);
            bitfield_bit_used[bitfield_id] = 0;
        }

        pi_cl_team_barrier_cc();

        timers_start();

        if (bitfield != -1)
        {
            errors += loop_on_bitfield(bitfield, delay, nb_tokens, min_alloc, max_alloc);
        }

        pi_cl_team_barrier_cc();

        timers_stop();

        if (bitfield != -1)
        {
            errors += pi_cl_bitfield_value_get(bitfield) != 0xffffffff;

            errors += bitfield_bit_used[bitfield_id] != 0xffffffff;
        }

        pi_cl_team_barrier_cc();
    }

    pi_cl_team_critical_enter();
    global_res += errors;
    pi_cl_team_critical_exit();

    pi_cl_team_barrier_cc();


    return global_res;
}



static int check_hw_bitfield_2cores(int max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW bitfield test (2 cores, max_delay: %d)\n", max_delay);

    bitfield_test_init(&bitfield_test);

    // bitfield=0, 2 cores
    if (bitfield_test_team(&bitfield_test, 2))
    {
        return -1;
    }

    // nb_tokens=200, delay=max_delay, nb_alloc=32
    int res = bitfield_test_exec(&bitfield_test, 200, max_delay, NB_ITER, 1, 32);

    bitfield_test_deinit(&bitfield_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_bitfield_9cores(int max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW bitfield test (9 cores, max_delay: %d)\n", max_delay);

    bitfield_test_init(&bitfield_test);

    // bitfield=0, 9 cores
    if (bitfield_test_team(&bitfield_test, 9))
        return -1;

    // nb_tokens=200, delay=max_delay, nb_alloc=32
    int res = bitfield_test_exec(&bitfield_test, 200, max_delay, NB_ITER, 1, 8);

    bitfield_test_deinit(&bitfield_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_bitfield_2cores_all_bitfields(int max_delay)
{
    int res = 0;

    if (pi_core_id() == 0)
        printf("Starting HW bitfield test (2 cores, all bitfields, max_delay: %d)\n", max_delay);

    for (int i=0; i<pi_cl_bitfield_nb_available(); i++)
    {
        bitfield_test_init(&bitfield_test);

        // bitfield=0, 2 cores
        if (bitfield_test_team(&bitfield_test, 2))
            return -1;

        // nb_tokens=10, delay=max_delay, nb_alloc=32
        res += bitfield_test_exec(&bitfield_test, 32, max_delay, NB_ITER, 32, 32);

        bitfield_test_deinit(&bitfield_test);
    }

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}


static int check_hw_bitfield_9cores_3bitfields(int max_delay)
{
    if (pi_core_id() == 0)
        printf("Starting HW bitfield test (9 cores, 3 bitfields, max_delay: %d)\n", max_delay);

    bitfield_test_init(&bitfield_test);

    // bitfield=0, 3 cores
    if (bitfield_test_team(&bitfield_test, 3))
        return -1;

    // bitfield=1, 3 cores
    if (bitfield_test_team(&bitfield_test, 3))
        return -1;

    // bitfield=2, 3 cores
    if (bitfield_test_team(&bitfield_test, 3))
        return -1;

    // nb_tokens=200, delay=max_delay, nb_alloc=32
    int res = bitfield_test_exec(&bitfield_test, 200, max_delay, NB_ITER, 1, 32);

    bitfield_test_deinit(&bitfield_test);

    if (pi_core_id() == 0)
        printf("%s\n", res ? "KO" : "OK");

    return res;
}
