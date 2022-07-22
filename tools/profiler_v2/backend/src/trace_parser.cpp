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
#include <time.h>
#include <regex>
#include <unistd.h>


//#define TRACE 1

#define NB_WORKERS 4


Getview_req::Getview_req()
  : current_trace_id(0), current_pending_traces(0)
{
}


Profiler_backend::Profiler_backend()
: hotspot(), max_timestamp(0), pending_request(NULL), getview_request(NULL), abort_request(false),
    nb_internal_signals(0), current_internal_signal_id(0), abort_get_view(false),started(false)
{
}


void Profiler_backend::viewer_routine()
{
    std::unique_lock<std::mutex> lock(this->mutex);

    while(1)
    {
        while (!this->getview_request)
        {
            this->cond.wait(lock);
        }

        if (this->getview_request->is_hotspot)
        {
            Getview_req *req = this->getview_request;
            int64_t start_timestamp=req->start_timestamp, end_timestamp=req->end_timestamp;

            this->getview_request = NULL;
            Hospot_results_implem *results = new Hospot_results_implem(this->hotspot);

            std::unordered_map<int, Hotspot_info *> *pc_infos;
            if (req->trace_id == -1)
            {
                for (auto x: this->hotspot_traces_array)
                {
                    x->get_hotspots(*results, start_timestamp, end_timestamp);
                }
            }
            else
            {
                Profiler_trace *trace = this->traces_array[req->trace_id];
                trace->get_hotspots(*results, start_timestamp, end_timestamp);
            }

            this->user_interface->update_hotspots(req->tag, results, start_timestamp, end_timestamp, this->max_timestamp);
        }
        else
        {
            // In case a request is already pending, they are 2 cases:
            //   - All traces are already being processed, just let them finish, they will send the
            //     to the frontend when they are done, which will drop it
            //   - Only part of the traces have been processed, in this case, adjust the current
            //     trace ID so that the latest one will take care of it.
            if (this->pending_request && this->pending_request->current_trace_id != this->pending_request->trace_ids.size())
            {
                this->pending_request->current_trace_id = this->pending_request->trace_ids.size();
            }

            this->pending_request = this->getview_request;
            this->pending_request->results->trace_slots.resize(this->pending_request->trace_ids.size());
            this->getview_request = NULL;

            this->cond.notify_all();
        }
    }
}


void Profiler_backend::worker_routine()
{
    std::unique_lock<std::mutex> lock(this->mutex);

    while(1)
    {
        while (this->pending_request == NULL)
        {
            this->cond.wait(lock);
        }

        Getview_req *req = this->pending_request;
        Profiler_view_results *results = req->results;

        int64_t start_timestamp=req->start_timestamp, end_timestamp=req->end_timestamp;

        int trace_index = req->current_trace_id++;
        req->current_pending_traces++;

        // If we took the last trace, remove the request so that no one else continues with it
        // and the other can start processing another request
        if (trace_index == req->trace_ids.size() - 1)
        {
            this->pending_request = NULL;
        }

        //clock_t begin = clock();

        std::vector<std::vector<Profiler_backend_slot_info>> &traces = results->trace_slots;
        int64_t slot_duration = req->nb_slots > 0 ? (end_timestamp - start_timestamp) / req->nb_slots : 0;

        int trace_id = req->trace_ids[trace_index];

        traces[trace_index].resize(req->nb_slots);

        if (trace_id == -1)
        {
            for (int j=0; j<req->nb_slots; j++)
            {
                traces[trace_index][j].value = 0;
            }
        }
        else
        {
            Profiler_trace *trace = this->traces_array[trace_id];
            // We need to keep the global lock until we get the trace to avoid race conditions
            // on signal registering.
            // We need to release it to let other threads compute other traces.
            // Then we need to take the trace lock to avoir accessing it while events are being added.
            lock.unlock();
            std::unique_lock<std::mutex> trace_lock(trace->mutex);
            trace->get_view(traces[trace_index], start_timestamp, slot_duration, req->nb_slots);
            trace_lock.unlock();
            lock.lock();
        }

        req->current_pending_traces--;

        // If there is no trace being processed and the last one has be processed, send the results
        // to the front-end
        if (req->current_pending_traces == 0 && req->current_trace_id == req->trace_ids.size())
        {
            //clock_t end = clock();

            //printf("Processed request time: %f, nb_slots: %ld\n", (double)(end - begin) / CLOCKS_PER_SEC, req->trace_ids.size());

            if (this->verbose_level > 0)
            {
                printf("[\033[34mPROFILER\033[0m] Update view reply (tag: %p, start_timestamp: %ld, end_timestamp: %ld, max_timestamp: %ld, nb_slots: %d)\n", req->tag, start_timestamp, end_timestamp, max_timestamp, req->nb_slots);
            }

            if (this->verbose_level > 100)
            {
                int64_t time = start_timestamp;
                printf("[\033[34mPROFILER\033[0m] ");
                for (int i=0; i<req->nb_slots; i++)
                {
                    if (time <= max_timestamp)
                    {
                        printf("%d ", (int)traces[0][i].value);
                    }
                    else
                    {
                        printf(" - ");
                    }
                    if (((i+1) % 16) == 0)
                    {
                        printf("\n");
                    }
                    time += slot_duration;
                }
                printf("\n");
            }

            results->max_timestamp = max_timestamp;
            results->start_timestamp = start_timestamp;
            results->end_timestamp = end_timestamp;
            delete req;
            lock.unlock();
            this->user_interface->update_view(results);
            lock.lock();
        }
    }
}


void Profiler_backend::open(Profiler_backend_user_interface *user_interface, std::string gvsoc_config_path, js::config *json)
{
    this->verbose_level = 0;

    if (const char *verbose_level = std::getenv("PROFILER_BACKEND_VERBOSE"))
    {
        this->verbose_level = atoi(verbose_level);
    }

    js::config *signals_generate = json->get("signals_generate");
    if (signals_generate)
    {
        for (js::config *signal_generate: signals_generate->get_elems())
        {
            this->signals_generate[signal_generate->get("from_signal")->get_str()] = signal_generate;
            this->nb_internal_signals++;
        }
    }

    this->viewer_thread = new std::thread(&Profiler_backend::viewer_routine, this);

    for (int i=0; i<NB_WORKERS; i++)
    {
        this->worker_threads.push_back(new std::thread(&Profiler_backend::worker_routine, this));
    }

    this->user_interface = user_interface;
    this->gvsoc = gv::gvsoc_new();

    this->gvsoc->open(gvsoc_config_path);
    this->gvsoc->vcd_bind(this);

    if (const char *verbose_level = std::getenv("PROFILER_BACKEND_VERBOSE"))
    {
        this->verbose_level = atoi(verbose_level);
    }
}


void Profiler_backend::trace_setup(std::string trace_path, bool enabled)
{
    if (enabled)
    {
        this->gvsoc->event_add(trace_path);
    }
    else
    {
        this->gvsoc->event_exclude(trace_path);
    }
}


void Profiler_backend::hotspot_bynary_trace(std::string trace_name)
{
    Profiler_trace *trace = this->traces_map[trace_name];

    if (trace != NULL)
    {
        this->hotspot_binary_traces[trace_name] = trace;
        trace->hotspot_binary_info_enable();
    }
    else
    {
        this->binary_traces_to_enable[trace_name] = trace_name;
    }
}

void Profiler_backend::hotspot_bynary_traces(std::vector<std::string> trace_paths)
{
    for (auto trace_name: trace_paths)
    {
        this->hotspot_bynary_trace(trace_name);
    }
}


void Profiler_backend::hotspot_enable(std::string trace_path)
{
    Profiler_trace *trace = this->traces_map[trace_path];
    this->hotspot_traces[trace_path] = trace;

    if (trace != NULL)
    {
        this->hotspot_traces_array.push_back(trace);
        trace->hotspot_conf(true);
    }
    else
    {
        this->hotspot_traces_to_enable[trace_path] = trace_path;
    }
}


void Profiler_backend::hotspot_disable(std::string trace_path)
{
    Profiler_trace *trace = this->hotspot_traces[trace_path];
    this->hotspot_traces.erase(trace_path);

    if (trace)
    {
        trace->hotspot_conf(false);
    }

    if (this->hotspot_traces_to_enable.count(trace_path) != 0)
    {
        this->hotspot_traces_to_enable.erase(trace_path);
    }
}


void Profiler_backend::start(int64_t time)
{
    if (!this->started)
    {
        this->started = true;
        this->gvsoc->start();
    }

    if (time == -1)
    {
        this->gvsoc->run();
    }
    else
    {
        this->gvsoc->step(time);
    }
}

void Profiler_backend::stop()
{
    this->gvsoc->stop();
}

void Profiler_backend::push_request(Getview_req *req)
{
    std::unique_lock<std::mutex> lock(this->mutex);
 
    if (this->getview_request)
    {
        if (this->getview_request)
        {
            delete this->getview_request->results;
        }
        delete this->getview_request;
    }

    this->getview_request = req;

    this->cond.notify_all();

    lock.unlock();
}


void Profiler_backend::get_view(Profiler_view_results *results)
{
    if (this->verbose_level > 0)
    {
        printf("[\033[34mPROFILER\033[0m] Get view request (tag: %p, start_timestamp: %ld, end_timestamp: %ld, nb_slots: %d)\n",
            results->tag, results->start_timestamp, results->end_timestamp, results->nb_slots);
    }

    if (results->end_timestamp < -1)
    {
        fprintf(stderr, "Invalid end timestamp (value: %ld)\n", results->end_timestamp);
        exit(-1);
    }

    if (results->start_timestamp < -1)
    {
        fprintf(stderr, "Invalid start timestamp (value: %ld)\n", results->end_timestamp);
        exit(-1);
    }

    Getview_req *req = new Getview_req();

    if (results->start_timestamp == -1)
    {
        results->start_timestamp = 0;
    }

    if (results->end_timestamp == -1)
    {
        results->end_timestamp = this->max_timestamp;
    }

    req->results = results;
    req->is_hotspot = false;
    req->start_timestamp = results->start_timestamp;
    req->end_timestamp = results->end_timestamp;
    req->nb_slots = results->nb_slots;
    req->tag = results->tag;
    req->trace_ids = results->trace_ids;

    this->push_request(req);
}

Profiler_event_value Profiler_trace_page::get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value)
{
    Profiler_event *event = this->first_event;

#ifdef TRACE
    printf("[TRACE PAGE] Get average on %ld -> %ld\n", start_timestamp, end_timestamp);
#endif

    // Find the event right before the period
    while (event->next && event->next->timestamp <= start_timestamp)
    {
        event = event->next;
    }

    // And checks that all events after this one overlapping the period have same value
    uint64_t value = event->value.value_r;
#ifdef TRACE
    printf("[TRACE PAGE] Accounting event %ld value %f\n", event->timestamp, event->value);
#endif

    event = event->next;

    while (event && event->timestamp < end_timestamp)
    {
#ifdef TRACE
        printf("[TRACE PAGE] Accounting event %ld value %f\n", event->timestamp, event->value);
#endif

        if (event->value.value_r != value)
        {
            return no_value;
        }

        event = event->next;
    }

#ifdef TRACE
    printf("[TRACE PAGE] Average is %f\n", value);
#endif

    return (Profiler_event_value){.value_r=value};
}

Profiler_event_value Profiler_interval::get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value)
{
    if (this->trace_page)
    {
        return this->trace_page->get_average_logical(start_timestamp, end_timestamp, no_value);
    }
    else
    {
        return this->interval_page->get_average_logical(start_timestamp, end_timestamp, no_value);
    }
}

Profiler_event_value Profiler_interval_page::get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value)
{
    Profiler_interval *interval = &this->intervals[0];
    Profiler_interval *last_interval = &this->intervals[this->current];
    Profiler_interval *partial_start_interval = NULL;
    Profiler_interval *partial_end_interval = NULL;
    Profiler_event_value current_value = no_value;

#ifdef TRACE
    printf("[PAGE L%d] GET start %ld end %ld\n", this->level, start_timestamp, end_timestamp);
#endif

    // Special case where the period is after the last event
    if (this->intervals[this->current - 1].end_timestamp < start_timestamp)
    {
        return this->intervals[this->current - 1].get_average_logical(start_timestamp, end_timestamp, no_value);
    }

    // First look for the first interval covering 
    while (interval != last_interval)
    {
        if  (interval->end_timestamp <= start_timestamp)
        {
            interval++;
            continue;
        }

        if (interval->start_timestamp < start_timestamp)
        {
            if (end_timestamp <= interval->end_timestamp)
            {
                return interval->get_average_logical(start_timestamp, end_timestamp, no_value);
            }
            else
            {
                partial_start_interval = interval;
                interval++;
            }
        }
        break;
    }


    // Now look for entire intervals
    while (interval != last_interval)
    {
        if (interval->end_timestamp <= end_timestamp)
        {
#ifdef TRACE
            printf("[PAGE L%d] get value %f from interval %ld -> %ld\n", this->level, interval->value, interval->start_timestamp, interval->end_timestamp);
#endif

            // Case where the period entirely covers the interval
            if (current_value.value_r == no_value.value_r)
            {
                // In case we still not have any current value, just take the interval one and continue
                current_value = interval->value;
                if (current_value.value_r == no_value.value_r)
                {
                    return no_value;
                }
            }
            else
            {
                // Otherwise, if it is different return -1 since we cannot distinguish
                if (current_value.value_r != interval->value.value_r)
                {
                    return no_value;
                }
            }

            interval++;
        }
        else
        {
            // Case we reached the end of the period, remember to enter this partial interval and leave
            partial_end_interval = interval;
            break;
        }
    }

    if (partial_end_interval)
    {
        Profiler_event_value value = partial_end_interval->get_average_logical(start_timestamp, end_timestamp, no_value);
#ifdef TRACE
        printf("[PAGE L%d] get value %f from partial end interval %ld -> %ld\n", this->level, value, partial_end_interval->start_timestamp, partial_end_interval->end_timestamp);
#endif

        if (value.value_r == no_value.value_r)
        {
            return no_value;
        }

        if (current_value.value_r != no_value.value_r && value.value_r != current_value.value_r)
        {
            return no_value;
        }

        current_value = value;
    }

    if (partial_start_interval)
    {
        Profiler_event_value value = partial_start_interval->get_average_logical(start_timestamp, end_timestamp, no_value);
#ifdef TRACE
        printf("[PAGE L%d] get value %f from partial start interval %ld -> %ld\n", this->level, value, partial_start_interval->start_timestamp, partial_start_interval->end_timestamp);
#endif
        if (value.value_r == no_value.value_r)
        {
            return no_value;
        }

        if (current_value.value_r != no_value.value_r && value.value_r != current_value.value_r)
        {
            return no_value;
        }

        current_value = value;
    }

#ifdef TRACE
    printf("[PAGE L%d] returns %f\n", this->level, current_value);
#endif

    return current_value;
}


void Profiler_backend::close()
{
}


void Profiler_event::dump(std::string indent)
{
    printf("%s[%ld] event value %f\n", indent.c_str(), this->timestamp, this->value.value_d);
}





void Profiler_interval::dump(std::string indent)
{
    printf("%sInterval TS: %ld->%ld vaue: %f\n", indent.c_str(), this->start_timestamp, this->end_timestamp, this->value.value_d);
    if (this->trace_page)
    {
        this->trace_page->dump(indent + "  ");
    }
    else
    {
        this->interval_page->dump(indent + "  ");
    }
}


void Profiler_interval_page::dump(std::string indent)
{
    printf("%sPage (level=%d)\n", indent.c_str(), this->level);

    for (int i=0; i<this->current; i++)
    {
        this->intervals[i].dump(indent + "  ");
    }
}


void Profiler_interval_page::register_trace_page(Profiler_trace_page *trace_page)
{
    trace_page->set_parent(&this->intervals[this->current]);
    this->intervals[this->current++].set_trace_page(trace_page);
}

void Profiler_interval_page::register_interval_page(Profiler_interval_page *interval_page)
{
    interval_page->set_parent(&this->intervals[this->current]);
    this->intervals[this->current++].set_interval_page(interval_page);
}


Profiler_interval::Profiler_interval()
    : trace_page(NULL), interval_page(NULL)
{
    this->start_timestamp = -1;
}

void Profiler_interval::set_interval_page(Profiler_interval_page *interval_page)
{
    this->interval_page = interval_page;
}

void Profiler_interval::set_trace_page(Profiler_trace_page *trace_page)
{
    this->trace_page = trace_page;
}

Profiler_interval_page::Profiler_interval_page(int level)
    : level(level), current(0), parent(NULL)
{
    Profiler_interval *prev = NULL;
    for (int i=0; i<PROFILER_INTERVAL_PAGE_SIZE; i++)
    {
        Profiler_interval *interval = &this->intervals[i];
        interval->parent = this;
        interval->start_timestamp = -1;
        interval->prev = prev;
        prev = interval;
    }
}


Profiler_event *Profiler_trace_page::alloc_event(int size)
{
    uint8_t *data = this->current_data;
    this->current_data += size;
    this->current_data_size -= size;

    Profiler_event *event = (Profiler_event *)data;
    if (this->last_alloc_event)
    {
        this->last_alloc_event->next = event;
    }

    event->next = NULL;
    event->parent = this;
    this->last_alloc_event = event;

    if (this->first_event == NULL)
    {
        this->first_event = this->last_alloc_event;
    }

    return event;
}

Profiler_trace_page::Profiler_trace_page(Profiler_trace *trace)
    : trace(trace), current_data(NULL), current_data_size(0), last_alloc_event(NULL), first_event(NULL)
{
    this->current_data = new uint8_t[PROFILER_TRACE_PAGE_SIZE];
    this->current_data_size = PROFILER_TRACE_PAGE_SIZE;

    trace->register_trace_page(this);
}


void Profiler_trace_page::dump(std::string indent)
{
    Profiler_event *event = this->first_event;

    while(event)
    {
        event->dump(indent);
        event = event->next;
    }
}


void Profiler_backend::event_register(int id, std::string path, gv::Vcd_event_type type, int width)
{
    this->event_register_internal(id + this->nb_internal_signals, path, type, width, (Profiler_event_value){.value_d=-1});
}

void Profiler_backend::event_register_internal(int id, std::string path, gv::Vcd_event_type type, int width, Profiler_event_value no_value)
{
    if (this->verbose_level >= 5)
    {
        printf("[\033[34mPROFILER\033[0m] Registering new trace (path: %s, id: %d)\n", path.c_str(), id);
    }

    if (this->traces_array.size() <= id)
    {
        this->traces_array.resize(id + 1);
    }
    this->traces_array[id] = new Profiler_trace(this, path, id, type, width, no_value);
    this->traces_map[path] = this->traces_array[id];

    Profiler_backend_trace trace = { .id=id, .path=path, .type=PROFILER_BACKEND_TRACE_LOGICAL, .width=width};
    this->user_interface->new_trace(trace);

    if (this->hotspot_traces_to_enable.count(path) != 0)
    {
        this->hotspot_traces_to_enable.erase(path);
        this->hotspot_enable(path);
    }
    if (this->binary_traces_to_enable.count(path) != 0)
    {
        this->binary_traces_to_enable.erase(path);
        this->hotspot_bynary_trace(path);
    }


    js::config *signal_generate = this->signals_generate[path];

    if (signal_generate != NULL)
    {
        std::string type = signal_generate->get("type")->get_str();
        std::string path = signal_generate->get("path")->get_str();

        if (type == "binary_function")
        {
            std::vector<std::string> binary_traces;

            for (js::config *binary_trace: signal_generate->get("binaries")->get_elems())
            {
                binary_traces.push_back(binary_trace->get_str());
            }

            int signal_id = this->current_internal_signal_id++;
            this->event_register_internal(signal_id, path, gv::Vcd_event_type_string, sizeof(void *), (Profiler_event_value){.value_p=NULL});

            this->traces_array[id]->generate_binary_function_signal(this->traces_array[signal_id], binary_traces);
        }
    }
}

void Profiler_backend::event_update_logical(int64_t timestamp, int id, uint64_t value, int flags)
{
    this->event_update_value(timestamp, id, (Profiler_event_value) { .value_d=(double)value }, flags, (Profiler_event_value){.value_d=-2});
}

void Profiler_backend::event_update_value(int64_t timestamp, int id, Profiler_event_value value, int flags, Profiler_event_value no_value)
{
    id += this->nb_internal_signals;

    std::unique_lock<std::mutex> lock(this->mutex);

    if (id >= this->traces_array.size() || this->traces_array[id] == NULL)
    {
        printf("Received invalid trace ID: %d\n", id);
        exit(-1);
    }

    Profiler_trace *trace = this->traces_array[id];

    if (trace)
    {
        if (this->verbose_level >= 10)
        {
            printf("[\033[34mPROFILER\033[0m] Update logical trace (id: %d, timestamp: %ld, value: 0x%lx)\n", id, timestamp, value.value_r);
        }

        std::unique_lock<std::mutex> trace_lock(trace->mutex);
        trace->event_update_logical(timestamp, value, flags, no_value);
        trace_lock.unlock();
    }

    if (timestamp > this->max_timestamp)
    {
        this->max_timestamp = timestamp;
    }

    lock.unlock();
}

void Profiler_event::set_value(int64_t timestamp, Profiler_event_value value, Profiler_event_value no_value)
{
    this->timestamp = timestamp;
    this->value = value;
    this->parent->add_value_logical(timestamp, value, no_value);
}

void Profiler_interval::add_value_logical(int64_t timestamp, Profiler_event_value value, Profiler_event_value no_value)
{
    if (this->start_timestamp == -1)
    {
        this->start_timestamp = timestamp;
        this->value = value;
        if (this->prev)
        {
            this->prev->end_timestamp = timestamp;
        }
    }
    else if (value.value_r != this->value.value_r)
    {
        this->value = no_value;
    }

    this->end_timestamp = timestamp;
    this->parent->add_value_logical(timestamp, value, no_value);
}

void Profiler_backend::event_update_bitfield(int64_t timestamp, int id, uint8_t *value, uint8_t *flags)
{
    printf("%s %d\n", __FILE__, __LINE__);
}

void Profiler_backend::event_update_real(int64_t timestamp, int id, double value)
{
    printf("%s %d\n", __FILE__, __LINE__);
}

void Profiler_backend::event_update_string(int64_t timestamp, int id, std::string value)
{
    id += nb_internal_signals;

    std::unique_lock<std::mutex> lock(this->mutex);
    Profiler_trace *trace = this->traces_array[id];

    trace->event_update_string(timestamp, value);

    lock.unlock();
}


void Profiler_backend::get_hotspots(void *tag, int trace_id, int64_t start_timestamp, 
    int64_t end_timestamp)
{
    Getview_req *req = new Getview_req();

    if (this->verbose_level > 0)
    {
        printf("[\033[34mPROFILER\033[0m] Get hotspots request (tag: %p, trace_id: %d, start_timestamp: %ld, end_timestamp: %ld)\n", tag, trace_id, start_timestamp, end_timestamp);
    }

    if (end_timestamp < -1)
    {
        fprintf(stderr, "Invalid end timestamp (value: %ld)\n", end_timestamp);
        exit(-1);
    }

    if (start_timestamp < -1)
    {
        fprintf(stderr, "Invalid start timestamp (value: %ld)\n", end_timestamp);
        exit(-1);
    }

    req->start_timestamp = start_timestamp;
    req->end_timestamp = end_timestamp;
    req->is_hotspot = true;
    req->tag = tag;
    req->trace_id = trace_id;
    req->results = NULL;

    this->push_request(req);
}


Profiler_binary *Profiler_backend::add_static_binary(std::string path)
{
    this->hotspot.add_static_binary(path);

    if (this->binaries[path] == NULL)
    {
        Profiler_binary *binary = new Profiler_binary(path);
        this->binaries[path] = binary;
    }

    return this->binaries[path];
}


Profiler_trace *Profiler_backend::get_trace(std::string path)
{
    return this->traces_map[path];
}



Profiler_binary::Profiler_binary(std::string elf_file)
{
    std::string command = "gen-debug-info " + elf_file;

    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::invalid_argument("Unable to open binary: " + elf_file + "\n");
    }
    char buffer[128];
    std::string result = "";

    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }

    std::unordered_map<std::string, const char *> files;
    std::unordered_map<std::string, const char *> functions;
    std::stringstream ss(result);
    std::string line;
    std::regex regex{R"([\s]+)"};
    while(std::getline(ss, line,'\n'))
    {
        char functionname[256], inlined_function[256], filename[256];
        uint64_t addr;
        int lineno;
        int nb_item = sscanf(line.c_str(), "%lx %s %s %s %d", &addr, functionname, inlined_function, filename, &lineno);

        if (nb_item == 5)
        {
            const char *file = files[filename];
            if (file == NULL)
            {
                file = strdup(filename);
                files[file] = file;
            }
            const char *function = functions[functionname];
            if (function == NULL)
            {
                function = strdup(functionname);
                functions[function] = function;
            }

            this->infos[addr] = (Profiler_binary_info) {
                .pc = addr,
                .function = function,
                .inlined_function = function,
                .file = file,
                .line = lineno
            };
        }
    }
}

Profiler_view_results::Profiler_view_results(void *tag, std::vector<int> &trace_ids, int64_t start_timestamp,
    int64_t end_timestamp, int nb_slots)
    : tag(tag), trace_ids(trace_ids), start_timestamp(start_timestamp), end_timestamp(end_timestamp),
    nb_slots(nb_slots)
{
}