/**
 * GPIO input test using push button on Gapuino.
 * Before using check your board configuration.
 */

/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
struct pi_device gpio;

volatile uint8_t done = 0;
void __pi_cb_gpio(void *arg)
{
    done++;
    pi_gpio_e gpio_pin = (pi_gpio_e) arg;
    uint32_t val = 0;
    pi_gpio_pin_read(&gpio, gpio_pin, &val);
    printf("GPIO callback GPIO_A%d %d %d\n", gpio_pin & 0xFF, done, val);
}

void test_gpio(void)
{
    int32_t errors = 0;
    uint32_t value = 0;
    struct pi_gpio_conf gpio_conf = {0};
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&gpio, &gpio_conf);
    errors = pi_gpio_open(&gpio);
    if (errors)
    {
        printf("Error opening GPIO %d\n", errors);
        pmsis_exit(errors);
    }
    pi_task_t cb_gpio = {0};

    pi_gpio_e gpio_in = PI_GPIO_A0_PAD_12_A3;
    pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_RISE;
    pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;

    pi_task_callback(&cb_gpio, __pi_cb_gpio, (void *) gpio_in);
    /* Configure gpio input. */
    pi_gpio_pin_configure(&gpio, gpio_in, cfg_flags);
    value = pi_gpio_pin_read(&gpio, gpio_in, &value);
    printf("GPIO opened, in val: %d\n", value);
    pi_gpio_pin_task_add(&gpio, gpio_in, &cb_gpio, irq_type);
    pi_gpio_pin_notif_configure(&gpio, gpio_in, irq_type);

    while (1)
    {
        pi_gpio_pin_read(&gpio, gpio_in, &value);
        printf("GPIO %d in: %d\n", gpio_in & 0xFF, value);
        pi_time_wait_us(1000000);
        pi_gpio_pin_read(&gpio, gpio_in, &value);
        printf("GPIO %d in: %d\n", gpio_in & 0xFF, value);
        pi_time_wait_us(1000000);
    }
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS GPIO Input ***\n\n");
    return pmsis_kickoff((void *) test_gpio);
}

