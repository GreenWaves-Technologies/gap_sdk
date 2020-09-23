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
#include <vp/itf/clk.hpp>
#include <vp/trace/trace_engine.hpp>
#include <regex.h>
#include <vector>
#include <thread>
#include <string.h>

class trace_regex
{
public:
    trace_regex(std::string path, regex_t *regex, std::string file_path) : path(path), regex(regex), file_path(file_path) {}

    std::string path;
    regex_t *regex;
    std::string file_path;
};

class trace_domain : public vp::trace_engine
{

public:
    trace_domain(js::config *config);

    void set_trace_level(const char *trace_level);
    void add_paths(int events, int nb_path, const char **paths);
    void add_path(int events, const char *path);
    void add_exclude_path(int events, const char *path);
    void add_trace_path(int events, std::string path);
    void add_exclude_trace_path(int events, std::string path);
    void reg_trace(vp::trace *trace, int event, string path, string name);

    void pre_pre_build();
    int build();
    void start();
    void run();
    void quit();
    void pause();
    int join();

    void step(int64_t timestamp);

    void check_traces();

    int get_max_path_len() { return max_path_len; }

    int exchange_max_path_len(int max_len)
    {
        if (max_len > max_path_len)
            max_path_len = max_len;
        return max_path_len;
    }

    int get_trace_level() { return this->trace_level; }

private:
    void check_trace_active(vp::trace *trace, int event = 0);

    std::unordered_map<std::string, trace_regex *> trace_regexs;
    std::unordered_map<std::string, trace_regex *> trace_exclude_regexs;
    std::unordered_map<std::string, trace_regex *> events_path_regex;
    std::unordered_map<std::string, trace_regex *> events_exclude_path_regex;
    int max_path_len = 0;
    vp::trace_level_e trace_level = vp::TRACE;
    std::vector<vp::trace *> init_traces;
    std::unordered_map<std::string, FILE *> trace_files;

    vp::component *power_engine;
};

vp::trace_engine::trace_engine(js::config *config)
    : event_dumper(this), vp::component(config), first_trace_to_dump(NULL)
{

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (int i = 0; i < TRACE_EVENT_NB_BUFFER; i++)
    {
        event_buffers.push_back(new char[TRACE_EVENT_BUFFER_SIZE]);
    }
    current_buffer = event_buffers[0];
    event_buffers.erase(event_buffers.begin());
    current_buffer_size = 0;
    this->first_pending_event = NULL;

    thread = new std::thread(&trace_engine::vcd_routine, this);
}

trace_domain::trace_domain(js::config *config)
    : vp::trace_engine(config)
{
}

void trace_domain::run()
{
    this->power_engine->run();
}

void trace_domain::step(int64_t timestamp)
{
    this->power_engine->step(timestamp);
}

void trace_domain::quit()
{
    this->power_engine->quit();
}

void trace_domain::pause()
{
    this->power_engine->pause();
}

int trace_domain::join()
{
    return this->power_engine->join();
}

void trace_domain::check_trace_active(vp::trace *trace, int event)
{
    std::string full_path = trace->get_full_path();

    trace->set_event_active(false);

    if (event)
    {
        for (auto &x : events_path_regex)
        {
            if (regexec(x.second->regex, full_path.c_str(), 0, NULL, 0) == 0)
            {
                std::string file_path = x.second->file_path;                
                vp::Event_trace *event_trace;
                if (trace->is_real)
                    event_trace = event_dumper.get_trace_real(full_path, file_path);
                else if (trace->is_string)
                    event_trace = event_dumper.get_trace_string(full_path, file_path);
                else
                    event_trace = event_dumper.get_trace(full_path, file_path, trace->width);
                trace->set_event_active(true);
                trace->event_trace = event_trace;
            }
        }

        for (auto &x : this->events_exclude_path_regex)
        {
            if (regexec(x.second->regex, full_path.c_str(), 0, NULL, 0) == 0)
            {
                trace->set_event_active(false);
            }
        }
    }
    else
    {
        for (auto &x : this->trace_regexs)
        {
            if (regexec(x.second->regex, full_path.c_str(), 0, NULL, 0) == 0)
            {
                std::string file_path = x.second->file_path;
                if (file_path == "")
                {
                    trace->trace_file = stdout;
                }
                else
                {
                    if (this->trace_files.count(file_path) == 0)
                    {
                        FILE *file = fopen(file_path.c_str(), "w");
                        if (file == NULL)
                            throw std::logic_error("Unable to open file: " + file_path);
                        this->trace_files[file_path] = file;
                    }
                    trace->trace_file = this->trace_files[file_path];
                }
                trace->set_active(true);
            }
        }

        for (auto &x : this->trace_exclude_regexs)
        {
            if (regexec(x.second->regex, full_path.c_str(), 0, NULL, 0) == 0)
            {
                if (event)
                    trace->set_event_active(false);
                else
                    trace->set_active(false);
            }
        }
    }
}

void trace_domain::check_traces()
{
    for (auto x : this->traces_array)
    {
        this->check_trace_active(x, x->is_event);
    }
}

void trace_domain::reg_trace(vp::trace *trace, int event, string path, string name)
{
    trace->set_trace_manager(this);

    this->traces_array.push_back(trace);
    trace->id = this->traces_array.size() - 1;

    int len = path.size() + name.size() + 1;
    if (len > max_path_len)
        max_path_len = len;

    string full_path;

    if (name[0] != '/')
        full_path = path + "/" + name;
    else
        full_path = name;

    traces_map[full_path] = trace;
    trace->set_full_path(full_path);
    trace->is_event = event;

    trace->trace_file = stdout;

    this->check_trace_active(trace, event);
}

void trace_domain::pre_pre_build()
{
    new_service("trace", static_cast<trace_engine *>(this));

    for (auto x : this->get_vp_config()->get("traces/include_regex")->get_elems())
    {
        std::string trace_path = x->get_str();
        this->add_trace_path(0, trace_path);
    }
    for (auto x : this->get_vp_config()->get("events/include_regex")->get_elems())
    {
        std::string trace_path = x->get_str();
        this->add_trace_path(1, trace_path);
    }

    this->set_trace_level(this->get_vp_config()->get_child_str("traces/level").c_str());
}

int trace_domain::build()
{
    this->power_engine = this->new_component("", this->get_js_config(), "vp.power_engine_impl");

    js::config *config = get_js_config()->get("gvsoc");

    auto vcd_traces = config->get("events/traces");

    if (vcd_traces != NULL)
    {
        for (auto x : vcd_traces->get_childs())
        {
            std::string type = x.second->get_child_str("type");
            std::string path = x.second->get_child_str("path");

            if (type == "string")
            {
                vp::trace *trace = new vp::trace();
                traces.new_trace_event_string(path, trace);
            }
            else if (type == "int")
            {
                vp::trace *trace = new vp::trace();
                traces.new_trace_event(path, trace, 32);
                this->init_traces.push_back(trace);
            }
        }
    }

    return 0;
}

void trace_domain::start()
{
    for (auto x : this->init_traces)
    {
        x->event(NULL);
    }

    this->traces.set_trace_manager((vp::trace_engine *)this->get_service("trace"));
}



void trace_domain::add_exclude_path(int events, const char *path)
{
    regex_t *regex = new regex_t();

    if (events)
    {
        char *delim = (char *)::index(path, '@');
        if (delim)
        {
            *delim = 0;
        }

        if (this->events_path_regex.count(path) > 0)
        {
            delete this->events_path_regex[path];
            this->events_path_regex.erase(path);
        }
        else
        {
            this->events_exclude_path_regex[path] = new trace_regex(path, regex, "");
        }
    }
    else
    {
        char *delim = (char *)::index(path, ':');
        if (delim)
        {
            *delim = 0;
        }
        if (this->trace_regexs.count(path) > 0)
        {
            delete this->trace_regexs[path];
            this->trace_regexs.erase(path);
        }
        else
        {
            this->trace_exclude_regexs[path] = new trace_regex(path, regex, "");
        }
    }

    regcomp(regex, path, 0);
}



void trace_domain::add_path(int events, const char *path)
{
    regex_t *regex = new regex_t();

    if (events)
    {
        const char *file_path = "all.vcd";
        char *delim = (char *)::index(path, '@');
        if (delim)
        {
            *delim = 0;
            file_path = delim + 1;
        }

        if (this->events_exclude_path_regex.count(path) > 0)
        {
            delete this->events_exclude_path_regex[path];
            this->events_exclude_path_regex.erase(path);
        }

        this->events_path_regex[path] = new trace_regex(path, regex, file_path);
    }
    else
    {
        std::string file_path = "";
        char *dup_path = strdup(path);
        char *sep = strchr(dup_path, ':');
        if (sep)
        {
            *sep = 0;
            file_path = sep + 1;
            path = dup_path;
        }

        if (this->trace_exclude_regexs.count(path) > 0)
        {
            delete this->trace_exclude_regexs[path];
            this->trace_exclude_regexs.erase(path);
        }

        this->trace_regexs[path] = new trace_regex(path, regex, file_path);
    }

    regcomp(regex, path, 0);
}

void trace_domain::add_trace_path(int events, std::string path)
{
    this->add_path(events, path.c_str());
}

void trace_domain::add_exclude_trace_path(int events, std::string path)
{
    this->add_exclude_path(events, path.c_str());
}

void trace_domain::add_paths(int events, int nb_path, const char **paths)
{
    for (int i = 0; i < nb_path; i++)
    {
        add_path(events, paths[i]);
    }
}



void trace_domain::set_trace_level(const char *trace_level)
{
    if (strcmp(trace_level, "error") == 0)
    {
        this->trace_level = vp::ERROR;
    }
    else if (strcmp(trace_level, "warning") == 0)
    {
        this->trace_level = vp::WARNING;
    }
    else if (strcmp(trace_level, "info") == 0)
    {
        this->trace_level = vp::INFO;
    }
    else if (strcmp(trace_level, "debug") == 0)
    {
        this->trace_level = vp::DEBUG;
    }
    else if (strcmp(trace_level, "trace") == 0)
    {
        this->trace_level = vp::TRACE;
    }
}

extern "C" vp::component *vp_constructor(js::config *config)
{

    return new trace_domain(config);
}
