// MBED OS
#include "cmsis_os2.h"
// HYPERBUS CMSIS driver
#include "hyperbus_api.h"

#define BUFFER_SIZE       1024
#define CONF_REG_DEFAULT  0x8F1F
#define RXTX_ADDR         0x800

GAP_L2_DATA  uint8_t txHyperbusSamples[BUFFER_SIZE];
GAP_L2_DATA  uint8_t rxHyperbusSamples[BUFFER_SIZE];

#define   ID0  0x0
#define   ID1  0x1

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

/* Configuration Register 0 */
#define   CR0    0x1000
/* Configuration Register 1 */
#define   CR1    0x1001

int main()
{
    printf("Fabric controller code execution for mbed_os HYPERBUS RAM driver test\n");

    int error = 0;
    /* Default is 6 Clock Lateny */
    /*
     *   0000 - 5 Clock Latency
     *   0001 - 6 Clock Latency
     *   0010 - Reserved
     *   0000 - Reserved
     *   ...
     *   1110 - 3 Clock Latency
     *   1111 - 4 Clock Latency
     */
    int latency = 1;

    hyperbus_t hyperbus0;
    /* HYPERBUS pins init, HYPERBUS udma channel init */
    hyperbus_init(&hyperbus0, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);

    /* Config memory maximum transfer data length for TX and RX*/
    hyperbus_set_max_length(&hyperbus0, 1, 0x1ff, 0, uHYPERBUS_Ram);
    hyperbus_set_max_length(&hyperbus0, 1, 0x1ff, 1, uHYPERBUS_Ram);

    /* Config memory access timing for TX and RX*/
    hyperbus_set_timing(&hyperbus0, 4, 4, 4, latency, 0, uHYPERBUS_Ram);
    hyperbus_set_timing(&hyperbus0, 4, 4, 4, latency, 1, uHYPERBUS_Ram);

    /* Initialize the samples */
    for (int i = 0; i< BUFFER_SIZE; i++)
        txHyperbusSamples[i] = i;

    /* Write RAM */
    hyperbus_block_write(&hyperbus0, RXTX_ADDR, (uint32_t *) txHyperbusSamples, BUFFER_SIZE, uHYPERBUS_Mem_Access, uHYPERBUS_Ram);

    /* Read RAM */
    hyperbus_block_read(&hyperbus0, RXTX_ADDR, (uint32_t *) rxHyperbusSamples, BUFFER_SIZE, uHYPERBUS_Mem_Access, uHYPERBUS_Ram);

    for (int k = 0; k < BUFFER_SIZE; k++)
    {
        if((rxHyperbusSamples[k] % 256) != (unsigned char)(k)) error++;
        #ifdef HYPERBUS_DEBUG
        printf("RX_ DATA[%d] = 0x%02x\n", k, rxHyperbusSamples[k]);
        #endif
    }

    /* Read RAM register */
    uint16_t config_reg = hyperbus_read(&hyperbus0, CR0, uHYPERBUS_Reg_Access, uHYPERBUS_Ram);

    if(config_reg != (CONF_REG_DEFAULT & (0xFF0F | (latency << 4)))) error++;
    printf("Read CR0 6 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny to 5 clock */
    config_reg &= 0xFF0F;
    hyperbus_write(&hyperbus0, CR0, config_reg, uHYPERBUS_Reg_Access, uHYPERBUS_Ram);

    /* Read RAM register */
    config_reg = hyperbus_read(&hyperbus0, CR0, uHYPERBUS_Reg_Access, uHYPERBUS_Ram);
    printf("Read CR0 5 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny back to 6 clock */
    config_reg |= 0x0010;
    hyperbus_write(&hyperbus0, CR0, config_reg, uHYPERBUS_Reg_Access, uHYPERBUS_Ram);


    /* Release hyperbus driver */
    hyperbus_free(&hyperbus0);


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
