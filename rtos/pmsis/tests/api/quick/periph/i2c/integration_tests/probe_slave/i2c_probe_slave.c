#include <stdio.h>
#include <stdbool.h>

#include "pmsis.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2c_slave.h"
#include <bsp/eeprom/virtual_eeprom.h>

#define I2C_SLAVE_L2_TEST_ADDRESS (0x1c019000)

#define I2C_SLAVE_ADDR0 (0x14) // just some random address
#define I2C_SLAVE_ADDR0_10BITS (0x114)

#define INTERFACE_SLAVE0   (0)
#define MASTER_ENABLED (0)

#define PRINTF(...) printf(__VA_ARGS__)

// ===================================================
// STATIC VARIABLES
// ===================================================

/* slave device */
static pi_device_t slave_dev0;

// ===================================================
// FUNCTIONS
// ===================================================

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
    else
    {
        printf("Slave configured - address 0x%x\n", I2C_SLAVE_ADDR0);
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
    return 0;
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

    pi_time_wait_us(1000 * 1000 * 600);

    /* close slave */
    if(!ret && slave_close())
    {
        PRINTF("Error while closing slave\n");
        ret = 1;
    }

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

