
/***Include****/
#include "pmsis.h"
#include <stdio.h>
#include <stdint.h>

static struct pi_device i2c;

//I2C init func
static void i2c_init(uint8_t itf, uint8_t addr)
{
    ///* De-clock gating, enable I3C clock */
    struct pi_i2c_conf i2c_conf;

    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.itf = itf;
    i2c_conf.max_baudrate = 100000;
    pi_i2c_conf_set_slave_addr(&i2c_conf, addr << 1, 0);
    //pi_i2c_conf_set_wait_cycles(conf, 2048);

    pi_open_from_conf(&i2c, &i2c_conf);

    if (pi_i2c_open(&i2c))
    {
        printf("I2C open failed\n");
        pmsis_exit(-1);
    }
}

static void i2c_close(void)
{
    pi_i2c_close(&i2c);
}

static uint8_t i2c_write(uint8_t *buf, uint16_t size)
{
    int res = pi_i2c_write(&i2c, buf, size, PI_I2C_XFER_START | PI_I2C_XFER_STOP);
    if (res == PI_OK)
    {
        /* we received an ACK */
        return 1;
    }
    /* we received a NACK */
    return 0;
}

int scan(uint8_t itf)
{
    printf("Scanning interface %d\n", itf);
    uint8_t buf[1] = {0};
    uint8_t peripherals[128] = {0};

    int found = 0;
    for (int i = 0; i < 128; i++)
    {
        i2c_init(itf, i);
        peripherals[i] = i2c_write(buf ,1);
        if (peripherals[i] != 0)
        {
            found = 1;
        }
        i2c_close();
    }

    if (found)
    {
        int count = 0;
        printf("Found peripherals: ");
        for (int i = 0; i < 128; i++)
        {
            if(peripherals[i] != 0)
            {
                count++;
                printf("0x%02X, ", i);
            }
        }
        printf("\n");
        printf("interface %d: %d peripherals found\n", itf, count);
    }
    else
    {
        printf("interface %d: No peripheral found\n", itf);
    }

    return 0;
}

void test_kickoff()
{
    int ret = scan(0);
    //ret += scan(1);
    //ret += scan(2);
    pmsis_exit(ret);
}

int main()
{
    printf("==== I2C Scan ====\n");
    return pmsis_kickoff((void *) test_kickoff);
}
