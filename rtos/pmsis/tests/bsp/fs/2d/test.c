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
#include <bsp/bsp.h>
#include "stdio.h"

#ifdef FLASH_TYPE
#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif
#endif

#if defined(FS_HOST)
#include <bsp/fs/hostfs.h>

#elif defined(FS_READ_FS)
#include <bsp/fs/readfs.h>

#elif defined(FS_LFS)
#include <bsp/fs/pi_lfs.h>

#else
#error "Unknown FS"
#endif

#define HYPER_FLASH 0
#define SPI_FLASH   1
#define MRAM        2

#define BUFF_SIZE 1024
#define COUNT 2

#define OFFSET 16
#define STRIDE 32
#define LENGTH 16

static PI_L2 char buff[2][BUFF_SIZE];
static int count_done = 0;
static pi_fs_file_t *file[2];

#ifdef __PULP_OS__
static struct pi_pulpos_conf os_conf;
#endif

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
  pi_cl_fs_req_t req0, req1;

  pi_cl_fs_copy_2d(file[0], OFFSET, buff[0], BUFF_SIZE, STRIDE, LENGTH, 1, &req0);
  pi_cl_fs_copy_2d(file[1], OFFSET, buff[1], BUFF_SIZE, STRIDE, LENGTH, 1, &req1);

  pi_cl_fs_wait(&req0);
  pi_cl_fs_wait(&req1);

#else
  pi_task_t task0, task1;

  pi_fs_copy_2d_async(file[0], OFFSET, buff[0], BUFF_SIZE, STRIDE, LENGTH, 1, pi_task_callback(&task0, end_of_rx, (void *)0));
  pi_fs_copy_2d_async(file[1], OFFSET, buff[1], BUFF_SIZE, STRIDE, LENGTH, 1, pi_task_callback(&task1, end_of_rx, (void *)1));

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
  struct pi_cluster_conf conf;
  struct pi_cluster_task cluster_task;

  pi_cluster_conf_init(&conf);

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

#if defined(FS_HOST)
  printf("Starting test (type: host)\n");
#elif defined(FS_READ_FS)
  printf("Starting test (type: read_fs)\n");
#elif defined(FS_LFS)
  printf("Starting test (type: lfs)\n");
#endif

  //error_conf(NULL, handle_async_error, NULL);

#if defined(FS_HOST)

  struct pi_hostfs_conf conf;
  pi_hostfs_conf_init(&conf);

#elif defined(FS_READ_FS)

  struct pi_readfs_conf conf;
  pi_readfs_conf_init(&conf);

#elif defined(FS_LFS)

  struct pi_lfs_conf conf;
  pi_lfs_conf_init(&conf);

#endif

#if !defined(FS_HOST)
#ifdef FLASH_TYPE
#ifdef USE_HYPERFLASH
struct pi_hyperflash_conf flash_conf;
#elif USE_MRAM
struct pi_mram_conf flash_conf;
#else
#if defined(CONFIG_ATXP032)
struct pi_atxp032_conf flash_conf;
#else
struct pi_spiflash_conf flash_conf;
#endif
#endif

#ifdef USE_HYPERFLASH
  pi_hyperflash_conf_init(&flash_conf);
#elif USE_MRAM
  pi_mram_conf_init(&flash_conf);
#else
#if defined(CONFIG_ATXP032)
  pi_atxp032_conf_init(&flash_conf);
#else
  pi_spiflash_conf_init(&flash_conf);
#endif
#endif

#else
  struct pi_default_flash_conf flash_conf;
  pi_default_flash_conf_init(&flash_conf);
#endif

  pi_open_from_conf(&flash, &flash_conf);

  if (pi_flash_open(&flash))
    return -1;
#endif

  conf.fs.flash = &flash;

  pi_open_from_conf(&fs, &conf);

  if (pi_fs_mount(&fs))
    return -2;

  file[0] = pi_fs_open(&fs, "flash_file_0.bin", 0);
  if (file[0] == NULL) return -3;

  file[1] = pi_fs_open(&fs, "flash_file_1.bin", 0);
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

      unsigned int index = (i / LENGTH) * STRIDE + (i % LENGTH) + OFFSET;

      if (j == 0)
        expected = index & 0x7f;
      else
        expected = index | 0x80;
      if (expected != buff[j][i])
      {
        printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, buff[j][i]);
        return -6;
      }
    }
  }

  pi_fs_unmount(&fs);

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
