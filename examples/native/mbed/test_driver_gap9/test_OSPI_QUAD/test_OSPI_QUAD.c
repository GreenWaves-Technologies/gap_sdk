// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"
#include "mbed_wait_api.h"

#define BUFFER_SIZE       128
#define P_ERR 6
#define E_ERR 5
#define DUMMY 8

#define FAST_READ  1
#define FLASH_ADDR  0x200

#define OCTO_SPI    0x0
#define QUAD_SPI    0x1
#define SINGLE_SPI  0x2

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

static uint32_t read_register(uint8_t cmd, int size)
{
    uint32_t reg = 0;

    HYPERBUS0->OSPI_CMD    = (cmd << 8);
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( QUAD_SPI << 8) | (0x0 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)&reg, size);

    return reg;
}

static void write_enable(int spi_type)
{
    HYPERBUS0->OSPI_CMD    = (0x06 << 8);
    HYPERBUS0->OSPI_ADDR   = 0x0;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( spi_type << 8) | (0x0 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)NULL, 0);
}

static void conf_flash()
{
    /* Set QPI mode - Single */
    write_enable(SINGLE_SPI);

    // Set dummy cycles    :  CR2V[3:0] = DUMMY
    // Address length of 24:  CR2V[7] = 0
    // Address length of 32:  CR2V[7] = 1
    HYPERBUS0->OSPI_CMD    = (0x71 << 8);
    HYPERBUS0->OSPI_ADDR   = (0x80000300 | (0x40 | DUMMY));
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x4 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)NULL, 0);
}

static void erase_page_in_flash(uint32_t flash_addr)
{
    write_enable(QUAD_SPI);

    HYPERBUS0->OSPI_CMD    = (0x20 << 8);
    HYPERBUS0->OSPI_ADDR   = (flash_addr << 8);
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( QUAD_SPI << 8) | (0x3 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)NULL, 0);
}

static void write_page_in_flash(uint32_t flash_addr)
{
    write_enable(QUAD_SPI);

    //p4pp age program
    HYPERBUS0->OSPI_CMD    = (0x02 << 8);
    HYPERBUS0->OSPI_ADDR   = (flash_addr << 8);
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( QUAD_SPI << 8) | (0x3 << 4) | 1;
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE);
}

static void read_page_from_flash(uint32_t flash_addr)
{
    /* Fast read - 133 MHz, Normal read - 50 MHz */
    HYPERBUS0->TIMING_CFG  = (665 << 18) | (0 << 17) | (1 << 14) | (6 << 10) | DUMMY;

    #ifdef USE_0xEC
    /* Temporary solution, 2 bytes command with 2 bytes alternative for specific device for OSPI_CMD
     * The address size OSPI_ADDR is always 4.
     * As a result, if address_size <=4, always only OSPI_ADDR bytes are written.
     *              if 4 < address_size <=6, 4 bytes OSPI_ADDR are written firstly, then 1-2 bytes ALternative bytes are written.
     *              Other size has no sense. 
     */
    HYPERBUS0->OSPI_CMD    = 0x0A << 24 | (0xEC << 8);
    HYPERBUS0->OSPI_ADDR   = (flash_addr);
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( 1 << 10) | ( QUAD_SPI << 8) | (0x5 << 4) | 1;
    #else
    HYPERBUS0->OSPI_CMD    = (0xEB << 8);
    HYPERBUS0->OSPI_ADDR   = (flash_addr << 8) | 0x0A;
    HYPERBUS0->OSPI_CFG    = ( 0x7 << 12 ) | ( QUAD_SPI << 8) | (0x4 << 4) | 1;
    #endif
    HYPERBUS0->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);
}

static int wait_process_end(uint32_t err_bit)
{
    // Wait the end of erasing process
    // Status Register 1(SR1V)
    // bit 0 -> 1-busy 0-idle
    // bit 1 -> 1-we
    // bit 6 -> 1-error 0-OK
    uint8_t ret;
    do {
        ret = read_register(0x05, 4);

        if (ret & (1 << err_bit)) {
            printf("P_ERR bit = 1, an error occurs in the programming operation\n");
            return 1;
        }

    } while(ret & 0x01);

    return 0;
}

int test_octo_spi(int frequency)
{
    /* Clock gating enable */
    UDMA_GC->CG |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask((PER_ID_HYPERBUS << 2) + 3);
    /* Enable IRQ */
    HYPERBUS0->IRQ_EN  = 1;

    HYPERBUS0->TIMING_CFG  = (665 << 18) | (0 << 17) | (1 << 14) | (6 << 10) | 0;
    HYPERBUS0->DEVICE_TYPE = (0 << 2) | (0 << 1) | 0;
    HYPERBUS0->CLK_DIV     = 25000000/frequency - 1;

    // Flash Configuration
    conf_flash();

    /* Read ID in single mode */
    int id = read_register(0x9f, 4);
    printf("ID = %x\n", id);

    /* Erase page */
    erase_page_in_flash(FLASH_ADDR);
    printf("Erasing Flash ... \n");
    wait_process_end(E_ERR);

    /* Write buffer */
    write_page_in_flash(FLASH_ADDR);
    printf("Writing Flash ... \n");
    wait_process_end(P_ERR);

    /* Read back buffer */
    printf("Reading Flash ... \n");
    read_page_from_flash(FLASH_ADDR);

    // Check read values
    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if(rx_buffer[i] != (char)(i)) error++;
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os OCTO QUAD SPI test\n");

    int error = 0;

    for (int i = 0; i< BUFFER_SIZE; i++)
    {
        tx_buffer[i] = i;
        rx_buffer[i] = 0xff;
    }

    error = test_octo_spi(25*1000000);

    if (error)
        printf("Test failed\n");
    else
        printf("Test success\n");

    return 0;
}
