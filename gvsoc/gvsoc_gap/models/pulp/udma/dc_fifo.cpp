/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "udma_impl.hpp"
#include "dc_fifo.hpp"
#include "archi/utils.h"


Dc_fifo::Dc_fifo(int size, vp::component *top, std::string trace_path,
    vp::clock_engine *snd_clock, vp::clock_engine *rcv_clock, 
    vp::clock_event *rcv_event,
    vp::clock_event *unstall_event)
{
    this->top = top;
    this->size = size;
    this->rcv_fsm_event = top->event_new(this, Dc_fifo::handle_rcv_fsm);
    this->snd_fsm_event = top->event_new(this, Dc_fifo::handle_snd_fsm);
    this->snd_stall_event = top->event_new(this, Dc_fifo::handle_snd_stall);
    this->snd_clock = snd_clock;
    this->rcv_clock = rcv_clock;
    this->rcv_event = rcv_event;
    this->unstall_event = unstall_event;

    top->traces.new_trace(trace_path + "/dc_fifo", &this->trace, vp::DEBUG);

    if (size < 4)
    {
        this->trace.fatal("DC FIFO size should be at least 4\n");
    }
}


void Dc_fifo::reset(bool active)
{
    this->nb_elems = 0;
    this->snd_div = 0;
    this->stalled = false;
    this->stalled_sync = false;

    this->rcv_clock->cancel(rcv_fsm_event);
    this->snd_clock->cancel(snd_fsm_event);
    this->snd_clock->cancel(snd_stall_event);
    this->rcv_clock->cancel(rcv_event);
    this->snd_clock->cancel(unstall_event);
}


int64_t Dc_fifo::get_snd_cycle_count(int64_t cycles)
{
    if (this->snd_div == 0)
    {
        return cycles;
    }
    else
    {
        return cycles * this->snd_div;
    }
}


void Dc_fifo::push(uint8_t *data, int size)
{
    this->rcv_data_queue.push(data);
    this->rcv_size_queue.push(size);

    this->trace.msg(vp::trace::LEVEL_DEBUG, "Pushing data to DC fifo (data: %p, size: %d, nb_elems: %d)\n", data, size, this->nb_elems);

    // Since the FIFO gets full when there is only 3 elements, check if we need to trigger the
    // full signal resynchronization.
    if (this->nb_elems == this->size - 4)
    {
        this->nb_elems_check = this->snd_clock->get_cycles();
    }
    this->nb_elems++;

    this->check_state();
}


uint8_t *Dc_fifo::pop(int *size)
{
    uint8_t *result = this->rcv_data_queue.front();
    *size = this->rcv_size_queue.front();
    this->rcv_data_queue.pop();
    this->rcv_size_queue.pop();

    this->trace.msg(vp::trace::LEVEL_DEBUG, "Popping data from DC fifo (data: %p, size: %d, nb_elems: %d)\n", result, *size, this->nb_elems);

    this->nb_elems--;

    // Check if the FIFO can be unstalled.
    // This happens when it is stalled and becomes empty
    if (!this->snd_stall_event->is_enqueued() && this->stalled && this->nb_elems == 0)
    {
        // In this case we trigger the unstall signal resynchronization since it takes some time
        // to propagate (1 clock cycle on sender side)
        this->snd_clock->enqueue_ext(this->snd_stall_event, this->get_snd_cycle_count(1));
    }

    this->check_state();

    return result;
}


bool Dc_fifo::is_full()
{
    return this->nb_elems >= this->size || this->stalled;
}


bool Dc_fifo::is_empty()
{
    return this->rcv_data_queue.empty();
}



void Dc_fifo::check_state()
{
    // To simplify we schedule receiver and sender FSM at every cycle as soon as there is
    // at least one element in the FIFO
    if (!this->rcv_fsm_event->is_enqueued() && this->nb_elems > 0)
    {
        this->rcv_clock->enqueue_ext(this->rcv_fsm_event, 1);
    }

    if (!this->snd_fsm_event->is_enqueued() && this->nb_elems > 0)
    {
        this->snd_clock->enqueue_ext(this->snd_fsm_event, this->get_snd_cycle_count(1));
    }
}


void Dc_fifo::handle_rcv_fsm(void *__this, vp::clock_event *event)
{
    Dc_fifo *_this = (Dc_fifo *)__this;
    _this->trace.msg(vp::trace::LEVEL_TRACE, "Receiver FSM handler\n");

    // In the receiver FSM, we just have to check if we should scheduler a pop
    // if the FIFO is not empty.
    // This takes 4 cycles for a data to get ready for receiver once it is pushed.
    if (!_this->rcv_event->is_enqueued() && !_this->rcv_data_queue.empty())
    {
        _this->trace.msg(vp::trace::LEVEL_TRACE, "Enqueue pop\n");
        _this->rcv_clock->enqueue_ext(_this->rcv_event, 4);
    }
}


void Dc_fifo::handle_snd_fsm(void *__this, vp::clock_event *event)
{
    Dc_fifo *_this = (Dc_fifo *)__this;

    // In the sender FSM, we just need to care about the handling of the stall signal.

    // The stall signal is resynchronized and take 2 cycles to take effect
    // This one is the intermediate once
    if (_this->stalled_sync)
    {
        // If true, stall the fifo
        _this->stalled_sync = false;
        _this->stalled = true;
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Stalling fifo\n");

        // Be carefull, since it takes some time to the stall signal to propagate,
        // The FIFO might have become empty since it was triggered.
        // In this case, immediatly unstall the FIFO (which takes 1 cycle)
        if (!_this->snd_stall_event->is_enqueued() && _this->nb_elems == 0)
        {
            _this->snd_clock->enqueue_ext(_this->snd_stall_event, _this->get_snd_cycle_count(1));
        }
    }

    // If we reach the number of cycles where the check of the number of elemets should be done,
    // Check if we have more than 3 free slots, and if no, trigger the stall signal resync.
    if (_this->nb_elems_check < _this->snd_clock->get_cycles() && _this->size - _this->nb_elems <= 3 && !_this->stalled && !_this->stalled_sync)
    {
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Fifo stall sync\n");
        _this->stalled_sync = true;
    }

    _this->check_state();
}


void Dc_fifo::handle_snd_stall(void *__this, vp::clock_event *event)
{
    Dc_fifo *_this = (Dc_fifo *)__this;
    // Just enqueue the sender unstall event
    _this->stalled = false;
    _this->snd_clock->enqueue_ext(_this->unstall_event, _this->get_snd_cycle_count(1));
}
