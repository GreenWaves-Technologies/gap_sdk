/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <vp/vp.hpp>
#include <vp/itf/clk.hpp>
#include <vp/trace/trace_engine.hpp>
#include <regex.h>
#include <vector>
#include <thread>
#include <string.h>

class trace_domain : public vp::trace_engine
{

public:

  trace_domain(const char *config);

  void set_trace_level(const char *trace_level);
  void add_paths(int events, int nb_path, const char **paths);
  void add_path(int events, const char *path);
  void reg_trace(vp::trace *trace, int event, string path, string name);

  int build();
  void start();

  int get_max_path_len() { return max_path_len; }

  int exchange_max_path_len(int max_len) {
    if (max_len > max_path_len) max_path_len = max_len;
    return max_path_len;
  }

  int get_trace_level() { return this->trace_level; }

private:

  std::vector<regex_t *> path_regex;
  std::vector<std::string> path_regex_file;
  std::vector<regex_t *> events_path_regex;
  std::vector<string> events_file;
  int max_path_len = 0;
  vp::trace_level_e trace_level = vp::TRACE;
  std::vector<vp::trace *> init_traces;
  std::map<std::string, FILE *> trace_files;
};


vp::trace_engine::trace_engine(const char *config)
  : event_dumper(this), vp::component(config), first_trace_to_dump(NULL)
{

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  for (int i=0; i<TRACE_EVENT_NB_BUFFER; i++)
  {
    event_buffers.push_back(new char[TRACE_EVENT_BUFFER_SIZE]);
  }
  current_buffer = event_buffers[0];
  event_buffers.erase(event_buffers.begin());
  current_buffer_size = 0;
  this->first_pending_event = NULL;

  thread = new std::thread(&trace_engine::vcd_routine, this);
}

trace_domain::trace_domain(const char *config)
: vp::trace_engine(config)
{

}



void trace_domain::reg_trace(vp::trace *trace, int event, string path, string name)
{
  this->traces_array.push_back(trace);
  trace->id = this->traces_array.size() - 1;

  int len = path.size() + name.size() + 1;
  if (len > max_path_len) max_path_len = len;

  string full_path;

  if (name[0] != '/')
    full_path = path + "/" + name;
  else
    full_path = name;

  traces_map[full_path] = trace;

  trace->trace_file = stdout;

  int index = 0;
  for (auto& x: event ? events_path_regex : path_regex) {

    if (regexec(x, full_path.c_str(), 0, NULL, 0) == 0)
    {
      if (event)
      {
        vp::Event_trace *event_trace;
        if (trace->is_real)
          event_trace = event_dumper.get_trace_real(full_path, this->events_file[index]);
        else if (trace->is_string)
          event_trace = event_dumper.get_trace_string(full_path, this->events_file[index]);
        else
          event_trace = event_dumper.get_trace(full_path, this->events_file[index], trace->width);
        trace->set_event_active(true);
        trace->event_trace = event_trace;
      }
      else
      {
        std::string file_path = path_regex_file[index];
        if (file_path == "")
        {
          trace->trace_file = stdout;
        }
        else
        {
          if (this->trace_files[file_path] == NULL)
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
    index++;
  }
}

int trace_domain::build()
{
  new_service("trace", static_cast<trace_engine *>(this));

  auto vcd_traces = get_js_config()->get("vcd/traces");

  if (vcd_traces != NULL)
  {
    for (auto x: vcd_traces->get_childs())
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
  for (auto x: this->init_traces)
  {
    x->event(NULL);
  }
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
    events_path_regex.push_back(regex);
    events_file.push_back((char *)file_path);
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
    path_regex.push_back(regex);
    path_regex_file.push_back(file_path);
  }

  regcomp(regex, path, 0);
}

void trace_domain::add_paths(int events, int nb_path, const char **paths)
{
  for (int i=0; i<nb_path; i++)
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

extern "C" void vp_trace_add_paths(void *comp, int events, int nb_path, const char **paths)
{
  ((trace_domain *)comp)->add_paths(events, nb_path, paths);
}

extern "C" void vp_trace_level(void *comp, const char *level)
{
  ((trace_domain *)comp)->set_trace_level(level);
}

extern "C" int vp_trace_exchange_max_path_len(void *comp, int max_len)
{
  return ((trace_domain *)comp)->exchange_max_path_len(max_len);
}

extern "C" void *vp_constructor(const char *config)
{
  return (void *)new trace_domain(config);
}
