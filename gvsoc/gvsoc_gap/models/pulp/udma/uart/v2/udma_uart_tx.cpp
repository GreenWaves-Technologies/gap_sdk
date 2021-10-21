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

#include "udma_uart_tx.hpp"
#include "udma_uart.hpp"
#include "archi/utils.h"
#include "vp/itf/uart.hpp"

using namespace std::placeholders;

Uart_tx_fsm::Uart_tx_fsm(udma* top, Uart_periph* periph)
    : top(top), periph(periph),
    state(UART_TX_STATE_START),
    current_word(0),
    current_bits(0),
    stop_bits(0),
    cts(0),
    can_send(true),
    parity(0)
{
    pending_word_event = top->event_new(this, Uart_tx_fsm::handle_pending_word);
}

void Uart_tx_fsm::handle_pending_word(void *__this, vp::clock_event *event)
{
    Uart_tx_fsm* _this = (Uart_tx_fsm*)__this;
    _this->periph->trace.msg(vp::trace::LEVEL_TRACE,
            "TX_FSM: STATE=%d !\n", _this->state);

    int bit = -1;
    //bool end = false;

    if (_this->state == UART_TX_STATE_START)
    {
        _this->periph->trace.msg(vp::trace::LEVEL_TRACE, "TX_FSM: START\n");
        _this->parity = 0;

        bit = 1;
        /* check if CTS is 0 to send next work if and only if cts_enabled */
        if (!_this->send_queue.empty() && _this->can_send)
        {
            _this->state = UART_TX_STATE_DATA;
            _this->current_bits = _this->periph->regmap.setup.bit_length_get() + 5;
            _this->current_word = _this->send_queue.front();
            _this->send_queue.pop();
            _this->periph->trace.msg(vp::trace::LEVEL_DEBUG, "Sending byte (value: 0x%x)\n", _this->current_word);
            bit = 0;
        }
    }
    else if (_this->state == UART_TX_STATE_DATA)
    {
        _this->periph->trace.msg(vp::trace::LEVEL_TRACE, "TX_FSM: DATA\n");
        bit = _this->current_word & 1;
        _this->current_word >>= 1;
        _this->parity ^= bit;

        _this->current_bits -= 1;
        if (_this->current_bits == 0)
        {
            _this->stop_bits = _this->periph->regmap.setup.stop_bits_get() + 1;
            if (_this->periph->regmap.setup.parity_ena_get())
                _this->state = UART_TX_STATE_PARITY;
            else
            {
                _this->state = UART_TX_STATE_STOP;
            }
        }
    }
    else if (_this->state == UART_TX_STATE_PARITY)
    {
        bit = _this->parity;
        _this->state = UART_TX_STATE_STOP;
    }
    else if (_this->state == UART_TX_STATE_STOP)
    {
        bit = 1;
        _this->stop_bits--;
        if (_this->stop_bits <= 0)
        {
            _this->state = UART_TX_STATE_START;
        }

        /* request new byte */
        _this->periph->tx_channel->get_data(1);

        /* send irq if needed */
        if (0 != _this->periph->regmap.irq_en.tx_irq_get())
        {
            _this->top->trigger_event(_this->periph->tx_soc_event);
        }
    }

    /* update TX pad with current bit */
    if (bit != -1)
    {
        /* verify that UART interface is connected */
        if (!_this->periph->uart_itf.is_bound())
        {
            _this->top->get_trace()->warning(
                    "Trying to send to UART interface while it is not connected\n");
        }
        else
        {
            if (_this->periph->regmap.setup.tx_ena_get())
            {
                _this->top->get_trace()->msg(vp::trace::LEVEL_TRACE,
                        "Sending bit (value: %d)\n", bit);

                /* update pad */
                _this->periph->send_bit(bit);
            }
        }
    }

    /* verify if there is still something to send */
    _this->check_state();
}

void Uart_tx_fsm::check_state()
{
    if(this->periph->regmap.setup.cts_en_get() == 0 ||
            this->cts == 0)
    {
        this->can_send = true;
    }
    else
    {
        this->can_send = false;
    }

    if ((!this->send_queue.empty() && this->can_send)
            || this->current_bits != 0
            || this->stop_bits != 0)
    {
        if (!(this->pending_word_event)->is_enqueued())
        {
            /* vary number of cycles depending clock divider */
            this->top->get_periph_clock()->enqueue_ext(this->pending_word_event,
                    this->periph->regmap.setup.clkdiv_get() + 1);
            this->update_busy(1);
        }
    }
    else
    {
        this->update_busy(0);
    }
}

void Uart_tx_fsm::update_busy(int busy)
{
    this->periph->regmap.status.tx_busy_set(busy);
}

void Uart_tx_fsm::update_cts(int cts)
{
    this->cts = cts;
    this->check_state();
}


void Uart_tx_fsm::reset(bool active)
{
    if (!active)
    {
        this->periph->send_bit(1);

        this->clear_fsm();
        this->clear_fifo();

        /* cancel event */
        if (this->pending_word_event->is_enqueued())
        {
            this->top->get_periph_clock()->cancel(this->pending_word_event);
        }
    }
}

void Uart_tx_fsm::push_bytes(uint8_t* data, int size)
{
    this->periph->trace.msg(vp::trace::LEVEL_TRACE,
            "Pushing data to send on TX (size: %d)\n", size);

    /* push data to queue */
    for (int i = 0; i < size; i++)
    {
        this->periph->trace.msg(vp::trace::LEVEL_TRACE,
                "data[%d] = %x\n", i, data[i]);
        this->send_queue.push(data[i]);
    }

    this->periph->trace.msg(vp::trace::LEVEL_TRACE,
            "send_queue length : %ld\n", this->send_queue.size());

    /* check if event needs to be enqueued */
    this->check_state();
}

void Uart_tx_fsm::clear_fifo(void)
{
    while (!this->send_queue.empty())
    {
        this->send_queue.pop();
    }
}

void Uart_tx_fsm::clear_fsm(void)
{
    this->state = UART_TX_STATE_START;
    this->current_bits = 0;
    this->stop_bits = 0;
    this->parity = 0;
    this->cts = 0;
}
