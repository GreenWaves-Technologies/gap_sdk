/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://github.com/ARMmbed/i2cee-driver/
 */
// Here's an example using a 24LC256 on a GR PEACH
#include "mbed.h"
#include "I2CEEBlockDevice.h"

// Create EEPROM device on I2C bus with 32kbytes of memory
I2CEEBlockDevice i2cee(I2C0_SDA, I2C0_SCL, 0xA0, 256*1024, 256, 200000);

GAP_L2_DATA uint8_t CMD2[] = {0x00, 0x00};

int main() {
    printf("i2cee test\n");

    // Initialize the device and print the memory layout
    i2cee.init();
    printf("i2cee size: %llu\n",         i2cee.size());
    printf("i2cee read size: %llu\n",    i2cee.get_read_size());
    printf("i2cee program size: %llu\n", i2cee.get_program_size());
    printf("i2cee erase size: %llu\n",   i2cee.get_erase_size());

    // Write "Hello World!" to the first block
    char *buffer = (char*)malloc(i2cee.get_erase_size());
    sprintf(buffer, "Hello World!\n");
    i2cee.erase(0, i2cee.get_erase_size());
    i2cee.program(buffer, 0, i2cee.get_erase_size());

    // Read back what was stored
    i2cee.read(buffer, 0, i2cee.get_erase_size());
    printf("%s", buffer);

    // Deinitialize the device
    i2cee.deinit();
}
