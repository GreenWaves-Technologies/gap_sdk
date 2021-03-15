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


class Aes_periph;


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
         * \brief TODO
         *
         * \param data TODO
         * \param size TODO
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
         * \brief TODO
         *
         * \param active TODO
         */
        void reset(bool active);

    private:
        /**
         * \brief TODO
         *
         * \param req TODO
         * \param offset TODO
         *
         * \returns TODO
         */
        vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);

        /**
         * \brief TODO
         *
         * \param reg_offset TODO
         * \param size TODO
         * \param value TODO
         * \param is_write TODO
         */
        void dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        vp::trace         trace;
        vp_regmap_udma_aes regmap;

        /** AES RX channel, used to transmit data from the AES */
        Aes_rx_channel *rx_channel;

        /** AES TX channel, used to transmit data to the AES */
        Aes_tx_channel *tx_channel;
};


#endif
