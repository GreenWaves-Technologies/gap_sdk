// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"
#include "mbed_wait_api.h"


#define BUFFER_SIZE   16
uint8_t tx_buffer[BUFFER_SIZE];


void csi2_init(void)
{
    /* Power Up */
    PMU_CSI2PowerOn();

    /* Disable clock gating */
    UDMA_GC->CG |= 1 << PER_ID_CSI2;

    /* set event mask */
    SOC_EU_SetFCMask((PER_ID_CSI2 << 2));
    SOC_EU_SetFCMask((PER_ID_CSI2 << 2) + 1);
    SOC_EU_SetFCMask((PER_ID_CSI2 << 2) + 2);
    SOC_EU_SetFCMask((PER_ID_CSI2 << 2) + 3);

    /* 2 lane raw 8 : pixel_clk = 42.67/2 ns (46.87 Mhz), apb_clk = 10 Mhz, cci_clk = 400 / 2 Khz */
    CSI2->CLK_DIV = CSI2_CLK_DIV_PIXEL(0) | CSI2_CLK_DIV_APB(5) | CSI2_CLK_DIV_CCI(117);
}

void cci_read(uint8_t *buffer, char size)
{
    uint8_t reg = 0;

    CSI2_PIXEL->CCI_DEV_ADDR   = 0x3C;
    CSI2_PIXEL->CCI_ADDRESS    = 0x05;
    CSI2_PIXEL->CCI_READ_WRITE = (0 << 7) | (size & 0x7F);

    do {
        wait_us(100);
    } while (CSI2_PIXEL->CCI_STATUS != 0x03);

    for (int i = 0; i < size; i++)
        buffer[i] = CSI2_PIXEL->CCI_READ_DATA;

    while (CSI2_PIXEL->CCI_STATUS) {
        CSI2_PIXEL->CCI_STATUS = 0x01;
    }
}

void cci_write(uint8_t *buffer, char size)
{
    CSI2_PIXEL->CCI_DEV_ADDR   = 0x3C;
    CSI2_PIXEL->CCI_ADDRESS    = 0x05;

    for (int i = 0; i < size; i++)
        CSI2_PIXEL->CCI_WRITE_DATA = buffer[i];

    CSI2_PIXEL->CCI_READ_WRITE = (1 << 7) | (size & 0x7F);

    do {
        wait_us(100);
    } while (CSI2_PIXEL->CCI_STATUS != 0x01);

    do {
        CSI2_PIXEL->CCI_STATUS = 0x01;
    } while (CSI2_PIXEL->CCI_STATUS);
}

int main()
{
    printf("Fabric controller code execution for mbed_os CSI2 driver test\n");

    int error = 0;

    /* Set 24 - 25 pin to alt 2 - CSI2_CCI */
    /* volatile uint32_t val = PORTA->PADFUN[1] & ~(0xF0000); */
    /* PORTA->PADFUN[1]      = val | (uint32_t)(0xA0000); */

    /* Initialize the buffer */
    for (int i = 0; i < BUFFER_SIZE; i++)
        tx_buffer[i] = i;

    FLL_SetFrequency(uFLL_SOC, 46.87*2*1000000, 0);

    csi2_init();

    cci_write(tx_buffer, 3);

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
