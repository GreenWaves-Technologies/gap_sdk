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
    __IO  uint32_t OSPI_ALTER;               /**< HYPERBUS Octo SPI address register, offset: 0x38 */
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

void ospi_write(uint32_t l2_addr, int size)
{
    if (size)
        TEST_HYPERBUS->TRANS_AUTO  = 1;
    else
        TEST_HYPERBUS->TRANS_AUTO  = 0;

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
    if (size)
        TEST_HYPERBUS->TRANS_AUTO  = 1;
    else
        TEST_HYPERBUS->TRANS_AUTO  = 0;

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

static uint32_t read_register(uint8_t cmd, int size)
{
    uint32_t reg = 0;

    TEST_HYPERBUS->OSPI_CMD    = (cmd << 8);
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x0 << 4) | 1;
    TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_read((uint32_t)&reg, size);

    return reg;
}

static void write_enable()
{
    TEST_HYPERBUS->OSPI_CMD    = (0x06 << 8);
    TEST_HYPERBUS->EXT_ADDR    = 0x0;
    TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x0 << 4) | 1;
    TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)NULL, 0);
}

static void conf_flash()
{
    /* Set QPI mode - Single */
    write_enable();

    // Set dummy cycles
    TEST_HYPERBUS->OSPI_CMD    = (0x71 << 8);
    TEST_HYPERBUS->EXT_ADDR    = (0x80000300 | DUMMY);
    TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x4 << 4) | 1;
    TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)NULL, 0);
}

static void erase_page_in_flash(uint32_t flash_addr)
{
    write_enable();

    TEST_HYPERBUS->OSPI_CMD    = (0x20 << 8);
    TEST_HYPERBUS->EXT_ADDR    = (flash_addr << 8);
    TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x3 << 4) | 1;
    TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)NULL, 0);
}

static void write_page_in_flash(uint32_t flash_addr)
{
    write_enable();

    //p4pp age program
    TEST_HYPERBUS->OSPI_CMD    = (0x02 << 8);
    TEST_HYPERBUS->EXT_ADDR    = (flash_addr << 8);
    TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x3 << 4) | 1;
    TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
    ospi_write((uint32_t)tx_buffer, BUFFER_SIZE);
}

static void read_page_from_flash(uint32_t flash_addr)
{
    /* Fast read - 133 MHz, Normal read - 50 MHz */
    if( FAST_READ == 1) {
        TEST_HYPERBUS->TIMING_CFG  = (665 << 18) | (0 << 17) | (1 << 14) | (6 << 10) | DUMMY;

        TEST_HYPERBUS->OSPI_CMD    = (0x0B << 8);
        TEST_HYPERBUS->EXT_ADDR    = (flash_addr << 8);
        TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x3 << 4) | 1;
        TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
        ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);

        TEST_HYPERBUS->TIMING_CFG  = (665 << 18) | (0 << 17) | (1 << 14) | (6 << 10) | 0;
    } else {
        TEST_HYPERBUS->OSPI_CMD    = (0x03 << 8);
        TEST_HYPERBUS->EXT_ADDR    = (flash_addr << 8);
        TEST_HYPERBUS->OSPI_CFG    = ( 0x7 << 12 ) | ( SINGLE_SPI << 8) | (0x3 << 4) | 1;
        TEST_HYPERBUS->OSPI_CSN    = (0x0 << 4) | 0;
        ospi_read((uint32_t)rx_buffer, BUFFER_SIZE);
    }
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
    UDMA_GC->RST |= (1 << PER_ID_HYPERBUS);

    /* Attach event unit for end interrupt */
    SOC_EU_SetFCMask(40 + 31);

    /* Enable IRQ */
    TEST_HYPERBUS->IRQ_EN      = HYPERBUS_IRQ_EN(1);

    TEST_HYPERBUS->TIMING_CFG  = (665 << 18) | (0 << 17) | (1 << 14) | (6 << 10) | 0;
    TEST_HYPERBUS->DEVICE_TYPE = (0 << 2) | (0 << 1) | 0;
    TEST_HYPERBUS->CLK_DIV     = 25000000/frequency - 1;

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
    printf("Fabric controller code execution for mbed_os OCTO Single SPI test\n");

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

    return error;
}
