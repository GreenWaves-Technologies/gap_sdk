/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */

#if defined(GPIO_INPUT)
volatile uint8_t done = 0;
struct pi_device gpio_in;

void __pi_cb_gpio(void *arg)
{
    done++;
    pi_gpio_e gpio_pin = (pi_gpio_e) arg;
    uint32_t value = 0;
    int32_t errors = 0;
    errors = pi_gpio_pin_read(&gpio_in, gpio_pin, &value);
    if (errors)
    {
        printf("GPIO read error %ld\n", errors);
        pmsis_exit(-7);
    }
    printf("GPIO callback GPIO_A%d %d, done=%d\n", (gpio_pin & PI_GPIO_NUM_MASK), value, done);
}
#endif  /* GPIO_INPUT */

void test_gpio(void)
{
    int32_t errors = 0;
    uint32_t val_in = 0, val_out = 0;
    uint32_t wait_us = 1000000;

    struct pi_gpio_conf gpio_conf = {0};

    #if defined(GPIO_OUTPUT)
    struct pi_device gpio_out;

    pi_gpio_e gpio_pin_o = PI_GPIO_A49; /* PI_GPIO_A02-PI_GPIO_A05 */

    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&gpio_out, &gpio_conf);
    gpio_conf.port = (gpio_pin_o & PI_GPIO_NUM_MASK) / 32;
    errors = pi_gpio_open(&gpio_out);
    if (errors)
    {
        printf("Error opening GPIO %d\n", errors);
        pmsis_exit(errors);
    }

    pi_gpio_flags_e cfg_flags_out = PI_GPIO_OUTPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;

    /* Configure gpio output. */
    pi_gpio_pin_configure(&gpio_out, gpio_pin_o, cfg_flags_out);
    printf("GPIO_A%d configured as output.\n", (gpio_pin_o & PI_GPIO_NUM_MASK));
    #endif  /* GPIO_OUTPUT */

    #if defined(GPIO_INPUT)
    pi_gpio_e gpio_pin_i = PI_GPIO_A48;

    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&gpio_in, &gpio_conf);
    gpio_conf.port = (gpio_pin_i & PI_GPIO_NUM_MASK) / 32;
    errors = pi_gpio_open(&gpio_in);
    if (errors)
    {
        printf("Error opening GPIO %d\n", errors);
        pmsis_exit(errors);
    }

    pi_task_t cb_gpio = {0};
    pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_RISE;
    pi_gpio_flags_e cfg_flags_in = PI_GPIO_INPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;

    /* Callback task. */
    pi_task_callback(&cb_gpio, __pi_cb_gpio, (void *) gpio_pin_i);

    /* Configure gpio input. */
    pi_gpio_pin_configure(&gpio_in, gpio_pin_i, cfg_flags_in);
    printf("GPIO_A%d configured as input.\n", (gpio_pin_i & PI_GPIO_NUM_MASK));
    pi_gpio_pin_task_add(&gpio_in, gpio_pin_i, &cb_gpio, irq_type);
    //pi_gpio_pin_notif_configure(&gpio_in, gpio_pin_i, irq_type);
    #endif  /* GPIO_INPUT */

    uint32_t index = 10;
    while (index > 0)
    {
        #if defined(GPIO_OUTPUT)
        val_out = 0;
        errors = pi_gpio_pin_write(&gpio_out, gpio_pin_o, val_out);
        if (errors)
        {
            printf("GPIO write error %ld\n", errors);
            pmsis_exit(-2);
        }
        printf("GPIO_A%d out: %lx\n", (gpio_pin_o & PI_GPIO_NUM_MASK), val_out);
        #endif

        #if defined(GPIO_INPUT)
        /* Read input values. */
        errors = pi_gpio_pin_read(&gpio_in, gpio_pin_i, &val_in);
        if (errors)
        {
            printf("GPIO read error %ld\n", errors);
            pmsis_exit(-3);
        }
        printf("GPIO_A%d in: %ld\n", (gpio_pin_i & PI_GPIO_NUM_MASK), val_in);
        #endif  /* GPIO_INPUT */

        pi_time_wait_us(wait_us);

        #if defined(GPIO_OUTPUT)
        val_out = 1;
        errors = pi_gpio_pin_write(&gpio_out, gpio_pin_o, val_out);
        if (errors)
        {
            printf("GPIO write error %ld\n", errors);
            pmsis_exit(-4);
        }
        printf("GPIO_A%d out: %lx\n", (gpio_pin_o & PI_GPIO_NUM_MASK), val_out);
        #endif  /* GPIO_OUTPUT */

        #if defined(GPIO_INPUT)
        errors = pi_gpio_pin_read(&gpio_in, gpio_pin_i, &val_in);
        if (errors)
        {
            printf("GPIO read error %ld\n", errors);
            pmsis_exit(-5);
        }
        printf("GPIO_A%d in: %ld\n", (gpio_pin_i & PI_GPIO_NUM_MASK), val_in);
        #endif  /* GPIO_INPUT */

        pi_time_wait_us(wait_us);
        index--;

        #if defined(GPIO_OUTPUT) && defined(GPIO_INPUT)
        if (val_out != val_in)
        {
            printf("GPIO %d in=%ld != GPIO %d out=%ld\n",
                   (gpio_pin_i & PI_GPIO_NUM_MASK), val_in,
                   (gpio_pin_o & PI_GPIO_NUM_MASK), val_out);
            errors = -6;
            break;
        }
        #endif  /* GPIO_OUTPUT && GPIO_INPUT */
    }
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    return pmsis_kickoff((void *) test_gpio);
}

