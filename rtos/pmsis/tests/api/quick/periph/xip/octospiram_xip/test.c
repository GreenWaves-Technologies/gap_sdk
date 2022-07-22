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

#ifdef USE_SPIRAM
#include <bsp/ram/spiram.h>
#endif

/** Internal includes - we don't want to expose xip in user api **/
#include "pmsis/implem/drivers/xip/xip_internal.h"

/* Variables used. */
#define XIP_PAGE_SIZE    (0) //(XIP_PAGE_SIZE_4KB)
#define BUFFER_ALIGN_SIZE      ( 512 << XIP_PAGE_SIZE )
#define BUFF_SIZE ( BUFFER_ALIGN_SIZE * 3)

#define XIP_CUR_DEVICE XIP_DEVICE_HYPER1

static char *buff[2];
static char *rcv_buff[2];
static uint32_t hyper_buff[2];
static int count = 0;
static struct pi_device hyper;
static struct pi_task fc_tasks[2];

static uint8_t *xip_buff = (uint8_t*)0x20000000;
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
  pi_ram_read_async(&hyper, hyper_buff[i], rcv_buff[i], BUFF_SIZE, pi_task_callback(&fc_tasks[i], end_of_rx, (void *)i));
}

int test_entry()
{
#ifdef USE_HYPERRAM
    printf("Entering main controller (mode: hyperram)\n");

  struct pi_hyperram_conf conf;
  pi_hyperram_conf_init(&conf);
#else
#if defined(CONFIG_APS25XXXN)
    printf("Entering main controller (mode: octospi ram aps25xxn)\n");

  struct pi_aps25xxxn_conf conf;
  pi_aps25xxxn_conf_init(&conf);
  conf.xip_en = 1;
#else
    printf("Entering main controller (mode: quad spi ram)\n");

  struct pi_spiram_conf conf;
  pi_spiram_conf_init(&conf);
#endif
#endif

  for (int i=0; i<2; i++)
  {
    buff[i] = pmsis_l2_malloc(BUFF_SIZE);
    if (buff[i] == NULL)
      return -1;
    rcv_buff[i] = pmsis_l2_malloc(BUFF_SIZE);
    if (rcv_buff[i] == NULL)
      return -2;
  }

  pi_open_from_conf(&hyper, &conf);

  if (pi_ram_open(&hyper))
    return -3;

  if (pi_ram_alloc(&hyper, &hyper_buff[0], BUFF_SIZE*2))
    return -4;

  hyper_buff[1] = (hyper_buff[0]+BUFF_SIZE);
  //if (pi_ram_alloc(&hyper, &hyper_buff[1], BUFF_SIZE))
  //  return -5;

  for (int i=0; i<BUFF_SIZE; i++)
  {
      buff[0][i] = i & 0x7f;
      buff[1][i] = i | 0x80;
  }

  pi_ram_write_async(&hyper, hyper_buff[0], buff[0], BUFF_SIZE, pi_task_callback(&fc_tasks[0], end_of_tx, (void *)0));

  pi_ram_write_async(&hyper, hyper_buff[1], buff[1], BUFF_SIZE, pi_task_callback(&fc_tasks[1], end_of_tx, (void *)1));

  while(count != 2) {
    pi_yield();
  }

  /*********************************/
  pi_device_t *xip_device = pmsis_l2_malloc(sizeof(pi_device_t));
  pi_xip_conf_t xip_conf = {0};
  xip_conf.start_addr = 0x20000000;
  xip_conf.ext_start_addr = hyper_buff[0];
  xip_conf.page_size = XIP_PAGE_SIZE;
  xip_conf.mount_size = (BUFF_SIZE*2) / (512 << XIP_PAGE_SIZE);
  xip_conf.device_id = XIP_CUR_DEVICE;
  for( int i = 0; i<4; i++ )
  {
      uint32_t page_ptr = pmsis_l2_malloc((512 << XIP_PAGE_SIZE));
      //printf("page ptr unaligned = %x size = %x \n",page_ptr,(512 << XIP_PAGE_SIZE)*2);
      page_ptr = page_ptr & (~((512 << XIP_PAGE_SIZE)-1));
      //printf("page ptr aligned = %x\n",page_ptr);
      __pi_xip_set_page_config(XIP_CUR_DEVICE, i, page_ptr);
  }
  printf("xip page alloc done\n");

  xip_device->config = &xip_conf;
  if(__pi_xip_open(&(xip_device->data), &xip_conf))
  {
      printf("xip open failed\n");
      pmsis_exit(-1);
  }
  printf("xip open success\n");

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
      if (expected != xip_buff[i+(BUFF_SIZE*j)])
      {
        printf("Error, xip buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, xip_buff[i+(BUFF_SIZE*j)]);
        return -7;
      }
    }
  }

  pi_ram_free(&hyper, hyper_buff[0], BUFF_SIZE);
  pi_ram_free(&hyper, hyper_buff[1], BUFF_SIZE);
  pi_ram_close(&hyper);

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
