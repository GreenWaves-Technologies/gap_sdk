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

void Ne16::constant_setup() {
  this->h_size_in  = this->i_major<this->subtile_nb_ho ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_hi;
  this->w_size_in  = this->j_major<this->subtile_nb_wo ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_wi;
  this->h_size_out = this->i_major<this->subtile_nb_ho ? this-> FILTER_SIZE : this->subtile_rem_ho;
  this->w_size_out = this->j_major<this->subtile_nb_wo ? this-> FILTER_SIZE : this->subtile_rem_wo;

  this->h_size_in_hw = (this->i_major<this->subtile_nb_ho-1 || this->subtile_rem_hi==0) ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_hi;
  this->w_size_in_hw = (this->j_major<this->subtile_nb_wo-1 || this->subtile_rem_wi==0) ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_wi;
  
  // compute these products also with pure multiplexing
  this->h_size_in_X_w_size_in = (this->i_major<this->subtile_nb_ho  && this->j_major<this->subtile_nb_wo)  ? (this->fs == 3 ? this->F_BUFFER_SIZE*this->F_BUFFER_SIZE : this->FILTER_SIZE*this->FILTER_SIZE) :
                                (this->i_major<this->subtile_nb_ho  && this->j_major>=this->subtile_nb_wo) ? (this->fs == 3 ? this->F_BUFFER_SIZE*this->subtile_rem_wi     : this->FILTER_SIZE*this->subtile_rem_wi)   :
                                (this->i_major>=this->subtile_nb_ho && this->j_major<this->subtile_nb_wo)  ? (this->fs == 3 ? this->subtile_rem_hi*this->F_BUFFER_SIZE     : this->subtile_rem_hi*this->FILTER_SIZE)   :
                                                                                                (this->fs == 3 ? this->subtile_rem_hi*this->subtile_rem_wi         : this->subtile_rem_hi*this->subtile_rem_wi);
  this->h_size_out_X_w_size_out = (this->i_major<this->subtile_nb_ho  && this->j_major<this->subtile_nb_wo)  ? this->FILTER_SIZE*this->FILTER_SIZE :
                                  (this->i_major<this->subtile_nb_ho  && this->j_major>=this->subtile_nb_wo) ? this->FILTER_SIZE*this->subtile_rem_wo  :
                                  (this->i_major>=this->subtile_nb_ho && this->j_major<this->subtile_nb_wo)  ? this->subtile_rem_ho*this->FILTER_SIZE  :
                                                                                                               this->subtile_rem_ho*this->subtile_rem_wo;

  this->k_in_major = this->depthwise ? this->k_out_major : this->k_in_major_iter;
}

void Ne16::streamin_setup() {

  auto tp = this->depthwise ? this->TP_IN : this->TP_OUT;

  auto outfeat_hom_iter = this->FILTER_SIZE * this->outfeat_d2_stride;
  auto outfeat_wom_iter = this->FILTER_SIZE * this->outfeat_d1_stride;
 
  auto base_addr_streamin = this->outfeat_ptr + this->i_major*outfeat_hom_iter + this->j_major*outfeat_wom_iter + this->k_out_major*tp*this->quantization_bits/8;

  auto k_out_lim = this->depthwise ? 1 :
                   (this->k_out_major == this->subtile_nb_ko-1 && this->subtile_rem_ko != this->TP_OUT && this->subtile_rem_ko != 0) ? this->subtile_rem_ko : this->TP_OUT;

  auto h_size_out_X_w_size_out_with_strb = (k_out_lim <= 8)  ? 9  :
                                           (k_out_lim <= 16) ? 18 :
                                           (k_out_lim <= 24) ? 27 : 36;

  this->col_enable = xt::zeros<int32_t>({3,3});
  for(auto i=0; i<this->h_size_out; i++) {
    for(auto j=0; j<this->w_size_out; j++) {
      xt::view(this->col_enable, i, j) = 1;
    }
  }

  this->vld_streamin = Ne16VectorLoad<uint8_t>(
    this,
    base_addr_streamin, // base_addr
    h_size_out_X_w_size_out_with_strb*(k_out_lim/8 > 0 ? k_out_lim/8 + (k_out_lim%8 == 0 ? 0 : 1) : 1), // word_length
    this->outfeat_d0_stride, // word_stride
    k_out_lim/8 > 0 ? k_out_lim/8 + (k_out_lim%8 == 0 ? 0 : 1) : 1, //w_size_out, // line_length
    this->outfeat_d1_stride, // line_stride
    this->FILTER_SIZE, // block_length
    this->outfeat_d2_stride, // block_stride
    false
  );

  this->streamin_k_out_lim = this->TP_OUT/8; ///
  if(this->k_out_major == this->subtile_nb_ko-1 && this->subtile_rem_ko != this->TP_OUT && this->subtile_rem_ko != 0) { // last k_in tile, only if it requires padding
    this->streamin_k_out_lim = this->subtile_rem_ko/8 + (this->subtile_rem_ko%8 == 0 ? 0 : 1);
  }
  this->streamin_k_out_iter = 0;
  this->streamin_i_out_iter = 0;
  this->streamin_j_out_iter = 0;

}

// iterated h_size_out_X_w_size_out times
int Ne16::streamin_cycle() {
  int64_t cycles = 0;

  xt::xarray<uint8_t> xx = xt::zeros<uint8_t>({32});
  auto k_out_last = (this->streamin_k_out_iter+1)*8;
  if(this->k_out_major == this->subtile_nb_ko-1 && this->subtile_rem_ko != this->TP_OUT && this->subtile_rem_ko != 0) { // last k_in tile, only if it requires padding
    k_out_last = k_out_last < this->subtile_rem_ko ? k_out_last : this->subtile_rem_ko;
  }
  if(this->col_enable(this->streamin_i_out_iter, this->streamin_j_out_iter)) {
    xx = this->vld_streamin.ex((k_out_last-this->streamin_k_out_iter*8)*4, cycles);
  }
  for (auto i=this->streamin_k_out_iter*8; i<k_out_last; i++) {
    xt::view(this->accum, i, this->streamin_i_out_iter*this->FILTER_SIZE+this->streamin_j_out_iter) = 
      (xt::cast<int32_t>(xt::view(xx, (i-this->streamin_k_out_iter*8)*4+0)) << 0 ) |
      (xt::cast<int32_t>(xt::view(xx, (i-this->streamin_k_out_iter*8)*4+1)) << 8 ) |
      (xt::cast<int32_t>(xt::view(xx, (i-this->streamin_k_out_iter*8)*4+2)) << 16) |
      (xt::cast<int32_t>(xt::view(xx, (i-this->streamin_k_out_iter*8)*4+3)) << 24);
  }
  return (int) cycles;
}

bool Ne16::streamin_exit_idx() {
  auto h_size_out = this->mode_linear ? 1 : this->h_size_out;
  auto w_size_out = this->mode_linear ? 1 : this->w_size_out;
  if(this->streamin_i_out_iter == h_size_out-1 && this->streamin_j_out_iter == w_size_out-1 && this->streamin_k_out_iter == this->streamin_k_out_lim-1) {
    return true;
  }
  else {
    return false;
  }
}

void Ne16::streamin_update_idx() {
  if(this->streamin_k_out_iter < this->streamin_k_out_lim-1) {
    this->streamin_k_out_iter++;
  } 
  else if(this->streamin_j_out_iter < this->FILTER_SIZE-1) {
    this->streamin_k_out_iter = 0;
    this->streamin_j_out_iter++;
  }
  else {
    this->streamin_k_out_iter = 0;
    this->streamin_j_out_iter = 0;
    this->streamin_i_out_iter++;
  }
}

