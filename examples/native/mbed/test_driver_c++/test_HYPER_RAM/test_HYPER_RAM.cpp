#include "mbed.h"

#define BUFFER_SIZE       1024
#define CONF_REG_DEFAULT  0x8F1F
#define RXTX_ADDR         0x800

GAP_L2_DATA  uint8_t txHyperbusSamples[BUFFER_SIZE];
GAP_L2_DATA  uint8_t rxHyperbusSamples[BUFFER_SIZE];

#define   ID0  0x0000
#define   ID1  0x0002

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
#define   CR1    0x1002

HYPERBUS hyperbus(HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);

int error_count = 0;

static void hyperbus_rx_callback(int events)
{
    printf("Start Check!\n");
    for (int k = 0; k < BUFFER_SIZE; k++)
    {
        if((rxHyperbusSamples[k] % 256) != (unsigned char)(k)) error_count++;
        #ifdef HYPERBUS_DEBUG
        printf("RX_ DATA[%d] = 0x%02x\n", k, rxHyperbusSamples[k]);
        #endif
    }
}

static void hyperbus_tx_callback(int events)
{
    if(events == HYPERBUS_EVENT_COMPLETE)
    {
        printf("TX is finished, event = %d!\n", events);

        event_callback_t event;
        event = hyperbus_rx_callback;

        printf("Start RX!\n");

        /* Read RAM */
        hyperbus.transfer(RXTX_ADDR, (uint32_t *)NULL, 0, (uint32_t *) rxHyperbusSamples, BUFFER_SIZE, event);
    } else {
        printf("TX Fail!\n");
    }
}

int main()
{
    printf("Fabric controller code execution for mbed_os HYPERBUS RAM driver test\n");

    /* Initialize the samples */
    for (int i = 0; i< BUFFER_SIZE; i++)
        txHyperbusSamples[i] = i;

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

#if defined(__GAP8__)
    char latency = 1;

    /* Config memory maximum transfer data length for TX and RX*/
    hyperbus.set_max_length(uHYPERBUS_Ram, 0x1ff, 1);
    hyperbus.set_max_length(uHYPERBUS_Ram, 0x1ff, 1);

    /* Config memory access timing for TX and RX*/
    hyperbus.set_timing(uHYPERBUS_Ram, 4, 4, 4, latency);
    hyperbus.set_timing(uHYPERBUS_Ram, 4, 4, 4, latency);
#else
    char latency = 6;

    hyperbus.set_device_type(uHYPERBUS_HYPERBUS_MODE, uHYPERBUS_Ram, uHYPERBUS_Flash);
    hyperbus.set_timing(latency, 0, 6, 1, 1, 666);
#endif

    printf("Start TX!\n");
    event_callback_t event;
    event = hyperbus_tx_callback;
    /* Write RAM */
    hyperbus.transfer(RXTX_ADDR, (uint32_t *) txHyperbusSamples, BUFFER_SIZE, (uint32_t *)NULL, 0, event);

    /* Wait TX RX Check finish */
    wait_ms(1);

    /* Read RAM register */
    uint16_t config_reg = hyperbus.read(CR0, uHYPERBUS_Reg_Access);

    if(config_reg != CONF_REG_DEFAULT) error_count++;
    printf("Read CR0 6 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny to 5 clock */
    config_reg &= 0xFF0F;
    hyperbus.write(CR0, config_reg, uHYPERBUS_Reg_Access);

    /* Read RAM register */
    config_reg = hyperbus.read(CR0, uHYPERBUS_Reg_Access);
    printf("Read CR0 5 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny back to 6 clock */
    config_reg |= 0x0010;
    hyperbus.write(CR0, config_reg, uHYPERBUS_Reg_Access);

    if (error_count)
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
