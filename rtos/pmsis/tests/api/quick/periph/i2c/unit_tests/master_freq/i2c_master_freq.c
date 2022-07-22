/**
 * I2C Frequency Test
 *
 **/

#include <stdio.h>
#include <stdbool.h>

#include "pmsis.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2c_slave.h"
#include <bsp/eeprom/virtual_eeprom.h>

#define I2C_SLAVE_L2_TEST_ADDRESS (0x1c019000)

#define I2C_SLAVE_ADDR0 (0x14 << 1) // just some random address

#define INTERFACE_MASTER0  (0)

#define PRINTF(...)
//#define PRINTF(...) printf(__VA_ARGS__)


// ===================================================
// STATIC VARIABLES
// ===================================================

/* master device */
static pi_device_t* master_dev0 = NULL;

/* master async read buffer */
static uint8_t async_read_buff[2] = { 0 , 0 };

// ===================================================
// FUNCTIONS
// ===================================================

/**
 * \brief setup the master device
 *
 * \returns -1 if initialization failed
 *          else 0
 */
int master_setup(pi_device_t* master_device, uint8_t i2c_interface, uint16_t slave_addr, int is_10bits,
                 uint32_t baudrate)
{
    int status = 0;
    struct pi_i2c_conf* conf = pi_l2_malloc(sizeof(struct pi_i2c_conf));
    //TODO free the configuration ?
    pi_i2c_conf_init(conf);

    conf->itf = i2c_interface;
    conf->max_baudrate = baudrate;
    /* set target slave address */
    pi_i2c_conf_set_slave_addr(conf, slave_addr, is_10bits);
    pi_i2c_conf_set_wait_cycles(conf, 0);

    pi_open_from_conf(master_device, conf);

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
int masters_launch(uint32_t baudrate)
{
    printf("Launching test with baudrate %ld kHz\n", baudrate / 1000);
    /* initialize device */
    master_dev0 = pi_l2_malloc(sizeof(pi_device_t));

    if (master_setup(master_dev0, INTERFACE_MASTER0, I2C_SLAVE_ADDR0, 0, baudrate))
    {
        PRINTF("Master 0 configuration failed!\n");
        return -1;
    }

    /* send requests and verify results */
    uint8_t write_buff0[3] = {3 , 0, 0};
    uint8_t read_buff[32] = {0};
    uint32_t read_buff_size = sizeof(read_buff)/sizeof(read_buff[0]);
    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    int status;

    int errors = 0;

    int index = 2000;
    while(index > 0)
    {
        PRINTF("Loop %d\n", index);
        index--;

        /* reset read_buff */
        for (size_t i = 0; i < read_buff_size; i++)
        {
            read_buff[i] = 0;
        }

        // Synchronous tests
        write_buff0[2]++; // increase the first written byte, so that we can tell if the master is working
        status = pi_i2c_write(master_dev0, write_buff0, 3, flag);
        /* write a byte */
        if (status != PI_OK)
        {
            printf("Write status: %d\n", status);
            errors++;
        }
        /* set read address */
        status = pi_i2c_write(master_dev0, write_buff0, 2, flag);
        if (status != PI_OK)
        {
            printf("Write status: %d\n", status);
            errors++;
        }
        PRINTF("Synchronous Write done.\n");
        /* read address */
        status = pi_i2c_read(master_dev0, read_buff, read_buff_size, flag);
        if (status != PI_OK)
        {
            printf("Invalid Read status: %d\n", status);
            errors++;
        }
        else
        {
            for (size_t i = 0; i < read_buff_size; i++)
            {
                if (read_buff[i] != 88)
                {
                    printf("Incorrect read value, expected 88, got %d\n", read_buff[i]);
                    errors++;
                }
            }
        }
        PRINTF("Synchronous Read done.\n");
    }

    /* close and free */
    pi_i2c_close(master_dev0);
    pi_l2_free(master_dev0, sizeof(pi_device_t));

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

    /* launch the master */
    int ret_100 = masters_launch(100000);
    if(ret_100)
    {
        printf("test 100kHz returned with %d errors\n", ret_100);
    }
    int ret_400 = masters_launch(400000);
    if(ret_400)
    {
        printf("test 400kHz returned with %d errors\n", ret_400);
    }

    ret = ret_100 + ret_400;
    if(ret)
    {
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

