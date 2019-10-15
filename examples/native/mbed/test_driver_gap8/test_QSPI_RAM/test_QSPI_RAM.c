// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This example is intended to work with
//                     APM's PSRAM part# APS1604M-SQ
//  For other part #, availablle opcodes and required read/write latencies
//  may be different. You should check the datasheet.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



#include<string.h>

#include "cmsis_os2.h"

#include "spi_multi_api.h"

#include "mbed_wait_api.h"



// ------------------------------------------

// Uncomment to print out every byte read back (vs. expected if relevant) :
//#define VERBOSE_READBACK

#define SPI_FQCY_MHz  12
// Max = core clk /2 -- Granularity = core_clk/2,/3,/4,etc.

//** BEWARE ** !!
// on APM PSRAM there is a max duration for CS# low, hence for Page Size * Clock Period **
// e.g. on APS1604M-SQ it's t(CEM)= 8us (std temp range) / 4us (extended temp range)

#define PAGE_SIZE 64                    // Fixed page size of target PSRAM
#define TOTAL_TRANSFER_SIZE  (1024*PAGE_SIZE) // must it a multiple of PAGE_SIZE  (max here = (L2size - margin)/2 for RX and TX buff so <256KB)
#define ADD_OFFSET  0x00080030          // to start accesses at an address different from 0x00000000

// ------------------------------------------

static void read_page_from_ram(spi_t *spim, uint32_t ram_addr, uint8_t spi_mode);
static void write_page_in_ram(spi_t *spim, uint32_t ram_addr, uint8_t spi_mode);


// ------------------------------------------


GAP_L2_DATA uint8_t     RX_BUFFER[TOTAL_TRANSFER_SIZE];
GAP_L2_DATA uint8_t     TX_BUFFER[TOTAL_TRANSFER_SIZE];

spi_command_sequence_t s_command;
spi_t   spim0;



// =========================================================================================

int main()
{

    printf("---------------------\n");
    printf("RAM SPI test\n");

#if defined VERBOSE_READBACK
    printf("\nVERBOSE_READBACK defined: Result of each read will be printed out\n");
#endif

    printf("---------------------\n\n");

    uint32_t error = 0;

    // --------- GAP8 SPI Configutation   -----------------------------------

    /* SPI pins init, SPI udma channel init */
    //    spi_init(&spim0, SPI0_SDIO0, SPI0_SDIO1, SPI0_SCLK, SPI0_CSN0);
    // default is actually ok both for SPI and QPI...

    printf("SPI0_MOSI  = %d \n", SPI0_SDIO0);
    printf("SPI0_MISO  = %d \n", SPI0_SDIO1);
    printf("SPI0_SDIO2 = %d \n", SPI0_SDIO2);
    printf("SPI0_SDIO3 = %d \n", SPI0_SDIO3);
    printf("SPI0_CSN0  = %d \n", SPI0_CSN0);
    printf("SPI0_SCLK  = %d \n", SPI0_SCLK);


    /* SPI bits, cpha, cpol configuration */
    spi_format(&spim0, 8, 0, 0);

    /* Set SPI fequency */
    spi_frequency(&spim0, SPI_FQCY_MHz*1000000);

    // SPI Configuration -- Start in single SPI mode
    spi_master_qspi(&spim0, uSPI_Single);


    // ---------  After power-up, reset RAM   -----------------------------------

    // *** BEWARE -- CS# must go back high between the 2 commands
    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x66); //RSTEN
    spi_master_cs(&spim0, 1);
    wait(0.1);
    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x99); //RST
    spi_master_cs(&spim0, 1);

    printf("RAM was reset after power-up\n");


    //  ---------  Change PSRAM I/O drive strength  -----------------------------------

    // This may depend on RAM part and on board
    // APS1604M-SQ drives at Rout = 50ohm by default -- THIS DID NOT WORK WITH TEST BOARD
    // (although it already had 330ohm series res. on SPI lines)
    // APS1604M-SQ allows to also drive under 100 or 200ohm, which worked

    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0xB1); //MR Write
    spi_master_write(&spim0, 0x00); //MR Add = 0x000000, Byte2
    spi_master_write(&spim0, 0x00); //MR Add = 0x000000, Byte1
    spi_master_write(&spim0, 0x00); //MR Add = 0x000000, Byte0
    spi_master_write(&spim0, 0x72); //0x72 for 200ohm, 0x71 for 100 ohm, 0x70 for 50ohm
    spi_master_cs(&spim0, 1);
    wait(0.1);

    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0xB5); //MR Read (requires 8 cycles latency)
    spi_master_write(&spim0, 0x00); //MR Add = 0x000000, Byte2
    spi_master_write(&spim0, 0x00); //MR Add = 0x000000, Byte1
    spi_master_write(&spim0, 0x00); //MR Add = 0x000000, Byte0
    for (uint8_t i=0; i<8; i++)
    {
        spi_master_write(&spim0, 0x00);         // latency cycle1
    }
    uint8_t  ModeReg = spi_master_write(&spim0, 0x00);
    spi_master_cs(&spim0, 1);
    printf("Mode Register now = 0x%x\n", ModeReg);


    // --------- Sanity Check: Read ID in single mode     -----------------------

    uint8_t id[8];
    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x9f);         // RDID Read Identification Command

    spi_master_write(&spim0, 0x00);         // Dummy 24-bit address    byte1
    spi_master_write(&spim0, 0x00);         // Dummy 24-bit address    byte2
    spi_master_write(&spim0, 0x00);         // Dummy 24-bit address    byte3


    id[0] = spi_master_write(&spim0, 0x00); // Manuf. ID (normally 0x0D)
    id[1] = spi_master_write(&spim0, 0x00); // KGD (normally Ox5D)
    id[2] = spi_master_write(&spim0, 0x00); // EID MSByte
    id[3] = spi_master_write(&spim0, 0x00);
    id[4] = spi_master_write(&spim0, 0x00);
    id[5] = spi_master_write(&spim0, 0x00);
    id[6] = spi_master_write(&spim0, 0x00);
    id[7] = spi_master_write(&spim0, 0x00);  // EID LSByte
    spi_master_cs(&spim0, 1);

    printf("Manuf ID = %x  (normally 0x0D)\n", (uint16_t)id[0]);
    printf("KGD = %x  (normally 0x5D)\n", (uint16_t)id[1]);
    printf("EID = 0x");
    for (uint8_t i=0; i<6; i++)
    {
        printf("%1x", id[i+2]);
    }
    printf("\n");


    // 1.a) ===  Single SPI, single byte accesses    ===========================

    printf("\nSingle SPI Test\n\n");

    // Init Tx Buffer
    for (uint32_t i=0; i<TOTAL_TRANSFER_SIZE; i++)  // i as address of start of page in Flash
    {
        TX_BUFFER[i] = i*2+1;  // 0 to 255 then wrap-around (8 bits!)
    }

    // SPI Configuration -- single SPI mode
    spi_master_qspi(&spim0, uSPI_Single);

    uint8_t write_val =  0x88;
    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x02);         // Single SPI Write Command
    spi_master_write(&spim0, 0xAA);         // Random address, byte1
    spi_master_write(&spim0, 0xBB);         // Random address, byte2
    spi_master_write(&spim0, 0xCC);         // Random address, byte3
    spi_master_write(&spim0, write_val);         // Some write value
    spi_master_cs(&spim0, 1);
    wait(0.1);

    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x03);         // Single SPI Slow Read Command
    spi_master_write(&spim0, 0xAA);         // read address, byte1
    spi_master_write(&spim0, 0xBB);         // read address, byte2
    spi_master_write(&spim0, 0xCC);         // read address, byte3
    uint8_t read_val =  spi_master_write(&spim0, 0x00);
    spi_master_cs(&spim0, 1);
    if (read_val == write_val)
    {
        printf("Single Byte SPI slow speed read OK\n");
    }
    else
    {
        printf("Single Byte SPI slow speed read FAILED\n");
    }

    // -----

    write_val =  0x22;
    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x02);         // Single SPI Write Command
    spi_master_write(&spim0, 0xAA);         // some write address, byte1
    spi_master_write(&spim0, 0xBB);         // write address, byte2
    spi_master_write(&spim0, 0xCC);         // write address, byte3
    spi_master_write(&spim0, write_val);         // Some write value
    spi_master_cs(&spim0, 1);
    wait(0.1);

    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x0B);         // Single SPI Fast Read Command
    spi_master_write(&spim0, 0xAA);         // read address, byte1
    spi_master_write(&spim0, 0xBB);         // read address, byte2
    spi_master_write(&spim0, 0xCC);         // read address, byte3
    spi_master_write(&spim0, 0x00);         // Need 8 clock cycles waiting (eq. to 1 byte) in Fast Read...
    read_val =  spi_master_write(&spim0, 0x00);
    spi_master_cs(&spim0, 1);
    if (read_val == write_val)
    {
        printf("Single Byte SPI fast speed read OK\n");
    }
    else
    {
        printf("Single Byte SPI fast speed read FAILED\n");
    }


    // 1.b) ===  Single SPI, page access     ===================================

    // ---------- write someting in RAM   ----------------------------------------

    printf("Writing page in RAM (single SPI)... \n");

    for (uint32_t start_add=0; start_add<TOTAL_TRANSFER_SIZE; start_add+=PAGE_SIZE)  // i as address of start of page in Flash
    {
        write_page_in_ram(&spim0, start_add, uSPI_Single);    //write PAGE_TX_BUFFER starting at start_add
    }


    // ---------- Read back   ---------------------------------------------------

    printf("Reading RAM (single SPI)... \n");

    for (uint32_t start_add=0; start_add<TOTAL_TRANSFER_SIZE; start_add+=PAGE_SIZE)  // i as address of start of page in Flash
    {
       read_page_from_ram(&spim0, start_add, uSPI_Single );
    }


 // ---------- check what we've read is what we wrote  ------------------------------

    for (uint32_t j = 0; j < TOTAL_TRANSFER_SIZE; j++)
    {

        if( RX_BUFFER[j] == TX_BUFFER[j] )
        {
#ifdef  VERBOSE_READBACK
            printf(" %x ", RX_BUFFER[j]);
#endif
        }
        else
        {
            error++;
#ifdef  VERBOSE_READBACK
            printf(" %x.vs.%x ", RX_BUFFER[j], TX_BUFFER[j]);
#endif
        }
    }

    if (error)
    {
        printf("\nSingle SPI Test failed with %d errors\n", error);
    }
    else
    {
        printf("\nSingle SPI Test success\n");
    }


    // 2) ==  Switch to QPI Mode       ========================================

    printf("\nQuad SPI Test\n\n");

    spi_master_cs(&spim0, 0);
    spi_master_write(&spim0, 0x35);         // Enter Quad Mode Command
    spi_master_cs(&spim0, 1);

    spi_master_qspi(&spim0, uSPI_Quad);


    // 2) ===  Quad SPI, page access     ===================================

    // Re-init Tx Buffer (to different contents)
    for (uint32_t i=0; i<TOTAL_TRANSFER_SIZE; i++)  // i as address of start of page in Flash
    {
        TX_BUFFER[i] =  i*((i/256)+1);
    }

    // ---------- write someting in RAM   ----------------------------------------

    printf("Writing in RAM (QSPI)... \n");

    for (uint32_t start_add=0; start_add<TOTAL_TRANSFER_SIZE; start_add+=PAGE_SIZE)  // i as address of start of page in Flash
    {
        write_page_in_ram(&spim0, start_add, uSPI_Quad);
    }


//    printf("GO\n");
//    wait(2);


    // ---------- Read back   ---------------------------------------------------

    printf("Reading RAM (QSPI)  ... \n");

    for (uint32_t start_add=0; start_add<TOTAL_TRANSFER_SIZE; start_add+=PAGE_SIZE)  // i as address of start of page in Flash
    {
       read_page_from_ram(&spim0, start_add, uSPI_Quad );
    }


 // ---------- check what we've read is what we wrote  ------------------------------

    for (uint32_t j = 0; j < TOTAL_TRANSFER_SIZE; j++)
    {

        if( RX_BUFFER[j] == TX_BUFFER[j] )
        {
#ifdef  VERBOSE_READBACK
            printf(" %x ", RX_BUFFER[j]);
#endif
        }
        else
        {
            error++;
#ifdef  VERBOSE_READBACK
            printf(" %x.vs.%x ", RX_BUFFER[j], TX_BUFFER[j]);
#endif
        }
    }

    if (error)
    {
        printf("\nQuad SPI Test failed with %d errors\n", error);
    }
    else
    {
        printf("\nQuad SPI Test success\n");
    }



    // ===  Back to single SPI Mode       ========================================

    printf("\nBack to single SPI before exit\n\n");

    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0xF5;         // Exit Quad Mode Command
    s_command.cmd_bits  = 8;
    s_command.cmd_mode = uSPI_Quad;
    spi_master_transfer_command_sequence(&spim0, &s_command);

    spi_master_qspi(&spim0, uSPI_Single);




    // ==== END ===================================================================

    /* SPI free */
    spi_free(&spim0);


    return error;


}

// ===================================================================================

static void write_page_in_ram(spi_t *spim, uint32_t ram_addr, uint8_t spi_mode)
{
        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        if (spi_mode== uSPI_Single)
        {
            s_command.cmd       = 0x02; // Write singl SPI Cmd
        }
        else
        {
            s_command.cmd       = 0x38; // Write in QPI mode (can use opcode 0x02 or 0x38)
        }
        s_command.cmd_bits  = 8;
        s_command.cmd_mode  = spi_mode;
        s_command.addr_bits = 24;
        s_command.addr      = (ram_addr+ADD_OFFSET) <<8;
        s_command.addr_mode = spi_mode;
        s_command.tx_bits   = PAGE_SIZE*8;
        s_command.tx_data   = 0;
        s_command.tx_buffer = (uint8_t *)&TX_BUFFER[ram_addr];  //works because we're writing starting at ram_addr=0
        s_command.data_mode = spi_mode;
        spi_master_transfer_command_sequence(&spim0, &s_command);
}



static void read_page_from_ram(spi_t *spim, uint32_t ram_addr, uint8_t spi_mode)
{
        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        if (spi_mode== uSPI_Single)
        {
            s_command.cmd       = 0x03;  //single SPI read
        }
        else
        {
            s_command.cmd       = 0x0B;  // QSPI read (0x0B @4 cycle latency or faster clock capable 0xEB @ 6clock latency)
        }
        s_command.cmd_bits  = 8;
        s_command.cmd_mode  = spi_mode;
        s_command.addr_bits = 24;
        s_command.addr      = (ram_addr+ADD_OFFSET) <<8;
        s_command.addr_mode = spi_mode;
        if (spi_mode== uSPI_Single)      // single SPI
        {
            s_command.dummy = 0;
        }
        else if (s_command.cmd == 0xEB)  // QSPI fast read
        {
            s_command.dummy = 6;
        }
        else    // QSPI std read
        {
            s_command.dummy = 4;
        }
        s_command.rx_bits   = PAGE_SIZE*8;
        s_command.rx_buffer = (uint8_t *)&RX_BUFFER[ram_addr];  //works because we're writing starting at ram_addr=0
        s_command.data_mode = spi_mode;

        spi_master_transfer_command_sequence(&spim0, &s_command);
}
