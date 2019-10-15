#include <stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

volatile int count = 0;
#define IRQ_NUM 6

void irq_handler() {
    count++;
    printf("count = %d\n", count);
}

/* handler wrapper  */
Handler_Wrapper(irq_handler);

int main()
{
    printf("Fabric controller code execution for fc bug test\n");

    asm volatile (
        "1:\n\t"
        "la     a0, irq_handler     \n\t"
        "add    a5, a0, x0          \n\t"
        "sw     a0, -8(a5)          \n\t"
        "lui    a3, 4000            \n\t"
        "lw     a5, -8(a5)         \n\t"
        "beq    s2, x0,  2f         \n\t"
        "jalr   a5                  \n\t"
        "add    a1, x0,  x0         \n"
        "2:\n\t"
        "jalr   a5                  \n\t"
        "add    a2, x0,  x0         \n\t"
        "3:"
        );
    return 0;
}
