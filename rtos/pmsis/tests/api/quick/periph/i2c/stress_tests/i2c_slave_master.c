/**
 * I2C Stress Test
 *
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
#include <stdbool.h>

#include "pmsis.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2c_slave.h"
#include <bsp/eeprom/virtual_eeprom.h>

#define I2C_SLAVE_ADDR0 (0x14) // just some random address
#define I2C_SLAVE_ADDR0_10BITS (0x114)

#define I2C_SLAVE_ADDR1 (0x24) // just some random address
#define I2C_SLAVE_ADDR1_10BITS (0x124)

#define I2C_SLAVE_ADDR2 (0x34) // just some random address
#define I2C_SLAVE_ADDR2_10BITS (0x134)

#define INTERFACE_SLAVE0   (1)
#define INTERFACE_SLAVE1   (0)
#define INTERFACE_SLAVE2   (2)
#define INTERFACE_MASTER0  (1)
#define INTERFACE_MASTER1  (0)
#define MASTER_ENABLED (1)

#define PRINTF(...)
//#define PRINTF(...) printf(__VA_ARGS__)

// ===================================================
// STATIC VARIABLES
// ===================================================

/* slave device */
static pi_device_t slave_dev0;
static pi_device_t slave_dev1;
static pi_device_t slave_dev2;

/* master device */
static pi_device_t* master_dev0 = NULL;
static pi_device_t* master_dev1 = NULL;
static pi_device_t* master_dev2 = NULL;

/* master async read buffer */
static uint8_t async_read_buff[2] = { 0 , 0 };


// ===================================================
// FUNCTIONS
// ===================================================

/* flag for async read/write */
static volatile uint8_t done = 0;
static volatile uint8_t async_error = 0;

/* Callback for end of RX transfer. */
static void __end_of_rx(void *arg)
{
    pi_task_t* task = (pi_task_t *) arg;
    int status = pi_i2c_get_request_status(task);
    async_error = 0;
    if (status != PI_OK)
    {
        async_error = status;
    }
    done = 1;
}

/* Callback for end of TX transfer. */
static void __end_of_tx(void *arg)
{
    pi_task_t* task = (pi_task_t *) arg;
    pi_task_t* cb = (pi_task_t *) task->arg[2];
    int status = pi_i2c_get_request_status(task);
    if (status != PI_OK)
    {
        async_error = status;
        done =1;
    }
    else
    {
        pi_i2c_read_async(master_dev0, async_read_buff, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP, cb);
    }
}

/**
 * \brief setup slave to act as an EEPROM
 *
 * Setup 7 and 10 bits addresses since I2C supports 2 addresses slots
 *
 * \param slave_device pointer to the slave device
 * \param i2c_interface the interface to be used
 */
int slave_setup(pi_device_t* slave_device, uint8_t i2c_interface, uint32_t addr_7bits, uint32_t addr_10bits)
{
    struct pi_virtual_eeprom_conf conf;
    pi_virtual_eeprom_conf_init(&conf);
    conf.i2c_addr = addr_7bits;
    conf.i2c_itf = i2c_interface;
    pi_open_from_conf(slave_device, &conf);
    int ret = pi_eeprom_slave_open(slave_device);

    if(!ret)
    {
        PRINTF("Slave correctly configured on I2C interface %d\n", i2c_interface);
    }
    else
    {
        PRINTF("slave open failed\n");
    }

    return ret;
}

/**
 * \brief launch the slave
 *
 * \returns -1 if initialization failed
 *          else 0
 */
int slave_launch(void)
{

    if (slave_setup(&slave_dev0, INTERFACE_SLAVE0, I2C_SLAVE_ADDR0, I2C_SLAVE_ADDR0_10BITS))
    {
        PRINTF("Slave 0 configuration failed!\n");
        return -1;
    }

    if (slave_setup(&slave_dev1, INTERFACE_SLAVE1, I2C_SLAVE_ADDR1, I2C_SLAVE_ADDR1_10BITS))
    {
        PRINTF("Slave 1 configuration failed!\n");
        return -1;
    }

    if (slave_setup(&slave_dev2, INTERFACE_SLAVE2, I2C_SLAVE_ADDR2, I2C_SLAVE_ADDR2_10BITS))
    {
        PRINTF("Slave 2 configuration failed!\n");
        return -1;
    }

    return 0;
}

/**
 * \brief close the slave
 *
 * \returns -1 if operation failed
 *          else 0
 */
int slave_close(void)
{
    PRINTF("Closing slaves.\n");
    pi_eeprom_slave_close(&slave_dev0);
    pi_eeprom_slave_close(&slave_dev1);
    pi_eeprom_slave_close(&slave_dev2);
    return 0;
}

/**
 * \brief setup the master device
 *
 * \returns -1 if initialization failed
 *          else 0
 */
int master_setup(pi_device_t* master_device, uint8_t i2c_interface, uint16_t slave_addr, int is_10bits)
{
    int status = 0;
    struct pi_i2c_conf conf;

    pi_i2c_conf_init(&conf);
    conf.itf = i2c_interface;
    /* set target slave address */
    pi_i2c_conf_set_slave_addr(&conf, slave_addr, is_10bits);
    pi_i2c_conf_set_wait_cycles(&conf, 0);

    pi_open_from_conf(master_device, &conf);

    if(pi_i2c_open(master_device))
    {
        PRINTF("Error while opening the I2C interface\n");
        status = -1;
    }

    return status;
}

/**
 * \brief launch the master
 *
 * \returns -1 if initialization failed
 *          else 0
 */
int masters_launch(void)
{
    /* initialize device */
    master_dev0 = pi_l2_malloc(sizeof(pi_device_t));
    master_dev1 = pi_l2_malloc(sizeof(pi_device_t));

    if (master_setup(master_dev0, INTERFACE_MASTER0, I2C_SLAVE_ADDR1, 0))
    {
        PRINTF("Master 0 configuration failed!\n");
        return -1;
    }

    if (master_setup(master_dev1, INTERFACE_MASTER1, I2C_SLAVE_ADDR0, 0))
    {
        PRINTF("Master 1 configuration failed!\n");
        return -1;
    }

    /* send requests and verify results */
    uint8_t write_buff0[3] = {3 , 0, 0};
    uint8_t write_buff1[3] = {3 , 0, 0};
    uint8_t read_buff[1] = {0};
    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    int status;

    int errors = 0;

    int index = 20000;
    while(index > 0)
    {
        PRINTF("Loop %d\n", index);
        index--;

        // Asynchronous tests
        write_buff0[2]++; // increase the first written byte, so that we can tell if the master is working
        pi_task_t callback_tx, callback_rx;
        pi_task_callback(&callback_tx, __end_of_tx, &callback_tx);
        callback_tx.arg[2] = (int) &callback_rx;
        pi_task_callback(&callback_rx, __end_of_rx, &callback_rx);
        pi_i2c_write_async(master_dev0, write_buff0, 3, flag, &callback_tx);

        // Synchronous tests
        write_buff1[2]++; // increase the first written byte, so that we can tell if the master is working
        status = pi_i2c_write(master_dev1, write_buff1, 3, flag);
        /* write a byte */
        if (status != PI_OK)
        {
            printf("Write status: %d\n", status);
            errors++;
        }
        /* set read address */
        status = pi_i2c_write(master_dev1, write_buff1, 2, flag);
        if (status != PI_OK)
        {
            printf("Write status: %d\n", status);
            errors++;
        }
        PRINTF("Synchronous Write done.\n");
        /* read address */
        status = pi_i2c_read(master_dev1, read_buff, 1, flag);
        if (status != PI_OK)
        {
            printf("Read status: %d\n", status);
            errors++;
        }
        else if (read_buff[0] != write_buff1[2])
        {
            printf("Incorrect EEPROM value, expected %d, got %d\n", write_buff1[2], read_buff[0]);
            errors++;
        }
        else
        {
            /* value should be increasing */
            PRINTF("Read data: %d\n", read_buff[0]);
        }
        PRINTF("Synchronous Read done.\n");

        // wait asynchronous tests
        while (!done)
        {
            PRINTF("Waiting async\n");
            pi_yield();
        }
        PRINTF("Waiting async done.\n");

        if (async_error)
        {
            printf("Async error: %d\n", async_error);
            errors++;
        }
        done = 0;
        async_error = 0;
    }

    /* close and free */
    pi_i2c_close(master_dev0);
    pi_l2_free(master_dev0, sizeof(pi_device_t));

    pi_i2c_close(master_dev1);
    pi_l2_free(master_dev1, sizeof(pi_device_t));

    return errors;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
int test_main(void)
{
    PRINTF("Application start\n");

    int ret = 0;

    /* launch slave */
    if(slave_launch())
    {
        PRINTF("Error while opening slave");
        ret = 1;
    }

#if MASTER_ENABLED
    /* launch the master */
    if(!ret && masters_launch())
    {
        PRINTF("Error while running master\n");
        ret = 1;
    }
#else
    pi_time_wait_us(1000 * 1000 * 600);
#endif

    /* close slave */
    if(!ret && slave_close())
    {
        PRINTF("Error while closing slave\n");
        ret = 1;
    }

    if(ret)
    {
        //TODO make a global error counter ? (atomic ?)
        // volatile should be sufficient since there is only one core
        printf("test returned with %d errors\n", ret);
    }
    pmsis_exit(ret);
    while(1);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void*) test_main);
}

