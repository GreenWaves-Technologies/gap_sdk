#include "mbed.h"

// SPI0_CSN0 Chip select is contolled by inner UDMA
SPI spi(SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CSN0); // mosi, miso, sclk, ssel

int main() {
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);

    // Select the device by seting chip select low
    spi.udma_cs(0);

    // Send 0x8f, the command to read the WHOAMI register
    spi.write(0x8F);

    // Send a dummy byte to receive the contents of the WHOAMI register
    int whoami = spi.write(0x00);
    printf("WHOAMI register = 0x%X\n", whoami);

    // Deselect the device
    spi.udma_cs(1);

    return 0;
}
