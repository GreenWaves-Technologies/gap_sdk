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
  // GPIO initialization
  rt_gpio_init(0, GPIO);

  // Configure GPIO as an inpout
  rt_gpio_set_dir(0, 1<<GPIO, RT_GPIO_IS_IN);

  // Now get input values
  for (int i=0; i<10; i++)
  {
    printf("Got GPIO value: %d\n", 0, rt_gpio_get_pin_value(0, GPIO));
    rt_time_wait_us(500000);
  }

  return 0;
}
