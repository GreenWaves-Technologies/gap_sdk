#include <stdio.h>
#include "cmsis.h"
#include "gap_common.h"

int main()
{
    printf("Fabric controller code execution for mbed_os DBG\n");

    uint32_t reg = 0;

    DBG->IE = (1 << 3);
    asm volatile ("ebreak");

    while (reg != 0xa5) {
        reg = (SOC_CTRL->JTAG & 0xF00) >> 8;
    }

    SOC_CTRL->JTAG = 0x5a;

    while (reg != 0x5a) {
        reg = (SOC_CTRL->JTAG & 0xF00) >> 8;
    }

    SOC_CTRL->JTAG = 0xa5;

    while (reg != 0x00) {
        reg = (SOC_CTRL->JTAG & 0xF00) >> 8;
    }

    int error = 0;
    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return 0;
}
