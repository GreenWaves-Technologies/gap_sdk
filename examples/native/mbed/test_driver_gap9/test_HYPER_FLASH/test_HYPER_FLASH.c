// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define RAM_ADDR     0x0
#define FLASH_ADDR   0x10000000

#define LATENCY                 6

#define BUFFER_SIZE             256
#define DEVICE_READY_OFFSET     7
#define ERASE_STATUS_OFFSET     5
#define PROGRAM_STATUS_OFFSET   4

/* Write and read address */
#define SA                      0x4000

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
} HYPERBUS_Type0;



#define TEST_HYPERBUS                        ((HYPERBUS_Type0 *)(0x1A102080 + PER_ID_HYPERBUS * 0x80))

typedef struct {
    uint16_t data;
    uint16_t addr;
} cmdSeq;

/* Chip erase */
/* cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x10, 0x555}}; */

/* Sector erase */
cmdSeq Erase_Seq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};
cmdSeq VCR_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {(0x8e0b | ((LATENCY - 5) << 4)), 0x0}};
cmdSeq Reg_Seq      = {0x70, 0x555};

/* Write 512/4 = 128 word to Sector addr 0x4xxx */
cmdSeq WB_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x25, SA}, {BUFFER_SIZE - 1, SA}};
cmdSeq WP_Seq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};
cmdSeq BPC_Seq     = {0x29, SA};
cmdSeq ID_Seq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x90, 0x555}};
cmdSeq ASO_Exit_Seq= {0xF0, 0};

uint8_t txHyperbusSamples[BUFFER_SIZE];
uint8_t rxHyperbusSamples[BUFFER_SIZE];

uint32_t id_reg;


void hyper_write(int addr, uint32_t l2_addr, int size)
{
    TEST_HYPERBUS->EXT_ADDR   = FLASH_ADDR + addr;

    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    TEST_HYPERBUS->TX_DEST     = 1;
    TEST_HYPERBUS->RX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = l2_addr;
    TEST_HYPERBUS->TRANS_SIZE  = size;
    TEST_HYPERBUS->TRANS_CFG   = 2;

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

void hyper_read(int addr, uint32_t l2_addr, int size)
{
    TEST_HYPERBUS->EXT_ADDR   = FLASH_ADDR + addr;

    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    TEST_HYPERBUS->RX_DEST     = 1;
    TEST_HYPERBUS->TX_DEST     = 0x7F;
    TEST_HYPERBUS->TRANS_ADDR  = l2_addr;
    TEST_HYPERBUS->TRANS_SIZE  = size;
    TEST_HYPERBUS->TRANS_CFG   = 3;

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

uint16_t read_status()
{
    /* Read status register */
    hyper_write(Reg_Seq.addr << 1, (uint32_t)&Reg_Seq.data, 2);

    hyper_read(0, (uint32_t)&id_reg, 2);

    #ifdef HYPERBUS_DEBUG
    printf("Read status_reg = 0x%04x\n", id_reg);
    #endif

    return id_reg;
}

uint32_t read_ID()
{
    /* Read status register */
    for (int i = 0; i < 3; i++)
        hyper_write(ID_Seq[i].addr << 1, (uint32_t)&ID_Seq[i].data, 2);

    /*
     * manufacturer ID = 0x0001
     * Device       ID = 0x007e
     */
    hyper_read(0, (uint32_t)&id_reg, 4);

    printf("ID = 0x%x\n", id_reg);

    /* Reset / ASO exit */
    hyper_write(ASO_Exit_Seq.addr << 1, (uint32_t)&ASO_Exit_Seq.data, 2);

    return id_reg;
}

void conf_flash()
{
    /* Set VCR to 5 delay cycles */
    for (int i = 0; i < 4; i++)
        hyper_write(VCR_Seq[i].addr << 1, (uint32_t)&VCR_Seq[i].data, 2);
}

void erase_page_in_flash( uint32_t flash_addr)
{
    /* Erase sector 0x0E00 */
    for ( int i = 0; i < 5; i++) {
        hyper_write(Erase_Seq[i].addr << 1, (uint32_t)&Erase_Seq[i].data, 2);
    }

    hyper_write(flash_addr, (uint32_t)&Erase_Seq[5].data, 2);
}

void write_page_in_flash( uint32_t flash_addr)
{
    int i = 0;

    /* Write to Buffer command sequence */
    for (i = 0; i < 3; i++)
        hyper_write(WP_Seq[i].addr << 1, (uint32_t)&WP_Seq[i].data, 2);

    /* Word Program */
    hyper_write(flash_addr, (uint32_t ) txHyperbusSamples, BUFFER_SIZE);
}

void read_page_from_flash( uint32_t flash_addr)
{
    hyper_read(flash_addr, (uint32_t ) rxHyperbusSamples, BUFFER_SIZE);
}

int wait_process_end( uint32_t err_bit)
{
    uint16_t reg;

    /* Wait the end of process
     * Status Register (SR)
     * bit 4 -> program status bit, 0-success ; 1-failure
     * bit 5 -> erase status bit,   0-success ; 1-failure
     * bit 7 -> device ready bit,   0-busy    ; 1-ready
     */
    do {
        reg = read_status();

        if ( reg & (1 << err_bit))
        {
            printf("ERR bit = 1, an error occurs in  operation\n");
            return 1;
        }
    } while( !(reg & (1 << DEVICE_READY_OFFSET)));

    return 0;
}

int test_hyperbus()
{
    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);
    UDMA_GC->RST |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(40 + 31);

    TEST_HYPERBUS->TIMING_CFG  =
        HYPERBUS_TIMING_CFG_CSM(666)                           |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(1) |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(1)                      |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(6)                     |
        HYPERBUS_TIMING_CFG_LATENCY1( LATENCY )                |
        HYPERBUS_TIMING_CFG_LATENCY0(0);

    TEST_HYPERBUS->MBR0       = RAM_ADDR;
    TEST_HYPERBUS->MBR1       = FLASH_ADDR;
    TEST_HYPERBUS->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(1) | HYPERBUS_DEVICE_DT0(0) | HYPERBUS_DEVICE_TYPE(1);
    TEST_HYPERBUS->IRQ_EN      = HYPERBUS_IRQ_EN(1);
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    /* Initialize the samples */
    for (int i = 0; i< BUFFER_SIZE; i++)
        txHyperbusSamples[i] = i;

    /* Configuration */
    conf_flash();

    /* Read ID */
    read_ID();

    /* Erase chip */
    erase_page_in_flash(SA);
    wait_process_end(ERASE_STATUS_OFFSET);
    printf("Erase finished.\n");

    /* Write buffer no burst */
    write_page_in_flash(SA);
    wait_process_end(PROGRAM_STATUS_OFFSET);
    printf("Write finished.\n");

    /* Read buffer burst */
    read_page_from_flash(SA);
    printf("Read finished.\n");

    /* Error check */
    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if(rxHyperbusSamples[i] != (uint16_t) i) error++;

#ifdef HYPERBUS_DEBUG
        printf("RX_DATA[%d] = 0x%04x\n", i, rxHyperbusSamples[i]);
#endif
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os HYPERBUS FLASH driver test\n");

    int error;
    error = test_hyperbus();

    if (error)
    {
        printf("Test failed\n");
        return -1;
    }
    else
    {
        printf("Test success\n");
        return 0;
    }
}
