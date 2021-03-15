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

void Ne16::normquant_shift_setup() {
  // set up streamer to address input activations (byte-based)
  auto tp = this->depthwise ? this->TP_IN : this->TP_OUT;
  auto base_addr_nqs = this->scale_shift_ptr + this->k_out_major*tp;

  this->vld_nqs = Ne16VectorLoad<uint8_t>(
    this,
    base_addr_nqs, // base_addr
    1, // word_length
    tp, // word_stride
    1, // line_length
    0, // line_stride
    1, // block_length
    0, // block_stride
    false
  );  
}

int  Ne16::normquant_shift_cycle() {
  int64_t cycles = 0;
  xt::view(this->nqs, xt::all()) = this->vld_nqs.ex(this->TP_OUT, cycles);
  return (int) cycles;
}

void Ne16::normquant_mult_setup() {
  // set up streamer to address input activations (byte-based)
  auto tp = this->depthwise ? this->TP_IN : this->TP_OUT;
  auto base_addr_nq = this->scale_ptr + this->k_out_major*tp*(this->normalization_bits/8);

  this->vld_nq = Ne16VectorLoad<uint8_t>(
    this,
    base_addr_nq, // base_addr
    tp/(this->normalization_bits/8), // word_length
    4, // word_stride
    this->normalization_bits, // line_length
    0, // line_stride
    1, // block_length
    0, // block_stride
    false
  );

  this->nq_lim = this->normalization_bits;
  if(this->k_out_major == this->subtile_nb_ko-1 && this->subtile_rem_ko != this->TP_OUT && this->subtile_rem_ko != 0) { // last k_in tile, only if it requires padding
    this->nq_lim = this->normalization_bits == 32 ? this->subtile_rem_ko :
                   this->normalization_bits == 16 ? this->subtile_rem_ko / 2 + (this->subtile_rem_ko % 2 ? 1 : 0) :
                                                    this->subtile_rem_ko / 4 + (this->subtile_rem_ko % 4 ? 1 : 0) ;
  }
  this->nq_iter = 0;
}

int  Ne16::normquant_mult_cycle() {
  int64_t cycles = 0;
  xt::xarray<uint8_t> nq = this->vld_nq.ex(4, cycles);
  // FIXME casting --> 1) load NQS; 2) load NQ and compute MULT; 3) load NQB and compute shift+bias
  if(this->normalization_bits == 8) {
    auto nmult = 4;
    for(auto i=0; i<nmult; i++) {
      for(auto col=0; col<this->NR_COLUMN; col++) {
        xt::view(this->accum, this->nq_iter*nmult+i, col) = xt::view(this->accum, this->nq_iter*nmult+i, col) * xt::view(nq, i);
      }
    }
  }
  else if(this->normalization_bits == 16) {
    auto nmult = 2;
    xt::xarray<uint16_t> nq16 = xt::zeros<uint16_t>({2});
    xt::view(nq16, 0) = xt::view(nq, 0) + (xt::view(nq, 1) << 8);
    xt::view(nq16, 1) = xt::view(nq, 2) + (xt::view(nq, 3) << 8);
    for(auto i=0; i<2; i++) {
      for(auto col=0; col<this->NR_COLUMN; col++) {
        xt::view(this->accum, this->nq_iter*nmult+i, col) = xt::view(this->accum, this->nq_iter*nmult+i, col) * xt::view(nq16, i);
      }
    }
  }
  else if(this->normalization_bits == 32) {
    xt::xarray<uint32_t> nq32 = xt::zeros<uint32_t>({1});
    xt::view(nq32, 0) = xt::view(nq, 0) + (xt::view(nq, 1) << 8) + (xt::view(nq, 2) << 16) + (xt::view(nq, 3) << 24);
    for(auto col=0; col<this->NR_COLUMN; col++) {
      xt::view(this->accum, this->nq_iter, col) = xt::view(this->accum, this->nq_iter, col) * xt::view(nq32, 0);
    }
  }
  return (int) cycles;
}

bool Ne16::normquant_mult_exit_idx() {
  if(this->nq_iter == this->nq_lim-1) {
    return true;
  }
  else {
    return false;
  }
}

void Ne16::normquant_mult_update_idx() {
  this->nq_iter++;
}

void Ne16::normquant_bias_setup() {
  // set up streamer to address input activations (byte-based)
  auto tp = this->depthwise ? this->TP_IN : this->TP_OUT;
  auto base_addr_nqb = this->scale_bias_ptr + this->k_out_major*tp*4;

  this->vld_nqb = Ne16VectorLoad<uint8_t>(
    this,
    base_addr_nqb, // base_addr
    tp/8, // word_length
    32, // word_stride
    tp/8, // line_length
    0, // line_stride
    1, // block_length
    0, // block_stride
    false
  );

  this->nqb_lim = this->normalization_bits;
  if(this->k_out_major == this->subtile_nb_ko-1 && this->subtile_rem_ko != this->TP_OUT && this->subtile_rem_ko != 0) { // last k_in tile, only if it requires padding
    this->nqb_lim = this->subtile_rem_ko;
  }
  this->nqb_iter = 0;
}

int  Ne16::normquant_bias_cycle() {
  int64_t cycles = 0;
  xt::xarray<int32_t> nqb32 = xt::zeros<int32_t>({8});
  if(this->norm_option_bias) {
    xt::xarray<uint8_t> nqb = this->vld_nqb.ex(32, cycles);
    for(auto i=0; i<8; i++) {
      xt::view(nqb32, i) = xt::cast<int32_t>(xt::view(nqb, i*4) + (xt::view(nqb, i*4+1) << 8) + (xt::view(nqb, i*4+2) << 16) + (xt::view(nqb, i*4+3) << 24));
    }
    for(auto col=0; col<this->NR_COLUMN; col++) {
      xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col) = (xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col) + nqb32);
    }
    if(this->norm_option_shift) {
      for(auto col=0; col<this->NR_COLUMN; col++) {
        xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col) = xt::cast<int32_t>(xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col)) >> xt::view(this->nqs, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8));
      }
    }
    else {
      for(auto col=0; col<this->NR_COLUMN; col++) {
        xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col) = xt::cast<int32_t>(xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col)) >> this->quantization_right_shift;
      }
    }
  }
  else {
    if(this->norm_option_shift) {
      for(auto col=0; col<this->NR_COLUMN; col++) {
        xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col) = (xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col)) >> xt::view(this->nqs, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8));
      }
    }
    else {
      for(auto col=0; col<this->NR_COLUMN; col++) {
        xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col) = (xt::view(this->accum, xt::range(this->nqb_iter*8, (this->nqb_iter+1)*8), col)) >> this->quantization_right_shift;
      }
    }
  }
  return (int) cycles;
}

bool Ne16::normquant_bias_exit_idx() {
  if(this->nqb_iter == 3) {
    return true;
  }
  else {
    return false;
  }
}

void Ne16::normquant_bias_update_idx() {
  this->nqb_iter++;
}
