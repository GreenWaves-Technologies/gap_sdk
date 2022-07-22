/* 
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"
#include "stdio.h"
#include "bsp/bsp.h"
#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif
#include "bsp/flash.h"
#include <string.h>

#define HYPER_FLASH 0
#define SPI_FLASH   1
#define MFLASH        2

#ifndef FULL_PLOT
#define FULL_PLOT 0
#endif

#define BUFF_SIZE (32*1024)
#define NB_ITER   4

static PI_L2 char buff[2][BUFF_SIZE];
static uint32_t hyper_buff[2];
static struct pi_device hyper;

#ifdef USE_HYPERFLASH
#define FLASH_NAME "hyperflash"
#elif defined(USE_MRAM)
#define FLASH_NAME "mram"
#else
#define FLASH_NAME "spiflash"
#endif

#define BANDWIDTH 25000000


int bench_transfer_custom(int is_read, int is_2d, int size, int len, int stride, int l2_offset,int flash_offset, int periph_freq, int hyper_freq, int fc_freq, int *instr, int *load)
{
#if defined(USE_HYPERFLASH)
  struct pi_hyperflash_conf conf;
  pi_hyperflash_conf_init(&conf);
#elif defined(USE_MRAM)
  struct pi_mram_conf conf;
  pi_mram_conf_init(&conf);
#elif defined(USE_SPIFLASH)
#if defined(CONFIG_ATXP032)
  struct pi_atxp032_conf conf;
  pi_atxp032_conf_init(&conf);
  conf.baudrate = 25000000;
#else
  struct pi_spiflash_conf conf;
  pi_spiflash_conf_init(&conf);
  conf.baudrate = 25000000;
#endif
#endif
  int time;

  if (periph_freq != 0)
  {
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, periph_freq*1000000);
  }

  if (fc_freq != 0)
  {
    pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq*1000000);
  }

  pi_open_from_conf(&hyper, &conf);

  if (hyper_freq)
    conf.baudrate = hyper_freq * 1000000;

  if (pi_flash_open(&hyper))
    return -1;

  hyper_buff[0] = 0;

  if (load)
  {
    pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_ACTIVE_CYCLES));
  }
  else
  {
    pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR));
  }

  pi_task_t tasks[NB_ITER];

  for (int j=0; j<3; j++)
  {
    pi_perf_reset();
    pi_perf_start();

    if (is_2d)
    {
      for (int i=0; i<NB_ITER; i++)
      {
        pi_flash_copy_2d_async(&hyper, 0, buff[0] + l2_offset, size, stride, len, is_read, pi_task_block(&tasks[i]));
      }
    }
    else
    {
      for (int i=0; i<NB_ITER; i++)
      {
        pi_flash_copy_async(&hyper, 0 + flash_offset, buff[0] + l2_offset, size, is_read, pi_task_block(&tasks[i]));
      }
    }

    for (int i=0; i<NB_ITER; i++)
    {
      pi_task_wait_on(&tasks[i]);
    }
  }
  
  pi_perf_stop();

  if (instr)
    *instr = pi_perf_read(PI_PERF_INSTR) / NB_ITER;

  if (load)
  {
    *load = pi_perf_read(PI_PERF_ACTIVE_CYCLES) * 100 / pi_perf_read(PI_PERF_CYCLES);
  }

  time = (uint64_t)pi_perf_read(PI_PERF_CYCLES) * 1000000000 / pi_freq_get(PI_FREQ_DOMAIN_FC);

  int bandwidth = (uint64_t)size * 1000000000 * NB_ITER / time;


  pi_flash_close(&hyper);

  return bandwidth;
}

int bench_transfer(int is_read, int is_2d, int aligned, int full, int periph_freq, int hyper_freq, int fc_freq, int dump)
{
#if defined(USE_HYPERFLASH)
  struct pi_hyperflash_conf conf;
  pi_hyperflash_conf_init(&conf);
#elif defined(USE_MRAM)
  struct pi_mram_conf conf;
  pi_mram_conf_init(&conf);
#elif defined(USE_SPIFLASH)
  struct pi_spiflash_conf conf;
  pi_spiflash_conf_init(&conf);
#endif
  int size;
  int time;

  if (periph_freq != 0)
  {
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, periph_freq*1000000);
  }

  if (fc_freq != 0)
  {
    pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq*1000000);
  }

  pi_open_from_conf(&hyper, &conf);

  if (hyper_freq)
    conf.baudrate = hyper_freq * 1000000;

  if (pi_flash_open(&hyper))
    return -1;

  hyper_buff[0] = 0;

  pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR));

  for (int j=0; j<3; j++)
  {
    pi_perf_reset();
    pi_perf_start();

    if (is_2d)
    {
      for (int i=0; i<NB_ITER; i++)
      {
        if (aligned)
          pi_flash_copy_2d(&hyper, 0, buff[0], 24192, 31752, 756, is_read);
        else
          pi_flash_copy_2d(&hyper, 1, buff[0], 24192, 31752+3, 756+1, is_read);
        size = 24192;
      }
    }
    else
    {
      for (int i=0; i<NB_ITER; i++)
      {
        if (!full)
          size = 2;
        else
          size = 4096;
        if (aligned)
          pi_flash_copy(&hyper, 0, buff[0], size, is_read);
        else
          pi_flash_copy(&hyper, 0, buff[0]+1, size, is_read);
      }
    }
  }
  

  pi_perf_stop();

  time = (uint64_t)pi_perf_read(PI_PERF_CYCLES) * 1000000 / pi_freq_get(PI_FREQ_DOMAIN_FC);

  char test_name[128];
  char *type = is_read ? "read" : "write";

  if (dump)
  {
    if (is_2d)
    {
      if (aligned)
        sprintf(test_name, "Hyperflash 2D %s (aligned / size: %d / periph_freq: %d / hyper_freq: %d, fc_freq: %d)", type, size, periph_freq, hyper_freq, fc_freq);
      else
        sprintf(test_name, "Hyperflash 2D %s (misaligned / size: %d / periph_freq: %d / hyper_freq: %d, fc_freq: %d)", type, size, periph_freq, hyper_freq, fc_freq);
    }
    else
    {
      if (aligned)
        sprintf(test_name, "Hyperflash %s (aligned / size: %d / periph_freq: %d / hyper_freq: %d, fc_freq: %d)", type, size, periph_freq, hyper_freq, fc_freq);
      else
        sprintf(test_name, "Hyperflash %s (misaligned / size: %d / periph_freq: %d / hyper_freq: %d, fc_freq: %d)", type, size, periph_freq, hyper_freq, fc_freq);
    }

    printf("%-60s, FC instructions      , %d\n", test_name, pi_perf_read(PI_PERF_INSTR) / NB_ITER);
    printf("                                                            , FC cycles            , %d\n", pi_perf_read(PI_PERF_CYCLES) / NB_ITER);
    printf("                                                            , Time (us)            , %d\n", time / NB_ITER);
  }

  int bandwidth = size * 100 / (pi_perf_read(PI_PERF_CYCLES) / NB_ITER);

  if (dump)
    printf("                                                            , Bandwidth            , %d.%2.2d\n", bandwidth / 100, bandwidth % 100);

  int bandwidth_us = size * 100 * NB_ITER / time;

  if (dump)
    printf("                                                            , Bandwidth (byte/us)  , %d.%2.2d\n", bandwidth_us / 100, bandwidth_us % 100);


  pi_flash_close(&hyper);

  return bandwidth_us;
}

static int bench_find_bw(int is_read, int is_2d, int max_bw, int ratio, int size, int precision, int l2_offset, int flash_offset)
{
  int chunk = size/2;
  int sign = -1;
  int prev_ratio = 100;
  int transfer_size = size;

  while(1)
  {
    while((size + sign*chunk <= 0 || size + sign*chunk >= transfer_size) && chunk > 1)
      chunk /= 2;

    // Case where target bw cannot be reached
    if (size + sign*chunk >= transfer_size)
      return transfer_size;

    size = size + sign*chunk;

    int64_t bw;

    if (is_2d)
      bw = bench_transfer_custom(is_read, is_2d, transfer_size, size, 1024, l2_offset, flash_offset, 0, 0, 0, NULL, NULL);
    else
      bw = bench_transfer_custom(is_read, is_2d, size, 0, 0, l2_offset, flash_offset, 0, 0, 0, NULL, NULL);

    if (bw == -1)
      return -1;

    int current_ratio = bw*100 / max_bw;
    int crossed = (prev_ratio >= ratio && current_ratio <= ratio) || (prev_ratio <= ratio && current_ratio >= ratio);

    //printf("BENCH size %d got bw %d ratio %d\n", size, (int)bw, current_ratio);
    if (crossed)
    {
      if (chunk <= precision)
      {
        return size;
      }
      else
      {
        sign *= -1;
        chunk /= 2;
        //printf("Crossed sign %d chunk %d\n", sign, chunk);
      }
    }
        

    prev_ratio = current_ratio;

  }
}




int bench_transfers(int is_read, int is_2d, int l2_offset, int flash_offset, int full)
{
  char *name = is_read ?
    is_2d ? 
      l2_offset ? "misaligned_bandwidth_2d_read" : "bandwidth_2d_read" : 
      l2_offset ? "misaligned_bandwidth_1d_read" : "bandwidth_1d_read" :
    is_2d ? 
      l2_offset ? "misaligned_bandwidth_2d_write" : "bandwidth_2d_write" : 
      l2_offset ? "misaligned_bandwidth_1d_write" : "bandwidth_1d_write";

  printf("    Benchmarking %s\n", name);

  if (full)
  {
    if (is_2d)
    {
      for (int size = 4; size < 8192; size *= 2)
      {
        int load = 0;
        int bw = bench_transfer_custom(is_read, is_2d, 8192, size, 1024, l2_offset, flash_offset, 200, 25, 200, NULL, &load);
        printf("@BENCH@%s.bw_len_%d_%s=%f@DESC@Bandwidth (MByte/s with chunk size 8192, len %d)@\n", FLASH_NAME, size, name, (float)bw/1000000, size);
        printf("@BENCH@%s.load_len_%d_%s=%d@DESC@FC load (with chunk size 8192, len %d)@\n", FLASH_NAME, size, name, load, size);
      }
    }
    else
    {
      for (int size = 4; size <= 8192; size *= 2)
      {
        int load = 0;
        int bw = bench_transfer_custom(is_read, is_2d, size, size, 1024, l2_offset, flash_offset, 200, 25, 200, NULL, &load);
        printf("@BENCH@%s.bw_size_%d_%s=%f@DESC@Bandwidth (MByte/s size %d)@\n", FLASH_NAME, size, name, (float)bw/1000000,size);
        printf("@BENCH@%s.load_size_%d_%s=%d@DESC@FC load (size %d)@\n", FLASH_NAME, size, name, load, size);
      }
    }

  }
  else
  {
    if (is_2d)
    {
      int instr = 0;
      int max_bw_1d = BANDWIDTH;

      int bw_1d = bw_1d = bench_transfer_custom(is_read, is_2d, 8192, 1024, 1024, l2_offset, flash_offset, 0, 0, 200, &instr, NULL);
      if (bw_1d == -1)
        return -1;

      printf("        Got max bandwidth %d\n", bw_1d);

      int chunk_90 = bench_find_bw(is_read, is_2d, max_bw_1d, 90, 8192, 4, l2_offset, flash_offset);
      printf("        Got 90%% bandwidth at %d\n", chunk_90);
      if (chunk_90 == -1)
        return -1;

      int chunk_50 = bench_find_bw(is_read, is_2d, max_bw_1d, 50, 8192, 4, l2_offset, flash_offset);
      printf("        Got 50%% bandwidth at %d\n", chunk_50);
      if (chunk_50 == -1)
        return -1;

      int chunk_10 = bench_find_bw(is_read, is_2d, max_bw_1d, 10, 8192, 4, l2_offset, flash_offset);
      printf("        Got 10%% bandwidth at %d\n", chunk_10);
      if (chunk_10 == -1)
        return -1;

      printf("@BENCH@hyperflash.max_%s=%d@DESC@Maximum bandwidth (byte/s with chunk size 8192, len 1024)@\n", name, bw_1d);
      printf("@BENCH@hyperflash.max_%s_instr=%d@DESC@FC instructions at maximum bandwidth (for one chunk of 8192, len 1024)@\n", name, instr);
      printf("@BENCH@hyperflash.90_%s=%d@DESC@Len where bandwidth is 90%% of the ideal bandwidth (50MByte/s)@\n", name, chunk_90);
      printf("@BENCH@hyperflash.50_%s=%d@DESC@Len where bandwidth is 50%% of the ideal bandwidth (50MByte/s)@\n", name, chunk_50);
      printf("@BENCH@hyperflash.10_%s=%d@DESC@Len where bandwidth is 10%% of the ideal bandwidth (50MByte/s)@\n", name, chunk_10);
    }
    else
    {
      int instr = 0;
      int max_bw_1d = BANDWIDTH;

      int bw_1d = bench_transfer_custom(is_read, is_2d, 8192, 0, 0, l2_offset, flash_offset, 0, 0, 200, &instr, NULL);
      if (bw_1d == -1)
        return -1;

      printf("        Got max bandwidth %d\n", bw_1d);

      int chunk_90 = bench_find_bw(is_read, is_2d, max_bw_1d, 90, 8192, 4, l2_offset, flash_offset);
      printf("        Got 90%% bandwidth at %d\n", chunk_90);
      if (chunk_90 == -1)
        return -1;

      int chunk_50 = bench_find_bw(is_read, is_2d, max_bw_1d, 50, 8192, 4, l2_offset, flash_offset);
      printf("        Got 50%% bandwidth at %d\n", chunk_50);
      if (chunk_50 == -1)
        return -1;

      int chunk_10 = bench_find_bw(is_read, is_2d, max_bw_1d, 10, 8192, 4, l2_offset, flash_offset);
      printf("        Got 10%% bandwidth at %d\n", chunk_10);
      if (chunk_10 == -1)
        return -1;

      printf("@BENCH@hyperflash.max_%s=%d@DESC@Maximum bandwidth (byte/s with chunk size 8192)@\n", name, bw_1d);
      printf("@BENCH@hyperflash.max_%s_instr=%d@DESC@FC instructions at maximum bandwidth (for one chunk of 8192)@\n", name, instr);
      printf("@BENCH@hyperflash.90_%s=%d@DESC@Chunk size where bandwidth is 90%% of the ideal bandwidth (50MByte/s)@\n", name, chunk_90);
      printf("@BENCH@hyperflash.50_%s=%d@DESC@Chunk size where bandwidth is 50%% of the ideal bandwidth (50MByte/s)@\n", name, chunk_50);
      printf("@BENCH@hyperflash.10_%s=%d@DESC@Chunk size where bandwidth is 10%% of the ideal bandwidth (50MByte/s)@\n", name, chunk_10);
    }
  }
  
  return 0;
}



int test_entry()
{
  memset(buff[0], 0, BUFF_SIZE);

  //bench_transfer(1, 0, 1, 1, 00, 25, 0);
  //bench_transfer(1, 0, 1, 1, 100, 00, 0);
  //bench_transfer(1, 0, 1, 1, 100, 25, 0);
  //bench_transfer(1, 0, 1, 1, 200, 25, 0);
  //bench_transfer(1, 0, 1, 1, 200, 100, 0);
  //bench_transfer(1, 0, 1, 1, 200, 200, 0);
  //bench_transfer(1, 0, 1, 1, 200, 200, 200);
  //return 0;

  printf("Benchmark start\n");

  if (bench_transfers(1, 0, 0, 0, FULL_PLOT))
    return -1;
  if (bench_transfers(1, 0, 1, 0, FULL_PLOT))
    return -1;
  if (bench_transfers(1, 1, 0, 0, FULL_PLOT))
    return -1;
  if (bench_transfers(1, 1, 1, 0, FULL_PLOT))
    return -1;

  if (bench_transfers(0, 0, 0, 0, FULL_PLOT))
    return -1;
  if (bench_transfers(0, 0, 1, 0, FULL_PLOT))
    return -1;
  if (bench_transfers(0, 1, 0, 0, FULL_PLOT))
    return -1;
  if (bench_transfers(0, 1, 1, 0, FULL_PLOT))
    return -1;

  return 0;




  bench_transfer(1, 0, 1, 1, 0, 0, 0, 1);
  bench_transfer(1, 0, 0, 1, 0, 0, 0, 1);
  bench_transfer(1, 1, 1, 1, 0, 0, 0, 1);
  bench_transfer(1, 1, 0, 1, 0, 0, 0, 1);
  bench_transfer(1, 0, 1, 0, 0, 0, 0, 1);
  bench_transfer(1, 0, 1, 1, 0, 50, 0, 1);
  return 0;
  //bench_transfer(1, 0, 1, 1, 200, 200, 0);
  //bench_transfer(1, 0, 1, 1, 100, 100, 0);
  //bench_transfer(1, 0, 1, 1, 100, 50, 0);
  //bench_transfer(1, 0, 1, 1, 100, 25, 0);
  //bench_transfer(1, 0, 1, 1, 100, 10, 0);
  //bench_transfer(1, 0, 1, 1, 100, 0, 0);
  //bench_transfer(1, 0, 1, 0, 100, 0, 0);
  //bench_transfer(1, 0, 1, 1, 10, 0, 0);
  //bench_transfer(1, 0, 1, 0, 10, 0, 0);
  printf("Benchmark stop\n");
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
