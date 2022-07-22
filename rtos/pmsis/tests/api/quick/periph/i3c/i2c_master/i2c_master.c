/**
 * I3C Integration Test
 *
 * Small eeprom like protocol but with L2
 * Leader send 32 bit address + 32 bit size
 * Slave send back content at that address
 *
 * This is done through the i3c_memory_rx_callback which is registered to be
 * called at each rx end of frame
 *
 * On the tx side, handler is just a dummy, as nothing is really needed there
 * (tx transfer will clear itself when transfer is done)
 *
 **/

#include <stdio.h>
#include <stdbool.h>

#include "pmsis.h"
#include "pmsis/drivers/i3c.h"
#include <bsp/eeprom/virtual_eeprom.h>

#define I3C_SLAVE_ADDR     (0xA0)
#define INTERFACE_MASTER   (0)
#define DATA_SIZE          (16)


// ===================================================
// STATIC VARIABLES
// ===================================================

/* master device */
static pi_device_t master_dev0;

uint8_t send_data[3] = {0, 0, 0};

// ===================================================
// FUNCTIONS
// ===================================================

/**
 * \brief setup the master device
 *
 * \returns -1 if initialization failed
 *          else 0
 */
int master_setup(pi_device_t* master_device, uint8_t i3c_interface, uint16_t slave_addr)
{
    int status = 0;
    struct pi_i3c_conf conf;

    pi_i3c_conf_init(&conf);
    conf.itf = i3c_interface;
    conf.max_baudrate = 40000000;
    /* set target slave address */
    pi_i3c_conf_set_slave_addr(&conf, slave_addr);

    pi_open_from_conf(master_device, &conf);

    if(pi_i3c_open(master_device))
    {
        printf("Error while opening the I3C interface\n");
        status = -1;
    }

    return status;
}

void m24fc1025_write_byte(uint32_t addr_17b, uint8_t value) {
    send_data[0] = (uint8_t) (addr_17b >> 8);
    send_data[1] = (uint8_t) (addr_17b & 0xFF);
    send_data[2] = value;

    /* Write */
    int status = 0;
    status = pi_i3c_write(&master_dev0, send_data, 3);
}

uint8_t m24fc1025_read_byte(uint32_t addr_17b) {
    uint8_t value;

    send_data[0] = (uint8_t) (addr_17b >> 8);
    send_data[1] = (uint8_t) (addr_17b & 0xFF);

    /* Read */
    int status = 0;
    status = pi_i3c_write(&master_dev0, send_data, 2);
    status = pi_i3c_read(&master_dev0, &value, 1);

    return value;
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
    int errors = 0;

    if (master_setup(&master_dev0, INTERFACE_MASTER, I3C_SLAVE_ADDR))
    {
        printf("Master 0 configuration failed!\n");
        return -1;
    }

    /* allocate buffers */
    uint8_t* read_buff  = pi_l2_malloc(sizeof(uint8_t) * (DATA_SIZE));

    // Write first 16 Bytes of memory
    for (uint8_t addr = 0; addr < DATA_SIZE ; addr++) { // Rotate bit
        m24fc1025_write_byte(addr, addr);
    }

    // Read data and write it to buffer
    for (uint32_t addr = 0; addr < DATA_SIZE; addr++) {
        read_buff[addr] = m24fc1025_read_byte(addr);
    }


    for (int32_t i = 0; i < DATA_SIZE; i++)
    {
        if(read_buff[i] != i)
            errors++;
    }

    /* close and free */
    pi_i3c_close(&master_dev0);

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

    int ret = 0;

    /* launch the master */
    if(!ret && masters_launch())
    {
        printf("Error while running master\n");
        ret = 1;
    }

    if(ret)
    {
        printf("test returned with %d errors\n", ret);
    } else {
        printf("test success\n");
    }
    pmsis_exit(ret);
    while(1);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void*) test_main);
}
