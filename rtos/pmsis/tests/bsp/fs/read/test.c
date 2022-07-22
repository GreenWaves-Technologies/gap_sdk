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

#define HYPER_FLASH 0
#define SPI_FLASH   1
#define MRAM        2

#if defined(FS_HOST)
#include <bsp/fs/hostfs.h>

#elif defined(FS_READ_FS)
#include <bsp/fs/readfs.h>

#elif defined(FS_LFS)
#include <bsp/fs/pi_lfs.h>

#else
#error "Unknown FS"
#endif


#define BUFF_SIZE 1024
#define COUNT 2

static PI_L2 char buff[2][BUFF_SIZE];
static int count_done = 0;
static pi_fs_file_t *file[2];
static struct pi_device os;
static struct pi_device fs;
static struct pi_device flash;
#ifdef FLASH_TYPE
#ifdef USE_HYPERFLASH
struct pi_hyperflash_conf flash_conf;
#elif USE_MRAM
struct pi_mram_conf flash_conf;
#else
#if defined(CONFIG_ATXP032)
struct pi_atxp032_conf flash_conf;
#else
#if defined(CONFIG_MX25U51245G)
  struct pi_mx25u51245g_conf flash_conf;
#else
static struct pi_spiflash_conf flash_conf;
#endif
#endif
#endif
#else
static struct pi_default_flash_conf flash_conf;
#endif
static struct pi_device cluster_dev;
static struct pi_cluster_conf cluster_conf;
static struct pi_cluster_task cluster_task;
static pi_task_t task0, task1;


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

  pi_cl_fs_read(file[0], buff[0], BUFF_SIZE, &req0);
  pi_cl_fs_read(file[1], buff[1], BUFF_SIZE, &req1);

  pi_cl_fs_wait(&req0);
  pi_cl_fs_wait(&req1);

#else

#ifdef FS_LFS

  pi_fs_read(file[0], buff[0], BUFF_SIZE);
  pi_fs_read(file[1], buff[1], BUFF_SIZE);

#else
  pi_fs_read_async(file[0], buff[0], BUFF_SIZE, pi_task_callback(&task0, end_of_rx, (void *)0));
  pi_fs_read_async(file[1], buff[1], BUFF_SIZE, pi_task_callback(&task1, end_of_rx, (void *)1));

  while(count_done != COUNT) {
    pi_yield();
  }
#endif

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

  pi_cluster_conf_init(&cluster_conf);

  cluster_conf.id = 0;

  pi_open_from_conf(&cluster_dev, &cluster_conf);

  pi_cluster_open(&cluster_dev);

  pi_cluster_task(&cluster_task, cluster_entry, (void *)&errors);

  pi_cluster_send_task_to_cl(&cluster_dev, &cluster_task);

  return errors;
}
#endif


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

static int test_entry()
{

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
  pi_hyperflash_conf_init(&flash_conf);
#elif USE_MRAM
  pi_mram_conf_init(&flash_conf);
#else
#if defined(CONFIG_ATXP032)
  pi_atxp032_conf_init(&flash_conf);
#else
#if defined(CONFIG_MX25U51245G)
  pi_mx25u51245g_conf_init(&flash_conf);
#else
  pi_spiflash_conf_init(&flash_conf);
#endif
#endif
#endif
#else
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

  file[0] = pi_fs_open(&fs, STR(FILE0), 0);
  if (file[0] == NULL) return -3;

  file[1] = pi_fs_open(&fs, STR(FILE1), 0);
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
