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

#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#include <bsp/ram/hyperram.h>
#else
#include <bsp/flash/spiflash.h>
#include <bsp/ram/spiram.h>
#endif

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




static int test_entry()
{
  struct pi_device flash;
  struct pi_flash_info flash_info;
  pi_task_t ram_task;

#ifdef USE_HYPERFLASH
    printf("Entering main controller (mode: hyperflash)\n");

  struct pi_hyperflash_conf flash_conf;
  struct pi_hyperram_conf ram_conf;
  pi_hyperflash_conf_init(&flash_conf);
  pi_hyperram_conf_init(&ram_conf);
#else
#if defined(CONFIG_ATXP032)
    printf("Entering main controller (mode: octospi ram aps25xxn)\n");

  struct pi_atxp032_conf flash_conf;
  pi_atxp032_conf_init(&flash_conf);
  struct pi_aps25xxxn_conf ram_conf;
  pi_aps25xxxn_conf_init(&ram_conf);
#else
    printf("Entering main controller (mode: spi flash)\n");

  struct pi_spiflash_conf flash_conf;
  struct pi_spiram_conf ram_conf;
  pi_spiflash_conf_init(&flash_conf);
  pi_spiram_conf_init(&ram_conf);
#endif
#endif

  pi_open_from_conf(&flash, &flash_conf);

  if (pi_flash_open(&flash))
    return -1;

  pi_flash_ioctl(&flash, PI_FLASH_IOCTL_INFO, (void *)&flash_info);



  pi_open_from_conf(&hyperram, &ram_conf);

  if (pi_ram_open(&hyperram))
    return -2;

  if (pi_ram_alloc(&hyperram, &hyper_buff, BUFF_SIZE))
    return -3;




  for (int j=0; j<NB_ITER; j++)
  {
    if (j == NB_ITER - 1)
    {
      pi_ram_free(&hyperram, hyper_buff, BUFF_SIZE);
      pi_ram_close(&hyperram);
    }

    // The beginning of the flash may contain runtime data such as the boot binary.
    // Round-up the flash start with the sector size to not erase it.
    // Also add small offset to better test erase (sector size aligned) and program (512 byte aligned).
    uint32_t flash_addr = ((flash_info.flash_start + flash_info.sector_size - 1) & ~(flash_info.sector_size - 1)) + 128;

    unsigned int size;
    get_info(&size);

    pi_flash_erase(&flash, flash_addr, size);

    while(size > 0)
    {
      for (int i=0; i<BUFF_SIZE; i++)
      {
        tx_buffer[i] = i*(j+1);
        rx_buffer[i] = 0;
        ram_buffer[i] = i*(j+2);
      }

      pi_flash_program(&flash, flash_addr, tx_buffer, BUFF_SIZE);

      if (j != NB_ITER - 1)
      {
        // Do hyperram read and write to see if this disturbs flash read
        pi_ram_write_async(&hyperram, hyper_buff, ram_buffer, BUFF_SIZE, pi_task_block(&ram_task));
      }

      pi_flash_read(&flash, flash_addr, rx_buffer, BUFF_SIZE);

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

  pi_flash_close(&flash);

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
