/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
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
#include <vp/itf/wire.hpp>

using namespace std;

class dpi_chip_wrapper;

extern "C" void dpi_external_edge(int handle, int value);

class Pad_group
{
public:
    Pad_group(dpi_chip_wrapper *top, std::string name) : name(name), top(top) {}

    static void edge_wrapper(void *_this, int64_t timestamp, int data);
    virtual void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data) {}
    virtual bool bind(std::string pad_name, Dpi_chip_wrapper_callback *callback) { return false; }

    std::string name;
    dpi_chip_wrapper *top;
};

class Qspim_group : public Pad_group
{
public:
    Qspim_group(dpi_chip_wrapper *top, std::string name) : Pad_group(top, name) {}
    void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data);
    bool bind(std::string pad_name, Dpi_chip_wrapper_callback *callback);
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


class I2s_group : public Pad_group
{
public:
    I2s_group(dpi_chip_wrapper *top, std::string name) : Pad_group(top, name) {}
    void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data);
    bool bind(std::string pad_name, Dpi_chip_wrapper_callback *callback);
    void rx_edge(int sck, int ws, int sd);

    vp::trace trace;
    vp::i2s_slave slave;
    Dpi_chip_wrapper_callback *sck_callback;
    Dpi_chip_wrapper_callback *ws_callback;
    Dpi_chip_wrapper_callback *sdi_callback;

    int sck;
    int ws;
    int sdo;
};


class Gpio_group : public Pad_group
{
public:
    Gpio_group(dpi_chip_wrapper *top, std::string name) : Pad_group(top, name) {}
    void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data);
    void rx_edge(int data);
    bool bind(std::string pad_name, Dpi_chip_wrapper_callback *callback);
    vp::trace trace;
    vp::wire_master<int> master;
    Dpi_chip_wrapper_callback *rx_callback;
};


class Uart_group : public Pad_group
{
public:
    Uart_group(dpi_chip_wrapper *top, std::string name) : Pad_group(top, name) {}
    void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data);
    void rx_edge(int data);
    void rx_edge_full(int data, int sck, int rtr);
    bool bind(std::string pad_name, Dpi_chip_wrapper_callback *callback);
    vp::trace trace;
    vp::uart_master master;
    vp::trace tx_trace;
    vp::trace rx_trace;
    Dpi_chip_wrapper_callback *rx_callback;
    Dpi_chip_wrapper_callback *sck_callback;
    Dpi_chip_wrapper_callback *cts_callback;
    int tx;
    int sck;
    int rts;
};


class I2C_group : public Pad_group
{
public:
    I2C_group(dpi_chip_wrapper *top, std::string name) : Pad_group(top, name) {}
    void edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data);
    void rx_edge(int data);
    bool bind(std::string pad_name, Dpi_chip_wrapper_callback *callback);
    vp::trace trace;
    vp::i2c_master master;
    Dpi_chip_wrapper_callback *rx_callback;
    int scl;
    int sda;
};


class Hyper_group : public Pad_group
{
public:
    Hyper_group(dpi_chip_wrapper *top, std::string name) : Pad_group(top, name) {}
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

    void *external_bind(std::string name, int handle);

    int build();
    void start();

private:
    static void qspim_sync(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id);
    static void uart_rx_edge(void *__this, int data, int id);
    static void uart_rx_edge_full(void *__this, int data, int sck, int rtr, int id);
    static void uart_sync(void *__this, int data, int sck, int rtr, int id);
    static void i2c_rx_edge(void *__this, int sda, int id);
    static void i2c_sync(void *__this, int scl, int sda, int id);
    static void hyper_sync_cycle(void *__this, int data, int id);
    static void i2s_slave_edge(void *__this, int sck, int ws, int sd, int id);
    static void gpio_rx_edge(void *__this, int data, int id);
    static void gpio_sync(void *__this, int data, int id);

    vp::trace trace;

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

    for (auto &group : groups->get_childs())
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
                Qspim_group *group = new Qspim_group(this, name);
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
                for (int i = 0; i < group->nb_cs; i++)
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
                Hyper_group *group = new Hyper_group(this, name);
                this->groups.push_back(group);
                traces.new_trace_event(name + "/data", &group->data_trace, 8);
                js::config *nb_cs_config = config->get("nb_cs");
                group->nb_cs = nb_cs_config ? nb_cs_config->get_int() : 1;
                for (int i = 0; i < group->nb_cs; i++)
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
            else if (type == "uart")
            {
                Uart_group *group = new Uart_group(this, name);
                new_master_port(name, &group->master);
                traces.new_trace(name, &group->trace, vp::WARNING);

                group->master.set_sync_meth_muxed(&dpi_chip_wrapper::uart_rx_edge, nb_itf);
                group->master.set_sync_full_meth_muxed(&dpi_chip_wrapper::uart_rx_edge_full, nb_itf);
                this->groups.push_back(group);
                traces.new_trace_event(name + "/tx", &group->tx_trace, 1);
                traces.new_trace_event(name + "/rx", &group->rx_trace, 1);
                nb_itf++;
            }
            else if (type == "i2c")
            {
                I2C_group *group = new I2C_group(this, name);
                new_master_port(name, &group->master);
                traces.new_trace(name, &group->trace, vp::WARNING);

                group->master.set_sync_meth_muxed(&dpi_chip_wrapper::i2c_rx_edge, nb_itf);
                this->groups.push_back(group);
                nb_itf++;
            }
            else if (type == "gpio")
            {
                Gpio_group *group = new Gpio_group(this, name);
                new_master_port(name, &group->master);
                traces.new_trace(name, &group->trace, vp::WARNING);

                group->master.set_sync_meth_muxed(&dpi_chip_wrapper::gpio_rx_edge, nb_itf);
                this->groups.push_back(group);
                nb_itf++;
            }
            else if (type == "i2s")
            {
                I2s_group *group = new I2s_group(this, name);
                new_slave_port(name, &group->slave);
                traces.new_trace(name, &group->trace, vp::WARNING);
                group->slave.set_sync_meth_muxed(&dpi_chip_wrapper::i2s_slave_edge, nb_itf);
                this->groups.push_back(group);

                group->ws = 0;
                group->sdo = 0;

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


void dpi_chip_wrapper::uart_rx_edge(void *__this, int data, int id)
{
    dpi_chip_wrapper *_this = (dpi_chip_wrapper *)__this;
    Uart_group *group = static_cast<Uart_group *>(_this->groups[id]);
    group->rx_edge(data);
}


void dpi_chip_wrapper::uart_rx_edge_full(void *__this, int data, int sck, int rtr, int id)
{
    dpi_chip_wrapper *_this = (dpi_chip_wrapper *)__this;
    Uart_group *group = static_cast<Uart_group *>(_this->groups[id]);
    group->rx_edge_full(data, sck, rtr);
}


void dpi_chip_wrapper::i2c_rx_edge(void *__this, int data, int id)
{
    dpi_chip_wrapper *_this = (dpi_chip_wrapper *)__this;
    I2C_group *group = static_cast<I2C_group *>(_this->groups[id]);
}


void dpi_chip_wrapper::gpio_rx_edge(void *__this, int data, int id)
{
    dpi_chip_wrapper *_this = (dpi_chip_wrapper *)__this;
    Gpio_group *group = static_cast<Gpio_group *>(_this->groups[id]);
    group->rx_edge(data);
}


void dpi_chip_wrapper::i2s_slave_edge(void *__this, int sck, int ws, int sd, int id)
{
    dpi_chip_wrapper *_this = (dpi_chip_wrapper *)__this;
    I2s_group *group = static_cast<I2s_group *>(_this->groups[id]);
    group->rx_edge(sck, ws, sd);
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

void *dpi_chip_wrapper::external_bind(std::string name, int handle)
{
    trace.msg("Binding pad (name: %s, handle: %d)\n", name.c_str(), handle);

    int delim_pos = name.find(".");
    std::string group_name = name.substr(0, delim_pos);
    std::string pad_name = name.substr(delim_pos + 1);

    for (auto x : this->groups)
    {
        if (x->name == group_name)
        {
            Dpi_chip_wrapper_callback *callback = new Dpi_chip_wrapper_callback();

            trace.msg("Found group\n");
            callback->function = &Pad_group::edge_wrapper;
            callback->_this = callback;
            callback->group = x;
            callback->name = pad_name;
            callback->is_cs = false;
            callback->is_sck = false;
            callback->handle = handle;

            if (x->bind(pad_name, callback))
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
    _this->top->get_time_engine()->update(timestamp);
    _this->edge(callback, timestamp, data);
}

/*
 * QSPIM
 */

bool Qspim_group::bind(std::string pad_name, Dpi_chip_wrapper_callback *callback)
{
    if (pad_name == "sck")
    {
        callback->pad_value = &this->sck;
        callback->is_sck = true;
    }
    else if (pad_name == "mosi")
    {
        callback->pad_value = &this->data_0;
    }
    else if (pad_name == "miso")
    {
        callback->pad_value = &this->data_1;
    }
    else if (pad_name.find("csn") != string::npos)
    {
        int cs = std::stoi(pad_name.substr(3));
        callback->pad_value = &this->cs[cs];
        callback->is_cs = true;
        callback->cs_id = cs;
    }
    else
    {
        return true;
    }

    return false;
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
            //this->trace.force_warning("Trying to synchronize unconnected pads\n");
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
                //this->trace.force_warning("Trying to synchronize unconnected pads\n");
            }
            else
            {
                this->master[this->active_cs]->sync(this->sck, this->data_0, this->data_1, this->data_2, this->data_3, 0xffffffff);
            }
        }
    }
}

/*
 * I2S
 */

bool I2s_group::bind(std::string pad_name, Dpi_chip_wrapper_callback *callback)
{
    callback->pad_value = NULL;

    if (pad_name == "sck")
    {
        callback->pad_value = &this->sck;
        callback->is_sck = true;
        this->sck_callback = callback;
    }
    else if (pad_name == "ws")
    {
        callback->pad_value = &this->ws;
        this->ws_callback = callback;
    }
    else if (pad_name == "sdi")
    {
        this->sdi_callback = callback;
        return true;
    }
    else if (pad_name == "sdo")
    {
        callback->pad_value = &this->sdo;
    }
    else
    {
        return true;
    }

    return false;
}

void I2s_group::edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "I2S edge (timestamp: %ld, name: %s, value: %d)\n", timestamp, callback->name.c_str(), data);

    *(callback->pad_value) = data;

    if (callback->is_sck)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "I2S clock (name: %s, value: %d)\n", callback->name.c_str(), data);

        if (this->slave.is_bound())
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "I2S clock  SYNC(name: %s, value: %d)\n", callback->name.c_str(), data);
            this->slave.sync(this->sck, this->ws, this->sdo);
        }
    }
}

void I2s_group::rx_edge(int sck, int ws, int sd)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "External EDGE\n");
    dpi_external_edge(this->sdi_callback->handle, sd);
}

/*
 * UART
 */

bool Uart_group::bind(std::string pad_name, Dpi_chip_wrapper_callback *callback)
{
    if (pad_name == "rx")
    {
        this->rx_callback = callback;
    }
    else if (pad_name == "clk")
    {
        this->sck_callback = callback;
        callback->pad_value = &this->sck;
    }
    else if (pad_name == "cts")
    {
        this->cts_callback = callback;
    }
    else if (pad_name == "tx")
    {
        callback->pad_value = &this->tx;
    }
    else if (pad_name == "rts")
    {
        callback->pad_value = &this->rts;
    }
    return false;
}

void Uart_group::edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "UART edge (timestamp: %ld, name: %s, value: %d)\n", timestamp, callback->name.c_str(), data);

    if (callback->pad_value)
        *(callback->pad_value) = data;

    if (this->master.is_bound())
    {
        this->master.sync_full(this->tx, this->sck, this->rts);
    }
}

void Uart_group::rx_edge(int data)
{
    this->rx_trace.event((uint8_t *)&data);

    dpi_external_edge(this->rx_callback->handle, data);
}

void Uart_group::rx_edge_full(int data, int sck, int rtr)
{
    this->rx_trace.event((uint8_t *)&data);

    if (this->rx_callback)
        dpi_external_edge(this->rx_callback->handle, data);

    if (this->sck_callback)
        dpi_external_edge(this->sck_callback->handle, sck);

    if (this->cts_callback)
        dpi_external_edge(this->cts_callback->handle, rtr);
}


/*
 * I2C
 */

bool I2C_group::bind(std::string pad_name, Dpi_chip_wrapper_callback *callback)
{
    if (pad_name == "scl")
    {
        callback->pad_value = &this->scl;
        callback->is_sck = true;
    }
    else if (pad_name == "sda")
    {
        callback->pad_value = &this->sda;
        this->rx_callback = callback;
    }
    return false;
}

void I2C_group::edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "I2C edge (timestamp: %ld, name: %s, value: %d)\n", timestamp, callback->name.c_str(), data);

    *(callback->pad_value) = data;

    if (this->master.is_bound())
    {
        this->master.sync(this->scl, this->sda);
    }
}

void I2C_group::rx_edge(int data)
{
    dpi_external_edge(this->rx_callback->handle, data);
}


/*
 * GPIO
 */

bool Gpio_group::bind(std::string pad_name, Dpi_chip_wrapper_callback *callback)
{
    this->rx_callback = callback;
    return false;
}


void Gpio_group::edge(Dpi_chip_wrapper_callback *callback, int64_t timestamp, int data)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "GPIO edge (timestamp: %ld, name: %s, value: %d)\n", timestamp, callback->name.c_str(), data);

    if (this->master.is_bound())
    {
        this->master.sync(data);
    }
}


void Gpio_group::rx_edge(int data)
{
    dpi_external_edge(this->rx_callback->handle, data);
}


void dpi_chip_wrapper::start()
{
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new dpi_chip_wrapper(config);
}
