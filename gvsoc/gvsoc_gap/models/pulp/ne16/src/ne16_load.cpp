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

void Ne16::load_setup() {
  // set up streamer to address input activations (byte-based)

  this->h_size_in  = (this->i_major < this->subtile_nb_ho-1) || (this->subtile_rem_hi==0) ? (this->fs == 1 ? 3 : 5) : this->subtile_rem_hi;
  this->w_size_in  = (this->j_major < this->subtile_nb_wo-1) || (this->subtile_rem_wi==0) ? (this->fs == 1 ? 3 : 5) : this->subtile_rem_wi;
  this->h_size_out = (this->i_major < this->subtile_nb_ho-1) || (this->subtile_rem_ho==0) ? 3 : this->subtile_rem_ho;
  this->w_size_out = (this->j_major < this->subtile_nb_wo-1) || (this->subtile_rem_wo==0) ? 3 : this->subtile_rem_wo;

  this->h_size_in_X_w_size_in = this->h_size_in * this->w_size_in;
  this->h_size_out_X_w_size_out = this->h_size_out * this->w_size_out;

  this->h_size_in_hw = (this->i_major<this->subtile_nb_ho-1 || this->subtile_rem_hi==0) ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_hi;
  this->w_size_in_hw = (this->j_major<this->subtile_nb_wo-1 || this->subtile_rem_wi==0) ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_wi;
 
  auto k_in_major = this->depthwise ? this->k_out_major : this->k_in_major_iter;

  if(this->trace_level == L3_ALL) {
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   h_size_in=%d\n", this->h_size_in);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   w_size_in=%d\n", this->w_size_in);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   h_size_out=%d\n", this->h_size_out);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   w_size_out=%d\n", this->w_size_out);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   h_size_in_X_w_size_in=%d\n", this->h_size_in_X_w_size_in);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   h_size_out_X_w_size_out=%d\n", this->h_size_out_X_w_size_out);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   k_in_major=%d\n", k_in_major);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   h_size_in_hw=%d\n", this->h_size_in_hw);
    this->trace.msg(vp::trace::LEVEL_DEBUG, "   w_size_in_hw=%d\n", this->w_size_in_hw);
  }

  auto infeat_hom_iter = this->FILTER_SIZE * this->infeat_d1_stride;
  auto infeat_wom_iter = this->FILTER_SIZE * this->infeat_d0_stride;

  auto base_addr_x = !this->mode_linear ? this->infeat_ptr + this->i_major*infeat_hom_iter + this->j_major*infeat_wom_iter + k_in_major*this->TP_IN :
                                          this->infeat_ptr + k_in_major*this->TP_IN*8 * (this->mode16 ? 2 : 2);

  this->vld_x = Ne16VectorLoad<uint8_t>(
    this,
    base_addr_x, // base_addr
    this->mode_linear ? (this->mode16 ? 32 : 16) : this->h_size_in_X_w_size_in, // word_length
    this->infeat_d0_stride, // word_stride
    this->mode_linear ? (this->mode16 ? 32 : 16) : this->w_size_in, // line_length
    this->infeat_d1_stride, // line_stride
    this->mode_linear ? -1 : this->h_size_in_X_w_size_in, // block_length
    this->infeat_d2_stride, // block_stride
    false
  );

  if(!this->mode_linear) {
    // NOTE: this looks very reasonable, but different from the v1 model
    this->load_i_fbuf_lim = this->h_size_in;
    this->load_j_fbuf_lim = this->w_size_in;

    this->load_k_in_lim = this->TP_IN;
    this->load_padding = {0, 0}; // std::vector<uint32_t>

    if(k_in_major == this->subtile_nb_ki-1 && this->subtile_rem_ki != this->TP_IN) { // last k_in tile, only if it requires padding
      this->load_k_in_lim = this->subtile_rem_ki;
      this->load_padding = {0, static_cast<uint32_t>(this->TP_IN-this->subtile_rem_ki)};
    }
    this->load_i_fbuf = 0;
    this->load_j_fbuf = 0;
  }
  else {
    this->load_fbuf_lim = this->mode16 ? 2*this->TP_IN : this->TP_IN;
    this->load_k_in_lim = this->mode16 ? 2*this->TP_IN : this->TP_IN;
    this->load_padding = {0, 0};
    if(k_in_major == this->subtile_nb_ki-1 && this->subtile_rem_ki != this->TP_IN && this->subtile_rem_ki != 0) { // last k_in tile, only if it requires padding
       this->load_k_in_lim = this->subtile_rem_ki;
       this->load_fbuf_lim = this->mode16 ? 2*this->subtile_rem_ki : this->subtile_rem_ki;
    }
    this->load_i_fbuf = 0;
    this->load_j_fbuf = 0;
  }

  xt::view(this->x_buffer, xt::all()) = 0;
  xt::view(this->x_buffer_linear, xt::all()) = 0;
  
}

int Ne16::load_cycle() { // not linear
  int64_t cycles = 0;
  xt::view(this->x_buffer, this->load_i_fbuf, this->load_j_fbuf, xt::all()) = xt::pad(this->vld_x.ex(this->load_k_in_lim, cycles), this->load_padding);
  return (int) cycles;
}

int Ne16::load_cycle_linear() {
  int64_t cycles = 0;
  auto x = xt::pad(this->vld_x.ex(this->TP_IN, cycles), this->load_padding);
  xt::view(this->x_buffer_linear, this->load_i_fbuf, xt::all()) = x;
  return (int) cycles;
}

void Ne16::load_do_padding() { // not linear

  // add explicit padding
  auto right_lim  = (5-this->padding_right  > this->w_size_in_hw) ? this->w_size_in_hw : 5-this->padding_right;
  auto bottom_lim = (5-this->padding_bottom > this->h_size_in_hw) ? this->h_size_in_hw : 5-this->padding_bottom;

  // implicit padding (on the right/bottom) and explicit padding (all dimensions) define
  // sixteen regions:
  // +-------+-------+-------+-------+
  // | TL    | T     | TR    | TRR   |
  // +-------+-------+-------+-------+
  // | L     | body  | R     | RR    |
  // +-------+-------+-------+-------+
  // | BL    | B     | BR    | BRR   |
  // +-------+-------+-------+-------+
  // | BBL   | BB    | BBR   | BBRR  |
  // +-------+-------+-------+-------+

  if(!this->mode16) {

    // top-left
    if(this->padding_left  > 0 && this->j_major==0 || this->padding_top > 0 && this->i_major==0) {
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(0, this->padding_left), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
  
    // top
    if(this->padding_top  > 0 && this->i_major==0) {
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(this->padding_left, right_lim), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
    
    // top-right
    if((this->padding_right > 0 && this->j_major==this->subtile_nb_wo-1 || this->padding_top > 0 && this->i_major==0) && (right_lim <= this->w_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(right_lim, w_size_in_hw), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
  
    // right
    if((this->padding_right > 0 && this->j_major==this->subtile_nb_wo-1) && (right_lim <= this->w_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(this->padding_top, bottom_lim), xt::range(right_lim, this->w_size_in_hw), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
  
    // bottom-right
    if((this->padding_right > 0 && this->j_major==this->subtile_nb_wo-1 || this->padding_bottom > 0 && this->i_major==this->subtile_nb_ho-1) && (right_lim <= this->w_size_in_hw) && (bottom_lim <= this->h_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(bottom_lim, h_size_in_hw), xt::range(right_lim, w_size_in_hw), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
  
    // bottom
    if((this->padding_bottom > 0 && this->i_major==this->subtile_nb_ho-1) && (bottom_lim <= this->h_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(bottom_lim, this->h_size_in_hw), xt::range(this->padding_left, right_lim), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
  
    // bottom-left
    if((this->padding_left > 0 && this->j_major==0 || this->padding_bottom > 0 && this->i_major==this->subtile_nb_ho-1) && (bottom_lim <= this->h_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(bottom_lim, this->h_size_in_hw), xt::range(0, this->padding_left), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }
  
    // left
    if(this->padding_left > 0 && this->j_major==0) {
      xt::view(this->x_buffer, xt::range(this->padding_top, bottom_lim), xt::range(0, this->padding_left), xt::range(0, this->load_k_in_lim)) = this->padding_value;
    }

  }
  else {

    // top-left
    if(this->padding_left  > 0 && this->j_major==0 || this->padding_top > 0 && this->i_major==0) {
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(0, this->padding_left), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(0, this->padding_left), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
  
    // top
    if(this->padding_top  > 0 && this->i_major==0) {
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(this->padding_left, right_lim), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(this->padding_left, right_lim), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
    
    // top-right
    if((this->padding_right > 0 && this->j_major==this->subtile_nb_wo-1 || this->padding_top > 0 && this->i_major==0) && (right_lim <= this->w_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(right_lim, w_size_in_hw), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(0, this->padding_top), xt::range(right_lim, w_size_in_hw), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
  
    // right
    if((this->padding_right > 0 && this->j_major==this->subtile_nb_wo-1) && (right_lim <= this->w_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(this->padding_top, bottom_lim), xt::range(right_lim, this->w_size_in_hw), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(this->padding_top, bottom_lim), xt::range(right_lim, this->w_size_in_hw), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
  
    // bottom-right
    if((this->padding_right > 0 && this->j_major==this->subtile_nb_wo-1 || this->padding_bottom > 0 && this->i_major==this->subtile_nb_ho-1) && (right_lim <= this->w_size_in_hw) && (bottom_lim <= this->h_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(bottom_lim, h_size_in_hw), xt::range(right_lim, w_size_in_hw), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(bottom_lim, h_size_in_hw), xt::range(right_lim, w_size_in_hw), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
  
    // bottom
    if((this->padding_bottom > 0 && this->i_major==this->subtile_nb_ho-1) && (bottom_lim <= this->h_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(bottom_lim, this->h_size_in_hw), xt::range(this->padding_left, right_lim), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(bottom_lim, this->h_size_in_hw), xt::range(this->padding_left, right_lim), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
  
    // bottom-left
    if((this->padding_left > 0 && this->j_major==0 || this->padding_bottom > 0 && this->i_major==this->subtile_nb_ho-1) && (bottom_lim <= this->h_size_in_hw)) {
      xt::view(this->x_buffer, xt::range(bottom_lim, this->h_size_in_hw), xt::range(0, this->padding_left), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(bottom_lim, this->h_size_in_hw), xt::range(0, this->padding_left), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }
  
    // left
    if(this->padding_left > 0 && this->j_major==0) {
      xt::view(this->x_buffer, xt::range(this->padding_top, bottom_lim), xt::range(0, this->padding_left), xt::range(0, this->load_k_in_lim,   2)) = this->padding_value & 0xff;
      xt::view(this->x_buffer, xt::range(this->padding_top, bottom_lim), xt::range(0, this->padding_left), xt::range(1, this->load_k_in_lim+1, 2)) = this->padding_value >> 8;
    }


  }

}

void Ne16::load_do_extract() {
  // extract x_array from x_buffer
  if(this->mode_linear) {
    xt::view(this->x_array, xt::all()) = 0;
    for(auto i=0; i<(this->mode16 ? 4 : 2); i++) { // spatial loop - implemented as a set of muxes
      for(auto j=0; j<8; j++) {
        xt::view(this->x_array, i, j, xt::all()) = xt::view(this->x_buffer_linear, i*8+j);
      }
    }
  }
  else if(this->fs == 3) {
    for(auto i_col=0; i_col<this->NR_COLUMN; i_col++) { // spatial loop - implemented as a set of muxes
      auto i = i_col / this->FILTER_SIZE;
      auto j = i_col % this->FILTER_SIZE;
      xt::xarray<uint8_t> x_buffer_view = xt::eval(xt::view(this->x_buffer, xt::range(i, i+this->FILTER_SIZE), xt::range(j, j+this->FILTER_SIZE), xt::all()));
      auto shape = xt::adapt(x_buffer_view.shape());
      xt::view(this->x_array, i_col, xt::all()) = x_buffer_view.reshape({shape[0] * shape[1], shape[2]});
    }
  }
  else { // in 1x1 mode, fill only the first qw rows
    xt::view(this->x_array, xt::all()) = 0;
    for(auto i_row=0; i_row<this->qw; i_row++) { // spatial loop - implemented as a set of muxes
      xt::xarray<uint8_t> x_buffer_view = xt::eval(xt::view(this->x_buffer, xt::range(0, this->FILTER_SIZE), xt::range(0, this->FILTER_SIZE), xt::all()));
      auto shape = xt::adapt(x_buffer_view.shape());
      xt::view(this->x_array, xt::all(), i_row) = x_buffer_view.reshape({shape[0] * shape[1], shape[2]});
    }
  }
}

void Ne16::load_filter_masking() {
  // filter masking
  xt::xarray<int32_t> W_mask = xt::ones<int32_t>({this->fs, this->fs});
  if(this->fs == 3) {
    if(this->filter_mask_top > 0)
      xt::view(W_mask, xt::range(0, this->filter_mask_top), xt::range(0, this->fs)) = 0;
    if(this->filter_mask_right > 0)
      xt::view(W_mask, xt::range(0, this->fs), xt::range(this->fs-this->filter_mask_right, this->fs)) = 0;
    if(this->filter_mask_bottom > 0)
      xt::view(W_mask, xt::range(this->fs-this->filter_mask_bottom, this->fs), xt::range(0, this->fs)) = 0;
    if(this->filter_mask_left > 0)
      xt::view(W_mask, xt::range(0, this->fs), xt::range(0, this->filter_mask_left)) = 0;
  }
  this->row_enable = xt::flatten(W_mask);
}

bool Ne16::load_exit_idx() {
  if(this->mode_linear) {
    if(this->load_i_fbuf == this->load_fbuf_lim-1) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    if(this->load_i_fbuf == this->load_i_fbuf_lim-1 && this->load_j_fbuf == this->load_j_fbuf_lim-1) {
      return true;
    }
    else {
      return false;
    }
  }
}

void Ne16::load_update_idx() {
  if(this->mode_linear) {
    this->load_i_fbuf++;
  }
  else {
    if(this->load_j_fbuf == this->load_j_fbuf_lim-1) {
      this->load_j_fbuf = 0;
      this->load_i_fbuf++;
    }
    else {
      this->load_j_fbuf++;
    }
  }
}
