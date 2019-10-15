#include "mbed.h"

SPI spi(SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CSN0_A9); // mosi, miso, sclk, ssel

const uint8_t buffer[] = "How do you do today\r\n";

static void spi_callback(int events)
{
    if(events == SPI_EVENT_COMPLETE)
    {
        printf("Callback, event = %d!\n", events);
        printf("Test success!\n");
    } else {
        printf("Test failed!\n");
    }
}

int main() {
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);

    event_callback_t event;

    event = spi_callback;

    spi.transfer((uint8_t *)buffer, sizeof(buffer), (uint8_t *)NULL, 0, event);

    wait(0.01);

    return 0;
}
