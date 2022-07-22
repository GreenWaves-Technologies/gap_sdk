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
#include <bsp/bsp.h>

#if defined(FLASH_TYPE)
#include <bsp/flash/hyperflash.h>

#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif

#endif

#define VERBOSE 1

#ifdef USE_HYPERFLASH
#define FLASH_NAME "HYPERFLASH"
#endif

#ifdef USE_SPIFLASH
#define FLASH_NAME "SPIFLASH"
#endif

#ifdef USE_MRAM
#define FLASH_NAME "MRAM"
#endif

#if !defined(TEST_QUICK) && !defined(TEST_BASIC) && !defined(TEST_ROBUST)
#define TEST_QUICK 1
#endif

#if !defined(TEST_ASYNC_2D) && !defined(TEST_SYNC_2D) && !defined(TEST_ASYNC) && !defined(TEST_SYNC)
#define TEST_SYNC 1
#endif

#if !defined(TEST_READ) && !defined(TEST_WRITE)
#define TEST_READ 1
#endif

#if defined(TEST_READ)
#define LOC2EXT 0
#else
#define LOC2EXT 1
#endif

#define BUFF_SIZE 1024

#ifdef USE_SPIFLASH
#define BURST_SIZE 256
#define QUICK_BUFF_SIZE (BUFF_SIZE/64)
#define BUFF_SIZE0 (BURST_SIZE*2)                     // 2 bursts
#define BUFF_SIZE1 (BURST_SIZE*3)                     // 3 bursts
#define BUFF_SIZE2 (BURST_SIZE*4)                     // 4 bursts
#define BUFF_SIZE3 (BURST_SIZE + BURST_SIZE/2)        // 1.5 bursts
#define BUFF_SIZE4 (BURST_SIZE*2 + BURST_SIZE/2)      // 2.5 bursts
#define BUFF_SIZE5 (BURST_SIZE*3 + BURST_SIZE/2)      // 3.5 bursts
#define BUFF_SIZE6 (BURST_SIZE + 3)                   // 1 burst and 3 bytes
#define BUFF_SIZE7 (BURST_SIZE*2 + 3)                 // 2 burst and 3 bytes
#define BUFF_SIZE8 (BURST_SIZE*3 + 3)                 // 3 burst and 3 bytes
#define BUFF_SIZE9 (BURST_SIZE + BURST_SIZE/2 + 3)    // 1.5 bursts abd 3 bytes
#define BUFF_SIZE10 (BURST_SIZE*2 + BURST_SIZE/2 + 3) // 2.5 bursts abd 3 bytes
#define BUFF_SIZE11 (BURST_SIZE*3 + BURST_SIZE/2 + 3) // 3.5 bursts abd 3 bytes
#else
#define QUICK_BUFF_SIZE BUFF_SIZE
#endif

#define NB_ASYNC_TRANSFERS 4

static struct pi_task fc_tasks[NB_ASYNC_TRANSFERS];
#ifdef USE_CLUSTER
PI_L1 static pi_cl_flash_req_t cl_tasks[NB_ASYNC_TRANSFERS];
#endif
static unsigned char buff[2][BUFF_SIZE*2];
static uint32_t hyper_buff[2];
static int count = 0;
static struct pi_device hyper;

static int pendings;


static void end_of_transfer(void *arg)
{
  pendings--;
}

// This test will launch several transfers in parallel for the given paflasheters
static int check_common_transfer(int loc2ext, int size, int l2_offset, int hyper_offset, int stride, int length, int async, int nb_transfers)
{
#ifdef __ZEPHYR__
  if (pi_is_fc())
#endif
  {
#ifdef VERBOSE
    printf("Checking %stransfer (loc2ext: %d, size: 0x%x, l2_offset: 0x%x, hyper_offset: 0x%x, stride: 0x%x, length: 0x%x)\n", async?"async " : "", loc2ext, size, l2_offset, hyper_offset, stride, length);
#endif
  }

  int transfer_size = size;
  if (stride)
  {
    transfer_size = stride * ((size + length - 1) / length);
  }

  //rt_perf_t perf;
  int errors = 0;
  int whole_area_size = (transfer_size+8)*2*nb_transfers;

  unsigned char *l2_buffers[nb_transfers];
  uint32_t hyper_buffers[nb_transfers];

  //rt_perf_init(&perf);
  //rt_perf_conf(&perf, 1<<RT_PERF_CYCLES);

  // Get Hyper and L2 areas where we will do the tests
  static uint32_t hyper_buff_current = 0x10000;
  uint32_t hyper_buff = hyper_buff_current;
  // MRAM requires 16 bytes alignments for erase
  hyper_buff_current += (whole_area_size + 15) & ~0xf;
  unsigned char *l2_buff = NULL;

  if (!pi_is_fc())
  {
#ifdef USE_CLUSTER
    pi_cl_l2_malloc(whole_area_size, &alloc_req);
    l2_buff = pi_cl_l2_malloc_wait(&alloc_req);
#endif
  }
  else
  {
    l2_buff = pmsis_l2_malloc(whole_area_size);
  }

  if (hyper_buff == 0) return -1;
  if (l2_buff == NULL) return -1;

  // Initialize hyperflash and l2 buffers with values
  // to check afterwards if we correctly accessed
  // the hyperflash
  for (int i=0; i<whole_area_size; i++)
  {
    l2_buff[i] = 0x80 | i;
  }

  if (!pi_is_fc())
  {
#ifdef USE_CLUSTER
    pi_cl_flash_req_t req;
    pi_cl_flash_write(&hyper, hyper_buff, l2_buff, whole_area_size, &req);
    pi_cl_flash_write_wait(&req);
#endif
  }
  else
  {
#if defined(USE_MRAM) || defined(__PLATFORM_GVSOC__) || defined(__PLATFORM_BOARD__)
    // MRAM model does not start with a good state if we don't erase it
    pi_flash_erase(&hyper, hyper_buff, whole_area_size);
#endif
    pi_flash_program(&hyper, hyper_buff, l2_buff, whole_area_size);
  }

  for (int i=0; i<whole_area_size; i++)
  {
    l2_buff[i] = i & 0x7f;
  }


  // Initialize all transfers
  for (int i=0; i<nb_transfers; i++)
  {
    // The transfer l2 and hyper buffer is taken in the middle so that we can check overflows
    int aligned_transfer_size = (transfer_size + 7) & ~0x7;
    l2_buffers[i] = &l2_buff[aligned_transfer_size*2*i + aligned_transfer_size/2 + l2_offset];
    hyper_buffers[i] = hyper_buff + aligned_transfer_size*2*i + aligned_transfer_size/2 + hyper_offset;
  }


  pendings = nb_transfers;

  // rt_perf_reset(&perf);
  // rt_perf_start(&perf);

  if (!pi_is_fc())
  {
#ifdef USE_CLUSTER
    for (int i=0; i<nb_transfers; i++)
    {
      if (async)
      {
        if (!loc2ext)
        {
          if (stride)
            pi_cl_flash_read_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &cl_tasks[i]);
          else
            pi_cl_flash_read(&hyper, hyper_buffers[i], l2_buffers[i], size, &cl_tasks[i]);
        }
        else
        {
          // if (stride)
          //   pi_cl_flash_write_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &cl_tasks[i]);
          // else
          //   pi_cl_flash_write(&hyper, hyper_buffers[i], l2_buffers[i], size, &cl_tasks[i]);
        }
      }
      else
      {
        if (!loc2ext)
        {
          if (stride)
          {
            pi_cl_flash_req_t req;
            pi_cl_flash_read_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &req);
            pi_cl_flash_read_wait(&req);
          }
          else
          {
            pi_cl_flash_req_t req;
            pi_cl_flash_read(&hyper, hyper_buffers[i], l2_buffers[i], size, &req);
            pi_cl_flash_read_wait(&req);
          }
        }
        else
        {
          // if (stride)
          // {
          //   pi_cl_flash_req_t req;
          //   pi_cl_flash_write_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &req);
          //   pi_cl_flash_write_wait(&req);
          // }
          // else
          // {
          //   pi_cl_flash_req_t req;
          //   pi_cl_flash_write(&hyper, hyper_buffers[i], l2_buffers[i], size, &req);
          //   pi_cl_flash_write_wait(&req);
          // }
        }
      }
    }
#endif
  }
  else
  {
    for (int i=0; i<nb_transfers; i++)
    {
      if (async)
      {
        pi_task_callback(&fc_tasks[i], end_of_transfer, (void *)i);
        if (!loc2ext)
        {
          if (stride)
            pi_flash_copy_2d_async(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, 1, &fc_tasks[i]);
          else
            pi_flash_read_async(&hyper, hyper_buffers[i], l2_buffers[i], size, &fc_tasks[i]);
        }
        else
        {
          if (stride)
            pi_flash_copy_2d_async(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, 0, &fc_tasks[i]);
          // else
          //   pi_flash_write_async(&hyper, hyper_buffers[i], l2_buffers[i], size, &fc_tasks[i]);
        }
      }
      else
      {
        if (!loc2ext)
        {
          if (stride)
            pi_flash_copy_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, 1);
          else
            pi_flash_read(&hyper, hyper_buffers[i], l2_buffers[i], size);
        }
        else
        {
          if (stride)
            pi_flash_copy_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, 0);
          // else
          //   pi_flash_write(&hyper, hyper_buffers[i], l2_buffers[i], size);
        }
      }
    }
  }

  // rt_perf_stop(&perf);

  // if (async)
  // {
  //   if (rt_perf_read(RT_PERF_CYCLES) > nb_transfers * 1000)
  //   {
  //     //printf("Took to many cycles for an asynchronous transfer (cycles: %d)\n", rt_perf_read(RT_PERF_CYCLES));
  //     //return -1;
  //   }
  // }

  //printf("Total cycles: %d\n", rt_perf_read(RT_PERF_CYCLES));

  if (async)
  {
    if (!pi_is_fc())
    {
#ifdef USE_CLUSTER
      for (int i=0; i<nb_transfers; i++)
      {
        // TODO this is a temporary workaround for a compiler bug which is messing-up the loop due to HW loops
        hal_compiler_barrier();
        if (!loc2ext)
          pi_cl_flash_read_wait(&cl_tasks[i]);
        else
          pi_cl_flash_write_wait(&cl_tasks[i]);
        hal_compiler_barrier();
      }
#endif
    }
    else
    {
      while(pendings != 0)
      {
        pi_yield();
      }
    }
  }

  if (loc2ext)
  {

    if (!pi_is_fc())
    {
#ifdef USE_CLUSTER
      pi_cl_flash_req_t req;
      pi_cl_flash_read(&hyper, hyper_buff, l2_buff, whole_area_size, &req);
      pi_cl_flash_read_wait(&req);
#endif
    }
    else
    {
      pi_flash_read(&hyper, hyper_buff, l2_buff, whole_area_size);
    }
  }

  for (int i=0; i<whole_area_size; i++)
  {
    //printf("%d: %x\n", i, l2_buff[i]);
  }

  if (loc2ext)
  {
    for (int i=0; i<nb_transfers; i++)
    {
      int aligned_transfer_size = (transfer_size + 7) & ~0x7;
      unsigned char *l2_buffer = &l2_buff[aligned_transfer_size*2*i];
      for (int j=0; j<transfer_size*2; j++)
      {
        unsigned char expected;
        if (j < hyper_offset + aligned_transfer_size/2 || j >= hyper_offset + transfer_size + aligned_transfer_size/2)
        {
          expected = ((j & 0x7f) + i * aligned_transfer_size * 2) | 0x80;
        }
        else
        {
          if (stride)
          {
            int hyper_index = j - hyper_offset - aligned_transfer_size/2;
            int line_index = hyper_index / stride;
            int line_offset = hyper_index % stride;

            if (line_offset >= length || line_offset + line_index*length >= size)
            {
              expected = ((j & 0x7f) + i * aligned_transfer_size * 2) | 0x80;
            }
            else
              expected = (line_index * length + line_offset + aligned_transfer_size / 2 + l2_offset + i * aligned_transfer_size * 2) & 0x7f;
          }
          else
          {
            expected = (j + l2_offset - hyper_offset + i * aligned_transfer_size * 2) & 0x7f;
          }
        }

        if (expected != l2_buffer[j])
        {
          printf("Error at index %d addr %p: expected: %2.2x, got: %2.2x\n", aligned_transfer_size*2*i + j, &l2_buffer[j], expected, l2_buffer[j]);
          errors++;
        }
      }
    }
  }
  else
  {
    for (int i=0; i<nb_transfers; i++)
    {

      int aligned_transfer_size = (transfer_size + 7) & ~0x7;
      unsigned char *l2_buffer = &l2_buff[aligned_transfer_size*2*i];
      for (int j=0; j<transfer_size*2; j++)
      {
        unsigned char expected;
        int within;

        if (j < l2_offset + aligned_transfer_size/2 || j >= l2_offset + size + aligned_transfer_size/2)
        {
          expected = (j + i * aligned_transfer_size * 2) & 0x7f;
          within = 0;
        }
        else
        {
          if (stride)
          {
            int l2_index = j - l2_offset - aligned_transfer_size/2;
            int line_index = l2_index / length;
            int hyper_index = line_index*stride + l2_index % length;
            expected = (hyper_index + aligned_transfer_size / 2 + hyper_offset + i * aligned_transfer_size * 2) | 0x80;
            within = 1;
          }
          else
          {
            expected = (j - l2_offset + hyper_offset + i * aligned_transfer_size * 2) | 0x80;
          }
        }

          //printf("Index %d, addr: %p, within transfer: %d, expected: %2.2x, got: %2.2x\n", j, &l2_buffer[j], within, expected, l2_buffer[j]);

        if (expected != l2_buffer[j])
        {
          printf("    Error at index %d addr %p: expected: %2.2x, got: %2.2x\n", aligned_transfer_size*2*i + j, &l2_buffer[j], expected, l2_buffer[j]);
          errors++;
        }
      }
    }
  }

  if (l2_buff)
    pmsis_l2_malloc_free(l2_buff, whole_area_size);

  return errors;
}

static int check_transfer(int loc2ext, int size, int l2_offset, int hyper_offset, int stride, int length, int async)
{
  return check_common_transfer(loc2ext, size, l2_offset, hyper_offset, stride, length, async, async ? NB_ASYNC_TRANSFERS : 1);
}

static int check_transfers_for_size(int size, int async)
{
#if defined(TEST_QUICK)
  if (check_transfer(LOC2EXT, size, 0, 0, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 1, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 0, 0, 0, async)) return -1;

#elif defined(TEST_BASIC) || defined(TEST_ROBUST)
  if (check_transfer(LOC2EXT, size, 0, 0, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 1, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 2, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 3, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 0, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 1, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 2, 0, 0, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 3, 0, 0, async)) return -1;
#endif

  return 0;
}

static int check_transfers_2d_for_size_and_stride(int size, int stride, int length, int async)
{
#if defined(TEST_QUICK)
  if (check_transfer(LOC2EXT, size, 0, 0, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 1, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 0, stride, length, async)) return -1;

#elif defined(TEST_BASIC) || defined(TEST_ROBUST)
  if (check_transfer(LOC2EXT, size, 0, 0, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 1, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 2, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 0, 3, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 0, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 1, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 2, stride, length, async)) return -1;
  if (check_transfer(LOC2EXT, size, 1, 3, stride, length, async)) return -1;
#endif

  return 0;
}

static int check_transfers_2d_for_size(int size, int async)
{
#if defined(TEST_QUICK)
  if (check_transfers_2d_for_size_and_stride(size, size/3 + 8, size/3, async)) return -1;

#elif defined(TEST_BASIC)
  if (check_transfers_2d_for_size_and_stride(size, size/3 + 8, size/3, async)) return -1;

#elif defined(TEST_ROBUST)
  if (check_transfers_2d_for_size_and_stride(size, size/16 + 8, size/16, async)) return -1;
  if (check_transfers_2d_for_size_and_stride(size, size/8 + 8, size/8, async)) return -1;
  if (check_transfers_2d_for_size_and_stride(size, size/8 + 7, size/8, async)) return -1;
  if (check_transfers_2d_for_size_and_stride(size, size/3 + 8, size/3, async)) return -1;
#endif

  return 0;
}

static int exec_tests()
{

#if defined(TEST_QUICK)

#ifdef TEST_SYNC

#ifdef __GAP8__
 #ifdef USE_SPIFLASH
   if (check_transfer(LOC2EXT, BUFF_SIZE0, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE1, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE2, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE3, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE4, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE5, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE6, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE7, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE8, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE9, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE10, 0, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE11, 0, 0, 0, 0, 0)) return -1;

   if (check_transfer(LOC2EXT, BUFF_SIZE0, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE1, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE2, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE3, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE4, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE5, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE6, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE7, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE8, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE9, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE10, 1, 0, 0, 0, 0)) return -1;
   if (check_transfer(LOC2EXT, BUFF_SIZE11, 1, 0, 0, 0, 0)) return -1;
 #endif
#endif
  if (check_transfers_for_size(QUICK_BUFF_SIZE, 0)) return -1;
#endif

#ifdef TEST_ASYNC
  if (check_transfers_for_size(QUICK_BUFF_SIZE, 1)) return -1;
#endif

#ifdef TEST_SYNC_2D
  if (check_transfers_2d_for_size(QUICK_BUFF_SIZE, 0)) return -1;
#endif

#ifdef TEST_ASYNC_2D
  if (check_transfers_2d_for_size(QUICK_BUFF_SIZE, 1)) return -1;
#endif



#elif defined(TEST_BASIC)

#ifdef TEST_SYNC
  if (check_transfers_for_size(BUFF_SIZE, 0)) return -1;
#endif

#ifdef TEST_ASYNC
  if (check_transfers_for_size(BUFF_SIZE, 1)) return -1;
#endif

#ifdef TEST_SYNC_2D
  if (check_transfers_2d_for_size(BUFF_SIZE, 0)) return -1;
#endif

#ifdef TEST_ASYNC_2D
  if (check_transfers_2d_for_size(BUFF_SIZE, 1)) return -1;
#endif



#elif defined(TEST_ROBUST)

#ifdef TEST_SYNC
  if (check_transfers_for_size(BUFF_SIZE/64, 0)) return -1;
  if (check_transfers_for_size(BUFF_SIZE/16, 0)) return -1;
  if (check_transfers_for_size(BUFF_SIZE, 0)) return -1;
#endif

#ifdef TEST_ASYNC
  //  if (check_transfers_for_size(BUFF_SIZE/64, 1)) return -1;
  if (check_transfers_for_size(BUFF_SIZE/16, 1)) return -1;
  if (check_transfers_for_size(BUFF_SIZE, 1)) return -1;
#endif

#ifdef TEST_SYNC_2D
  if (check_transfers_2d_for_size(BUFF_SIZE/64, 0)) return -1;
  if (check_transfers_2d_for_size(BUFF_SIZE/16, 0)) return -1;
  if (check_transfers_2d_for_size(BUFF_SIZE, 0)) return -1;
#endif

#ifdef TEST_ASYNC_2D
  if (check_transfers_2d_for_size(BUFF_SIZE/64, 1)) return -1;
  if (check_transfers_2d_for_size(BUFF_SIZE/16, 1)) return -1;
  if (check_transfers_2d_for_size(BUFF_SIZE, 1)) return -1;
#endif

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

int test_entry()
{
    int err;

#ifdef FLASH_TYPE
#ifdef USE_HYPERFLASH
    printf("Entering main controller (device: %s)\n", FLASH_NAME);
    struct pi_hyperflash_conf conf;
    pi_hyperflash_conf_init(&conf);
#elif defined(USE_MRAM)
    printf("Entering main controller (device: %s)\n", FLASH_NAME);
    struct pi_mram_conf conf;
    pi_mram_conf_init(&conf);
#else
#if defined(CONFIG_ATXP032)
    printf("Entering main controller (mode: octospi ram aps25xxn)\n");
  struct pi_atxp032_conf conf;
  pi_atxp032_conf_init(&conf);
#else
#if defined(CONFIG_MX25U51245G)
    printf("Entering main controller (mode: octospi flash mx25u51245g)\n");
    struct pi_mx25u51245g_conf conf;
    pi_mx25u51245g_conf_init(&conf);
#else
    printf("Entering main controller (device: %s)\n", FLASH_NAME);
    struct pi_spiflash_conf conf;
    pi_spiflash_conf_init(&conf);
#endif
#endif
#endif
#else
    printf("Entering main controller (mode: default)\n");
    struct pi_default_flash_conf conf;
    pi_default_flash_conf_init(&conf);
#endif

    pi_open_from_conf(&hyper, &conf);

    int32_t error = pi_flash_open(&hyper);
    if (error)
    {
        return -1;
    }

    hyper_buff[0] = 0;
    hyper_buff[1] = BUFF_SIZE*2;

#ifdef USE_CLUSTER
    err = exec_tests_on_cluster();
#else
    err = exec_tests();
#endif

    if (err == 0)
      printf("Test success\n");
    else
      printf("Test failure\n");
    return err;
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
