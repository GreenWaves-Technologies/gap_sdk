/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS
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
#include <gv/gvsoc.hpp>

class Gvsoc_launcher : public gv::Gvsoc
{
public:
    void open(std::string config_path);

    void close();

    void run();

    int64_t stop();

    int64_t step(int64_t duration);

    gv::Io_binding *io_bind(gv::Io_user *user, std::string comp_name, std::string itf_name);

    void vcd_bind(gv::Vcd_user *user);
    void event_add(std::string path, bool is_regex);
    void event_exclude(std::string path, bool is_regex);

private:

    void *handler;
    vp::component *instance;
};

gv::Gvsoc *gv::gvsoc_new()
{
    return new Gvsoc_launcher();
}

void Gvsoc_launcher::open(std::string config_path)
{
    this->handler = vp::__gv_create(config_path, NULL);
    this->instance = ((vp::top *)this->handler)->top_instance;

    gv_start(this->handler);
}

void Gvsoc_launcher::close()
{
    gv_destroy(this->handler);
}

void Gvsoc_launcher::run()
{
    gv_step(this->handler, 0);
}

int64_t Gvsoc_launcher::stop()
{
    this->instance->stop_exec();
    return this->instance->get_time();
}

int64_t Gvsoc_launcher::step(int64_t duration)
{
    gv_step(this->handler, duration);
    return 0;
}

gv::Io_binding *Gvsoc_launcher::io_bind(gv::Io_user *user, std::string comp_name, std::string itf_name)
{
    return (gv::Io_binding *)this->instance->external_bind(comp_name, itf_name, (void *)user);
}

void Gvsoc_launcher::vcd_bind(gv::Vcd_user *user)
{
    this->instance->traces.get_trace_manager()->set_vcd_user(user);
}

void Gvsoc_launcher::event_add(std::string path, bool is_regex)
{
    this->instance->traces.get_trace_manager()->conf_trace(1, path, 1);
}

void Gvsoc_launcher::event_exclude(std::string path, bool is_regex)
{
    this->instance->traces.get_trace_manager()->conf_trace(1, path, 0);
}
