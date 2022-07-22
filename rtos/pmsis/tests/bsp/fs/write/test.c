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

#if defined(FS_HOST)
#include <bsp/fs/hostfs.h>

#elif defined(FS_READ_FS)
#include <bsp/fs/readfs.h>

#elif defined(FS_LFS)
#include <bsp/fs/pi_lfs.h>

#else
#error "Unknown FS"
#endif

#define BUFF_SIZE (16*1024)
#define ITER_SIZE 1024
#define COUNT 2

static PI_L2 char buff[2][ITER_SIZE];
static pi_fs_file_t *file[2];
static pi_fs_file_t *tx_file[2];
static int done;
static int wrote_size[2];
static pi_task_t task[2];

static PI_L2 char check_buff[BUFF_SIZE];

static void end_of_rx(void *arg);

static void end_of_tx(void *arg)
{
  int index = (int)arg;

  wrote_size[index] += ITER_SIZE;

  if (wrote_size[0] == BUFF_SIZE && wrote_size[1] == BUFF_SIZE)
  {
    done = 1;
  }
  else
  {
    if (wrote_size[index] < BUFF_SIZE)
      pi_fs_read_async(file[index], buff[index], ITER_SIZE, pi_task_callback(&task[index], end_of_rx, (void *)index));
  }
}

static void end_of_rx(void *arg)
{
  int index = (int)arg;
  pi_fs_write_async(tx_file[index], buff[index], ITER_SIZE, pi_task_callback(&task[index], end_of_tx, (void *)index));
}

static int exec_tests()
{
  pi_task_t task0, task1;

  done = 0;
  wrote_size[0] = 0;
  wrote_size[1] = 0;

#ifdef FS_LFS

  for(int i=0; i<(BUFF_SIZE/ITER_SIZE); i++)
  {
      pi_fs_read(file[0], buff[0], ITER_SIZE);
      pi_fs_write(tx_file[0], buff[0], ITER_SIZE);
      pi_fs_read(file[1], buff[1], ITER_SIZE);
      pi_fs_write(tx_file[1], buff[1], ITER_SIZE);
  }

#else
  pi_fs_read_async(file[0], buff[0], ITER_SIZE, pi_task_callback(&task[0], end_of_rx, (void *)0));
  pi_fs_read_async(file[1], buff[1], ITER_SIZE, pi_task_callback(&task[1], end_of_rx, (void *)1));

  while(!done) {
    pi_yield();
  }
#endif

  return 0;
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


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)


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

  struct pi_hyperflash_conf flash_conf;

  pi_hyperflash_conf_init(&flash_conf);

  pi_open_from_conf(&flash, &flash_conf);

  if (pi_flash_open(&flash))
    return -1;


  conf.fs.flash = &flash;

  pi_open_from_conf(&fs, &conf);

  if (pi_fs_mount(&fs))
    return -2;

  file[0] = pi_fs_open(&fs, STR(FILE0), PI_FS_FLAGS_READ);
  if (file[0] == NULL) return -3;

  file[1] = pi_fs_open(&fs, STR(FILE1), PI_FS_FLAGS_READ);
  if (file[1] == NULL) return -4;

  tx_file[0] = pi_fs_open(&fs, STR(TX_FILE0), PI_FS_FLAGS_WRITE);
  if (tx_file[0] == NULL) return -3;

  tx_file[1] = pi_fs_open(&fs, STR(TX_FILE1), PI_FS_FLAGS_WRITE);
  if (tx_file[1] == NULL) return -4;

#ifdef USE_CLUSTER
  if (exec_tests_on_cluster())
    return -5;
#else
  if (exec_tests())
    return -5;
#endif

  // Close file to wrote data on flash.
  pi_fs_close(tx_file[0]);
  pi_fs_close(tx_file[1]);
  
  for (int j=0; j<2; j++)
  {

    pi_fs_file_t *file;

    if (j == 0)
      file = pi_fs_open(&fs, STR(TX_FILE0), PI_FS_FLAGS_READ);
    else
      file = pi_fs_open(&fs, STR(TX_FILE1), PI_FS_FLAGS_READ);

    pi_fs_read(file, check_buff, BUFF_SIZE);

    for (int i=0; i<BUFF_SIZE; i++)
    {
      unsigned char expected;
      if (j == 0)
        expected = i & 0x7f;
      else
        expected = i | 0x80;
      if (expected != check_buff[i])
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
