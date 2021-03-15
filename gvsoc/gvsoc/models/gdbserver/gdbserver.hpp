/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Andreas Traber
 */

#ifndef __VP_GDB_SERVER_HPP__
#define __VP_GDB_SERVER_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include "vp/gdbserver/gdbserver_engine.hpp"


class Rsp;


class Gdb_server : public vp::component, vp::Gdbserver_engine
{
public:
    Gdb_server(js::config *config);
    
    void pre_pre_build();
    int build();
    void start();

    int io_access(uint32_t addr, int size, uint8_t *data, bool is_write);

    int register_core(vp::Gdbserver_core *core);
    void signal(vp::Gdbserver_core *core);
    int set_active_core(int id);
    vp::Gdbserver_core *get_core(int id=-1);
    std::vector<vp::Gdbserver_core *> get_cores() { return this->cores_list; }

    void lock() { this->get_time_engine()->lock(); }
    void unlock() { this->get_time_engine()->unlock(); }

    vp::trace     trace;


private:
    static void handle(void *__this, vp::clock_event *event);
    Rsp *rsp;
    vp::io_req io_req;
    vp::clock_event *event;
    vp::io_master out;
    bool pending_access;
    std::unordered_map<int, vp::Gdbserver_core *> cores;
    std::vector<vp::Gdbserver_core *> cores_list;
    int active_core;
};

#endif