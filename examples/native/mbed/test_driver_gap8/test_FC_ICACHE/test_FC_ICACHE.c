#include <stdio.h>
#include "cmsis.h"
#include "gap_common.h"

int main()
{
    printf("Fabric controller code execution for mbed_os FC ICACHE \n");

    for (int i = 0; i < 100; i++) {
        SCBC->ICACHE_ENABLE = 0xF;
        SCBC->ICACHE_ENABLE = 0x0;
    }

    printf("Test success\n");

    return 0;
}
