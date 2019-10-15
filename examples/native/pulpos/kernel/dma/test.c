/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "rt/rt_api.h"
#include "stdio.h"

#define BUFF_SIZE 2048

static char l2_buff0[BUFF_SIZE];
static char l2_buff1[BUFF_SIZE];

static rt_event_t * dma_finished_event;

static void parallel_operation(void *arg)
{
  char * cluster_l1_buff = (char *) arg;
  int start = rt_core_id() * BUFF_SIZE / 8;
  int end = start - 1 + BUFF_SIZE/8;
  printf("(%d, %d) Task %d - %d\n", rt_cluster_id(), rt_core_id(), start, end);
  for (int i = start; i <= end; i++)
  {
    cluster_l1_buff[i] = (char)(cluster_l1_buff[i] * 3);
  }
}

static void cluster_entry(void *arg)
{
  // Retrieve the pointer to the allocated memory
  char * cluster_l1_buff = (char *) arg;

  printf("(%d, %d) Entered cluster\n", rt_cluster_id(), rt_core_id());

  // copy from L2 to shared L1
  rt_dma_copy_t dmaCp;
  rt_dma_memcpy((int)l2_buff0, (int)cluster_l1_buff, BUFF_SIZE, RT_DMA_DIR_EXT2LOC, 0, &dmaCp);

  // Wait for the operation to finish
  rt_dma_wait(&dmaCp);

  // Start all the cores up
  rt_team_fork(8, parallel_operation, cluster_l1_buff);

  // Copy from shared L1 to L2
  rt_dma_memcpy((int)l2_buff1, (int)cluster_l1_buff, BUFF_SIZE, RT_DMA_DIR_LOC2EXT, 0, &dmaCp);

  // Wait for it to finish
  rt_dma_wait(&dmaCp);
}

int main()
{
  printf("Entering main controller\n");

  // Allocate events on the default scheduler
  if (rt_event_alloc(NULL, 4)) return -1;

  // Initialize the buffer in L2 memory
  for (int i=0; i<BUFF_SIZE; i++)
  {
    l2_buff0[i] = i;
  }

  // Power on the cluster
  rt_cluster_mount(1, 0, 0, NULL);

  // Allocate a buffer in the shared L1 memory
  char * cluster_l1_buff = rt_alloc(RT_ALLOC_CL_DATA, BUFF_SIZE);

  // Call the cluster with the buffer address as an argument. Block until finished.
  rt_cluster_call(NULL, 0, cluster_entry, cluster_l1_buff, NULL, 0, 0, rt_nb_pe(), NULL);

  // Free the buffer
  rt_free(RT_ALLOC_CL_DATA, cluster_l1_buff, BUFF_SIZE);

  // Switch off the cluster
  rt_cluster_mount(0, 0, 0, NULL);

  // Verify that the operation occurred
  for (int i=0; i<BUFF_SIZE; i++)
  {
    if (l2_buff1[i] != (char)(i * 3)) {
      printf("Test failed: ERROR at index %d: expecting 0x%x, got 0x%x\n", i, i*3, l2_buff1[i]);
      return -1;
    }
  }

  printf("Test success\n");

  return 0;
}
