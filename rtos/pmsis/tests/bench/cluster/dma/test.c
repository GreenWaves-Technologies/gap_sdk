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

#include <stdio.h>
#include "pmsis.h"

#define NB_ITER 16
#define MAX_SIZE (16384)
#define L2_MAX_SIZE (16384*8)

static PI_L1 char *l1_buff[2], *l2_buff[2];
static PI_L1 pi_cl_dma_cmd_t copy[NB_ITER];


static void launch_dma_bench(int l2_to_l1, int both, int size, int ext_align, int loc_align, int nb_iter, int dump)
{
  pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR));
  pi_perf_reset();
  pi_perf_start();

  if (both)
  {
    for (int i=0; i<NB_ITER; i++)
    {
      pi_cl_dma_cmd_t copy0, copy1;
      pi_cl_dma_cmd((unsigned int)(l2_buff[0]+ext_align), (unsigned int)(l1_buff[0]+loc_align), size, PI_CL_DMA_DIR_LOC2EXT, &copy0);
      pi_cl_dma_cmd((unsigned int)(l2_buff[1]+ext_align), (unsigned int)(l1_buff[1]+loc_align), size, PI_CL_DMA_DIR_EXT2LOC, &copy1);
      pi_cl_dma_cmd_wait(&copy0);
      pi_cl_dma_cmd_wait(&copy1);
    }
  }
  else
  {
    for (int i=0; i<nb_iter; i++)
    {
      pi_cl_dma_cmd((unsigned int)(l2_buff[0]+ext_align), (unsigned int)(l1_buff[0]+loc_align), size, l2_to_l1 ? PI_CL_DMA_DIR_EXT2LOC : PI_CL_DMA_DIR_LOC2EXT, &copy[i]);
      pi_cl_dma_cmd_wait(&copy[i]);
    }
  }

  pi_perf_stop();

  int factor = 1;
  if (both)
    factor *= 2;

  if (dump)
  {
  char test_name[128];

  sprintf(test_name, "DMA transfer (direction: %s, size: %d, ext_align: %d, loc_align: %d)", both ? "both" : l2_to_l1 ? "L2 to L1" : "L1 to L2", size, ext_align, loc_align);

  //printf("Instructions per transfer: %d\n", pi_perf_read(PI_PERF_INSTR) / factor / nb_iter);


  printf("%s\n    CL instructions      , %d\n", test_name, pi_perf_read(PI_PERF_INSTR) / nb_iter);
  printf("    CL cycles            , %d\n", pi_perf_read(PI_PERF_CYCLES) / nb_iter);

  int bandwidth = size * factor * nb_iter * 100 / pi_perf_read(PI_PERF_CYCLES);
  printf("    Bandwidth            , %d.%2.2d\n", bandwidth / 100, bandwidth % 100);
  }

  // for (int i=0; i<nb_iter; i++)
  // {
  //   pi_cl_dma_cmd_wait(&copy[i]);
  // }
} 


static void launch_dma_2d_bench(int l2_to_l1, int both, int size, int stride, int length, int ext_align, int loc_align, int nb_iter, int dump)
{
  pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR));
  pi_perf_reset();
  pi_perf_start();

  if (both)
  {
    for (int i=0; i<nb_iter; i++)
    {
      pi_cl_dma_cmd_t copy0, copy1;
      pi_cl_dma_cmd_2d((unsigned int)(l2_buff[0]+ext_align), (unsigned int)(l1_buff[0]+loc_align), size, stride, length, PI_CL_DMA_DIR_LOC2EXT, &copy0);
      pi_cl_dma_cmd_2d((unsigned int)(l2_buff[1]+ext_align), (unsigned int)(l1_buff[1]+loc_align), size, stride, length, PI_CL_DMA_DIR_EXT2LOC, &copy1);
      pi_cl_dma_cmd_wait(&copy0);
      pi_cl_dma_cmd_wait(&copy1);
    }
  }
  else
  {
    for (int i=0; i<nb_iter; i++)
    {
      pi_cl_dma_cmd_t copy;
      pi_cl_dma_cmd_2d((unsigned int)(l2_buff[0]+ext_align), (unsigned int)(l1_buff[0]+loc_align), size, stride, length, l2_to_l1 ? PI_CL_DMA_DIR_EXT2LOC : PI_CL_DMA_DIR_LOC2EXT, &copy);
      pi_cl_dma_cmd_wait(&copy);
    }
  }

  pi_perf_stop();

  int factor = 1;
  if (both)
    factor *= 2;

  char test_name[128];

  sprintf(test_name, "DMA 2D transfer (direction: %s, size: %d, stride: %d, len: %d, ext_align: %d, loc_align: %d)", both ? "both" : l2_to_l1 ? "L2 to L1" : "L1 to L2", size, stride, length, ext_align, loc_align);

  //printf("Instructions per transfer: %d\n", pi_perf_read(PI_PERF_INSTR) / factor / nb_iter);


  printf("%s\n    CL instructions      , %d\n", test_name, pi_perf_read(PI_PERF_INSTR) / nb_iter);
  printf("    CL cycles            , %d\n", pi_perf_read(PI_PERF_CYCLES) / nb_iter);

  int bandwidth = size * factor * nb_iter * 100 / pi_perf_read(PI_PERF_CYCLES);
  printf("    Bandwidth            , %d.%2.2d\n", bandwidth / 100, bandwidth % 100);
} 

static void cluster_entry(void *arg)
{
  printf("Benchmark start\n");

  launch_dma_2d_bench(1, 1, 1024, 64, 32, 0, 0, NB_ITER, 1);
  launch_dma_2d_bench(1, 1, 1024, 64, 32, 1, 0, NB_ITER, 1);
  launch_dma_2d_bench(1, 1, 1024, 64, 32, 1, 3, NB_ITER, 1);
  launch_dma_2d_bench(1, 1, 1024, 0x7FFF, 256, 0, 0, NB_ITER, 1);
  launch_dma_2d_bench(1, 1, 1024, 0x8000, 256, 0, 0, NB_ITER, 1);

  launch_dma_2d_bench(1, 0, 1024, 64, 32, 0, 0, NB_ITER, 1);
  launch_dma_2d_bench(1, 0, 1024, 64, 32, 1, 0, NB_ITER, 1);
  launch_dma_2d_bench(1, 0, 1024, 64, 32, 1, 3, NB_ITER, 1);
  launch_dma_2d_bench(1, 0, 1024, 0x7FFF, 256, 0, 0, NB_ITER, 1);
  launch_dma_2d_bench(1, 0, 1024, 0x8000, 256, 0, 0, NB_ITER, 1);

  launch_dma_bench(1, 0, 16   , 0, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 64   , 0, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 256  , 0, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 1024 , 0, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 4096 , 0, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 16384, 0, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 16384, 1, 0, NB_ITER, 1);
  launch_dma_bench(1, 0, 16384, 1, 3, NB_ITER, 1);

  launch_dma_bench(0, 0, 16   , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 64   , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 256  , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 1024 , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 4096 , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 16384, 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 16384, 1, 0, NB_ITER, 1);
  launch_dma_bench(0, 0, 16384, 1, 3, NB_ITER, 1);

  launch_dma_bench(0, 1, 1024 , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 1, 2048 , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 1, 4096 , 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 1, 16384, 0, 0, NB_ITER, 1);
  launch_dma_bench(0, 1, 16384, 1, 0, NB_ITER, 1);
  launch_dma_bench(0, 1, 16384, 1, 3, NB_ITER, 1);

  printf("Benchmark stop\n");
}

static int test_entry()
{
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;

  pi_cluster_conf_init(&conf);

  conf.id = 0;
    
  pi_open_from_conf(&cluster_dev, &conf);
  pi_cluster_open(&cluster_dev);

  l1_buff[0] = pmsis_l1_malloc(MAX_SIZE);
  l1_buff[1] = pmsis_l1_malloc(MAX_SIZE);
  l2_buff[0] = pmsis_l2_malloc(L2_MAX_SIZE);
  l2_buff[1] = pmsis_l2_malloc(L2_MAX_SIZE);
  if (l1_buff[0] == NULL || l1_buff[1] == NULL || l2_buff[0] == NULL || l2_buff[1] == NULL)
    return -1;

  struct pi_cluster_task task;

  pi_cluster_task(&task, cluster_entry, NULL);

  pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&task, cluster_entry, NULL));

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
