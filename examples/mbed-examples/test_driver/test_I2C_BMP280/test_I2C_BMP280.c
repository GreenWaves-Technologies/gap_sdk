#include<string.h>
// MBED OS
#include "cmsis_os2.h"
#include "i2c_api.h"

#define BMP280_Device_Address    0xEC

#define BMP280_ID_REG            0xD0
#define BMP280_ID_VALUE          0x58

#define BMP280_RESET_REG         0xE0

GAP_L2_DATA uint8_t id_read = 0;
GAP_L2_DATA uint8_t cmd[] = {BMP280_ID_REG};

int main()
{

    printf("Fabric controller code execution for mbed_os I2C driver BMP280 test\n");

    i2c_t i2c0;
    /* I2C pins init, I2C udma channel init */
    i2c_init(&i2c0, I2C0_SDA, I2C0_SCL);

    printf("I2C0_SDA  = %d \n", I2C0_SDA);
    printf("I2C0_SCL  = %d \n", I2C0_SCL);

    /* Set fequence to 100kHz */
    uint32_t freq = 100000;
    i2c_frequency(&i2c0, freq);
    printf("Frequncy set done, %d Hz\n", freq);

    /* Read ID value iof BMP280 */
    i2c_write(&i2c0, BMP280_Device_Address, (const char*)&cmd, 1, 0);
    i2c_read(&i2c0, BMP280_Device_Address, (char*)&id_read, 1, 1);

    printf ("BMP280_ID_VALUE : %X\n", id_read);

    if (id_read == BMP280_ID_VALUE)
    {
        printf("Test success\n");
        return 0;
    } else {
        printf("Test failed\n");
        return -1;
    }
}
