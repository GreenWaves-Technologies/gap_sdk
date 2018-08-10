/*
 * This example shows how to block the core until a GPIO
 * value has changed.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#if PULP_CHIP == CHIP_GAP
#define GPIO 3
#else
#define GPIO 20
#endif

#define NB_EDGE 10

int main()
{
  // GPIO initialization
  rt_gpio_init(0, GPIO);

  // Configure GPIO as an inpout
  rt_gpio_set_dir(0, 1<<GPIO, RT_GPIO_IS_IN);

  // Trigger notifications on both rising and falling edges
  rt_gpio_set_sensitivity(0, GPIO, RT_GPIO_SENSITIVITY_EDGE);

  // Now wait for a few edges and see how long it takes
  unsigned long long start = rt_time_get_us();

  for (int i=0; i<NB_EDGE; i++)
  {
    rt_gpio_wait(0, GPIO);
  }

  unsigned long long end = rt_time_get_us();

  printf("Got %d edges in %d us\n", NB_EDGE, end - start);

  return 0;
}
