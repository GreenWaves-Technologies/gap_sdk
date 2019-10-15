#include<string.h>
// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "spi_multi_api.h"

#define BUFFER_SIZE 64
#define L2_ADDR 0x1c070000
#define DUMMY_CYCLE 16

/* Command List
 * reg0[7:0] // bit 0 enables qpi
 * reg1[7:0] // number of dummy cycles
 * reg2[7:0] // wrap length, low
 * reg3[7:0] // wrap length, high
 */
#define WR_REG0   0x01
#define WR_REG1   0x11
#define WR_REG2   0x20
#define WR_REG3   0x30
#define WR_MEM    0x02

#define RD_REG0   0x05
#define RD_REG1   0x07
#define RD_REG2   0x21
#define RD_REG3   0x31
#define RD_MEM    0x0B

GAP_L2_DATA char SPI_RX_BUFFER[BUFFER_SIZE];  // BUFFER_SIZE * 8 bits
GAP_L2_DATA char SPI_TX_BUFFER[BUFFER_SIZE];  // BUFFER_SIZE * 8 bits

uint8_t uint32_invert(uint32_t reg) {
    uint32_t reg_inv = 0;

    reg_inv |= ((reg & (0xff << 0 * 8)) << (3 * 8));
    reg_inv |= ((reg & (0xff << 1 * 8)) << (1 * 8));
    reg_inv |= ((reg & (0xff << 2 * 8)) >> (1 * 8));
    reg_inv |= ((reg & (0xff << 3 * 8)) >> (3 * 8));

    return (reg_inv & 0xff);
}

spi_command_sequence_t s_command;

static void write_register(spi_t *spim, uint8_t cmd, uint8_t val, uint8_t qpi)
{
    /* Set QPI mode */
    spi_master_qspi(spim, qpi);

    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = cmd;
    s_command.tx_bits   = 8;
    s_command.tx_data   = val;
    s_command.cmd_mode  = qpi;
    spi_master_transfer_command_sequence(spim, &s_command);

    printf("Write CMD_0x%x = 0x%x\n", cmd, val);
}

static uint32_t read_register(spi_t *spim, uint8_t cmd, uint8_t qpi)
{
    /* Set QPI mode */
    spi_master_qspi(spim, qpi);

    uint32_t reg = spi_master_write(spim, cmd);
    printf("Read CMD_0x%x = 0x%x\n", cmd, uint32_invert(reg));

    return reg;
}

static void write_memory(spi_t *spim, uint32_t l2_addr, uint16_t size, uint8_t qpi)
{
    /* Set QPI mode */
    spi_master_qspi(spim, qpi);

    /* Wrap length, high */
    write_register(spim, WR_REG3, (size/4) >> 8, qpi);

    /* Wrap length, low */
    write_register(spim, WR_REG2, (size/4) & 0xFF, qpi);

    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = WR_MEM;
    s_command.addr_bits = 32;
    s_command.addr      = l2_addr;
    s_command.cmd_mode  = qpi;
    s_command.addr_mode = qpi;
    s_command.tx_bits   = size * 8;
    if(s_command.tx_bits == 32)
        s_command.tx_data   = 0x03020100;
    else
        s_command.tx_data   = 0;
    s_command.tx_buffer = (uint32_t*)SPI_TX_BUFFER;
    spi_master_transfer_command_sequence(spim, &s_command);

    printf("Write MEM address from 0x%x : size 0x%x\n", l2_addr, size);
}

static void read_memory(spi_t *spim, uint32_t l2_addr, uint32_t dummy_cycle, uint16_t size, uint8_t qpi)
{
    /* Set QPI mode */
    spi_master_qspi(spim, qpi);

    /* Wrap length, high */
    write_register(spim, WR_REG3, (size/4) >> 8, qpi);

    /* Wrap length, low */
    write_register(spim, WR_REG2, (size/4) & 0xFF, qpi);

    /* Number of dummy cycles */
    write_register(spim, WR_REG1, dummy_cycle - 1, qpi);

    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = RD_MEM;
    s_command.addr_bits = 32;
    s_command.addr      = l2_addr;
    s_command.cmd_mode  = qpi;
    s_command.addr_mode = qpi;
    s_command.dummy     = dummy_cycle;
    s_command.rx_bits   = size * 8;
    s_command.rx_buffer = (uint32_t*)SPI_RX_BUFFER;
    spi_master_transfer_command_sequence(spim, &s_command);

    printf("Read MEM address from 0x%x : size 0x%x\n", l2_addr, size);
}

int main()
{
    printf("Fabric controller code execution for mbed_os SPI SLAVE LOOP test\n");

    spi_t spis0;
    /* SPI pins init, SPI udma channel init */
    spis_init(&spis0, SPIS_SDIO1_B40, SPIS_SDIO0_A44, SPIS_SCLK_B13, SPIS_CSN_A15, SPIS_SDIO2_B14, SPIS_SDIO3_A8);

    printf("SPIS_SDIO0  = %d \n", SPIS_SDIO0_A44);
    printf("SPIS_SDIO1  = %d \n", SPIS_SDIO1_B40);
    printf("SPIS_SDIO2  = %d \n", SPIS_SDIO2_B14);
    printf("SPIS_SDIO3  = %d \n", SPIS_SDIO3_A8);
    printf("SPIS_CLK    = %d \n", SPIS_SCLK_B13);
    printf("SPIS_CSN    = %d \n", SPIS_CSN_A15);

    spi_t spim0;
    /* SPI pins init, SPI udma channel init */
    spi_init(&spim0, SPI0_SDIO0, SPI0_SDIO1, SPI0_SCLK, SPI0_CSN0);

    printf("SPI0_MOSI  = %d \n", SPI0_SDIO0);
    printf("SPI0_MISO  = %d \n", SPI0_SDIO1);
    printf("SPI0_SDIO2 = %d \n", SPI0_SDIO2);
    printf("SPI0_SDIO3 = %d \n", SPI0_SDIO3);
    printf("SPI0_CSN0  = %d \n", SPI0_CSN0);
    printf("SPI0_SCLK  = %d \n", SPI0_SCLK);

    /* SPI bits, cpha, cpol configuration */
    spi_format(&spim0, 32, 0, 0);

    /* Set fequence to 10MHz */
    spi_frequency(&spim0, 10000000);

    /* Wrap length, high */
    write_register(&spim0, WR_REG3, 0x00, uSPI_Single);

    /* Wrap length, low */
    write_register(&spim0, WR_REG2, 0x00, uSPI_Single);

    /* Number of dummy cycles */
    write_register(&spim0, WR_REG1, 0x00, uSPI_Single);

    /* Enable QPI. REG0[0] = 1 */
    write_register(&spim0, WR_REG0, 0x01, uSPI_Single);

    /* Read register 0 */
    read_register(&spim0, RD_REG0, uSPI_Quad);

    // Initialize TX buffer
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        SPI_TX_BUFFER[i] = (char)(i);
    }

    // Write and read memory
    int error = 0;
    for (int i = 1; i <= BUFFER_SIZE / 4; i++) {
        /* Write to memory */
        write_memory(&spim0, L2_ADDR, 4 * i, uSPI_Quad);

        /* Read from meory */
        read_memory(&spim0, L2_ADDR, DUMMY_CYCLE, 4 * i, uSPI_Quad);

        /* Check result */
        for (int j = 0; j < 4 * i; j++)
        {
            if(SPI_RX_BUFFER[j] != (char)(j)) error++;
            /* printf("%x \n", SPI_RX_BUFFER[j]); */
        }
    }

    /* for (int i = 0; i < BUFFER_SIZE / 4; i++) */
    /* { */
    /*     printf("Mem_addr 0x%08x = 0x%08x \n",  (L2_ADDR + 4 * i), (*(volatile uint32_t *)(L2_ADDR + 4 * i))); */
    /* } */

    /* Disable QPI. BIT[0] = 0 */
    write_register(&spim0, WR_REG0, 0x00, uSPI_Quad);

    /* Read register 0 */
    read_register(&spim0, RD_REG0, uSPI_Single);

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
