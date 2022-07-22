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
#include <bsp/eeprom/24xx1025.h>

#define BUFF_SIZE 256

static PI_L2 unsigned char buffer[2][BUFF_SIZE];

static int test_entry()
{
  struct pi_24xx1025_conf conf;
  static struct pi_device eeprom;

  pi_24xx1025_conf_init(&conf);

  pi_open_from_conf(&eeprom, &conf);

  if (pi_eeprom_open(&eeprom))
    return -1;

  for (int i=0; i<BUFF_SIZE; i++)
  {
    buffer[0][i] = i;
  }

  pi_eeprom_write(&eeprom, 13, (uint8_t *)buffer[0], BUFF_SIZE);

  pi_eeprom_read(&eeprom, 13, (uint8_t *)buffer[1], BUFF_SIZE);

  for (int i=0; i<BUFF_SIZE; i++)
  {
    if (buffer[1][i] != (i & 0xff))
    {
      printf("Got error at index %d, expected %2.2x got %2.2x\n", i, i & 0xff, buffer[1][i]);
      return -1;
    }
  }

  pi_eeprom_close(&eeprom);

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
