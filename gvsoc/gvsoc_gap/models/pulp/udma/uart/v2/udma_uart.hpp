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

#ifndef __PULP_UDMA_UDMA_UART_V2_HPP__
#define __PULP_UDMA_UDMA_UART_V2_HPP__

#include <udma_uart/udma_uart_regfields.h>
#include <udma_uart/udma_uart_gvsoc.h>

#include "udma_uart_rx.hpp"
#include "udma_uart_tx.hpp"

/*
 * UART
 */

class Uart_periph;


/**
 * \brief channel used to receive data from UART link
 */
class Uart_rx_channel : public Udma_rx_channel
{
    public:
        /**
         * \brief uart RX channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to Uart peripheral
         * \param name name of the channel
         */
        Uart_rx_channel(udma *top, Uart_periph *periph, string name);

    private:
        /** pointer to uart peripheral */
        Uart_periph *periph;

        /**
         * \brief called when channel is active
         */
        void wait_active_done(void);
};

/**
 * \brief channel used to transmit data to UART link
 */
class Uart_tx_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief uart tx channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to uart peripheral
         * \param name name of the channel
         */
        Uart_tx_channel(udma *top, Uart_periph *periph, string name);

        /**
         * \brief called when data is given to the channel, forwards to TX FSM
         *
         * \param data pointer to the data
         * \param size size of the data in bytes
         */
        void push_data(uint8_t *data, int size);

    private:
        /** pointer to uart peripheral */
        Uart_periph *periph;
};

/**
 * \brief UART peripheral
 */
class Uart_periph : public Udma_periph
{
    friend class Uart_rx_channel;
    friend class Uart_tx_channel;
    friend class Uart_rx_fsm;
    friend class Uart_tx_fsm;

    public:
        /**
         * \brief constructor
         *
         * \param top pointer to udma core
         * \param id TODO
         * \param itf_id interface identifier
         */
        Uart_periph(udma *top, int id, int itf_id);

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
        /** UART interface */
        vp::uart_master uart_itf;

    private:
        /**
         * \brief RX_DEST register handler
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value pointer to the value
         * \param is_write whether the request is a write or a read
         */
        void rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief TX_DEST register handler
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value pointer to the value
         * \param is_write whether the request is a write or a read
         */
        void tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief STATUS register handler
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value pointer to the value
         * \param is_write whether the request is a write or a read
         */
        void status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief ERROR register handler
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value pointer to the value
         * \param is_write whether the request is a write or a read
         */
        void error_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief MISC register handler
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value pointer to the value
         * \param is_write whether the request is a write or a read
         */
        void misc_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief called when pin values change
         *
         * \param __this pointer to the object
         * \param data value of the RX pin
         * \param sck value of the CLK pin (USART mode)
         * \param cts value of the CTS pin (control flow mode)
         */
        static void rx_sync(void *__this, int data, int sck, int cts);

        /**
         * \brief sends a bit to the interface
         *
         * \param bit value of the bit to send
         */
        void send_bit(int bit);

        /**
         * \brief send rts to the interface
         *
         * \param rts desired value of the RTS pin
         */
        void send_rts(int rts);

        /**
         * \brief update all the pin values on the interface
         */
        void sync_pins(void);

        /** value of the TX data pin */
        int tx_bit;
        /** value of the RX RTS pin */
        int rx_rts;

        /** UART register map */
        vp_regmap_udma_uart regmap;

        /** TX channel, used to transmit data to UART TX*/
        Uart_tx_channel *tx_channel;
        /** RX channel, used to receive data from UART RX*/
        Uart_rx_channel *rx_channel;

        /** TX FSM */
        Uart_tx_fsm tx_fsm;
        /** RX fsm */
        Uart_rx_fsm rx_fsm;

        /** trace utility */
        vp::trace trace;

        /** error soc_event */
        int error_soc_event;

        /** rx soc event */
        int rx_soc_event;

        /** tx soc event */
        int tx_soc_event;
};

#endif
