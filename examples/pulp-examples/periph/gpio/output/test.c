/*
 * This example shows how drive a GPIO as an output.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define GPIO 18

int main()
{
  // GPIO initialization
  rt_gpio_init(0, GPIO);

  // Configure GPIO as an outpout
  rt_gpio_set_dir(0, 1<<GPIO, RT_GPIO_IS_OUT);

  // Now set several values
  rt_gpio_set_pin_value(0, GPIO, 1);

  rt_time_wait_us(200);

  rt_gpio_set_pin_value(0, GPIO, 0);

  rt_time_wait_us(200);

  rt_gpio_set_pin_value(0, GPIO, 1);

  return 0;
}
