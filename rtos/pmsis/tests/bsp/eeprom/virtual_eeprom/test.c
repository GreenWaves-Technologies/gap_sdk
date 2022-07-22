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
#include <bsp/eeprom/virtual_eeprom.h>

static int master_setup(pi_device_t* master_device, uint8_t i2c_interface, uint16_t slave_addr, int is_10bits)
{
    int status = 0;
    struct pi_i2c_conf* conf = pi_l2_malloc(sizeof(struct pi_i2c_conf));
    //TODO free the configuration ?
    pi_i2c_conf_init(conf);

    conf->itf = i2c_interface;
    /* set target slave address */
    pi_i2c_conf_set_slave_addr(conf, slave_addr, is_10bits);
    pi_i2c_conf_set_wait_cycles(conf, 0);

    pi_open_from_conf(master_device, conf);

    if(pi_i2c_open(master_device))
    {
        printf("Error while opening the I2C interface\n");
        status = -1;
    }

    return status;
}

#define TEST_SIZE (35)

static int master_test(void)
{
    int errors = 0;
    pi_device_t master_dev0;

    if (master_setup(&master_dev0, 2, 0x14, 0))
    {
        printf("Master 0 configuration failed!\n");
        return -1;
    }

    /* send requests and verify results */
    uint8_t write_buff[3 + TEST_SIZE];
    write_buff[0] = 3;
    write_buff[1] = 0;
    for (int i = 0; i < TEST_SIZE; i++)
    {
        write_buff[i+2] = 0x10 + i;
    }
    uint8_t read_buff[TEST_SIZE] = {0};

    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    int status;

    // Synchronous tests
    status = pi_i2c_write(&master_dev0, write_buff, TEST_SIZE + 2, flag);
    if (status != PI_OK)
    {
        printf("Write status: %d\n", status);
        errors++;
    }
    status = pi_i2c_write(&master_dev0, write_buff, 2, flag);
    if (status != PI_OK)
    {
        printf("Write addr status: %d\n", status);
        errors++;
    }
    status = pi_i2c_read(&master_dev0, read_buff, TEST_SIZE, flag);
    if (status != PI_OK)
    {
        printf("Read status: %d\n", status);
        errors++;
    }

    for (int i = 0; i < TEST_SIZE; i++) {
        if (read_buff[i] != write_buff[i+2])
        {
            printf("mem[%d] = %d, expected %d\n", i, read_buff[i], write_buff[i+2]);
            errors++;
        }
    }

    /* close and free */
    pi_i2c_close(&master_dev0);
    pi_l2_free(&master_dev0, sizeof(pi_device_t));

    if (status != PI_OK || errors != 0)
    {
        printf("Tests failed with %d errors\n", errors);
        return -1;
    }

    return 0;

}

static int test_entry()
{
  struct pi_virtual_eeprom_conf conf;
  static struct pi_device eeprom_slave;

  struct pi_virtual_eeprom_conf conf2;
  static struct pi_device eeprom_slave2;

  pi_virtual_eeprom_conf_init(&conf);
  conf.i2c_itf = 1;


  pi_open_from_conf(&eeprom_slave, &conf);

  if (pi_eeprom_slave_open(&eeprom_slave))
  {
    return -1;
  }

  pi_virtual_eeprom_conf_init(&conf2);
  conf2.i2c_addr = 0x22;
  conf2.i2c_itf = 0;
  pi_open_from_conf(&eeprom_slave2, &conf2);
  if (pi_eeprom_slave_open(&eeprom_slave2))
  {
    return -1;
  }

  int errors = master_test();

  pi_eeprom_slave_close(&eeprom_slave);
  pi_eeprom_slave_close(&eeprom_slave2);

  if (!errors)
  {
      printf("Test success\n");
  }
  else
  {
      printf("Test failed\n");
  }

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
