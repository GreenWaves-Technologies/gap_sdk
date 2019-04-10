#include "rt/rt_api.h"

#define DUMMY_LOOP_COUNT 10000
#define FREQ_MARGIN 15

static int do_dummy_loop(int iter)
{
  rt_perf_t perf;
  rt_perf_init(&perf);
  rt_perf_conf(&perf, 1<<RT_PERF_ACTIVE_CYCLES);
  rt_perf_reset(&perf);
  rt_perf_start(&perf);

  volatile int i;
  for (i=0; i<iter; i++);

  rt_perf_stop(&perf);

  return rt_perf_read(RT_PERF_ACTIVE_CYCLES);
}



static void cluster_entry(void *arg)
{
  *(int *)arg = do_dummy_loop(DUMMY_LOOP_COUNT * *(unsigned int *)arg);
}


static int check_cl_freq(int freq)
{
  int factor = freq / 1000000;
  uint64_t cycles = factor;

  //printf("Checking with cluster frequency: %d\n", freq);

  rt_freq_set(RT_FREQ_DOMAIN_CL, freq);

  unsigned int start = rt_time_get_us();

  rt_cluster_call(NULL, 0, cluster_entry, (void *)&cycles, NULL, 0, 0, 0, NULL);

  unsigned int diff = rt_time_get_us() - start;

  //printf("Got cycles: %d\t Time: %d\n", cycles, diff);

  //printf("Read frequency: %d KHz\n", cycles * 100 / diff * 10);

  int read_freq = cycles * 1000000 / diff;
  printf("%3d%% error for frequency %10d (got %10d)\n", (read_freq - freq) / (freq / 100), freq, read_freq);

  return 0;
}


static inline int abs(int val)
{
  return val >= 0 ? val : -val;
}

static int check_fc_freq(int freq)
{
  int cycles;

  //printf("Checking with FC frequency: %d\n", freq);

  rt_freq_set(RT_FREQ_DOMAIN_FC, freq);

  unsigned int start = rt_time_get_us();

  cycles = do_dummy_loop(DUMMY_LOOP_COUNT * freq / 1000000);

  unsigned int diff = rt_time_get_us() - start;

  //printf("Got cycles: %d\t Time: %d\n", cycles, diff);

  int read_freq = cycles * 100 / diff * 10000;
  int error = (read_freq - freq) / (freq / 100);
  printf("%3d%% error for frequency %10d (got %10d)\n", error, freq, read_freq);

  return abs(error) > FREQ_MARGIN;
}

int main()
{

  printf("Cluster frequency test\n");

  rt_cluster_mount(1, 0, 0, NULL);

  int cl_freqs[] = {10000000, 20000000, 50000000, 100000000, 150000000, 200000000, 250000000, 300000000};

  for (int i=0; i<sizeof(cl_freqs)/sizeof(int); i++)
  {
    if (check_cl_freq(cl_freqs[i]))
      return -1;
  }

  rt_cluster_mount(0, 0, 0, NULL);

  printf("\n");
  printf("FC frequency test\n");

  int fc_freqs[] = {10000000, 20000000, 50000000, 100000000, 150000000, 200000000, 250000000, 300000000};

  for (int i=0; i<sizeof(fc_freqs)/sizeof(int); i++)
  {
    if (check_fc_freq(fc_freqs[i]))
      return -1;
  }

  printf("Test success\n");
  return 0;
}
