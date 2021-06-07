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

#include "udma_i2s_v3.hpp"
#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/i2s.hpp"

using namespace std::placeholders;


I2s_periph::I2s_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "i2s" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &trace, vp::DEBUG);

    this->i2s_itf.set_sync_meth(&I2s_periph::rx_sync);

    top->new_slave_port(this, "i2s" + std::to_string(itf_id), &this->i2s_itf);

    for (int i=0; i<I2S_NB_PDM_IN; i++)
    {
        top->new_master_port(this, "i2s" + std::to_string(itf_id) + "_pdm_in_" + std::to_string(i), &this->pdm_in_itf[i]);
    }

    for (int i=0; i<I2S_NB_PDM_OUT; i++)
    {
        top->new_master_port(this, "i2s" + std::to_string(itf_id) + "_pdm_out_" + std::to_string(i), &this->pdm_out_itf[i]);
    }

    top->new_master_port(itf_name + "_clk_out", &this->clk_out_itf);
    top->new_master_port(itf_name + "_ws_out", &this->ws_out_itf);

    this->clk_in_itf.set_sync_meth(&I2s_periph::clk_in_sync);
    top->new_slave_port(this, itf_name + "_clk_in", &this->clk_in_itf);
    this->ws_in_itf.set_sync_meth(&I2s_periph::ws_in_sync);
    top->new_slave_port(this, itf_name + "_ws_in", &this->ws_in_itf);

    this->regmap.build(top, &this->trace, itf_name);
    this->regmap.err_status.register_callback(std::bind(&I2s_periph::err_status_req, this, _1, _2, _3, _4));
    this->regmap.glb_setup.register_callback(std::bind(&I2s_periph::glb_setup_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_0.register_callback(std::bind(&I2s_periph::slot_cfg_0_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_1.register_callback(std::bind(&I2s_periph::slot_cfg_1_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_2.register_callback(std::bind(&I2s_periph::slot_cfg_2_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_3.register_callback(std::bind(&I2s_periph::slot_cfg_3_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_4.register_callback(std::bind(&I2s_periph::slot_cfg_4_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_5.register_callback(std::bind(&I2s_periph::slot_cfg_5_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_6.register_callback(std::bind(&I2s_periph::slot_cfg_6_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_7.register_callback(std::bind(&I2s_periph::slot_cfg_7_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_8.register_callback(std::bind(&I2s_periph::slot_cfg_8_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_9.register_callback(std::bind(&I2s_periph::slot_cfg_9_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_10.register_callback(std::bind(&I2s_periph::slot_cfg_10_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_11.register_callback(std::bind(&I2s_periph::slot_cfg_11_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_12.register_callback(std::bind(&I2s_periph::slot_cfg_12_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_13.register_callback(std::bind(&I2s_periph::slot_cfg_13_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_14.register_callback(std::bind(&I2s_periph::slot_cfg_14_req, this, _1, _2, _3, _4));
    this->regmap.slot_cfg_15.register_callback(std::bind(&I2s_periph::slot_cfg_15_req, this, _1, _2, _3, _4));
    this->regmap.word_size_0_1.register_callback(std::bind(&I2s_periph::word_size_0_1_req, this, _1, _2, _3, _4));
    this->regmap.word_size_2_3.register_callback(std::bind(&I2s_periph::word_size_2_3_req, this, _1, _2, _3, _4));
    this->regmap.word_size_4_5.register_callback(std::bind(&I2s_periph::word_size_4_5_req, this, _1, _2, _3, _4));
    this->regmap.word_size_6_7.register_callback(std::bind(&I2s_periph::word_size_6_7_req, this, _1, _2, _3, _4));
    this->regmap.word_size_8_9.register_callback(std::bind(&I2s_periph::word_size_8_9_req, this, _1, _2, _3, _4));
    this->regmap.word_size_10_11.register_callback(std::bind(&I2s_periph::word_size_10_11_req, this, _1, _2, _3, _4));
    this->regmap.word_size_12_13.register_callback(std::bind(&I2s_periph::word_size_12_13_req, this, _1, _2, _3, _4));
    this->regmap.word_size_14_15.register_callback(std::bind(&I2s_periph::word_size_14_15_req, this, _1, _2, _3, _4));
    this->regmap.slot_en.register_callback(std::bind(&I2s_periph::slot_en_req, this, _1, _2, _3, _4));

    this->clk_event = top->event_new(this, I2s_periph::handle_clk);
    this->rx_fifo_event = top->event_new(this, I2s_periph::handle_rx_fifo);

    for (int i=0; i<16; i++)
    {
        this->rx_channels.push_back(new I2s_rx_channel(top, this, i, itf_name + "rx_slot_" + std::to_string(i)));
        this->tx_channels.push_back(new I2s_tx_channel(top, this, i, itf_name + "tx_slot_" + std::to_string(i)));
    }

    this->rx_fifo = new Udma_fifo<uint32_t>(4);
    this->rx_fifo_slot_id = new Udma_fifo<uint32_t>(4);
}


void I2s_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->regmap.reset(active);

    while (!this->tx_fifo.empty()) { this->tx_fifo.pop(); }
    while (!this->tx_fifo_slot_id.empty()) { this->tx_fifo_slot_id.pop(); }

    for (auto x: this->rx_channels)
    {
        x->reset(active);
    }

    for (auto x: this->tx_channels)
    {
        x->reset(active);
    }

    if (active)
    {
        this->sd = (2 << 2) | 2;
        this->clk_value = 0;
        this->ws_count = 0;
        this->ws_value = 0;
        this->slot_en = 0;
        this->slot_en_sync = 0;
        this->prev_ws_value = 0;
        this->prev_sck_value = 0;
        this->rx_pending_bits = 0;
        this->rx_pending_value = 0;
    }
}


vp::io_req_status_e I2s_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}


// This function is called as soon as RX fifo is not empty to propagate the samples to the udma channel
void I2s_periph::handle_rx_fifo(void *__this, vp::clock_event *event)
{
    I2s_periph *_this = (I2s_periph *)__this;
    int slot_id = _this->rx_fifo_slot_id->get_first();
    I2s_rx_channel *channel = _this->rx_channels[slot_id];

    // First the channel must be ready, i.e. ready to accept one sample
    if (channel->is_ready())
    {
        uint32_t sample = _this->rx_fifo->pop();
        _this->rx_fifo_slot_id->pop();

        // Then it should also be active (have an active transfer), otherwise the sample is droppped
        // and an error is generated
        if (channel->is_active())
        {
            channel->push_data((uint8_t *)&sample, channel->slot_cfg->rx_dsize_get() + 1, channel->slot_cfg->id_rx_get());
        }
        else
        {
            _this->regmap.err_status.set(_this->regmap.err_status.get() | (1 << slot_id));
        }
    }
}


void I2s_periph::start_frame()
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting frame\n");

    this->tx_wait_data_init |= (~this->slot_en) & this->slot_en_sync;

    uint32_t slot_disabled = this->slot_en & (~this->slot_en_sync);
    
    std::queue<uint32_t> new_tx_fifo;
    std::queue<int> new_tx_fifo_slot_id;

    while(!this->tx_fifo_slot_id.empty())
    {
        int slot_id = this->tx_fifo_slot_id.front();
        this->tx_fifo_slot_id.pop();
        if (((slot_disabled >> slot_id) & 1) == 0)
        {
            if (!this->tx_fifo.empty())
                new_tx_fifo.push(this->tx_fifo.front());
            new_tx_fifo_slot_id.push(slot_id);
        }
        if (!this->tx_fifo.empty())
        {
            this->tx_fifo.pop();
        }
    }

    this->tx_fifo.swap(new_tx_fifo);
    this->tx_fifo_slot_id.swap(new_tx_fifo_slot_id);

    this->slot_en = this->slot_en_sync;
    this->global_en = this->global_en_sync;
    this->tx_pending_bits = 0;
    this->active_channel = 0;
    this->rx_pending_bits = this->regmap.glb_setup.slot_width_get() + 1;
}


void I2s_periph::handle_clk_ws_update()
{
    if (this->clk_value == 1)
    {
        if (this->prev_ws_value == 0 && this->ws_value == 1)
        {
            this->current_ws_delay = this->regmap.glb_setup.ws_delay_get();
            if (this->current_ws_delay == 0)
            {
                this->start_frame();
            }
        }

        this->prev_ws_value = this->ws_value;
    }
    else if (this->clk_value == 0)
    {
        if (this->current_ws_delay > 0)
        {
            this->current_ws_delay--;

            if (this->current_ws_delay == 0)
            {
                this->start_frame();
            }
        }

        int sdo = 2;
        I2s_tx_channel *channel = this->tx_channels[this->active_channel];

        if ((this->slot_en >> this->active_channel) & 1)
        {
            if (channel->slot_cfg->tx_en_get() && !channel->slot_cfg->byp_get())
            {
                if (this->tx_pending_bits == 0)
                {
                    this->tx_pending_bits = this->regmap.glb_setup.slot_width_get() + 1;

                    // Get sample from FIFO
                    uint32_t data = 0;
                    
                    // In case the slot has been enabled just before, it's possible that there is no sample
                    // in the FIFO for this slot, in this case, just take 0 as sample
                    if (!this->tx_fifo.empty() && this->tx_fifo_slot_id.front() == this->active_channel)
                    {
                        this->tx_wait_data_init &= ~(1 << this->active_channel);
                        data = this->tx_fifo.front();
                        this->tx_fifo.pop();
                        this->tx_fifo_slot_id.pop();
                    }
                    else
                    {
                        if (((this->tx_wait_data_init >> this->active_channel) & 1) == 0)
                        {
                            this->regmap.err_status.set(this->regmap.err_status.get() | (1 << (this->active_channel + 16)));
                        }
                    }

                    this->tx_pending_value = this->handle_tx_format(channel, data);

                    // Now ask the next sample to the channel so that it is ready for the
                    // next frame.
                    // The channel id is pushed now since we the samples are received in order
                    // and we don't know what is the slot when we receive the sample from the channel

                    this->tx_fifo_slot_id.push(this->active_channel);
                    channel->get_data(channel->slot_cfg->tx_dsize_get() + 1);
                }
                sdo = (this->tx_pending_value >> (this->regmap.glb_setup.slot_width_get() + 1 - 1)) & 1;
                this->tx_pending_value <<= 1;
                this->tx_pending_bits--;
            }
        }

        if (this->regmap.glb_setup.full_duplex_en_get())
        {
            this->sd = (this->sd & 0x3) | (sdo << 2);
        }
        else
        {
            this->sd = sdo;
        }

    }
}


uint32_t I2s_periph::handle_tx_format(I2s_tx_channel *channel, uint32_t sample)
{
    int global_width = this->regmap.glb_setup.slot_width_get() + 1;
    int msb_first = channel->slot_cfg->tx_msb_first_get();
    int left_align = channel->slot_cfg->tx_left_align_get();
    int sign_extend = channel->slot_cfg->tx_sign_get();
    int dummy_cycles = global_width - channel->word_size;

    sample = sample & (1ULL << channel->word_size) - 1;

    if (sign_extend)
    {
        sample = ((int32_t)sample) << (32 - channel->word_size) >> (32 - channel->word_size);
    }

    if (dummy_cycles)
    {
        if (msb_first)
        {
            if (left_align)
            {
                sample <<= dummy_cycles;
            }
            else
            {
            }
        }
        else
        {
            if (left_align)
            {
                uint32_t value = 0;
                for (int i=0; i<global_width; i++)
                {
                    value = (value << 1) | (sample & 1);
                    sample >>= 1;
                }
                sample = value;
            }
            else
            {
                uint32_t value = 0;
                for (int i=0; i<channel->word_size; i++)
                {
                    value = (value << 1) | (sample & 1);
                    sample >>= 1;
                }
                sample = value;
            }
        }
    }
    else
    {
        if (!msb_first)
        {
            uint32_t value = 0;
            for (int i=0; i<channel->word_size; i++)
            {
                value = (value << 1) | (sample & 1);
                sample >>= 1;
            }
            sample = value;
        }
    }

    return sample;
}


uint32_t I2s_periph::handle_rx_format(I2s_rx_channel *channel, uint32_t sample)
{
    int global_width = this->regmap.glb_setup.slot_width_get() + 1;
    int msb_first = channel->slot_cfg->rx_msb_first_get();
    int left_align = channel->slot_cfg->rx_left_align_get();
    int sign_extend = channel->slot_cfg->rx_sign_get();
    int dummy_cycles = global_width - channel->word_size;

    if (dummy_cycles)
    {
        if (msb_first)
        {
            if (left_align)
            {
                sample >>= dummy_cycles;
            }
            else
            {
            }
        }
        else
        {
            if (left_align)
            {
                uint32_t value = 0;
                for (int i=0; i<global_width; i++)
                {
                    value = (value << 1) | (sample & 1);
                    sample >>= 1;
                }
                sample = value;
            }
            else
            {
                uint32_t value = 0;
                for (int i=0; i<channel->word_size; i++)
                {
                    value = (value << 1) | (sample & 1);
                    sample >>= 1;
                }
                sample = value;
            }
        }
    }
    else
    {
        if (!msb_first)
        {
            uint32_t value = 0;
            for (int i=0; i<channel->word_size; i++)
            {
                value = (value << 1) | (sample & 1);
                sample >>= 1;
            }
            sample = value;
        }
    }

    if (sign_extend)
    {
        sample = ((int32_t)sample) << (32 - channel->word_size) >> (32 - channel->word_size);
    }

    return sample;
}


void I2s_periph::handle_clk_edge()
{
    if (this->global_en && this->clk_value <= 1 && this->prev_sck_value != this->clk_value)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "Clk edge (sck: %d, ws: %d, sd: 0x%x)\n", this->clk_value, this->ws_value, this->rx_sync_value);

        if (this->regmap.glb_setup.pdm_en_get())
        {
            this->prev_sck_value = this->clk_value;

            int pdm_pol = this->regmap.glb_setup.pdm_pol_get();
            int pdm_out_0 = 2, pdm_out_1 = 2;

            if ((pdm_pol >> 0) & 1)
            {
                if (this->clk_value)
                {
                    if  (this->pdm_in_itf[0].is_bound())
                    {
                        this->pdm_in_itf[0].sync((this->rx_sync_value >> 0) & 1);
                    }
                }
                else
                {
                    if  (this->pdm_in_itf[1].is_bound())
                    {
                        this->pdm_in_itf[1].sync((this->rx_sync_value >> 0) & 1);
                    }
                }
            }
            else
            {
                if (this->clk_value)
                {
                    if  (this->pdm_out_itf[0].is_bound())
                    {
                        this->pdm_out_itf[0].sync_back(&pdm_out_0);
                    }
                }
            }

            if ((pdm_pol >> 1) & 1)
            {
                if (this->clk_value)
                {
                    if  (this->pdm_in_itf[2].is_bound())
                    {
                        this->pdm_in_itf[2].sync((this->rx_sync_value >> 2) & 1);
                    }
                }
                else
                {
                    if  (this->pdm_in_itf[3].is_bound())
                    {
                        this->pdm_in_itf[3].sync((this->rx_sync_value >> 2) & 1);
                    }
                }
            }
            else
            {
                if (this->clk_value)
                {
                    if  (this->pdm_out_itf[1].is_bound())
                    {
                        this->pdm_out_itf[1].sync_back(&pdm_out_1);
                    }
                }
            }

            if  (this->i2s_itf.is_bound())
            {
                this->i2s_itf.sync(
                    this->clk_value,
                    2,
                    pdm_out_0 | (pdm_out_1 << 2)
                );
            }

            this->check_state();
        }
        else
        {
            if (this->ws_out_itf.is_bound())
            {
                this->ws_out_itf.sync(this->ws_value);
            }

            if (this->clk_out_itf.is_bound())
            {
                this->clk_out_itf.sync(this->clk_value);
            }

            this->prev_sck_value = this->clk_value;

            // data is sampled on raising edge, and ws is changed on falling edge
            if (this->clk_value == 1)
            {
                unsigned int sdo = (this->rx_sync_value >> 2) & 0x1;
                unsigned int sdi = (this->rx_sync_value) & 0x1;

                // If the WS delay is 0, restart from first slot and start sampling
                if (this->ws_delay == 0)
                {
                    //this->rx_pending_bits = this->regmap.glb_setup.slot_width_get() + 1;
                }

                if (this->rx_pending_bits > 0)
                {
                    this->rx_pending_value = (this->rx_pending_value << 1) | sdi;
                    this->rx_pending_bits--;

                    this->trace.msg(vp::trace::LEVEL_DEBUG, "Appending incoming bit (bit: %d, new_value: 0x%x, remaining_bits: %d)\n", sdi, this->rx_pending_value, this->rx_pending_bits);

                    // In case the sample sampling is over, enqueue the sample and switch to next slot
                    if (this->rx_pending_bits == 0)
                    {
                        if (this->active_channel < 16 && ((this->slot_en >> this->active_channel) & 1))
                        {
                            I2s_rx_channel *channel = this->rx_channels[this->active_channel];

                            // Convert the sample to the specified format
                            uint32_t sample = this->handle_rx_format(channel, this->rx_pending_value);

                            this->trace.msg(vp::trace::LEVEL_DEBUG, "Received new sample (value: 0x%x, formatted_value: 0x%x)\n", this->rx_pending_value, sample);

                            if (channel->slot_cfg->rx_en_get())
                            {
                                // There is an output FIFO for rx samples which takes care of absorbing
                                // L2 contentions. If the FIFO is full, we just drop the sample and generate an error
                                if (this->rx_fifo->is_full())
                                {
                                    this->regmap.err_status.set(this->regmap.err_status.get() | (1 << this->active_channel));
                                }
                                else
                                {
                                    this->rx_fifo->push(sample);
                                    this->rx_fifo_slot_id->push(this->active_channel);
                                }
                            }
                        }

                        this->active_channel++;
                        if (this->active_channel == 16)
                        {
                            this->active_channel = 0;
                        }
                        this->rx_pending_bits = this->regmap.glb_setup.slot_width_get() + 1;
                        this->rx_pending_value = 0;
                    }
                }

                this->handle_clk_ws_update();

            }
            else
            {
                if (this->regmap.clkcfg_setup.ws_src_get() == 0)
                {
                    // WS generation
                    if (this->ws_count == 0)
                    {
                        this->ws_value = 1;
                        this->ws_delay = this->regmap.glb_setup.ws_delay_get();
                    }
                    else
                    {
                        this->ws_value = 0;
                        this->ws_delay--;
                    }

                    this->ws_count++;
                    if (this->ws_count == (this->regmap.glb_setup.slot_width_get() + 1) * (this->regmap.glb_setup.frame_length_get() + 1))
                    {
                        this->ws_count = 0;
                    }
                }


                this->handle_clk_ws_update();

            }

            if  (this->i2s_itf.is_bound())
            {
                this->i2s_itf.sync(
                    this->regmap.clkcfg_setup.clk_src_get() || this->regmap.clkcfg_setup.clk_ext_src_get() ? 2 : this->clk_value ^ this->regmap.clkcfg_setup.clk_edge_get(),
                    this->regmap.clkcfg_setup.ws_src_get() || this->regmap.clkcfg_setup.ws_ext_src_get() ? 2 : this->ws_value ^ this->regmap.clkcfg_setup.ws_edge_get(),
                    this->sd
                );
            }

            this->check_state();
        }
    }
}


void I2s_periph::handle_clk(void *__this, vp::clock_event *event)
{
    I2s_periph *_this = (I2s_periph *)__this;

    _this->clk_value ^= 1;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Updating clock (clk: %d)\n", _this->clk_value);

    _this->handle_clk_edge();
}


void I2s_periph::check_state()
{
    // Check if we should generate the next internal clock edge
    if (this->global_en && (this->regmap.clkcfg_setup.clk_src_get() == 0 || this->regmap.glb_setup.pdm_en_get()) && !this->clk_event->is_enqueued())
    {
        if (this->regmap.clk_fast.fast_en_get())
        {
            this->top->get_fast_clock()->enqueue(this->clk_event, this->regmap.clkcfg_setup.clk_div_get());
        }
        else
        {
            this->top->get_periph_clock_dual_edges()->enqueue(this->clk_event, this->regmap.clkcfg_setup.clk_div_get());
        }
    }

    // Check if we should propagate a sample from rx fifo to the udma channel
    if (!this->rx_fifo_event->is_enqueued() && !this->rx_fifo->is_empty())
    {
        this->top->event_enqueue(this->rx_fifo_event, 1);
    }
}


void I2s_periph::slot_en_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_en.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->slot_en_sync = this->regmap.slot_en.get();
    }
}


void I2s_periph::err_status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.err_status.update(reg_offset, size, value, is_write);

    this->regmap.err_status.set(0);
}


void I2s_periph::glb_setup_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.glb_setup.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        // TODO global enable should be synchronized on start of frame
        this->global_en = this->regmap.glb_setup.global_en_get();
        this->global_en_sync = this->global_en;
        //this->global_en_sync = this->regmap.glb_setup.global_en_get();
//
        //if (this->global_en_sync)
        //{
        //    this->global_en = this->global_en_sync;
        //}
    }

    this->check_state();
}


void I2s_periph::slot_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int slot_id)
{
    I2s_rx_channel *rx_channel = this->rx_channels[slot_id];
    int rx_channel_id = rx_channel->slot_cfg->id_rx_get();
    this->top->channel_register(rx_channel_id, rx_channel);
    I2s_tx_channel *tx_channel = this->tx_channels[slot_id];
    if (!tx_channel->slot_cfg->byp_get())
    {
        int tx_channel_id = tx_channel->slot_cfg->tx_id_get();
        this->top->channel_register(tx_channel_id, tx_channel);
    }
}


void I2s_periph::slot_cfg_0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_0.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 0);
}


void I2s_periph::slot_cfg_1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_1.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 1);
}


void I2s_periph::slot_cfg_2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_2.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 2);
}


void I2s_periph::slot_cfg_3_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_3.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 3);
}


void I2s_periph::slot_cfg_4_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_4.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 4);
}


void I2s_periph::slot_cfg_5_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_5.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 5);
}


void I2s_periph::slot_cfg_6_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_6.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 6);
}


void I2s_periph::slot_cfg_7_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_7.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 7);
}


void I2s_periph::slot_cfg_8_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_8.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 8);
}


void I2s_periph::slot_cfg_9_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_9.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 9);
}


void I2s_periph::slot_cfg_10_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_10.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 10);
}


void I2s_periph::slot_cfg_11_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_11.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 11);
}


void I2s_periph::slot_cfg_12_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_12.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 12);
}


void I2s_periph::slot_cfg_13_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_13.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 13);
}


void I2s_periph::slot_cfg_14_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_14.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 14);
}


void I2s_periph::slot_cfg_15_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.slot_cfg_15.update(reg_offset, size, value, is_write);
    this->slot_cfg_req(reg_offset, size, value, is_write, 15);
}


void I2s_periph::word_size_0_1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_0_1.update(reg_offset, size, value, is_write);
    this->rx_channels[0]->update_word_size();
    this->rx_channels[1]->update_word_size();
    this->tx_channels[0]->update_word_size();
    this->tx_channels[1]->update_word_size();
}


void I2s_periph::word_size_2_3_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_2_3.update(reg_offset, size, value, is_write);
    this->rx_channels[2]->update_word_size();
    this->rx_channels[3]->update_word_size();
    this->tx_channels[2]->update_word_size();
    this->tx_channels[3]->update_word_size();
}


void I2s_periph::word_size_4_5_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_4_5.update(reg_offset, size, value, is_write);
    this->rx_channels[4]->update_word_size();
    this->rx_channels[5]->update_word_size();
    this->tx_channels[4]->update_word_size();
    this->tx_channels[5]->update_word_size();
}


void I2s_periph::word_size_6_7_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_6_7.update(reg_offset, size, value, is_write);
    this->rx_channels[6]->update_word_size();
    this->rx_channels[7]->update_word_size();
    this->tx_channels[6]->update_word_size();
    this->tx_channels[7]->update_word_size();
}


void I2s_periph::word_size_8_9_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_8_9.update(reg_offset, size, value, is_write);
    this->rx_channels[8]->update_word_size();
    this->rx_channels[9]->update_word_size();
    this->tx_channels[8]->update_word_size();
    this->tx_channels[9]->update_word_size();
}


void I2s_periph::word_size_10_11_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_10_11.update(reg_offset, size, value, is_write);
    this->rx_channels[10]->update_word_size();
    this->rx_channels[11]->update_word_size();
    this->tx_channels[10]->update_word_size();
    this->tx_channels[11]->update_word_size();
}


void I2s_periph::word_size_12_13_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_12_13.update(reg_offset, size, value, is_write);
    this->rx_channels[12]->update_word_size();
    this->rx_channels[13]->update_word_size();
    this->tx_channels[12]->update_word_size();
    this->tx_channels[13]->update_word_size();
}


void I2s_periph::word_size_14_15_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.word_size_14_15.update(reg_offset, size, value, is_write);
    this->rx_channels[14]->update_word_size();
    this->rx_channels[15]->update_word_size();
    this->tx_channels[14]->update_word_size();
    this->tx_channels[15]->update_word_size();
}


void I2s_periph::clk_in_sync(void *__this, int value)
{
    I2s_periph *_this = (I2s_periph *)__this;

    if (_this->regmap.clkcfg_setup.clk_ext_src_get())
    {
        _this->clk_value = value;

        _this->handle_clk_edge();
    }
}


void I2s_periph::ws_in_sync(void *__this, int value)
{
    I2s_periph *_this = (I2s_periph *)__this;

    if (_this->regmap.clkcfg_setup.ws_ext_src_get())
    {
        _this->ws_value = value;
    }
}


void I2s_periph::rx_sync(void *__this, int sck, int ws, int sd)
{
    I2s_periph *_this = (I2s_periph *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received data (sck: %d, ws: %d, sd: 0x%x)\n", sck, ws, sd);

    if (_this->regmap.glb_setup.full_duplex_en_get())
    {
        I2s_tx_channel *channel = _this->tx_channels[_this->active_channel];
        if (channel->slot_cfg->byp_get())
        {
            _this->sd = (_this->sd & 0x3) | ((sd & 0x3) << 2);
        }
    }

    // Just store the incoming data, it will be sampled with the clock
    _this->rx_sync_value = sd;

    if (_this->regmap.clkcfg_setup.ws_src_get())
    {
        _this->ws_value = ws ^ _this->regmap.clkcfg_setup.ws_edge_get();
    }

    if (_this->regmap.clkcfg_setup.clk_src_get())
    {
        _this->clk_value = sck ^ _this->regmap.clkcfg_setup.clk_edge_get();
        _this->handle_clk_edge();
    }

}


I2s_tx_channel::I2s_tx_channel(udma *top, I2s_periph *periph, int id, string name) : Udma_tx_channel(top, name), periph(periph), id(id)
{
    switch (id)
    {
        case 0:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_0; break;
        case 1:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_1; break;
        case 2:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_2; break;
        case 3:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_3; break;
        case 4:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_4; break;
        case 5:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_5; break;
        case 6:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_6; break;
        case 7:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_7; break;
        case 8:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_8; break;
        case 9:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_9; break;
        case 10: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_10; break;
        case 11: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_11; break;
        case 12: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_12; break;
        case 13: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_13; break;
        case 14: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_14; break;
        case 15: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_15; break;
    }
}


void I2s_tx_channel::push_data(uint8_t *data, int size)
{
    uint32_t value = 0;
    memcpy((void *)&value, (void *)data, size);

    this->periph->tx_fifo.push(value);
}


void I2s_tx_channel::reset(bool active)
{
    Udma_tx_channel::reset(active);
}


void I2s_tx_channel::update_word_size()
{
    switch (this->id)
    {
        case  0: this->word_size = this->periph->regmap.word_size_0_1.word_size_tx_0_get() + 1; break;
        case  1: this->word_size = this->periph->regmap.word_size_0_1.word_size_tx_1_get() + 1; break;
        case  2: this->word_size = this->periph->regmap.word_size_2_3.word_size_tx_0_get() + 1; break;
        case  3: this->word_size = this->periph->regmap.word_size_2_3.word_size_tx_1_get() + 1; break;
        case  4: this->word_size = this->periph->regmap.word_size_4_5.word_size_tx_0_get() + 1; break;
        case  5: this->word_size = this->periph->regmap.word_size_4_5.word_size_tx_1_get() + 1; break;
        case  6: this->word_size = this->periph->regmap.word_size_6_7.word_size_tx_0_get() + 1; break;
        case  7: this->word_size = this->periph->regmap.word_size_6_7.word_size_tx_1_get() + 1; break;
        case  8: this->word_size = this->periph->regmap.word_size_8_9.word_size_tx_0_get() + 1; break;
        case  9: this->word_size = this->periph->regmap.word_size_8_9.word_size_tx_1_get() + 1; break;
        case 10: this->word_size = this->periph->regmap.word_size_10_11.word_size_tx_0_get() + 1; break;
        case 11: this->word_size = this->periph->regmap.word_size_10_11.word_size_tx_1_get() + 1; break;
        case 12: this->word_size = this->periph->regmap.word_size_12_13.word_size_tx_0_get() + 1; break;
        case 13: this->word_size = this->periph->regmap.word_size_12_13.word_size_tx_1_get() + 1; break;
        case 14: this->word_size = this->periph->regmap.word_size_14_15.word_size_tx_0_get() + 1; break;
        case 15: this->word_size = this->periph->regmap.word_size_14_15.word_size_tx_1_get() + 1; break;
    }
}


I2s_rx_channel::I2s_rx_channel(udma *top, I2s_periph *periph, int id, string name) : Udma_rx_channel(top, name), periph(periph), id(id)
{
    switch (id)
    {
        case 0:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_0; break;
        case 1:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_1; break;
        case 2:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_2; break;
        case 3:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_3; break;
        case 4:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_4; break;
        case 5:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_5; break;
        case 6:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_6; break;
        case 7:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_7; break;
        case 8:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_8; break;
        case 9:  this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_9; break;
        case 10: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_10; break;
        case 11: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_11; break;
        case 12: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_12; break;
        case 13: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_13; break;
        case 14: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_14; break;
        case 15: this->slot_cfg = (vp_udma_i2s_slot_cfg_0 *)&periph->regmap.slot_cfg_15; break;
    }
}


void I2s_rx_channel::reset(bool active)
{
    Udma_rx_channel::reset(active);
}


void I2s_rx_channel::update_word_size()
{
    switch (this->id)
    {
        case 0: this->word_size = this->periph->regmap.word_size_0_1.word_size_rx_0_get() + 1; break;
        case 1: this->word_size = this->periph->regmap.word_size_0_1.word_size_rx_1_get() + 1; break;
        case 2: this->word_size = this->periph->regmap.word_size_2_3.word_size_rx_0_get() + 1; break;
        case 3: this->word_size = this->periph->regmap.word_size_2_3.word_size_rx_1_get() + 1; break;
        case 4: this->word_size = this->periph->regmap.word_size_4_5.word_size_rx_0_get() + 1; break;
        case 5: this->word_size = this->periph->regmap.word_size_4_5.word_size_rx_1_get() + 1; break;
        case 6: this->word_size = this->periph->regmap.word_size_6_7.word_size_rx_0_get() + 1; break;
        case 7: this->word_size = this->periph->regmap.word_size_6_7.word_size_rx_1_get() + 1; break;
        case 8: this->word_size = this->periph->regmap.word_size_8_9.word_size_rx_0_get() + 1; break;
        case 9: this->word_size = this->periph->regmap.word_size_8_9.word_size_rx_1_get() + 1; break;
        case 10: this->word_size = this->periph->regmap.word_size_10_11.word_size_rx_0_get() + 1; break;
        case 11: this->word_size = this->periph->regmap.word_size_10_11.word_size_rx_1_get() + 1; break;
        case 12: this->word_size = this->periph->regmap.word_size_12_13.word_size_rx_0_get() + 1; break;
        case 13: this->word_size = this->periph->regmap.word_size_12_13.word_size_rx_1_get() + 1; break;
        case 14: this->word_size = this->periph->regmap.word_size_14_15.word_size_rx_0_get() + 1; break;
        case 15: this->word_size = this->periph->regmap.word_size_14_15.word_size_rx_1_get() + 1; break;
    }
}