/*
 * This example shows how to periodically call a function callback
 * using a virtual timer.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define NB_TICKS 20

static int nb_ticks = 0;

static void timer_handle(void *arg)
{
  printf("[%d] Entered user handler\n", rt_time_get_us());
  nb_ticks++;
}

int main()
{
  rt_timer_t timer;

  // Create a periodic timer
  if (rt_timer_create(&timer, RT_TIMER_PERIODIC, rt_event_get(NULL, timer_handle, (void *)&timer)))
    return -1;

  // Start it. This will call the function we provided everytime the
  // specified period is reached.
  rt_timer_start(&timer, 500000);

  // Wait until we reach a certain number of calls
  while(nb_ticks < NB_TICKS)
  {
    rt_event_yield(NULL);
  }

  // Properly close down the timer to not receive anymore the calls
  rt_timer_stop(&timer);
  rt_timer_destroy(&timer);
  printf ("Test successful \n");

  return 0;
}
