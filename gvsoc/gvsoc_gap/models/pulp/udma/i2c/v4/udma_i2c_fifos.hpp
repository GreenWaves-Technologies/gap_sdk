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

#include <cstdint>
#include <queue>

#include "vp/vp.hpp"
#include "vp/clock/clock_event.hpp"

#include "../../udma_v4_impl.hpp"

class Pop_fifo
{
    // automatically retrieves data from the associated channel
    public:
        Pop_fifo(udma* top, Udma_tx_channel* pop_channel, uint32_t number_of_elements, uint32_t element_size);

        // to be called by pop_channel when new data arrives
        void push_data(uint8_t* data, int size);

        // returns true if it contains at least one element (ie element_size bytes)
        bool empty(void);

        // start requesting data to fill the fifo
        void start(void);

        // return a byte
        uint8_t get_data(void);

        void reset(void);
    private:
        /***********/
        /* Methods */
        /***********/

        // request enough data to fill the fifo
        void request_data(void);

        void enqueue_event(void);

        /******************/
        /* Static methods */
        /******************/
        static void st_check_ready_event_handler(void* __this, vp::clock_event* event);

        /**********/
        /* Fields */
        /**********/
        udma* top;
        Udma_tx_channel* pop_channel;

        uint32_t number_of_elements;
        uint32_t element_size;

        std::queue<uint8_t> received_bytes;

        vp::clock_event check_ready_event;
};

class Push_fifo
{
    //automatically pushes data to the associated channel
    public:
        Push_fifo(udma* top, Udma_rx_channel* push_channel, uint32_t element_size);

        void push(uint8_t *data, size_t size);

        void reset(void);
    private:
        /***********/
        /* Methods */
        /***********/
        void enqueue_event(void);
        void push_to_channel(void);

        /******************/
        /* Static methods */
        /******************/
        static void st_loop_event_handler(void* __this, vp::clock_event* event);

        /**********/
        /* Fields */
        /**********/
        udma* top;
        Udma_rx_channel* push_channel;
        uint32_t element_size;

        std::queue<uint8_t> bytes_to_send;

        vp::clock_event loop_event;
};
