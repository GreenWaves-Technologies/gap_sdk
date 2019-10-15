#include<string.h>
// MBED OS
#include "cmsis_os2.h"
#include "i2c_api.h"
#include "mbed_wait_api.h"

#ifdef __PLATFORM_RTL__
#define M24M02_Device_Address    0xA8
#else
#define M24M02_Device_Address    0xA0
#endif

uint8_t init_buffer[] = {0x00, 0x50, 0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};
uint8_t tx_buffer[]   = {0x00, 0x50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t rx_addr[]     = {0x00, 0x50};
uint8_t rx_buffer[16];

#define TARGET_SLAVE      0

#define I3C               0
#define I2C               1

#define TOC               0
#define ROC               1
#define TID               1

#define REGULAR_CMD       0
#define IMM_CMD           1

int instance = 0;

static I3C_Type *const i3c_addrs[] = I3C_BASE_PTRS;

void i3c_init()
{
    /* De-clock gating, enable I3C clock */
    SOC_CTRL->I3C_CTRL = SOC_CTRL_I3C_CTRL_EN0(1) | SOC_CTRL_I3C_CTRL_EN1(1);

    /* Enable IRQ */
    SOC_EU_SetFCMask(S_EVENT_I3C0);
    SOC_EU_SetFCMask(S_EVENT_I3C1);
}

/* Size is in byte */
void i3c_write(char addr_7_bits, uint32_t *buf, uint16_t size, uint8_t target_slave)
{
    i3c_addrs[instance]->MST_CNTL_EN = 0x1;
    i3c_addrs[instance]->DEVICE_ADDR_TABLE[target_slave] = (I2C << 31)| 0x83000 | addr_7_bits;

    if (size <= 4) {
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = buf[0];
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (0 << 29) | (size << 23) | (target_slave << 16) | IMM_CMD;

    } else {
        int trans_length = (size >> 2) + ((size & 0x3) ? 1 : 0);

        for (int i = 0; i < trans_length; i++)
        {
            i3c_addrs[instance]->DATA_TX_FIFO     = buf[i];
        }
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = (size << 16);
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (0 << 29) | (target_slave << 16) | (TID << 3) | REGULAR_CMD;
    }

    do {
        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
    } while (!READ_I3C_IRQ_RSP_DONE(i3c_addrs[instance]->IRQ_STATUS));
}

/* Size is in byte */
void i3c_read(char addr_7_bits, uint32_t *buf, uint16_t size, uint8_t target_slave)
{
    int reg;

    i3c_addrs[instance]->MST_CNTL_EN = 0x1;
    i3c_addrs[instance]->DEVICE_ADDR_TABLE[target_slave] = (I2C << 31) | 0x83000 | addr_7_bits;

    if (size <= 4) {
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = 0x0;
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (1 << 29) | (size << 23) | (target_slave << 16) | IMM_CMD;

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        reg = i3c_addrs[instance]->RESP;
        reg = i3c_addrs[instance]->IRQ_STATUS;

        buf[0] = i3c_addrs[instance]->DATA_RX_FIFO;
    } else {
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = (size << 16);
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (1 << 29) | (target_slave << 16) | (TID << 3) | REGULAR_CMD;

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        reg = i3c_addrs[instance]->RESP;
        reg = i3c_addrs[instance]->IRQ_STATUS;

        int trans_length = (size >> 2) + ((size & 0x3) ? 1 : 0);

        for (int i = 0; i < trans_length; i++)
        {
            buf[i] = i3c_addrs[instance]->DATA_RX_FIFO;
        }
    }
}

int test_i3c(int num)
{
    instance = num;

    i3c_init();

    /* Init the zone where we do the test */
    i3c_write(M24M02_Device_Address >> 1, (uint32_t*) init_buffer, sizeof(init_buffer), TARGET_SLAVE);
    wait_ms(10);

    /* Write into EEPROM */
    i3c_write(M24M02_Device_Address >> 1, (uint32_t*) tx_buffer, sizeof(tx_buffer), TARGET_SLAVE);
    wait_ms(10);

    /* Read from EEPROM */
    i3c_write(M24M02_Device_Address >> 1, (uint32_t*) rx_addr, 2, TARGET_SLAVE);
    i3c_read (M24M02_Device_Address >> 1, (uint32_t*) rx_buffer, 16, TARGET_SLAVE);

    int error = 0;
    for (int i = 0; i< 16; i++) {
        if (rx_buffer[i] != i) error++;
        printf("I2C rec=%02x\n", rx_buffer[i]);
    }

    return error;
}

int main()
{

    printf("Fabric controller code execution for mbed_os I3C driver M24M02 test\n");

    int error = 0;

    /* I3C0 */
    error = test_i3c(0);
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    /* I3C1 */
    error = test_i3c(1);
    if (error) printf("Test failed with %d errors\n", error);
    else printf("\nTest success\n");

    return error;
}
