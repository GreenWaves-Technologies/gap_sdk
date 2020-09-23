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

#ifndef __GV__GVSOC_PROXY_HPP__
#define __GV__GVSOC_PROXY_HPP__

#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

class Gvsoc_proxy
{
public:
    Gvsoc_proxy(std::string config_path);

    int open();

    void run();

    int64_t pause();

    void close();

    void add_event_regex(std::string regex);

    void remove_event_regex(std::string regex);

    void add_trace_regex(std::string regex);

    void remove_trace_regex(std::string regex);

private: 

    void proxy_loop();

    std::mutex mutex;
    std::condition_variable cond;
    int req_pipe[2];
    int reply_pipe[2];
    std::string config_path;
    std::thread *loop_thread;

    bool running;
    int64_t stopped_timestamp;

};


#endif