// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"


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
/* GAP_L2_DATA  cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x10, 0x555}}; */

/* Sector erase */
GAP_L2_DATA  cmdSeq Erase_Seq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};
GAP_L2_DATA  cmdSeq VCR_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {0x8e0b, 0x0}};
GAP_L2_DATA  cmdSeq Reg_Seq     = {0x70, 0x555};

/* Write 512/4 = 128 word to Sector addr 0x4xxx */
GAP_L2_DATA  cmdSeq WB_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x25, SA}, {BUFFER_SIZE - 1, SA}};
GAP_L2_DATA  cmdSeq WP_Seq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};
GAP_L2_DATA  cmdSeq BPC_Seq     = {0x29, SA};
GAP_L2_DATA  cmdSeq ID_Seq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x90, 0x555}};
GAP_L2_DATA  cmdSeq ASO_Exit_Seq= {0xF0, 0};

/* The high 16 bits for txHyperbusSamples is useless */
GAP_L2_DATA  uint16_t txHyperbusSamples[BUFFER_SIZE];
GAP_L2_DATA  uint16_t rxHyperbusSamples[BUFFER_SIZE];

GAP_L2_DATA  uint32_t id_reg;

uint16_t read_status(hyperbus_t *obj)
{
    /* Read status register */
    hyperbus_write(obj, Reg_Seq.addr << 1, Reg_Seq.data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);

    uint16_t status_reg = hyperbus_read(obj, 0, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);

    #ifdef HYPERBUS_DEBUG
    printf("Read status_reg = 0x%04x\n", status_reg);
    #endif

    return status_reg;
}

uint32_t read_ID(hyperbus_t *obj)
{
    /* Read status register */
    for (int i = 0; i < 3; i++)
        hyperbus_write(obj, ID_Seq[i].addr << 1, ID_Seq[i].data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);

    /*
     * manufacturer ID = 0x0001
     * Device       ID = 0x007e
     */
    hyperbus_block_read(obj, 0, &id_reg, 2 * sizeof(uint16_t), uHYPERBUS_Mem_Access, uHYPERBUS_Flash);

    #ifdef HYPERBUS_DEBUG
    printf("ID = 0x%x\n", id_reg);
    #endif

    /* Reset / ASO exit */
    hyperbus_write(obj, ASO_Exit_Seq.addr << 1, ASO_Exit_Seq.data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);

    return id_reg;
}

void conf_flash(hyperbus_t *obj)
{
    /* Set VCR to 5 delay cycles */
    for (int i = 0; i < 4; i++)
        hyperbus_write(obj, VCR_Seq[i].addr << 1, VCR_Seq[i].data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);
}

void erase_page_in_flash(hyperbus_t *obj, uint32_t flash_addr)
{
    /* Erase sector 0x0E00 */
    for ( int i = 0; i < 5; i++) {
        hyperbus_write(obj, Erase_Seq[i].addr << 1, Erase_Seq[i].data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);
    }

    hyperbus_write(obj, flash_addr, Erase_Seq[5].data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);
}

void write_page_in_flash(hyperbus_t *obj, uint32_t flash_addr)
{
    int i = 0;

    /* Write to Buffer command sequence */
    for (i = 0; i < 3; i++)
        hyperbus_write(obj, WP_Seq[i].addr << 1, WP_Seq[i].data, uHYPERBUS_Mem_Access, uHYPERBUS_Flash);

    /* Word Program */
    hyperbus_block_write(obj, flash_addr, (uint32_t *) txHyperbusSamples, BUFFER_SIZE * sizeof(uint16_t), uHYPERBUS_Mem_Access, uHYPERBUS_Flash);
}

void read_page_from_flash(hyperbus_t *obj, uint32_t flash_addr)
{
    hyperbus_block_read(obj, flash_addr, (uint32_t *) rxHyperbusSamples, BUFFER_SIZE * sizeof(uint16_t), uHYPERBUS_Mem_Access, uHYPERBUS_Flash);
}

int wait_process_end(hyperbus_t *obj, uint32_t err_bit)
{
    uint16_t reg;

    /* Wait the end of process
     * Status Register (SR)
     * bit 4 -> program status bit, 0-success ; 1-failure
     * bit 5 -> erase status bit,   0-success ; 1-failure
     * bit 7 -> device ready bit,   0-busy    ; 1-ready
     */
    do {
        reg = read_status(obj);

        if ( reg & (1 << err_bit))
        {
            printf("ERR bit = 1, an error occurs in  operation\n");
            return 1;
        }
    } while( !(reg & (1 << DEVICE_READY_OFFSET)));

    return 0;
}

int test_hyperbus()
{
    hyperbus_t hyperbus0;

    /* Hyperbus Flash Default latency is 5 clock, can not change */
    int latency = 0;

    /* HYPERBUS pins init, HYPERBUS udma channel init */
    hyperbus_init(&hyperbus0, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);

    /* Config memory access timing for TX and RX*/
    hyperbus_set_timing(&hyperbus0, 4, 4, 4, latency, 0, uHYPERBUS_Flash);
    hyperbus_set_timing(&hyperbus0, 4, 4, 4, latency, 1, uHYPERBUS_Flash);

    /* Initialize the samples */
    for (int i = 0; i< BUFFER_SIZE; i++)
        txHyperbusSamples[i] = i;

    /* Configuration */
    conf_flash(&hyperbus0);

    /* Read ID */
    read_ID(&hyperbus0);

    /* Erase chip */
    erase_page_in_flash(&hyperbus0, SA);
    wait_process_end(&hyperbus0,  ERASE_STATUS_OFFSET);
    printf("Erase finished.\n");

    /* Write buffer no burst */
    write_page_in_flash(&hyperbus0, SA);
    wait_process_end(&hyperbus0,  PROGRAM_STATUS_OFFSET);
    printf("Write finished.\n");

    /* Read buffer burst */
    read_page_from_flash(&hyperbus0, SA);
    printf("Read finished.\n");

    /* Release hyperbus driver */
    hyperbus_free(&hyperbus0);

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
    printf("Fabric controller code execution for mbed_os HYPERBUS FLASH driver test\n");

    int error;
    error = test_hyperbus();

    printf("clk  = %d \n", HYPERBUS_CLK);
    printf("clkn = %d \n", HYPERBUS_CLKN);
    printf("rwds = %d \n", HYPERBUS_RWDS);
    printf("dq0  = %d \n", HYPERBUS_DQ0);
    printf("dq1  = %d \n", HYPERBUS_DQ1);
    printf("dq2  = %d \n", HYPERBUS_DQ2);
    printf("dq3  = %d \n", HYPERBUS_DQ3);
    printf("dq4  = %d \n", HYPERBUS_DQ4);
    printf("dq5  = %d \n", HYPERBUS_DQ5);
    printf("dq6  = %d \n", HYPERBUS_DQ6);
    printf("dq7  = %d \n", HYPERBUS_DQ7);

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
