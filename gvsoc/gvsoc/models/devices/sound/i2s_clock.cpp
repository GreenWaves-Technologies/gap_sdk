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
#include <vp/itf/io.hpp>
#include <vp/itf/i2s.hpp>
#include <vp/itf/clock.hpp>


class I2s_clock : public vp::component
{
    friend class Stim_txt;

public:
    I2s_clock(js::config *config);

    int build();
    void start();

protected:

    static void handler(void *__this, vp::clock_event *event);

    vp::i2s_master i2s_itf;
    vp::clock_master clock_cfg;
    vp::clock_event *event;

    int sck;
    bool is_ws;
};


I2s_clock::I2s_clock(js::config *config)
    : vp::component(config)
{
}


int I2s_clock::build()
{
    this->new_master_port("i2s", &this->i2s_itf);
    this->new_master_port("clock_cfg", &this->clock_cfg);

    this->event = this->event_new(I2s_clock::handler);
    this->sck = 0;
    this->is_ws = this->get_js_config()->get_child_bool("is_ws");

    return 0;
}


void I2s_clock::start()
{
    int frequency = this->get_js_config()->get_child_int("frequency");

    if (frequency != 0)
    {
        this->clock_cfg.set_frequency(frequency);
        this->event_enqueue(this->event, 1);
    }
}


void I2s_clock::handler(void *__this, vp::clock_event *event)
{
    I2s_clock *_this = (I2s_clock *)__this;

    if (_this->is_ws)
    {
        _this->i2s_itf.sync(2, _this->sck, 2);
    }
    else
    {
        _this->i2s_itf.sync(_this->sck, 2, 2);
    }

    _this->sck ^= 1;

    _this->event_enqueue(_this->event, 1);
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new I2s_clock(config);
}
