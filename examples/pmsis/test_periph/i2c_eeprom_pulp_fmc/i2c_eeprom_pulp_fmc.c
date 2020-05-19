/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "pmsis.h"
#include "pmsis/drivers/i2c.h"

#define EEPROM_DEVICE_ADDRESS 0xA0
#define BUFF_SIZE             16
#define TEST_ADDR             0x0060
#define TEST_VALUE            0x45

/* Buffer to write in EEPROM : BUF_SIZE + 2, for the memory address. */
unsigned char write_buff[BUFF_SIZE+2] = {0};
/* Buffer to read from EEPROM : BUF_SIZE. */
unsigned char read_buff[BUFF_SIZE] = {0};
/* BUffer holding the memory address for read transactions. */
unsigned char addr_buff[2] = {0};

void data_init(int nb)
{
    addr_buff[0] = ((unsigned int)TEST_ADDR) >> 8;
    addr_buff[1] = ((unsigned int)TEST_ADDR) & 0xff;
    write_buff[0] = ((unsigned int)TEST_ADDR) >> 8;
    write_buff[1] = ((unsigned int)TEST_ADDR) & 0xff;
    for (int i=0; i<nb; i++)
    {
        write_buff[i+2] = i + (unsigned char) TEST_VALUE;
    }
}

int i2c_open_close_test()
{
    int errors = 0;
    pi_device_t *i2c_dev = pi_l2_malloc(sizeof(pi_device_t));
    struct pi_i2c_conf *conf = pi_l2_malloc(sizeof(struct pi_i2c_conf));
    conf->itf = 0;
    pi_i2c_conf_init(conf);

    pi_open_from_conf(i2c_dev,conf);
    if(pi_i2c_open(i2c_dev))
    {
        printf("open failed\n");
        errors ++;
        return errors;
    }

    pi_i2c_close(i2c_dev);

    return errors;
}

int i2c_eeprom_read_write_test()
{
    int errors = 0;
    pi_device_t *i2c_dev = pi_malloc(sizeof(pi_device_t));
    struct pi_i2c_conf *conf = pi_malloc(sizeof(struct pi_i2c_conf));
    pi_i2c_conf_init(conf);
    conf->itf = 0;
    pi_i2c_conf_set_slave_addr(conf, EEPROM_DEVICE_ADDRESS);
    pi_i2c_conf_set_wait_cycles(conf, 2048);

    pi_open_from_conf(i2c_dev,conf);
    if(pi_i2c_open(i2c_dev))
    {
        printf("open failed\n");
        errors ++;
        return errors;
    }

    // I2C open is done -- let's jam
    data_init(BUFF_SIZE);
    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    // BUFF_SIZE + 2 to take address into account
    pi_i2c_write(i2c_dev, write_buff, BUFF_SIZE+2, flag);


    pi_i2c_write(i2c_dev, addr_buff, 2, flag);
    

    pi_i2c_read(i2c_dev, read_buff, BUFF_SIZE, flag);


    for (int i=0; i<BUFF_SIZE; i++)
    {
        if (write_buff[i+2] != read_buff[i])
        {
            printf("Write : %x  Read : %x\n", write_buff[i+2], read_buff[i]);
            errors++;
        }
    }
    pi_i2c_close(i2c_dev);

    return errors;
}

int test_main(void)
{
    printf("Application start, with freq\n");
    
    int ret = i2c_open_close_test();
    printf("i2c_open_close_test done with result: %x \n",ret);

    ret |= i2c_eeprom_read_write_test();
    printf("i2c_eeprom_read_write_test done with result: %x \n",ret);

    if(ret)
    {
        printf("test returned with %d errors\n",ret);
    }
    pmsis_exit(ret);
    while(1);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void*) test_main);
}

