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



int vp::power::power_trace::init(component *top, std::string name, vp::power::power_trace *parent)
{
    this->top = top;
    top->traces.new_trace_event_real(name, &this->trace);
    this->dynamic_energy_for_cycle = 0;
    this->report_dynamic_energy = 0;
    this->report_leakage_energy = 0;
    this->curent_cycle_timestamp = 0;

    // If no trace parent is specified, take the default one of the parent component
    if (parent == NULL)
    {
        vp::component *component = top->get_parent();
        if (component)
        {
            parent = component->power.get_power_trace();
        }
    }

    this->parent = parent;

    this->trace.event_real(0);

    this->current_dynamic_power = 0;
    this->current_dynamic_power_timestamp = 0;

    this->current_leakage_power = 0;
    this->current_leakage_power_timestamp = 0;

    this->trace_event = this->top->event_new((void *)this, vp::power::power_trace::trace_handler);

    return 0;
}



void vp::power::power_trace::trace_handler(void *__this, vp::clock_event *event)
{
    // This handler is used to resynchronize the VCD trace after a quantum of energy has been accounted,
    // since it has to be somehow removed from vcd trace value in the next cycle
    vp::power::power_trace *_this = (vp::power::power_trace *)__this;
    // Just redump the VCD trace, this will recompute teh instant power and the quantum will automatically be removed
    _this->dump_vcd_trace();
}



void vp::power::power_trace::report_start()
{
    // Since the report start may be triggered in the middle of several events
    // for power consumptions, include what has already be accounted
    // in the same cycle.
    this->report_dynamic_energy = this->get_dynamic_energy_for_cycle();
    this->report_leakage_energy = 0;
    this->report_start_timestamp = this->top->get_time();
}



void vp::power::power_trace::get_report_energy(double *dynamic, double *leakage)
{
    *dynamic = this->get_report_dynamic_energy();
    *leakage = this->get_report_leakage_energy();
}



void vp::power::power_trace::get_report_power(double *dynamic, double *leakage)
{
    double childs_dynamic = 0, childs_leakage = 0;

    // To get the power on the report window, we just get the total energy and divide by the window duration
    this->top->power.get_report_energy_from_childs(&childs_dynamic, &childs_leakage);
    *dynamic = (childs_dynamic + this->get_report_dynamic_energy()) / (this->top->get_time() - this->report_start_timestamp);
    *leakage = (childs_leakage + this->get_report_leakage_energy()) / (this->top->get_time() - this->report_start_timestamp);
}



void vp::power::power_trace::dump(FILE *file)
{
    fprintf(file, "Trace path; Dynamic power (W); Leakage power (W); Total (W); Percentage\n");

    double dynamic, leakage;
    this->get_report_power(&dynamic, &leakage);
    double total = dynamic + leakage;

    fprintf(file, "%s; %.12f; %.12f; %.12f; 1.0\n", this->trace.get_full_path().c_str(), dynamic, leakage, total);

    this->top->power.dump_child_traces(file, total);

    fprintf(file, "\n");
}



void vp::power::power_trace::dump_vcd_trace()
{
    // TODO this is wasting time and should be removed once fake component such as time domain and trace domain
    // are not in the component hierarchy anymore
    if (this->top->get_path() == "")
        return;

    double power = 0.0;

    // To dump the VCD trace, we need to compute the instant power, since this is what is reported.
    // This is easy for background and leakage power. For enery quantum, we get the amount of energy for the current
    // cycle and compute the instant power using the clock engine period.

    // Some component do not have clocks. They cannot use energy quantum but they can still use background
    // power and leakage
    if (this->top->get_clock())
    {
        int64_t period = this->top->get_period();
        if (period != 0)
        {
            power += this->get_dynamic_energy_for_cycle() / period;
        }
    }
    double power_background = this->current_dynamic_power + this->current_leakage_power;

    // Also account the power from childs since VCD traces are hierarchical
    double childs_power = this->top->power.get_power_from_childs();
    this->current_power = power + power_background + childs_power;

    // Dump the instant power to trace
    this->trace.event_real(current_power);

    // If there was a contribution from energy quantum, schedule an event in the next cycle so that we dump again 
    // the trace since teh quantum implicitely disappears and overal power is modified
    if (!this->trace_event->is_enqueued() && power > 0)
    {
        this->top->event_enqueue(this->trace_event, 1);
    }

    // Notify the parent that this trace was dumped so that the upper traces can be dumped as well
    if (this->parent)
    {
        this->parent->dump_vcd_trace();
    }
}



void vp::power::power_trace::account_dynamic_power()
{
    // We need to compute the energy spent on the current windows since we are starting a new one with different power.

    // First measure the duration of the windows
    int64_t diff = this->top->get_time() - this->current_dynamic_power_timestamp;

    if (diff > 0)
    {
        // Then energy based on the current power. Note that this can work only if the
        // power was constant over the period, which is the case, since this function is called
        // before any modification to the power.
        double energy = this->current_dynamic_power * diff;
        this->report_dynamic_energy += energy;

        // And update the timestamp to the current one to start a new window
        this->current_dynamic_power_timestamp = this->top->get_time();
    }
}



void vp::power::power_trace::account_leakage_power()
{
    // We need to compute the energy spent on the current windows since we are starting a new one with different power.

    // First measure the duration of the windows
    int64_t diff = this->top->get_time() - this->current_leakage_power_timestamp;
    if (diff > 0)
    {
        // Then energy based on the current power. Note that this can work only if the
        // power was constant over the period, which is the case, since this function is called
        // before any modification to the power.
        double energy = this->current_leakage_power * diff;
        this->report_leakage_energy += energy;

        // And update the timestamp to the current one to start a new window
        this->current_leakage_power_timestamp = this->top->get_time();
    }
}



void vp::power::power_trace::inc_dynamic_energy(double quantum)
{
    // Since we need to account the energy for the current amount of the cycle, check if it needs to be flushed
    this->flush_dynamic_energy_for_cycle();

    // Then account it to both the total amount and to the cycle amount
    this->dynamic_energy_for_cycle += quantum;
    this->report_dynamic_energy += quantum;

    // Redump VCD trace since teh instant power is impacted
    this->dump_vcd_trace();
}



void vp::power::power_trace::inc_dynamic_power(double power_incr)
{
    // Leakage and dynamic are handled differently since they are reported separately,
    // In both cases, first compute the power on current period, start a new one,
    // and change the power so that it is constant over the period, to properly
    // compute the energy.
    this->account_dynamic_power();
    this->current_dynamic_power += power_incr;

    // Redump VCD trace since teh instant power is impacted
    this->dump_vcd_trace();
}



void vp::power::power_trace::inc_leakage_power(double power_incr)
{
    // Leakage and dynamic are handled differently since they are reported separately,
    // In both cases, first compute the power on current period, start a new one,
    // and change the power so that it is constant over the period, to properly
    // compute the energy.
    this->account_leakage_power();
    this->current_leakage_power += power_incr;

    // Redump VCD trace since teh instant power is impacted
    this->dump_vcd_trace();
}
