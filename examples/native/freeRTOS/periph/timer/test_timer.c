/* PMSIS includes */
#include "pmsis.h"
#include "pmsis/implem/drivers/pmsis_it.h"

#define NB_ITER          ( 5 )

static volatile uint32_t done = 0;

static void timer_handler()
{
    done++;
    printf("Timer IRQ: %d, val=%d\n", done, pi_timer_value_read(FC_TIMER_1));
}

HANDLER_WRAPPER_LIGHT(timer_handler);

void test_timer(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    uint32_t time_us = 200000;
    NVIC_SetVector(FC_IRQ_TIMER0_HI_EVT, (uint32_t) __handler_wrapper_light_timer_handler);
    NVIC_EnableIRQ(FC_IRQ_TIMER0_HI_EVT);
    printf("Timer IRQ set every %dus.\n", time_us);
    pi_timer_irq_set(FC_TIMER_1, time_us, 0);

    while (done != (uint32_t) NB_ITER)
    {
        pi_yield();
    }
    pi_timer_stop(FC_TIMER_1);

    errors = (done != (uint32_t) NB_ITER);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Timer ***\n\n");
    return pmsis_kickoff((void *) test_timer);
}
