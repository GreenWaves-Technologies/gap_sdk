/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"

#define NB_EVENTS 3

static inline void get_rtc_info(int *div, uint32_t *count, unsigned int *time)
{
#if defined(__PLATFORM_RTL__)
#if !defined(RTC_VERSION) || RTC_VERSION == 1
    *div = 0x10;
    *count = 0x5;
    *time = 0x6*0x10*10000/32768*100; // In periodic mode dolphin RTC is counting specified count + 1
#else
    *div = 0;
    *count = 0x400;
    *time = 0x400*10000/32768*100;
#endif
#else
    *div = 0;
#if !defined(RTC_VERSION) || RTC_VERSION == 1
    *div = 0x8000;
    *count = 5;
    *time = 6000000; // In periodic mode dolphin RTC is counting specified count + 1
#else
    *count = 2*32768;
    *time = 2000000;
#endif
#endif
}


static struct pi_device rtc;
int count = NB_EVENTS;

static void times_up(void *arg)
{
  printf ("Count Down %d Finished!\n", count);
  count--;
}

int test_entry()
{
    printf("RTC test start\n");

    int time;
    struct pi_rtc_conf conf;
    uint32_t countdown;
    pi_task_t task;

    pi_rtc_conf_init(&conf);

    get_rtc_info(&conf.clk_div, &countdown, &time);
    conf.mode = PI_RTC_MODE_TIMER;
    conf.counter = countdown;
    pi_open_from_conf(&rtc, &conf);

    if (pi_rtc_open(&rtc))
        return -1;

    pi_rtc_ioctl(&rtc, PI_RTC_TIMER_ATTACH_TASK, (void *)pi_task_callback(&task, times_up, NULL));
    pi_rtc_timer_set(&rtc, countdown);
    pi_rtc_ioctl(&rtc, PI_RTC_TIMER_START, (void *)1);

    unsigned long long start = pi_time_get_us();

    while (count)
        pi_yield();

    unsigned long long end = pi_time_get_us();

    int diff = (unsigned long long)(time*NB_EVENTS) - end + start;
    if (diff < 0)
      diff = -diff;
    int error = diff * 100 / (time*NB_EVENTS);

    pi_rtc_ioctl(&rtc, PI_RTC_TIMER_STOP, NULL);

    pi_rtc_close(&rtc);

    printf("End of test (estimated time: %d, measured time: %lld, diff: %d / %d %%)\n", time*NB_EVENTS, end-start, diff, error);

    if (error > 10)
      return -1;

    return 0;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
