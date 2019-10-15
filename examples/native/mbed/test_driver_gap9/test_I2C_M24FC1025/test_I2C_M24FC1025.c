#include "m24fc1025.h"

#define BUFFER_SIZE 4

uint8_t rx_buffer[BUFFER_SIZE];

int test_i2c (uint8_t dev_addr)
{
    /* Initialize buffer */
    for (int i = 0; i< BUFFER_SIZE; i++) {
        rx_buffer[i] = 0xFF;
    }


    i2c_t i2c0;
    /* I2C pins init, I2C udma channel init */
    i2c_init(&i2c0, I2C0_SDA, I2C0_SCL);

    /* Set fequence to 100kHz */
    uint32_t freq = 100000;
    i2c_frequency(&i2c0, freq);
    printf("Frequncy = %d Hz\n", freq);

    m24fc1025_init(dev_addr);

    // Write first 16 Bytes of memory
    for (uint8_t addr = 0; addr < BUFFER_SIZE ; addr++) { // Rotate bit
        m24fc1025_write_byte(&i2c0, addr, addr);
    }

    // Read data and write it to buffer
    for (uint32_t addr = 0; addr < BUFFER_SIZE; addr++) {
        rx_buffer[addr] = m24fc1025_read_byte(&i2c0, addr);
    }

    int error = 0;
    for (int i = 0; i< BUFFER_SIZE; i++) {
        if (rx_buffer[i] != i) error++;
        printf("I2C rec=%02x\n", rx_buffer[i]);
    }

    return error;
}

int main()
{

    printf("Fabric controller code execution for mbed_os I2C driver M24FC1025 test\n");

    int error = 0;

    /* Test i2c flash 0 */
    error = test_i2c(0b00);

    /* Test i2c flash 1 */
    error = test_i2c(0b01);

    if (error == 0) {
        printf("Test success\n");
        return 0;
    } else {
        printf("Test failed\n");
        return -1;
    }
}
