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
    printf("First GPIO callback GPIO_A%d %d %d\n", gpio_pin & 0xFF, done, val);
}

void __pi_cb_gpio_2(void *arg)
{
    done++;
    pi_gpio_e gpio_pin = (pi_gpio_e) arg;
    uint32_t val = 0;
    pi_gpio_pin_read(&gpio, gpio_pin, &val);
    printf("Second GPIO callback GPIO_A%d %d %d\n", gpio_pin & 0xFF, done, val);
}

void __pi_cb_gpio_3(void *arg)
{
    done++;
    pi_gpio_e gpio_pin = (pi_gpio_e) arg;
    uint32_t val = 0;
    pi_gpio_pin_read(&gpio, gpio_pin, &val);
    printf("Third GPIO callback GPIO_A%d %d %d\n", gpio_pin & 0xFF, done, val);
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

    pi_gpio_e gpio_in = PI_GPIO_A0_PAD_12_A3;
    pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_RISE;
    pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;

    /* Configure gpio input. */
    pi_gpio_pin_configure(&gpio, gpio_in, cfg_flags);
    value = pi_gpio_pin_read(&gpio, gpio_in, &value);
    printf("GPIO opened, in val: %d\n", value);
    pi_gpio_pin_notif_configure(&gpio, gpio_in, irq_type);

    /* Initialize cb. */
    uint32_t gpio_mask = (1 << (gpio_in & PI_GPIO_NUM_MASK));
    pi_gpio_callback_t cb_gpio[3] = {0};
    pi_gpio_callback_init(&cb_gpio[0], gpio_mask, __pi_cb_gpio, (void *) gpio_in);
    pi_gpio_callback_init(&cb_gpio[1], gpio_mask, __pi_cb_gpio_2, (void *) gpio_in);
    pi_gpio_callback_init(&cb_gpio[2], gpio_mask, __pi_cb_gpio_3, (void *) gpio_in);

    /* Attach callback. */
    pi_gpio_callback_add(&gpio, &cb_gpio[0]);
    pi_gpio_callback_add(&gpio, &cb_gpio[1]);
    pi_gpio_callback_add(&gpio, &cb_gpio[2]);

    uint8_t exec = 0;
    while (1)
    {
        pi_gpio_pin_read(&gpio, gpio_in, &value);
        printf("GPIO %d in: %d\n", gpio_in & 0xFF, value);
        pi_time_wait_us(1000000);
        pi_gpio_pin_read(&gpio, gpio_in, &value);
        printf("GPIO %d in: %d\n", gpio_in & 0xFF, value);
        pi_time_wait_us(1000000);
        if (done && (exec == 0))
        {
            exec = 1;
            pi_gpio_callback_remove(&gpio, &cb_gpio[1]);
        }
        if (exec == 1)
        {
            exec = 2;
            pi_gpio_callback_remove(&gpio, &cb_gpio[2]);
        }
        if (exec == 2)
        {
            exec = 3;
            pi_gpio_callback_remove(&gpio, &cb_gpio[0]);
        }
    }
    pi_gpio_close(&gpio);
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS GPIO Input ***\n\n");
    return pmsis_kickoff((void *) test_gpio);
}

