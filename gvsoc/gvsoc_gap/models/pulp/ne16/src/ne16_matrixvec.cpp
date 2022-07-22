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

// XTENSOR REMOVAL

uint8_t* __WeightUnpack(uint8_t *w, int size, bool mode16, uint8_t *wu_mode8, uint8_t *wu_mode16) {
    auto idx=0;
    if (mode16) {
        for (auto j=0; j<2; j++) {
            for (auto i=0; i<size; i++) {
                for (auto k=0; k<8; k++) {
                    wu_mode16[k+(idx)*16]      = (w[idx]>>k) & 0x1;
                    wu_mode16[k+(idx)*16+8]    = (w[idx]>>k) & 0x1;
                }
                idx++;
            }
        }
        return wu_mode16;
    }
    else {
        for (auto j=0; j<2; j++) {
            for (auto i=0; i<size; i++) {
                for (auto k=0; k<8; k++) {
                    wu_mode8[k+(idx)*8] = (w[idx]>>k) & 0x1;
                }
                idx++;
            }
        }
        return wu_mode8;
    }
}

int __BinConvBlock(
    uint8_t* w,
    uint8_t* x,
    int scale=0,
    bool mode16=false
) {
    auto sum = 0;
    if(mode16) {
        int16_t wx_lo[8];
        int16_t wx_hi[8];

        for (auto i=0, j=0; i<8; i++, j+=2) {
            wx_lo[i] = w[i] * x[j];
        }
        for (auto i=0, j=1; i<8; i++, j+=2) {
            wx_hi[i] = w[i] * x[j];
        }
        for (auto i=0; i<8; i++) {
            sum += ((wx_hi[i]<<8) + wx_lo[i]);
        }
        return (sum * scale);
    }
    else {
        for (auto i=0; i<16; i++) {
            sum = sum + w[i] * x[i];
        }
        return (sum * scale);
    }
}

void Ne16::__BinConvArray(
    uint8_t             size,
    uint8_t*            weight_en,
    int                 scale,
    int                 idx,
    int32_t*            block_enable_linear,
    int32_t*            row_enable,
    int32_t*            mac_enable,
    bool                weight_shift,
    bool                weight_invert,
    bool                use_row_as_scale,
    bool                mode16,
    bool                mode_linear,
    int                 mv_qw_iter,
    int                 mv_k_out_iter,
    bool                fake_load,
    uint32_t            thread_idx
) {
    auto psum_block_idx = 0;
    bool print_enable = (mv_qw_iter==0) && (mv_k_out_iter==0);
    int64_t psum_block[81] = {0};
    int64_t psum_column[9] = {0};

    for(auto c=0; c<this->NR_COLUMN; c++) { // spatial loop - over columns
        psum_column[c]=0;
        for(auto r=0; r<this->COLUMN_SIZE; r++) { // spatial loop - over blocks in a column
            if(row_enable[r] == 0) {
                continue;
            } // row disabling to implement filter masks
            auto scale_loc = use_row_as_scale ? (1 << r) : scale;

            uint8_t *src_w = &weight_en[r*16];
            if ((!mode_linear) || (mode16 && (c<4)) || (c<2)) {
                src_w += (!mode_linear) ? 0 : (mode16 && (c<4)) ? (c<<3) : (c<<7);
                uint8_t weight_enable[16];

                if (mode_linear && (block_enable_linear[psum_block_idx] == 0)) {
                    for (auto j=0; j<16; j++) {
                        weight_enable[j] = 0;
                    }
                }
                else {
                    for (auto j=0; j<16; j++) {
                        weight_enable[j] = (*src_w++) & mac_enable[j];
                    }
                }
                psum_block[psum_block_idx] = __BinConvBlock(weight_enable, &x_array[r*this->TP_IN+c*this->COLUMN_SIZE*this->TP_IN], scale_loc, mode16);
            }

            if(weight_shift && weight_invert) {
                psum_block[psum_block_idx] = - psum_block[psum_block_idx];
            }
            psum_column[c] += psum_block[psum_block_idx];
            psum_block_idx++;
        }

        if (fake_load) {
            if(!mode_linear) {
                if(weight_shift) {
                    for (int32_t i=0; i<(this->TP_OUT*this->COLUMN_SIZE); i+=this->COLUMN_SIZE) {
                        this->accum[i+c] += psum_column[c];
                    }
                }
                else {
                    this->accum[idx*this->COLUMN_SIZE+c] += psum_column[c];
                }
            }
            else if(c==3) {
                if(weight_shift) {
                    for (int32_t i=0; i<(this->TP_OUT*this->COLUMN_SIZE); i+=this->COLUMN_SIZE) {
                        this->accum[i] += psum_column[0] + psum_column[1] + psum_column[2] + psum_column[3];
                    }
                }
                else {
                    this->accum[idx*this->COLUMN_SIZE] += psum_column[0] + psum_column[1] + psum_column[2] + psum_column[3];
                }
            }
        }
        else {
            this->accum_buffer[c+thread_idx] = psum_column[c];
        }
    }
}

void Ne16::__weightoffs(
  int dw_iter,
  int32_t* row_enable,
  int32_t* mac_enable
) {
    auto start_s = 1;
    for(auto s=start_s; s<this->SHIFT_CYCLES; s++) { // temporal loop - fake weight for Wmin offsetting // FIXME: how to properly do this in 1x1 mode?
        // fake-load and unpack weight bits
        uint8_t read_size = (this->mode_linear) ? (this->mode16 ? 32 : 16) : (this->FILTER_SIZE*this->FILTER_SIZE);
        uint8_t weight_ld [read_size<<1] = {0};
        if((this->fs == 3) || this->mode_linear) {
            for (auto j=0; j<2; j++) {
                for (auto i=0; i<read_size; i++) {
                    weight_ld[i+j*read_size] = 0xff;
                }
            }
        }
        else {
            for (auto j=0; j<2; j++) {
                weight_ld[j*read_size] = 0xff;
            }
        }
        uint8_t *weight;
        uint8_t wu_mode8[read_size<<4] = {0};
        uint8_t wu_mode16[read_size<<5] = {0};
        weight = __WeightUnpack(weight_ld, read_size, false, wu_mode8, wu_mode16);
        auto scale = this->Wmin;
        int32_t block_enable_linear[this->COLUMN_SIZE*this->NR_COLUMN];
        int32_t *src = block_enable_linear;
        if (this->mode16 && this->mode_linear)
        {
            for (auto rr = 0; rr < this->NR_COLUMN; rr++)
            {
                for (auto cc = 0; cc < this->COLUMN_SIZE; cc++)
                {
                    auto i_kin_16bit = ((cc < 8) && (rr < 4)) ? (rr * 8 + cc) : -1;
                    auto load_fbuf_lim = this->load_fbuf_lim;
                    *src++ = ((i_kin_16bit != -1) && (i_kin_16bit < load_fbuf_lim));
                }
            }
        }
        else {
            for (auto j=0; j<this->COLUMN_SIZE*this->NR_COLUMN; j++) {
                *src++ = 1;
            }
        }
        this->__BinConvArray(read_size, weight, scale, this->depthwise ? dw_iter : 0, block_enable_linear, row_enable, mac_enable, !this->depthwise, false, false, this->mode16, this->mode_linear, 0, 0, true, 0);
    }
}

void Ne16::depthwise_setup() {
    this->k_out_lim_dw = (this->k_in_major == this->subtile_nb_ki-1 && this->subtile_rem_ki != this->TP_IN && this->subtile_rem_ki != 0) ? this->subtile_rem_ki : this->TP_IN;
    this->dw_lim = this->depthwise ? this->k_out_lim_dw : 1;
    this->dw_iter = 0;
    for (auto i=0; i<this->TP_IN; i++) {
        this->mac_enable[i] = 0;
    }
}

void Ne16::depthwise_update_idx() {
    this->dw_iter++;
}

// XTENSOR REMOVAL
void Ne16::weightoffs() {
    if(this->depthwise) {
        for (auto i=0; i<this->TP_IN; i++) {
            this->mac_enable[i] = 0;
        }
        this->mac_enable[this->dw_iter] = 1;
    }
    else {
        for (auto i=0; i<this->TP_IN; i++) {
            this->mac_enable[i] = 1;
        }
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
    this->base_addr_W_linear = this->weights_ptr + (this->k_out_major * this->qw * (this->mode16 ? 1 : 2) * this->k_in * 32) + k_in_major * 32;
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

    this->mv_qw_lim     = ((this->fs == 3) || this->mode_linear) ? this->qw : 1;
    this->mv_k_out_lim  = this->depthwise ? 1 : (((this->k_out_major == (this->subtile_nb_ko-1)) && (this->subtile_rem_ko != this->TP_OUT) && (this->subtile_rem_ko != 0)) ? this->subtile_rem_ko : this->TP_OUT);
    this->mv_k_out_iter = 0;
    this->mv_qw_iter    = 0; // was simply qw
}

int Ne16::fill_weight_buffer (uint8_t *w_buf, uint32_t offset) {
    auto& vld_W     = this->mode_linear ? (this->vld_W_linear) : (this->depthwise ? this->vld_W_dw : ((this->fs == 3) ? this->vld_W_3x3 : this->vld_W_1x1));
    auto read_size  = (this->fs == 3) ? (this->FILTER_SIZE*this->FILTER_SIZE) : ((this->mode_linear) ? 16 : this->qw);
    int64_t cycles = 0;

    uint8_t *dst=w_buf+offset;
    vld_W.ex((read_size<<1), dst, cycles);

    return (int) cycles;
}

void Ne16::acc_reduction (uint32_t i, uint32_t k_out) {
    auto thread_idx = i*this->mv_qw_lim;
    for (auto j=0; j<this->mv_qw_lim; j++) {
        if (!this->mode_linear) {
            for (auto c=0; c<this->COLUMN_SIZE; c++) {
                this->accum[k_out+c] += this->accum_buffer[c+thread_idx];
            }
        }
        else {
            this->accum[k_out] += this->accum_buffer[0+thread_idx] + this->accum_buffer[1+thread_idx] + this->accum_buffer[2+thread_idx] + this->accum_buffer[3+thread_idx];
        }
        thread_idx += this->COLUMN_SIZE;
    }
}

// XTENSOR REMOVAL
int Ne16::matrixvec_cycle(uint8_t *w_buf, uint32_t offset, int mv_qw_iter, int mv_k_out_iter, uint32_t thread_idx) {
    auto read_size  = (this->fs == 3) ? (this->FILTER_SIZE*this->FILTER_SIZE) : ((this->mode_linear) ? 16 : this->qw);
    auto k_out      = this->depthwise ? this->dw_iter : mv_k_out_iter;

    // load and unpack weight bits
    int64_t cycles = 0;
    uint8_t wu_mode8[read_size<<4];
    uint8_t wu_mode16[read_size<<5];
    uint8_t* weight = __WeightUnpack(&w_buf[offset], read_size, this->mode16, wu_mode8, wu_mode16);
    auto scale = 1 << mv_qw_iter;

    int32_t block_enable_linear [this->NR_COLUMN*this->COLUMN_SIZE];
    int32_t *src = block_enable_linear;
    if (this->mode16 && this->mode_linear)
    {
        for (auto rr = 0; rr < this->NR_COLUMN; rr++) {
            for (auto cc = 0; cc < this->COLUMN_SIZE; cc++) {
                auto i_kin_16bit = ((cc < 8) && (rr < 4)) ? (rr * 8 + cc) : -1;
                auto load_fbuf_lim = this->load_fbuf_lim;
                *src++ = (((i_kin_16bit != -1) && (i_kin_16bit < load_fbuf_lim)) ? 1 : 0);
            }
        }
    }
    else {
        for (auto rr = 0; rr < this->NR_COLUMN; rr++) {
            for (auto cc = 0; cc < this->COLUMN_SIZE; cc++) {
                *src++ = 1;
            }
        }
    }

    this->__BinConvArray(read_size, weight, scale, k_out, block_enable_linear, this->row_enable, this->mac_enable, false, false, (this->fs==1) && (!this->mode_linear), this->mode16, this->mode_linear, mv_qw_iter, mv_k_out_iter, false, thread_idx);

    return (int) cycles;
}

bool Ne16::matrixvec_exit_idx() {
    if((this->mv_k_out_iter == (this->mv_k_out_lim-1)) && (this->mv_qw_iter == (this->mv_qw_lim-1))) {
        return true;
    }
    else {
        return false;
    }
}

void Ne16::matrixvec_update_idx() {
    if(this->mv_qw_iter == (this->mv_qw_lim-1)) {
        this->mv_qw_iter = 0;
        this->mv_k_out_iter++;
    }
    else {
        this->mv_qw_iter++;
    }
}

bool Ne16::matrixvec_to_matrixvec_idx() {
    if(this->dw_iter == (this->dw_lim-1)) {
        return true;
    }
    else {
        return false;
    }
}

bool Ne16::matrixvec_to_load_idx() {
    auto k_in_major_lim = this->depthwise ? 1 : this->subtile_nb_ki;
    if(this->k_in_major_iter == (k_in_major_lim-1)) {
        return true;
    }
    else {
        return false;
    }
}
