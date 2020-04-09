/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <vp/vp.hpp>
#include <gv/dpi_chip_wrapper.hpp>
#include <stdio.h>
#include <vp/itf/io.hpp>
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/jtag.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/hyper.hpp>
#include <vp/itf/clock.hpp>
#include <vp/itf/i2c.hpp>
#include <vp/itf/i2s.hpp>

using namespace std;


class Pad_group
{
public:
    Pad_group(std::string name) : name(name) {}

    static void edge_wrapper(void *_this, int64_t timestamp, int data);
    virtual void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data) {}

    std::string name;
};


class Qspim_group : public Pad_group
{
public:
  Qspim_group(std::string name) : Pad_group(name) {}
  void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data);
  vp::trace trace;
  vp::trace data_0_trace;
  vp::trace data_1_trace;
  vp::trace data_2_trace;
  vp::trace data_3_trace;
  int nb_cs;
  vector<vp::trace *> cs_trace;
  vector<vp::qspim_master *> master;
  vector<vp::wire_master<bool> *> cs_master;
  int active_cs;

  int sck;
  int data_0;
  int data_1;
  int data_2;
  int data_3;
  int *cs;
};


class Hyper_group : public Pad_group
{
public:
  Hyper_group(std::string name) : Pad_group(name) {}
  vp::trace data_trace;
  int nb_cs;
  vector<vp::trace *> cs_trace;
  vector<vp::hyper_master *> master;
  vector<vp::wire_master<bool> *> cs_master;
  int active_cs;
};


class dpi_chip_wrapper : public vp::component
{

public:

    dpi_chip_wrapper(js::config *config);

    void *external_bind(std::string name);

    int build();
    void start();

private:
    static void qspim_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id);

    static void hyper_sync_cycle(void *__this, int data, int id);

    vp::trace     trace;

    vector<Pad_group *> groups;

    int nb_itf = 0;
};


static dpi_chip_wrapper *gv_chip_wrapper = NULL;



dpi_chip_wrapper::dpi_chip_wrapper(js::config *config)
: vp::component(config)
{
    gv_chip_wrapper = this;
}


int dpi_chip_wrapper::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    js::config *groups = get_js_config()->get("groups");

    for (auto& group: groups->get_childs())
    {
        std::string name = group.first;
        js::config *config = group.second;
        js::config *type_config = config->get("type");
        if (type_config)
        {
            std::string type = type_config->get_str();

            trace.msg("Found pad group (group: %s, type: %s)\n",
                name.c_str(), type.c_str());

            if (type == "qspim")
            {
                Qspim_group *group = new Qspim_group(name);
                group->active_cs = -1;
                this->groups.push_back(group);

                traces.new_trace(name, &group->trace, vp::WARNING);

                traces.new_trace_event(name + "/data_0", &group->data_0_trace, 1);
                traces.new_trace_event(name + "/data_1", &group->data_1_trace, 1);
                traces.new_trace_event(name + "/data_2", &group->data_2_trace, 1);
                traces.new_trace_event(name + "/data_3", &group->data_3_trace, 1);

                js::config *nb_cs_config = config->get("nb_cs");
                group->nb_cs = nb_cs_config ? nb_cs_config->get_int() : 1;
                group->cs = new int[group->nb_cs];
                for (int i=0; i<group->nb_cs; i++)
                {
                    vp::trace *trace = new vp::trace;
                    traces.new_trace_event(name + "/cs_" + std::to_string(i), trace, 4);
                    group->cs_trace.push_back(trace);
                    vp::qspim_master *itf = new vp::qspim_master;
                    itf->set_sync_meth_muxed(&dpi_chip_wrapper::qspim_sync, nb_itf);

                    new_master_port(name + "_cs" + std::to_string(i) + "_data", itf);
                    group->master.push_back(itf);

                    vp::wire_master<bool> *cs_itf = new vp::wire_master<bool>;
                    new_master_port(name + "_cs" + std::to_string(i), cs_itf);
                    group->cs_master.push_back(cs_itf);

                }

                nb_itf++;
            }
            else if (type == "hyper")
            {
                Hyper_group *group = new Hyper_group(name);
                this->groups.push_back(group);
                traces.new_trace_event(name + "/data", &group->data_trace, 8);
                js::config *nb_cs_config = config->get("nb_cs");
                group->nb_cs = nb_cs_config ? nb_cs_config->get_int() : 1;
                for (int i=0; i<group->nb_cs; i++)
                {
                    vp::trace *trace = new vp::trace;
                    traces.new_trace_event(name + "/cs_" + std::to_string(i), trace, 1);
                    group->cs_trace.push_back(trace);
                    vp::hyper_master *itf = new vp::hyper_master;
                    itf->set_sync_cycle_meth_muxed(&dpi_chip_wrapper::hyper_sync_cycle, nb_itf);

                    new_master_port(name + "_cs" + std::to_string(i) + "_data", itf);
                    group->master.push_back(itf);

                    vp::wire_master<bool> *cs_itf = new vp::wire_master<bool>;
                    new_master_port(name + "_cs" + std::to_string(i), cs_itf);
                    group->cs_master.push_back(cs_itf);
                }
                nb_itf++;
            }
        }
    }

    return 0;
}


void dpi_chip_wrapper::qspim_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id)
{
    #if 0
  padframe *_this = (padframe *)__this;
  Qspim_group *group = static_cast<Qspim_group *>(_this->groups[id]);

  if (mask & (1<<0))
    group->data_0_trace.event((uint8_t *)&data_0);
  if (mask & (1<<1))
    group->data_1_trace.event((uint8_t *)&data_1);
  if (mask & (1<<2))
    group->data_2_trace.event((uint8_t *)&data_2);
  if (mask & (1<<3))
    group->data_3_trace.event((uint8_t *)&data_3);

  group->slave.sync(data_0, data_1, data_2, data_3, mask);
  #endif
}


void dpi_chip_wrapper::hyper_sync_cycle(void *__this, int data, int id)
{
    #if 0
  dpi_chip_wrapper *_this = (dpi_chip_wrapper *)__this;

  Hyper_group *group = static_cast<Hyper_group *>(_this->groups[id]);
  group->data_trace.event((uint8_t *)&data);
  group->slave.sync_cycle(data);
  #endif
}


void *dpi_chip_wrapper::external_bind(std::string name)
{
    trace.msg("Binding pad (name: %s)\n", name.c_str());

    int delim_pos = name.find(".");
    std::string group_name = name.substr(0, delim_pos);
    std::string pad_name = name.substr(delim_pos+1);

    for (auto x: this->groups)
    {
        if (x->name == group_name)
        {
            Dpi_chip_wrapper_callback *callback = new Dpi_chip_wrapper_callback();

            callback->function = &Pad_group::edge_wrapper;
            callback->_this = callback;
            callback->group = x;
            callback->name = pad_name;
            callback->is_cs = false;
            callback->is_sck = false;

            Qspim_group *group = (Qspim_group *)x;

            if (pad_name == "sck")
            {
                callback->pad_value = &group->sck;
                callback->is_sck = true;
            }
            else if (pad_name == "mosi")
            {
                callback->pad_value = &group->data_0;
            }
            else if (pad_name == "miso")
            {
                callback->pad_value = &group->data_1;
            }
            else if (pad_name.find("cs") != string::npos)
            {
                int cs = std::stoi(pad_name.substr(2));
                callback->pad_value = &group->cs[cs];
                callback->is_cs = true;
                callback->cs_id = cs;
            }
            else
            {
                return NULL;
            }

            return (void *)callback;
        }
    }

    return NULL;
}


void Pad_group::edge_wrapper(void *__this, int64_t timestamp, int data)
{
    Dpi_chip_wrapper_callback *callback = (Dpi_chip_wrapper_callback *)__this;
    Pad_group *_this = (Pad_group *)callback->group;
    _this->edge(callback, timestamp, data);
}


void Qspim_group::edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "SPI edge (name: %s, value: %d)\n", callback->name.c_str(), data);

    *(callback->pad_value) = data;

    if (callback->is_cs)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "SPI CS (name: %s, value: %d)\n", callback->name.c_str(), data);

        if (!this->cs_master[callback->cs_id]->is_bound())
        {
            this->trace.force_warning("Trying to synchronize unconnected pads\n");
        }
        else
        {
            if (data == 0)
                this->active_cs = callback->cs_id;
            else
                this->active_cs = -1;

            this->cs_master[callback->cs_id]->sync(!data);
        }
    }
    else if (callback->is_sck)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "SPI clock (name: %s, value: %d)\n", callback->name.c_str(), data);

        if (this->active_cs != -1)
        {
            if (!this->master[this->active_cs]->is_bound())
            {
                this->trace.force_warning("Trying to synchronize unconnected pads\n");
            }
            else
            {
                this->master[this->active_cs]->sync(this->sck, this->data_0, this->data_1, this->data_2, this->data_3, 0xffffffff);
            }
        }
    }
}


void dpi_chip_wrapper::start()
{
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new dpi_chip_wrapper(config);
}
