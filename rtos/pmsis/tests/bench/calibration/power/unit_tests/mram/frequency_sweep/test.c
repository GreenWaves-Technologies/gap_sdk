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


#define BUFF_SIZE 4096

static int frequencies[] = {
    5000000, 7000000, 9000000, 11000000, 13000000, 15000000,
    17000000, 19000000, 21000000, 23000000, 25000000, 27000000,
    29000000, 31000000, 33000000, 35000000, 37000000, 40000000
};


struct pi_device flash;


static PI_L2 unsigned char rx_buffer[BUFF_SIZE];


static int check_frequency(int frequency)
{

    pi_flash_ioctl(&flash, PI_FLASH_IOCTL_SET_BAUDRATE, (void *)frequency);

    pi_flash_read(&flash, 0, rx_buffer, BUFF_SIZE);
}


static int test_entry()
{
    printf("Entering main controller\n");

    struct pi_mram_conf flash_conf;
    struct pi_flash_info flash_info = {0};

    pi_mram_conf_init(&flash_conf);

    pi_open_from_conf(&flash, &flash_conf);

    if (pi_flash_open(&flash))
        return -1;

    pi_flash_erase(&flash, 0, BUFF_SIZE);

    for (int i=0; i<BUFF_SIZE; i++)
    {
        rx_buffer[i] = i*7;
    }

    pi_flash_program(&flash, 0, rx_buffer, BUFF_SIZE);

    *(volatile int *)0x1C000000 = 0xAABBCCDD;

    for (int i=0; i<sizeof(frequencies)/sizeof(int); i++)
    {
        check_frequency(frequencies[i]);
    }

    *(volatile int *)0x1C000000 = 0xDDCCBBAA;

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
