/*
 * This example shows how to use a virtual timer to trigger a function call
 * once.
 */

#include <stdio.h>
#include <rt/rt_api.h>
#include <stdint.h>

#define NB_TICKS 20

static int nb_ticks = 0;

static void timer_handle(void *arg)
{
  rt_timer_t *timer = (rt_timer_t *)arg;
  printf("[%d] Entered user handler\n", rt_time_get_us());
  nb_ticks++;
  if (nb_ticks < NB_TICKS)
    rt_timer_start(timer, 1000);
}

int main()
{
  rt_timer_t timer;

  // Create a periodic timer
  if (rt_timer_create(&timer, RT_TIMER_ONE_SHOT, rt_event_get(NULL, timer_handle, (void *)&timer)))
    return -1;

  // Start it. This will call the function we provided once the
  // specified period is reached. The callback will then re-arm
  // the timer.
  rt_timer_start(&timer, 1000000);

  // Wait until we reach a certain number of calls
  while(nb_ticks < NB_TICKS)
  {
    rt_event_yield(NULL);
  }

  // Properly close down the timer to not receive anymore the calls
  rt_timer_stop(&timer);
  rt_timer_destroy(&timer);
  printf("Test success\n");

  return 0;
}
