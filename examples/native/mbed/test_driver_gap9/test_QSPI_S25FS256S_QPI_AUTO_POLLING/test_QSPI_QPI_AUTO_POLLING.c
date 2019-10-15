#include<string.h>
// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "spi_multi_api.h"

#define BUFFER_SIZE 512
#define P_ERR 6
#define E_ERR 5
#define DUMMY 15

#define S25FS256_SR_WIP      ((uint8_t)0x01)    /*!< Write in progress */
#define S25FS256_SR_WEL      ((uint8_t)0x02)    /*!< Write enable latch */

GAP_L2_DATA uint8_t SPI_RX_BUFFER[BUFFER_SIZE];
GAP_L2_DATA uint8_t SPI_TX_BUFFER[BUFFER_SIZE];

spi_command_sequence_t s_command;

uint32_t reg;

static uint32_t read_register(spi_t *spim, uint8_t cmd, int size)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = cmd;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.rx_bits   = (size << 3);
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Quad;

    spi_master_transfer_command_sequence(spim, &s_command);

    return reg;
}

static void write_enable(spi_t *spim, uint8_t qpi)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x06;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = qpi;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void conf_flash(spi_t *spim)
{
    write_enable(spim, uSPI_Single);

    // Set dummy cycles
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = (0x80000380 | DUMMY);
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);

    write_enable(spim, uSPI_Single);

    // Set page size to 512
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = 0x00800004;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_bits   = 8;
    s_command.tx_data   = 0x10;
    s_command.data_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);

    write_enable(spim, uSPI_Single);

    // QPI enable:  CR2V[6] = 1
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = 0x00800003;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_bits   = 8;
    s_command.tx_data   = (0xC0 | DUMMY);
    s_command.data_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void erase_page_in_flash(spi_t *spim)
{
    write_enable(spim, uSPI_Quad);

    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x20;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = 0;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_mode = uSPI_Quad;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void write_page_in_flash(spi_t *spim)
{
    write_enable(spim, uSPI_Quad);

    //p4pp age program
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x02;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = 0;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_bits   = BUFFER_SIZE*8;
    s_command.tx_data   = 0;
    s_command.tx_buffer = SPI_TX_BUFFER;
    s_command.data_mode = uSPI_Quad;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void read_page_from_flash(spi_t *spim, unsigned int flash_addr)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0xEC;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = flash_addr;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_mode = uSPI_Quad;
    s_command.rx_bits   = BUFFER_SIZE*8;
    s_command.alter_data_bits = 8;
    s_command.alter_data      = 0x0A;
    s_command.alter_data_mode = uSPI_Quad;
    s_command.dummy     = DUMMY;
    s_command.rx_buffer = (uint8_t *)SPI_RX_BUFFER;
    s_command.data_mode = uSPI_Quad;

    spi_master_transfer_command_sequence(spim, &s_command);
}

static void wait_process_end(spi_t *spim, unsigned int err_bit)
{
    // Wait the end of erasing process
    // Status Register 1(SR1V)
    // bit 0 -> 1-busy 0-idle
    // bit 1 -> 1-we
    // bit 6 -> 1-error 0-OK
    spi_polling_config_t conf;
    conf.csn         = 0;
    conf.cmd         = 0x05;
    conf.cmd_mode    = uSPI_Quad;
    conf.match_type  = uSPI_Match_Bits_Zero;
    conf.match_bits  = 8;
    conf.match_value = 0XFFFF & ~(S25FS256_SR_WIP);
    conf.data_mode   = uSPI_Quad;
    conf.time_out    = 0xF000;

    if (spi_master_auto_polling(spim, &conf) == uStatus_Fail)
        printf ("Polling Failed...\n");
}

int main()
{
    printf("Fabric controller code execution for mbed_os SPI driver test\n");

    /* Initialize the buffer */
    for (int i = 0; i< BUFFER_SIZE; i++)
        SPI_TX_BUFFER[i] = i;

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
    spi_format(&spim0, 8, 0, 0);

    /* Set fequence to 10MHz */
    spi_frequency(&spim0, 10000000);

    // Flash Configuration
    conf_flash(&spim0);

    /* Set QPI mode */
    spi_master_qspi(&spim0, uSPI_Quad);

    /* Read ID in QPI mode */
    int id = read_register(&spim0, 0x9f, 4);
    printf("ID = %x\n", id);

    // Erase page
    erase_page_in_flash(&spim0);
    printf("Erasing Flash ... \n");
    wait_process_end(&spim0, E_ERR);

    // Write buffer
    write_page_in_flash(&spim0);
    printf("Writing Flash ... \n");
    wait_process_end(&spim0, P_ERR);

    // Read back buffer
    printf("Reading Flash ... \n");
    read_page_from_flash(&spim0, 0);

    /* SPI free */
    spi_free(&spim0);

    // Check read values
    int error = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        #if DEBUG == 1
        printf(" %x ", SPI_RX_BUFFER[i]);
        #endif
        if(SPI_RX_BUFFER[i] != (char)(i)) error++;
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("\nTest success\n");

    return error;
}
