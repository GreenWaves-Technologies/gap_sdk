// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define BUFFER_SIZE       128
#define RXTX_ADDR         0x1000

#define MAN_ID        0xC2
#define MEM_TYPE      0x80
#define MEM_DENSITY   0x3A

#define OCTO_SPI    0x0
#define QUAD_SPI    0x1
#define SINGLE_SPI  0x2

#define OSPI_STR      1
#define OSPI_DTR      2

#define LATENCY       5

uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

void ospi_write(uint32_t l2_addr, int size)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    HYPERBUS0->UDMA_HYPERBUS.TX_SADDR = l2_addr;
    HYPERBUS0->UDMA_HYPERBUS.TX_SIZE  = size;
    HYPERBUS0->UDMA_HYPERBUS.TX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

void ospi_read(uint32_t l2_addr, int size)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    HYPERBUS0->UDMA_HYPERBUS.RX_SADDR = l2_addr;
    HYPERBUS0->UDMA_HYPERBUS.RX_SIZE  = size;
    HYPERBUS0->UDMA_HYPERBUS.RX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}


static void write_enable(int single_str_dtr)
{
    if (single_str_dtr == SINGLE_SPI ) {
        HYPERBUS0->OSPI_CMD    = (0x06 << 8);
        HYPERBUS0->OSPI_ADDR   = 0x0;
        HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x0 << 4) | 1;
        HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;

    } else if (single_str_dtr == OSPI_STR) {
        HYPERBUS0->OSPI_CMD    = 0x06F9;
        HYPERBUS0->OSPI_ADDR   = 0x0;
        HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( OCTO_SPI << 8) | (0x0 << 4) | 2;
        HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;

    } else if (single_str_dtr == OSPI_DTR) {
        HYPERBUS0->OSPI_CMD    = 0x06F9;
        HYPERBUS0->OSPI_ADDR   = 0x0;
        HYPERBUS0->OSPI_CFG    = ( 0x0 << 12 ) | ( OCTO_SPI << 8) | (0x0 << 4) | 2;
        HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    }

    ospi_write((uint32_t)NULL, 0);
}

static int read_id()
{
    uint32_t reg = 0;

    /* Single SPI RDID */
    HYPERBUS0->TIMING_CFG  &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    HYPERBUS0->OSPI_CMD    = 0x9f00;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x4 << 0) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)&reg, 3);

    if((reg & 0xFFFFFF) != (uint32_t)((MEM_DENSITY << 16) | (MEM_TYPE << 8) | MAN_ID))
        return -1;

    write_enable(SINGLE_SPI);
    /* Set Dummy Cycle 6 - 0x07 */
    reg = 0x7;
    HYPERBUS0->OSPI_CMD    = 0x7200;
    HYPERBUS0->OSPI_ADDR   = 0x300;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)&reg, 1);

    write_enable(SINGLE_SPI);
    /* Set OPI en STR - 0x01 */
    reg = 0x01;
    HYPERBUS0->OSPI_CMD    = 0x7200;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)&reg, 1);

    /* Set latency to 5 */
    HYPERBUS0->TIMING_CFG  &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    HYPERBUS0->TIMING_CFG  |= HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);

    HYPERBUS0->TIMING_CFG  = (665 << 18) | (1 << 17) | (1 << 14) | (6 << 10) | 5;
    HYPERBUS0->OSPI_CMD    = 0x9f60;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( OCTO_SPI << 8) | (0x4 << 4) | 2;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)&reg, 3);

    if((reg & 0xFFFFFF) != (uint32_t)((MEM_DENSITY << 16) | (MEM_TYPE << 8) | MAN_ID))
        return -1;

    printf("RDID = 0x%x\n", reg);

    return 0;
}

int test_ospi(int frequency)
{
    int error = 1;

    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 3);
    /* Enable IRQ */
    HYPERBUS0->IRQ_EN  = 1;

    HYPERBUS0->TIMING_CFG  =
        HYPERBUS_TIMING_CFG_CSM(666)                           |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(1) |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(1)                      |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(6)                     |
        HYPERBUS_TIMING_CFG_LATENCY1(0)                        |
        HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);

    HYPERBUS0->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(0) | HYPERBUS_DEVICE_DT0(0) | HYPERBUS_DEVICE_TYPE(0);
    HYPERBUS0->CLK_DIV     = 25000000/frequency - 1;

    /* Read ID in single mode */
    error = read_id();

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os OSPI FLASH driver test\n");

    int error = 0;

    for (int i = 0; i< BUFFER_SIZE; i++)
    {
        tx_buffer[i] = i;
        rx_buffer[i] = 0xff;
    }

    error = test_ospi(25*1000000);

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return 0;
}
