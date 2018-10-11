#include "mbed.h"

SPI spi(SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CSN0); // mosi, miso, sclk, ssel

// Chip select is contolled by inner ip
#define BUFFER_SIZE 512
#define P_ERR 6
#define E_ERR 5
#define DUMMY 15

GAP_L2_DATA uint8_t SPI_RX_BUFFER[BUFFER_SIZE] ;  // 2560 bits
GAP_L2_DATA uint8_t SPI_TX_BUFFER[BUFFER_SIZE];

spi_command_sequence_t sequence;

static void conf_flash()
{
    spi.udma_cs(0);
    spi.write(0x06);
    spi.udma_cs(1);

    // Set dummy cycles
    memset(&sequence, 0, sizeof(spi_command_sequence_t));
    sequence.cmd       = 0x71;
    sequence.cmd_bits  = 8;
    sequence.addr_bits = 32;
    sequence.addr      = (0x80000380 | DUMMY);
    spi.transfer_command_sequence(&sequence);

    spi.udma_cs(0);
    spi.write(0x06);
    spi.udma_cs(1);

    // Set page size to 512
    memset(&sequence, 0, sizeof(spi_command_sequence_t));
    sequence.cmd       = 0x71;
    sequence.cmd_bits  = 8;
    sequence.addr_bits = 32;
    sequence.addr      = 0x00800004;
    sequence.tx_bits   = 8;
    sequence.tx_data   = 0x10;
    spi.transfer_command_sequence(&sequence);
}

static void erase_page_in_flash()
{
    spi.udma_cs(0);
    spi.write(0x06);
    spi.udma_cs(1);

    memset(&sequence, 0, sizeof(spi_command_sequence_t));
    sequence.cmd       = 0x20;
    sequence.cmd_bits  = 8;
    sequence.addr_bits = 32;
    sequence.addr      = 0;
    spi.transfer_command_sequence(&sequence);
}

static void write_page_in_flash()
{
    spi.udma_cs(0);
    spi.write(0x06);
    spi.udma_cs(1);

    //p4pp age program
    memset(&sequence, 0, sizeof(spi_command_sequence_t));
    sequence.cmd       = 0x02;
    sequence.cmd_bits  = 8;
    sequence.addr_bits = 32;
    sequence.addr      = 0;
    sequence.tx_bits   = BUFFER_SIZE*8;
    sequence.tx_buffer = SPI_TX_BUFFER;
    spi.transfer_command_sequence(&sequence);
}

static void read_page_from_flash(uint32_t flash_addr)
{
    memset(&sequence, 0, sizeof(spi_command_sequence_t));

    sequence.cmd       = 0x0C;
    sequence.cmd_bits  = 8;
    sequence.addr_bits = 32;
    sequence.addr      = flash_addr;
    sequence.rx_bits   = BUFFER_SIZE*8;
    sequence.dummy     = DUMMY;
    sequence.rx_buffer = SPI_RX_BUFFER;

    spi.transfer_command_sequence(&sequence);
}

static int wait_process_end(uint32_t err_bit)
{
    // Wait the end of erasing process
    // Status Register 1(SR1V)
    // bit 0 -> 1-busy 0-idle
    // bit 1 -> 1-we
    // bit 6 -> 1-error 0-OK
    uint16_t read_status;
    do {
        spi.udma_cs(0);
        spi.write(0x05);
        read_status = spi.write(0x00);
        spi.udma_cs(1);
        //printf("read_status = %x\n", read_status);

        if (read_status & (1 << err_bit))
        {
            printf("P_ERR bit = 1, an error occurs in the programming operation\n");
            return 1;
        }

    } while(read_status & 0x01);

    return 0;
}

int main() {
    /* Initialize the buffer */
    for (int i = 0; i < BUFFER_SIZE; i++)
        SPI_TX_BUFFER[i] = i;

    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);

    // Configure flash
    conf_flash();

    // Erase page
    erase_page_in_flash();
    printf("Erasing Flash ... \n");
    wait_process_end(E_ERR);

    // Write buffer
    write_page_in_flash();
    printf("Writing Flash ... \n");
    wait_process_end(P_ERR);

    // Read back buffer
    printf("Reading Flash ... \n");
    read_page_from_flash(0);


    // Check read values
    int error = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %x ", SPI_RX_BUFFER[i]);
        if(SPI_RX_BUFFER[i] != (char)(i)) error++;
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
