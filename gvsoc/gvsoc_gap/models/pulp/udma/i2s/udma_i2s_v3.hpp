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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __PULP_UDMA_I2S_UDMA_I2S_V3_HPP__
#define __PULP_UDMA_I2S_UDMA_I2S_V3_HPP__


#include <vp/vp.hpp>
#include "../udma_impl.hpp"
#include <udma_i2s/udma_i2s_regs.h>
#include <udma_i2s/udma_i2s_regfields.h>
#include <udma_i2s/udma_i2s_gvsoc.h>
#include <vp/itf/wire.hpp>


#define I2S_NB_PDM_IN  4
#define I2S_NB_PDM_OUT 2

class I2s_periph;


class I2s_rx_channel : public Udma_rx_channel
{
public:
    I2s_rx_channel(udma *top, I2s_periph *periph, int id, string name);
    void reset(bool active);
    void update_word_size();

    vp_udma_i2s_slot_cfg_0 *slot_cfg;
    int word_size;

private:
    I2s_periph *periph;

    int id;
};


class I2s_tx_channel : public Udma_tx_channel
{
public:
    I2s_tx_channel(udma *top, I2s_periph *periph, int id, string name);
    void reset(bool active);
    void update_word_size();
    void push_data(uint8_t *data, int size);

    vp_udma_i2s_slot_cfg_0 *slot_cfg;
    int word_size;

private:
    I2s_periph *periph;

    int id;
};


class I2s_periph : public Udma_periph
{
    friend class I2s_rx_channel;
    friend class I2s_tx_channel;

public:
    I2s_periph(udma *top, int id, int itf_id);
    vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
    void reset(bool active);

protected:

private:

    static void clk_in_sync(void *__this, int value);
    static void ws_in_sync(void *__this, int value);
    void handle_clk_edge();
    uint32_t handle_tx_format(I2s_tx_channel *channel, uint32_t sample);
    uint32_t handle_rx_format(I2s_rx_channel *channel, uint32_t sample);
    static void handle_clk(void *__this, vp::clock_event *event);
    static void handle_rx_fifo(void *__this, vp::clock_event *event);
    static void rx_sync(void *, int sck, int ws, int sd);
    void err_status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void glb_setup_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int slot_id);
    void slot_cfg_0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_3_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_4_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_5_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_6_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_7_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_8_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_9_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_10_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_11_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_12_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_13_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_14_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_cfg_15_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_0_1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_2_3_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_4_5_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_6_7_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_8_9_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_10_11_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_12_13_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void word_size_14_15_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void slot_en_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void check_state();
    void handle_clk_ws_update();
    void start_frame();

    vp::trace trace;
    vp::wire_master<int> clk_out_itf;
    vp::wire_slave<int> clk_in_itf;
    vp::wire_master<int> ws_out_itf;
    vp::wire_slave<int> ws_in_itf;
    vp::wire_master<int> pdm_in_itf[I2S_NB_PDM_IN];
    vp::wire_master<int> pdm_out_itf[I2S_NB_PDM_OUT];
    vp_regmap_udma_i2s regmap;
    vp::i2s_slave i2s_itf;
    vp::clock_event *clk_event;
    vp::clock_event *rx_fifo_event;
    int clk_value;
    int ws_value;
    int ws_count;
    int ws_delay;
    uint32_t rx_pending_value;
    uint32_t rx_sync_value;
    int rx_pending_bits;
    int tx_pending_bits;
    uint32_t tx_pending_value;
    std::vector<I2s_rx_channel *> rx_channels;
    std::vector<I2s_tx_channel *> tx_channels;
    int active_channel;
    Udma_fifo<uint32_t> *rx_fifo;
    Udma_fifo<uint32_t> *rx_fifo_slot_id;
    std::queue<uint32_t> tx_fifo;
    std::queue<int> tx_fifo_slot_id;
    uint32_t slot_en;
    uint32_t slot_en_sync;
    uint32_t global_en;
    uint32_t global_en_sync;
    int prev_ws_value;
    int prev_sck_value;
    int current_ws_delay;
    int sd;
    uint32_t tx_wait_data_init;
};

#endif
