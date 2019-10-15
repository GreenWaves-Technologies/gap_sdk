// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define BUFFER_SIZE       64
#define RXTX_ADDR         0x1000
#define LATENCY           5

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

int test_hyper(int frequency)
{
    int error = 0;

    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 3);
    /* Enable IRQ */
    HYPERBUS0->IRQ_EN  = 1;

    HYPERBUS0->TIMING_CFG  =
        HYPERBUS_TIMING_CFG_CSM(666)                           |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(0) |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(1)                      |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(6)                     |
        HYPERBUS_TIMING_CFG_LATENCY1(0)                        |
        HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);

    HYPERBUS0->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(0) | HYPERBUS_DEVICE_DT0(0) | HYPERBUS_DEVICE_TYPE(0);
    HYPERBUS0->CLK_DIV     = 25000000/frequency - 1;

    // OCTO PSRAM Register Read 0 (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 1 (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x1;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 2 (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x2;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 3 (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x3;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 4 (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x4;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);
    // OCTO PSRAM Register Read 8 (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x8;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);

    // OCTO PSRAM Register Write (0 latency)
    HYPERBUS0->TIMING_CFG  &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    HYPERBUS0->OSPI_CMD    = 0xC000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    rx_buffer[0] = 0x08;
    rx_buffer[1] = 0x08;
    ospi_write((uint32_t)rx_buffer, 2);

    // OCTO PSRAM Register Read 0 (5 latency)
    HYPERBUS0->TIMING_CFG  &= ~HYPERBUS_TIMING_CFG_LATENCY0_MASK;
    HYPERBUS0->TIMING_CFG  |= HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2);

    // OCTO PSRAM RAM Write
    HYPERBUS0->OSPI_CMD    = 0x8000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE);

    // OCTO PSRAM RAM Read
    HYPERBUS0->OSPI_CMD    = 0x0000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);

    // OCTO PSRAM RAM linear Write
    HYPERBUS0->OSPI_CMD    = 0xA000;
    HYPERBUS0->OSPI_ADDR   = 0x100;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE);

    // OCTO PSRAM RAM linear Read
    HYPERBUS0->OSPI_CMD    = 0x2000;
    HYPERBUS0->OSPI_ADDR   = 0x100;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);


    for (int i = 0; i < BUFFER_SIZE; i++)
        if(tx_buffer[i] != rx_buffer[i]) {
            error++;
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

    error = test_hyper(25*1000000);

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return 0;
}
