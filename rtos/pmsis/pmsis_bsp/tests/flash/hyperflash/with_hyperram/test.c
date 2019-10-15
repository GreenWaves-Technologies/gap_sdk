/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "stdio.h"
#include <bsp/bsp.h>
#include <bsp/flash/hyperflash.h>
#include <bsp/ram/hyperram.h>

#define HYPER_FLASH 0
#define SPI_FLASH   1
#define MRAM        2

#define BUFF_SIZE 2048
#define PROGRAM_SIZE_RTL   BUFF_SIZE
#define PROGRAM_SIZE_OTHER ((1<<18)*4)

#define NB_ITER 2


static inline void get_info(unsigned int *program_size)
{
#if defined(ARCHI_PLATFORM_RTL)
  if (rt_platform() == ARCHI_PLATFORM_RTL)
  {
    *program_size = PROGRAM_SIZE_RTL;
  }
  else
  {
    *program_size = PROGRAM_SIZE_OTHER;
  }
#else
  *program_size = PROGRAM_SIZE_OTHER;
#endif  /* __PULP_OS__ */
}


static PI_L2 unsigned char rx_buffer[BUFF_SIZE];
static PI_L2 unsigned char tx_buffer[BUFF_SIZE];
static PI_L2 unsigned char ram_buffer[BUFF_SIZE];
static uint32_t hyper_buff;
static struct pi_device hyperram;



static void end_of_tx(void *arg)
{
}



static int test_entry()
{
  struct pi_device flash;
  struct hyperflash_conf flash_conf;
  struct flash_info flash_info;
  struct hyperram_conf ram_conf;
  pi_task_t ram_task;

  printf("Entering main controller\n");


  hyperflash_conf_init(&flash_conf);

  pi_open_from_conf(&flash, &flash_conf);

  if (flash_open(&flash))
    return -1;

  flash_ioctl(&flash, FLASH_IOCTL_INFO, (void *)&flash_info);



  hyperram_conf_init(&ram_conf);

  pi_open_from_conf(&hyperram, &ram_conf);

  if (ram_open(&hyperram))
    return -2;

  if (ram_alloc(&hyperram, &hyper_buff, BUFF_SIZE))
    return -3;




  for (int j=0; j<NB_ITER; j++)
  {
    if (j == NB_ITER - 1)
    {
      ram_free(&hyperram, hyper_buff, BUFF_SIZE);
      ram_close(&hyperram);
    }

    // The beginning of the flash may contain runtime data such as the boot binary.
    // Round-up the flash start with the sector size to not erase it.
    // Also add small offset to better test erase (sector size aligned) and program (512 byte aligned).
    uint32_t flash_addr = ((flash_info.flash_start + flash_info.sector_size - 1) & ~(flash_info.sector_size - 1)) + 128;

    unsigned int size;
    get_info(&size);

    flash_erase(&flash, flash_addr, size);

    while(size > 0)
    {
      for (int i=0; i<BUFF_SIZE; i++)
      {
        tx_buffer[i] = i*(j+1);
        rx_buffer[i] = 0;
        ram_buffer[i] = i*(j+2);
      }

      flash_program(&flash, flash_addr, tx_buffer, BUFF_SIZE);

      if (j != NB_ITER - 1)
      {
        // Do hyperram read and write to see if this disturbs flash read
        ram_write_async(&hyperram, hyper_buff, ram_buffer, BUFF_SIZE, pi_task_callback(&ram_task, end_of_tx, (void *)&hyperram));
      }


      flash_read(&flash, flash_addr, rx_buffer, BUFF_SIZE);

      pi_task_wait_on(&ram_task);

      for (int i=0; i<BUFF_SIZE; i++)
      {
          if (rx_buffer[i] != (unsigned char)(i*(j+1)))
          {
            printf("Error at index %d, expected 0x%2.2x, got 0x%2.2x\n", i, (unsigned char)i, rx_buffer[i]);
            printf("TEST FAILURE\n");
            return -4;
          }
      }
      size -= BUFF_SIZE;
      flash_addr += BUFF_SIZE;
    }
  }

  flash_close(&flash);

  printf("TEST SUCCESS\n");

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
