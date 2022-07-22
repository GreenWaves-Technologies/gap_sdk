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
#include <stdint.h>
#include <string>


#define HOTSPOT_PAGE_SIZE      (16)
#define HOTSPOT_INTERVAL_PAGE_SIZE (4)


class Hotspot_interval_page;
class Hotspot_interval;
class Hotspot;


typedef struct
{
    uint64_t pc;
    const char *function;
    const char *inlined_function;
    const char *file;
    int line;
} Hotspot_debug;

typedef struct
{
    uint32_t pc;
    int64_t duration;
    uint64_t count;
    int64_t timestamp;
    Hotspot_debug *debug;
} Hotspot_pc;


class Hospot_results_implem : public Hospot_results
{
public:
    Hospot_results_implem(Hotspot &hotspot);

    void get_files(std::vector<const char *> &files);
    void get_functions(std::vector<const char *> &functions, const char *file=NULL);
    void get_hotspots(std::list<Hotspot_info *> &hotspots, const char *file=NULL, const char *function=NULL);
    void get_function_hotspots(std::list<Hotspot_function_info *> &hotspots, const char *file=NULL);
    void get_file_hotspots(std::list<Hotspot_file_info *> &hotspots);
    void get_line_hotspots(std::list<Hotspot_line_info *> &hotspots, const char *file=NULL, const char *function=NULL);

    void append_pc(Hotspot_pc &pc, int64_t start_timestamp, int64_t end_timestamp);

    std::unordered_map<int, Hotspot_info *> infos;
private:
    Hotspot &hotspot;

};



class Hotspot_page
{
public:
    Hotspot_page();
    bool is_full() { return current == HOTSPOT_PAGE_SIZE; }
    Hotspot_pc *alloc();
    void set_parent(Hotspot_interval *interval) { this->parent = interval; }
    void get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp);

    Hotspot_interval *parent;

    int64_t get_start_timestamp() { return current == 0 ? -1 : this->infos[0].timestamp; }
    int64_t get_end_timestamp() { return current == 0 ? -1 : this->infos[current-1].timestamp + this->infos[current-1].duration; }

private:
    Hotspot_pc infos[HOTSPOT_PAGE_SIZE];
    int current;
};



class Hotspot_interval
{
public:
    Hotspot_interval();
    void set_interval_page(Hotspot_interval_page *interval_page);
    void set_page(Hotspot_page *trace_page);
    void update(int64_t timestamp, uint32_t pc, uint64_t duration);
    void get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp);

    int64_t start_timestamp;
    int64_t end_timestamp;
    Hotspot_interval_page *parent;
    Hotspot_interval *prev;    

private:
    Hotspot_page *page;
    Hotspot_interval_page *interval_page;
};



class Hotspot_interval_page
{
    friend class Hotspot_interval;

public:
    Hotspot_interval_page(int level);
    bool is_full() { return current == HOTSPOT_INTERVAL_PAGE_SIZE; }
    void set_parent(Hotspot_interval *interval) { this->parent = interval; }
    void register_page(Hotspot_page *page);
    void register_interval_page(Hotspot_interval_page *interval_page);
    void update(int64_t timestamp, uint32_t pc, uint64_t duration);
    void get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp);

    Hotspot_interval intervals[HOTSPOT_INTERVAL_PAGE_SIZE];
    
    int64_t start_timestamp;
    int64_t end_timestamp;

protected:
    int level;

private:
    int current;
    Hotspot_interval *parent;
    std::unordered_map<uint32_t, Hotspot_pc *> infos;
};

class Profiler_trace_hotspot
{
public:
    Profiler_trace_hotspot();
    void update(int64_t timestamp, uint64_t value);
    void get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp);

private:
    void register_page(Hotspot_page *page);
    void add_interval_page();

    Hotspot_page *current_hotspot_page;
    int64_t current_timestamp;
    uint32_t last_pc;
    std::vector<std::vector<Hotspot_interval_page *>> interval_pages;
    int last_level;
};


class Hotspot
{
public:
    Hotspot();
    void add_static_binary(std::string elf_file);
    inline Hotspot_debug *get(uint64_t pc) { return &this->infos[pc]; }

private:
    std::unordered_map<uint64_t, Hotspot_debug> infos;
};