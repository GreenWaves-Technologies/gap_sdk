/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


/**
 * I2C slave test
 * Small eeprom like protocol but with L2
 * Leader send 32 bit address + 32 bit size
 * Slave send back content at that address
 *
 * This is done through the i2c_memory_rx_callback which is registered to be
 * called at each rx end of frame
 *
 * On the tx side, handler is just a dummy, as nothing is really needed there
 * (tx transfer will clear itself when transfer is done)
 *
 **/

#include <stdio.h>

#include "pmsis.h"
#include "pmsis/drivers/i2c_slave.h"

#define I2C_SLAVE_L2_TEST_ADDRESS (0x1c019000)
#define I2C_SLAVE_ADDR0 (0x14) // just some random address
#define I2C_SLAVE_ADDR0_10BITS (0x114)
#define TEST_VALUE_BASE (0x01)

#define I2C_SLAVE_INTERFACE (1)
#define I2C_MASTER_INTERFACE (2)

// 64 * 4 bytes
#define BUFF_SIZE (16)
#define I2C_SLAVE_L2_TEST_SIZE (BUFF_SIZE*4)

/* Buffer to write in EEPROM : BUF_SIZE + 2, for the memory address. */
uint32_t write_buff[BUFF_SIZE+2] = {0};
/* Buffer to read from EEPROM : BUF_SIZE. */
uint32_t read_buff[BUFF_SIZE] = {0};
/* BUffer holding the memory address & size for read transactions. */
uint32_t addr_buff[2] = {0};


// buffer to hold rx on slave side
// BUFF SIZE + addr + size + an extra byte for matcher
uint8_t rx_buffer[((BUFF_SIZE+2)*4)+1] = {0};

uint32_t g_l2_buff[BUFF_SIZE];

#ifdef ASYNC
/* asynchronous variables */
static volatile uint8_t done = 0;
static volatile uint8_t async_error = 0;

/* asynchronous read callback */
static void __read_callback(void *arg)
{
    pi_task_t* task = (pi_task_t *) arg;
    /* check if the request succeed */
    int status = pi_i2c_get_request_status(task);
    async_error = 0;
    if (status != PI_OK)
    {
        async_error = status;
        printf("Status: %d\n", status);
    }
    done = 1;
}
#endif

void data_init(int nb)
{
    // set the whole addr field to some ducky value
    // allows to check if transfer makes sense
    rx_buffer[0] = 0x6F;
    rx_buffer[1] = 0x7F;
    rx_buffer[2] = 0x8F;
    rx_buffer[3] = 0x9F;
    rx_buffer[4] = 0xAF;

    for (int i=0; i<nb; i++)
    {
        g_l2_buff[i] = 0xdeadbeef;
    }

    addr_buff[0] = I2C_SLAVE_L2_TEST_ADDRESS;
    addr_buff[1] = I2C_SLAVE_L2_TEST_SIZE;

    write_buff[0] = I2C_SLAVE_L2_TEST_ADDRESS;
    write_buff[1] = I2C_SLAVE_L2_TEST_SIZE;
    for (int i=0; i<nb; i++)
    {
        write_buff[i+2] = i + 1UL;
    }
}

void i2c_memory_emu_rx_callback(pi_i2c_slave_args_t *arg)
{
    uint32_t l2_addr = *(uint32_t*)arg->l2_buffer;
    uint32_t size = *(uint32_t*)(arg->l2_buffer + 4);

    // ignoring addr
    if(arg->nb_bytes > 8)
    {
        // single transfer --> just reset buffer
        memcpy(g_l2_buff,arg->l2_buffer+8, size);
#ifdef TRACE
        for(int i = 0; i < BUFF_SIZE; i++)
        {
            printf("g_l2_buff[%i] = %x\n",i,g_l2_buff[i]);
        }
#endif
    }

    pi_i2c_slave_stop_rx(arg->handle);
    pi_i2c_slave_stop_tx(arg->handle);
    pi_i2c_slave_unlock(arg->handle, 1);
    pi_i2c_slave_set_tx(arg->handle, g_l2_buff, (BUFF_SIZE*4));
    pi_i2c_slave_set_rx(arg->handle, rx_buffer, ((BUFF_SIZE+2)*4)+1);

    arg->ret = 0;
}


void i2c_memory_emu_tx_callback(pi_i2c_slave_args_t *arg)
{
    if(arg->nb_bytes == 0)
    {
        pi_i2c_slave_unlock(arg->handle, 0);
        arg->ret = 0;
        return;
    }
    // nothing to do for this emulation except unlocking and reloading buf
    pi_i2c_slave_stop_tx(arg->handle);
    pi_i2c_slave_unlock(arg->handle, 0);
    pi_i2c_slave_set_tx(arg->handle, g_l2_buff, BUFF_SIZE*4);
    arg->ret = 0;
}

int i2c_slave_setup(pi_device_t *slave_dev)
{
    struct pi_i2c_slave_conf *conf = pi_l2_malloc(sizeof(struct pi_i2c_slave_conf));
    pi_i2c_slave_conf_init(conf);
    conf->itf =  I2C_SLAVE_INTERFACE;
    pi_i2c_slave_conf_set_addr0(conf, I2C_SLAVE_ADDR0, 0x1F, 0, 1, 0);

    conf->rx_callback = i2c_memory_emu_rx_callback;
    conf->tx_callback = i2c_memory_emu_tx_callback;

    pi_open_from_conf(slave_dev, conf);
    int ret = pi_i2c_slave_open(slave_dev);
    if(!ret)
    {
        pi_i2c_slave_set_rx(slave_dev->data, rx_buffer, ((BUFF_SIZE+2)*4)+1);
        pi_i2c_slave_set_tx(slave_dev->data, g_l2_buff, BUFF_SIZE*4);
    }
    else
    {
        printf("slave open failed\n");
    }

    return ret;
}

int i2c_slave_read_write_test()
{
    int errors = 0;
    pi_device_t *i2c_dev = pi_l2_malloc(sizeof(pi_device_t));
    pi_device_t *slave_dev = pi_l2_malloc(sizeof(pi_device_t));
    struct pi_i2c_conf *conf = pi_l2_malloc(sizeof(struct pi_i2c_conf));
    pi_i2c_conf_init(conf);
    conf->itf = I2C_MASTER_INTERFACE;
    pi_i2c_conf_set_slave_addr(conf, I2C_SLAVE_ADDR0, 0);
    pi_i2c_conf_set_wait_cycles(conf, 0);

    pi_open_from_conf(i2c_dev,conf);
    if(pi_i2c_open(i2c_dev))
    {
        printf("open failed\n");
        errors ++;
        return errors;
    }

    // I2C open is done -- let's jam
    data_init(BUFF_SIZE);

    i2c_slave_setup(slave_dev);

    /* synchronous requests */
    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    // BUFF_SIZE + 2 to take address into account
    if(pi_i2c_write(i2c_dev, (uint8_t*) write_buff, (BUFF_SIZE+2)*4, flag) != PI_OK)
    {
        printf("Error during first write\n");
        errors++;
    }
    // read back
    if(pi_i2c_write(i2c_dev, (uint8_t*) addr_buff, 2*4, flag) != PI_OK)
    {
        printf("Error during second write\n");
        errors++;
    }

#ifdef ASYNC
    /* asynchronous request */
    done = 0;
    async_error = 0;

    pi_task_t callback_read;
    /* you need to pass the task as parameter in order to be able to retrieve the request status */
    pi_task_callback(&callback_read, __read_callback, &callback_read);

    pi_i2c_read_async(i2c_dev, (uint8_t*) read_buff, BUFF_SIZE*4, flag, &callback_read);

    while(!done)
    {
        pi_yield();
    }

    if (async_error)
    {
        printf("Error during first read\n");
        errors++;
    }
#else
    if(pi_i2c_read(i2c_dev, (uint8_t*) read_buff, BUFF_SIZE*4, flag) != PI_OK)
    {
        printf("Error during second write\n");
        errors++;
    }
#endif

    /* results verification */

    for (int i=0; i<BUFF_SIZE; i++)
    {
        if (write_buff[i+2] != read_buff[i])
        {
            printf("Write : %x  Read : %x\n", write_buff[i+2], read_buff[i]);
            printf("g_l2_buff[%i] = %x\n",i,g_l2_buff[i]);
            errors++;
        }
    }
    pi_i2c_slave_close(slave_dev);
    pi_i2c_close(i2c_dev);

    return errors;
}

int i2c_slave_setup_10bits(pi_device_t *slave_dev)
{
    struct pi_i2c_slave_conf *conf = pi_l2_malloc(sizeof(struct pi_i2c_slave_conf));
    pi_i2c_slave_conf_init(conf);
    conf->itf =  I2C_SLAVE_INTERFACE;
    pi_i2c_slave_conf_set_addr0(conf, I2C_SLAVE_ADDR0_10BITS, 0x1F, 1, 1, 0);

    conf->rx_callback = i2c_memory_emu_rx_callback;
    conf->tx_callback = i2c_memory_emu_tx_callback;

    pi_open_from_conf(slave_dev, conf);
    int ret = pi_i2c_slave_open(slave_dev);
    if(!ret)
    {
        pi_i2c_slave_set_rx(slave_dev->data, rx_buffer, ((BUFF_SIZE+2)*4)+1);
        pi_i2c_slave_set_tx(slave_dev->data, g_l2_buff, BUFF_SIZE * 4);
    }
    else
    {
        printf("slave open failed\n");
    }

    return ret;
}

int i2c_slave_read_write_10_bits_test()
{
    int errors = 0;
    pi_device_t *i2c_dev = pi_l2_malloc(sizeof(pi_device_t));
    pi_device_t *slave_dev = pi_l2_malloc(sizeof(pi_device_t));
    struct pi_i2c_conf *conf = pi_l2_malloc(sizeof(struct pi_i2c_conf));
    pi_i2c_conf_init(conf);
    conf->itf = I2C_MASTER_INTERFACE;
    pi_i2c_conf_set_slave_addr(conf, I2C_SLAVE_ADDR0_10BITS, 1);
    pi_i2c_conf_set_wait_cycles(conf, 0);

    pi_open_from_conf(i2c_dev,conf);
    if(pi_i2c_open(i2c_dev))
    {
        printf("open failed\n");
        errors ++;
        return errors;
    }

    // I2C open is done -- let's jam
    data_init(BUFF_SIZE);

    i2c_slave_setup_10bits(slave_dev);

    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    // BUFF_SIZE + 2 to take address into account
    if(pi_i2c_write(i2c_dev, (uint8_t*) write_buff, (BUFF_SIZE+2)*4, flag) != PI_OK)
    {
        printf("Error during 10 bits first write\n");
        errors++;
    }
    // read back
    if (pi_i2c_write(i2c_dev, (uint8_t*) addr_buff, 2*4, flag) != PI_OK)
    {
        printf("Error during 10 bits second write\n");
        errors++;
    }

#ifdef ASYNC
    /* asynchronous request */
    done = 0;
    async_error = 0;

    pi_task_t callback_read;
    /* you need to pass the task as parameter in order to be able to retrieve the request status */
    pi_task_callback(&callback_read, __read_callback, &callback_read);

    pi_i2c_read_async(i2c_dev, (uint8_t*) read_buff, BUFF_SIZE*4, flag, &callback_read);

    while(!done)
    {
        pi_yield();
    }

    if (async_error)
    {
        printf("Error during first read\n");
        errors++;
    }
#else
    if(pi_i2c_read(i2c_dev, (uint8_t*) read_buff, BUFF_SIZE*4, flag) != PI_OK)
    {
        printf("Error during second write\n");
        errors++;
    }
#endif

    for (int i=0; i<BUFF_SIZE; i++)
    {
        if (write_buff[i+2] != read_buff[i])
        {
            printf("Write : %x  Read : %x\n", write_buff[i+2], read_buff[i]);
            printf("g_l2_buff[%i] = %x\n",i,g_l2_buff[i]);
            errors++;
        }
    }
    pi_i2c_slave_close(slave_dev);
    pi_i2c_close(i2c_dev);

    return errors;
}

int test_main(void)
{
    printf("Application start\n");

    int ret = 0;
    int ret_10 = 0;

    ret += i2c_slave_read_write_test();
    printf("i2c_slave_read_write_test done with result: %x \n",ret);

    ret_10 += i2c_slave_read_write_10_bits_test();
    printf("i2c_slave_read_write_10_bits_test done with result: %x \n",ret_10);

    ret += ret_10;
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

