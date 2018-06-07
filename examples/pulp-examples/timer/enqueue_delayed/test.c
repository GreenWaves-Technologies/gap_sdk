/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

/*
 * This example shows how to execute a callback after a specific delay.
 * This is using events. Once the delay has passed, the event is pushed
 * to the scheduler and executed when it is scheduled.
 */

#include <stdio.h>
#include "rt/rt_api.h"

#define WAIT_TIME 1200

static unsigned long long exec_time = 0;


// This callback is an event callback. It is not called from an interrupt
// handler, but is called from our main function (through the call to
// rt_event_execute), so it is safe to call anything from there.
static void timer_callback()
{
    exec_time = rt_time_get_us();
    printf("Got event at time %lld\n", exec_time);
}

int main()
{

    unsigned long long enqueue_time;
    int err = 0;

    // The scheduler does not contain any event after booting, we need to allocate
    // the one we will use,
    if (rt_event_alloc(NULL, 1)) return -1;

    enqueue_time = rt_time_get_us();

    // This will register our event and push it to the runtime scheduler as soon
    // as the specified amount of time has passed.
    // The specified amount of time is experessed in microseconds and is a minimal
    // amount, the actual amount may be bigger due to timer resolution.
    rt_event_push_delayed(rt_event_get(NULL, timer_callback, NULL), WAIT_TIME);

    printf("Enqueued event at time %lld\n", enqueue_time);

    // Now wait until the event is executed.
    // we have to explicitetly call the scheduler so that our event is executed
    // as the scheduler is not executing in background.
    while(exec_time == 0)
    {
        rt_event_execute(NULL, 1);
    }

    if ((exec_time - enqueue_time) < WAIT_TIME)
        err++;
    else
        printf("Test success\n");

    return err;

}
