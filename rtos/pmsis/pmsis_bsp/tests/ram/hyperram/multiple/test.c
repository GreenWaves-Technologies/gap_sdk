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
#include <bsp/ram/hyperram.h>

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
#define NB_ASYNC_TRANSFERS 4

static struct pi_task fc_tasks[NB_ASYNC_TRANSFERS];
#ifdef USE_CLUSTER
PI_L1 static cl_ram_req_t cl_tasks[NB_ASYNC_TRANSFERS];
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

// This test will launch several transfers in parallel for the given parameters
static int check_common_transfer(int loc2ext, int size, int l2_offset, int hyper_offset, int stride, int length, int async, int nb_transfers)
{
#ifdef __ZEPHYR__
  if (pi_is_fc())
#endif
  {
    printf("Checking %stransfer (loc2ext: %d, size: 0x%x, l2_offset: 0x%x, hyper_offset: 0x%x, stride: 0x%x, length: 0x%x)\n", async?"async " : "", loc2ext, size, l2_offset, hyper_offset, stride, length);
  }

  int transfer_size = size;
  if (stride)
  {
    transfer_size = stride * ((size + length - 1) / length);
  }

  //rt_perf_t perf;
  int errors = 0;
  int whole_area_size = transfer_size*2*nb_transfers;

  unsigned char *l2_buffers[nb_transfers];
  uint32_t hyper_buffers[nb_transfers];

  //rt_perf_init(&perf);
  //rt_perf_conf(&perf, 1<<RT_PERF_CYCLES);

  // Get Hyper and L2 areas where we will do the tests
  uint32_t hyper_buff;
  unsigned char *l2_buff = NULL;

  if (!pi_is_fc())
  {
#ifdef USE_CLUSTER
    cl_ram_alloc_req_t hyper_alloc_req;
    pi_cl_alloc_req_t alloc_req;
    cl_ram_alloc(&hyper, whole_area_size, &hyper_alloc_req);
    if (cl_ram_alloc_wait(&hyper_alloc_req, &hyper_buff))
      return -1;

    pi_cl_l2_malloc(whole_area_size, &alloc_req);
    l2_buff = pi_cl_l2_malloc_wait(&alloc_req);
#endif
  }
  else
  {
    if (ram_alloc(&hyper, &hyper_buff, whole_area_size))
      return -1;

    l2_buff = pmsis_l2_malloc(whole_area_size);
  }

  if (hyper_buff == 0) return -1;
  if (l2_buff == NULL) return -1;

  // Initialize hyperram and l2 buffers with values
  // to check afterwards if we correctly accessed
  // the hyperram
  for (int i=0; i<whole_area_size; i++)
  {
    l2_buff[i] = 0x80 | i;
  }

  if (!pi_is_fc())
  {
#ifdef USE_CLUSTER
    cl_ram_req_t req;
    cl_ram_write(&hyper, hyper_buff, l2_buff, whole_area_size, &req);
    cl_ram_write_wait(&req);
#endif
  }
  else
  {
    ram_write(&hyper, hyper_buff, l2_buff, whole_area_size);
  }

  for (int i=0; i<whole_area_size; i++)
  {
    l2_buff[i] = i & 0x7f;
  }


  // Initialize all transfers
  for (int i=0; i<nb_transfers; i++)
  {
    // The transfer l2 and hyper buffer is taken in the middle so that we can check overflows
    l2_buffers[i] = &l2_buff[transfer_size*2*i + transfer_size/2 + l2_offset];
    hyper_buffers[i] = hyper_buff + transfer_size*2*i + transfer_size/2 + hyper_offset;
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
            cl_ram_read_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &cl_tasks[i]);
          else
            cl_ram_read(&hyper, hyper_buffers[i], l2_buffers[i], size, &cl_tasks[i]);
        }
        else
        {
          if (stride)
            cl_ram_write_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &cl_tasks[i]);
          else
            cl_ram_write(&hyper, hyper_buffers[i], l2_buffers[i], size, &cl_tasks[i]);
        }
      }
      else
      {
        if (!loc2ext)
        {
          if (stride)
          {
            cl_ram_req_t req;
            cl_ram_read_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &req);
            cl_ram_read_wait(&req);
          }
          else
          {
            cl_ram_req_t req;
            cl_ram_read(&hyper, hyper_buffers[i], l2_buffers[i], size, &req);
            cl_ram_read_wait(&req);
          }
        }
        else
        {
          if (stride)
          {
            cl_ram_req_t req;
            cl_ram_write_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &req);
            cl_ram_write_wait(&req);
          }
          else
          {
            cl_ram_req_t req;
            cl_ram_write(&hyper, hyper_buffers[i], l2_buffers[i], size, &req);
            cl_ram_write_wait(&req);
          }
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
            ram_read_2d_async(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &fc_tasks[i]);
          else
            ram_read_async(&hyper, hyper_buffers[i], l2_buffers[i], size, &fc_tasks[i]);
        }
        else
        {
          if (stride)
            ram_write_2d_async(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length, &fc_tasks[i]);
          else
            ram_write_async(&hyper, hyper_buffers[i], l2_buffers[i], size, &fc_tasks[i]);
        }
      }
      else
      {
        if (!loc2ext)
        {
          if (stride)
            ram_read_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length);
          else
            ram_read(&hyper, hyper_buffers[i], l2_buffers[i], size);
        }
        else
        {
          if (stride)
            ram_write_2d(&hyper, hyper_buffers[i], l2_buffers[i], size, stride, length);
          else
            ram_write(&hyper, hyper_buffers[i], l2_buffers[i], size);
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
        #if defined(PMSIS_DRIVERS)
        hal_compiler_barrier();
        #else
        rt_compiler_barrier();
        #endif  /* PMSIS_DRIVERS */
        if (!loc2ext)
          cl_ram_read_wait(&cl_tasks[i]);
        else
          cl_ram_write_wait(&cl_tasks[i]);
        #if defined(PMSIS_DRIVERS)
        hal_compiler_barrier();
        #else
        rt_compiler_barrier();
        #endif  /* PMSIS_DRIVERS */
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
      cl_ram_req_t req;
      cl_ram_read(&hyper, hyper_buff, l2_buff, whole_area_size, &req);
      cl_ram_read_wait(&req);
#endif
    }
    else
    {
      ram_read(&hyper, hyper_buff, l2_buff, whole_area_size);
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
      unsigned char *l2_buffer = &l2_buff[transfer_size*2*i];
      for (int j=0; j<transfer_size*2; j++)
      {
        unsigned char expected;
        if (j < hyper_offset + transfer_size/2 || j >= hyper_offset + transfer_size + transfer_size/2)
        {
          expected = ((j & 0x7f) + i * transfer_size * 2) | 0x80;
        }
        else
        {
          if (stride)
          {
            int hyper_index = j - hyper_offset - transfer_size/2;
            int line_index = hyper_index / stride;
            int line_offset = hyper_index % stride;

            if (line_offset >= length || line_offset + line_index*length >= size)
              expected = ((j & 0x7f) + i * transfer_size * 2) | 0x80;
            else
              expected = (line_index * length + line_offset + transfer_size / 2 + l2_offset + i * transfer_size * 2) & 0x7f;
          }
          else
          {
            expected = (j + l2_offset - hyper_offset + i * transfer_size * 2) & 0x7f;
          }
        }

        if (expected != l2_buffer[j])
        {
          printf("Error at index %d: expected: %2.2x, got: %2.2x\n", transfer_size*2*i + j, expected, l2_buffer[j]);
          errors++;
        }
      }
    }
  }
  else
  {
    for (int i=0; i<nb_transfers; i++)
    {
      unsigned char *l2_buffer = &l2_buff[transfer_size*2*i];
      for (int j=0; j<transfer_size*2; j++)
      {
        unsigned char expected;

        if (j < l2_offset + transfer_size/2 || j >= l2_offset + size + transfer_size/2)
        {
          expected = (j + i * transfer_size * 2) & 0x7f;
        }
        else
        {
          if (stride)
          {
            int l2_index = j - l2_offset - transfer_size/2;
            int line_index = l2_index / length;
            int hyper_index = line_index*stride + l2_index % length;
            expected = (hyper_index + transfer_size / 2 + hyper_offset + i * transfer_size * 2) | 0x80;
          }
          else
          {
            expected = (j - l2_offset + hyper_offset + i * transfer_size * 2) | 0x80;
          }
        }

        if (expected != l2_buffer[j])
        {
          printf("Error at index %d: expected: %2.2x, got: %2.2x\n", transfer_size*2*i + j, expected, l2_buffer[j]);
          errors++;
        }
      }
    }
  }





  // Clean-up before leaving
  ram_free(&hyper, hyper_buff, whole_area_size);

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

int test_entry()
{
    printf("Entering main controller\n");

    struct hyperram_conf conf;
    hyperram_conf_init(&conf);

    pi_open_from_conf(&hyper, &conf);

    int32_t error = ram_open(&hyper);
    if (error)
    {
        return -1;
    }

    if (ram_alloc(&hyper, &hyper_buff[0], BUFF_SIZE*2))
    {
        return -2;
    }

    if (ram_alloc(&hyper, &hyper_buff[1], BUFF_SIZE*2))
    {
        return -4;
    }

#ifdef USE_CLUSTER
    return exec_tests_on_cluster();
#else
    return exec_tests();
#endif
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
