/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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