/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://github.com/ARMmbed/spif-driver/blob/master/SPIFBlockDevice.cpp
 */
// Here's an example using the MX25R SPI flash device on the GAP8
#include "mbed.h"
#include "SPIFBlockDevice.h"

// Create flash device on SPI bus with PTE5 as chip select
SPIFBlockDevice spif(SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CSN1, 15000000);

int main() {
    printf("spif test\n");

    // Initialize the SPI flash device and print the memory layout
    spif.init();
    printf("spif size: %llu\n",         spif.size());
    printf("spif read size: %llu\n",    spif.get_read_size());
    printf("spif program size: %llu\n", spif.get_program_size());
    printf("spif erase size: %llu\n",   spif.get_erase_size());

    // Write "Hello World!" to the first block
    char *buffer = (char*)malloc(spif.get_erase_size());
    sprintf(buffer, "Hello World!\n");
    spif.erase(0, spif.get_erase_size());
    spif.program(buffer, 0, spif.get_erase_size());

    // Read back what was stored
    spif.read(buffer, 0, spif.get_erase_size());
    printf("%s", buffer);

    // Deinitialize the device
    spif.deinit();
    free(buffer);
}
