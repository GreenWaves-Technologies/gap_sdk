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
#include <vp/power/power_engine.hpp>
#include <regex.h>
#include <vector>
#include <thread>
#include <string.h>



class power_manager : public vp::power_engine
{
public:
    power_manager(js::config *config);

    void pre_pre_build();

    int build();

    void run();

    int64_t step(int64_t timestamp);

    void quit(int status);

    void pause();

    void req_stop_exec();

    void register_exec_notifier(vp::Notifier *notifier);

    void stop_exec();

    int join();

    void start_capture();

    void stop_capture();

    void reg_trace(vp::power_trace *trace);

private:
    std::vector<vp::power_trace *> traces;

    vp::component *time_engine;
};



void power_manager::run()
{
    this->time_engine->run();
}



int64_t power_manager::step(int64_t timestamp)
{
    return this->time_engine->step(timestamp);
}



void power_manager::quit(int status)
{
    this->time_engine->quit(status);
}



void power_manager::pause()
{
    this->time_engine->pause();
}

void power_manager::stop_exec()
{
    this->time_engine->stop_exec();
}

void power_manager::req_stop_exec()
{
    this->time_engine->req_stop_exec();
}

void power_manager::register_exec_notifier(vp::Notifier *notifier)
{
    this->time_engine->register_exec_notifier(notifier);
}


int power_manager::join()
{
    return this->time_engine->join();
}



void power_manager::start_capture()
{
    for (auto trace : this->traces)
    {
        trace->clear();
    }
}



void power_manager::stop_capture()
{
    FILE *file = fopen("power_report.csv", "w");
    if (file == NULL)
    {
        vp_warning_always(&this->warning, "Failed to open power report file (path: %s)\n", "power_report.csv");
        return;
    }

    this->dump_traces_recursive(file);
}



void power_manager::reg_trace(vp::power_trace *trace)
{
    this->traces.push_back(trace);
}



vp::power_engine::power_engine(js::config *config)
    : vp::component(config)
{
}



void power_manager::pre_pre_build()
{
    this->new_service("power", static_cast<power_engine *>(this));
}



power_manager::power_manager(js::config *config)
    : vp::power_engine(config)
{
}



int power_manager::build()
{
    this->time_engine = this->new_component("", this->get_js_config(), "vp.time_domain_impl");
    return 0;
}



extern "C" vp::component *vp_constructor(js::config *config)
{
    return new power_manager(config);
}
