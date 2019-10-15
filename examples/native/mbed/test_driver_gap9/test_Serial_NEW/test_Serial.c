// MBED OS
#include "cmsis_os2.h"
// SPI CMSIS driver
#include "serial_api.h"

#define BUFFER_SIZE 4


#define UDMA_EVENT  0
#define IRQ         1
#define POLLING     2

uint8_t rx_buffer[BUFFER_SIZE];

static UART_Type *const s_uart[] = UART_BASE_PTRS;

UART_Type *UART_PTR;

void uart_write(uint32_t l2_addr, int size)
{
    /* Disable UDMA IRQ */
    int irq = EU_DisableUDMAIRQ();

    UART_PTR->UDMA_UART.TX_SADDR = l2_addr;
    UART_PTR->UDMA_UART.TX_SIZE  = size;
    UART_PTR->UDMA_UART.TX_CFG   = UDMA_CFG_EN(1);

    ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

    /* Restore IRQ */
    EU_RestoreUDMAIRQ(irq);
}

void uart_read(uint8_t *l2_addr, int size, int type)
{
    if (type == POLLING) {
        for (int i = 0; i < size; i++) {

            while (!UART_PTR->VALID);
            l2_addr[i] = UART_PTR->DATA;
        }
    } else if (type == UDMA_EVENT) {
        /* Disable UDMA IRQ */
        int irq = EU_DisableUDMAIRQ();

        UART_PTR->UDMA_UART.RX_SADDR = (uint32_t)l2_addr;
        UART_PTR->UDMA_UART.RX_SIZE  = size;
        UART_PTR->UDMA_UART.RX_CFG   = UDMA_CFG_EN(1);

        ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);

        /* Restore IRQ */
        EU_RestoreUDMAIRQ(irq);
    } else if (type == IRQ) {
        int cnt = 0;
        /* Disable UDMA IRQ */
        int irq = EU_DisableUDMAIRQ();

        while (cnt < size ) {
            ITC_WaitEvent_NOIRQ(FC_SOC_EVENT_IRQn);
            l2_addr[cnt++] = UART_PTR->DATA;
        }

        /* Restore IRQ */
        EU_RestoreUDMAIRQ(irq);
    }
}

void uart_set_baud(int baud) {
    /* Calculate the baud rate modulo divisor, sbr*/
    uint16_t div = 50000000 / baud;

    /* Disable UART RX TX */
    UART_PTR->SETUP &= ~(UART_SETUP_TX_ENA_MASK | UART_SETUP_RX_ENA_MASK);
    /* 8N1*/
    UART_PTR->SETUP = (UART_PTR->SETUP & ~(UART_SETUP_CLKDIV_MASK)) |
        UART_SETUP_CLKDIV(div) |
        UART_SETUP_TX_ENA(1)   |
        UART_SETUP_RX_ENA(1);

}

int test_uart(int id, int baud, int type)
{
    int error = 0;

    UART_PTR = s_uart[id];

    /* Clock gating disable */
    UDMA_GC->CG |= (1 << (PER_ID_UART0 + id));

    /* Clean event */
    SOC_EU_ClearFCMask((PER_ID_UART0 + id) << 2);
    SOC_EU_ClearFCMask(((PER_ID_UART0 + id) << 2) + 1);
    SOC_EU_ClearFCMask(((PER_ID_UART0 + id) << 2) + 2);
    UART_PTR->IRQ_EN  = 0;

    /* Clean RX FIFO */
    UART_PTR->SETUP   |= UART_SETUP_RX_CLEAN_FIFO(1);
    UART_PTR->SETUP   = 0;

    /* Set event / IRQ */
    if (type == UDMA_EVENT) {
        /* Attach event unit for end interrupt */
        SOC_EU_SetFCMask((PER_ID_UART0 + id) << 2);
        SOC_EU_SetFCMask(((PER_ID_UART0 + id) << 2) + 1);

    } else if (type == IRQ) {
        SOC_EU_SetFCMask(((PER_ID_UART0 + id) << 2) + 1);
        SOC_EU_SetFCMask(((PER_ID_UART0 + id) << 2) + 2);
        UART_PTR->IRQ_EN = UART_IRQ_EN_RX(1);

    } else if (type == POLLING) {
        SOC_EU_SetFCMask(((PER_ID_UART0 + id) << 2) + 1);
        UART_PTR->SETUP |= UART_SETUP_RX_POLLNG_EN(1);
    }

    /* 8N1*/
    UART_PTR->SETUP |= UART_SETUP_BIT_LENGTH(uUART_8bits) |
        UART_SETUP_STOP_BITS(0) |
        UART_SETUP_PARITY_ENA(0) |
        UART_SETUP_TX_ENA(1) |
        UART_SETUP_RX_ENA(1);

    uart_set_baud (baud);

    /* UART write and read */
    char tx_buffer = 0;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer = i;
        uart_write((uint32_t)&tx_buffer, 1);
    }

    uart_read(rx_buffer, BUFFER_SIZE, type);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (rx_buffer[i] != (uint8_t) i)
            error++;
    }

    /* Clock gating */
    UDMA_GC->CG &= ~(1 << (PER_ID_UART0 + id));

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os Serial driver test\n");

    int error = 0;

    error = test_uart(0, 115200, IRQ);
    error = test_uart(0, 115200, UDMA_EVENT);
    error = test_uart(0, 115200, POLLING);

    error = test_uart(1, 115200, IRQ);
    error = test_uart(1, 115200, UDMA_EVENT);
    error = test_uart(1, 115200, POLLING);

    if (error)
        printf("Test failed %d errors\n", error);
    else
        printf("Test success\n");

    return error;
}
