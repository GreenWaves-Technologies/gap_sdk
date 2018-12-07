/*
 * This example shows how get values from a GPIO
 * configured as an input.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define GPIO 2

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

    // Configure GPIO as an inpout
    rt_gpio_set_dir(0, 1<<GPIO, RT_GPIO_IS_IN);

    // Now get input values
    for (int i=0; i<10; i++)
    {
        printf("Got GPIO value: %d\n", rt_gpio_get_pin_value(0, GPIO));
        rt_time_wait_us(500000);
    }

    return 0;
}
