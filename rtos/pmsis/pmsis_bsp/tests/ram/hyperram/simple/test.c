/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "pmsis.h"
#include "stdio.h"
#include <bsp/bsp.h>
#include <bsp/ram/hyperram.h>

#define BUFF_SIZE 2048

static char *buff[2];
static char *rcv_buff[2];
static uint32_t hyper_buff[2];
static int count = 0;
static struct pi_device hyper;
static struct pi_task fc_tasks[2];

static void end_of_rx(void *arg)
{
  int i = (int)arg;
  printf("End of RX for id %d\n", (int)arg);
  count++;
}

static void end_of_tx(void *arg)
{
  printf("End of TX for id %d\n", (int)arg);
  int i = (int)arg;
  ram_read_async(&hyper, hyper_buff[i], rcv_buff[i], BUFF_SIZE, pi_task_callback(&fc_tasks[i], end_of_rx, (void *)i));
}

int test_entry()
{
  printf("Entering main controller\n");

  for (int i=0; i<2; i++)
  {
    buff[i] = pmsis_l2_malloc(BUFF_SIZE);
    if (buff[i] == NULL)
      return -1;
    rcv_buff[i] = pmsis_l2_malloc(BUFF_SIZE);
    if (rcv_buff[i] == NULL)
      return -2;
  }

  struct hyperram_conf conf;
  hyperram_conf_init(&conf);

  pi_open_from_conf(&hyper, &conf);

  if (ram_open(&hyper))
    return -3;

  if (ram_alloc(&hyper, &hyper_buff[0], BUFF_SIZE))
    return -4;

  if (ram_alloc(&hyper, &hyper_buff[1], BUFF_SIZE))
    return -5;

  for (int i=0; i<BUFF_SIZE; i++)
    {
      buff[0][i] = i & 0x7f;
      buff[1][i] = i | 0x80;
    }

  //  printf("%s %d\n", __FILE__,__LINE__);
  ram_write_async(&hyper, hyper_buff[0], buff[0], BUFF_SIZE, pi_task_callback(&fc_tasks[0], end_of_tx, (void *)0));

  ram_write_async(&hyper, hyper_buff[1], buff[1], BUFF_SIZE, pi_task_callback(&fc_tasks[1], end_of_tx, (void *)1));

  while(count != 2) {
    pi_yield();
  }

  for (int j=0; j<2; j++)
  {
    for (int i=0; i<BUFF_SIZE; i++)
    {
      unsigned char expected;
      if (j == 0)
        expected = i & 0x7f;
      else
        expected = i | 0x80;
      if (expected != rcv_buff[j][i])
      {
        printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, rcv_buff[j][i]);
        return -6;
      }
    }
  }

  ram_free(&hyper, hyper_buff[0], BUFF_SIZE);
  ram_free(&hyper, hyper_buff[1], BUFF_SIZE);
  ram_close(&hyper);

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
