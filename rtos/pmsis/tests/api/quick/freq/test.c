
#define ARCHI_CL_CID 0


#include <pmsis.h>

//#define DUMMY_LOOP_COUNT 10000
#define DUMMY_LOOP_COUNT 500
#define FREQ_MARGIN 10

static PI_CL_L1 volatile int started;
static PI_CL_L1 volatile int ended;


static int do_dummy_loop_cl(int iter)
{
  pi_perf_conf(1<<PI_PERF_CYCLES);
  pi_perf_reset();
  started = 1;
  pi_perf_start();

  volatile int i;
  for (i=0; i<iter; i++);

  pi_perf_stop();
  ended = 1;
  
  return pi_perf_read(PI_PERF_CYCLES);
}

static int do_dummy_loop(int iter)
{
  pi_perf_conf(1<<PI_PERF_CYCLES);
  pi_perf_reset();
  pi_perf_start();

  volatile int i;
  for (i=0; i<iter; i++);

  pi_perf_stop();
  
  return pi_perf_read(PI_PERF_CYCLES);
}



static void cluster_entry(void *arg)
{
  *(int *)arg = do_dummy_loop_cl(DUMMY_LOOP_COUNT * *(unsigned int *)arg);
}

static int check_cl_freq(int freq)
{
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task task;
  pi_task_t fc_task;
  int factor = freq / 1000000;
#if PULP_CHIP == CHIP_WOLFE
  uint32_t cycles = factor;
#else
  uint64_t cycles = factor;
#endif

  //printf("Checking with cluster frequency: %d\n", freq);

  pi_cluster_conf_init(&conf);
  pi_open_from_conf(&cluster_dev, &conf);
  pi_cluster_open(&cluster_dev);

  pi_freq_set(PI_FREQ_DOMAIN_CL, freq);

  pi_cluster_task(&task, cluster_entry, (void *)&cycles);

  started = 0;
  ended = 0;

  pi_cluster_send_task_to_cl_async(&cluster_dev, &task, pi_task_block(&fc_task));

  while(!started);

  unsigned int start = pi_time_get_us();

#if 0
  rt_time_wait_us(100);
  int start_cycles = hal_timer_count_get(hal_timer_cl_addr_glob(0, 0));

  rt_time_wait_us(10000);
  int stop_cycles = hal_timer_count_get(hal_timer_cl_addr_glob(0, 0));
#endif

  //rt_event_wait(event);

#if 0
  cycles = stop_cycles - start_cycles;

  unsigned int diff = 10000;

  rt_cluster_mount(0, 0, 0, NULL);

  //printf("Got cycles: %d\t Time: %d\n", cycles, diff);

  //printf("Read frequency: %d KHz\n", cycles * 100 / diff * 10);

  int read_freq = cycles * 1000 / diff * 1000; 
  printf("%3d%% error for frequency %10d (got %10d)\n", (read_freq - freq) / (freq / 100), freq, read_freq);
#else

  while(!ended);

  unsigned int diff = pi_time_get_us() - start;

  pi_task_wait_on(&fc_task);

  pi_cluster_close(&cluster_dev);

  //printf("Got cycles: %d\t Time: %d\n", cycles, diff);

  //printf("Read frequency: %d KHz\n", cycles * 100 / diff * 10);

  int read_freq = cycles * 1000 / diff * 1000; 
  printf("%3d%% error for frequency %10d (got %10d)\n", (read_freq - freq) / (freq / 100), freq, read_freq);
#endif

  return 0;
}

#if !defined(__FREERTOS__)
static inline int abs(int val)
{
  return val >= 0 ? val : -val;
}
#endif  /* __FREERTOS__ */

static int check_fc_freq(int freq)
{
  int cycles;

  //printf("Checking with FC frequency: %d\n", freq);

  pi_freq_set(PI_FREQ_DOMAIN_FC, freq);

  unsigned int start = pi_time_get_us();

  cycles = do_dummy_loop(DUMMY_LOOP_COUNT * freq / 1000000);

  unsigned int diff = pi_time_get_us() - start;

  //printf("Got cycles: %d\t Time: %d\n", cycles, diff);

  int read_freq = cycles * 100 / diff * 10000;
  int error = (read_freq - freq) / (freq / 100);
  printf("%3d%% error for frequency %10d (got %10d)\n", error, freq, read_freq);

  return abs(error) > FREQ_MARGIN;
}

int test_entry()
{
  printf("Cluster frequency test\n");

#if TEST_DURATION == 0
  int fc_freqs_for_cl[] = {25000000};
#else
  int fc_freqs_for_cl[] = {5000000, 50000000};
#endif
  //int fc_freqs_for_cl[] = {5000000, 50000000, 200000000};

  for (int i=0; i<sizeof(fc_freqs_for_cl)/sizeof(int); i++)
  {
    pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freqs_for_cl[i]);

#ifdef __PLATFORM_RTL__
#if TEST_DURATION == 0
    int cl_freqs[] = {5000000, 200000000};
#else
    int cl_freqs[] = {5000000, 50000000, 200000000};
#endif
#else
    int cl_freqs[] = {10000000, 20000000, 50000000, 100000000, 150000000, 200000000, 250000000};
#endif

    for (int i=0; i<sizeof(cl_freqs)/sizeof(int); i++)
    {
      if (check_cl_freq(cl_freqs[i]))
        return -1;
    }
  }

  printf("\n");
  printf("FC frequency test\n");

  int fc_freqs[] = {5000000, 50000000, 200000000};

  //int fc_freqs[] = {1000000, 2000000, 5000000, 10000000, 20000000, 50000000, 100000000, 150000000, 200000000, 250000000, 300000000};

  for (int i=0; i<sizeof(fc_freqs)/sizeof(int); i++)
  {
    if (check_fc_freq(fc_freqs[i]))
      return -1;
  }

  return 0;
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
