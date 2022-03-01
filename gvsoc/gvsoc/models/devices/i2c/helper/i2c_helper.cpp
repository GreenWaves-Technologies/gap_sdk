/*
 * Copyright (C) 2021  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// The same library is compiled with same flags for all gvsoc mode (normal, debug and system verilog)
// Force trace support to be able to have them.
#define VP_TRACE_ACTIVE 1

#include "i2c_helper.hpp"

#include <stdio.h>
#include <cassert>

namespace {
    void null_callback(i2c_operation_e id, i2c_status_e status, int value)
    {
        (void) id;
        (void) status;
        (void) value;
        //this->trace.msg(vp::trace::LEVEL_TRACE, "null callback: id=%d, status=%d, value=%d\n",
        //        id, status, value);
    }
}

I2C_helper::I2C_helper(vp::component* parent, vp::i2c_master* itf,
        i2c_enqueue_event_fn_t enqueue_event, i2c_cancel_event_fn_t cancel_event,
        std::string trace_path) :
    parent(parent),
    itf(itf),
    enqueue_event(enqueue_event),
    cancel_event(cancel_event),
    delay_low_ps(5),
    delay_high_ps(5),
    internal_state(I2C_INTERNAL_IDLE),
    scl(1),
    sda(1),
    desired_scl(1),
    desired_sda(1),
    sda_rise(1),
    is_starting(false),
    is_stopping(false),
    is_clock_enabled(false),
    clock_value(1),
    is_driving_scl(false),
    is_driving_sda(false),
    cb_master_operation(null_callback),
    clock_event(parent, this, I2C_helper::st_clock_event_handler),
    fsm_event(parent, this, I2C_helper::fsm_event_handler)
{
    assert(NULL != this->parent);
    assert(NULL != this->itf);

    parent->traces.new_trace(trace_path + "/i2c_helper", &this->trace, vp::DEBUG);

    this->trace.msg(vp::trace::LEVEL_TRACE, "Initializing helper interface\n");

    this->pending_data_bits = 0;
    this->fsm_waiting = false;
    this->input_scl = 1;
    this->input_sda = 1;
}

void I2C_helper::st_clock_event_handler(void* __this, vp::clock_event* event)
{
    assert(NULL != __this);
    assert(NULL != event);

    I2C_helper* _this = (I2C_helper*) __this;
    _this->clock_event_handler(event);
}


void I2C_helper::clock_toggle(void)
{
    if (this->is_clock_enabled)
    {
        this->clock_value ^= 1;

        this->enqueue_clock_toggle();
    }
}

void I2C_helper::enqueue_clock_toggle(void)
{
    if (this->is_clock_enabled)
    {
        if (this->clock_event.is_enqueued())
        {
            this->cancel_event(&this->clock_event);
        }

        const uint64_t delay = this->clock_value ? this->delay_low_ps : this->delay_high_ps;
        this->enqueue_event(&this->clock_event, delay);
    }
}


void I2C_helper::fsm_enqueue_event(int64_t delay)
{
    if (!this->fsm_event.is_enqueued())
    {
        this->enqueue_event(&this->fsm_event, delay);
    }
}


void I2C_helper::fsm_event_handler(void *__this, vp::clock_event* event)
{
    I2C_helper* _this = (I2C_helper *) __this;

    _this->fsm_waiting = false;
    _this->fsm_step();
}


void I2C_helper::clock_event_handler(vp::clock_event* event)
{
    assert(NULL != event);

    /* clock toggling */
    if (this->is_clock_enabled)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "Toggling clock (value: %d)\n", this->clock_value);
        if (this->clock_value)
        {
            /* switch to high */
            this->desired_scl = 1;
            this->sync_pins();
        }
        else
        {
            /* switch to low */
            this->desired_scl = 0;
            this->sync_pins();
        }
    }
}

void I2C_helper::register_callback(i2c_callback_t callback)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "register_callback: none\n");
    this->cb_master_operation = callback;
}

void I2C_helper::sync_pins(void)
{
    int res_scl = this->internal_state != I2C_INTERNAL_IDLE ? this->desired_scl : 1;
    int res_sda = this->internal_state != I2C_INTERNAL_IDLE ? this->desired_sda : 1;

    this->trace.msg(vp::trace::LEVEL_TRACE, "Synchronizing pins (scl:%d, sda:%d)\n", res_scl, res_sda);
    this->itf->sync(res_scl, res_sda);
}


void I2C_helper::set_timings(uint64_t delay_low_ps, uint64_t delay_high_ps)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "set_timings: delay_low_ps=%ld, delay_high_ps=%ld\n",
            delay_low_ps,
            delay_high_ps);
    this->delay_low_ps = delay_low_ps;
    this->delay_high_ps = delay_high_ps;
}

void I2C_helper::send_start(void)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Request to send start\n");

    this->is_starting = true;
    this->fsm_enqueue_event(1);
}

void I2C_helper::release_pins(void)
{

}

void I2C_helper::update_pins(int scl, int sda)
{
    this->input_scl = scl;
    this->input_sda = sda;
    this->fsm_enqueue_event(1);
}

void I2C_helper::send_address(int addr, bool is_write, bool is_10bits)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Request to send address (addr: 0x%x, is_write:%d, is_10bits:%d)\n",
            addr, is_write, is_10bits);

    //TODO support 10 bits mode
    assert(!is_10bits);

    int addr_byte = addr << 1 | (is_write ? 0 : 1);
    this->send_data(addr_byte);
}

void I2C_helper::send_ack(bool ack)
{
    // I2C_HELPER_DEBUG("send_ack: ack=%s\n", ack ? "true" : "false");
    // //TODO
    // this->expected_bit_value = ack ? 0 : 1;
    // this->is_driving_sda = 1;
    // this->enqueue_data_change(this->expected_bit_value);
}

void I2C_helper::send_data(int byte)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Request to send data (value: 0x%x)\n", byte);

    if (this->pending_data_bits)
    {
        this->trace.force_warning("Trying to send data while there is already one pending\n");
    }
    else
    {
        this->pending_data = byte;
        this->pending_data_bits = 8;
        this->fsm_enqueue_event(1);
    }
}

void I2C_helper::send_stop(void)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Request to stop\n");
    this->is_stopping = true;
}

void I2C_helper::start_clock(void)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Starting clock\n");

    //start high then loop(low -> high)
    this->is_clock_enabled = true;
    this->clock_value = this->scl ^ 1;
    this->enqueue_clock_toggle();
}

void I2C_helper::stop_clock(void)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Stopping clock\n");
    this->is_clock_enabled =false;
    this->cancel_event(&this->clock_event);
}

std::string I2C_helper::get_state_name(i2c_internal_state_e state)
{
    switch (state)
    {
        case I2C_INTERNAL_IDLE: return "idle";
        case I2C_INTERNAL_WAIT_START: return "wait_start";
        case I2C_INTERNAL_WAIT_STOP: return "wait_stop";
        case I2C_INTERNAL_START: return "start";
        case I2C_INTERNAL_WAIT_DATA: return "wait_data";
        case I2C_INTERNAL_DATA: return "data";
        case I2C_INTERNAL_DATA_READ: return "data_read";
        case I2C_INTERNAL_ACK: return "ack";
        case I2C_INTERNAL_STOP_CLOCK: return "stop_clock";
        case I2C_INTERNAL_STOP_CLOCK_WAIT: return "stop_clock_wait";
        case I2C_INTERNAL_RESTART: return "restart";
        case I2C_INTERNAL_STOP_0: return "stop_0";
        case I2C_INTERNAL_STOP_1: return "stop_1";
        default: return "unknown";
    }
}


void I2C_helper::send_data_bit()
{
    int bit = (this->pending_data >> 7) & 1;
    this->trace.msg(vp::trace::LEVEL_TRACE, "Sending bit (bit: %d)\n", bit);
    this->desired_sda = bit;
    this->pending_data <<= 1;
    this->pending_data_bits--;
}


void I2C_helper::fsm_step()
{
    if (this->fsm_waiting)
    {
        return;
    }

    this->trace.msg(vp::trace::LEVEL_TRACE, "FSM update (state: %s, prev_scl: %d, prev_sda: %d, scl: %d, sda: %d)\n",
        this->get_state_name(this->internal_state).c_str(), this->scl, this->sda, this->input_scl, this->input_sda);

    bool scl_rising = (this->input_scl == 1 && this->scl == 0);
    bool scl_falling = (this->input_scl == 0 && this->scl == 1);
    bool scl_steady = (this->input_scl == this->scl);

    bool sda_rising = (this->input_sda == 1 && this->sda == 0);
    bool sda_falling = (this->input_sda == 0 && this->sda == 1);

    this->scl = this->input_scl;
    this->sda =this-> input_sda;

    /* clock management */
    if (!scl_steady)
    {
        // Renqueue a clock toggle each time it toggles
        this->clock_toggle();
    }

    switch (this->internal_state)
    {
        case I2C_INTERNAL_IDLE:
            if (is_starting)
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Waiting start\n");
                this->internal_state = I2C_INTERNAL_WAIT_START;
                this->is_starting = false;
                this->desired_sda = 0;
                this->desired_scl = 1;
            }
            break;
        
        case I2C_INTERNAL_WAIT_START:
            if (scl_steady && sda_falling)
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Detected start, waiting for data\n");
                this->internal_state = I2C_INTERNAL_WAIT_DATA;
                this->cb_master_operation(I2C_OP_START, I2C_STATUS_OK, 0);
            }
            break;

        case I2C_INTERNAL_WAIT_STOP:
            if (scl_steady && sda_rising)
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Detected stop\n");
                this->internal_state = I2C_INTERNAL_IDLE;
                this->cb_master_operation(I2C_OP_STOP, I2C_STATUS_OK, 0);
            }
            break;

        case I2C_INTERNAL_WAIT_DATA:
            if (this->pending_data_bits)
            {
                this->trace.msg(vp::trace::LEVEL_TRACE, "Detected data, starting clock\n");
                this->internal_state = I2C_INTERNAL_DATA;
            }
            this->start_clock();
            break;

        case I2C_INTERNAL_DATA:
        {
            if (scl_falling)
            {
                this->send_data_bit();
            }
            else if (scl_rising)
            {
                if (this->pending_data_bits == 0)
                {
                    this->internal_state = I2C_INTERNAL_ACK;
                }
            }
            break;
        }

        case I2C_INTERNAL_DATA_READ:
        {
            if (scl_rising)
            {
                this->pending_data = (this->pending_data << 1) | this->sda;
                this->trace.msg(vp::trace::LEVEL_TRACE, "Sampled data (bit: %d, pending_value: 0x%x, pending_bits: %d)\n", this->sda, this->pending_data, this->pending_data_bits);
                this->pending_data_bits--;

                if (this->pending_data_bits == 0)
                {
                    this->cb_master_operation(I2C_OP_DATA, I2C_STATUS_OK, this->pending_data);
                    this->internal_state = I2C_INTERNAL_ACK;
                }
            }
            break;
        }

        case I2C_INTERNAL_ACK:
        {
            if (scl_rising)
            {
                int ack = this->sda;

                this->trace.msg(vp::trace::LEVEL_TRACE, "Sampled ack (value: %d)\n", ack);
                const i2c_status_e status = (ack == 1) ? I2C_STATUS_KO : I2C_STATUS_OK;
                this->cb_master_operation(I2C_OP_ACK, status, 0);
                this->internal_state = I2C_INTERNAL_STOP_CLOCK;
            }
            break;
        }

        case I2C_INTERNAL_STOP_CLOCK:
        {
            if (scl_falling)
            {
                this->desired_sda = 1;
                this->stop_clock();
                this->internal_state = I2C_INTERNAL_STOP_CLOCK_WAIT;
                this->fsm_waiting = true;
                this->fsm_enqueue_event(this->delay_high_ps);
            }
            break;
        }

        case I2C_INTERNAL_STOP_CLOCK_WAIT:
        {
            if (this->pending_data_bits)
            {
                // We must continue immediately with another byte of data
                // Send a bit now since there is no falling edge and let the usual 
                // state continue
                this->send_data_bit();
                this->start_clock();
                this->internal_state = I2C_INTERNAL_DATA;

            }
            else if (this->is_starting)
            {
                this->internal_state = I2C_INTERNAL_RESTART;
                this->is_starting = false;
                this->desired_scl = 1;
                this->fsm_waiting = true;
                this->fsm_enqueue_event(this->delay_high_ps);
            }
            else if (this->is_stopping)
            {
                this->internal_state = I2C_INTERNAL_STOP_0;
                this->is_stopping = false;
                this->desired_sda = 0;
                this->fsm_waiting = true;
                this->fsm_enqueue_event(this->delay_high_ps);
            }
            else
            {
                this->start_clock();
                this->pending_data_bits = 8;
                this->internal_state = I2C_INTERNAL_DATA_READ;
            }
            break;
        }

        case I2C_INTERNAL_STOP_0:
        {
            this->internal_state = I2C_INTERNAL_STOP_1;
            this->desired_scl = 1;
            this->fsm_waiting = true;
            this->fsm_enqueue_event(this->delay_high_ps);
            break;
        }

        case I2C_INTERNAL_STOP_1:
        {
            this->internal_state = I2C_INTERNAL_WAIT_STOP;
            this->desired_sda = 1;
            break;
        }

        case I2C_INTERNAL_RESTART:
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Waiting start\n");
            this->internal_state = I2C_INTERNAL_WAIT_START;
            this->desired_sda = 0;
            break;
        }
    }

    this->sync_pins();
}
