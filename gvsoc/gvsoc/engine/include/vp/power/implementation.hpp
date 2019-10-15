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



#ifndef __VP_POWER_IMPLEMENTATION_HPP__
#define __VP_POWER_IMPLEMENTATION_HPP__

#include "vp/vp_data.hpp"
#include "vp/power/power_engine.hpp"


inline void vp::power_trace::account_quantum(double quantum)
{
  this->incr(quantum);
  this->trace.event_real_pulse(this->top->get_period(), quantum, 0);
}

inline double vp::power_trace::get_value()
{
  if (this->timestamp < this->top->get_time())
  {
    this->timestamp = this->top->get_time();
    this->value = 0;
  }
  return this->value;
}

inline double vp::power_trace::get_total()
{
  this->account_power();
  return this->total;
}

inline double vp::power_trace::get_total_leakage()
{
  this->account_leakage_power();
  return this->total_leakage;
}

inline void vp::power_trace::flush()
{
  this->account_power();
  this->account_leakage_power();
}

inline void vp::power_trace::incr(double quantum, bool is_leakage)
{
  this->get_value();

  if (is_leakage)
  {
    this->total_leakage += quantum;
  }
  else
  {
    this->value += quantum;
    this->total += quantum;
  }

  if (this->top_trace)
    this->top_trace->incr(quantum, is_leakage);

  if (!is_leakage)
    this->trace.event_real_pulse(this->top->get_period(), this->value, 0);
}

#endif