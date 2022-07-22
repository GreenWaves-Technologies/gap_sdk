/**
 * GPIO input test using push button on Gapuino.
 * Before using check your board configuration.
 */

/* PMSIS includes */
#include "pmsis.h"

#define PIN_GPIO1   PI_PAD_073  // Pad CAM DATA0
#define PIN_GPIO2   PI_PAD_074  // Pad CAM DATA1

#define GPIO1       73
#define GPIO2       74 

#define GPIO_ALT    (1)

/* Variables used. */
struct pi_device gpio1;
struct pi_device gpio2;

volatile uint8_t done = 0;
void __pi_cb_gpio_1(void *arg)
{
    done++;
    uint32_t val = 0;
    pi_gpio_pin_read(&gpio1, GPIO1, &val);
    printf("GPIO callback GPIO1 %d %d\n", done, val);
}

void __pi_cb_gpio_2(void *arg)
{
    done++;
    uint32_t val = 0;
    pi_gpio_pin_read(&gpio2, GPIO2, &val);
    printf("GPIO callback GPIO2 %d %d\n", done, val);
}


void test_gpio(void)
{
    int32_t errors = 0;
    uint32_t value[2] = {0, 0};
    pi_task_t task[2];

    // Open GPIO1
    struct pi_gpio_conf gpio1_conf = {0};
    pi_gpio_conf_init(&gpio1_conf);
    gpio1_conf.port = GPIO1 / 32;
    pi_open_from_conf(&gpio1, &gpio1_conf);
    errors = pi_gpio_open(&gpio1);
    if (errors)
    {
        printf("Error opening GPIO %d\n", errors);
        pmsis_exit(errors);
    }
    printf("GPIO1 opened\n");

    //Open GPIO2
    struct pi_gpio_conf gpio2_conf = {0};
    pi_gpio_conf_init(&gpio2_conf);
    gpio2_conf.port = GPIO2 / 32;
    pi_open_from_conf(&gpio2, &gpio2_conf);
    errors = pi_gpio_open(&gpio2);
    if (errors)
    {
        printf("Error opening GPIO %d\n", errors);
        pmsis_exit(errors);
    }
    printf("GPIO2 opened\n");

    if (pi_gpio_pin_configure(&gpio1, GPIO1, PI_GPIO_INPUT))
    {
        pmsis_exit(-5);
    }

    if (pi_gpio_pin_configure(&gpio2, GPIO2, PI_GPIO_INPUT))
    {
        pmsis_exit(-5);
    }

    pi_pad_set_function(GPIO1,      GPIO_ALT);
    pi_pad_set_function(GPIO2,      GPIO_ALT);

    pi_gpio_pin_notif_configure(&gpio1, GPIO1, PI_GPIO_NOTIF_RISE);
    pi_gpio_pin_notif_configure(&gpio2, GPIO2, PI_GPIO_NOTIF_RISE);

    pi_gpio_pin_notif_clear(&gpio1, GPIO1);
    pi_gpio_pin_notif_clear(&gpio2, GPIO2);

    pi_task_callback(&task[0], __pi_cb_gpio_1, NULL);
    pi_task_callback(&task[1], __pi_cb_gpio_2, NULL);

    if (pi_gpio_pin_task_add(&gpio1, GPIO1, &task[0], PI_GPIO_NOTIF_RISE))
        pmsis_exit(-6);

    if (pi_gpio_pin_task_add(&gpio2, GPIO2, &task[1], PI_GPIO_NOTIF_RISE))
        pmsis_exit(-6);

    while(done < 10)
    {
        pi_gpio_pin_read(&gpio1, GPIO1, &value[0]);
        pi_gpio_pin_read(&gpio2, GPIO2, &value[1]);
        pi_time_wait_us(1000000);

        printf("GPIO1 = %d, GPIO2 = %d\n", value[0], value[1]);
    }

    //pi_gpio_close(&gpio1);
    //pi_gpio_close(&gpio2);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS GPIO Input ***\n\n");
    return pmsis_kickoff((void *) test_gpio);
}

