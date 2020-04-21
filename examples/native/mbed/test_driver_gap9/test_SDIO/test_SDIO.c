// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"
// SPI CMSIS driver
#include "spi_multi_api.h"
#include "stdlib.h"

#define SDIO_4BITS 1

#define BLOCK_SIZE 512

/* REAL_ADDR = BLOCK_COUNT * BLOCK_SIZE */
#define BLOCK_COUNT ( 0 )

#define SDIO_READ 1
#define SDIO_WRITE 0

uint8_t RX_BUFFER[BLOCK_SIZE * 2];
uint8_t TX_BUFFER[BLOCK_SIZE * 2];

// 128 bits response
volatile uint32_t resp[4] = {0};

/* -------------------------------------------------------------------------
   -- SDIO CMDs
   -------------------------------------------------------------------------
   See page 49 (60 pdf numbering) of Physical Layer Simplified Spec Ver2.00
*/
#define CMD0   0x00   // GO_IDLE_STATE
#define CMD2   0x20 // ALL_SEND_CID
#define CMD3   0x30 // SEND_RELATIVE_ADDR
#define ACMD6  0x60
#define CMD7   0x70
#define CMD8   0x80
#define CMD55  0x370
#define ACMD41 0x290 // ACMD, need to be used after APP_SPEC_CMD
#define CMD8   0x80
#define CMD9   0x090
#define CMD10  0x0A0
#define CMD11  0x0B0
#define CMD12  0x0C0
#define CMD13  0x0D0
#define CMD16  0x100
#define CMD17  0x110 // READ_SINGLE_BLOCK
#define CMD18  0x120
#define CMD24  0x180 // WRITE_SINGLE_BLOCK
#define CMD25  0x190 // WRITE_MULTIPLE_BLOCK

/* Response type */
#define RSP_48_CRC     0x1
#define RSP_48_NO_CRC  0x2
#define RSP_136        0x3
#define RSP_48_BUSY    0x4

typedef struct {
    __IO  uint32_t RX_DEST;
    __IO  uint32_t TX_DEST;
    __IO  uint32_t TRANS_AUTO;
    __IO  uint32_t TRANS_ADDR;
    __IO  uint32_t TRANS_SIZE;
    __IO  uint32_t TRANS_CFG;
    __IO  uint32_t reserved0;
    __IO  uint32_t reserved1;
    __IO  uint32_t EXT_ADDR;                 /**< HYPERBUS Memory access address register, offset: 0x20 */
    __IO  uint32_t TIMING_CFG;               /**< HYPERBUS Timing Configuration register, offset: 0x24 */
    __IO  uint32_t MBR0;                     /**< HYPERBUS Memory base address 0 register, offset: 0x28 */
    __IO  uint32_t MBR1;                     /**< HYPERBUS Memory base address 1 register, offset: 0x2C */
    __IO  uint32_t DEVICE_TYPE;              /**< HYPERBUS device type register, offset: 0x30 */
    __IO  uint32_t OSPI_CMD;                 /**< HYPERBUS Octo SPI command register, offset: 0x34 */
    __IO  uint32_t OSPI_ADDR;                /**< HYPERBUS Octo SPI address register, offset: 0x38 */
    __IO  uint32_t OSPI_CFG;                 /**< HYPERBUS Octo SPI cofiguration register, offset: 0x3C */
    __IO  uint32_t OSPI_CSN;                 /**< HYPERBUS Octo SPI chip select register, offset: 0x40 */
    __IO  uint32_t OSPI_JEDEC_RESET;         /**< HYPERBUS Octo SPI cofiguration register, offset: 0x44 */
    __IO  uint32_t reserved2;
    __IO  uint32_t reserved3;
    __IO  uint32_t BURST_SIZE;               /**< HYPERBUS Octo SPI burst size register, offset: 0x50 */
    __IO  uint32_t LINE_2D;                  /**< HYPERBUS Octo SPI burst size register, offset: 0x54 */
    __IO  uint32_t STRIDE_2D;                /**< HYPERBUS Octo SPI burst size register, offset: 0x58 */
    __IO  uint32_t BURST_ENABLE;             /**< HYPERBUS Octo SPI burst enable register, offset: 0x5C */
    __IO  uint32_t IRQ_EN;                   /**< HYPERBUS Octo SPI interrupt enable, offset: 0x60 */
    __IO  uint32_t CLK_DIV;                  /**< HYPERBUS Clock devider register, offset: 0x64 */
    __IO  uint32_t STATUS;                   /**< HYPERBUS Status register, offset: 0x68 */
    __IO  uint32_t CMD_ARG;                  /**< HYPERBUS SDIO argument register, offset: 0x6C */
    __IO  uint32_t RSP0;                     /**< HYPERBUS SDIO response 0 register, offset: 0x70 */
    __IO  uint32_t RSP1;                     /**< HYPERBUS SDIO response 0 register, offset: 0x74 */
    __IO  uint32_t RSP2;                     /**< HYPERBUS SDIO response 0 register, offset: 0x78 */
    __IO  uint32_t RSP3;                     /**< HYPERBUS SDIO response 0 register, offset: 0x7C */

} HYPERBUS_Type0;

#define TEST_HYPERBUS                        ((HYPERBUS_Type0 *)(0x1A102080 + PER_ID_HYPERBUS * 0x80))



static inline int SDIO_Wait()
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    do {
        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        if (TEST_HYPERBUS->STATUS & (1 << 3)) {
            /* Clear ERR flag */
            TEST_HYPERBUS->STATUS = (1 << 3);

            return -1;
        }
    } while ( !(TEST_HYPERBUS->STATUS & (1 << 4)));

    /* Clear EOT flag */
    TEST_HYPERBUS->STATUS = (1 << 4);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);

    return 0;
}

static void sdio_read_response(void)
{
    resp[0] = TEST_HYPERBUS->RSP0;
    resp[1] = TEST_HYPERBUS->RSP1;
    resp[2] = TEST_HYPERBUS->RSP2;
    resp[3] = TEST_HYPERBUS->RSP3;

    /* printf("RES: 0x%08x:%08x:%08x:%08x\n", resp[3], resp[2], resp[1], resp[0]); */
}

static int sdio_send_cmd(uint32_t cmd_op, uint32_t cmd_arg)
{
    TEST_HYPERBUS->OSPI_CMD  = (cmd_op << 20);
    TEST_HYPERBUS->CMD_ARG   = cmd_arg;
    TEST_HYPERBUS->TRANS_CFG |= (1 << 1);

    if ( SDIO_Wait() ) {
        printf("STATUS : %x\n", (TEST_HYPERBUS->STATUS >> 16));
        return -1;
    }

    if(cmd_op != CMD0)
        sdio_read_response();

    return 0;
}

////////////////////////////////////////////////////////////////
//                                                            //
//  test_init_sequence:                                        //
//                                                            //
//  Test 1: Init sequence, With response check                //
//  CMD 0. Reset Card                                         //
//  CMD 8. Get voltage (Only 2.0 Card response to this)       //
//  CMD55. Indicate Next Command are Application specific     //
//  ACMD44. Get Voltage windows                               //
//  CMD2. CID reg                                             //
//  CMD3. Get RCA.                                            //
//  CMD9. Get CSD.                                            //
//  CMD13. Get Status.                                        //
//                                                            //
////////////////////////////////////////////////////////////////
uint16_t test_init_sequence()
{
    // Send GO_TO_IDLE_STATE command
    for (int i = 0; i < 5; i++)
        sdio_send_cmd(CMD0, 0);

    // CMD 8. Get voltage (Only 2.0 Card response to this)
    sdio_send_cmd(CMD8 | RSP_48_CRC, 0x1AA);

    // Wait until busy is clear in the card
    do {
        // Send CMD 55
        sdio_send_cmd(CMD55 | RSP_48_CRC, 0);

        // Send ACMD 41
        sdio_send_cmd(ACMD41 | RSP_48_CRC, 0xC0100000);
    } while ((resp[0] >> 31) == 0);

    // Send CMD 2
    sdio_send_cmd(CMD2 | RSP_136, 0);

    // Send CMD 3
    sdio_send_cmd(CMD3 | RSP_48_CRC, 0);

    uint16_t rca = (resp[0] >> 16) & 0xFFFF;
    printf("rca: %x\n", rca);

    // Send CMD 9
    sdio_send_cmd(CMD9 | RSP_136, (rca << 16));

    /* Get the Card Class */
    printf ("Card Class = %x\n", resp[2] >> 20);

    sdio_send_cmd(CMD13 | RSP_48_CRC, (rca << 16));

    return rca;
}

////////////////////////////////////////////////////////////////
//                                                            //
//  TEST 2: Send data                                         //
//  init card                                                 //
//  CMD 7. Put Card in transfer state                         //
//  CMD 16. Set block size                                    //
//  CMD 55.                                                   //
//  ACMD 6. Set bus width                                     //
//  CMD 17. Read single block                                 //
//                                                            //
////////////////////////////////////////////////////////////////
void test_single_block_read()
{
    // Init_card
    uint16_t rca = test_init_sequence();

    // setup_card_to_transfer
    // Send CMD 7
    // select the card with previously obtained rca
    sdio_send_cmd(CMD7 | RSP_48_CRC, rca << 16);

    // Set Block Size 512
    // Send CMD 16
    sdio_send_cmd(CMD16 | RSP_48_CRC, BLOCK_SIZE);
    printf("Card status after Block Size set = %x\n", resp[0]);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(CMD55 | RSP_48_CRC, rca << 16);

    // Send ACMD 6
    sdio_send_cmd(ACMD6 | RSP_48_CRC, SDIO_4BITS ? 2 : 0);

    /* Read */
    TEST_HYPERBUS->RX_DEST     = 1;
    TEST_HYPERBUS->TX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = (uint32_t)TX_BUFFER;
    TEST_HYPERBUS->TRANS_SIZE  = BLOCK_SIZE;

    TEST_HYPERBUS->OSPI_CSN = (1 << 6)
        | (SDIO_4BITS << 7)
        | (0 << 8)
        | ((BLOCK_SIZE - 1) << 16);
    TEST_HYPERBUS->TRANS_CFG   = 1;
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    // Send CMD 17
    sdio_send_cmd(CMD17 | RSP_48_CRC, BLOCK_COUNT);
    TEST_HYPERBUS->TRANS_AUTO  = 0;

    /* Clear Setup */
    TEST_HYPERBUS->OSPI_CSN = 0;
}

/////////////////////////////////////////////////////////////////
//                                                             //
//  TEST 3: Send and receive data                              //
//  init card                                                  //
//  setup card for transfer                                    //
//  CMD 7. Put Card in transfer state                          //
//  CMD 16. Set block size                                     //
//  CMD 55.                                                    //
//  ACMD 6. Set bus width                                      //
//  CMD 24. Write data                                         //
//  CMD 17. Read data                                          //
//                                                             //
/////////////////////////////////////////////////////////////////

void test_single_block_write()
{
    // Init_card
    uint16_t rca = test_init_sequence();

    // Setup_card_to_transfer
    // Send CMD 7
    // Select the card with previously obtained rca
    sdio_send_cmd(CMD7 | RSP_48_CRC, rca << 16);

    // Set Block Size 512
    // Send CMD 16
    sdio_send_cmd(CMD16 | RSP_48_CRC, BLOCK_SIZE);
    printf("Card status after Block Size set = %x\n", resp[0]);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(CMD55 | RSP_48_CRC, rca << 16);

    // Send ACMD 6
    sdio_send_cmd(ACMD6 | RSP_48_CRC, SDIO_4BITS ? 2 : 0);

    /* Write */
    TEST_HYPERBUS->TX_DEST     = 1;
    TEST_HYPERBUS->RX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = (uint32_t)TX_BUFFER;
    TEST_HYPERBUS->TRANS_SIZE  = BLOCK_SIZE;
    TEST_HYPERBUS->TRANS_CFG   = 0;

    TEST_HYPERBUS->OSPI_CSN = (1 << 6)
        | (SDIO_4BITS << 7)
        | (0 << 8)
        | ((BLOCK_SIZE - 1) << 16);
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    // Send CMD 24
    sdio_send_cmd(CMD24 | RSP_48_CRC, BLOCK_COUNT);
    TEST_HYPERBUS->TRANS_AUTO  = 0;

    /* Clear Setup */
    TEST_HYPERBUS->OSPI_CSN = 0;


    /* Read */
    TEST_HYPERBUS->RX_DEST     = 1;
    TEST_HYPERBUS->TX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = (uint32_t)RX_BUFFER;
    TEST_HYPERBUS->TRANS_SIZE  = BLOCK_SIZE;

    TEST_HYPERBUS->OSPI_CSN = (1 << 6)
        | (SDIO_4BITS << 7)
        | (0 << 8)
        | ((BLOCK_SIZE - 1) << 16);
    TEST_HYPERBUS->TRANS_CFG   = 1;
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    // Send CMD 17
    sdio_send_cmd(CMD17 | RSP_48_CRC, BLOCK_COUNT);
    TEST_HYPERBUS->TRANS_AUTO  = 0;

    /* Clear Setup */
    TEST_HYPERBUS->OSPI_CSN = 0;
}

/////////////////////////////////////////////////////////////////
//                                                             //
//  TEST 4: Send and receive data                              //
//  init card                                                  //
//  setup card for transfer                                    //
//  CMD 7. Put Card in transfer state                          //
//  CMD 16. Set block size                                     //
//  CMD 55.                                                    //
//  ACMD 6. Set bus width                                      //
//  CMD 18. Read multiple block                                //
//                                                             //
/////////////////////////////////////////////////////////////////

void test_multiple_block_read(int block_count)
{
    assert(block_count >= 2);

    // Init_card
    uint16_t rca = test_init_sequence();

    // setup_card_to_transfer
    // Send CMD 7
    // select the card with previously obtained rca
    sdio_send_cmd(CMD7 | RSP_48_CRC, rca << 16);

    // Set Block Size 512
    // Send CMD 16
    sdio_send_cmd(CMD16 | RSP_48_CRC, BLOCK_SIZE);
    printf("Card status after Block Size set = %x\n", resp[0]);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(CMD55 | RSP_48_CRC, rca << 16);

    // Send ACMD 6
    sdio_send_cmd(ACMD6 | RSP_48_CRC, SDIO_4BITS ? 2 : 0);

    /* Read */
    TEST_HYPERBUS->RX_DEST     = 1;
    TEST_HYPERBUS->TX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = (uint32_t)TX_BUFFER;
    TEST_HYPERBUS->TRANS_SIZE  = BLOCK_SIZE * block_count;

    TEST_HYPERBUS->OSPI_CSN = (1 << 6)
        | (SDIO_4BITS << 7)
        | ((block_count - 1) << 8)
        | ((BLOCK_SIZE - 1) << 16);
    TEST_HYPERBUS->TRANS_CFG   = 1;
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    /* Stop transfer automatically */
    sdio_send_cmd(CMD18 | RSP_48_CRC, BLOCK_COUNT);
    TEST_HYPERBUS->TRANS_AUTO  = 0;

    /* Clear Setup */
    TEST_HYPERBUS->OSPI_CSN = 0;
}

/////////////////////////////////////////////////////////////////
//                                                             //
//  TEST 5: Send and receive data                              //
//  init card                                                  //
//  setup card for transfer                                    //
//  CMD 7. Put Card in transfer state                          //
//  CMD 16. Set block size                                     //
//  CMD 55.                                                    //
//  ACMD 6. Set bus width                                      //
//  CMD 25. Write multiple block                               //
//  CMD 18. Read multiple block                                //
//                                                             //
/////////////////////////////////////////////////////////////////

void test_multiple_block_write(int block_count)
{
    assert(block_count >= 2);

    // Init_card
    uint16_t rca = test_init_sequence();

    // Setup_card_to_transfer
    // Send CMD 7
    // Select the card with previously obtained rca
    sdio_send_cmd(CMD7 | RSP_48_CRC, rca << 16);

    // Set Block Size 512
    // Send CMD 16
    sdio_send_cmd(CMD16 | RSP_48_CRC, BLOCK_SIZE);
    printf("Card status after Block Size set = %x\n", resp[0]);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(CMD55 | RSP_48_CRC, rca << 16);

    // Send ACMD 6
    sdio_send_cmd(ACMD6 | RSP_48_CRC, SDIO_4BITS ? 2 : 0);

    /* Write */
    TEST_HYPERBUS->TX_DEST     = 1;
    TEST_HYPERBUS->RX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = (uint32_t)TX_BUFFER;
    TEST_HYPERBUS->TRANS_SIZE  = BLOCK_SIZE * block_count;
    TEST_HYPERBUS->TRANS_CFG   = 0;

    TEST_HYPERBUS->OSPI_CSN = (1 << 6)
        | (SDIO_4BITS << 7)
        | ((block_count - 1) << 8)
        | ((BLOCK_SIZE - 1) << 16);
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    // Send CMD 25
    sdio_send_cmd(CMD25 | RSP_48_CRC, BLOCK_COUNT);
    TEST_HYPERBUS->TRANS_AUTO  = 0;

    /* Clear Setup */
    TEST_HYPERBUS->OSPI_CSN = 0;

    /* Read */
    TEST_HYPERBUS->RX_DEST     = 1;
    TEST_HYPERBUS->TX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = (uint32_t)TX_BUFFER;
    TEST_HYPERBUS->TRANS_SIZE  = BLOCK_SIZE * block_count;

    TEST_HYPERBUS->OSPI_CSN = (1 << 6)
        | (SDIO_4BITS << 7)
        | ((block_count - 1) << 8)
        | ((BLOCK_SIZE - 1) << 16);
    TEST_HYPERBUS->TRANS_CFG   = 1;
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    /* Stop transfer automatically */
    sdio_send_cmd(CMD18 | RSP_48_CRC, BLOCK_COUNT);
    TEST_HYPERBUS->TRANS_AUTO  = 0;

    /* Clear Setup */
    TEST_HYPERBUS->OSPI_CSN = 0;
}

void udma_sdio_init(void)
{
    /* Set 12 - 17 pin to alt 2 - SDIO */
    volatile uint32_t val = PORTA->PADFUN[2] & ~(0xFFFC);
    PORTA->PADFUN[2] = val | (uint32_t)(0xFFFC);

    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);
    UDMA_GC->RST |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(40 + 31);

    TEST_HYPERBUS->DEVICE_TYPE = (1 << 3);

    TEST_HYPERBUS->IRQ_EN      = HYPERBUS_IRQ_EN(1);

    TEST_HYPERBUS->CLK_DIV     = 10;
}

int main()
{
    printf("Fabric controller code execution for mbed_os SDIO driver test\n");

    /* Initial interrupt, disable clock gating  */
    udma_sdio_init();

    test_init_sequence();

    test_single_block_read();
    test_single_block_write();

    test_multiple_block_read(2);
    test_multiple_block_write(2);

    /* CHECK */
    int error = 0;

    for(int i = 0; i < BLOCK_COUNT * 2; i++) {

        if(RX_BUFFER[i] != TX_BUFFER[i])
            error++;
    }

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
