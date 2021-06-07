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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "archi/utils.h"
#include "udma_v4_impl.hpp"
#include "udma_ctrl/udma_ctrl_regs.h"
#include "udma_ctrl/udma_ctrl_regfields.h"
#include "udma_ctrl/udma_ctrl_gvsoc.h"

#ifdef HAS_I2S
#include "i2s/udma_i2s_v3.hpp"
#endif

#ifdef HAS_I2C
#include "i2c/v4/udma_i2c.hpp"
#endif

#ifdef HAS_HYPER
#include "hyper/udma_hyper_v3.hpp"
#endif

using namespace std::placeholders;


void Udma_channel::reset(bool active)
{
    if (active)
    {
        this->active = false;
    }
}


void Udma_channel::set_addrgen(Udma_addrgen *addrgen)
{
    this->addrgen = addrgen;
    addrgen->register_channel(this);
}


void Udma_channel::unset_addrgen()
{
    if (this->addrgen)
    {
        this->addrgen->register_channel(NULL);
    }
    this->addrgen = NULL;
}


vp::io_req_status_e Udma_channel::req(vp::io_req *req, uint64_t offset)
{
    return this->access(offset, req->get_size(), req->get_data(), req->get_is_write());
}


vp::io_req_status_e Udma_channel::access(uint64_t offset, int size, uint8_t *value, bool is_write)
{
    if (this->addrgen == NULL)
    {
        return vp::IO_REQ_INVALID;
    }
    else
    {
        return this->addrgen->access(offset, size, value, is_write);
    }
}



Udma_channel::Udma_channel(udma *top, string name) : top(top), name(name)
{
}


Udma_periph::Udma_periph(udma *top, int id) : top(top), id(id)
{
}


void Udma_periph::clock_gate(bool new_is_on)
{
    if (is_on != new_is_on)
    {
        if (new_is_on)
            top->trace.msg(vp::trace::LEVEL_INFO, "Activating periph (periph: %d)\n", id);
        else
            top->trace.msg(vp::trace::LEVEL_INFO, "Dectivating periph (periph: %d)\n", id);
    }
    is_on = new_is_on;
}

void Udma_periph::reset(bool active)
{
    this->reset_active = active;

    if (active)
    {
        is_on = false;
    }

    if (channel0)
        channel0->reset(active);
    if (channel1)
        channel1->reset(active);
    if (channel2)
        channel2->reset(active);
}

vp::io_req_status_e Udma_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    return vp::IO_REQ_INVALID;
}

vp::io_req_status_e Udma_periph::req(vp::io_req *req, uint64_t offset)
{
    if (this->reset_active)
    {
        top->trace.force_warning("Accessing periph under reset (id: %d)\n", this->id);
        return vp::IO_REQ_INVALID;
    }

    if (!is_on)
    {
        //top->trace.warning("Trying to access periph while it is off (periph: %d)\n", id);
        //return vp::IO_REQ_INVALID;
        // TODO should dump the warning but the himax driver is buggy
        return vp::IO_REQ_OK;
    }

    return custom_req(req, offset);
}


void udma::trigger_event(int event)
{
    trace.msg(vp::trace::LEVEL_DEBUG, "Triggering event (event: %d)\n", event);
    event_itf.sync(event);
}


udma::udma(js::config *config)
    : vp::component(config)
{
}



void udma::cfg_cg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->ctrl_regmap.cfg_cg.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        for (int i = 0; i < nb_periphs; i++)
        {
            if (periphs[i] != NULL && periphs[i]->id == i)
                periphs[i]->clock_gate((this->ctrl_regmap.cfg_cg.get() >> i) & 1);
        }
    }
}


void udma::cfg_cg_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    uint32_t mask = 0;
    memcpy((void *)(((uint8_t *)&mask)+reg_offset), value, size);
    uint32_t reg_val = this->ctrl_regmap.cfg_cg.get_32() | mask;
    this->cfg_cg_req(0, 4, (uint8_t *)&reg_val, 1);
}


void udma::cfg_cg_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    uint32_t mask = 0;
    memcpy((void *)(((uint8_t *)&mask)+reg_offset), value, size);
    uint32_t reg_val = this->ctrl_regmap.cfg_cg.get_32() & ~mask;
    this->cfg_cg_req(0, 4, (uint8_t *)&reg_val, 1);
}


void udma::cfg_rstn_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->ctrl_regmap.cfg_rstn.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        for (int i = 0; i < this->nb_periphs; i++)
        {
            if (this->periphs[i] != NULL && this->periphs[i]->id == i)
                this->periphs[i]->reset(!((this->ctrl_regmap.cfg_rstn.get_32() >> i) & 1));
        }
    }
}


void udma::cfg_rstn_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    uint32_t mask = 0;
    memcpy((void *)(((uint8_t *)&mask)+reg_offset), value, size);
    uint32_t reg_val = this->ctrl_regmap.cfg_rstn.get_32() | mask;
    this->cfg_rstn_req(0, 4, (uint8_t *)&reg_val, 1);
}


void udma::cfg_rstn_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    uint32_t mask = 0;
    memcpy((void *)(((uint8_t *)&mask)+reg_offset), value, size);
    uint32_t reg_val = this->ctrl_regmap.cfg_rstn.get_32() & ~mask;
    this->cfg_rstn_req(0, 4, (uint8_t *)&reg_val, 1);
}


vp::io_req_status_e udma::conf_req(vp::io_req *req, uint64_t offset)
{
    if (this->ctrl_regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
        return vp::IO_REQ_INVALID;

    return vp::IO_REQ_OK;
}

vp::io_req_status_e udma::channel_req(vp::io_req *req, uint64_t offset)
{
    int channel_id = UDMA_CHANNEL_GET(offset);
    int local_offset = offset - UDMA_CHANNEL_OFFSET(channel_id);

    if (channel_id < this->nb_addrgen_linear)
    {
        return this->addrgen_linear[channel_id]->access(local_offset, req->get_size(), req->get_data(), req->get_is_write());
    }
    else if (channel_id < this->nb_addrgen_linear + this->nb_addrgen_2d)
    {
        return this->addrgen_linear[channel_id - this->nb_addrgen_linear]->access(local_offset, req->get_size(), req->get_data(), req->get_is_write());
    }

    return vp::IO_REQ_INVALID;
}

void udma::channel_register(int id, Udma_channel *channel)
{
    if (channel->is_stream)
    {
        if (channel->is_rx())
        {
            this->rx_channels->stream_in_channels[channel->stream_id] = NULL;
        }
        else
        {
            this->tx_channels->stream_out_ready_itf[channel->stream_id]->sync(false);
            this->tx_channels->stream_out_channels[channel->stream_id] = NULL;
        }
    }

    channel->is_stream = false;

    if (id == 0xff)
    {
        channel->unset_addrgen();
        return;
    }

    if (id < this->nb_addrgen_linear)
    {
        channel->set_addrgen(this->addrgen_linear[id]);
    }
    else if (id < this->nb_addrgen_linear + this->nb_addrgen_2d)
    {
        // TODO switch to 2d channels
        channel->set_addrgen(this->addrgen_2d[id - this->nb_addrgen_linear]);
    }
    else if (id >= 0xe0)
    {
        channel->is_stream = true;
        channel->stream_id = id - 0xe0;
        if (channel->is_rx())
        {
            this->rx_channels->stream_in_channels[channel->stream_id] = (Udma_rx_channel *)channel;
        }
        else
        {
            this->tx_channels->stream_out_ready_itf[channel->stream_id]->sync(true);
            this->tx_channels->stream_out_channels[channel->stream_id] = (Udma_tx_channel *)channel;
        }
        channel->unset_addrgen();
    }
    else if (id >= 0x60)
    {
    }
    else
    {
        trace.force_warning("Registering invalid channel (id: %d)\n", id);
    }
}

void udma::channel_unregister(int id, Udma_channel *channel)
{
    this->channel_register(0xff, NULL);
}

vp::io_req_status_e udma::periph_req(vp::io_req *req, uint64_t offset)
{
    uint32_t *data = (uint32_t *)req->get_data();
    uint64_t size = req->get_size();
    bool is_write = req->get_is_write();

    if (size != 4)
        return vp::IO_REQ_INVALID;

    int periph_id = UDMA_PERIPH_GET(offset);

    if (periph_id >= nb_periphs || periphs[periph_id] == NULL)
    {
        trace.force_warning("Accessing invalid periph (id: %d)\n", periph_id);
        return vp::IO_REQ_INVALID;
    }

    int real_id = periphs[periph_id]->id;

    return periphs[periph_id]->req(req, offset - (UDMA_PERIPH_OFFSET(real_id) - UDMA_PERIPH_AREA_OFFSET));
}

vp::io_req_status_e udma::req(void *__this, vp::io_req *req)
{
    udma *_this = (udma *)__this;

    uint64_t offset = req->get_addr();
    uint32_t *data = (uint32_t *)req->get_data();
    uint64_t size = req->get_size();
    bool is_write = req->get_is_write();

    _this->trace.msg(vp::trace::LEVEL_TRACE, "UDMA access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

    if (offset < UDMA_PERIPH_AREA_OFFSET)
    {
        return _this->conf_req(req, offset - UDMA_CTRL_OFFSET);
    }
    else if (offset >= UDMA_LIN_ADDRGEN_OFFSET)
    {
        return _this->channel_req(req, offset - UDMA_LIN_ADDRGEN_OFFSET);
    }
    else
    {
        return _this->periph_req(req, offset - UDMA_PERIPH_AREA_OFFSET);
    }

    return vp::IO_REQ_INVALID;
}

void udma::l2_grant(void *__this, vp::io_req *req)
{
    udma *_this = (udma *)__this;
    _this->trace.warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
}

void udma::l2_response(void *__this, vp::io_req *req)
{
    udma *_this = (udma *)__this;
    _this->trace.warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
}

void udma::clk_reg(component *__this, component *clock)
{
    udma *_this = (udma *)__this;
    _this->periph_clock = (vp::clock_engine *)clock;
}


void udma::dual_edges_clk_reg(component *__this, component *clock)
{
    udma *_this = (udma *)__this;
    _this->periph_clock_dual_edges = (vp::clock_engine *)clock;
}

void udma::fast_clk_reg(component *__this, component *clock)
{
    udma *_this = (udma *)__this;
    _this->fast_clock = (vp::clock_engine *)clock;
}

int udma::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);
    traces.new_trace("warning", &warning, vp::WARNING);

    in.set_req_meth(&udma::req);
    new_slave_port("input", &in);

    this->periph_clock_itf.set_reg_meth(&udma::clk_reg);
    new_slave_port("periph_clock", &this->periph_clock_itf);

    this->periph_clock_dual_edges_itf.set_reg_meth(&udma::dual_edges_clk_reg);
    new_slave_port("periph_clock_dual_edges", &this->periph_clock_dual_edges_itf);

    this->fast_clock_itf.set_reg_meth(&udma::fast_clk_reg);
    new_slave_port("fast_clock", &this->fast_clock_itf);

    nb_periphs = get_config_int("nb_periphs");
    periphs.reserve(nb_periphs);

    this->nb_channels = get_config_int("nb_channels");
    this->channels.resize(this->nb_channels);

    l2_read_fifo_size = get_config_int("properties/l2_read_fifo_size");

    l2_itf.set_resp_meth(&udma::l2_response);
    l2_itf.set_grant_meth(&udma::l2_grant);
    new_master_port("l2_itf", &l2_itf);

    new_master_port("event_itf", &event_itf);

    this->nb_addrgen_linear = this->get_config_int("nb_addrgen_linear");
    this->nb_addrgen_2d = this->get_config_int("nb_addrgen_2d");
    
    this->nb_udma_stream_in = this->get_config_int("nb_udma_stream_in");
    this->nb_udma_stream_out = this->get_config_int("nb_udma_stream_out");

    for (int i=0; i<this->nb_addrgen_linear; i++)
    {
        this->addrgen_linear.push_back(new Udma_addrgen_linear(this, i, i));
    }

    for (int i=0; i<this->get_config_int("nb_addrgen_2d"); i++)
    {
        this->addrgen_2d.push_back(new Udma_addrgen_2d(this, i, i+this->nb_addrgen_linear));
    }

    this->rx_channels = new Udma_rx_channels(this, l2_read_fifo_size);
    this->tx_channels = new Udma_tx_channels(this, l2_read_fifo_size);

    this->ctrl_regmap.build(this, &this->trace, "ctrl");

    this->ctrl_regmap.cfg_cg.register_callback(std::bind(&udma::cfg_cg_req, this, _1, _2, _3, _4));
    this->ctrl_regmap.cfg_cg_set.register_callback(std::bind(&udma::cfg_cg_set_req, this, _1, _2, _3, _4));
    this->ctrl_regmap.cfg_cg_clr.register_callback(std::bind(&udma::cfg_cg_clr_req, this, _1, _2, _3, _4));
    this->ctrl_regmap.cfg_rstn.register_callback(std::bind(&udma::cfg_rstn_req, this, _1, _2, _3, _4));
    this->ctrl_regmap.cfg_rstn_set.register_callback(std::bind(&udma::cfg_rstn_set_req, this, _1, _2, _3, _4));
    this->ctrl_regmap.cfg_rstn_clr.register_callback(std::bind(&udma::cfg_rstn_clr_req, this, _1, _2, _3, _4));

    for (int i = 0; i < nb_periphs; i++)
        periphs[i] = NULL;

    trace.msg(vp::trace::LEVEL_INFO, "Instantiating udma channels (nb_periphs: %d)\n", nb_periphs);

    js::config *interfaces = get_js_config()->get("interfaces");

    for (int i = 0; i < interfaces->get_size(); i++)
    {
        std::string name = interfaces->get_elem(i)->get_str();
        js::config *interface = get_js_config()->get(name);

        if (interface == NULL)
        {
            //warning.warning("Invalid JSON config, didn't find interface description (name: )\n");
            return -1;
        }

        int nb_channels = interface->get("nb_channels")->get_int();
        js::config *ids = interface->get("ids");
        js::config *offsets = interface->get("offsets");
        int size = interface->get_child_int("size");
        int version = interface->get("version")->get_int();

        trace.msg(vp::trace::LEVEL_INFO, "Instantiating interface (type: %s, nb_channels: %d, version: %d)\n", name.c_str(), nb_channels, version);

        for (int j = 0; j < nb_channels; j++)
        {
            int id = ids->get_elem(j)->get_int();
            int offset = offsets->get_elem(j)->get_int();

            if (0)
            {
            }
#ifdef HAS_SPIM
            else if (strcmp(name.c_str(), "spim") == 0)
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating SPIM channel (id: %d, offset: 0x%x)\n", id, offset);
                if (version == 4)
                {
                    Spim_periph_v4 *periph = new Spim_periph_v4(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_UART
            else if (strcmp(name.c_str(), "uart") == 0)
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating UART channel (id: %d, offset: 0x%x)\n", id, offset);
                if (version == 2)
                {
                    Uart_periph *periph = new Uart_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_HYPER
            else if (strcmp(name.c_str(), "hyper") == 0)
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating HYPER channel (id: %d, offset: 0x%x)\n", id, offset);
                if (version == 3)
                {
                    Hyper_periph *periph = new Hyper_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_I2S
            else if (strcmp(name.c_str(), "i2s") == 0)
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating I2S channel (id: %d, offset: 0x%x)\n", id, offset);
                if (version == 3)
                {
                    I2s_periph *periph = new I2s_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_I2C
            else if (strcmp(name.c_str(), "i2c") == 0)
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating I2C channel (id: %d, offset: 0x%x)\n", id, offset);
                if (version == 4)
                {
                    I2c_periph *periph = new I2c_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_CPI
            else if (strcmp(name.c_str(), "cpi") == 0)
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating CPI channel (id: %d, offset: 0x%x)\n", id, offset);
                if (version == 2)
                {
                    Cpi_periph *periph = new Cpi_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_AES
            else if (strcmp(name.c_str(), "aes") == 0)
            {
                if (version == 1)
                {
                    Aes_periph *periph = new Aes_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#if defined(HAS_SFU)
            else if (strcmp(name.c_str(), "sfu") == 0)
            {
                if (version == 1)
                {
                    Sfu_periph *periph = new Sfu_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#if defined(HAS_EMPTY_SFU)
            else if (strcmp(name.c_str(), "sfu") == 0)
            {
                if (version == 1)
                {
                    Sfu_periph_empty *periph = new Sfu_periph_empty(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
#ifdef HAS_FFC
            else if (strcmp(name.c_str(), "ffc") == 0)
            {
                if (version == 1)
                {
                    Ffc_periph *periph = new Ffc_periph(this, id, j);
                    periphs[id] = periph;
                }
                else
                {
                    throw logic_error("Non-supported udma version: " + std::to_string(version));
                }
            }
#endif
            else
            {
                trace.msg(vp::trace::LEVEL_INFO, "Instantiating channel (id: %d, offset: 0x%x)\n", id, offset);
            }
        }
    }

    return 0;
}

void udma::start()
{
}

void udma::reset(bool active)
{
    for (auto x: this->addrgen_linear)
    {
        x->reset(active);
    }

    for (auto x: this->addrgen_2d)
    {
        x->reset(active);
    }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new udma(config);
}
