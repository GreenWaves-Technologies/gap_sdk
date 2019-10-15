#include<string.h>
// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "spi_multi_api.h"

#define BOOT_WITH_QSPI  0

#define BUFFER_SIZE 256
#define P_ERR 6
#define E_ERR 5
#define DUMMY 8

#define FAST_READ  1

GAP_L2_DATA uint8_t SPI_RX_BUFFER[BUFFER_SIZE];
GAP_L2_DATA uint8_t SPI_TX_BUFFER[BUFFER_SIZE];

spi_command_sequence_t s_command;

uint32_t reg;

static uint32_t read_register(spi_t *spim, uint8_t cmd, int size)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = cmd;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.rx_bits   = (size << 3);
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Single;

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
    /* Set QPI mode - Single */
    write_enable(spim, uSPI_Single);

    // Set dummy cycles
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      =  (0x80000300 | DUMMY);
    s_command.cmd_mode  =  uSPI_Single;
    s_command.addr_mode =  uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void erase_page_in_flash(spi_t *spim)
{
    write_enable(spim, uSPI_Single);

    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x20;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 24;
    s_command.addr      = 0;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void write_page_in_flash(spi_t *spim)
{
    write_enable(spim, uSPI_Single);

    //p4pp age program
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x02;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 24;
    s_command.addr      = 0;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_bits   = BUFFER_SIZE*8;
    s_command.tx_data   = 0;
    s_command.tx_buffer = (uint8_t *)SPI_TX_BUFFER;
    s_command.data_mode = uSPI_Single;
    spi_master_transfer_command_sequence(spim, &s_command);
}

static void read_page_from_flash(spi_t *spim, uint32_t flash_addr)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    /* Fast read - 133 MHz, Normal read - 50 MHz */
    if( FAST_READ == 1) {
        s_command.cmd       = 0x0B;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 24;
        s_command.addr      = flash_addr;
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.rx_bits   = BUFFER_SIZE*8;
        s_command.dummy     = DUMMY;
        s_command.rx_buffer = (uint8_t *)SPI_RX_BUFFER;
        s_command.data_mode = uSPI_Single;
    } else {
        s_command.cmd       = 0x03;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 24;
        s_command.addr      = flash_addr;
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.rx_bits   = BUFFER_SIZE*8;
        s_command.rx_buffer = (uint8_t *)SPI_RX_BUFFER;
        s_command.data_mode = uSPI_Single;
    }

    spi_master_transfer_command_sequence(spim, &s_command);
}

static int wait_process_end(spi_t *spim, uint32_t err_bit)
{
    // Wait the end of erasing process
    // Status Register 1(SR1V)
    // bit 0 -> 1-busy 0-idle
    // bit 1 -> 1-we
    // bit 6 -> 1-error 0-OK
    uint8_t ret;
    do {
        ret = read_register(spim, 0x05, 4);

        if (ret & (1 << err_bit)) {
            printf("P_ERR bit = 1, an error occurs in the programming operation\n");
            return 1;
        }

    } while(ret & 0x01);

    return 0;
}

int test_spim(int spim_id, PinName mosi, PinName miso, PinName sclk, PinName csn)
{
    spi_t spim;
    /* SPI pins init, SPI udma channel init */
    spi_init(&spim, mosi, miso, sclk, csn);

    /* SPI bits, cpha, cpol configuration */
    spi_format(&spim, 8, 0, 0);

    /* Set fequence to 10MHz */
    spi_frequency(&spim, 10000000);

    /* Flash Configuration */
    if ((spim_id == 0) && BOOT_WITH_QSPI)
    {
        conf_flash(&spim);
    }

    /* Read ID in single mode */
    int id = read_register(&spim, 0x9f, 4);
    printf("SPIM%d ID = %x\n", spim_id, id);

    /* Erase page */
    erase_page_in_flash(&spim);
    printf("Erasing Flash ... \n");
    wait_process_end(&spim, E_ERR);

    /* Write buffer */
    write_page_in_flash(&spim);
    printf("Writing Flash ... \n");
    wait_process_end(&spim, P_ERR);

    /* Read back buffer */
    printf("Reading Flash ... \n");
    read_page_from_flash(&spim, 0);

    /* SPI free */
    spi_free(&spim);

    // Check read values
    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        #if DEBUG == 1
        printf(" %x ", SPI_RX_BUFFER[i]);
        #endif
        if(SPI_RX_BUFFER[i] != (char)(i)) error++;
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os SPI master 0-3 driver test\n");

    /* Initialize the buffer */
    for (int i = 0; i< BUFFER_SIZE; i++)
        SPI_TX_BUFFER[i] = i;

    int error = 0;
    // SPIM0
    error = test_spim(0, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CSN0);
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    // SPIM1
    error = test_spim(1, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CSN0);
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    // SPIM2
    error = test_spim(2, SPI2_MOSI, SPI2_MISO, SPI2_SCLK, SPI2_CSN0);
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    // SPIM3
    error = test_spim(3, SPI3_MOSI, SPI3_MISO, SPI3_SCLK, SPI3_CSN0);
    if (error) printf("Test failed with %d errors\n", error);
    else printf("\nTest success\n");

    return error;
}
