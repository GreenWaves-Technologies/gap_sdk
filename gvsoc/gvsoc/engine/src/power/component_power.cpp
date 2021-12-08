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



vp::power::component_power::component_power(vp::component &top)
    : top(top)
{
}



void vp::power::component_power::build()
{
    this->new_power_trace("power_trace", &this->power_trace);

    this->engine = (vp::power::engine *)top.get_service("power");

    for (auto trace : this->traces)
    {
        this->get_engine()->reg_trace(trace);
    }
}



int vp::power::component_power::new_power_trace(std::string name, vp::power::power_trace *trace, vp::power::power_trace *parent)
{
    if (trace->init(&top, name, parent))
        return -1;

    this->traces.push_back(trace);

    return 0;
}



int vp::power::component_power::new_power_source(std::string name, power_source *source, js::config *config, vp::power::power_trace *trace)
{
    if (trace == NULL)
    {
        trace = &this->power_trace;
    }

    if (source->init(&top, name, config, trace))
        return -1;

    source->setup(VP_POWER_DEFAULT_TEMP, VP_POWER_DEFAULT_VOLT, VP_POWER_DEFAULT_FREQ);

    return 0;
}



double vp::power::component_power::get_power_from_childs()
{
    double result = 0.0;
    for (auto &x : this->top.get_childs())
    {
        result += x->power.get_power_from_self_and_childs();
    }
    return result;
}


double vp::power::component_power::get_power_from_self_and_childs()
{
    double result = 0.0;

    for (auto &x : this->traces)
    {
        result += x->get_power();
    }

    result += this->get_power_from_childs();

    return result;
}



void vp::power::component_power::get_report_energy_from_childs(double *dynamic, double *leakage)
{
    for (auto &x : this->top.get_childs())
    {
        x->power.get_report_energy_from_self_and_childs(dynamic, leakage);
    }
}



void vp::power::component_power::get_report_energy_from_self_and_childs(double *dynamic, double *leakage)
{
    for (auto &x : this->traces)
    {
        double trace_dynamic, trace_leakage;
        x->get_report_energy(&trace_dynamic, &trace_leakage);
        *dynamic += trace_dynamic;
        *leakage += trace_leakage;
    }

    this->get_report_energy_from_childs(dynamic, leakage);
}



void vp::power::component_power::dump(FILE *file, double total)
{
    for (auto x:this->traces)
    {
        double dynamic, leakage;
        x->get_report_power(&dynamic, &leakage);
        double percentage = 0.0;
        if (total != 0.0)
        {
            percentage = (dynamic + leakage) / total;
        }
        fprintf(file, "%s; %.12f; %.12f; %.12f; %.6f\n", x->trace.get_full_path().c_str(), dynamic, leakage, dynamic + leakage, percentage);
    }
}



void vp::power::component_power::dump_child_traces(FILE *file, double total)
{
    for (auto &x : this->top.get_childs())
    {
        x->power.dump(file, total);
    }
}
