/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#include "iss.hpp"



void iss_resource_init(iss_t *iss)
{
    // Initialize all the resources and assign the first instance by default
    iss->cpu.resources.resize(iss_get_isa_set()->nb_resources);

    for (int i=0; i<iss_get_isa_set()->nb_resources; i++)
    {
        iss_resource_t *resource = &iss_get_isa_set()->resources[i];
        for (int j=0; j<resource->nb_instances; j++)
        {
            iss_resource_instance_t *instance = new iss_resource_instance_t;
            instance->latency = resource->latency;
            instance->bandwidth = resource->bandwidth;
            instance->cycles = 0;
            resource->instances.push_back(instance);
        }

        iss->cpu.resources[i] = resource->instances[0];
    }
}


// Called when an instruction with an associated resource is scheduled
iss_insn_t *iss_resource_offload(iss_t *iss, iss_insn_t *insn)
{
    // First get the instance associated to this core for the resource associated to this instruction
    iss_resource_instance_t *instance = iss->cpu.resources[insn->resource_id];
    int64_t cycles = 0;

    // Check if the instance is ready to accept an access
    if (iss->get_cycles() < instance->cycles)
    {
        // If not, account the number of cycles until the instance becomes available
        cycles = instance->cycles - iss->get_cycles();
        // And account the access on the instance. The time taken by the access is indicated by the resource bandwidth
        instance->cycles += instance->bandwidth;
    }
    else
    {
        // The instance is available, just account the time taken by the access, indicated by the resource bandwidth
        instance->cycles = iss->get_cycles() + instance->bandwidth;
    }

    // Account the latency of the resource on the core, as the result is available after the resource latency
    iss->cpu.state.insn_cycles += cycles + instance->latency - 1;  

    // Now that timing is modeled, execute the instruction
    return insn->resource_handler(iss, insn);
}