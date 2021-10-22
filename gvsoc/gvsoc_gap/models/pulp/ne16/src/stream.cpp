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
 * Authors: Francesco Conti, University of Bologna & GreenWaves Technologies (f.conti@unibo.it)
 */

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xrandom.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xpad.hpp"
#include <ne16.hpp>

// the NE16 can only access L1 memory in the range 0xY000_0000 -- 0xY001_FFFC, where Y=1 or 0
// in the model, Y is ignored
#define NE16_STREAM_L1_MASK 0x0001FFFF

Ne16StreamAccess::Ne16StreamAccess(
  Ne16 *ne16,
  int base_addr,
  int d0_length,
  int d0_stride,
  int d1_length,
  int d1_stride,
  int d2_length,
  int d2_stride,
  bool debug
) : ne16 ( ne16 ),
    base_addr     ( base_addr    ),
    d0_length   ( d0_length  ),
    d0_stride   ( d0_stride  ),
    d1_length   ( d1_length  ),
    d1_stride   ( d1_stride  ),
    d2_length  ( d2_length ),
    d2_stride  ( d2_stride ),
    debug         ( debug        ),
    current_addr  ( 0            )
{
  this->reset_iteration();
  if(this->debug) {
    this->print_config();
  }
}

int Ne16StreamAccess::get_base_addr() {
  return this->base_addr;
}
int Ne16StreamAccess::get_d0_length() {
  return this->d0_length;
}
int Ne16StreamAccess::get_d0_stride() {
  return this->d0_stride;
}
int Ne16StreamAccess::get_d1_length() {
  return this->d1_length;
}
int Ne16StreamAccess::get_d1_stride() {
  return this->d1_stride;
}
int Ne16StreamAccess::get_d2_length() {
  return this->d2_length;
}
int Ne16StreamAccess::get_d2_stride() {
  return this->d2_stride;
}

void Ne16StreamAccess::print_config() {
  std::cout << "[STREAMER] base_addr="  << std::hex << this->base_addr << std::dec << std::endl;
  std::cout << "[STREAMER] tot_length=" << this->d0_length << std::endl;
  std::cout << "[STREAMER] d0_stride="  << this->d0_stride << std::endl;
  std::cout << "[STREAMER] d0_length="  << this->d1_length << std::endl;
  std::cout << "[STREAMER] d1_stride="  << this->d1_stride << std::endl;
  std::cout << "[STREAMER] d1_length="  << this->d2_length << std::endl;
  std::cout << "[STREAMER] d2_stride="  << this->d2_stride << std::endl;
}

void Ne16StreamAccess::reset_iteration() {
  this->wa = 0;
  this->la = 0;
  this->ba = 0;
  this->wc = 1;
  this->lc = 1;
  this->bc = 1;
  this->oc = 0;
}

int Ne16StreamAccess::iterate() {
  if (this->d1_length < 0) {
    this->current_addr = this->base_addr + this->wa;
  }
  else if(this->d2_length < 0) {
    this->current_addr = this->base_addr + this->la + this->wa;
  }
  else {
    this->current_addr = this->base_addr + this->ba + this->la + this->wa;
  }
  this->oc++;
  if(this->debug) {
    std::cout << "[STREAMER] wa=" << this->wa << " la=" << this->la << " ba=" << this->ba << " oc=" << this->oc << std::endl;
    std::cout << "[STREAMER] wc=" << this->wc << " lc=" << this->lc << " bc=" << this->bc << " oc=" << this->oc << std::endl;
  }
  if((this->wc < this->d1_length) || (this->d1_length < 0)) {
    this->wa += this->d0_stride;
    this->wc += 1;
  }
  else if ((this->lc < this->d2_length) || (this->d2_length < 0)) {
    this->wa = 0;
    this->la += this->d1_stride;
    this->wc = 1;
    this->lc += 1;
  }
  else {
    this->wa = 0;
    this->la = 0;
    this->ba += this->d2_stride;
    this->wc = 1;
    this->lc = 1;
    this->bc += 1;
  }
  return this->current_addr;
}

template <class T>
Ne16VectorLoad<T>::Ne16VectorLoad(
  Ne16 *ne16,
  int base_addr,
  int d0_length,
  int d0_stride,
  int d1_length,
  int d1_stride,
  int d2_length,
  int d2_stride,
  bool debug
) : Ne16StreamAccess(ne16, base_addr, d0_length, d0_stride, d1_length, d1_stride, d2_length, d2_stride, debug) {
}

template <class T>
Ne16VectorLoad<T>::Ne16VectorLoad() : Ne16StreamAccess((Ne16 *) NULL, 0, 0, 0, 0, 0, 0, 0, false) {
}

template <class T>
xt::xarray<T> Ne16VectorLoad<T>::ex(int width, int64_t& cycles) {
  auto addr = this->iterate();
  uint8_t load_data[STREAM_MAX_WIDTH_BYTES];
  auto width_padded = width + 4;
  auto addr_padded = addr & ~0x3;
  auto width_words = width_padded*sizeof(T)/4;
  auto width_rem   = width_padded*sizeof(T)%4;
  int64_t max_latency = 0;
  for(auto i=0; i<width_words; i++) {
    this->ne16->io_req.init();
    this->ne16->io_req.set_addr(addr_padded+i*4 & NE16_STREAM_L1_MASK);
    this->ne16->io_req.set_size(4);
    this->ne16->io_req.set_data(load_data+i*4);
    this->ne16->io_req.set_is_write(false);
    int err = this->ne16->out.req(&this->ne16->io_req);
    if (err == vp::IO_REQ_OK) {
      int64_t latency = this->ne16->io_req.get_latency();
      if (latency > max_latency) {
        max_latency = latency;
      }
    }
    else {
      this->ne16->trace.fatal("Unsupported asynchronous reply\n");
    }
  }
  if(width_rem) {
    this->ne16->io_req.init();
    this->ne16->io_req.set_addr(addr_padded+width_words*4 & NE16_STREAM_L1_MASK);
    this->ne16->io_req.set_size(width_rem);
    this->ne16->io_req.set_data(load_data+width_words*4);
    this->ne16->io_req.set_is_write(false);
    int err = this->ne16->out.req(&this->ne16->io_req);
    if (err == vp::IO_REQ_OK) {
      // int64_t latency = this->ne16->io_req.get_latency();
      // if (latency > max_latency) {
      //   max_latency = latency;
      // }
    }
    else {
      this->ne16->trace.fatal("Unsupported asynchronous reply\n");
    }
  }
  std::ostringstream stringStream;

  if (this->ne16->trace_level == L3_ALL) {
    this->ne16->trace.msg(vp::trace::LEVEL_DEBUG, "Issuing read request (addr=0x%08x, size=%dB, latency=%d)\n", addr & NE16_STREAM_L1_MASK, width*sizeof(T), cycles+1);
  }
  xt::xarray<T> x = xt::zeros<T>({width});
  for(auto i=0; i<width; i++) {
    xt::view(x, i) = *(T *)(load_data + (addr & 0x3) + i*sizeof(T));
  }
  xt::print_options::set_line_width(1000);
  stringStream << "Read data: " << (this->ne16->trace_format?std::hex:std::dec) << x << std::dec << "\n";
  string s = stringStream.str();
  if (this->ne16->trace_level == L3_ALL) {
    this->ne16->trace.msg(vp::trace::LEVEL_DEBUG, s.c_str());
  }
  cycles += max_latency + 1;
  return x;
}

template <class T>
Ne16VectorStore<T>::Ne16VectorStore(
  Ne16 *ne16,
  int base_addr,
  int d0_length,
  int d0_stride,
  int d1_length,
  int d1_stride,
  int d2_length,
  int d2_stride,
  bool debug
) : Ne16StreamAccess(ne16, base_addr, d0_length, d0_stride, d1_length, d1_stride, d2_length, d2_stride, debug) {
}

template <class T>
Ne16VectorStore<T>::Ne16VectorStore() : Ne16StreamAccess((Ne16 *) NULL, 0, 0, 0, 0, 0, 0, 0, false) {
}

template <class T>
xt::xarray<T> Ne16VectorStore<T>::ex(xt::xarray<T> data, int width, int64_t& cycles, int32_t enable) {
  auto addr = this->iterate();
  uint8_t store_data[STREAM_MAX_WIDTH_BYTES];
  for(auto i=0; i<STREAM_MAX_WIDTH_BYTES; i++) {
    store_data[i] = 0;
  }
  for(auto i=0; i<width; i++) {
    *(T *)(store_data + i*sizeof(T)) = data(i);
  }
  auto width_bytes = width*sizeof(T);
  int64_t max_latency = 0;
  if(enable) {
    for(auto i=0; i<width_bytes; i++) {
      this->ne16->io_req.init();
      this->ne16->io_req.set_addr(addr+i & NE16_STREAM_L1_MASK);
      this->ne16->io_req.set_size(1);
      this->ne16->io_req.set_data(store_data+i);
      this->ne16->io_req.set_is_write(true);
      int err = this->ne16->out.req(&this->ne16->io_req);
      if (err == vp::IO_REQ_OK) {
        if(i%4 == 0) {  // apparently, for non-aligned bytes we get garbage latency
          int64_t latency = this->ne16->io_req.get_latency();
          if (latency > max_latency) {
            max_latency = latency;
          }
        }
      }
      else {
        this->ne16->trace.fatal("Unsupported asynchronous reply\n");
      }
    }
  }
  std::ostringstream stringStream;
  if (this->ne16->trace_level == L3_ALL) {
    this->ne16->trace.msg(vp::trace::LEVEL_DEBUG, "Issuing write request (addr=0x%08x, size=%dB, latency=%d)\n", addr & NE16_STREAM_L1_MASK, width*sizeof(T), cycles+max_latency+1);
  }
  xt::print_options::set_line_width(1000);
  if(enable) {
    stringStream << "Write data: " << (this->ne16->trace_format?std::hex:std::dec) << data << std::dec << "\n";
    string s = stringStream.str();
    if (this->ne16->trace_level == L3_ALL) {
      this->ne16->trace.msg(vp::trace::LEVEL_DEBUG, s.c_str());
    }
  }
  else {
    stringStream << "Write disabled" << "\n";
    string s = stringStream.str();
    if (this->ne16->trace_level == L3_ALL) {
      this->ne16->trace.msg(vp::trace::LEVEL_DEBUG, s.c_str());
    }
  }
  cycles += max_latency + 1;
  return data;
}

// template instantiations
template class Ne16VectorLoad<uint8_t>;
template class Ne16VectorStore<uint8_t>;
