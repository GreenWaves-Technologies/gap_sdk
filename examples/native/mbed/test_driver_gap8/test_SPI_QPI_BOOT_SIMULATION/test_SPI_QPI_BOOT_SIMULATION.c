#include<string.h>
// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "spi_multi_api.h"

#define BUFFER_SIZE 16
#define P_ERR 6
#define E_ERR 5
#define DUMMY 15

GAP_L2_DATA char SPI_RX_BUFFER[BUFFER_SIZE] ;  // 2560 bits

spi_command_sequence_t s_command;

static void write_enable(spi_t *spim, uint8_t qpi)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x06;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = qpi;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void spi_conf_flash(spi_t *spim)
{
    write_enable(spim, uSPI_Single);

    // Set dummy cycles
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = (0x80000202);
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);

    write_enable(spim, uSPI_Single);

    // Set dummy cycles
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = (0x8000030f);
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);
}


static void read_page_from_flash(spi_t *spim, unsigned int flash_addr)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0xEC;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = flash_addr;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Quad;
    s_command.rx_bits   = BUFFER_SIZE*8;
    s_command.alter_data_bits = 8;
    s_command.alter_data      = 0x00;
    s_command.alter_data_mode = uSPI_Quad;
    s_command.dummy     = 15;
    s_command.rx_buffer = (uint8_t *)SPI_RX_BUFFER;
    s_command.data_mode = uSPI_Quad;

    spi_master_transfer_command_sequence(spim, &s_command);
}


int main()
{
    printf("Fabric controller code execution for mbed_os SPI boot simulation : write 0 - f in flash first, then test!\n");

    spi_t spim0;

    /* SPI pins init, SPI udma channel init */
    spi_init(&spim0, SPI0_SDIO0, SPI0_SDIO1, SPI0_SCLK, SPI0_CSN0);

    printf("SPI0_MOSI  = %d \n", SPI0_SDIO0);
    printf("SPI0_MISO  = %d \n", SPI0_SDIO1);
    printf("SPI0_SDIO2 = %d \n", SPI0_SDIO2);
    printf("SPI0_SDIO3 = %d \n", SPI0_SDIO3);
    printf("SPI0_CSN0  = %d \n", SPI0_CSN0);
    printf("SPI0_SCLK  = %d \n", SPI0_SCLK);

    /* SPI bits, cpha, cpol configuration */
    /* Important!!!!!! */
    spi_format(&spim0, 32, 0, 0);

    // Flash Configuration
    spi_conf_flash(&spim0);

    /* Set QPI mode */
    spi_master_qspi(&spim0, 1);

    // Read back buffer
    printf("Reading Flash ... \n");
    read_page_from_flash(&spim0, 0);

    /* SPI free */
    spi_free(&spim0);

    // Check read values
    int error = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %x ", SPI_RX_BUFFER[i]);
        if(SPI_RX_BUFFER[i] != (char)(i)) error++;
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("\nTest success\n");

    return error;
}
