/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
//#define NO_PRINTF

#if defined(NO_PRINTF)
#define PRINTF(...)           ( (void) 0 )
#else
#define PRINTF                ( printf )
#endif  /* NO_PRINTF */

#define NB_ITER               ( 4 )
#define NB_FAIL               ( 2 )

#define EEPROM_DEVICE_ADDRESS ( 0xA0 )
#define ADDR_SIZE             ( 2 )
#define BUFF_SIZE             ( 16 )
#define TEST_ADDR             ( 0x0060 )
#define TEST_VALUE            ( 0x45 )

PI_L2 static pi_device_t i2c;
PI_L2 uint8_t write_buff[BUFF_SIZE + ADDR_SIZE] = {0};
PI_L2 uint8_t read_buff[BUFF_SIZE] = {0};
/* 0-RX | 1-TX */
PI_L2 static uint32_t timeout_us[NB_ITER][2] = {{00, 100}, /* OK */
                                                {0, 0},         /* OK */
                                                {100, 0},       /* KO */
                                                {0, 100}        /* KO */
};

static void data_init(uint32_t nb)
{
    write_buff[0] = ((uint32_t) TEST_ADDR) >> 8;
    write_buff[1] = ((uint32_t) TEST_ADDR) & 0xff;
    for (uint32_t i=0; i<nb; i++)
    {
        write_buff[i + (uint32_t) ADDR_SIZE] = i + (uint8_t) TEST_VALUE;
    }
}

static int32_t test_timeout_eeprom_write(uint8_t *tx_buff, uint32_t size,
                                         uint32_t timeout_us)
{
    int32_t errors = 0;
    pi_i2c_xfer_flags_e flag = (PI_I2C_XFER_STOP | PI_I2C_XFER_START);
    PRINTF("Sending write: l2_buf=%lx, size=%ld, timeout_us=%ld.\n",
           tx_buff, size, timeout_us);
    errors = pi_i2c_write_timeout(&i2c, tx_buff, size, flag, timeout_us);
    if (errors)
    {
        PRINTF("Timeout reached %ld us ! Test failed with %ld remaining bytes...\n",
               timeout_us, errors);
    }
    return errors;
}

static int32_t test_timeout_eeprom_read(uint8_t *addr_buff, uint8_t *rx_buff,
                                        uint32_t size, uint32_t timeout_us)
{
    int32_t errors = 0;
    pi_i2c_xfer_flags_e flag = (PI_I2C_XFER_STOP | PI_I2C_XFER_START);
    PRINTF("Sending addr: l2_buf=%lx, size=%ld.\n", addr_buff, (uint32_t) ADDR_SIZE);
    pi_i2c_write(&i2c, addr_buff, (uint32_t) ADDR_SIZE, flag);
    PRINTF("Sending read: l2_buf=%lx, size=%ld, timeout_us=%ld.\n",
           rx_buff, size, timeout_us);
    errors = pi_i2c_read_timeout(&i2c, rx_buff, size, flag, timeout_us);
    if (errors)
    {
        PRINTF("Timeout reached %ld us ! Test failed with %ld remaining bytes...\n",
               timeout_us, errors);
    }
    return errors;
}

static int32_t test_timeout_eeprom_check(uint8_t *tx_buff, uint8_t *rx_buff,
                                         uint32_t size)
{
    int32_t errors = 0;
    for (uint32_t i=0; i<size; i++)
    {
        if (tx_buff[i] != rx_buff[i])
        {
            errors++;
            PRINTF("Error at %ld: write=%x - read=%x\n", i, tx_buff[i], rx_buff[i]);
        }
    }
    return errors;
}

void test_entry(void)
{
    struct pi_i2c_conf conf;

    /* Init & open i2c. */
    pi_i2c_conf_init(&conf);
    conf.itf = 0;
    pi_i2c_conf_set_slave_addr(&conf, EEPROM_DEVICE_ADDRESS, 0);
    pi_i2c_conf_set_wait_cycles(&conf, 2048);
    pi_open_from_conf(&i2c, &conf);
    if (pi_i2c_open(&i2c))
    {
        PRINTF("I2c open failed !\n");
        pmsis_exit(-1);
    }

    int32_t timeout_id = 0;
    for (uint8_t i=0; i<2; i++)
    {
        timeout_id = pi_udma_timeout_alloc(PI_UDMA_TIMEOUT_MODE_TRANSFER);
        if (timeout_id == -1)
        {
            PRINTF("Timeout alloc failed !\n");
            pmsis_exit(-2 - i);
        }
        pi_i2c_ioctl(&i2c, PI_I2C_IOCTL_ATTACH_TIMEOUT_RX + 2*i, (void *) timeout_id);
    }

    data_init((uint32_t) BUFF_SIZE);

    int32_t errors = 0;
    int32_t status = 0;
    for (uint32_t iter=0; iter<(uint32_t) NB_ITER; iter++)
    {
        PRINTF("Write-Read with timeout_rx=%ld us, timeout_tx=%ld us.\n",
               timeout_us[iter][0], timeout_us[iter][1]);
        status = test_timeout_eeprom_write(&write_buff[0], (uint32_t) (BUFF_SIZE + ADDR_SIZE),
                                           timeout_us[iter][1]);
        if (!status)
        {
            status = test_timeout_eeprom_read(&write_buff[0], &read_buff[0],
                                               (uint32_t) BUFF_SIZE, timeout_us[iter][0]);
            if (!status)
            {
                status = test_timeout_eeprom_check(&write_buff[2], &read_buff[0],
                                                   (uint32_t) BUFF_SIZE);
                //errors = status;
                //break;
            }
        }
        printf("errors=%ld, status=%ld\n", errors, status);
        errors += status;
        status = 0;
    }

    pi_i2c_close(&i2c);

    if ((errors != (int32_t) NB_FAIL) || (status != 0))
    {
        PRINTF("Test failed !\n");
    }
    else
    {
        PRINTF("Test success !\n");
        errors = 0;
    }
    pmsis_exit(errors);
}

int main(void)
{
    return pmsis_kickoff((void*) test_entry);
}

