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
#include <vp/power/power_engine.hpp>
#include <regex.h>
#include <vector>
#include <thread>
#include <string.h>


class power_manager : public vp::power_engine
{
public:

  power_manager(const char *config);

  int build();

  void start_capture();

  void stop_capture();

  void reg_trace(vp::power_trace *trace);

private:
  std::vector<vp::power_trace *> traces;
};

void power_manager::start_capture()
{
  for (auto trace: this->traces)
  {
    trace->clear();
  }
}

void power_manager::stop_capture()
{
  FILE *file = fopen("power_report.csv", "w");
  if (file == NULL)
  {
    vp_warning_always(&this->warning, "Failed to open power report file (path: %s)\n", "power_report.csv");
    return;
  }

  fprintf(file, "Trace path; Dynamic power (W); Leakage power (W); Total (W);");

  for (auto trace: this->traces)
  {
    if (!trace->is_dumped())
      trace->get_top_trace()->dump(file);
  }
}

void power_manager::reg_trace(vp::power_trace *trace)
{
  this->traces.push_back(trace);
}


vp::power_engine::power_engine(const char *config)
  : vp::component(config)
{
  new_service("power", static_cast<power_engine *>(this));
}

power_manager::power_manager(const char *config)
: vp::power_engine(config)
{

}

int power_manager::build()
{
  return 0;
}

extern "C" void *vp_constructor(const char *config)
{
  void *engine = (void *)new power_manager(config);

  return engine;
}
