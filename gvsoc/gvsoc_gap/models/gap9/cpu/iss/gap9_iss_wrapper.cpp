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


#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include "archi/gvsoc/gvsoc.h"
#include "iss.hpp"
#include "gap9_iss_wrapper.hpp"

gap9_iss_wrapper::gap9_iss_wrapper(js::config *config)
: iss_wrapper(config)
{
}


void gap9_iss_wrapper::target_open()
{
    iss_decoder_item_t *isa = iss_isa_get(this, "f");
    int core_id = this->cpu.config.mhartid;

    for (int i=0; i<iss_get_isa_set()->nb_resources; i++)
    {
        iss_resource_t *resource = &iss_get_isa_set()->resources[i];
        int instance_id = core_id % resource->nb_instances;
        this->cpu.resources[i] = resource->instances[instance_id];
    }
}


extern "C" vp::component *vp_constructor(js::config *config)
{
  return new gap9_iss_wrapper(config);
}
