/*
 * Copyright (C) 2021 GreenWaves Technologies, SAS, ETH Zurich and
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

#include "i2c_corruptor.hpp"

#include "stdio.h"
#include <cassert>

#define CORRUPTOR_DEBUG(...) (this->trace.msg(vp::trace::LEVEL_TRACE, __VA_ARGS__))

I2c_corruptor::I2c_corruptor(js::config* config)
    : vp::component(config),
    i2c_helper(this,
            &this->i2c_itf,
            std::bind(&I2c_corruptor::i2c_enqueue_event,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            std::bind(&I2c_corruptor::i2c_cancel_event,
                this,
                std::placeholders::_1)
            ),
    start_event(this, this, &I2c_corruptor::st_start_event_handler),
    prev_sda(1),
    prev_scl(1),
    start_counter(0),
    start_threshold(2),
    is_started(false)
{
    assert(NULL != config);
    /* set helper callback */
    this->i2c_helper.register_callback(std::bind(&I2c_corruptor::i2c_helper_callback,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3));
    this->i2c_helper.set_timings(100, 100);
}

void I2c_corruptor::st_start_event_handler(void* __this, vp::clock_event* event)
{
    assert(NULL != __this);
    assert(NULL != event);

    I2c_corruptor* _this = (I2c_corruptor*) __this;

    if (!_this->i2c_helper.is_busy())
    {
        _this->is_started = true;
        _this->trace.msg(vp::trace::LEVEL_TRACE, "sending start\n");
        _this->i2c_helper.send_start();
    }
}

void I2c_corruptor::i2c_sync(void *__this, int scl, int sda)
{
    assert(NULL != __this);
    I2c_corruptor* _this = (I2c_corruptor*) __this;

    /* detect start to launch start */
    const bool scl_steady1 = (_this->prev_scl == 1 && scl == _this->prev_scl);
    const bool sda_falling = (_this->prev_sda == 1 && sda == 0);

    _this->prev_sda = sda;
    _this->prev_scl = scl;

    if (!_this->is_started)
    {
        if (!_this->i2c_helper.is_busy() && scl_steady1 && sda_falling)
        {
            /* increment start counter */
            _this->start_counter++;
        }
        if (_this->start_counter >= _this->start_threshold)
        {
            _this->start_counter = 0;
            /* trigger start */
            _this->trigger_start();
        }
    }

    _this->i2c_helper.update_pins(scl, sda);
}

void I2c_corruptor::trigger_start(void)
{
    if (!this->start_event.is_enqueued())
    {
        this->event_enqueue(&this->start_event, 1);
    }
}


void I2c_corruptor::reset(bool active)
{
    if (active)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "Resetting component\n");
        //TODO reset i2c interface
    }
}

int I2c_corruptor::build(void)
{
    traces.new_trace("trace", &this->trace, vp::DEBUG);
    this->trace.msg(vp::trace::LEVEL_TRACE, "Building component\n");

    this->i2c_itf.set_sync_meth(&I2c_corruptor::i2c_sync);
    this->new_master_port("i2c", &this->i2c_itf);

    this->new_master_port("clock_cfg", &this->clock_cfg);
    return 0;
}

void I2c_corruptor::start(void)
{
}

void I2c_corruptor::i2c_helper_callback(i2c_operation_e id, i2c_status_e status, int value)
{
    CORRUPTOR_DEBUG("CALLBACK id=%d, status=%d, value=%d !\n",
            id, status, value);

    switch(id)
    {
        case I2C_OP_START:
            if (this->is_started)
            {
                CORRUPTOR_DEBUG("Sending 0 to trigger arbitration loss\n");
                this->i2c_helper.send_data(0x0);
            }
            break;
        case I2C_OP_DATA:
            if (this->is_started)
            {
                CORRUPTOR_DEBUG("Sending ack to trigger arbitration loss\n");
                this->i2c_helper.send_ack(true);
            }
            break;
        case I2C_OP_ACK:
            if (this->is_started)
            {
                this->i2c_helper.send_stop();
            }
            break;
        case I2C_OP_STOP:
            if (this->is_started)
            {
                this->is_started = false;
            }
            break;
        default:
            break;
    }
}

void I2c_corruptor::i2c_enqueue_event(vp::clock_event* event, uint64_t time_unit)
{
    this->event_enqueue(event, time_unit);
}

void I2c_corruptor::i2c_cancel_event(vp::clock_event* event)
{
    this->event_cancel(event);
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new I2c_corruptor(config);
}
