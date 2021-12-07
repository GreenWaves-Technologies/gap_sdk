/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#pragma once

#include "vp/vp_data.hpp"



inline double vp::power::power_trace::get_power()
{
    return this->current_power;
}



inline double vp::power::power_trace::get_dynamic_energy_for_cycle()
{
    // First check if the current energy is for an old cycle
    this->flush_dynamic_energy_for_cycle();

    // And return the current total
    return this->dynamic_energy_for_cycle;
}



inline void vp::power::power_trace::flush_dynamic_energy_for_cycle()
{
    // Clear the current total if it is not for the current cycle
    if (this->curent_cycle_timestamp < this->top->get_time())
    {
        this->curent_cycle_timestamp = this->top->get_time();
        this->dynamic_energy_for_cycle = 0;
    }
}



inline double vp::power::power_trace::get_report_dynamic_energy()
{
    // First convert background power to energy
    this->account_dynamic_power();
    
    // And return the current total
    return this->report_dynamic_energy;
}



inline double vp::power::power_trace::get_report_leakage_energy()
{
    // First convert leakage power to energy
    this->account_leakage_power();

    // And return the current total
    return this->report_leakage_energy;
}
