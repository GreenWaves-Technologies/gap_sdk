/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#ifndef __VP_TIME_SCHEDULER_HPP__
#define __VP_TIME_SCHEDULER_HPP__

#include <vp/time/time_engine.hpp>

namespace vp {

    class time_event;

    typedef void (time_event_meth_t)(void *, time_event *event);

    class time_scheduler : public time_engine_client
    {
    public:
        time_scheduler(js::config *config);

        time_event *time_event_new(time_event_meth_t *meth);

        void time_event_del(time_event *event);

        time_event *enqueue(time_event *event, int64_t time);

        int64_t exec();

    private:
        time_event *first_event;
    };


    #define TIME_EVENT_PAYLOAD_SIZE 64
    #define TIME_EVENT_NB_ARGS 8

    class time_event
    {

        friend class time_scheduler;

    public:

        time_event(time_scheduler *comp, time_event_meth_t *meth);

        time_event(time_scheduler *comp, void *_this, time_event_meth_t *meth) 
        : comp(comp), _this(_this), meth(meth), enqueued(false) {}

        inline int get_payload_size() { return TIME_EVENT_PAYLOAD_SIZE; }
        inline uint8_t *get_payload() { return payload; }

        inline int get_nb_args() { return TIME_EVENT_NB_ARGS; }
        inline void **get_args() { return args; }

        inline bool is_enqueued() { return enqueued; }

        int64_t get_time() { return time; }

    private:
        uint8_t payload[TIME_EVENT_PAYLOAD_SIZE];
        void *args[TIME_EVENT_NB_ARGS];
        time_scheduler *comp;
        void *_this;
        time_event_meth_t *meth;
        time_event *next;
        bool enqueued;
        int64_t time;
    };    

};

inline vp::time_event *vp::time_scheduler::time_event_new(vp::time_event_meth_t *meth)
{
    return new time_event(this, meth);
}

inline void vp::time_scheduler::time_event_del(vp::time_event *event)
{
    delete event;
}

#endif
