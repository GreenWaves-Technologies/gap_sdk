// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"
// SPI CMSIS driver
#include "spi_multi_api.h"

#define BUFFER_SIZE 512

uint8_t RX_BUFFER[BUFFER_SIZE];
uint8_t TX_BUFFER[BUFFER_SIZE];

// 128 bits response
volatile uint32_t resp[4] = {0};

/* -------------------------------------------------------------------------
   -- SDIO CMDs
   -------------------------------------------------------------------------
   See page 49 (60 pdf numbering) of Physical Layer Simplified Spec Ver2.00
*/
#define CMD0   0x0   // GO_IDLE_STATE
#define CMD2   0x200 // ALL_SEND_CID
#define CMD3   0x300 // SEND_RELATIVE_ADDR
#define ACMD6  0x600
#define CMD7   0x700
#define CMD8   0x800
#define CMD55  0x3700
#define ACMD41 0x2900 // ACMD, need to be used after APP_SPEC_CMD
#define CMD8   0x800
#define CMD12  0x0C00
#define CMD16  0x1000
#define CMD17  0x1100 // READ_SINGLE_BLOCK
#define CMD18  0x1200
#define CMD24  0x1800 // WRITE_BLOCK

/* Response type */
#define RSP_48_CRC     0x1
#define RSP_48_NO_CRC  0x2
#define RSP_136        0x3
#define RSP_48_BUSY    0x4

static inline int SDIO_Wait()
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    do {
        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        if (SDIO->STATUS & SDIO_STATUS_ERROR_MASK) {
            /* Clear ERR flag */
            SDIO->STATUS = SDIO_STATUS_ERROR_MASK;

            return -1;
        }
    } while ( !(SDIO->STATUS & SDIO_STATUS_EOT_MASK));

    /* Clear EOT flag */
    SDIO->STATUS = SDIO_STATUS_EOT_MASK;

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);

    return 0;
}

static void sdio_read_response(void)
{
    resp[0] = SDIO->RSP0;
    resp[1] = SDIO->RSP1;
    resp[2] = SDIO->RSP2;
    resp[3] = SDIO->RSP3;

    // printf("RES: 0x%08x:%08x:%08x:%08x\n", resp[0], resp[1], resp[2], resp[3]);
}

static void sdio_send_cmd(uint32_t cmd_op, uint32_t cmd_arg)
{
    SDIO->CMD_OP  = cmd_op;
    SDIO->CMD_ARG = cmd_arg;
    SDIO->START   = 1;

    if ( SDIO_Wait() ) {
        printf("STATUS : %x\n", (SDIO->STATUS >> 16));
        return;
    }

    sdio_read_response();
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
//                                                            //
////////////////////////////////////////////////////////////////
uint16_t test_init_sequence()
{
    // Send GO_TO_IDLE_STATE command
    for (int i = 0; i < 5; i++)
        sdio_send_cmd(CMD0, 0);

    // CMD 8. Get voltage (Only 2.0 Card response to this)
    sdio_send_cmd(CMD8 | RSP_48_CRC, 0);

    // Wait until busy is clear in the card
    do {
        // Send CMD 55
        sdio_send_cmd(CMD55 | RSP_48_CRC, 0);

        // Send ACMD 41
        sdio_send_cmd(ACMD41 | RSP_48_CRC, 0);
    } while (resp[0] >> 31 );

    // Send CMD 2
    sdio_send_cmd(CMD2 | RSP_136, 0);

    // Send CMD 3
    sdio_send_cmd(CMD3 | RSP_48_CRC, 0);

    uint16_t rca = (resp[0] >> 16) & 0xFFFF;
    printf("rca: %x\n", rca);

    return rca;
}


////////////////////////////////////////////////////////////////
//                                                            //
//  TEST 2: Send data                                         //
//  init card                                                 //
//  CMD 7. Put Card in transfer state                         //
//  CMD 55.                                                   //
//  ACMD 6. Set bus width                                     //
//  CMD 17. Read data                                         //
//                                                            //
////////////////////////////////////////////////////////////////
uint16_t test_rec_data()
{
    // Init_card
    uint16_t rca = test_init_sequence();

    // setup_card_to_transfer
    // Send CMD 7
    // select the card with previously obtained rca
    sdio_send_cmd(CMD7 | RSP_48_CRC, rca << 16);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(CMD55 | RSP_48_CRC, 0);

    // Send ACMD 6
    sdio_send_cmd(ACMD6 | RSP_48_CRC, 2);

    printf("Card status after Bus width set = %x\n", resp[0]);

    /* Read */
    SDIO->UDMA_SDIO.RX_SADDR = (uint32_t)TX_BUFFER;
    SDIO->UDMA_SDIO.RX_SIZE  = BUFFER_SIZE;
    SDIO->UDMA_SDIO.RX_CFG   = 0x10;
    SDIO->DATA_SETUP         = SDIO_DATA_SETUP_EN(1) |
        SDIO_DATA_SETUP_RWN(1)       |
        SDIO_DATA_SETUP_QUAD(1)      |
        SDIO_DATA_SETUP_BLOCK_NUM(0) |
        SDIO_DATA_SETUP_BLOCK_SIZE(512 - 1);

    /* Stop transfer automatically */
    sdio_send_cmd(CMD17 | RSP_48_CRC, 0);

    /* Clear Setup */
    SDIO->DATA_SETUP = 0;

    return rca;
}

/////////////////////////////////////////////////////////////////
//                                                             //
//  TEST 3: Send and receive data                              //
//  init card                                                  //
//  setup card for transfer                                    //
//  CMD 24. Write data                                         //
//  CMD 17. Read data                                          //
//  CMD 12. Stop transfer                                      //
//                                                             //
/////////////////////////////////////////////////////////////////

void test_send_rec_data()
{
    // Init_card
    uint16_t rca = test_init_sequence();

    // Setup_card_to_transfer
    // Send CMD 7
    // Select the card with previously obtained rca
    sdio_send_cmd(CMD7 | RSP_48_CRC, rca << 16);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(CMD55 | RSP_48_CRC, 0);

    // Send ACMD 6
    sdio_send_cmd(ACMD6 | RSP_48_CRC, 2);

    printf("Card status after Bus width set = %x\n", resp[0]);

    /* Write */
    SDIO->UDMA_SDIO.TX_SADDR = (uint32_t)TX_BUFFER;
    SDIO->UDMA_SDIO.TX_SIZE  = BUFFER_SIZE;
    SDIO->UDMA_SDIO.TX_CFG   = 0x10;
    SDIO->DATA_SETUP         = SDIO_DATA_SETUP_EN(1) |
        SDIO_DATA_SETUP_RWN(0)       |
        SDIO_DATA_SETUP_QUAD(1)      |
        SDIO_DATA_SETUP_BLOCK_NUM(0) |
        SDIO_DATA_SETUP_BLOCK_SIZE(512 - 1);
    sdio_send_cmd(CMD24 | RSP_48_CRC, 0);

    /* Clear Setup */
    SDIO->DATA_SETUP = 0;

    /* Read */
    SDIO->UDMA_SDIO.RX_SADDR = (uint32_t)RX_BUFFER;
    SDIO->UDMA_SDIO.RX_SIZE  = BUFFER_SIZE;
    SDIO->UDMA_SDIO.RX_CFG   = 0x10;
    SDIO->DATA_SETUP         = SDIO_DATA_SETUP_EN(1) |
        SDIO_DATA_SETUP_RWN(1)       |
        SDIO_DATA_SETUP_QUAD(1)      |
        SDIO_DATA_SETUP_BLOCK_NUM(0) |
        SDIO_DATA_SETUP_BLOCK_SIZE(512 - 1);
    sdio_send_cmd(CMD17 | RSP_48_CRC, 0);

    /* Clear Setup */
    SDIO->DATA_SETUP = 0;

    /* Stop transfer */
    sdio_send_cmd(CMD12 | RSP_48_CRC, 0);
}

void udma_sdio_init(void)
{
    /* Set 12 - 17 pin to alt 2 - SDIO */
    volatile uint32_t val0 = PORTA->PADFUN[0] & ~(0xFF000000);
    volatile uint32_t val1 = PORTA->PADFUN[1] & ~(0xF);
    PORTA->PADFUN[0] = val0 | (uint32_t)(0xAA000000);
    PORTA->PADFUN[1] = val1 | (uint32_t)(0xA);

    /* Disable clock gating */
    UDMA_GC->CG |= (1 << PER_ID_SDIO);

    /* set event mask */
    SOC_EU_SetFCMask((PER_ID_SDIO << 2));
    SOC_EU_SetFCMask((PER_ID_SDIO << 2) + 1);
    SOC_EU_SetFCMask((PER_ID_SDIO << 2) + 2);
    SOC_EU_SetFCMask((PER_ID_SDIO << 2) + 3);

    SDIO->CLK_DIV = SDIO_CLK_DIV_VALID(1) | SDIO_CLK_DIV(5);
}

int main()
{
    printf("Fabric controller code execution for mbed_os SDIO driver test\n");

    /* Initial interrupt, disable clock gating  */
    udma_sdio_init();

    test_init_sequence();

    test_rec_data();

    test_send_rec_data();

    /* CHECK */
    int error = 0;

    for(int i = 0; i < BUFFER_SIZE; i++) {

        if(RX_BUFFER[i] != TX_BUFFER[i])
            error++;
    }

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
