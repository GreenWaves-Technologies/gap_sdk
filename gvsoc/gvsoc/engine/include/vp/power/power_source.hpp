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


inline void vp::power::power_source::turn_on()
{
    this->is_on = true;
    this->is_dynamic_power_on = true;
    this->check();
}


inline void vp::power::power_source::turn_off()
{
    this->is_on = false;
    this->is_dynamic_power_on = false;
    this->check();
}

inline void vp::power::power_source::turn_dynamic_power_on()
{
    this->is_dynamic_power_on = true;
    this->check();
}

inline void vp::power::power_source::turn_dynamic_power_off()
{
    this->is_dynamic_power_on = false;
    this->check();
}


inline void vp::power::power_source::leakage_power_start()
{
    // Only start if leakage is defined
    if (this->leakage != -1)
    {
        this->is_leakage_power_started = true;
        this->check();
    }
}



inline void vp::power::power_source::leakage_power_stop()
{
    // Only stop if leakage is defined
    if (this->leakage != -1)
    {
        this->is_leakage_power_started = false;
        this->check();
    }
}



inline void vp::power::power_source::dynamic_power_start()
{
    // Only start accounting background power if it is is defined
    if (this->background_power != -1)
    {
        this->is_dynamic_power_started = true;
        this->check();
    }
}



inline void vp::power::power_source::dynamic_power_stop()
{
    // Only stop accounting background power if it is is defined
    if (this->background_power != -1)
    {
        this->is_dynamic_power_started = false;
        this->check();
    }
}



inline void vp::power::power_source::account_energy_quantum()
{
    // Only account energy is a quantum is defined
    if (this->is_on && this->is_dynamic_power_on && this->quantum != -1)
    {
        this->trace->inc_dynamic_energy(this->quantum);
    }
}
