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
#include "bsp/bsp.h"
#include "bsp/fs.h"
#include <bsp/fs/readfs.h>
#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif

#include "pmsis/cluster/cluster_team/cl_team.h"

#define STACK_SIZE 2048

#define BUFF_SIZE 1024

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

static int nb_fork;
static int nb_callback_exec;

#ifdef USE_HYPERFLASH
struct pi_hyperflash_conf flash_conf;
#elif USE_MRAM
struct pi_mram_conf flash_conf;
#else
#if defined(CONFIG_ATXP032)
struct pi_atxp032_conf flash_conf;
#else
static struct pi_spiflash_conf flash_conf;
#endif
#endif
static struct pi_device fs;
static struct pi_device flash;

static PI_L2 char buff[BUFF_SIZE];

static void pe_entry(void *arg)
{
  int *var = (int *)arg;
  pi_cl_team_critical_enter();
  *var |= 1 << (pi_core_id());
  pi_cl_team_critical_exit();
}

static int check_fork(int nb_cores)
{
  unsigned int var = 0;
  nb_fork++;
  pi_cl_team_fork(nb_cores, pe_entry, (void *)&var);
  return (1<<pi_cl_team_nb_cores()) - 1 != var;
}

static void cluster_entry(void *arg)
{
  int *errors = arg;
  unsigned int core_mask = 0;

  pi_cl_team_fork(pi_cl_cluster_nb_pe_cores(), pe_entry, &core_mask);

  *errors = core_mask != ((1<<pi_cl_cluster_nb_pe_cores()) - 1);
}

static int do_tests()
{
    struct pi_device cluster_dev;
    pi_task_t task, task_fs;
    int errors = 0;
    int cluster_errors = 0;
    pi_fs_file_t *file;
  
    // First launch some processing on cluster side
    {
        struct pi_cluster_conf conf;
        struct pi_cluster_task cluster_task;

        pi_cluster_conf_init(&conf);
        pi_open_from_conf(&cluster_dev, &conf);
        pi_cluster_open(&cluster_dev);

        pi_cluster_send_task_to_cl_async(&cluster_dev, pi_cluster_task(&cluster_task, cluster_entry, (void *)&cluster_errors), pi_task_block(&task));
    }

    // Then read a file from flash
    {
        struct pi_readfs_conf conf;
        pi_readfs_conf_init(&conf);

#ifdef USE_HYPERFLASH
        pi_hyperflash_conf_init(&flash_conf);
        flash_conf.hyper_cs = 0;
        flash_conf.hyper_itf = 0;

#elif USE_MRAM
        pi_mram_conf_init(&flash_conf);
#else
#if defined(CONFIG_ATXP032)
        pi_atxp032_conf_init(&flash_conf);
        flash_conf.spi_cs = 0;
        flash_conf.spi_itf = 0;

#else
        pi_spiflash_conf_init(&flash_conf);
#endif
#endif

        pi_open_from_conf(&flash, &flash_conf);

        if (pi_flash_open(&flash))
          return -1;

        conf.fs.flash = &flash;

        pi_open_from_conf(&fs, &conf);

        if (pi_fs_mount(&fs))
          return -2;

        file = pi_fs_open(&fs, STR(FILE_PATH), 0);
        if (file == NULL) return -1;

        pi_fs_read_async(file, buff, BUFF_SIZE, pi_task_block(&task_fs));
    }

    // Wait for end of cluster processing
    {
        pi_task_wait_on(&task);
        pi_cluster_close(&cluster_dev);
        errors += cluster_errors;
    }

    // Wait for end of fs read
    {
        pi_task_wait_on(&task_fs);
        pi_fs_close(file);
        pi_fs_unmount(&fs);
        pi_flash_close(&flash);
    }

  return errors;
}


int test_entry()
{
  int errors = 0;

  printf("Starting test\n");

  errors += do_tests();

  if (errors)
    printf("Test failure\n");
  else
    printf("Test success\n");

  return errors;
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
