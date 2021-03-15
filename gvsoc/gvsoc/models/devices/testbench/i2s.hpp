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

#ifndef __TESTBENCH_I2S_VERIF_HPP__
#define __TESTBENCH_I2S_VERIF_HPP__

#include "testbench.hpp"

class Testbench;

class I2s_verif
{
public:
    I2s_verif(Testbench *top, vp::i2s_master *itf, int itf_id, int cs, int mem_size);

    vp::trace trace;
    vp::i2s_master *itf;
};

#endif