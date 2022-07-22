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


#include <vector>
#include <list>
#include <stdint.h>
#include <string>
#include "json.hpp"


typedef struct
{
    uint64_t pc;
    int64_t duration;
    int64_t count;
    float percentage;
    const char *function;
    const char *inlined_function;
    const char *file;
    int line;
} Hotspot_info;

typedef struct
{
    int64_t duration;
    int64_t count;
    float percentage;
    const char *function;
    const char *file;
    int line;
} Hotspot_line_info;

typedef struct
{
    int64_t duration;
    float percentage;
    const char *function;
    const char *file;
    int line;
} Hotspot_function_info;

typedef struct
{
    int64_t duration;
    float percentage;
    const char *file;
} Hotspot_file_info;


class Hospot_results
{
public:
    // Return all the files
    virtual void get_files(std::vector<const char *> &files) = 0;
    // Return all the functions from the specified file (can be NULL)
    virtual void get_functions(std::vector<const char *> &functions, const char *file=NULL) = 0;
    // Get all the hotspots from the specified function and file (can be NULL)
    virtual void get_hotspots(std::list<Hotspot_info *> &hotspots, const char *file=NULL, const char *function=NULL) = 0;
    // Return the hotposts gathered by functions
    virtual void get_function_hotspots(std::list<Hotspot_function_info *> &hotspots, const char *file=NULL) = 0;
    // Return the hotpots gathered by file
    virtual void get_file_hotspots(std::list<Hotspot_file_info *> &hotspots) = 0;
    // Return the hotspots gathered by line for the specified function and file (can be NULL)
    virtual void get_line_hotspots(std::list<Hotspot_line_info *> &hotspots, const char *file=NULL, const char *function=NULL) = 0;
};


typedef struct
{
public:
    union {
        double value;
        void *value_p;
        int64_t value_i;
        uint64_t value_r;
    };
} Profiler_backend_slot_info;


typedef enum
{
    PROFILER_BACKEND_TRACE_LOGICAL
} Profiler_backend_trace_type_e;


typedef struct
{
public:
    int id;
    std::string path;
    Profiler_backend_trace_type_e type;
    int width;
} Profiler_backend_trace;


class Profiler_view_results
{
public:
    Profiler_view_results(void *tag, std::vector<int> &trace_ids, int64_t start_timestamp,
        int64_t end_timestamp, int nb_slots);

    void *tag;
    std::vector<int> trace_ids;
    std::vector<std::vector<Profiler_backend_slot_info>> trace_slots;
    int64_t start_timestamp;
    int64_t end_timestamp;
    int64_t max_timestamp;
    int nb_slots;
};


// Interface which must be implemented by the user of the backend
class Profiler_backend_user_interface
{
public:
    virtual void update_view(Profiler_view_results *results) = 0;

    virtual void update_hotspots(void *tag, Hospot_results *result,
        int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp) = 0;

    virtual void new_trace(Profiler_backend_trace &trace) = 0;
};



// Interface provided by the backend
class Profiler_backend_interface
{
public:
    virtual void open(  Profiler_backend_user_interface *user_interface, 
                        std::string gvsoc_config_path, js::config *json) = 0;
    virtual void trace_setup(std::string trace_path, bool enabled) = 0;
    virtual void hotspot_enable(std::string trace_path) = 0;
    virtual void hotspot_disable(std::string trace_path) = 0;
    virtual void hotspot_bynary_traces(std::vector<std::string> trace_paths) = 0;
    virtual void close() = 0;
    virtual void get_view(Profiler_view_results *results) = 0;
                            
    virtual void get_hotspots(void *tag, int trace_id=-1, int64_t start_timestamp=-1, 
        int64_t end_timestamp=-1) = 0;

    virtual void start(int64_t time=-1) = 0;
    virtual void stop() = 0;
};
