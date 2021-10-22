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

#include "udma_i2c_fifos.hpp"

#include <cassert>

/********************/
/* POP FIFO METHODS */
/********************/

Pop_fifo::Pop_fifo(udma* top, Udma_tx_channel* pop_channel, uint32_t number_of_elements, uint32_t element_size)
    : number_of_elements(number_of_elements),
    element_size(element_size),
    pop_channel(pop_channel),
    check_ready_event(top, this, Pop_fifo::st_check_ready_event_handler),
    top(top)
{
    assert(NULL != pop_channel);
    assert(NULL != top);
}

void Pop_fifo::st_check_ready_event_handler(void* __this, vp::clock_event* event)
{
    assert(NULL != __this);
    assert(NULL != event);

    Pop_fifo* _this = (Pop_fifo*) __this;

    _this->request_data();
}

void Pop_fifo::push_data(uint8_t* data, int size)
{
    assert(size >= 0);

    for (int i = 0; i < size; i++)
    {
        this->received_bytes.push(data[i]);
    }

    this->request_data();
}

void Pop_fifo::start(void)
{
    this->request_data();
}

bool Pop_fifo::empty(void)
{
    return !(this->received_bytes.size() >= this->element_size);
}

uint8_t Pop_fifo::get_data(void)
{
    uint8_t byte = 0x0;
    if (!this->received_bytes.empty())
    {
        byte = this->received_bytes.front();
        this->received_bytes.pop();
    }

    this->request_data();

    return byte;
}

void Pop_fifo::request_data(void)
{
    const size_t current_size = this->received_bytes.size();
    const size_t full_size =  this->number_of_elements * this->element_size;

    if (current_size < full_size)
    {
        const size_t needed_size = full_size - current_size;
        const int requested_size = this->pop_channel->requested_size;

        if(requested_size < needed_size)
        {
            const size_t size_to_request = needed_size - requested_size;

            if (this->pop_channel->is_ready())
            {
                this->pop_channel->get_data(size_to_request);
            }
            else
            {
                this->enqueue_event();
            }
        }
    }
}

void Pop_fifo::enqueue_event(void)
{
    if (!this->check_ready_event.is_enqueued())
    {
        auto clk = this->top->get_periph_clock();
        assert(NULL != clk);
        clk->event_enqueue(&this->check_ready_event, 1);
    }
}

void Pop_fifo::reset(void)
{
    while(!this->received_bytes.empty())
    {
        this->received_bytes.pop();
    }

    if (this->check_ready_event.is_enqueued())
    {
        auto clk = this->top->get_periph_clock();
        assert(NULL != clk);
        clk->cancel(&this->check_ready_event);
    }
}


/*********************/
/* PUSH FIFO METHODS */
/*********************/

Push_fifo::Push_fifo(udma* top, Udma_rx_channel* push_channel, uint32_t element_size)
    : element_size(element_size),
    loop_event(top, this, Push_fifo::st_loop_event_handler),
    push_channel(push_channel),
    top(top)
{
    assert(NULL != push_channel);
    assert(NULL != top);
}

void Push_fifo::st_loop_event_handler(void* __this, vp::clock_event* event)
{
    assert(NULL != __this);
    assert(NULL != event);

    Push_fifo* _this = (Push_fifo*) __this;

    _this->push_to_channel();
}

void Push_fifo::reset(void)
{
    while(!this->bytes_to_send.empty())
    {
        this->bytes_to_send.pop();
    }

    if (this->loop_event.is_enqueued())
    {
        auto clk = this->top->get_periph_clock();
        assert(NULL != clk);
        clk->cancel(&this->loop_event);
    }
}

void Push_fifo::push(uint8_t* data, size_t size)
{
    assert(NULL != data);

    for (size_t i = 0; i < size; i++)
    {
        this->bytes_to_send.push(data[i]);
    }

    this->push_to_channel();
}

void Push_fifo::push_to_channel(void)
{
    if (!this->bytes_to_send.empty())
    {

        if(this->push_channel->is_ready()
            && this->push_channel->is_active())
        {
            uint8_t byte = this->bytes_to_send.front();
            this->bytes_to_send.pop();

            this->push_channel->push_data(&byte, 1);
        }
        else
        {
            this->enqueue_event();
        }
    }
}

void Push_fifo::enqueue_event(void)
{
    if (!this->loop_event.is_enqueued())
    {
        auto clk = this->top->get_periph_clock();
        assert(NULL != clk);
        clk->event_enqueue(&this->loop_event, 1);
    }
}
