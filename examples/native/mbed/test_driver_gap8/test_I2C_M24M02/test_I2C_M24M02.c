#include<string.h>
// MBED OS
#include "cmsis_os2.h"
#include "i2c_api.h"
#include "mbed_wait_api.h"

#ifdef __PLATFORM_RTL__
#define M24M02_Device_Address    0xA8
#else
#define M24M02_Device_Address    0xA0
#endif


GAP_L2_DATA uint8_t init_buffer[] = {0x00, 0x50, 0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};
GAP_L2_DATA uint8_t tx_buffer[]   = {0x00, 0x50, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
GAP_L2_DATA uint8_t rx_addr[]     = {0x00, 0x50};
GAP_L2_DATA uint8_t rx_buffer[16];

int main()
{

    printf("Fabric controller code execution for mbed_os I2C driver M24M02 test\n");

    i2c_t i2c0;
    /* I2C pins init, I2C udma channel init */
    i2c_init(&i2c0, I2C0_SDA, I2C0_SCL);

    printf("I2C0_SDA  = %d \n", I2C0_SDA);
    printf("I2C0_SCL  = %d \n", I2C0_SCL);

    /* Set fequence to 100kHz */
    uint32_t freq = 100000;
    i2c_frequency(&i2c0, freq);
    printf("Frequncy set done, %d Hz\n", freq);

    /* Init the zone where we do the test */
    i2c_write(&i2c0, M24M02_Device_Address, (const char*)init_buffer, sizeof(init_buffer), 1);
    wait(0.01);

    /* Write into EEPROM */
    i2c_write(&i2c0, M24M02_Device_Address, (const char*)tx_buffer, sizeof(tx_buffer), 1);
    wait(0.01);

    /* Read from EEPROM */
    i2c_write(&i2c0, M24M02_Device_Address, (const char*)rx_addr, 2, 0);
    i2c_read(&i2c0, M24M02_Device_Address, (char*)rx_buffer, 16, 1);

    int error = 0;
    for (int i = 0; i< 16; i++) {
        if (rx_buffer[i] != i) error++;
        printf("I2C rec=%02x\n", rx_buffer[i]);
    }

    if (error == 0) {
        printf("Test success\n");
        return 0;
    } else {
        printf("Test failed\n");
        return -1;
    }
}
