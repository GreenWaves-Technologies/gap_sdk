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

#if defined(FLASH_TYPE)
#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif
#endif

#define HYPER_FLASH 0
#define SPI_FLASH   1
#define MRAM        2

#if defined(USE_HYPERFLASH)
#define BUFF_SIZE 1024
#define PROGRAM_SIZE_OTHER ((1<<18)*4)
#define PROGRAM_SIZE_RTL   BUFF_SIZE
#define FLASH_NAME "HYPER"
#elif defined(USE_MRAM)
#define BUFF_SIZE 1024
#define PROGRAM_SIZE_OTHER ((1<<12))
#define PROGRAM_SIZE_RTL   PROGRAM_SIZE_OTHER
#define FLASH_NAME "MRAM"
#else
#define BUFF_SIZE 1024
#define PROGRAM_SIZE_OTHER ((1<<10))
#define PROGRAM_SIZE_RTL   PROGRAM_SIZE_OTHER
#define FLASH_NAME "SPI"
#endif

#define NB_ITER 1



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

static int test_entry()
{
  struct pi_device flash;
#ifdef FLASH_TYPE
#ifdef USE_HYPERFLASH
  printf("Entering main controller (flash: %s)\n", FLASH_NAME);

  struct pi_hyperflash_conf flash_conf;
#elif USE_MRAM
  printf("Entering main controller (flash: %s)\n", FLASH_NAME);

  struct pi_mram_conf flash_conf;
#else
#if defined(CONFIG_ATXP032)
    printf("Entering main controller (mode: octospi flash atxp032)\n");

  struct pi_atxp032_conf flash_conf;
#else
#if defined(CONFIG_MX25U51245G)
    printf("Entering main controller (mode: octospi flash mx25u51245g)\n");
  struct pi_mx25u51245g_conf flash_conf;
#else
  printf("Entering main controller (flash: %s)\n", FLASH_NAME);

  struct pi_spiflash_conf flash_conf;
#endif
#endif
#endif
#else
    printf("Entering main controller (mode: default)\n");
  struct pi_default_flash_conf flash_conf;
#endif
  struct pi_flash_info flash_info = {0};

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

  pi_flash_ioctl(&flash, PI_FLASH_IOCTL_INFO, (void *)&flash_info);

  for (int j=0; j<NB_ITER; j++)
  {
    // The beginning of the flash may contain runtime data such as the boot binary.
    // Round-up the flash start with the sector size to not erase it.
    // Also add small offset to better test erase (sector size aligned) and program (512 byte aligned).

    uint32_t flash_addr = ((flash_info.flash_start + flash_info.sector_size - 1) & ~(flash_info.sector_size - 1)) + 128;

    int size;
    get_info(&size);

    // Erase takes lot of time and is not mandatory on some models, drop it in fast mode
#if !defined(TEST_TYPE_FAST) || defined(USE_MRAM)
    pi_flash_erase(&flash, flash_addr, size);
#endif

    while(size > 0)
    {
      for (int i=0; i<BUFF_SIZE; i++)
      {
        tx_buffer[i] = i*(j+1);
        rx_buffer[i] = 0;
      }

      pi_flash_program(&flash, flash_addr, tx_buffer, BUFF_SIZE);
      pi_flash_read(&flash, flash_addr, rx_buffer, BUFF_SIZE);

      for (int i=0; i<BUFF_SIZE; i++)
      {
          //printf("Index %d: got %x expected %x\n", i, rx_buffer[i], tx_buffer[i]);

          if (rx_buffer[i] != (unsigned char)(i*(j+1)))
          {
            printf("Error at index %d, expected 0x%2.2x, got 0x%2.2x\n", i, (unsigned char)i, rx_buffer[i]);
            printf("TEST FAILURE\n");
            return -2;
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
