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

#ifndef __VP_POWER_POWER_HPP__
#define __VP_POWER_POWER_HPP__

#include "json.hpp"
#include "vp/vp_data.hpp"

namespace vp {

  #define VP_POWER_DEFAULT_TEMP  25
  #define VP_POWER_DEFAULT_VOLT  1.2
  #define VP_POWER_DEFAULT_FREQ  50

  class Linear_table;

  class power_engine;

  class power_trace
  {
  public:
    int init(component *top, std::string name);

    inline bool get_active() { return trace.get_event_active(); }

    inline void account_quantum(double quantum);

    void set_power(double quantum, bool is_leakage);

    inline void incr(double quantum, bool is_leakage=false);

    inline double get_value();

    inline double get_total();

    inline double get_total_leakage();

    bool is_dumped() { return this->dumped; }

    power_trace *get_top_trace();

    void clear();
    
    void flush();

    void dump(FILE *file);

    void collect();

    void reg_top_trace(vp::power_trace *trace);
    void reg_child_trace(vp::power_trace *trace);

    void get(double *dynamic, double *leakage);

    vp::trace     trace;

  private:
    void account_power();
    void account_leakage_power();

    component *top;
    power_trace *top_trace = NULL;
    std::vector<power_trace *> child_traces;
    double value;
    double total;
    double total_leakage;
    int64_t timestamp;
    int64_t last_clear_timestamp;

    double current_power;
    int64_t current_power_timestamp;

    double current_leakage_power;
    int64_t current_leakage_power_timestamp;

    bool dumped;
  };

  class power_source
  {
    friend class component_power;

  public:
    inline void account_event() { this->trace->account_quantum(this->quantum); }
    inline void power_on() { if (!this->is_on) this->trace->set_power(this->quantum, this->is_leakage); this->is_on = true; }
    inline void power_off() { if (this->is_on) this->trace->set_power(-this->quantum, this->is_leakage); this->is_on = false; }

    inline double get_quantum() { return this->quantum; }


  protected:
    int init(component *top, std::string name, js::config *config, power_trace *trace, bool is_leakage);
    void setup(double temp, double volt, double freq);

  private:
    Linear_table *table = NULL;
    double quantum;
    component *top;
    power_trace *trace;
    bool is_on = false;
    bool is_leakage;
  };

};

#endif