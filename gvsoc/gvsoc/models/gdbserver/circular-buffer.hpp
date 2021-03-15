/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies SA
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
 * Authors: Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include <stdint.h>
#include <memory>

#define DEFAULT_BUFFER_SIZE (16 * 1024)

class CircularCharBuffer {
public:
    CircularCharBuffer(size_t size) :
        buf(std::unique_ptr<char[]>(new char[size])),
        max_size(size)
    { }

    size_t write_copy(const void * s, size_t l);
    void write_block(void **s, size_t *l);
    void commit_write(size_t l);
    size_t read_copy(void *s, size_t l);
    void read_block(void **s, size_t *l);
    void commit_read(size_t l);
    void reset();
    bool is_empty();
    bool is_full();
    size_t size();
    size_t capacity() { return max_size; }
    size_t available() { return max_size - size(); }

private:
    std::unique_ptr<char[]> buf;
    size_t head = 0;
    size_t tail = 0;
    const size_t max_size;
    bool full = false;
};
#endif