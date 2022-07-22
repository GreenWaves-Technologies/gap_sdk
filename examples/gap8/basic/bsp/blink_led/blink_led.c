/**
 * This example blinks a user led on Gapuino boards(Gapuino_V2),
 * Gapoc_a and Gapoc_b boards.
 */

/* PMSIS includes */
#include "pmsis.h"
#include "bsp/bsp.h"

void blink_led(void)
{
    printf("Entering main controller\n");

    printf("Blinking USER LED\n");
    pi_gpio_pin_configure(NULL, GPIO_USER_LED, PI_GPIO_OUTPUT);
    while (1)
    {
        pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
        pi_time_wait_us(100000);
        pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
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
