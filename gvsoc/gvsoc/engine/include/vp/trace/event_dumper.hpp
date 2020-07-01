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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __VP_TRACE_EVENT_DUMPER_HPP__
#define __VP_TRACE_EVENT_DUMPER_HPP__

#include <stdio.h>
#include <unordered_map>

namespace vp {

  class Event_dumper;

  class Event_file 
  {
  public:
    virtual void dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask) {}
    virtual void close() {}
    virtual void add_trace(string name, int id, int width, bool is_real, bool is_string) {}

  protected:

    int64_t last_timestamp = -1;
    FILE *file;
    bool header_dumped = false;
  };

  class Event_trace
  {
  public:
    Event_trace(string trace_name, Event_file *file, int width, bool is_real, bool is_string);
    void reg(int64_t timestamp, uint8_t *event, int width, uint8_t flags, uint8_t *flag_mask);
    inline void dump(int64_t timestamp) { file->dump(timestamp, id, this->buffer, this->width, this->is_real, this->is_string, this->flags, this->flags_mask); }
    bool is_real = false;
    bool is_string;
    Event_trace *next;
    bool is_enqueued;
    int width;
    int bytes;

  private:
    Event_file *file;
    int id;
    uint8_t *buffer;
    uint8_t flags;
    uint8_t *flags_mask;

  };

  class Event_dumper
  {
  public:
    Event_dumper(vp::component *comp) : comp(comp) {}
    Event_trace *get_trace(string trace_name, string file_name, int width, bool is_real=false, bool is_string=false);
    Event_trace *get_trace_real(string trace_name, string file_name);
    Event_trace *get_trace_string(string trace_name, string file_name);
    void close();

    vp::component *comp;

  private:
    std::map<std::string, Event_trace *> event_traces;
    std::map<std::string, Event_file *> event_files;
  };

  class Vcd_file : public Event_file
  {
  public:
    Vcd_file(Event_dumper *dumper, string path);
    void close();
    void add_trace(string name, int id, int width, bool is_real, bool is_string);
    void dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask);

  private:
    string parse_path(string path, bool begin);
  };

  class Lxt2_file : public Event_file
  {
  public:
    Lxt2_file(Event_dumper *dumper, string path);
    void close();
    void add_trace(string name, int id, int width, bool is_real, bool is_string);
    void dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask);

  private:
    struct lxt2_wr_trace *trace;
    std::vector<struct lxt2_wr_symbol *> symbols;
  };


  class Fst_file : public Event_file
  {
  public:
    Fst_file(Event_dumper *dumper, string path);
    void close();
    void add_trace(string name, int id, int width, bool is_real, bool is_string);
    void dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask);

  private:
    string parse_path(string path, bool begin);

    void *writer;
    std::vector<uint32_t> vars;
  };


  class Raw_file : public Event_file
  {
  public:
    Raw_file(Event_dumper *dumper, string path);
    void close();
    void add_trace(string name, int id, int width, bool is_real, bool is_string);
    void dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask);

  private:
    void *dumper;
    std::unordered_map<int, void *> traces;
  };

};

#endif
