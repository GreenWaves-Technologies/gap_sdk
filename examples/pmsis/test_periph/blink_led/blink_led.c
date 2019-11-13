/* PMSIS includes */
#include "pmsis.h"

void blink_led(void)
{
    printf("Entering main controller\n");

    printf("Blinking LED3 on GAPOC_A\n");
    pi_gpio_pin_configure(NULL, PI_GPIO_A0_PAD_12_A3, PI_GPIO_OUTPUT);
    while (1)
    {
        pi_gpio_pin_write(NULL, 0, 0);
        pi_time_wait_us(100000);
        pi_gpio_pin_write(NULL, 0, 1);
        pi_time_wait_us(100000);
    }

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Blink LED ***\n\n");
    return pmsis_kickoff((void *) blink_led);
}
