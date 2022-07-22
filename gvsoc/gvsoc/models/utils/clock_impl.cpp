/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

#include <vp/vp.hpp>
#include <vp/itf/clock.hpp>
#include <stdio.h>
#include <string.h>

class Clock : public vp::component
{

public:
    Clock(js::config *config);

    int build();
    void reset(bool active);

private:
    static void edge_handler(void *__this, vp::clock_event *event);
    void raise_edge();
    static void power_sync(void *__this, bool active);

    vp::wire_slave<bool> power_itf;
    vp::clock_master clock_ctrl_itf;
    vp::clock_master clock_sync_itf;
    vp::clock_event *event;
    int value;
    float target_frequency;
    float frequency;
    float powerup_time;
    bool powered_on;
    int64_t start_time;
};

void Clock::edge_handler(void *__this, vp::clock_event *event)
{
    Clock *_this = (Clock *)__this;
    _this->raise_edge();
}

void Clock::raise_edge()
{
    if (this->clock_sync_itf.is_bound())
    {
        this->get_trace()->msg(vp::trace::LEVEL_TRACE, "Changing clock level (level: %d)\n", value);

        if (this->target_frequency && this->clock_ctrl_itf.is_bound())
        {
            int64_t diff_time = this->get_time()- this->start_time;
            if (diff_time >= this->powerup_time)
            {
                this->clock_ctrl_itf.set_frequency(this->target_frequency);
                this->target_frequency = 0;
            }
            else
            {
                float frequency = this->target_frequency * (0.1 + 0.9 * diff_time / this->powerup_time);
                this->clock_ctrl_itf.set_frequency(frequency);
            }
        }

        this->clock_sync_itf.sync(value);
        this->value ^= 1;
        this->event_enqueue(this->event, 1);
    }
}


void Clock::power_sync(void *__this, bool active)
{
    Clock *_this = (Clock *)__this;

    _this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Changing clock power (is_on: %d)\n", active);

    if (active != _this->powered_on)
    {
        if (active)
        {
            _this->target_frequency = _this->frequency;
            _this->start_time = _this->get_time();
            _this->event_enqueue(_this->event, 1);
        }
        else
        {
            if (_this->event->is_enqueued())
            {
                _this->event_cancel(_this->event);
            }
        }
    }

    _this->powered_on = active;
}


Clock::Clock(js::config *config)
    : vp::component(config)
{
}

int Clock::build()
{
    this->event = this->event_new(Clock::edge_handler);

    this->power_itf.set_sync_meth(&Clock::power_sync);
    this->new_slave_port("power", &this->power_itf);
    this->new_master_port("clock_ctrl", &this->clock_ctrl_itf);
    this->new_master_port("clock_sync", &this->clock_sync_itf);
    this->value = 0;
    this->powerup_time = this->get_js_config()->get_child_int("powerup_time");
    this->powered_on = this->get_js_config()->get("powered_on") == NULL || this->get_js_config()->get_child_bool("powered_on");

    return 0;
}

void Clock::reset(bool active)
{
    if (!active)
    {
        this->frequency = this->get_clock()->get_frequency();
        this->clock_sync_itf.set_frequency(this->get_clock()->get_frequency() / 2);

        if (this->powered_on)
        {
            if (!this->event->is_enqueued())
            {
                this->event_enqueue(this->event, 1);
            }
        }
    }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Clock(config);
}
