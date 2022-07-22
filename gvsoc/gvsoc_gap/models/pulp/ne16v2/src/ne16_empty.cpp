/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
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
 * Authors: Francesco Conti, University of Bologna & GreenWaves Technologies (f.conti@unibo.it)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "vp/vp.hpp"
#include <vp/itf/io.hpp>

class Ne16 : public vp::component
{
    friend class Ne16_base;

public:
    Ne16(js::config *config);

    int build();
    void reset(bool active);


private:
    vp::io_master out;
    vp::io_slave in;
    vp::wire_master<bool> irq;

};

Ne16::Ne16(js::config *config)
    : vp::component(config)
{
}


void Ne16::reset(bool active)
{
}


int Ne16::build()
{
    this->new_master_port("out", &this->out);

    this->new_master_port("irq", &this->irq);

    this->new_slave_port("input", &this->in);

    return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Ne16(config);
}
