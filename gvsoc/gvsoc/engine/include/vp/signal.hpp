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

#include <stdint.h>

namespace vp {

    class signal
    {
    public:
        signal(block *parent, int64_t reset);
        void set(int64_t value) { this->value = value; }
        void setu(uint64_t value) { this->value = value; }
        int64_t get() { return this->value; }
        uint64_t getu() { return this->value; }
        void reset(bool active);
    private:
        int64_t value;
        int64_t reset_value;
    };
};
