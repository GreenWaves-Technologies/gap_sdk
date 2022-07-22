/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "stdio.h"
#include "pmsis/cluster/dma/cl_dma.h"

#if STRIDE >= 0x10000
#define BUFF_SIZE (STRIDE*2)
#elif STRIDE >= 0x8000
#define BUFF_SIZE (STRIDE*4)
#elif STRIDE >= 0x4000
#define BUFF_SIZE (STRIDE*6)
#else
#define BUFF_SIZE (STRIDE*8)
#endif

#define LENGTH 300

#define COPY_SIZE (BUFF_SIZE/STRIDE*LENGTH)

static char *ext_buff0;
static char *ext_buff1;
static char *loc_buff;

static void cluster_entry(void *arg)
{
  //printf("(%d, %d) Entered cluster got ptr %p\n", cl_cluster_id(), cl_core_id(), loc_buff);

  pi_cl_dma_cmd_t copy;

  pi_cl_dma_cmd_2d((int)ext_buff0, (int)loc_buff, COPY_SIZE, STRIDE, LENGTH, PI_CL_DMA_DIR_EXT2LOC, &copy);

  pi_cl_dma_cmd_wait(&copy);

  for (int i=0; i<COPY_SIZE; i++)
  {
    loc_buff[i] = (char )(loc_buff[i] * 3);
  }

  pi_cl_dma_cmd((int)ext_buff1, (int)loc_buff, COPY_SIZE, PI_CL_DMA_DIR_LOC2EXT, &copy);

  pi_cl_dma_cmd_wait(&copy);
}

static int test_entry()
{

  printf("Entering main controller\n");

#if 1 //def ARCHI_HAS_FC

  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task cluster_task;
  struct pi_task task;

  ext_buff0 = pi_l2_malloc(BUFF_SIZE);
  ext_buff1 = pi_l2_malloc(BUFF_SIZE);
  if (ext_buff0 == NULL || ext_buff1 == NULL)
    return -1;

  pi_cluster_conf_init(&conf);

  pi_open_from_conf(&cluster_dev, &conf);
    
  pi_cluster_open(&cluster_dev);

  loc_buff = pmsis_l1_malloc(COPY_SIZE);
  if (loc_buff == NULL)
    return -1;

  for (int i=0; i<BUFF_SIZE; i++)
  {
    if ((i % STRIDE) < LENGTH)
      ext_buff0[i] = i / STRIDE * LENGTH + (i % STRIDE);
    else
      ext_buff0[i] = 0;
  }
  pi_cluster_task(&cluster_task, cluster_entry, NULL);

  pi_cluster_send_task_to_cl(&cluster_dev, &cluster_task);

  for (int i=0; i<COPY_SIZE; i++)
  {
    if (ext_buff1[i] != (char)(i * 3)) {
      printf("ERROR at index %d / addr %p: expecting 0x%x, got 0x%x\n", i, &ext_buff1[i], (char)(i * 3), ext_buff1[i]);
      return -1;
    }
  }

  pi_cluster_close(&cluster_dev);

#else

  loc_buff = pmsis_l1_malloc(BUFF_SIZE);

  for (int i=0; i<BUFF_SIZE; i++)
  {
    if ((i % 32) < 16)
      ext_buff0[i] = i / 32 * 16 + (i % 32);
    else
      ext_buff0[i] = 0;
  }

  cluster_entry(NULL);
  
  for (int i=0; i<COPY_SIZE; i++)
  {
    if (ext_buff1[i] != (char)(i * 3)) {
      printf("ERROR at index %d / addr %p: expecting 0x%x, got 0x%x\n", i, &ext_buff1[i], i*3, ext_buff1[i]);
      return -1;
    }
  }

#endif

  printf("TEST SUCCESS\n");

  return 0;
}

static void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
