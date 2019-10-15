/*
 * This example shows how drive a GPIO as an output.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define GPIO 3

int main()
{

    rt_padframe_profile_t *profile_gpio = rt_pad_profile_get("hyper_gpio");

    if (profile_gpio == NULL) {
        printf("pad config error\n");
        return 1;
    }
    rt_padframe_set(profile_gpio);

    // GPIO initialization
    rt_gpio_init(0, GPIO);

    // Configure GPIO as an outpout
    rt_gpio_set_dir(0, 1<<GPIO, RT_GPIO_IS_OUT);

    // Now set several values
    while (1){
    rt_gpio_set_pin_value(0, GPIO, 1);

    rt_time_wait_us(500000);

    rt_gpio_set_pin_value(0, GPIO, 0);

    rt_time_wait_us(500000);

    }
    rt_gpio_set_pin_value(0, GPIO, 1);

    return 0;
}

