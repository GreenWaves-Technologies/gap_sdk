/**
 * Defines/Types and small low level driver for udma core main functionnalities
 */

#ifndef __UDMA_SDIO_H__
#define __UDMA_SDIO_H__

#ifdef __GAP9__

#include "pmsis_hal.h"
#include "udma_core.h"
#include "pmsis_soc_eu.h"
#include "pmsis_itc.h"

#if (defined(TRACE) || defined (DEBUG))
extern int printf  (const char *,  ... );
#endif
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
#define CMD55  0x3700 // APP_SPEC_CMD
#define ACMD41 0x2900 // Get voltage window--ACMD, need to be used after APP_SPEC_CMD
#define CMD8   0x800 // Send voltage
#define CMD12  0x0C00 // stop command
#define CMD16  0x1000
#define CMD17  0x1100 // READ_SINGLE_BLOCK
#define CMD18  0x1200 // READ_MULTIPLE_BLOCK
#define CMD24  0x1800 // WRITE_BLOCK
#define CMD25  0x1900 // WRITE_MULT_BLOCK

/* Response type */
#define RSP_48_CRC     0x1
#define RSP_48_NO_CRC  0x2
#define RSP_136        0x3
#define RSP_48_BUSY    0x4

typedef struct sdio_transfer
{
    uint32_t sdio_address;
    uint32_t buffer_address;
    uint32_t size;
    uint8_t  quad;
} sdio_transfer_t;


/* -------------------------------------------------------------------------
   -- SDIO Functions
   -------------------------------------------------------------------------
*/

/**
 * Wait for the sdcard to answer a command or process data
 **/
static inline int sdio_wait(void)
{
#ifndef _USE_PMSIS_OS_
    do
    {
        hal_itc_wait_for_event_no_irq(FC_SOC_EVENT_IRQN);
        // TODO:
        // Check for which soc event happened!
        if (SDIO->status & SDIO_STATUS_ERROR_MASK)
        {
            /* Clear ERR flag */
            SDIO->status = SDIO_STATUS_ERROR_MASK;
            return -1;
        }
    } while (!(SDIO->status & SDIO_STATUS_EOT_MASK));
    /* Clear EOT flag */
    SDIO->status = SDIO_STATUS_EOT_MASK;
#endif
    return 0;
}

/**
 * Response should be a 4 uint32_t wide array
 **/
static inline void sdio_read_response(volatile uint32_t *response)
{
    response[0] = SDIO->rsp0;
    response[1] = SDIO->rsp1;
    response[2] = SDIO->rsp2;
    response[3] = SDIO->rsp3;
}

/**
 * Send a command and wait until the sd card answers
 * retuns 1 if an error/timeout happened
 * else response is filled with sdcard response
 **/
static inline int sdio_send_cmd(uint32_t cmd_op, uint32_t cmd_arg,
        volatile uint32_t *response)
{
    SDIO->cmd_op  = cmd_op;
    SDIO->cmd_arg = cmd_arg;
    SDIO->start   = 1;

    if ( sdio_wait() )
    {
#if (defined(TRACE) || defined (DEBUG))
        printf("STATUS : %lx\n", (SDIO->status >> 16));
#endif // TRACE||DEBUG
        return (SDIO->status >> 16);
    }
    sdio_read_response(response);
    return 0;
}

/**
 * udma_sdio_init
 * base udma side init: unset clock gating, set padframe
 * activate irq
 * set clk div
 */
static inline void udma_sdio_init(uint32_t clk_div)
{
    /* Clear the first bits corresponding to sdio */
    volatile uint32_t val0 = PORTA->PADFUN[0] & ~(0xFF000000);
    volatile uint32_t val1 = PORTA->PADFUN[1] & ~(0xF);
    /* set sdio/qspi ports to alt 2 (sdio) */
    PORTA->PADFUN[0] = val0 | (uint32_t)(0xAA000000);
    PORTA->PADFUN[1] = val1 | (uint32_t)(0xA);

    /* Disable clock gating */
    udma_init_device(UDMA_SDIO_ID(0));

    /* set event mask */
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SDIO_RX(0));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SDIO_TX(0));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SDIO_EOT(0));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SDIO_ERROR(0));

    // 0x100 to enable | 0xFF&clk_div for the value
    SDIO->clk_div = (0x100UL | clk_div);
}

/** udma_sdio_init_sequence:
 * Initialization sequence for sdio card
 * CMD 0. Reset Card
 * CMD 8. Get voltage (Only 2.0 Card response to this)
 * CMD55. Indicate Next Command are Application specific
 * ACMD41. Get Voltage window
 * CMD2. CID reg
 * CMD3. Get RCA.
 **/
static inline uint16_t udma_sdio_init_sequence()
{
    uint32_t response[4];
    // Send GO_TO_IDLE_STATE command
    sdio_send_cmd(SDIO_CMD_GO_IDLE_STATE(), 0, response);

    SDIO->stopcmd_op  = SDIO_CMD_SD_STOP();
    SDIO->stopcmd_arg = 0;

    // CMD 8. Get voltage (Only 2.0 Card response to this)
    sdio_send_cmd(SDIO_CMD_SEND_VOLTAGE(), 0, response);

    // Wait until busy is clear in the card
    do {
        // Send CMD 55
        sdio_send_cmd(SDIO_CMD_APP_SPEC_CMD(), 0, response);

        // Send ACMD 41
        sdio_send_cmd(SDIO_ACMD_HCS(), 0, response);
    } while ((response[0] >> 31) == 0);

    // Send CMD 2
    sdio_send_cmd(SDIO_CMD_SEND_CID(), 0, response);

    // Send CMD 3
    sdio_send_cmd(SDIO_CMD_SEND_RCA(), 0, response);

    uint16_t rca = (response[0] >> 16) & 0xFFFF;

    return rca;
}

/**
 * udma_sdio_select_card_with_rca
 * Must come before transfers, and after udma_sdio_init_sequence
 * Card selection (put to data ready state)
 * Card configuration (data lines)
 * CMD7: Put card in data ready state
 * CMD55+ACMD6: configure data lines (if quad, 4 data lines, else 1)
 */
static inline void udma_sdio_select_card_with_rca(uint32_t rca, int quad)
{
    uint32_t response[4];
    uint32_t bus_width = quad ? 2 : 0;
    // setup_card_to_transfer
    // Send CMD 7
    // select the card with previously obtained rca
    sdio_send_cmd(SDIO_CMD_SELECT(), rca << 16, response);

    // Set bus width
    // Send CMD 55
    sdio_send_cmd(SDIO_CMD_APP_SPEC_CMD(), 0, response);

    // Send ACMD 6
    sdio_send_cmd(SDIO_ACMD_CFG_QUAD(), bus_width, response);
}

/**
 * udma_sdio_read_block
 * Must come after udma_sdio_select_card_with_rca
 * read one block (512 bytes) of data from sdcard
 * use sdio_tranfer_t structure for setup
 */
static inline void udma_sdio_read_block(sdio_transfer_t *transfer)
{
    uint32_t response[4]; // we don't really care about the answer here
    /* Read */
    SDIO->udma_sdio.rx_saddr = transfer->buffer_address;
    SDIO->udma_sdio.rx_size  = transfer->size;
    SDIO->udma_sdio.rx_cfg   = 0x10;
    
    sdio_data_setup_t setup = {0};
    setup.data_en    = 1;
    setup.data_rwn   = 1;
    setup.data_quad  = !!transfer->quad;
    setup.block_num  = 0;
    setup.block_size = (512UL-1UL);
    SDIO->data_setup = setup.raw;

    /* Send only one block */
    sdio_send_cmd(SDIO_CMD_READ_SINGLE_BLOCK(), transfer->sdio_address, response);

    /* Clear Setup */
    SDIO->data_setup = 0;
}

/**
 * udma_sdio_read_block
 * Must come after udma_sdio_select_card_with_rca
 * read n block (512 bytes) of data from sdcard
 * use sdio_tranfer_t structure for setup and n as number of blocks
 */
static inline void udma_sdio_read_n_block(sdio_transfer_t *transfer, int n)
{
    uint32_t response[4]; // we don't really care about the answer here
    /* Read */
    SDIO->udma_sdio.rx_saddr = transfer->buffer_address;
    SDIO->udma_sdio.rx_size  = transfer->size;
    SDIO->udma_sdio.rx_cfg   = 0x10;
    
    sdio_data_setup_t setup = {0};
    setup.data_en    = 1;
    setup.data_rwn   = 1;
    setup.data_quad  = !!transfer->quad;
    setup.block_num  = (n-1);
    setup.block_size = (512UL-1UL);
    SDIO->data_setup = setup.raw;

    /* Send only one block */
    sdio_send_cmd(SDIO_CMD_READ_MULT_BLOCK(), transfer->sdio_address, response);

    /* Clear Setup */
    SDIO->data_setup = 0;
}

static inline void udma_sdio_write_block(sdio_transfer_t *transfer)
{
    uint32_t response[4]; // we don't really care about the answer here
    /* Read */
    SDIO->udma_sdio.tx_saddr = transfer->buffer_address;
    SDIO->udma_sdio.tx_size  = transfer->size;
    SDIO->udma_sdio.tx_cfg   = 0x10;
    
    sdio_data_setup_t setup = {0};
    setup.data_en    = 1;
    setup.data_rwn   = 0;
    setup.data_quad  = !!transfer->quad;
    setup.block_num  = (0);
    setup.block_size = (512UL-1UL);
    SDIO->data_setup = setup.raw;
    
    /* Send only one block */
    sdio_send_cmd(SDIO_CMD_WRITE_SINGLE_BLOCK(), transfer->sdio_address, response);

    /* Clear Setup */
    SDIO->data_setup = 0;
}

static inline void udma_sdio_write_n_block(sdio_transfer_t *transfer, int n)
{
    uint32_t response[4]; // we don't really care about the answer here
    /* Read */
    SDIO->udma_sdio.tx_saddr = transfer->buffer_address;
    SDIO->udma_sdio.tx_size  = transfer->size;
    SDIO->udma_sdio.tx_cfg   = 0x10;
    
    sdio_data_setup_t setup = {0};
    setup.data_en    = 1;
    setup.data_rwn   = 0;
    setup.data_quad  = !!transfer->quad;
    setup.block_num  = (n-1);
    setup.block_size = (512UL-1UL);
    SDIO->data_setup = setup.raw;

    /* Send only one block */
    sdio_send_cmd(SDIO_CMD_WRITE_MULT_BLOCK(), transfer->sdio_address, response);

    /* Clear Setup */
    SDIO->data_setup = 0;
}
#endif
#endif
