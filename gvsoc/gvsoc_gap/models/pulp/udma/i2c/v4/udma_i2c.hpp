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
#include <udma_i2c/udma_i2c_macros.h>

#include "udma_i2c_ucode.hpp"
#include "i2c_helper.hpp"
#include "udma_i2c_fifos.hpp"

/*
 * I2C */

class I2c_periph;


/**
 * \brief channel used to receive data from I2C link
 */
class I2c_lead_rx_channel : public Udma_rx_channel
{
    public:
        /**
         * \brief i2c RX channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to I2c peripheral
         * \param name name of the channel
         */
        I2c_lead_rx_channel(udma *top, I2c_periph *periph, string name);

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
class I2c_lead_tx_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief i2c tx channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to i2c peripheral
         * \param name name of the channel
         */
        I2c_lead_tx_channel(udma *top, I2c_periph *periph, string name);

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
 * \brief channel used to receive data from I2C link
 */
class I2c_foll_rx_channel : public Udma_rx_channel
{
    public:
        /**
         * \brief i2c RX channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to I2c peripheral
         * \param name name of the channel
         */
        I2c_foll_rx_channel(udma *top, I2c_periph *periph, string name);

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
class I2c_foll_tx_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief i2c tx channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to i2c peripheral
         * \param name name of the channel
         */
        I2c_foll_tx_channel(udma *top, I2c_periph *periph, string name);

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
 * \brief channel used to transmit data to I2C link
 */
class I2c_cmd_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief i2c cmd channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to i2c peripheral
         * \param name name of the channel
         */
        I2c_cmd_channel(udma *top, I2c_periph *periph, string name);

        /**
         * \brief called when data is given to the channel
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
    friend class I2c_lead_rx_channel;
    friend class I2c_lead_tx_channel;
    friend class I2c_foll_rx_channel;
    friend class I2c_foll_tx_channel;
    friend class I2c_cmd_channel;

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
         * \brief TODO
         */
        void push_cmd(uint8_t* data, int size);

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

        //TODO
        static void st_delayed_start_event_handler(void* __this, vp::clock_event* event);

        /**
         * \brief TODO
         */
        void status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief TODO
         */
        void cmd_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief TODO
         */
        void lead_rx_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief TODO
         */
        void lead_tx_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief TODO
         */
        static void i2c_sync(void *__this, int scl, int sda);

        /**
         * \brief decoded ucode handler
         *
         * TODO
         */
        void ucode_handler(ucode_data_t data);

        //TODO
        void i2c_enqueue_event(vp::clock_event* event, uint64_t time_ps);

        //TODO
        void i2c_cancel_event(vp::clock_event* event);

        //TODO
        void i2c_helper_callback(i2c_operation_e id, i2c_status_e status, int value);

        //TODO
        void process_command(void);

        void lead_purge(void);

        //TODO
        bool is_waiting_i2c_operation(void);

        //TODO
        bool is_in_error_mode(void);

        //TODO
        void i2c_start(void);

        /** delayed start event */
        vp::clock_event delayed_start_event;

        /** I2C register map */
        vp_regmap_udma_i2c regmap;

        /** TX channel, used to transmit data to I2C TX*/
        I2c_lead_tx_channel lead_tx_channel;
        Pop_fifo lead_tx_fifo;
        /** RX channel, used to receive data from I2C RX*/
        I2c_lead_rx_channel lead_rx_channel;
        Push_fifo lead_rx_fifo;

        /** TX channel, used to transmit data to I2C TX*/
        I2c_foll_tx_channel *foll_tx_channel;
        /** RX channel, used to receive data from I2C RX*/
        I2c_foll_rx_channel *foll_rx_channel;

        /** CMD channel, used to get micro-code */
        I2c_cmd_channel cmd_channel;
        Pop_fifo cmd_fifo;

        /* flags for i2c operations */
        bool is_waiting_i2c_start;
        bool is_waiting_i2c_data;
        bool is_waiting_i2c_ack;
        bool is_waiting_i2c_stop;
        bool is_waiting_i2c_stop_to_start;

        /** repeat downcounter */
        int32_t repeat_downcounter;

        /** ucode decoder */
        I2c_ucode ucode_decoder;

        /** i2c helper */
        I2C_helper i2c_helper;

        /** trace utility */
        vp::trace trace;

        /* TODO doc
         * indicate when there is a tx/rx request */
        bool is_tx_write;
        int tx_write_offset;

        bool is_rx_write;
        int rx_write_offset;

        /** leader event number */
        int leader_event_number;
        /** follower event number */
        int follower_event_number;
};

#endif
