#include <stdlib.h>
// MBED OS
#include "rtx_lib.h"
// GAP Driver API
#include "gap_common.h"

volatile int count = 0;
#define IRQ_NUM  6

void irq_handler() {
    count++;
    printf("count = %d\n", count);
}

/* handler wrapper  */
Handler_Wrapper(irq_handler);

int main()
{
    printf("Fabric controller code execution for mbed_os SW IRQ test\n");

    // Bind handler wrapper to SW event
    NVIC_SetVector(IRQ_NUM, (uint32_t)__handler_wrapper_irq_handler);
    NVIC_EnableIRQ(IRQ_NUM);

    // Trigger IRQ
#if defined(__GAP8__)
    EU_EVT_DemuxTrigSet(IRQ_NUM, 1);
#elif defined(__GAP9__)
    ITC_SetIRQ(IRQ_NUM);
#endif

    while(count==0);

    if(count)
    {
        printf("Test success\n");
        #ifdef JENKINS_TEST_FLAG
        exit(0);
        #else
        return 0;
        #endif
    }
    else
    {
        printf("Test failed\n");
        #ifdef JENKINS_TEST_FLAG
        exit(-1);
        #else
        return -1;
        #endif
    }
    return 0;
}
