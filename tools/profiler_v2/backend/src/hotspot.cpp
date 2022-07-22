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
#include <sstream>
#include <string>
#include <string.h>
#include <regex>


Hospot_results_implem::Hospot_results_implem(Hotspot &hotspot)
  : hotspot(hotspot)
{

}


void Hospot_results_implem::get_files(std::vector<const char *> &files)
{
    for (auto& x: this->infos)
    {
        Hotspot_info *info = x.second;

        if (info->file && std::find(files.begin(), files.end(), info->file) == files.end())
        {
            files.push_back(info->file);
        }
    }
}

void Hospot_results_implem::get_functions(std::vector<const char *> &functions, const char *file)
{
    for (auto& x: this->infos)
    {
        Hotspot_info *info = x.second;

        if ((file == NULL || info->file == file) && std::find(functions.begin(), functions.end(), info->function) == functions.end())
        {
            functions.push_back(info->function);
        }
    }
}

bool compare_pc(Hotspot_info *first, Hotspot_info *second)
{
    return first->duration > second->duration;
}

void Hospot_results_implem::get_hotspots(std::list<Hotspot_info *> &result, const char *file, const char *function)
{
    int64_t duration = 0;

    for (auto x: this->infos)
    {
        Hotspot_info *info = x.second;
        if ((file == NULL || file == info->file) && (function == NULL || function == info->function))
        {
            duration += info->duration;
            result.push_front(x.second);
        }
    }

    for (auto x: result)
    {
        x->percentage = (double)x->duration / duration * 100;
    }

    result.sort(compare_pc);
}

bool compare_line(Hotspot_line_info *first, Hotspot_line_info *second)
{
    return first->duration > second->duration;
}

void Hospot_results_implem::get_line_hotspots(std::list<Hotspot_line_info *> &result, const char *file, const char *function)
{
    int64_t duration = 0;
    std::unordered_map<std::string, Hotspot_line_info *> lines;

    for (auto x: this->infos)
    {
        Hotspot_info *info = x.second;
        if ((file == NULL || (info->file && strcmp(file, info->file)) == 0) && (function == NULL || (info->function && strcmp(function, info->function))))
        {
            duration += info->duration;

            const char *file = info->file;

            if (file == NULL)
            {
                file = "";
            }

            std::string full_name = std::string(file) + std::to_string(info->line);

            if (lines.count(full_name) == 0)
            {
                Hotspot_line_info *line_info = new Hotspot_line_info();
                line_info->duration = 0;
                line_info->function = info->function;
                line_info->file = info->file;
                line_info->line = info->line;
                line_info->count = info->count;
                lines[full_name] = line_info;
                result.push_front(line_info);
            }

            Hotspot_line_info *line_info = lines[full_name];
            line_info->duration += info->duration;
            line_info->count += info->count;
        }
    }

    for (auto x: result)
    {
        x->percentage = (double)x->duration / duration * 100;
    }

    result.sort(compare_line);
}

bool compare_function(Hotspot_function_info *first, Hotspot_function_info *second)
{
    return first->duration > second->duration;
}

void Hospot_results_implem::get_function_hotspots(std::list<Hotspot_function_info *> &result, const char *file)
{
    int64_t duration = 0;
    std::unordered_map<std::string, Hotspot_function_info *> functions;

    for (auto x: this->infos)
    {
        Hotspot_info *info = x.second;
        if (file == NULL || file == info->file)
        {
            duration += info->duration;

            const char *function = info->function;

            if (function == NULL)
            {
                function = "";
            }

            if (functions.count(function) == 0)
            {
                Hotspot_function_info *function_info = new Hotspot_function_info();
                function_info->duration = 0;
                function_info->function = function;
                function_info->file = info->file;
                function_info->line = -1;
                functions[function] = function_info;
                result.push_front(function_info);
            }

            Hotspot_function_info *function_info = functions[function];
            function_info->duration += info->duration;
            if (function_info->line == -1 || function_info->line > info->line)
            {
                function_info->line = info->line;
            }
        }
    }

    for (auto x: result)
    {
        x->percentage = (double)x->duration / duration * 100;
    }

    result.sort(compare_function);
}


bool compare_file(Hotspot_file_info *first, Hotspot_file_info *second)
{
    return first->duration > second->duration;
}

void Hospot_results_implem::get_file_hotspots(std::list<Hotspot_file_info *> &result)
{
    int64_t duration = 0;
    std::unordered_map<std::string, Hotspot_file_info *> files;

    for (auto x: this->infos)
    {
        Hotspot_info *info = x.second;

        const char *file = info->file;

        if (file == NULL)
        {
            file = "";
        }

        if (files.count(file) == 0)
        {
            Hotspot_file_info *file_info = new Hotspot_file_info();
            file_info->duration = 0;
            file_info->file = file;
            files[file] = file_info;
            result.push_front(file_info);
        }

        Hotspot_file_info *file_info = files[file];
        duration += info->duration;
        file_info->duration += info->duration;
    }

    for (auto x: result)
    {
        x->percentage = (double)x->duration / duration * 100;
    }

    result.sort(compare_file);
}


void Hospot_results_implem::append_pc(Hotspot_pc &pc, int64_t start_timestamp, int64_t end_timestamp)
{
    Hotspot_info *info = this->infos[pc.pc];

    if (start_timestamp < pc.timestamp)
    {
        start_timestamp = pc.timestamp;
    }

    if (end_timestamp > pc.timestamp + pc.duration)
    {
        end_timestamp = pc.timestamp + pc.duration;
    }

    int64_t duration = end_timestamp - start_timestamp;

    if (info == NULL)
    {
        info = new Hotspot_info();
        info->pc = pc.pc;
        info->duration = duration;
        info->count = pc.count;
        this->infos[pc.pc] = info;

        Hotspot_debug *debug = this->hotspot.get(info->pc);
        if (debug != NULL)
        {
            info->function = debug->function;
            info->inlined_function = debug->inlined_function;
            info->file = debug->file;
            info->line = debug->line;
        }
        else
        {
            info->function = NULL;
            info->inlined_function = NULL;
            info->file = NULL;
            info->line = 0;
        }
    }
    else
    {
        info->duration += duration;
        info->count += pc.count;
    }
}


Hotspot_interval::Hotspot_interval()
{
    this->page = NULL;
    this->interval_page = NULL;
    this->parent = NULL;
    this->start_timestamp = -1;
    this->end_timestamp = -1;
}


void Hotspot_interval::set_interval_page(Hotspot_interval_page *interval_page)
{
    this->interval_page = interval_page;

    // Case where interval page and just been created and first page of lower level
    // is beeing registered
    if (this->start_timestamp == -1)
    {
        this->start_timestamp = interval_page->start_timestamp;
    }
    if (this->end_timestamp == -1)
    {
        this->end_timestamp = interval_page->end_timestamp;
    }
}


void Hotspot_interval::set_page(Hotspot_page *page)
{
    this->page = page;

    // Case where interval page and just been created and first page of lower level
    // is beeing registered
    if (this->start_timestamp == -1)
    {
        this->start_timestamp = page->get_start_timestamp();
    }
    if (this->end_timestamp == -1)
    {
        this->end_timestamp = page->get_end_timestamp();
    }
}


void Hotspot_interval::update(int64_t timestamp, uint32_t pc, uint64_t duration)
{
    if (this->start_timestamp == -1)
    {
        this->start_timestamp = timestamp;
    }

    this->end_timestamp = timestamp + duration;

    this->parent->update(timestamp, pc, duration);
}


void Hotspot_interval::get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp)
{
    if (this->page)
    {
        this->page->get_hotspots(hotspots, start_timestamp, end_timestamp);
    }
    else
    {
        this->interval_page->get_hotspots(hotspots, start_timestamp, end_timestamp);
    }
}


void Hotspot_interval_page::update(int64_t timestamp, uint32_t pc, uint64_t duration)
{
    Hotspot_pc *info = this->infos[pc];

    if (info == NULL)
    {
        info = new Hotspot_pc();
        this->infos[pc] = info;
    }

    info->pc = pc;
    info->duration += duration;
    info->count++;

    if (this->start_timestamp == -1)
    {
        this->start_timestamp = timestamp;
    }

    this->end_timestamp = timestamp + duration;

    if (this->parent)
    {
        this->parent->update(timestamp, pc, duration);
    }
}


Hotspot_interval_page::Hotspot_interval_page(int level)
{
    this->current = 0;
    this->level = level;
    this->parent = NULL;
    this->start_timestamp = -1;
    this->end_timestamp = -1;

    Hotspot_interval *prev = NULL;
    for (int i=0; i<HOTSPOT_INTERVAL_PAGE_SIZE; i++)
    {
        Hotspot_interval *interval = &this->intervals[i];
        interval->parent = this;
        interval->start_timestamp = -1;
        interval->prev = prev;
        prev = interval;
    }
}


void Hotspot_interval_page::register_page(Hotspot_page *page)
{
    page->set_parent(&this->intervals[this->current]);
    this->intervals[this->current++].set_page(page);
}

void Hotspot_interval_page::register_interval_page(Hotspot_interval_page *interval_page)
{
    if (this->current == 0 && interval_page->start_timestamp != -1)
    {
        this->start_timestamp = interval_page->start_timestamp;
    }
    if (interval_page->end_timestamp != -1)
    {
        this->end_timestamp = interval_page->end_timestamp;
    }

    for (auto &it: interval_page->infos)
    {
        Hotspot_pc *new_info = it.second;
        Hotspot_pc *info = this->infos[new_info->pc];
        if (info == NULL)
        {
            info = new Hotspot_pc();
            info->pc = new_info->pc;
            info->duration = new_info->duration;
            info->count = new_info->count;
            this->infos[new_info->pc] = info;
        }
        else
        {
            info->duration += new_info->duration;
            info->count = new_info->count;
        }
    }

    interval_page->set_parent(&this->intervals[this->current]);
    this->intervals[this->current++].set_interval_page(interval_page);
}


void Hotspot_interval_page::get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp)
{
    // If the page entirely fit the interval, just accounts page infos
    if (this->start_timestamp >= start_timestamp && this->end_timestamp <= end_timestamp)
    {
        for(auto& it: this->infos)
        {
            hotspots.append_pc(*it.second, start_timestamp, end_timestamp);
        }
    }
    // Otherwise, browse the internal intervals and account the ones falling into the interval
    else
    {
        for (int i=0; i<this->current; i++)
        {
            Hotspot_interval *interval = &this->intervals[i];

            if (interval->start_timestamp >= end_timestamp || interval->end_timestamp <= start_timestamp)
            {
                continue;
            }

            interval->get_hotspots(hotspots, start_timestamp, end_timestamp);
        }
    }
}


Hotspot_pc *Hotspot_page::alloc()
{
    Hotspot_pc *result = &this->infos[this->current];
    this->current++;
    return result;
}


Profiler_trace_hotspot::Profiler_trace_hotspot()
{
    this->current_hotspot_page = NULL;
    this->current_timestamp = -1;
    this->interval_pages.push_back({});
    this->last_level = -1;
}


void Profiler_trace_hotspot::add_interval_page()
{
    int level = 0;
    Hotspot_interval_page *interval_page = NULL;

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
            Hotspot_interval_page *page = new Hotspot_interval_page(level);
            if (this->interval_pages[level].size() > 0)
            {
                page->intervals[0].prev = &this->interval_pages[level].back()->intervals[HOTSPOT_INTERVAL_PAGE_SIZE - 1];
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


void Profiler_trace_hotspot::register_page(Hotspot_page *page)
{
    Hotspot_interval_page *interval_page = NULL;

    if (this->interval_pages[0].size() == 0 || this->interval_pages[0].back()->is_full())
    {
        this->add_interval_page();
    }

    this->interval_pages[0].back()->register_page(page);
}


void Profiler_trace_hotspot::get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp)
{
    if (this->last_level != -1)
    {
        this->interval_pages[this->last_level][0]->get_hotspots(hotspots, start_timestamp, end_timestamp);
    }
}


void Profiler_trace_hotspot::update(int64_t timestamp, uint64_t value)
{
    if (this->current_timestamp == -1)
    {
        this->last_pc = value;
    }
    else
    {
        if (this->current_hotspot_page == NULL || this->current_hotspot_page->is_full())
        {
            this->current_hotspot_page = new Hotspot_page();
            this->register_page(this->current_hotspot_page);
        }

        Hotspot_pc *info = this->current_hotspot_page->alloc();

        info->pc = this->last_pc;
        info->timestamp = this->current_timestamp;
        info->duration = timestamp - this->current_timestamp;
        info->count = 1;
        this->current_hotspot_page->parent->update(this->current_timestamp, info->pc, info->duration);
        this->last_pc = value;
    }

    this->current_timestamp = timestamp;
}

Hotspot_page::Hotspot_page()
{
    this->current = 0;
}

void Hotspot_page::get_hotspots(Hospot_results_implem &hotspots, int64_t start_timestamp, int64_t end_timestamp)
{
    for (int i=0; i<this->current; i++)
    {
        Hotspot_pc &info = this->infos[i];
        if (info.timestamp + info.duration <= start_timestamp || info.timestamp >= end_timestamp)
        {
            continue;
        }
        hotspots.append_pc(info, start_timestamp, end_timestamp);
    }
}


Hotspot::Hotspot()
{

}


void Hotspot::add_static_binary(std::string elf_file)
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

            this->infos[addr] = (Hotspot_debug) {
                .pc = addr,
                .function = function,
                .inlined_function = function,
                .file = file,
                .line = lineno
            };
        }
    }
}
