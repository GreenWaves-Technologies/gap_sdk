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

void Ne16::k_in_major_update_idx() {
  this->k_in_major_iter++;
  if(this->trace_level == L3_ALL) {
    this->trace.msg(vp::trace::LEVEL_DEBUG, "  k_out_major=%d\n", this->k_out_major);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "  i_major=%d\n", this->i_major);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "  j_major=%d\n", this->j_major);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "  k_in_major_iter=%d\n", k_in_major);
  }
}

void Ne16::high_update_idx() {
  if(this->j_major == this->subtile_nb_wo-1 && this->i_major == this->subtile_nb_ho-1) {
    this->k_out_major++;
    this->i_major = 0;
    this->j_major = 0;
    this->k_in_major_iter = 0;
  }
  else if(this->j_major == this->subtile_nb_wo-1) {
    this->i_major++;
    this->j_major = 0;
    this->k_in_major_iter = 0;
  }
  else {
    this->j_major++;
    this->k_in_major_iter = 0;
  }
}
