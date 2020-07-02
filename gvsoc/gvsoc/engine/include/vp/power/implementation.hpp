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