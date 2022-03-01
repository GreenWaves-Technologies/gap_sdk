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



class cluster : public vp::component
{

public:
    cluster(js::config *config);

    vp::port *get_slave_port(std::string name) { return this->ports[name]; }
    vp::port *get_master_port(std::string name) { return this->ports[name]; }

    void add_slave_port(std::string name, vp::slave_port *port) { this->add_port(name, port); }
    void add_master_port(std::string name, vp::master_port *port) { this->add_port(name, port); }

    int build();
    void start();
    void reset(bool active);
    void power_supply_set(int state);

    void dump_traces(FILE *file);

    static void cluster_clock_gating_en_sync(void *__this, bool value);
    static void timer_busy_sync(void *__this, bool value, int id);
    static void ne16_busy_sync(void *__this, bool value);
    static void ico_busy_sync(void *__this, bool value);
    static void dma_busy_sync(void *__this, bool value);
    static void cores_busy_sync(void *__this, bool value, int id);

private:
    void add_port(std::string name, vp::port *port);
    std::map<std::string, vp::port *> ports;
    void check_clock_gating();

    vp::wire_slave<bool>          cluster_clock_gating_en_itf;
    vp::wire_slave<bool>          timer_busy_itf[2];
    vp::wire_slave<bool>          ne16_busy_itf;
    vp::wire_slave<bool>          ico_busy_itf;
    vp::wire_slave<bool>          dma_busy_itf;
    vp::wire_slave<bool>          cores_busy_itf[9];

    int timer_busy;
    bool ne16_busy;
    bool ico_busy;
    bool dma_busy;
    int cores_busy;

    bool busy_sync;

    bool clock_gating_en;

    vp::trace     trace;

    vp::power::power_source background_power;
};



cluster::cluster(js::config *config)
    : vp::component(config)
{
}


void cluster::dump_traces(FILE *file)
{
    this->power.get_power_trace()->dump(file);
}


void cluster::check_clock_gating()
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Checking cluster clock gating (timer: 0x%x, ne16: %d, ico: %d, dma: %d, cores: 0x%x)\n",
        this->timer_busy, this->ne16_busy, this->ico_busy, this->dma_busy, this->cores_busy);

    bool busy = this->timer_busy || this->ne16_busy || this->ico_busy || this->dma_busy || this->cores_busy;

    if (busy != this->busy_sync)
    {
        if (this->clock_gating_en && !busy)
        {
            this->power.power_supply_set_all(2);
        }
        else
        {
            this->power.power_supply_set_all(3);
        }
        this->busy_sync = busy;
    }
}

void cluster::cluster_clock_gating_en_sync(void *__this, bool value)
{
    cluster *_this = (cluster *)__this;
    _this->clock_gating_en = value;
    _this->check_clock_gating();
}

void cluster::timer_busy_sync(void *__this, bool value, int id)
{
    cluster *_this = (cluster *)__this;
    _this->timer_busy = (_this->timer_busy & ~(1<<id)) | (value << id);
    _this->check_clock_gating();
}

void cluster::ne16_busy_sync(void *__this, bool value)
{
    printf("%d BUSY %d\n", __LINE__, value);

}

void cluster::ico_busy_sync(void *__this, bool value)
{
    printf("%d BUSY %d\n", __LINE__, value);

}

void cluster::dma_busy_sync(void *__this, bool value)
{
    cluster *_this = (cluster *)__this;
    _this->dma_busy = value;
    _this->check_clock_gating();
}

void cluster::cores_busy_sync(void *__this, bool value, int id)
{
    cluster *_this = (cluster *)__this;
    _this->cores_busy = (_this->cores_busy & ~(1<<id)) | (value << id);
    _this->check_clock_gating();
}

void cluster::reset(bool active)
{
    if (active)
    {
        this->busy_sync = false;
        this->timer_busy = 0;
        this->ne16_busy = 0;
        this->ico_busy = 0;
        this->dma_busy = 0;
        this->cores_busy = 0;
        this->clock_gating_en = 0;
    }
}


int cluster::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->cluster_clock_gating_en_itf.set_sync_meth(&cluster::cluster_clock_gating_en_sync);
    new_slave_port("cluster_clock_gating_en", &this->cluster_clock_gating_en_itf);

    for (int i=0; i<2; i++)
    {
        this->timer_busy_itf[i].set_sync_meth_muxed(&cluster::timer_busy_sync, i);
        new_slave_port("timer" + std::to_string(i) + "_busy", &this->timer_busy_itf[i]);
    }

    this->ne16_busy_itf.set_sync_meth(&cluster::ne16_busy_sync);
    new_slave_port("ne16_busy", &this->ne16_busy_itf);

    this->ico_busy_itf.set_sync_meth(&cluster::ico_busy_sync);
    new_slave_port("ico_busy", &this->ico_busy_itf);

    this->dma_busy_itf.set_sync_meth(&cluster::dma_busy_sync);
    new_slave_port("dma_busy", &this->dma_busy_itf);

    for (int i=0; i<9; i++)
    {
        this->cores_busy_itf[i].set_sync_meth_muxed(&cluster::cores_busy_sync, i);
        new_slave_port("core_busy_" + std::to_string(i), &this->cores_busy_itf[i]);
    }

    this->power.new_power_source("background", &this->background_power, this->get_js_config()->get("power_models/background"));

    this->create_comps();
    this->create_ports();
    this->create_bindings();

    return 0;
}


void cluster::start()
{
    this->background_power.leakage_power_start();
    this->background_power.dynamic_power_start();
}



void cluster::add_port(std::string name, vp::port *port)
{
    vp_assert_always(port != NULL, this->get_trace(), "Adding NULL port\n");
    //vp_assert_always(this->ports[name] == NULL, this->get_trace(), "Adding already existing port\n");
    this->ports[name] = port;
}

void cluster::power_supply_set(int state)
{

    //printf("%s power set %d\n", this->get_path().c_str(), state);
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new cluster(config);
}
