#include<string.h>
// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "spi_multi_api.h"

#define ID  0xc21728c2

int main()
{
    printf("Fabric controller code execution for mbed_os SPI driver test\n");

    spi_t spim0;
    /* SPI pins init, SPI udma channel init */
    spi_init(&spim0, SPI0_SDIO0, SPI0_SDIO1, SPI0_SCLK, SPI0_CSN1);

    printf("SPI0_MOSI  = %d \n", SPI0_SDIO0);
    printf("SPI0_MISO  = %d \n", SPI0_SDIO1);
    printf("SPI0_SDIO2 = %d \n", SPI0_SDIO2);
    printf("SPI0_SDIO3 = %d \n", SPI0_SDIO3);
    printf("SPI0_CSN1  = %d \n", SPI0_CSN1);
    printf("SPI0_SCLK  = %d \n", SPI0_SCLK);


    /* SPI bits, cpha, cpol configuration */
    spi_format(&spim0, 32, 0, 0);

    /* Set fequence to 10MHz */
    spi_frequency(&spim0, 10000000);

    uint32_t id = spi_master_read(&spim0, 0x9f);
    printf("ID = %x\n", id);

    if(id != ID) {
        printf("Test failed\n");
        return -1;
    } else {
        printf("Test success\n");
    }

    return 0;
}
