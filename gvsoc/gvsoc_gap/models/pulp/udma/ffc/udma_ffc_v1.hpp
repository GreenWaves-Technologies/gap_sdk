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

#ifndef __PULP_UDMA_UDMA_FFC_V1_HPP__
#define __PULP_UDMA_UDMA_FFC_V1_HPP__

#include <vp/vp.hpp>
#include <archi/chips/gap9_v2/udma_ffc/udma_ffc_regs.h>
#include <archi/chips/gap9_v2/udma_ffc/udma_ffc_regfields.h>
#include <archi/chips/gap9_v2/udma_ffc/udma_ffc_structs.h>
#include <archi/chips/gap9_v2/udma_ffc/udma_ffc_gvsoc.h>
#include "../udma_impl.hpp"


class Ffc_periph;


/**
 * \brief Channel used to transmit data from the FFC
 */
class Ffc_rx_channel : public Udma_rx_channel
{
    public:
        /**
         * \brief FFC RX channel constructor
         *
         * \param top udma core
         * \param periph pointer to ffc peripheral
         * \param name channel name
         */
        Ffc_rx_channel(udma *top, Ffc_periph *periph, string name);

    private:
        /** pointer to ffc peripheral */
        Ffc_periph *periph;
};

/**
 * \brief Channel used to receive data to the FFC
 */
class Ffc_tx_channel : public Udma_tx_channel
{
    public:
        /**
         * \brief FFC TX channel constructor
         *
         * \param top pointer to udma core
         * \param periph pointer to ffc peripheral
         * \param name channel name
         */
        Ffc_tx_channel(udma *top, Ffc_periph *periph, string name);
        /**
         * \brief called when data is given to the channel, forward data to FFC
         *
         * \param data pointer to the data given
         * \param size size in bytes of the data given
         */
        void push_data(uint8_t *data, int size);

    private:
        /** pointer to ffc peripheral */
        Ffc_periph *periph;
};

/**
 * \brief FFC internal state
 */
typedef enum {
    FFC_STATE_IDLE,
    FFC_STATE_CONVERTING,
} ffc_state_e;

/**
 * \brief FFC Float types
 */
typedef enum {
    FFC_FLOAT_FP16      = 0,
    FFC_FLOAT_BFP16     = 1,
    FFC_FLOAT_FP32      = 2,
    FFC_FLOAT_FP32_BIS  = 3,
} ffc_float_e;

/**
 * \brief FFC Fixed point types
 */
typedef enum {
    FFC_FIXED_8  = 0,
    FFC_FIXED_16 = 1,
    FFC_FIXED_24 = 2,
    FFC_FIXED_32 = 3,
} ffc_fixed_e;

/**
 * \brief structure to hold pending elements
 */
typedef struct
{
    uint8_t size;    /*!< size in bytes of data */
    uint32_t data;   /*!< data */
    uint32_t cycles; /*!< latency in cycles */
} Ffc_element;

/**
 * \brief UDMA FFC (Fixed-point/Floating-point Converter) peripheral
 */
class Ffc_periph : public Udma_periph
{

    public:
        /**
         * \brief FFC constructor
         *
         * \param top udma core
         * \param id TODO
         * \param itf_id interface id
         */
        Ffc_periph(udma *top, int id, int itf_id);

        /**
         * \brief used to reset the peripheral
         *
         * \param active whether reset is active or not
         */
        void reset(bool active);

        /**
         * \brief used to process incoming data
         *
         * Convert incoming data and request new data to the tx channel if
         * needed.
         *
         * \param data data to convert
         * \param size size of the data
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
         * \brief rx channel register callback
         *
         * Called when the channel register is modified, update the channel connected
         * to the FFC.
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value value of the request
         * \param is_write flag to indicate whether request is write or not
         */
        void rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief tx channel register callback
         *
         * Called when the channel register is modified, update the channel connected
         * to the FFC.
         *
         * \param reg_offset register offset
         * \param size size of the request
         * \param value value of the request
         * \param is_write flag to indicate whether request is write or not
         */
        void tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

        /**
         * \brief update the internal state machine when an event is received
         *
         * Called when an event is triggered. Update the state machine
         * according to the current state. Depending on the state, it might
         * start or stop the conversion.
         *
         * \param __this pointer to the Ffc_periph object
         * \param event pointer to the event that was triggered
         */
        static void handle_event(void *__this, vp::clock_event *event);

        /**
         * \brief convert a floating point to a fixed point
         *
         * Convert a floating point to a fixed point according to multiple
         * parameters. Then send it to the rx channel.
         *
         * \param src pointer to the value to convert
         * \param fixed_type type of fixed point (ie size)
         * \param float_type type of floating point
         * \param precision fixed point precision (bit to indicate where the
         *                  comma is)
         * \param scale fixed point scale, if the fixed point needs to be shifted
         */
        void convert_to_fixed(uint8_t* src,
                ffc_fixed_e fixed_type,
                ffc_float_e float_type,
                int precision,
                int scale);
        /**
         * \brief convert a fixed point to a floating point
         *
         * Convert a fixed point to a floating point according to multiple
         * parameters. Then send it to the rx channel.
         *
         * \param src pointer to the value to convert
         * \param fixed_type type of fixed point (ie size)
         * \param float_type type of floating point
         * \param precision fixed point precision (bit to indicate where the
         *                  comma is)
         * \param scale fixed point scale, if the fixed point needs to be shifted
         */
        void convert_to_float(uint8_t* src,
                ffc_fixed_e fixed_type,
                ffc_float_e float_type,
                int precision,
                int scale);

        /**
         * \brief helper function to get tx request size
         *
         * \returns the number of bytes to request
         */
        int compute_requested_size(void);

        /**
         * \brief push data in ffc_queue
         *
         * \param data data to push
         * \param size size of the data (in bytes)
         */
        void push_data(uint8_t* data, uint8_t size);

        /**
         * \brief enqueue event if needed
         */
        void enqueue_event(void);

        /**********/
        /* Fields */
        /**********/

        /** trace utility */
        vp::trace         trace;

        /** FFC register map */
        vp_regmap_udma_ffc regmap;

        /** number of data to convert */
        uint64_t convert_num;

        /** current FFC state */
        ffc_state_e state;

        /** internal queue */
        std::deque<Ffc_element> ffc_queue;

        /** event used when a conversion is starting (or stopping) */
        vp::clock_event* event_convert;

        /** FFC RX channel, used to transmit data from the FFC */
        Ffc_rx_channel *rx_channel;

        /** FFC TX channel, used to transmit data to the FFC */
        Ffc_tx_channel *tx_channel;

        /** Busy signal for VCD tracing */
        vp::reg_8 busy;
};


#endif
