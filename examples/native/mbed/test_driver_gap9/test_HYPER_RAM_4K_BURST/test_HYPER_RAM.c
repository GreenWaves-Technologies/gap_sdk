// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define BUFFER_SIZE       4096
#define CONF_REG_DEFAULT  0x8F1F

#define ADDR_START        0x0
#define ADDR_END          0x800000

#define RAM_ADDR     0x0
#define FLASH_ADDR   0x10000000
#define LATENCY      6

uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

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

void hyper_write(int addr, uint32_t l2_addr, int size)
{
    HYPERBUS0->EXT_ADDR   = RAM_ADDR + addr;

    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    HYPERBUS0->UDMA_HYPERBUS.TX_SADDR = l2_addr;
    HYPERBUS0->UDMA_HYPERBUS.TX_SIZE  = size;
    HYPERBUS0->UDMA_HYPERBUS.TX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);

    if (HYPERBUS0->STATUS & 0x1)
    {
        HYPERBUS0->STATUS = 0x1;
        printf("TX ERROR\n");
    }
}

void hyper_read(int addr, uint32_t l2_addr, int size)
{
    HYPERBUS0->EXT_ADDR   = RAM_ADDR + addr;

    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    HYPERBUS0->UDMA_HYPERBUS.RX_SADDR = l2_addr;
    HYPERBUS0->UDMA_HYPERBUS.RX_SIZE  = size;
    HYPERBUS0->UDMA_HYPERBUS.RX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);

    if (HYPERBUS0->STATUS & 0x2)
    {
        HYPERBUS0->STATUS = 0x2;
        printf("RX ERROR\n");
    }
}

int test_hyper(int frequency)
{
    int error = 0;

    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(PER_ID_HYPERBUS << 2);
    SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 1);

    HYPERBUS0->TIMING_CFG  =
        HYPERBUS_TIMING_CFG_CSM(666)                           |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(1) |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(1)                      |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(6)                     |
        HYPERBUS_TIMING_CFG_LATENCY1(0)                        |
        HYPERBUS_TIMING_CFG_LATENCY0(LATENCY);

    HYPERBUS0->MBR0       = RAM_ADDR;
    HYPERBUS0->MBR1       = FLASH_ADDR;
    HYPERBUS0->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(1) | HYPERBUS_DEVICE_DT0(0) | HYPERBUS_DEVICE_TYPE(1);

    HYPERBUS0->CLK_DIV     = 25000000/frequency - 1;

    /* BURST configuration : Set Burst Size then Enable it */
    HYPERBUS0->BURST_SIZE   = 0x80;
    HYPERBUS0->BURST_ENABLE = 1;

    /* Write and Read 4K each time in all address of HYPERRAM */
    for (int addr = ADDR_START; addr < ADDR_END; addr+=BUFFER_SIZE)
    {
        hyper_write(addr, (uint32_t)tx_buffer, BUFFER_SIZE);
        hyper_read (addr, (uint32_t)rx_buffer, BUFFER_SIZE);

        for (int n = 0; n < BUFFER_SIZE; n++)
            if(tx_buffer[n] != rx_buffer[n]) {
                error++;
                return -1;
            }
    }


    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os HYPERBUS RAM 4K burst driver test\n");

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
