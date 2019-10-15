// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define FLASH0_ADDR   0x0
#define FLASH1_ADDR   0x20000000

#define LATENCY                 6

#define BUFFER_SIZE             256
#define DEVICE_READY_OFFSET     7
#define ERASE_STATUS_OFFSET     5
#define PROGRAM_STATUS_OFFSET   4

/* Write and read address */
#define SA                      0x4000

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

uint32_t flash_address = FLASH0_ADDR;

void hyper_write(int addr, uint32_t l2_addr, int size)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    HYPERBUS0->EXT_ADDR   = flash_address + addr;

    HYPERBUS0->UDMA_HYPERBUS.TX_SADDR = l2_addr;
    HYPERBUS0->UDMA_HYPERBUS.TX_SIZE  = size;
    HYPERBUS0->UDMA_HYPERBUS.TX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

void hyper_read(int addr, uint32_t l2_addr, int size)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    HYPERBUS0->EXT_ADDR   = flash_address + addr;

    HYPERBUS0->UDMA_HYPERBUS.RX_SADDR = l2_addr;
    HYPERBUS0->UDMA_HYPERBUS.RX_SIZE  = size;
    HYPERBUS0->UDMA_HYPERBUS.RX_CFG   = UDMA_CFG_EN(1);

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

void erase_page_in_flash( uint32_t addr)
{
    /* Erase sector 0x0E00 */
    for ( int i = 0; i < 5; i++) {
        hyper_write(Erase_Seq[i].addr << 1, (uint32_t)&Erase_Seq[i].data, 2);
    }

    hyper_write(addr, (uint32_t)&Erase_Seq[5].data, 2);
}

void write_page_in_flash( uint32_t addr)
{
    int i = 0;

    /* Write to Buffer command sequence */
    for (i = 0; i < 3; i++)
        hyper_write(WP_Seq[i].addr << 1, (uint32_t)&WP_Seq[i].data, 2);

    /* Word Program */
    hyper_write(addr, (uint32_t ) txHyperbusSamples, BUFFER_SIZE);
}

void read_page_from_flash( uint32_t addr)
{
    hyper_read(addr, (uint32_t ) rxHyperbusSamples, BUFFER_SIZE);
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

int test_hyperbus(int addr)
{
    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(PER_ID_HYPERBUS << 2);
    SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 1);

    HYPERBUS0->TIMING_CFG  = (665 << 18) | (1 << 17) | (1 << 14) | (6 << 10) | (LATENCY << 5) | LATENCY;
    HYPERBUS0->MBR0        = FLASH0_ADDR;
    HYPERBUS0->MBR1        = FLASH1_ADDR;
    HYPERBUS0->DEVICE_TYPE = (1 << 2) | (1 << 1) | 1;

    // Set access address
    flash_address = addr;

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
    printf("Fabric controller code execution for mbed_os TWO HYPERBUS FLASH test\n");

    int error;

    /* Test hyperflash 0 */
    error = test_hyperbus(FLASH0_ADDR);

    /* Test hyperflash 1 */
    error = test_hyperbus(FLASH1_ADDR);

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
