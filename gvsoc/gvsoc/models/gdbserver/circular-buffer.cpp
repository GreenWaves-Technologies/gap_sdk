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

#include "circular-buffer.hpp"

#include <string.h>

#ifndef min
#define min(a, b) ((a<b)?(a):(b))
#endif

size_t CircularCharBuffer::write_copy(const void * s, size_t l) {
  size_t copied = 0;
char * blk;
  size_t blk_len;
  while (copied <l && !full) {
    write_block((void**)&blk, &blk_len);
    if (blk_len <= 0) break;
    size_t to_copy = min(l, blk_len);
    memcpy(blk, s, to_copy);
    commit_write(to_copy);
    copied += to_copy;
  }
  return copied;
}

size_t CircularCharBuffer::read_copy(void *s, size_t l) {
  size_t copied = 0;
  char * blk;
  size_t blk_len;
  while (copied<l && size() > 0) {
    read_block((void**)&blk, &blk_len);
    size_t to_copy = min(l, blk_len);
    memcpy(s, blk, to_copy);
    commit_read(to_copy);
    copied += to_copy;
  }
  return copied;
}

void CircularCharBuffer::reset() {
  head = tail = 0; full = false;
}

bool CircularCharBuffer::is_empty() {
  return head == tail and !full;
}

bool CircularCharBuffer::is_full() {
  return full;
}

size_t CircularCharBuffer::size() {
  return (full?max_size:(head<=tail?tail-head:(max_size - head + tail)));
}

void CircularCharBuffer::commit_write(size_t l) {
  if (l > 0) {
    tail += l;
    if (tail >= max_size) tail -= max_size;
    // printf("write: head %lu tail %lu\n", head, tail);
    if (tail == head) full = true;
  }
}

void CircularCharBuffer::write_block(void **s, size_t *l) {
  if (is_empty()) {
    head = tail = 0;
    *s = &buf[0]; *l = max_size;
  } else if (is_full()) {
    *s = NULL; *l = 0;
  } else if (tail < head) {
    *s = &buf[tail]; *l = head - tail;  
  } else {
    *s = &buf[tail]; *l = max_size - tail;
  }
  // printf("writeb: tail %lu len %lu\n", tail, *l);
}

void CircularCharBuffer::commit_read(size_t l) {
  if (l > 0) {
    head += l;
    if (head >= max_size) head -= max_size;
    // printf("read: head %lu tail %lu\n", head, tail);
    full = false;
  }
}

void CircularCharBuffer::read_block(void **s, size_t *l) {
  if (is_empty()) {
    *s = NULL; *l = 0;
  } else if (tail <= head) {
    *s = &buf[head]; *l = max_size - head;  
  } else {
    *s = &buf[head]; *l = tail - head;
  }
  // printf("readb: head %lu len %lu\n", head, *l);
}

#undef min