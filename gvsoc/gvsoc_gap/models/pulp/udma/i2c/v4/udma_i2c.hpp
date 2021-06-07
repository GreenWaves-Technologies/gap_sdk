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

#ifndef __PULP_UDMA_UDMA_I2C_HPP__
#define __PULP_UDMA_UDMA_I2C_HPP__

#include <udma_i2c/udma_i2c_regfields.h>
#include <udma_i2c/udma_i2c_gvsoc.h>

/*
 * I2C
 */

class I2c_periph;


/**
 * \brief channel used to receive data from I2C link
 */
class I2c_rx_channel : public Udma_rx_channel
{
    public:
        /**
         * \brief i2c RX channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to I2c peripheral
         * \param name name of the channel
         */
        I2c_rx_channel(udma *top, I2c_periph *periph, string name);

    private:
        /** pointer to i2c peripheral */
        I2c_periph *periph;

        /**
         * \brief called when channel is active
         */
        void wait_active_done(void);
};

/**
 * \brief channel used to transmit data to I2C link
 */
class I2c_tx_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief i2c tx channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to i2c peripheral
         * \param name name of the channel
         */
        I2c_tx_channel(udma *top, I2c_periph *periph, string name);

        /**
         * \brief called when data is given to the channel, forwards to TX FSM
         *
         * \param data pointer to the data
         * \param size size of the data in bytes
         */
        void push_data(uint8_t *data, int size);

    private:
        /** pointer to i2c peripheral */
        I2c_periph *periph;
};

/**
 * \brief I2C peripheral
 */
class I2c_periph : public Udma_periph
{
    friend class I2c_rx_channel;
    friend class I2c_tx_channel;

    public:
        /**
         * \brief constructor
         *
         * \param top pointer to udma core
         * \param id TODO
         * \param itf_id interface identifier
         */
        I2c_periph(udma *top, int id, int itf_id);

        /**
         * \brief register custom requests handler
         *
         * \param req request
         * \param offset register offset
         *
         * \returns the status of the request
         */
        vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);

        /**
         * \brief reset the component and its sub-components
         *
         * \param active whether reset is active
         */
        void reset(bool active);

    protected:
        /** I2C interface */
        vp::i2c_master i2c_itf;

    private:
        /** I2C register map */
        vp_regmap_udma_i2c regmap;

        /** TX channel, used to transmit data to I2C TX*/
        I2c_tx_channel *tx_channel;
        /** RX channel, used to receive data from I2C RX*/
        I2c_rx_channel *rx_channel;

        /** trace utility */
        vp::trace trace;
};

#endif
