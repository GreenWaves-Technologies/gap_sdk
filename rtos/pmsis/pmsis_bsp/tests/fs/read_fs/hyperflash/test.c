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
#include "bsp/fs.h"
#include "bsp/flash/hyperflash.h"
#include "stdio.h"

#define BUFF_SIZE 1024
#define COUNT 2

static PI_L2 char buff[2][BUFF_SIZE];
static int count_done = 0;
static fs_file_t *file[2];

static void end_of_rx(void *arg)
{
  int index = (int)arg;
  printf("End of RX for id %d\n", index);
  count_done++;
}

#if 0
static void handle_error()
{
  int error = error_current();
  printf("Caught error (error code 0x%x): %s\n", error, error_str(error));
  exit(-1);
}

static void handle_async_error(void *arg, event_t *event, int error, void *object)
{
  printf("Received error (error code 0x%x, event %p, object: %p): %s\n", error, event, object, error_str(error));
  exit(-1);
}

#endif



static int exec_tests()
{
  int errors = 0;

#ifdef USE_CLUSTER
  cl_fs_req_t req0, req1;

  cl_fs_read(file[0], buff[0], BUFF_SIZE, &req0);
  cl_fs_read(file[1], buff[1], BUFF_SIZE, &req1);

  errors += cl_fs_wait(&req0) != BUFF_SIZE;
  errors += cl_fs_wait(&req1) != BUFF_SIZE;;

#else
  pi_task_t task0, task1;

  fs_read_async(file[0], buff[0], BUFF_SIZE, pi_task_callback(&task0, end_of_rx, (void *)0));
  fs_read_async(file[1], buff[1], BUFF_SIZE, pi_task_callback(&task1, end_of_rx, (void *)1));

  while(count_done != COUNT) {
    pi_yield();
  }

#endif

  return errors;
}



static void cluster_entry(void *arg)
{
  int *errors = (int *)arg;

  *errors = exec_tests();

}

#ifdef USE_CLUSTER
static int exec_tests_on_cluster()
{
  printf("Exec test on cluster\n");

  int errors = 0;

  struct pi_device cluster_dev;
  struct cluster_driver_conf conf;
  struct pi_cluster_task cluster_task;

  conf.id = 0;

  pi_open_from_conf(&cluster_dev, &conf);

  pi_cluster_open(&cluster_dev);

  pi_cluster_task(&cluster_task, cluster_entry, (void *)&errors);

  pi_cluster_send_task_to_cl(&cluster_dev, &cluster_task);

  return errors;
}
#endif



static int test_entry()
{

  struct pi_device fs;
  struct pi_device flash;

  printf("Entering main controller\n");

  //error_conf(NULL, handle_async_error, NULL);

  struct fs_conf conf;
  fs_conf_init(&conf);
  struct hyperflash_conf flash_conf;

  hyperflash_conf_init(&flash_conf);

  pi_open_from_conf(&flash, &flash_conf);

  if (flash_open(&flash))
    return -1;


  conf.flash = &flash;

  pi_open_from_conf(&fs, &conf);

  if (fs_mount(&fs))
    return -2;

  file[0] = fs_open(&fs, "flash_file_0.bin", 0);
  if (file[0] == NULL) return -3;

  file[1] = fs_open(&fs, "flash_file_1.bin", 0);
  if (file[1] == NULL) return -4;

#ifdef USE_CLUSTER
  if (exec_tests_on_cluster())
    return -5;
#else
  if (exec_tests())
    return -5;
#endif

  for (int j=0; j<2; j++)
  {
    for (int i=0; i<BUFF_SIZE; i++)
    {
      unsigned char expected;
      if (j == 0)
        expected = i & 0x7f;
      else
        expected = i | 0x80;
      if (expected != buff[j][i])
      {
        printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, buff[j][i]);
        return -6;
      }
    }
  }

  fs_unmount(&fs);

  printf("Test success\n");

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
