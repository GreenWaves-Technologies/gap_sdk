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

#pragma once

#include <udma_uart/udma_uart_regfields.h>
#include <udma_uart/udma_uart_gvsoc.h>

class Uart_periph;

/**
 * \brief states for UART reception
 */
typedef enum
{
    UART_RX_STATE_WAIT_START,
    UART_RX_STATE_DATA,
    UART_RX_STATE_PARITY,
    UART_RX_STATE_WAIT_STOP
} uart_rx_state_e;

class Uart_rx_fsm {
    public:
        /**
         * \brief uart rx fsm constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to the uart periph
         */
        Uart_rx_fsm(udma* top, Uart_periph* periph);

        /**
         * \brief used to receive bits on UART RX
         *
         * \param bit received bit (0 or 1)
         * \param sck usart clock
         * \param cts clear-to-send signal
         */
        void handle_rx_bits(int bit, int sck);

        /**
         * \brief resets the component
         *
         * \param active whether reset is active
         */
        void reset(bool active);

        /**
         * \brief called when the udma channel gets to active state
         *
         * Used to know if we can push data to it and update RTS.
         */
        void notify_active(void);

        /**
         * \brief clears the component fifo
         */
        void clear_fifo(void);

        /**
         * \brief clears the component fsm
         */
        void clear_fsm(void);

    public:

        /**
         * \brief update the internal state of the component
         */
        void check_state(void);

        /**
         * \brief check if we need and can push data to the udma channel
         *
         * If we cannot enqueue an event to wait
         */
        void check_push(void);

        /**
         * \brief push to a reception fifo
         *
         * \param word word to push to the fifo
         *
         * \warning word will be dropped if there is no space inside the fifo
         */
        void push_in_fifo(int word);

        /**
         * \brief update the value of RTS depending on whether if can receive data
         */
        void update_rts(void);

        /**
         * \brief update the busy register field
         *
         * \param busy value of the register field
         */
        void update_busy(int busy);

        /**
         * \brief push data in udma channel
         */
        void push_in_channel(void);

        /**
         * \brief event handler called to sample data
         *
         * \param __this pointer to the object
         * \param event event triggering this handler
         */
        static void sampling_handler(void* __this, vp::clock_event* event);

        /**
         * \brief event handler called to push data to udma channel
         *
         * \param __this pointer to the object
         * \param event event triggering this handler
         */
        static void push_handler(void* __this, vp::clock_event* event);

        /** state of the rx state machine */
        uart_rx_state_e state;

        /** event used to sample data */
        vp::clock_event *sampling_event;

        /** event used to push data to the udma channel */
        vp::clock_event *push_event;

        /** whether we are sampling */
        bool sampling;

        /** reception fifo */
        Udma_fifo<int> fifo_receive;

        /** bit being sampled */
        int bit;

        /** bit of parity */
        int parity;

        /** number of stop bits */
        int stop_bits;

        /** current byte/word */
        uint8_t  pending_rx_byte;

        /** number of received bits, to know if we received a full word */
        int nb_received_bits;

        /** pointer to periph */
        Uart_periph* periph;
        /** pointer to udma core */
        udma* top;
};

