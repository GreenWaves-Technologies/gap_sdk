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

#include "vp/vp.hpp"
#include "vp/trace/trace.hpp"



void vp::power::power_source::setup(double temp, double volt, double freq)
{
    // When temperature, voltage or frequency is modified, only impact dynamic energy quantum,
    // dynamic background power or leakage if they are defined, which is the case if they are not -1
    if (this->quantum != -1)
    {
        this->quantum = this->table->get(temp, volt, freq);
    }
    if (this->background_power != -1)
    {
        this->background_power = this->table->get(temp, volt, freq);
    }
    if (this->leakage != -1)
    {
        this->leakage = this->table->get(temp, volt, freq);
    }
}



int vp::power::power_source::init(component *top, std::string name, js::config *source_config, vp::power::power_trace *trace)
{
    this->top = top;
    this->trace = trace;

    this->quantum = -1;
    this->background_power = -1;
    this->leakage = -1;

    if (source_config == NULL)
    {
        //snprintf(vp_error, VP_ERROR_SIZE, "Didn't find power trace (name: %s)",  name.c_str());
        return -1;
    }

    // Parse the JSON config and extract power numbers
    for (auto elem: source_config->get_childs())
    {
        js::config *config = elem.second;
        bool is_leakage = false;

        if (elem.first == "dynamic")
        {

        }
        else if (elem.first == "leakage")
        {
            is_leakage = true;
        }
        else
        {
            top->get_trace()->fatal("Unknown power source type: %s\n", elem.first.c_str());
            return -1;
        }

        try
        {
            js::config *type_cfg = config->get("type");
            if (type_cfg == NULL)
            {
                //snprintf(vp_error, VP_ERROR_SIZE, "Didn't find power trace type (name: %s)", name.c_str());
                return -1;
            }

            js::config *unit_cfg = config->get("unit");
            if (unit_cfg == NULL)
            {
                //snprintf(vp_error, VP_ERROR_SIZE, "Didn't find power trace unit (name: %s)", name.c_str());
                return -1;
            }


            if (unit_cfg->get_str() == "pJ")
            {
                // Setting it to 0 will make sure it is defined and the engine will properly maintain it
                this->quantum = 0;
            }
            else if (unit_cfg->get_str() == "W")
            {
                if (is_leakage)
                {
                    // Setting it to 0 will make sure it is defined and the engine will properly maintain it
                    this->leakage = 0;
                }
                else
                {
                    // Setting it to 0 will make sure it is defined and the engine will properly maintain it
                    this->background_power = 0;
                }
            }
            else
            {
                snprintf(vp_error, VP_ERROR_SIZE, "Unknown unit (name: %s, unit: %s)", name.c_str(), unit_cfg->get_str().c_str());
                return -1;
            }


            if (type_cfg->get_str() == "linear")
            {
                js::config *values = config->get("values");
                if (values == NULL)
                {
                    snprintf(vp_error, VP_ERROR_SIZE, "Didn't find any value for linear power model");
                    return -1;
                }

                this->table = new Linear_table(values);
            }
            else
            {
                snprintf(vp_error, VP_ERROR_SIZE, "%s", type_cfg->get_str().c_str());
                return -1;
            }
        }
        catch (std::logic_error &e)
        {
            snprintf(vp_error, VP_ERROR_SIZE, "%s", e.what());
            return -1;
        }
    }

    return 0;  
}
