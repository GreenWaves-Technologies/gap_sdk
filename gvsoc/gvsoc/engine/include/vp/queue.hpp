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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#pragma once

#include <string.h>

namespace vp {

    class queue_elem;
    class block;

    class queue : public block
    {
    public:
        queue(block *parent);
        void push_back(queue_elem *elem);
        void push_front(queue_elem *elem);
        queue_elem *head();
        queue_elem *pop();
        bool empty();
        void reset(bool active);
    private:
        static void cancel_callback(void *__this, vp::queue_elem *elem);
        queue_elem *first=NULL;
        queue_elem *last;
    };

    class queue_elem
    {
        friend class queue;

    public:
        void cancel();

    protected:
        queue_elem *next;
        void *cancel_this;
        void (*cancel_callback)(void *, vp::queue_elem *);
    };
};
