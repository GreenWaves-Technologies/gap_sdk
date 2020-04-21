// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define BUFFER_SIZE       64
#define RXTX_ADDR         0x1000
#define LATENCY           5

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

uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

void ospi_write(uint32_t l2_addr, int size)
{
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

    if (TEST_HYPERBUS->STATUS & 0x1)
    {
        TEST_HYPERBUS->STATUS = 0x1;
        printf("TX ERROR\n");
    }
}

void ospi_read(uint32_t l2_addr, int size)
{
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

    if (TEST_HYPERBUS->STATUS & 0x2)
    {
        TEST_HYPERBUS->STATUS = 0x2;
        printf("RX ERROR\n");
    }
}

int test_hyper(int frequency)
{
    int error = 0;

    /* Clock gating enable */
    UDMA_GC->CG |= (1 << 12);
    UDMA_GC->RST |= (1 << 12);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(40 + 31);

    /* Enable IRQ */
    TEST_HYPERBUS->IRQ_EN  = 1;

    TEST_HYPERBUS->TIMING_CFG  =
        HYPERBUS_TIMING_CFG_CSM(666)                           |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(0) |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(1)                      |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(6)                     |
        HYPERBUS_TIMING_CFG_LATENCY1(0)                        |
        HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);

    TEST_HYPERBUS->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(0) | HYPERBUS_DEVICE_DT0(0) | HYPERBUS_DEVICE_TYPE(0);
    TEST_HYPERBUS->CLK_DIV     = 25000000/frequency - 1;
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    // OCTO PSRAM Register Read 0 (5 latency)
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    TEST_HYPERBUS->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 1 (5 latency)
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x1;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 2 (5 latency)
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x2;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 3 (5 latency)
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x3;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 4 (5 latency)
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x4;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 8 (5 latency)
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x8;
    ospi_read((uint32_t)rx_buffer, 2);

    // OCTO PSRAM Register Write (0 latency)
    TEST_HYPERBUS->TIMING_CFG  &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    TEST_HYPERBUS->OSPI_CMD    = 0xC000;
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    rx_buffer[0] = 0x09;
    rx_buffer[1] = 0x09;
    ospi_write((uint32_t)rx_buffer, 2);

    // OCTO PSRAM Register Read 0 (5 latency)
    TEST_HYPERBUS->TIMING_CFG  &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    TEST_HYPERBUS->TIMING_CFG  |= HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);
    TEST_HYPERBUS->OSPI_CMD    = 0x4000;
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    ospi_read((uint32_t)rx_buffer, 2);

    // Wrap test, READ_CMD = 0x0000  WRITE_CMD = 0x8000
    TEST_HYPERBUS->OSPI_CMD    = (1 << 17) | (0 << 16) | 0x0000;

    // OCTO PSRAM RAM Write
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE);
    // OCTO PSRAM RAM Read
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);

    // Linear test, READ_CMD = 0x2000  WRITE_CMD = 0xA000
    TEST_HYPERBUS->OSPI_CMD    = (1 << 17) | (0 << 16) | 0x2000;
    // OCTO PSRAM RAM linear Write
    TEST_HYPERBUS->EXT_ADDR    = 0x100;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE);
    // OCTO PSRAM RAM linear Read
    TEST_HYPERBUS->EXT_ADDR    = 0x100;
    ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);


    for (int i = 0; i < BUFFER_SIZE; i++) {
        if(tx_buffer[i] != rx_buffer[i]) {
            error++;
        }
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os APMomery PSRAM driver test\n");

    int error = 0;

    for (int i = 0; i< BUFFER_SIZE; i++)
    {
        tx_buffer[i] = i;
        rx_buffer[i] = 0xff;
    }

    error = test_hyper(5*1000000);

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return error;
}
