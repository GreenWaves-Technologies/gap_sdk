#include "mbed.h"
#include "HYPERBUSBlockDevice.h"

// Create flash device on SPI bus with PTE5 as chip select
HYPERBUSBlockDevice bd(HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                       HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                       HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);

int main() {
    printf("hyperbus filesystem test\n");

    // Initialize the SPI flash device and print the memory layout
    bd.init();
    printf("hyperbus filesystem size: %llu\n",         bd.size());
    printf("hyperbus filesystem read size: %llu\n",    bd.get_read_size());
    printf("hyperbus filesystem program size: %llu\n", bd.get_program_size());
    printf("hyperbus filesystem erase size: %llu\n",   bd.get_erase_size());

    // Write "Hello World!" to the first block
    char *buffer = (char*)malloc(bd.get_erase_size());

    sprintf(buffer, "Hello World!\n");
    bd.erase(0, bd.get_erase_size());
    bd.program(buffer, 0, bd.get_erase_size());

    // Read back what was stored
    bd.read(buffer, 0, bd.get_erase_size());
    printf("%s\n", buffer);

    // Deinitialize the device
    bd.deinit();
    free(buffer);
}
