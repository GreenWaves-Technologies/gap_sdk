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

#include "../../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/uart.hpp"

using namespace std::placeholders;


Uart_rx_fsm::Uart_rx_fsm(udma* top, Uart_periph* periph)
    : top(top), periph(periph),
    state(UART_RX_STATE_WAIT_START),
    bit(1),
    parity(0),
    stop_bits(1),
    sampling(false),
    pending_rx_byte(0),
    nb_received_bits(0),
    fifo_receive(8)
{
    this->sampling_event = top->event_new(this, Uart_rx_fsm::sampling_handler);
    this->push_event = top->event_new(this, Uart_rx_fsm::push_handler);
    this->periph->send_rts(1);
}


void Uart_rx_fsm::reset(bool active)
{
    if (active)
    {
        this->periph->send_rts(1);

        this->clear_fifo();
        this->clear_fsm();

        /* cancel events */
        if (this->sampling_event->is_enqueued())
        {
            this->top->get_periph_clock()->cancel(this->sampling_event);
        }
        if (this->push_event->is_enqueued())
        {
            this->top->get_periph_clock()->cancel(this->push_event);
        }
    }
}


void Uart_rx_fsm::sampling_handler(void* __this, vp::clock_event* event)
{
    Uart_rx_fsm* _this = (Uart_rx_fsm*) __this;

    int bit = _this->bit;

    _this->periph->trace.msg(vp::trace::LEVEL_DEBUG, "Sampling RX bit: %d\n", bit);
    if (_this->state == UART_RX_STATE_WAIT_START)
    {
        _this->periph->trace.msg(vp::trace::LEVEL_TRACE, "RX_FSM: START\n");

        if (bit == 0)
        {
            _this->parity = 0;
            _this->state = UART_RX_STATE_DATA;
            _this->update_busy(1);
        }
    }
    else if (_this->state == UART_RX_STATE_DATA)
    {
        const int word_length = _this->periph->regmap.setup.bit_length_get() + 5;
        _this->periph->trace.msg(vp::trace::LEVEL_TRACE, "RX_FSM: DATA\n");
        _this->pending_rx_byte = (_this->pending_rx_byte >> 1)
            | (bit << (word_length - 1));
        _this->nb_received_bits++;
        _this->parity ^= bit;

        if (_this->nb_received_bits == word_length)
        {
            _this->periph->trace.msg(vp::trace::LEVEL_DEBUG,
                    "RX_FSM: pushing data (value: 0x%x)\n", _this->pending_rx_byte);

            _this->push_in_fifo(_this->pending_rx_byte);

            _this->nb_received_bits = 0;
            if (_this->periph->regmap.setup.parity_ena_get())
            {
                _this->state = UART_RX_STATE_PARITY;
            }
            else
            {
                _this->stop_bits = _this->periph->regmap.setup.stop_bits_get() + 1;
                _this->state = UART_RX_STATE_WAIT_STOP;
            }

            /* trigger rx soc event if needed */
            if (0 != _this->periph->regmap.irq_en.rx_irq_get())
            {
                _this->top->trigger_event(_this->periph->rx_soc_event);
            }
        }
    }
    else if (_this->state == UART_RX_STATE_PARITY)
    {
        _this->periph->trace.msg(vp::trace::LEVEL_TRACE, "RX_FSM: PARITY\n");
        if (bit != _this->parity)
        {
            _this->periph->regmap.error.err_parity_set(1);
            /* throw a soc event if needed */
            if (0 != _this->periph->regmap.irq_en.err_irq_get())
            {
                _this->top->trigger_event(_this->periph->error_soc_event);
            }
        }
        _this->stop_bits = _this->periph->regmap.setup.stop_bits_get() + 1;
        _this->state = UART_RX_STATE_WAIT_STOP;
    }
    else if (_this->state == UART_RX_STATE_WAIT_STOP)
    {
        _this->periph->trace.msg(vp::trace::LEVEL_TRACE, "RX_FSM: STOP\n");
        if (bit == 1)
        {
            _this->stop_bits--;
            if (_this->stop_bits == 0)
            {
                _this->state = UART_RX_STATE_WAIT_START;
                _this->sampling = false;
                _this->update_busy(0);
            }
        }
    }

    if (_this->state != UART_RX_STATE_WAIT_START)
    {
        _this->check_state();
    }
}

void Uart_rx_fsm::push_handler(void* __this, vp::clock_event* event)
{
    Uart_rx_fsm* _this = (Uart_rx_fsm*) __this;
    _this->push_in_channel();
}

void Uart_rx_fsm::push_in_fifo(int word)
{
    if (!this->fifo_receive.is_full())
    {
        /* we put it in fifo if we can, else we drop it */
        this->fifo_receive.push(word);
    }
    else
    {
        this->periph->regmap.error.err_overflow_set(1);
        /* throw a soc event if needed */
        if (0 != this->periph->regmap.irq_en.err_irq_get())
        {
            this->top->trigger_event(this->periph->error_soc_event);
        }

    }

    this->update_rts();
    this->push_in_channel();

}

void Uart_rx_fsm::push_in_channel(void)
{
    if (this->periph->rx_channel->is_ready()
            && this->periph->rx_channel->is_active()
            && !this->fifo_receive.is_empty())
    {
        uint8_t byte = this->fifo_receive.pop();
        this->periph->rx_channel->push_data(&byte, 1);
        this->update_rts();
    }
    this->check_push();
}

void Uart_rx_fsm::update_rts(void)
{
    if (this->fifo_receive.get_size() <= this->periph->regmap.setup_2.rts_high_limit_get()
                && this->periph->regmap.setup.rts_en_get())
    {
        this->periph->send_rts(0);
    }
    else
    {
        /* not accepting new data */
        this->periph->send_rts(1);
    }
}

void Uart_rx_fsm::check_push(void)
{
    if(!this->fifo_receive.is_empty()
            && !this->push_event->is_enqueued()
            && this->periph->rx_channel->is_ready() && this->periph->rx_channel->is_active())
    {
        this->top->get_periph_clock()->enqueue_ext(this->push_event, 1);
    }
}

void Uart_rx_fsm::check_state(void)
{
    if ((!this->sampling && bit == 0) || this->sampling)
    {
        if (!(this->sampling_event)->is_enqueued())
        {
            /* vary number of cycles depending clock divider */
            int period = this->periph->regmap.setup.clkdiv_get() + 1;

            this->top->get_periph_clock()->enqueue_ext(this->sampling_event,
                    (this->sampling ? period : period / 2) );
            this->sampling = true;
        }
    }
}

void Uart_rx_fsm::handle_rx_bits(int bit, int sck)
{
    (void) sck;
    this->bit = bit;
    this->check_state();
}

void Uart_rx_fsm::notify_active(void)
{
    this->push_in_channel();
    this->update_rts();
}

void Uart_rx_fsm::update_busy(int busy)
{
    this->periph->regmap.status.rx_busy_set(busy);
}

void Uart_rx_fsm::clear_fifo(void)
{
    while(!this->fifo_receive.is_empty())
    {
        this->fifo_receive.pop();
    }
}

void Uart_rx_fsm::clear_fsm(void)
{
    this->state = UART_RX_STATE_WAIT_START;
    this->nb_received_bits = 0;
    this->bit = 1;
    this->sampling = false;
}
