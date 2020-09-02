/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BMP280_Device_Address    ( 0x76 )

PI_L2 uint8_t dev_addr = 0;

void i2c_detection(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;
    int32_t devices = 0;

    struct pi_device i2c;
    struct pi_i2c_conf i2c_conf;

    /* Init & open i2c. */
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.itf = 0;
    i2c_conf.max_baudrate = 100000;
    i2c_conf.cs = (uint32_t) (BMP280_Device_Address << 1);

    pi_open_from_conf(&i2c, &i2c_conf);

    errors = pi_i2c_open(&i2c);
    if (errors)
    {
        printf("I2C open failed %d\n", errors);
        pmsis_exit(-1);
    }

    int8_t detect = -1;
    for (uint8_t i = 0x3; i < 0x78; i++)
    {
        i2c_conf.cs = (i << 1);
        detect = pi_i2c_detect(&i2c, &i2c_conf, &dev_addr);
        if (detect == 0x00)
        {
            printf("Device found at address : 0x%X, 0x%X.\n", i, (i << 1));
            devices++;
        }
    }

    errors = (devices == 0);
    printf("\ni2c bus scan : %d device(s) found.\n", devices);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t*** PMSIS I2C Detection ***\n\n");
    return pmsis_kickoff((void *) i2c_detection);
}
