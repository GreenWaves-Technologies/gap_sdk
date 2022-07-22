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
#include <vp/signal.hpp>

vp::block::block(block *parent)
    : parent(parent)
{
    if (parent)
    {
        parent->add_block(this);
    }
}


void vp::block::reset_all(bool active)
{
    for (block *block: this->subblocks)
    {
        block->reset_all(active);
    }

    for (signal *signal: this->signals)
    {
        signal->reset(active);
    }

    this->reset(active);
}


void vp::block::add_block(block *block)
{
    this->subblocks.push_back(block);
}

void vp::block::add_signal(vp::signal *signal)
{
    this->signals.push_back(signal);
}
