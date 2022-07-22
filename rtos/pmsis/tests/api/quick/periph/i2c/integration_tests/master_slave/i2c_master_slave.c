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

#define I2C_SLAVE_ADDR0 (0x51)
#define I2C_SLAVE_ADDR0_10BITS (0x114)

#if defined(__PLATFORM_FPGA__)

#define ITERATIONS         (20000)
#define INTERFACE_SLAVE   (0)
#define INTERFACE_MASTER  (1)
static void print_iter(int index)
{
    if (0 == (index % 500))
    {
        printf("Loop %d\n", index);
    }
}

#else /* defined(__PLATFORM_FPGA__) */

#define ITERATIONS         (2)

#ifndef INTERFACE_SLAVE
#define INTERFACE_SLAVE   (2)
#endif

#ifndef INTERFACE_MASTER
#define INTERFACE_MASTER  (1)
#endif

static void print_iter(int index)
{
    printf("Loop %d\n", index);
}
#endif /* defined(__PLATFORM_FPGA__) */

#if INTERFACE_SLAVE == INTERFACE_MASTER
#error "Slave and master cannot be on the same interface"
#endif

#define ADDRESS_SIZE       (2)
#define DATA_SIZE          (40)


// ===================================================
// STATIC VARIABLES
// ===================================================

/* slave device */
static pi_device_t slave_dev0;

/* master device */
static pi_device_t master_dev0;

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
        printf("Slave correctly configured on I2C interface %d\n", i2c_interface);
    }
    else
    {
        printf("slave open failed\n");
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

    if (slave_setup(&slave_dev0, INTERFACE_SLAVE, I2C_SLAVE_ADDR0, I2C_SLAVE_ADDR0_10BITS))
    {
        printf("Slave 0 configuration failed!\n");
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
    printf("Closing slaves.\n");
    pi_eeprom_slave_close(&slave_dev0);
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
        printf("Error while opening the I2C interface\n");
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

    if (master_setup(&master_dev0, INTERFACE_MASTER, I2C_SLAVE_ADDR0, 0))
    {
        printf("Master 0 configuration failed!\n");
        return -1;
    }
    printf("Master configured on interface %d\n", INTERFACE_MASTER);

    /* allocate buffers */
    uint8_t* write_buff1 = pi_l2_malloc(sizeof(uint8_t) * (ADDRESS_SIZE + DATA_SIZE));
    uint8_t* read_buff = pi_l2_malloc(sizeof(uint8_t) * (DATA_SIZE));

    /* initialize address */
    write_buff1[0] = 3;
    write_buff1[1] = 0;
    for (int32_t i = 0; i < DATA_SIZE; i++)
    {
        write_buff1[i+ADDRESS_SIZE] = i;
    }

    /* send requests and verify results */
    pi_i2c_xfer_flags_e flag = PI_I2C_XFER_STOP | PI_I2C_XFER_START;
    int status;

    int errors = 0;

    int index = ITERATIONS;
    while(index > 0)
    {
        print_iter(index);
        index--;

        /* update written data */
        for (int32_t i = 0; i < DATA_SIZE; i++)
        {
            write_buff1[i+ADDRESS_SIZE] = (write_buff1[i+ADDRESS_SIZE] + i);
        }

        status = pi_i2c_write(&master_dev0, write_buff1, ADDRESS_SIZE+DATA_SIZE, flag);
        /* write data */
        if (status != PI_OK)
        {
            printf("Write status: %d\n", status);
            pmsis_exit(-1);
        }

        /* set read address */
        status = pi_i2c_write(&master_dev0, write_buff1, ADDRESS_SIZE, flag);
        if (status != PI_OK)
        {
            printf("Write status: %d\n", status);
            pmsis_exit(-1);
        }
        /* read address */
        status = pi_i2c_read(&master_dev0, read_buff, DATA_SIZE, flag);
        if (status != PI_OK)
        {
            printf("Read status: %d\n", status);
            pmsis_exit(-1);
        }

        /* verify read data */
        for (int32_t i = 0; i < DATA_SIZE; i++)
        {
            if (read_buff[i] != write_buff1[i+ADDRESS_SIZE])
            {
                printf("Loop #%d: i = %d", index, i);
                printf("Incorrect EEPROM value, expected %d, got %d\n", write_buff1[2], read_buff[0]);
                pmsis_exit(-1);
            }
        }
    }

    /* close and free */
    pi_i2c_close(&master_dev0);

    return errors;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
int test_main(void)
{
    printf("Application start\n");

#if INTERFACE_MASTER == 3 || INTERFACE_SLAVE == 3
    printf("Setting pad functions for I2C3\n");
    pi_pad_set_function(PI_PAD_046, PI_PAD_FUNC2);
    pi_pad_set_function(PI_PAD_047, PI_PAD_FUNC2);
#endif

    int ret = 0;

    /* launch slave */
    if(slave_launch())
    {
        printf("Error while opening slave");
        ret = 1;
    }

    /* launch the master */
    if(!ret && masters_launch())
    {
        printf("Error while running master\n");
        ret = 1;
    }

    /* close slave */
    if(!ret && slave_close())
    {
        printf("Error while closing slave\n");
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

