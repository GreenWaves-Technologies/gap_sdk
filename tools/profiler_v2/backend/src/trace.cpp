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


#include "profiler_backend.hpp"
#include <iostream>

//#define TRACE 1


Profiler_trace::Profiler_trace(Profiler_backend *top, std::string path, int id, gv::Vcd_event_type type, int width, Profiler_event_value no_value)
: top(top), path(path), id(id), type(type), width(width), current_page(NULL), last_level(-1), no_value(no_value)
{
    this->interval_pages.push_back({});
    this->hotspot_enabled = false;
    this->binary_info = false;
}


Profiler_event_value Profiler_trace::get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value)
{
    return this->interval_pages[this->last_level][0]->get_average_logical(start_timestamp, end_timestamp, no_value);
}


void Profiler_trace::get_view(std::vector<Profiler_backend_slot_info> &slots, int64_t start_timestamp, int64_t slot_duration, int nb_slots)
{
    for (int i=0; i<nb_slots; i++)
    {
        Profiler_backend_slot_info *slot = &slots[i];
#ifdef TRACE
        printf("[SLOT] %ld -> %ld\n", start_timestamp, start_timestamp + slot_duration);
#endif
        if (slot_duration == 0 || this->last_level == -1)
        {
            slot->value = 0;
        }
        else
        {
            slot->value_r = this->get_average_logical(start_timestamp, start_timestamp + slot_duration, this->no_value).value_r;
            if (this->id < 20 && slot->value == -1)
            {
                exit(1);
            }
        }
#ifdef TRACE
        printf("[SLOT] value %f\n", slot->value);
#endif

        start_timestamp += slot_duration;
    }
}

void Profiler_trace::dump(std::string indent)
{
    this->interval_pages.back()[0]->dump(indent);
}


void Profiler_trace::add_interval_page()
{
    int level = 0;
    Profiler_interval_page *interval_page = NULL;

    while (1)
    {
        bool new_page = false;
        if (this->interval_pages.size() <= level)
        {
            this->interval_pages.push_back({});
        }

        if (this->interval_pages[level].size() == 0 || this->interval_pages[level].back()->is_full())
        {
            if (level > this->last_level)
            {
                this->last_level = level;
            }
            Profiler_interval_page *page = new Profiler_interval_page(level);
            if (this->interval_pages[level].size() > 0)
            {
                page->intervals[0].prev = &this->interval_pages[level].back()->intervals[PROFILER_INTERVAL_PAGE_SIZE - 1];
            }
            else if (this->interval_pages[level].size() == 0 && level > 0)
            {
                page->intervals[0].start_timestamp = this->interval_pages[level - 1].front()->intervals[0].start_timestamp;
            }
            this->interval_pages[level].push_back(page);
            new_page = true;
        }

        if (level >= 1 && this->interval_pages[level-1].size() == 2)
        {
            this->interval_pages[level].back()->register_interval_page(this->interval_pages[level-1].front());
        }

        if (interval_page)
        {
            this->interval_pages[level].back()->register_interval_page(interval_page);
        }

        if (!new_page || this->interval_pages[level].size() == 1)
        {
            break;
        }

        interval_page = this->interval_pages[level].back();

        level++;
    }
}


void Profiler_trace::register_trace_page(Profiler_trace_page *trace_page)
{
    Profiler_interval_page *interval_page = NULL;

    if (this->interval_pages[0].size() == 0 || this->interval_pages[0].back()->is_full())
    {
        this->add_interval_page();
    }

    this->interval_pages[0].back()->register_trace_page(trace_page);
}


Profiler_event *Profiler_trace::alloc_event(int size)
{
    static int nb_event = 0;

    nb_event++;

    if (nb_event >= 100*1000*1000)
    {
        if (nb_event == 100*1000*1000)
        {
            fprintf(stderr, "Profiler has reached maximum number of events\n");
        }
        return NULL;
    }

    if (this->current_page == NULL || !this->current_page->has_size(size))
    {
        this->current_page = new Profiler_trace_page(this);
    }

    return this->current_page->alloc_event(size);
}


void Profiler_trace::event_update_logical(int64_t timestamp, Profiler_event_value value, int flags, Profiler_event_value no_value)
{
    Profiler_event *event = this->alloc_event(sizeof(Profiler_event));
    if (event == NULL)
    {
        return;
    }

    event->set_value(timestamp, value, no_value);

    if (this->hotspot_enabled)
    {
        this->hotspot.update(timestamp, value.value_d);
    }

    for (auto trace: this->trace_gen)
    {
        trace->event_update_logical(timestamp, value, flags, no_value);
    }
}


void Profiler_trace::event_update_string(int64_t timestamp, std::string value)
{
    if (this->binary_info)
    {
        if (value.find("static enable ") == 0)
        {
            std::stringstream ss(value.substr(14)); 
 
            std::string s; 
            while (std::getline(ss, s, ' ')) { 
                Profiler_binary *binary = this->top->add_static_binary(s);
                this->add_binary(binary);
                for (Profiler_trace_gen *trace: this->binary_waiters)
                {
                    trace->add_binary(binary);
                }
            } 
        }
    }
}

void Profiler_trace::add_binary_waiter(Profiler_trace_gen *trace)
{
    this->binary_waiters.push_back(trace);

    for (Profiler_binary *binary: this->binaries)
    {
        trace->add_binary(binary);
    }
}

void Profiler_trace::add_binary(Profiler_binary *binary)
{
    this->binaries.push_back(binary);
}

void Profiler_trace::hotspot_conf(bool enabled)
{
    this->hotspot_enabled = enabled;
}


void Profiler_trace::get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp)
{
    this->hotspot.get_hotspots(hotspots, start_timestamp, end_timestamp);
}

void Profiler_trace::generate_binary_function_signal(Profiler_trace *generated_trace, std::vector<std::string> binary_traces)
{
    Profiler_trace_binary_infos *info = new Profiler_trace_binary_infos(this->top, generated_trace, binary_traces);
    this->trace_gen.push_back(info);

}

std::vector<Profiler_binary *> Profiler_trace::get_binaries()
{
    return this->binaries;
}


void Profiler_trace_binary_infos::add_binary(Profiler_binary *binary)
{
    this->binaries.push_back(binary);
}


void Profiler_trace_binary_infos::event_update_logical(int64_t timestamp, Profiler_event_value value, int flags, Profiler_event_value no_value)
{   
    if (!this->init)
    {
        for (auto trace_name: binary_traces)
        {
            Profiler_trace *trace = this->top->get_trace(trace_name);

            if (trace)
            {
                trace->add_binary_waiter(this);
            }
        }
        this->init = true;
    }

    const char *function = NULL;
    if (flags == 1)
    {
        function = (const char *)1;
    }
    else
    {
        for (Profiler_binary *binary: this->binaries)
        {
            Profiler_binary_info *info = binary->get(value.value_d);
            if (info)
            { 
                function = info->function;
                break;
            }
        }
    }

    if (function == NULL)
    {
        function = "";
    }

    if (function && function != this->last_function)
    {
        this->last_function = function;
        this->trace->event_update_logical(timestamp, (Profiler_event_value) { .value_p=(void *)function }, 0, (Profiler_event_value){.value_p=NULL});
    }
}

Profiler_trace_binary_infos::Profiler_trace_binary_infos(Profiler_backend *top, Profiler_trace *trace, std::vector<std::string> binary_traces)
{
    this->init = false;
    this->binary_traces = binary_traces;
    this->top = top;
    this->trace = trace;
    this->last_function = NULL;
    this->trace->add_binary_waiter(this);
}