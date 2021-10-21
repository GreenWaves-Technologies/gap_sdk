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
#include <map>
#include <functional>

typedef enum
{
    CMD_MISC_NOP                 = 0x00,
    CMD_MISC_WAIT                = 0x01,
    CMD_MISC_REPEAT              = 0x02,
    CMD_MISC_WAIT_I2C_PERIOD_END = 0x03,
    CMD_TIMING                   = 0x10,
    CMD_FOLL_ADDR                = 0x20,
    CMD_LEAD_START               = 0x30,
    CMD_LEAD_SEND                = 0x31,
    CMD_LEAD_SEND_IMM            = 0x32,
    CMD_LEAD_SEND_IMM_ADDR       = 0x37,
    CMD_LEAD_RECV                = 0x33,
    CMD_LEAD_RECV_LAST           = 0x34,
    CMD_LEAD_STOP                = 0x36,
    CMD_EVENT_RECV               = 0x40,
    CMD_EVENT_SEND               = 0x41,
    CMD_UDMA_TX_CHAN_CFG         = 0x50,
    CMD_UDMA_RX_CHAN_CFG         = 0x51,
    CMD_NOT_IMPLEMENTED,
} I2c_ucode_ids;

/**
 * \brief TODO
 */
typedef struct
{
    I2c_ucode_ids id;
    union {
        struct {
            int div_prescaler;
            int delay_low;
            int delay_high;
        } timings;
        struct {
            int register_address;
        } tx_chan_cfg;
        struct {
            int register_address;
        } rx_chan_cfg;
        struct {
            int counter;
        } repeat;
        struct {
            int slot_index;
            int address;
            int mask;
            bool enable_7bits;
            bool enable_10bits;
            bool enable_sof_rcv;
            bool enable_eof_rcv;
            bool enable_sof_snd;
            bool enable_eof_snd;
            bool enable_push;
        } foll_addr;
        struct {
            int byte;
        } lead_send_imm;
        struct {
            bool is_10bits;
            int address;
        } lead_send_imm_addr;
        struct {
            int event_index;
        } event_recv;
    } value;
} ucode_data_t;

/** TODO */
typedef std::function<void(ucode_data_t)> ucode_callback_t;

/** TODO */
typedef std::function<ucode_data_t(uint32_t)> ucode_decoder_t;

/**
 * \brief TODO
 *
 * this class handles ucode decoding and give a structure back to its
 * registered callback for easy processing
 */
class I2c_ucode
{
    public:
        /**
         * \brief TODO
         */
        I2c_ucode(void);

        /**
         * \brief TODO
         */
        bool decode(uint32_t ucode);

        /**
         * \brief TODO
         */
        void register_callback(ucode_callback_t callback);
    private:
        /** map link ids to decoders and callbacks */
        std::map<uint8_t, ucode_decoder_t> id_parsers;

        /** general callback */
        ucode_callback_t callback;
};
