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

#include <ne16.hpp>

void Ne16::debug_x_buffer() {
  if(this->mode_linear) {
    std::ostringstream stringStream;
    stringStream << "x_buffer[32,16] = \n" << (this->trace_format?std::hex:std::dec) << std::setw(2) << this->x_buffer_linear << std::dec << "\n";
    std::string copyOfStr = stringStream.str();
    this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
  }
  else {
    std::ostringstream stringStream;
    stringStream << "x_buffer[5,5,16] = \n" << (this->trace_format?std::hex:std::dec) << std::setw(2) << this->x_buffer << std::dec << "\n";
    std::string copyOfStr = stringStream.str();
    this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
  }
}

void Ne16::debug_x_array() {
  // if(this->mode_linear) {
  //   this->trace.msg(vp::trace::LEVEL_DEBUG, "x_buffer_linear[32,16] = \n");
  //   for (auto i=0; i<32; i++) {
  //     for (auto k=0; k<16; k++) {
  //       this->trace.msg(vp::trace::LEVEL_DEBUG, "  %02x", xt::view(this->x_buffer, i, k));
  //     }
  //     this->trace.msg(vp::trace::LEVEL_DEBUG, "\n");
  //   }
  // }
  // else {
    std::ostringstream stringStream;
    stringStream << "x_array[9,9,16] = \n" << xt::print_options::threshold(10000) << (this->trace_format?std::hex:std::dec) << std::setw(2) << this->x_array << std::dec << "\n";
    std::string copyOfStr = stringStream.str();
    this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
  // }
}

void Ne16::debug_accum(){
  std::ostringstream stringStream;
  stringStream << "accum[9,32] = \n" << (this->trace_format?std::hex:std::dec) << std::setw(8) << xt::cast<int32_t>(this->accum) << std::dec << "\n";
  std::string copyOfStr = stringStream.str();
  this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
}

// void Ne16::debug_psum_column(){
//   std::ostringstream stringStream;
//   stringStream << "psum_column[9] = \n" << (this->trace_format?std::hex:std::dec) << std::setw(8) << xt::cast<int32_t>(this->psum_column) << std::dec << "\n";
//   std::string copyOfStr = stringStream.str();
//   this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
// }

void Ne16::debug_psum_block(){
  std::ostringstream stringStream;
  stringStream << "psum_block[9,9] = \n" << (this->trace_format?std::hex:std::dec) << std::setw(8) << xt::cast<int32_t>(this->psum_block) << std::dec << "\n";
  std::string copyOfStr = stringStream.str();
  this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
}
