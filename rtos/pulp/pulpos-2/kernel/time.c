/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"

static PI_FC_L1 uint32_t pos_time_timer_count;
PI_FC_L1 pi_task_t *pos_time_first_delayed;
static PI_FC_L1 pos_cbsys_t pos_time_cbsys_poweroff;
static PI_FC_L1 pos_cbsys_t pos_time_cbsys_poweron;


#ifndef CONFIG_REF_CLOCK
#define CONFIG_REF_CLOCK ARCHI_REF_CLOCK
#endif

#ifndef CONFIG_NO_FAST_OSC
#define TIMER_CLOCK CONFIG_REF_CLOCK
#define TIMER_SOURCE 1
#else
// In case no oscillator is available, this means we are in open loop, in this case, just take
// FLL worst case frequency
#define TIMER_CLOCK ARCHI_FLL_OPEN_LOOP_CLOCK
#define TIMER_SOURCE 0
#endif

void pos_time_timer_handler_asm();


static int pos_time_poweroff(void *arg)
{
    // Remember the current timer count so that we can restore it
    // when the system is powered-on
    pos_time_timer_count = timer_count_get(timer_base_fc(0, 1));

    return 0;
}

static int pos_time_poweron(void *arg)
{
    // Restore the timer count we saved before shutdown
    timer_count_set(timer_base_fc(0, 1), pos_time_timer_count);

    return 0;
}


void pos_time_timer_handler()
{
    pi_task_t *event = pos_time_first_delayed;

    uint32_t current_time = timer_count_get(timer_base_fc(0, 1));

    // First dequeue and push to their scheduler all events with the same number of
    // ticks as they were waiting for the same time.
    while (event && (current_time - event->implem.time) < 0x7fffffff)
    {
        pi_task_t *next = event->next;
        pos_task_push_locked(event);
        event = next;
    }

    // Update the wait list with the next waiting event which has a different number
    // of ticks
    pos_time_first_delayed = event;

    // Now re-arm the timer in case there are still some events
    if (pos_time_first_delayed)
    {
        // Be carefull to set the new comparator from the current time plus a number of ticks
        // in order to set a value which is not before the actual count.
        // This may just delay a bit the events which is fine as the specified
        // duration is a minimum.
        timer_cmp_set(timer_base_fc(0, 1),
                      timer_count_get(timer_base_fc(0, 1)) +
                          pos_time_first_delayed->implem.time - current_time);

        timer_conf_set(timer_base_fc(0, 1),
                       TIMER_CFG_LO_ENABLE(1) |
                           TIMER_CFG_LO_IRQEN(1) |
                           TIMER_CFG_LO_CCFG(TIMER_SOURCE));
    }
    else
    {
        // Set back default state where timer is only counting with
        // no interrupt
        timer_conf_set(timer_base_fc(0, 1),
                       TIMER_CFG_LO_ENABLE(1) |
                           TIMER_CFG_LO_CCFG(TIMER_SOURCE));

        // Also clear timer interrupt as we might have a spurious one after
        // we entered the handler
#ifdef ARCHI_HAS_FC
        pos_irq_clr(1 << ARCHI_FC_EVT_TIMER0_HI);
#else
        pos_irq_clr(1 << ARCHI_EVT_TIMER0_HI);
#endif
    }
}


unsigned long long pi_time_get_us()
{
    // Get 64 bit timer counter value and convert it to microseconds
    // as the timer input is connected to the ref clock.
    unsigned int count = timer_count_get(timer_base_fc(0, 1));
    return ((unsigned long long)count) * 1000000 / TIMER_CLOCK;
}

void pi_task_push_delayed_us(pi_task_t *event, uint32_t us)
{
    int irq = hal_irq_disable();

    int set_irq = 0;
    pi_task_t *current = pos_time_first_delayed, *prev = NULL;
    unsigned int ticks, ticks_from_now;
    uint32_t current_time = timer_count_get(timer_base_fc(0, 1));

        // First compute the corresponding number of ticks.
        // The specified time is the minimum we must, so we have to round-up
        // the number of ticks.
    ticks = ((uint64_t)us * TIMER_CLOCK + 999999) / 1000000;

    // In order to simplify time comparison, we sacrify the MSB to avoid overflow
    // as the given amount of time must be short
    uint32_t time = (current_time & 0x7fffffff) + ticks;
    event->implem.time = current_time + ticks;

    // Enqueue the event in the wait list.
    while (current && (current->implem.time & 0x7fffffff) < time)
    {
        prev = current;
        current = current->next;
    }

    if (prev)
    {
        prev->next = event;
    }
    else
    {
        set_irq = 1;
        pos_time_first_delayed = event;
    }
    event->next = current;

    // And finally update the timer trigger time in case we enqueued the event
    // at the head of the wait list.
    if (set_irq)
    {
        // This is important to reload the current time, in case the previous code
        // took too much time so that the interrupt is not missed
        uint32_t timer = timer_count_get(timer_base_fc(0, 1)) + ticks;
        timer_cmp_set(timer_base_fc(0, 1), timer);

        timer_conf_set(timer_base_fc(0, 1),
                       TIMER_CFG_LO_ENABLE(1) |
                           TIMER_CFG_LO_IRQEN(1) |
                           TIMER_CFG_LO_CCFG(TIMER_SOURCE));
    }

    hal_irq_restore(irq);
}

void pos_time_wait_us(int time_us)
{
    pi_task_t task;
    pi_task_push_delayed_us(pi_task_block(&task), time_us);
    pi_task_wait_on(&task);
}

void pi_time_wait_us(int time_us)
{
    pos_time_wait_us(time_us);
}

void __attribute__((constructor)) pos_time_init()
{
    pos_time_first_delayed = NULL;

    // Configure the FC timer in 64 bits mode as it will be used as a common
    // timer for all virtual timers.
    // We also use the ref clock to make the frequency stable unless no oscillator is available

    timer_conf_set(timer_base_fc(0, 1),
                   TIMER_CFG_LO_ENABLE(1) |
                       TIMER_CFG_LO_RESET(1) |
                       TIMER_CFG_LO_CCFG(TIMER_SOURCE));

#if defined(ARCHI_HAS_FC)
    pos_irq_mask_set(1 << ARCHI_FC_EVT_TIMER0_HI);
#else
    pos_irq_set_handler(ARCHI_EVT_TIMER0_HI, pos_time_timer_handler_asm);
    pos_irq_mask_set(1 << ARCHI_EVT_TIMER0_HI);
#endif

    pos_cbsys_add(&pos_time_cbsys_poweroff, POS_CBSYS_POWEROFF, pos_time_poweroff, NULL);
    pos_cbsys_add(&pos_time_cbsys_poweron, POS_CBSYS_POWERON, pos_time_poweron, NULL);
}
