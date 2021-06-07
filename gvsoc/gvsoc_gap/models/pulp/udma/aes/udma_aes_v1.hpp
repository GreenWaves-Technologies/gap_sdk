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

#ifndef __PULP_UDMA_UDMA_AES_V1_HPP__
#define __PULP_UDMA_UDMA_AES_V1_HPP__

#include <vp/vp.hpp>
#include <archi/chips/gap9_v2/udma_aes/udma_aes_regs.h>
#include <archi/chips/gap9_v2/udma_aes/udma_aes_regfields.h>
#include <archi/chips/gap9_v2/udma_aes/udma_aes_structs.h>
#include <archi/chips/gap9_v2/udma_aes/udma_aes_gvsoc.h>
#include "../udma_impl.hpp"
#include "udma_aes_model_v1.hpp"


class Aes_periph;

#define AES_BLOCK_SIZE_BYTES (AES_BLOCKLEN)


/**
 * \brief Channel used to transmit data from the AES
 */
class Aes_rx_channel : public Udma_rx_channel
{
    public:
        /**
         * \brief AES RX channel constructor
         *
         * \param top udma core
         * \param periph pointer to aes peripheral
         * \param name channel name
         */
        Aes_rx_channel(udma *top, Aes_periph *periph, string name);

    private:
        /** pointer to aes peripheral */
        Aes_periph *periph;
};

/**
 * \brief Channel used to receive data to the AES
 */
class Aes_tx_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief AES TX channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to aes peripheral
         * \param name channel name
         */
        Aes_tx_channel(udma *top, Aes_periph *periph, string name);

        /**
         * \brief called when data is given to the channel, forwards to AES
         *
         * \param data pointer to the data
         * \param size size of the data in bytes
         */
        void push_data(uint8_t *data, int size);

    private:

        /** pointer to aes peripheral */
        Aes_periph *periph;
};

/**
 * \brief UDMA AES peripheral
 */
class Aes_periph : public Udma_periph
{

    public:
        /**
         * \brief AES constructor
         *
         * \param top udma core
         * \param id TODO
         * \param itf_id interface id
         */
        Aes_periph(udma *top, int id, int itf_id);

        /**
         * \brief used to reset the peripheral
         *
         * \param active whether reset is active or not
         */
        void reset(bool active);

        /**
         * \brief processing incoming data
         *
         * \param data data to process
         * \param size size of the data in bytes
         */
        void process_data(uint8_t* data, int size);

    private:
        /**
         * \brief custom register request callback
         *
         * \param req request details
         * \param offset offset
         *
         * \returns whether request was valid or not
         */
        vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);

        /**
         * \brief channels register callback
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value value of the request
         * \param is_write flag to indicate whether request is write or not
         */
        void dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief setup register callback
         *
         * used to trigger key init and block reset
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value value of the request
         * \param is_write flag to indicate whether request is write or not
         */
        void setup_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief AES processing event handler
         *
         * called when processing event is done, convert data and
         * trigger a sending event
         */
        static void processing_handler(void* __this, vp::clock_event* event);

        /**
         * \brief AES sending event handler
         *
         * called when a sending event is done, send data to rx channel if possible
         * until there is no data left to send. If it is not possible to send now,
         * reenqueues a sending event.
         * Once all data has been sent, request a new AES block.
         */
        static void sending_handler(void* __this, vp::clock_event* event);

        /**
         * \brief AES key init handler
         *
         * called when a key init event is done. Initializes the AES key.
         */
        static void key_init_handler(void* __this, vp::clock_event* event);

        /** trace utility */
        vp::trace         trace;

        /** AES register map */
        vp_regmap_udma_aes regmap;

        /**
         * event used to mimic the time taken by an AES conversion (encryption/decryption).
         * Duration of the event varies according to key size.
         */
        vp::clock_event* processing_event;

        /**
         * event used to trigger the sending of data to rx channel
         */
        vp::clock_event* sending_event;

        /**
         * event used to initialize AES key. Mimics the time taken by real key initialization.
         */
        vp::clock_event* key_init_event;

        /** queue used to store incoming data while waiting for a complete
         *  block to start processing */
        std::queue<uint8_t> incoming_data;

        /** queue used to store data to send */
        std::queue<uint8_t> outgoing_data;

        /** block currently being processed */
        uint8_t current_block[AES_BLOCK_SIZE_BYTES];
        /** used for CBC mode, starts as the IV (block_rst) */
        uint8_t previous_block[AES_BLOCK_SIZE_BYTES];

        /** Internal AES model context */
        AES_ctx aes_ctx;

        /** AES RX channel, used to transmit data from the AES */
        Aes_rx_channel *rx_channel;

        /** AES TX channel, used to transmit data to the AES */
        Aes_tx_channel *tx_channel;
};


#endif
