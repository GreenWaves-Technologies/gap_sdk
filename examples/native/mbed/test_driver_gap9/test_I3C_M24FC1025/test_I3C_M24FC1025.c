// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"
#include "mbed_wait_api.h"

#define BUFFER_SIZE 4

#define TARGET_SLAVE      0

#define I3C               0
#define I2C               1

#define TOC               0
#define ROC               1
#define TID               1

#define REGULAR_CMD       0
#define IMM_CMD           1

#define M24FC1025_Address    0xA0

int instance = 0;

static I3C_Type *const i3c_addrs[] = I3C_BASE_PTRS;

uint8_t rx_buffer[BUFFER_SIZE];

uint8_t send_data[3] = {0, 0, 0};

/* Size is in byte */
void i3c_write(char addr_7_bits, uint32_t *buf, uint16_t size, uint8_t target_slave)
{
    i3c_addrs[instance]->MST_CNTL_EN = 0x1;
    i3c_addrs[instance]->DEVICE_ADDR_TABLE[target_slave] = (I2C << 31)| 0x83000 | addr_7_bits;

    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    if (size <= 4) {
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = buf[0];
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (0 << 29) | (size << 23) | (target_slave << 16) | IMM_CMD;

    } else {
        for (int i = 0; i < size; i+=4)
        {
            i3c_addrs[instance]->DATA_TX_FIFO     = buf[i >> 1];
        }
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = (size << 16);
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (0 << 29) | (target_slave << 16) | (TID << 3) | REGULAR_CMD;
    }

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

/* Size is in byte */
void i3c_read(char addr_7_bits, uint32_t *buf, uint16_t size, uint8_t target_slave)
{
    int reg;

    i3c_addrs[instance]->MST_CNTL_EN = 0x1;
    i3c_addrs[instance]->DEVICE_ADDR_TABLE[target_slave] = (I2C << 31) | 0x83000 | addr_7_bits;

    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    if (size <= 4) {
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = 0x0;
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (1 << 29) | (size << 23) | (target_slave << 16) | IMM_CMD;

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        reg = i3c_addrs[instance]->RESP;

        if (i3c_addrs[instance]->IRQ_STATUS & 0x1)
            buf[0] = i3c_addrs[instance]->DATA_RX_FIFO;
    } else {
        i3c_addrs[instance]->CMD_TR_REQ_REG1      = (size << 16);
        i3c_addrs[instance]->CMD_TR_REQ_REG2      = (TOC << 31) | (ROC << 30) | (1 << 29) | (target_slave << 16) | (TID << 3) | REGULAR_CMD;

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        reg = i3c_addrs[instance]->RESP;
        reg = i3c_addrs[instance]->IRQ_STATUS;

        for (int i = 0; i < size; i+=4)
        {
            buf[i] = i3c_addrs[instance]->DATA_RX_FIFO;
        }
    }

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

void m24fc1025_write_byte(uint32_t addr_17b, uint8_t value) {
    send_data[0] = (uint8_t) (addr_17b >> 8);
    send_data[1] = (uint8_t) (addr_17b & 0xFF);
    send_data[2] = value;

    /* Write */
    i3c_write(M24FC1025_Address >> 1, (uint32_t*) send_data, 3, TARGET_SLAVE);
    wait_ms(1);
}

uint8_t m24fc1025_read_byte(uint32_t addr_17b) {
    uint8_t value[1];

    send_data[0] = (uint8_t) (addr_17b >> 8);
    send_data[1] = (uint8_t) (addr_17b & 0xFF);

    /* Read */
    i3c_write(M24FC1025_Address >> 1, (uint32_t*) send_data, 2, TARGET_SLAVE);
    i3c_read (M24FC1025_Address >> 1, (uint32_t*) value, 1, TARGET_SLAVE);
    wait_ms(1);

    return value[0];
}

int test_i3c(int num)
{
    instance = num;

    /* De-clock gating, enable I3C clock */
    SOC_CTRL->I3C_CTRL = SOC_CTRL_I3C_CTRL_EN0(1) | SOC_CTRL_I3C_CTRL_EN1(1);

    /* Enable IRQ */
    SOC_EU_SetFCMask(S_EVENT_I3C0);
    SOC_EU_SetFCMask(S_EVENT_I3C1);

    // Write first 16 Bytes of memory
    for (uint8_t addr = 0; addr < BUFFER_SIZE ; addr++) { // Rotate bit
        m24fc1025_write_byte(addr, addr);
    }

    // Read data and write it to buffer
    for (uint32_t addr = 0; addr < BUFFER_SIZE; addr++) {
        rx_buffer[addr] = m24fc1025_read_byte(addr);
    }

    SOC_EU_ClearFCMask(S_EVENT_I3C0);
    SOC_EU_ClearFCMask(S_EVENT_I3C1);

    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf("I2C rec=%02x\n", rx_buffer[i]);
        if (rx_buffer[i] != (char)i) error++;
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os I3C driver M24FC1025 test\n");

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
