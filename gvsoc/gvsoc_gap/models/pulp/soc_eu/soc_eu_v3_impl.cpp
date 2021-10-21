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
#include <vp/itf/wire.hpp>
#include <vp/itf/clock.hpp>
#include <string.h>
#include <archi/soc_eu/soc_eu_v2.h>
#include <archi/chips/gap9_v2/soc_eu/soc_eu_regs.h>
#include <archi/chips/gap9_v2/soc_eu/soc_eu_regfields.h>
#include <archi/chips/gap9_v2/soc_eu/soc_eu_gvsoc.h>


using namespace std::placeholders;

#include <queue>


class soc_eu;


class soc_eu_target
{
public:
    soc_eu_target(soc_eu *top, std::string name, std::string itf_name, int id);
    static void soc_event_sync(void *_this, int ready, int id);
    void reset(bool active);

    vp::wire_master<int> event_itf;
    std::vector<vp::reg_32 *> mask_regs;
    std::string name;
    std::queue<int> fifo;
    int nb_waiting_events[256];
};


class soc_eu : public vp::component
{

public:
    soc_eu(js::config *config);

    int build();
    void start();
    void reset(bool active);

    static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:
    void sw_event_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void fc_mask_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int reg_id);
    void cl_mask_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int reg_id);
    void pr_mask_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int reg_id);
    void fc_mask_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void fc_mask_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    static void event_in_sync(void *__this, int event);
    static void ref_clock_sync(void *_this, bool value);
    void trigger_event(int event);

    vp::io_req_status_e trigger_event_req(int reg_offset, int size, bool is_write, uint8_t *data);
    vp::io_req_status_e mask_req(int target_id, int reg_id, int reg_offset, int size, bool is_write, uint8_t *data);

    int nb_fc_events;
    int first_fc_event;
    int ref_clock_event;

    vp::trace trace;
    vp::io_slave in;

    vp::wire_slave<int> event_in_itf;
    vp::clock_slave ref_clock;
    vp::wire_master<bool> ref_clock_event_itf;

    vector<soc_eu_target *> targets;

    std::vector<vp::wire_master<bool> *> irq_redirect;
    std::map<std::string, vp::wire_master<bool> *> irq_redirect_itfs;
    vp_regmap_soc_eu regmap;
};


soc_eu::soc_eu(js::config *config)
    : vp::component(config)
{
}


void soc_eu::trigger_event(int event)
{
    int word_id = event >> 5;
    int bit_id = event & 0x1f;

    if (event < this->irq_redirect.size() && this->irq_redirect[event] != NULL)
    {
        if (this->irq_redirect[event]->is_bound())
        {
            this->irq_redirect[event]->sync(true);
        }
    }

    for (auto target : this->targets)
    {
        if (((target->mask_regs[word_id]->get() >> bit_id) & 1) == 0)
        {
            trace.msg("Generating %s event (event: %d)\n", target->name.c_str(), event);
            if (!target->event_itf.is_bound())
                trace.msg("Trying to send event to %s while it is not instantiated\n", target->name.c_str());
            else
            {
                int ready;
                target->event_itf.sync_back(&ready);
                if (!ready)
                {
                    if (target->nb_waiting_events[event] == 4)
                    {
                        trace.force_warning("Pushing too many events (event: %d)\n", event);
                    }
                    else
                    {
                        target->nb_waiting_events[event]++;
                        target->fifo.push(event);
                    }
                }
                else
                {
                    target->event_itf.sync(event);
                }
            }
        }
    }
}


void soc_eu::sw_event_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.sw_event.update(reg_offset, size, value, is_write);

    uint32_t data = this->regmap.sw_event.get();
    for (unsigned int i = 0; i < this->nb_fc_events; i++)
    {
        if ((data >> i) & 1)
        {
            this->trigger_event(i + this->first_fc_event);
        }
    }
}


void soc_eu::fc_mask_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (size == 4 && is_write)
    {
        uint32_t data = *(uint32_t *)value;
        vp::reg_32 *fc_mask_reg = this->targets[0]->mask_regs[data / 32];
        uint32_t reg_value = fc_mask_reg->get();
        reg_value |= 1 << (data % 32);
        fc_mask_reg->update(0, 4, (uint8_t *)&reg_value, true);
    }
}


void soc_eu::fc_mask_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (size == 4 && is_write)
    {
        uint32_t data = *(uint32_t *)value;
        vp::reg_32 *fc_mask_reg = this->targets[0]->mask_regs[data / 32];
        uint32_t reg_value = fc_mask_reg->get();
        reg_value &= ~(1 << (data % 32));
        fc_mask_reg->update(0, 4, (uint8_t *)&reg_value, true);
    }
}


void soc_eu::event_in_sync(void *__this, int event)
{
    soc_eu *_this = (soc_eu *)__this;
    _this->trace.msg("Received incoming event (event: %d)\n", event);
    _this->trigger_event(event);
}


void soc_eu::ref_clock_sync(void *__this, bool value)
{
    soc_eu *_this = (soc_eu *)__this;
    _this->trace.msg("Received ref clock event, generating event (event: %d)\n", _this->ref_clock_event);
    _this->ref_clock_event_itf.sync(true);
    _this->trigger_event(_this->ref_clock_event);
}


void soc_eu::fc_mask_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int reg_id)
{
    vp::reg_32 *fc_mask_reg = this->targets[0]->mask_regs[reg_id];
    fc_mask_reg->update(reg_offset, size, value, is_write);
}


void soc_eu::cl_mask_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int reg_id)
{
}


void soc_eu::pr_mask_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int reg_id)
{
}


vp::io_req_status_e soc_eu::req(void *__this, vp::io_req *req)
{
    soc_eu *_this = (soc_eu *)__this;

    if (_this->regmap.access(req->get_addr(), req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}



int soc_eu::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);
    in.set_req_meth(&soc_eu::req);
    new_slave_port("input", &in);

    this->ref_clock_event = this->get_config_int("ref_clock_event");
    this->ref_clock.set_sync_meth(&soc_eu::ref_clock_sync);
    this->new_slave_port("ref_clock", &this->ref_clock);

    event_in_itf.set_sync_meth(&soc_eu::event_in_sync);
    new_slave_port("event_in", &event_in_itf);

    nb_fc_events = get_config_int("properties/nb_fc_events");
    first_fc_event = get_config_int("properties/first_fc_event");

    new_master_port("ref_clock_event", &ref_clock_event_itf);

    for (auto x: this->get_js_config()->get("irq_redirect")->get_elems())
    {
        int irq_id = x->get_elem(0)->get_int();
        std::string itf_name = x->get_elem(1)->get_str();

        this->irq_redirect.resize(irq_id + 1);
        vp::wire_master<bool> *itf = this->irq_redirect_itfs[itf_name];
        if (itf == NULL)
        {
            itf = new vp::wire_master<bool>;
            this->irq_redirect_itfs[itf_name] = itf;
        }

        this->new_master_port(itf_name, itf);
        this->irq_redirect[irq_id] = itf;
    }

    this->targets.push_back(new soc_eu_target(this, "FC", "fc_event_itf", 0));
    this->targets.push_back(new soc_eu_target(this, "PR", "pr_event_itf", 1));
    this->targets.push_back(new soc_eu_target(this, "CL", "cl_event_itf", 2));

    this->regmap.build(this, &this->trace);
    this->regmap.sw_event.register_callback(std::bind(&soc_eu::sw_event_req, this, _1, _2, _3, _4));
    this->regmap.fc_mask_0.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 0));
    this->regmap.fc_mask_1.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 1));
    this->regmap.fc_mask_2.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 2));
    this->regmap.fc_mask_3.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 3));
    this->regmap.fc_mask_4.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 4));
    this->regmap.fc_mask_5.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 5));
    this->regmap.fc_mask_6.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 6));
    this->regmap.fc_mask_7.register_callback(std::bind(&soc_eu::fc_mask_req, this, _1, _2, _3, _4, 7));
    this->regmap.cl_mask_0.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 0));
    this->regmap.cl_mask_1.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 1));
    this->regmap.cl_mask_2.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 2));
    this->regmap.cl_mask_3.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 3));
    this->regmap.cl_mask_4.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 4));
    this->regmap.cl_mask_5.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 5));
    this->regmap.cl_mask_6.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 6));
    this->regmap.cl_mask_7.register_callback(std::bind(&soc_eu::cl_mask_req, this, _1, _2, _3, _4, 7));
    this->regmap.pr_mask_0.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 0));
    this->regmap.pr_mask_1.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 1));
    this->regmap.pr_mask_2.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 2));
    this->regmap.pr_mask_3.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 3));
    this->regmap.pr_mask_4.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 4));
    this->regmap.pr_mask_5.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 5));
    this->regmap.pr_mask_6.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 6));
    this->regmap.pr_mask_7.register_callback(std::bind(&soc_eu::pr_mask_req, this, _1, _2, _3, _4, 7));
    this->regmap.fc_mask_set.register_callback(std::bind(&soc_eu::fc_mask_set_req, this, _1, _2, _3, _4));
    this->regmap.fc_mask_clr.register_callback(std::bind(&soc_eu::fc_mask_clr_req, this, _1, _2, _3, _4));

    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_0);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_1);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_2);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_3);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_4);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_5);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_6);
    this->targets[0]->mask_regs.push_back(&this->regmap.fc_mask_7);

    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_0);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_1);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_2);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_3);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_4);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_5);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_6);
    this->targets[1]->mask_regs.push_back(&this->regmap.cl_mask_7);

    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_0);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_1);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_2);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_3);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_4);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_5);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_6);
    this->targets[2]->mask_regs.push_back(&this->regmap.pr_mask_7);

    return 0;
}


void soc_eu::reset(bool active)
{
    this->regmap.reset(active);
    
    for (auto x: this->targets)
    {
        x->reset(active);
    }
}


void soc_eu::start()
{
}


soc_eu_target::soc_eu_target(soc_eu *top, std::string name, std::string itf_name, int id)
    : name(name)
{
    this->event_itf.set_sync_meth_muxed(&soc_eu_target::soc_event_sync, id);
    top->new_master_port(this, itf_name, &this->event_itf);
}


void soc_eu_target::reset(bool active)
{
    if (!active)
    {
        for (int i=0; i<256; i++)
        {
            this->nb_waiting_events[i] = 0;
        }

        while (!this->fifo.empty())
        {
            this->fifo.pop();
        }
    }
}


void soc_eu_target::soc_event_sync(void *__this, int ready, int id)
{
    soc_eu_target *_this = (soc_eu_target *)__this;

    while (!_this->fifo.empty())
    {
        int ready;
        _this->event_itf.sync_back(&ready);
        if (!ready)
        {
            break;
        }

        int event = _this->fifo.front();
        _this->fifo.pop();
        _this->nb_waiting_events[event]--;
        _this->event_itf.sync(event);
    }
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new soc_eu(config);
}
