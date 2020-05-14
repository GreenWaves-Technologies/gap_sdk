#include "pmsis.h"
#include "bsp/bsp.h"


void helloworld(void)
{
    printf("Entering app, update ID: %d\n");

    pi_gpio_pin_configure(NULL, GPIO_USER_LED, PI_GPIO_OUTPUT);

    while (1)
    {
        pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
        pi_time_wait_us(1000000 / BLINK_SPEED);
        pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
        pi_time_wait_us(1000000 / BLINK_SPEED);
    }

    pmsis_exit(0);
}

int main(void)
{
    return pmsis_kickoff((void *) helloworld);
}

