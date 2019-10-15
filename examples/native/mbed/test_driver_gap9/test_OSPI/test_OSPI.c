// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define UDMA_EVENT  0
#define IRQ         1
#define POLLING     2

#define BUFFER_SIZE       64
#define RXTX_ADDR         0x1000

uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

void ospi_write(uint32_t l2_addr, int size, int type)
{
    if (type == POLLING) {
        /* Clear end flag*/
        HYPERBUS0->STATUS |= HYPERBUS_STATUS_TXRX_END(1);

        HYPERBUS0->UDMA_HYPERBUS.TX_SADDR = l2_addr;
        HYPERBUS0->UDMA_HYPERBUS.TX_SIZE  = size;
        HYPERBUS0->UDMA_HYPERBUS.TX_CFG   = UDMA_CFG_EN(1);

        /* Wait transfer end */
        while (!(HYPERBUS0->STATUS & HYPERBUS_STATUS_TXRX_END_MASK));
        /* Clear end flag*/
        HYPERBUS0->STATUS |= HYPERBUS_STATUS_TXRX_END(1);
    } else {
        /* Disable UDMA IRQ */
        int irq = EU_DisableUDMAIRQ();

        HYPERBUS0->UDMA_HYPERBUS.TX_SADDR = l2_addr;
        HYPERBUS0->UDMA_HYPERBUS.TX_SIZE  = size;
        HYPERBUS0->UDMA_HYPERBUS.TX_CFG   = UDMA_CFG_EN(1);

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        /* Restore IRQ */
        EU_RestoreUDMAIRQ(irq);
    }
}

void ospi_read(uint32_t l2_addr, int size, int type)
{
    if (type == POLLING) {
        /* Clear end flag*/
        HYPERBUS0->STATUS |= HYPERBUS_STATUS_TXRX_END(1);

        HYPERBUS0->UDMA_HYPERBUS.RX_SADDR = l2_addr;
        HYPERBUS0->UDMA_HYPERBUS.RX_SIZE  = size;
        HYPERBUS0->UDMA_HYPERBUS.RX_CFG   = UDMA_CFG_EN(1);

        /* Wait transfer end */
        while (!(HYPERBUS0->STATUS & HYPERBUS_STATUS_TXRX_END_MASK));
        /* Clear end flag*/
        HYPERBUS0->STATUS |= HYPERBUS_STATUS_TXRX_END(1);
    }
    else {
        /* Disable UDMA IRQ */
        int irq = EU_DisableUDMAIRQ();

        HYPERBUS0->UDMA_HYPERBUS.RX_SADDR = l2_addr;
        HYPERBUS0->UDMA_HYPERBUS.RX_SIZE  = size;
        HYPERBUS0->UDMA_HYPERBUS.RX_CFG   = UDMA_CFG_EN(1);

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        /* Restore IRQ */
        EU_RestoreUDMAIRQ(irq);
    }
}

int test_ospi(int frequency, int type)
{
    int error = 0;

    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);

    /* Clean */
    SOC_EU_ClearFCMask(PER_ID_HYPERBUS << 2);
    SOC_EU_ClearFCMask((PER_ID_HYPERBUS << 2) + 1);
    SOC_EU_ClearFCMask((PER_ID_HYPERBUS << 2) + 3);
    HYPERBUS0->IRQ_EN  = 0;

    /* Enable event / irq */
    if (type == UDMA_EVENT) {
        /* Attach event unit for end interrupt */
        SOC_EU_SetFCMask(PER_ID_HYPERBUS << 2);
        SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 1);
    } else if (type == IRQ) {
        SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 3);

        /* Enable IRQ */
        HYPERBUS0->IRQ_EN  = 1;
    }

    HYPERBUS0->TIMING_CFG  = (665 << 18) | (1 << 17) |(1 << 14) | (6 << 10) | 5;
    HYPERBUS0->DEVICE_TYPE = (0 << 2) | (0 << 1) | 0;
    HYPERBUS0->CLK_DIV     = 25000000/frequency - 1;

    // OCTO PSRAM Register Read (5 latency)
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2, type);

    // OCTO PSRAM Register Write (0 latency)
    HYPERBUS0->TIMING_CFG  = (665 << 18) | (1 << 17) | (1 << 14) | (6 << 10) | 0;
    HYPERBUS0->OSPI_CMD    = 0xC000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)rx_buffer, 2, type);

    // OCTO PSRAM Register Read (5 latency)
    HYPERBUS0->TIMING_CFG  = (665 << 18) | (1 << 17) | (1 << 14) | (6 << 10) | 5;
    HYPERBUS0->OSPI_CMD    = 0x4000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, 2, type);

    // OCTO PSRAM RAM Write
    HYPERBUS0->OSPI_CMD    = 0x8000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE, type);

    // OCTO PSRAM RAM Read
    HYPERBUS0->OSPI_CMD    = 0x0000;
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x1 << 12 ) | ( 0 << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, BUFFER_SIZE, type);

    for (int i = 0; i < BUFFER_SIZE; i++)
        if(tx_buffer[i] != rx_buffer[i]) {
            error++;
        }

    /* Clock gating */
    UDMA_GC->CG &= ~(1 << PER_ID_HYPERBUS);

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os HYPERBUS RAM driver test\n");

    int error = 1;

    for (int i = 0; i< BUFFER_SIZE; i++)
    {
        tx_buffer[i] = i;
        rx_buffer[i] = 0xff;
    }

    error = test_ospi(12*1000000,  IRQ);
    error = test_ospi(25*1000000,  POLLING);
    error = test_ospi(12*1000000,  UDMA_EVENT);

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return 0;
}
