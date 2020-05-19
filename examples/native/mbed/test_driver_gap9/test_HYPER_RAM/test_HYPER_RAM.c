// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define BUFFER_SIZE       128
#define CONF_REG_DEFAULT  0x8F1F
#define RXTX_ADDR         0x1000

#define RAM_ADDR     0x0
#define FLASH_ADDR   0x10000000
#define LATENCY      6

volatile uint8_t tx_buffer[BUFFER_SIZE];
volatile uint8_t rx_buffer[BUFFER_SIZE];

/*
 * CA bits     47    46    45    44-40  |  39-32  |  31-24  |  23-16  |  15-8  |  7-0
 *
 * ID0 (RO)       C0h or E0h                00h       00h       00h       00h     00h
 * ID1 (RO)       C0h or E0h                00h       00h       00h       00h     01h
 * CR0_R          C0h or E0h                00h       01h       00h       00h     00h
 * CR0_W             60h                    00h       01h       00h       00h     00h
 * CR1_R          C0h or E0h                00h       01h       00h       00h     01h
 * CR1_W             60h                    00h       01h       00h       00h     01h
 *
 * CA[44-16]  => ADDR[31-3]
 * CA[2-0]    => ADDR[2-0]
 */

/* Identification Register 0 */
#define   ID0    0x0000
/* Identification Register 1 */
#define   ID1    0x0002

/* Configuration Register 0 */
#define   CR0    0x1000
/* Configuration Register 1 */
#define   CR1    0x1002

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

void hyper_write(int addr, uint32_t l2_addr, int size)
{
    TEST_HYPERBUS->EXT_ADDR   = RAM_ADDR + addr;

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

void hyper_read(int addr, uint32_t l2_addr, int size)
{
    TEST_HYPERBUS->EXT_ADDR   = RAM_ADDR + addr;

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
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);
    UDMA_GC->RST |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(40 + 31);

    TEST_HYPERBUS->TIMING_CFG  =
        HYPERBUS_TIMING_CFG_CSM(666)                           |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(1) |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(1)                      |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(6)                     |
        HYPERBUS_TIMING_CFG_LATENCY1(0)                        |
        HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);

    TEST_HYPERBUS->MBR0       = RAM_ADDR;
    TEST_HYPERBUS->MBR1       = FLASH_ADDR;
    TEST_HYPERBUS->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(1) | HYPERBUS_DEVICE_DT0(0) | HYPERBUS_DEVICE_TYPE(1);

    TEST_HYPERBUS->CLK_DIV     = 25000000/frequency - 1;

    TEST_HYPERBUS->IRQ_EN      = HYPERBUS_IRQ_EN(1);
    TEST_HYPERBUS->TRANS_AUTO  = 1;

    hyper_read (((1 << 31 ) | ID0), (uint32_t)rx_buffer, 2);
    printf("ID0 = 0x%x\n", rx_buffer[1]);
    printf("ID0 = 0x%x\n", rx_buffer[0]);

    hyper_read (((1 << 31 ) | ID1), (uint32_t)rx_buffer, 2);
    printf("ID1 = 0x%x\n", rx_buffer[1]);
    printf("ID1 = 0x%x\n", rx_buffer[0]);

    tx_buffer[0] = 0x1E;
    tx_buffer[1] = 0x81;

    hyper_write(((1 << 31 ) | CR0), (uint32_t)tx_buffer, 2);
    hyper_read (((1 << 31 ) | CR0), (uint32_t)rx_buffer, 2);

    printf("CFG0 = 0x%x\n", rx_buffer[1]);
    printf("CFG0 = 0x%x\n", rx_buffer[0]);

    for (int i = 0; i< 2; i++)
        if(tx_buffer[i] != rx_buffer[i]) {
            error++;
            return -1;
        }

    hyper_read (((1 << 31 ) | CR1), (uint32_t)rx_buffer, 2);

    printf("CFG1 = 0x%x\n", rx_buffer[1]);
    printf("CFG1 = 0x%x\n", rx_buffer[0]);

    tx_buffer[0] = 0x00;
    tx_buffer[1] = 0x01;

    for (int addr = RXTX_ADDR; addr < (RXTX_ADDR + 5); addr++)
    {
        for (int i = 1; i <= BUFFER_SIZE; i++)
        {
            hyper_write(addr, (uint32_t)tx_buffer, i);
            hyper_read (addr, (uint32_t)rx_buffer, i);

            for (int n = 0; n < i; n++)
                if(tx_buffer[n] != rx_buffer[n]) {
                    error++;
                    return -1;
                }
        }
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os HYPERBUS RAM driver test\n");

    int error = 0;

    for (int i = 0; i< BUFFER_SIZE; i++)
    {
        tx_buffer[i] = i;
        rx_buffer[i] = 0xff;
    }

    error = test_hyper(25*1000000);

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return 0;
}
