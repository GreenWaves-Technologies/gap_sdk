/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BMP280_Device_Address    ( 0x000000EC )

#define BMP280_ID_REG            ( 0xD0 )
#define BMP280_ID_VALUE          ( 0x58 )

#define BMP280_RESET_REG         ( 0xE0 )

uint8_t id_read = 0;
uint8_t cmd = BMP280_ID_REG;
struct pi_device i2c;

#if (ASYNC)
static volatile uint8_t done = 0;

/* Callback for end of RX transfer. */
static void __end_of_rx(void *arg)
{
    printf("Read async done.\n");
    done = 1;
}

/* Callback for end of TX transfer. */
static void __end_of_tx(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Write async done.\n");
    pi_i2c_read_async(&i2c, &id_read, 1, PI_I2C_XFER_STOP, cb);
}
#endif  /* ASYNC */

void test_i2c(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;
    struct pi_i2c_conf i2c_conf;

    /* Init & open i2c. */
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.itf = 0;
    i2c_conf.max_baudrate = 100000;
    i2c_conf.cs = (uint32_t) BMP280_Device_Address;

    pi_open_from_conf(&i2c, &i2c_conf);

    if (pi_i2c_open(&i2c))
    {
        printf("I2C open failed\n");
        pmsis_exit(-1);
    }

    /* Read ID value of BMP280. */
    #if (ASYNC)
    pi_task_t callback_tx, callback_rx;
    pi_task_callback(&callback_tx, __end_of_tx, &callback_rx);
    pi_task_callback(&callback_rx, __end_of_rx, NULL);
    pi_i2c_write_async(&i2c, &cmd, 1, PI_I2C_XFER_START | PI_I2C_XFER_NO_STOP, &callback_tx);
    while (!done)
    {
        pi_yield();
    }
    #else
    pi_i2c_write(&i2c, &cmd, 1, PI_I2C_XFER_START | PI_I2C_XFER_NO_STOP);
    printf("Write sync done.\n");
    pi_i2c_read(&i2c, &id_read, 1, PI_I2C_XFER_STOP);
    printf("Read sync done.\n");
    #endif  /* ASYNC */
    printf("BMP280_ID_VALUE : 0x%x\n", id_read);

    errors = !(id_read == BMP280_ID_VALUE);
    printf("\nTest %s with %d error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t*** PMSIS I2C Test ***\n\n");
    return pmsis_kickoff((void *) test_i2c);
}
