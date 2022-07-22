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

#pragma once

#include <gv/gvsoc.hpp>

#include "profiler_backend_interface.hpp"

#include <thread>
#include <iostream>
#include <vector>
#include "trace_dumper_types.h"
#include "trace_dumper_utils.h"

#include <string>
#include <fstream>
#include <unordered_map>

#include <sys/types.h>
#include <regex.h>

#include <mutex>
#include <condition_variable>

#include <queue>

#include "profiler_backend_hotspot.hpp"

#define PROFILER_TRACE_PAGE_SIZE (128)
#define PROFILER_INTERVAL_PAGE_SIZE (128)


class Profiler_backend;
class Profiler_trace_page;
class Profiler_interval_page;
class Profiler_event;
class Profiler_trace;

typedef union {
    double value_d;
    void *value_p;
    int64_t value_i;
    uint64_t value_r;
} Profiler_event_value;


typedef struct
{
    uint64_t pc;
    const char *function;
    const char *inlined_function;
    const char *file;
    int line;
} Profiler_binary_info;

class Profiler_binary
{
public:
    Profiler_binary(std::string path);
    inline Profiler_binary_info *get(uint64_t pc) { return &this->infos[pc]; }
private:
    std::unordered_map<uint64_t, Profiler_binary_info> infos;
};


class Profiler_trace_gen
{
public:
    virtual void event_update_logical(int64_t timestamp, Profiler_event_value value, int flags, Profiler_event_value no_value) = 0;
    virtual void add_binary(Profiler_binary *binary) = 0;

};

class Profiler_trace_binary_infos : public Profiler_trace_gen
{
public:
    Profiler_trace_binary_infos(Profiler_backend *top, Profiler_trace *trace, std::vector<std::string> binary_traces);
    void event_update_logical(int64_t timestamp, Profiler_event_value value, int flags, Profiler_event_value no_value);
    void add_binary(Profiler_binary *binary);

private:
    std::vector<std::string> binary_traces;
    bool init;
    Profiler_backend *top;
    std::vector<Profiler_binary *> binaries;
    Profiler_trace *trace;
    const char *last_function;
};

class Profiler_trace : public Profiler_trace_gen
{
public:
	Profiler_trace(Profiler_backend *top, std::string path, int id, gv::Vcd_event_type type, int width, Profiler_event_value no_value);

    void handle_data(uint8_t *data, int size);
    Profiler_event *alloc_event(int size);

    void register_trace_page(Profiler_trace_page *trace_page);
    void add_interval_page();
    void dump(std::string indent="");

    void event_update_logical(int64_t timestamp, Profiler_event_value value, int flags, Profiler_event_value no_value);
    void event_update_string(int64_t timestamp, std::string value);

    Profiler_event_value get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value);

    void get_view(std::vector<Profiler_backend_slot_info> &slots, int64_t start_timestamp, int64_t slot_duration, int nb_slots);

    void get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp);

    void hotspot_conf(bool enabled);
    void hotspot_binary_info_enable() { this->binary_info = true; }

    void generate_binary_function_signal(Profiler_trace *generated_trace, std::vector<std::string> binary_traces);

    void add_binary(Profiler_binary *binary);
    void add_binary_waiter(Profiler_trace_gen *trace);
    std::vector<Profiler_binary *> get_binaries();

    Profiler_backend *top;
    std::string path;
    int id;
    gv::Vcd_event_type type;
    int width;
    std::vector<Profiler_trace_gen *> trace_gen;
    std::mutex mutex;

private:
    Profiler_trace_page *current_page;
    std::vector<std::vector<Profiler_interval_page *>> interval_pages;
    int last_level;
    bool hotspot_enabled;
    bool binary_info;
    Profiler_trace_hotspot hotspot;
    std::vector<Profiler_binary *> binaries;
    std::vector<Profiler_trace_gen *> binary_waiters;
    Profiler_event_value no_value;
};

class Profiler_event
{
public:
	uint64_t timestamp;
    Profiler_event *next;
    void dump(std::string indent="");
    
    Profiler_event_value value;

    Profiler_trace_page *parent;

    void set_value(int64_t timestamp, Profiler_event_value value, Profiler_event_value no_value);
};

class Profiler_interval
{
    friend class Profiler_interval_page;

public:
    Profiler_interval();
    void set_interval_page(Profiler_interval_page *interval_page);
    void set_trace_page(Profiler_trace_page *trace_page);
    void dump(std::string indent="");
    void add_value_logical(int64_t timestamp, Profiler_event_value value, Profiler_event_value no_value);
    Profiler_event_value get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value);

    int64_t start_timestamp;
    int64_t end_timestamp;
    Profiler_event_value value;
    Profiler_interval *prev;

protected:
    Profiler_interval_page *parent;

private:
    Profiler_trace_page *trace_page;
    Profiler_interval_page *interval_page;
};

class Profiler_interval_page
{
    friend class Profiler_interval;

public:
    Profiler_interval_page(int level);
    bool is_full() { return current == PROFILER_INTERVAL_PAGE_SIZE; }
    void register_trace_page(Profiler_trace_page *trace_page);
    void register_interval_page(Profiler_interval_page *interval_page);
    void dump(std::string indent="");
    Profiler_event_value get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value);
    void set_parent(Profiler_interval *interval) { this->parent = interval; }
    void add_value_logical(int64_t timestamp, Profiler_event_value value, Profiler_event_value no_value) { if (this->parent) this->parent->add_value_logical(timestamp, value, no_value); }

    Profiler_interval intervals[PROFILER_INTERVAL_PAGE_SIZE];
    
protected:
    int level;

private:
    int current;
    Profiler_interval *parent;
};

class Profiler_trace_page
{
public:
    Profiler_trace_page(Profiler_trace *trace);
    bool has_size(int size) { return this->current_data_size >= size; }
    Profiler_event *alloc_event(int size);
    void dump(std::string indent="");
    int current_data_size;
    void set_parent(Profiler_interval *interval) { this->parent = interval; }
    void add_value_logical(int64_t timestamp, Profiler_event_value value, Profiler_event_value no_value) { this->parent->add_value_logical(timestamp, value, no_value); }
    Profiler_event_value get_average_logical(int64_t start_timestamp, int64_t end_timestamp, Profiler_event_value no_value);

private:
    Profiler_trace *trace;
    uint8_t *current_data;
    Profiler_event *last_alloc_event;
    Profiler_event *first_event;
    Profiler_interval *parent;
};


class Getview_req
{
public:
    Getview_req();
    Profiler_view_results *results;
    int64_t start_timestamp;
    int64_t end_timestamp;
    int nb_slots;
    void *tag;
    bool is_hotspot;
    int trace_id;
    std::vector<int> trace_ids;

    int current_trace_id;
    int current_pending_traces;
};



class Profiler_backend: public Profiler_backend_interface, public gv::Vcd_user
{
public:
    Profiler_backend();
    void open(Profiler_backend_user_interface *user_interface, std::string gvsoc_config_path, js::config *json);
    void trace_setup(std::string trace_path, bool enabled);
    void hotspot_enable(std::string trace_path);
    void hotspot_disable(std::string trace_path);
    void hotspot_bynary_traces(std::vector<std::string> trace_paths);
    void close();
    void start(int64_t time=-1);
    void stop();

    void get_view(Profiler_view_results *results);

    void get_hotspots(void *tag, int trace_id=-1, int64_t start_timestamp=-1, 
        int64_t end_timestamp=-1);

    void event_register(int id, std::string path, gv::Vcd_event_type type, int width);
    void event_update_value(int64_t timestamp, int id, Profiler_event_value value, int flags, Profiler_event_value no_value);
    void event_update_logical(int64_t timestamp, int id, uint64_t value, int flags);
    void event_update_bitfield(int64_t timestamp, int id, uint8_t *value, uint8_t *flags);
    void event_update_real(int64_t timestamp, int id, double value);
    void event_update_string(int64_t timestamp, int id, std::string value);

    void event_register_internal(int id, std::string path, gv::Vcd_event_type type, int width, Profiler_event_value no_value);

    Profiler_binary *add_static_binary(std::string path);
    Profiler_trace *get_trace(std::string path);

private:
    int handle_packet();
    void viewer_routine();
    void worker_routine();
    void hotspot_bynary_trace(std::string trace_path);
    void push_request(Getview_req *req);

    Hotspot hotspot;

    Profiler_backend_user_interface *user_interface;
    gv::Gvsoc *gvsoc;

    std::ifstream event_file;

    std::unordered_map<std::string, Profiler_trace *> traces_map;
    std::vector<Profiler_trace *> traces_array;

    std::unordered_map<std::string, js::config *> signals_generate;

    int64_t max_timestamp;

    std::thread *viewer_thread;
    std::vector<std::thread *> worker_threads;
    std::mutex mutex;
    std::condition_variable cond;

    Getview_req *pending_request;
    Getview_req *getview_request;
    int current_request_trace;
    int current_request_pending_traces;
    bool abort_request;

    int verbose_level;

    std::vector<Profiler_trace *> hotspot_traces_array;
    std::unordered_map<std::string, Profiler_trace *> hotspot_traces;
    std::unordered_map<std::string, std::string> hotspot_traces_to_enable;
    std::unordered_map<std::string, Profiler_trace *> hotspot_binary_traces;
    std::unordered_map<std::string, std::string> binary_traces_to_enable;

    int nb_internal_signals;
    int current_internal_signal_id;

    std::unordered_map<std::string, Profiler_binary *> binaries;

    bool abort_get_view;
    bool started;
};
