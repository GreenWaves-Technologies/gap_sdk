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

#include "../../udma_impl.hpp"
#include <vp/vp.hpp>
#include <udma_uart/udma_uart_regfields.h>
#include <udma_uart/udma_uart_gvsoc.h>

class Uart_periph;

/**
 * \brief states for UART transmission
 */
typedef enum
{
    UART_TX_STATE_START,
    UART_TX_STATE_DATA,
    UART_TX_STATE_PARITY,
    UART_TX_STATE_STOP
} uart_tx_state_e;

class Uart_tx_fsm {
    public:
        /**
         * \brief uart tx fsm constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to the uart periph
         */
        Uart_tx_fsm(udma* top, Uart_periph* periph);

        /**
         * \brief push multiple bytes on TX
         *
         * \param data pointer to the data to send
         * \param size size in bytes of the data
         */
        void push_bytes(uint8_t* data, int size);

        /**
         * \brief change the cts value, to know if we can send data or not
         *
         * \param cts clear to send bit
         */
        void update_cts(int cts);

        /**
         * \brief update the RTS pin (for the RX side)
         *
         * \param rts ready to send bit
         */
        void update_rts(int rts);

        /**
         * \brief reset the component
         *
         * \param active whether reset is active
         */
        void reset(bool active);

        /**
         * \brief clears the component fifo
         */
        void clear_fifo(void);

        /**
         * \brief clears the component fsm
         */
        void clear_fsm(void);


    private:

        /**
         * \brief update the internal state of the component
         */
        void check_state();

        /**
         * \brief update the busy register field
         *
         * \param busy value of the busy register field
         */
        void update_busy(int busy);

        /**
         * \brief event handler called when there is a word to send
         *
         * \param __this pointer to the object
         * \param event event triggering this handler
         */
        static void handle_pending_word(void *__this, vp::clock_event *event);

        /** event used for sending words */
        vp::clock_event *pending_word_event;

        /** send queue */
        std::queue<uint8_t> send_queue;

        /** UART word to be sent */
        uint32_t current_word;

        /** number of bits to be sent in current word */
        int current_bits;

        /** state of the TX state machine */
        uart_tx_state_e state;

        /** number of stop bits */
        int stop_bits;

        /** parity bit */
        int parity;

        /** value of the cts pin */
        int cts;

        /** whether the component can send data */
        bool can_send;

        /** pointer to periph */
        Uart_periph* periph;
        /** pointer to udma core */
        udma* top;
};
