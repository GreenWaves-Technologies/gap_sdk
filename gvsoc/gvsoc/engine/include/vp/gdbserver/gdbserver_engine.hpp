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

#ifndef __VP_GDBSERVER_GDBSERVER_ENGINE_HPP__
#define __VP_GDBSERVER_GDBSERVER_ENGINE_HPP__

#include "vp/vp_data.hpp"
#include "vp/component.hpp"

namespace vp
{
    class Gdbserver_core
    {
    public:
        typedef enum
        {
            running,
            stopped
        } state;

        virtual int gdbserver_get_id() = 0;
        virtual std::string gdbserver_get_name() = 0;
        virtual int gdbserver_reg_set(int reg, uint8_t *value) = 0;
        virtual int gdbserver_reg_get(int reg, uint8_t *value) = 0;
        virtual int gdbserver_regs_get(int *nb_regs, int *reg_size, uint8_t *value) = 0;
        virtual int gdbserver_stop() = 0;
        virtual int gdbserver_cont() = 0;
        virtual int gdbserver_stepi() = 0;
        virtual int gdbserver_state() = 0;
    };


    class Gdbserver_engine
    {
    public:
        virtual int register_core(Gdbserver_core *core) = 0;
        virtual void signal(Gdbserver_core *core) = 0;
    };

};

#endif
