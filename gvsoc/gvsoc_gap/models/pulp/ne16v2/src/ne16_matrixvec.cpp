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

// as the internal max precision of NE16 is 32 bits, this is emulated by casting x to 32 bits here
xt::xarray<int64_t> __NormQuant(
  xt::xarray<int64_t> x,
  xt::xarray<int32_t> kappa_bn,
  xt::xarray<int32_t> lambda_bn,
  int32_t shift_reqnt,
  int clip_min,
  int clip_max,
  bool use_rounding,
  bool use_clip,
  bool use_cast
) {
  if(use_clip) {
    if(use_cast)
      return xt::clip((xt::cast<int32_t>(x) * kappa_bn + lambda_bn + (use_rounding ? 1<<(shift_reqnt-1) : 0)) >> shift_reqnt, clip_min, clip_max);
    else
      return xt::clip((x * kappa_bn + lambda_bn + (use_rounding ? 1<<(shift_reqnt-1) : 0)) >> shift_reqnt, clip_min, clip_max);
  }
  else
    return (xt::cast<int32_t>(x) * kappa_bn + lambda_bn + (use_rounding ? 1<<(shift_reqnt-1) : 0)) >> shift_reqnt;
}

xt::xarray<uint8_t> __WeightUnpack(
  xt::xarray<uint8_t> w,
  int                 size,
  bool                mode16
) {
  w = w.reshape({size,2,1});
  xt::xarray<uint8_t> wu = xt::zeros<uint8_t>({size,2,8});
  xt::view(wu, xt::all(), xt::all()) = xt::view(w, xt::all(), xt::all());
  wu = (wu >> xt::linspace(0, 7, 8).reshape({1, 1, 8})) & 0x1;
  if(mode16) {
    return xt::hstack(xt::xtuple(wu.reshape({size*2, 8}), wu.reshape({size*2, 8})));
  }
  return wu.reshape({size, 2*8});
}

xt::xarray<int64_t> __BinConvBlock(
  xt::xarray<uint8_t> w,
  xt::xarray<uint8_t> x,
  int scale=0,
  bool mode16=false
) {
  if(mode16) {
    
    xt::xarray<int64_t> wx_lo = xt::view(w, xt::range(0, 8)) * xt::view(x, xt::range(0, 16, 2)); // FIXME size
    xt::xarray<int64_t> wx_hi = xt::view(w, xt::range(0, 8)) * xt::view(x, xt::range(1, 17, 2)); // FIXME size
    auto wx = wx_hi * 256 + wx_lo;
    return xt::sum(wx, 0) * scale;
  }
  return xt::sum(w * x, 0) * scale;
}

void Ne16::__BinConvArray(
  xt::xarray<uint8_t>& weight,
  int                  scale,
  int                  idx,
  xt::xarray<int32_t>  block_enable_linear,
  xt::xarray<int32_t>  row_enable,
  xt::xarray<int32_t>  mac_enable,
  bool                 weight_shift,
  bool                 weight_invert,
  bool                 use_row_as_scale,
  bool                 mode16,
  bool                 mode_linear
) {
  for(auto c=0; c<this->NR_COLUMN; c++) { // spatial loop - over columns
    xt::view(this->psum_column, c) = 0;
    for(auto r=0; r<this->COLUMN_SIZE; r++) { // spatial loop - over blocks in a column
      if(row_enable(r) == 0) // row disabling to implement filter masks
        continue;
      auto scale_loc = use_row_as_scale ? 1 << r : scale;
      auto activ = xt::view(this->x_array, c, r, xt::all()); // 16x channels of 8-bit
      if(this->binconv_traces) {
        std::ostringstream stringStream;
        stringStream << "binconv: weight=" << xt::view(weight, r)*mac_enable << "activ=" << activ << " scale=" << scale_loc << " ==> " << xt::view(weight, r)*mac_enable * activ << " ==> " << std::hex << xt::sum(xt::view(weight, r)*mac_enable*activ, 0)*scale << std::dec << "\n";
        std::string copyOfStr = stringStream.str();
        this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
      }
      if (!mode_linear) {
        xt::view(this->psum_block, c, r) = __BinConvBlock(xt::view(weight, r) * mac_enable, activ, scale_loc, mode16);
      }
      else {
        xt::xarray<uint8_t> weight_lin = xt::zeros_like(weight);
        if(c==0) {
          xt::view(weight_lin, xt::range(0, 8)) = xt::view(weight, xt::range(0, 8));
        }
        else if(c==1){
          xt::view(weight_lin, xt::range(0, 8)) = xt::view(weight, xt::range(8, 16));
        }
        else if(c==2 && mode16){
          xt::view(weight_lin, xt::range(0, 8)) = xt::view(weight, xt::range(16, 24));
        }
        else if(c==3 && mode16){
          xt::view(weight_lin, xt::range(0, 8)) = xt::view(weight, xt::range(24, 32));
        }
        xt::view(this->psum_block, c, r) = __BinConvBlock(xt::view(weight_lin, r) * mac_enable * xt::view(block_enable_linear, c, r), activ, scale_loc, mode16);
      }
      if(weight_shift && weight_invert) {
        xt::view(this->psum_block, c, r) = -xt::view(this->psum_block, c, r);
      }
      xt::view(this->psum_column, c) += xt::view(this->psum_block, c, r);
    }
    if(!mode_linear) {
      if(weight_shift) {
        xt::view(this->accum, xt::all(), c) += xt::view(this->psum_column, c);
      }
      else {
        xt::view(this->accum, idx, c) += xt::view(this->psum_column, c);
      }
    }
    else if(c==3) {
      if(weight_shift) {
        xt::view(this->accum, xt::all(), 0) += xt::view(this->psum_column, 0) + xt::view(this->psum_column, 1) + xt::view(this->psum_column, 2) + xt::view(this->psum_column, 3);
      }
      else {
        xt::view(this->accum, idx, 0) += xt::view(this->psum_column, 0) + xt::view(this->psum_column, 1) + xt::view(this->psum_column, 2) + xt::view(this->psum_column, 3);
      }
    }
  }
}

void Ne16::__weightoffs(
  int dw_iter,
  xt::xarray<int32_t> row_enable,
  xt::xarray<int32_t> mac_enable
) {
  auto start_s = 1;
  for(auto s=start_s; s<this->SHIFT_CYCLES; s++) { // temporal loop - fake weight for Wmin offsetting // FIXME: how to properly do this in 1x1 mode?

    // fake-load and unpack weight bits
    auto read_size = (this->mode_linear) ? (this->mode16 ? 32 : 16) : this->FILTER_SIZE*this->FILTER_SIZE;
    xt::xarray<uint8_t> weight_ld = xt::zeros<uint8_t>({read_size, 2});
    if(this->fs == 3 || this->mode_linear)
      xt::view(weight_ld, xt::all()) = 0xff;
    else
      xt::view(weight_ld, 0, xt::all()) = 0xff;
    
    auto weight = __WeightUnpack(weight_ld, read_size, false); //this->mode16 & this->mode_linear);
    auto scale = this->Wmin;

    xt::xarray<int32_t> block_enable_linear = xt::ones<int32_t>({this->NR_COLUMN, this->COLUMN_SIZE});
    if (this->mode16 && this->mode_linear)
    {
      for (auto rr = 0; rr < this->NR_COLUMN; rr++)
      {
        for (auto cc = 0; cc < this->COLUMN_SIZE; cc++)
        {
          auto i_kin_16bit = (cc < 8 && rr < 4) ? rr * 8 + cc : -1;
          auto load_fbuf_lim = this->load_fbuf_lim;
          xt::view(block_enable_linear, rr, cc) = ((i_kin_16bit != -1 && i_kin_16bit < load_fbuf_lim) ? 1 : 0);
        }
      }
      // std::cout << "block_enable_linear=" << block_enable_linear << "\n";
    }

    this->__BinConvArray(weight, scale, this->depthwise ? dw_iter : 0, block_enable_linear, row_enable, mac_enable, !this->depthwise, false, false, this->mode16, this->mode_linear);
    
  }
}

void Ne16::depthwise_setup() {
  this->k_out_lim_dw = (this->k_in_major == this->subtile_nb_ki-1 && this->subtile_rem_ki != this->TP_IN && this->subtile_rem_ki != 0) ? this->subtile_rem_ki : this->TP_IN;
  this->dw_lim = this->depthwise ? this->k_out_lim_dw : 1;
  this->dw_iter = 0;
  this->mac_enable = xt::zeros<int32_t>({this->TP_IN});
}

void Ne16::depthwise_update_idx() {
  this->dw_iter++;
}

void Ne16::weightoffs() {
  if(this->depthwise) {
    xt::view(this->mac_enable, xt::all()) = 0;
    xt::view(this->mac_enable, this->dw_iter) = 1;
  }
  else {
    xt::view(this->mac_enable, xt::all()) = 1;
  }
  this->__weightoffs(this->dw_iter, this->row_enable, this->mac_enable);
}

void Ne16::matrixvec_setup() {

  auto k_in_major = this->depthwise ? this->k_out_major : this->k_in_major_iter;

  // set up streamer to address weights (32b word-based)
  // depthwise mode: layout is (subtile_nb_ki*qw, 9, TP_IN/8)
  this->base_addr_W_dw = this->weights_ptr + (k_in_major*this->qw) * this->FILTER_SIZE*this->FILTER_SIZE * 2;
  this->vld_W_dw = Ne16VectorLoad<uint8_t>(
    this,
    this->base_addr_W_dw, // base_addr
    this->subtile_nb_ki*this->qw, // word_length
    this->weights_d0_stride, // word_stride
    this->qw, // line_length
    this->weights_d1_stride, // line_stride
    -1, // block_length
    this->weights_d2_stride, // block_stride
    false
  );
  
  // 3x3 mode: layout is (k_out, subtile_nb_ki*qw, 9, TP_IN/8)
  this->base_addr_W_3x3 = this->weights_ptr + (k_out_major*this->TP_OUT*this->subtile_nb_ki*this->qw + k_in_major*this->qw) * this->FILTER_SIZE*this->FILTER_SIZE * (this->mode16 ? 1 : 2);
  this->vld_W_3x3 = Ne16VectorLoad<uint8_t>(
    this,
    this->base_addr_W_3x3, // base_addr
    this->TP_OUT*this->subtile_nb_ki*this->qw, // word_length
    this->weights_d0_stride, // word_stride
    this->qw, // line_length
    this->weights_d1_stride, // line_stride
    -1, // block_length
    this->weights_d2_stride, // block_stride
    false
  );

  // 1x1 mode: layout is (k_out, subtile_nb_ki, qw, TP_IN/8)
  this->base_addr_W_1x1 = this->weights_ptr + (k_out_major*this->TP_OUT*this->subtile_nb_ki*this->qw + k_in_major*this->qw) * (this->mode16 ? 1 : 2);
  this->vld_W_1x1 = Ne16VectorLoad<uint8_t>(
    this,
    this->base_addr_W_1x1, // base_addr
    this->TP_OUT*this->subtile_nb_ki, // word_length
    this->weights_d0_stride, // word_stride
    1, // line_length
    this->weights_d1_stride, // line_stride
    -1, // block_length
    this->weights_d2_stride, // block_stride
    false
  );
  
  // linear mode:
  auto linear_d0_stride = (this->mode16 ? 1 : 2) * this->k_in; // distance in QW = 2 * k_in
  auto linear_d1_stride = this->qw * (this->mode16 ? 1 : 2) * this->k_in; // distance in K_IN subtiles = 32
  this->base_addr_W_linear = this->weights_ptr + this->k_out_major * this->qw * (this->mode16 ? 1 : 2) * this->k_in * 32 + k_in_major * 32;
  this->vld_W_linear = Ne16VectorLoad<uint8_t>(
    this,
    this->base_addr_W_linear, // base_addr
    -1, // word_length
    linear_d0_stride, // word_stride
    this->qw, // line_length
    linear_d1_stride, // line_stride
    -1, // block_length
    32*this->k_in/(this->mode16 ? 1 : 1)*this->qw/8, // block_stride
    false
  );

  this->mv_qw_lim = (this->fs == 3 || this->mode_linear) ? this->qw : 1;
  this->mv_k_out_lim = this->depthwise ? 1 :
                       (this->k_out_major == this->subtile_nb_ko-1 && this->subtile_rem_ko != this->TP_OUT && this->subtile_rem_ko != 0) ? this->subtile_rem_ko : this->TP_OUT;
  this->mv_k_out_iter = 0;
  this->mv_qw_iter = 0; // was simply qw
}

int Ne16::matrixvec_cycle() {
  auto& vld_W =      this->mode_linear ? this->vld_W_linear       : this->depthwise ? this->vld_W_dw       : (this->fs == 3) ? this->vld_W_3x3       : this->vld_W_1x1;
  auto read_size = (this->fs == 3) ? this->FILTER_SIZE*this->FILTER_SIZE : (this->mode_linear) ? 16 : this->qw;
  auto k_out = this->depthwise ? this->dw_iter : this->mv_k_out_iter;

  // load and unpack weight bits
  int64_t cycles = 0;
  xt::xarray<uint8_t> weight_ld = vld_W.ex(read_size*2, cycles); // each packet is composed of read_size x 16 bit
  xt::xarray<uint8_t> weight_ld_scattered = xt::view(weight_ld, xt::all());
  if(this->ki_scatter > 1) {

    const int CONTIG_CHUNK = 2; // [Bytes]

    for (int ii=0; ii<32/CONTIG_CHUNK; ii++) { // MEMORY BANDWIDTH
      for(int jj=0; jj<CONTIG_CHUNK; jj++) {

        int oo = ii*CONTIG_CHUNK+jj;

        int ii_16 = ii;
        int ii_8  = ii/2;
        int ii_4  = ii/4;
        int ii_2  = ii/8;
        int ii_1  = ii/16;

        int jj_16 = jj;
        int jj_8  = (ii%2)*8 + oo%8;
        int jj_4  = (ii%4)*4 + oo%4;
        int jj_2  = (ii%8)*2 + oo%2;
        int jj_1  = ii%16    + oo%1;

        if(this->ki_scatter == 2) {
          xt::view(weight_ld_scattered, ii*CONTIG_CHUNK+jj) = xt::view(weight_ld, ii_8*CONTIG_CHUNK+jj_8);
        }
        else if(this->ki_scatter == 4) {
          xt::view(weight_ld_scattered, ii*CONTIG_CHUNK+jj) = xt::view(weight_ld, ii_4*CONTIG_CHUNK+jj_4);
        }
        else if(this->ki_scatter == 8) {
          xt::view(weight_ld_scattered, ii*CONTIG_CHUNK+jj) = xt::view(weight_ld, ii_2*CONTIG_CHUNK+jj_2);
        }
        else if(this->ki_scatter == 16) {
          xt::view(weight_ld_scattered, ii*CONTIG_CHUNK+jj) = xt::view(weight_ld, ii_1*CONTIG_CHUNK+jj_1);
        }
        else {
          xt::view(weight_ld_scattered, ii*CONTIG_CHUNK+jj) = xt::view(weight_ld, ii*CONTIG_CHUNK+jj);
        }

      }
    }

  }

  auto weight = __WeightUnpack(weight_ld_scattered, read_size, this->mode16);
  auto scale = 1 << this->mv_qw_iter;

  xt::xarray<int32_t> space = xt::logspace(0, 7, 8, 2);
  space = xt::reshape_view(space, {8, 1});

  xt::xarray<int32_t> block_enable_linear = xt::ones<int32_t>({NR_COLUMN, COLUMN_SIZE});
  if (this->mode16 && this->mode_linear)
  {
    for (auto rr = 0; rr < this->NR_COLUMN; rr++)
    {
      for (auto cc = 0; cc < this->COLUMN_SIZE; cc++)
      {
        auto i_kin_16bit = (cc < 8 && rr < 4) ? rr * 8 + cc : -1;
        auto load_fbuf_lim = this->load_fbuf_lim;
        xt::view(block_enable_linear, rr, cc) = ((i_kin_16bit != -1 && i_kin_16bit < load_fbuf_lim) ? 1 : 0);
      }
    }
  }
  
  this->__BinConvArray(weight, scale, k_out, block_enable_linear, this->row_enable, this->mac_enable, false, false, this->fs==1 && !this->mode_linear, this->mode16, this->mode_linear);

  return (int) cycles;
}

bool Ne16::matrixvec_exit_idx() {
  if(this->mv_k_out_iter == this->mv_k_out_lim-1 && this->mv_qw_iter == this->mv_qw_lim-1) {
    return true;
  }
  else {
    return false;
  }
}

void Ne16::matrixvec_update_idx() {
  if(this->mv_qw_iter == this->mv_qw_lim-1) {
    this->mv_qw_iter = 0;
    this->mv_k_out_iter++;
  }
  else {
    this->mv_qw_iter++;
  }
}

bool Ne16::matrixvec_to_matrixvec_idx() {
  if(this->dw_iter == this->dw_lim-1) {
    return true;
  }
  else {
    return false;
  }
}

bool Ne16::matrixvec_to_load_idx() {
  auto k_in_major_lim = this->depthwise ? 1 : this->subtile_nb_ki;
  if(this->k_in_major_iter == k_in_major_lim-1) {
    return true;
  }
  else {
    return false;
  }
}
