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

    this->h_size_in  = ((this->i_major < (this->subtile_nb_ho-1)) || (this->subtile_rem_hi==0)) ? (this->fs == 1 ? 3 : 5) : this->subtile_rem_hi;
    this->w_size_in  = ((this->j_major < (this->subtile_nb_wo-1)) || (this->subtile_rem_wi==0)) ? (this->fs == 1 ? 3 : 5) : this->subtile_rem_wi;
    this->h_size_out = ((this->i_major < (this->subtile_nb_ho-1)) || (this->subtile_rem_ho==0)) ? 3 : this->subtile_rem_ho;
    this->w_size_out = ((this->j_major < (this->subtile_nb_wo-1)) || (this->subtile_rem_wo==0)) ? 3 : this->subtile_rem_wo;

    this->h_size_in_X_w_size_in = this->h_size_in * this->w_size_in;
    this->h_size_out_X_w_size_out = this->h_size_out * this->w_size_out;

    this->h_size_in_hw = (this->i_major < (this->subtile_nb_ho-1) || (this->subtile_rem_hi==0)) ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_hi;
    this->w_size_in_hw = (this->j_major < (this->subtile_nb_wo-1) || (this->subtile_rem_wi==0)) ? (this->fs == 3 ? this->F_BUFFER_SIZE : this->FILTER_SIZE) : this->subtile_rem_wi;

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

    auto base_addr_x = (!this->mode_linear) ? (this->infeat_ptr + this->i_major*infeat_hom_iter + this->j_major*infeat_wom_iter + k_in_major*this->TP_IN) :
                                            (this->infeat_ptr + k_in_major*this->TP_IN*8 * (this->mode16 ? 2 : 2));

    this->vld_x = Ne16VectorLoad<uint8_t>(
        this,
        base_addr_x, // base_addr
        this->mode_linear ? (this->mode16 ? 32 : 16) : this->h_size_in_X_w_size_in, // word_length
        this->infeat_d0_stride, // word_stride
        this->mode_linear ? (this->mode16 ? 32 : 16) : this->w_size_in, // line_length
        this->infeat_d1_stride, // line_stride
        this->mode_linear ? -1 : this->h_size_in_X_w_size_in, // block_length
        this->infeat_d2_stride, // block_stride
        true
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
        if((k_in_major == (this->subtile_nb_ki-1)) && (this->subtile_rem_ki != this->TP_IN) && (this->subtile_rem_ki != 0)) { // last k_in tile, only if it requires padding
            this->load_k_in_lim = this->subtile_rem_ki;
            this->load_fbuf_lim = this->mode16 ? (2*this->subtile_rem_ki) : this->subtile_rem_ki;
        }
        this->load_i_fbuf = 0;
        this->load_j_fbuf = 0;
    }

    for (auto k=0; k<this->TP_IN*this->F_BUFFER_SIZE*this->F_BUFFER_SIZE; k++) {
        this->x_buffer[k] = 0;
    }

    for (auto j=0; j<(this->TP_IN)*32; j++) {
        this->x_buffer_linear[j] = 0;
    }
}

int Ne16::load_cycle() { // not linear
    int64_t cycles = 0;
    this->vld_x.ex(this->load_k_in_lim, &x_buffer[this->load_i_fbuf*this->TP_IN*this->F_BUFFER_SIZE+this->load_j_fbuf*this->TP_IN], cycles);
    return (int) cycles;
}

int Ne16::load_cycle_linear() {
    int64_t cycles = 0;
    uint8_t x[this->TP_IN] = {0};
    this->vld_x.ex(this->TP_IN, x, cycles);

    for (auto i=0; i<this->TP_IN; i++) {
        if (i<load_padding[0]) {
            x[i] = this->padding_value;
        } else if (i>(this->TP_IN - load_padding[1])) {
            x[i] = this->padding_value;
        }
    }

    for (uint8_t i=0; i<this->TP_IN; i++) {
        this->x_buffer_linear[this->load_i_fbuf+(i<<5)]=x[i];
    }
    return (int) cycles;
}

// XTENSOR REMOVAL
void Ne16::load_do_padding() { // not linear

    // add explicit padding
    auto right_lim  = ((5-this->padding_right)  > this->w_size_in_hw) ? this->w_size_in_hw : (5-this->padding_right);
    auto bottom_lim = ((5-this->padding_bottom) > this->h_size_in_hw) ? this->h_size_in_hw : (5-this->padding_bottom);

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

        auto inc0 = this->TP_IN*this->F_BUFFER_SIZE;
        auto inc1 = this->TP_IN;

        // top-left
        if((this->padding_left > 0) && (this->j_major==0) || (this->padding_top > 0) && (this->i_major==0)) {
            for (auto j=0; j<this->padding_left*inc1; j+=inc1) {
                for (auto i=0; i<this->padding_top*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // top
        if((this->padding_top > 0) && (this->i_major==0)) {
            for (auto j=this->padding_left*inc1; j<right_lim*inc1; j+=inc1) {
                for (auto i=0; i<this->padding_top*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // top-right
        if(((this->padding_right > 0) && (this->j_major == (this->subtile_nb_wo-1)) || (this->padding_top > 0) && (this->i_major==0)) && (right_lim <= this->w_size_in_hw)) {
            for (auto j=right_lim*inc1; j<w_size_in_hw*inc1; j+=inc1) {
                for (auto i=0; i<this->padding_top*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // right
        if(((this->padding_right > 0) && (this->j_major == (this->subtile_nb_wo-1))) && (right_lim <= this->w_size_in_hw)) {
            for (auto j=right_lim*inc1; j<this->w_size_in_hw*inc1; j+=inc1) {
                for (auto i=this->padding_top*inc0; i<bottom_lim*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // bottom-right
        if(((this->padding_right > 0) && (this->j_major == (this->subtile_nb_wo-1)) || (this->padding_bottom > 0) && (this->i_major == (this->subtile_nb_ho-1))) && (right_lim <= this->w_size_in_hw) && (bottom_lim <= this->h_size_in_hw)) {
            for (auto j=right_lim*inc1; j<w_size_in_hw*inc1; j+=inc1) {
                for (auto i=bottom_lim*inc0; i<h_size_in_hw*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // bottom
        if(((this->padding_bottom > 0) && (this->i_major == (this->subtile_nb_ho-1))) && (bottom_lim <= this->h_size_in_hw)) {
            for (auto j=this->padding_left*inc1; j<right_lim*inc1; j+=inc1) {
                for (auto i=bottom_lim*inc0; i<h_size_in_hw*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // bottom-left
        if(((this->padding_left > 0) && (this->j_major==0) || (this->padding_bottom > 0) && (this->i_major == (this->subtile_nb_ho-1))) && (bottom_lim <= this->h_size_in_hw)) {
            for (auto j=0; j<this->padding_left*inc1; j+=inc1) {
                for (auto i=bottom_lim*inc0; i<h_size_in_hw*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

        // left
        if((this->padding_left > 0) && (this->j_major==0)) {
            for (auto j=0; j<this->padding_left*inc1; j+=inc1) {
                for (auto i=this->padding_top*inc0; i<bottom_lim*inc0; i+=inc0) {
                    for (auto z=0; z<this->load_k_in_lim; z++) {
                        this->x_buffer[z+i+j] = this->padding_value;
                    }
                }
            }
        }

    }
    else {
        // ADDRESSING TO BE REDONE WHEN IN MODE16
        // top-left
        if((this->padding_left  > 0) && (this->j_major==0) || (this->padding_top > 0) && (this->i_major==0)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=0; j<this->padding_left; j++) {
                    for (auto i=0; i<this->padding_top; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=0; j<this->padding_left; j++) {
                    for (auto i=0; i<this->padding_top; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // top
        if((this->padding_top > 0) && (this->i_major==0)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=this->padding_left; j<right_lim; j++) {
                    for (auto i=0; i<this->padding_top; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=this->padding_left; j<right_lim; j++) {
                    for (auto i=0; i<this->padding_top; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // top-right
        if(((this->padding_right > 0) && (this->j_major == (this->subtile_nb_wo-1)) || (this->padding_top > 0) && (this->i_major==0)) && (right_lim <= this->w_size_in_hw)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=right_lim; j<w_size_in_hw; j++) {
                    for (auto i=0; i<this->padding_top; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=right_lim; j<w_size_in_hw; j++) {
                    for (auto i=0; i<this->padding_top; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // right
        if(((this->padding_right > 0) && (this->j_major == (this->subtile_nb_wo-1))) && (right_lim <= this->w_size_in_hw)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=right_lim; j<w_size_in_hw; j++) {
                    for (auto i=this->padding_top; i<bottom_lim; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=right_lim; j<w_size_in_hw; j++) {
                    for (auto i=this->padding_top; i<bottom_lim; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // bottom-right
        if(((this->padding_right > 0) && (this->j_major==(this->subtile_nb_wo-1)) || (this->padding_bottom > 0) && (this->i_major==(this->subtile_nb_ho-1))) && (right_lim <= this->w_size_in_hw) && (bottom_lim <= this->h_size_in_hw)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=right_lim; j<w_size_in_hw; j++) {
                    for (auto i=bottom_lim; i<h_size_in_hw; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=right_lim; j<w_size_in_hw; j++) {
                    for (auto i=bottom_lim; i<h_size_in_hw; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // bottom
        if(((this->padding_bottom > 0) && (this->i_major==(this->subtile_nb_ho-1))) && (bottom_lim <= this->h_size_in_hw)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=this->padding_left; j<right_lim; j++) {
                    for (auto i=bottom_lim; i<this->h_size_in_hw; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=this->padding_left; j<right_lim; j++) {
                    for (auto i=bottom_lim; i<this->h_size_in_hw; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // bottom-left
        if(((this->padding_left > 0) && (this->j_major==0) || (this->padding_bottom > 0) && (this->i_major==(this->subtile_nb_ho-1))) && (bottom_lim <= this->h_size_in_hw)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=0; j<this->padding_left; j++) {
                    for (auto i=bottom_lim; i<this->h_size_in_hw; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=0; j<this->padding_left; j++) {
                    for (auto i=bottom_lim; i<this->h_size_in_hw; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }

        // left
        if((this->padding_left > 0) && (this->j_major==0)) {
            for (auto z=0; z<this->load_k_in_lim; z+=2) {
                for (auto j=0; j<this->padding_left; j++) {
                    for (auto i=this->padding_top; i<bottom_lim; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = this->padding_value & 0xff;
                    }
                }
            }
            for (auto z=1; z<(this->load_k_in_lim+1); z+=2) {
                for (auto j=0; j<this->padding_left; j++) {
                    for (auto i=this->padding_top; i<bottom_lim; i++) {
                        this->x_buffer[z+j*this->TP_IN+i*this->TP_IN*this->F_BUFFER_SIZE] = (this->padding_value>>8);
                    }
                }
            }
        }
    }
}

// XTENSOR REMOVAL
void Ne16::load_do_extract() {
    // extract x_array from x_buffer
    if(this->mode_linear) {
        for (auto k=0; k<this->TP_IN*this->COLUMN_SIZE*this->NR_COLUMN; k++) {
            this->x_array[k] = 0;
        }
        for (auto i=0; i<(this->mode16 ? 4 : 2); i++) {
            for (auto j=0; j<8; j++) {
                for (auto k=0; k<this->TP_IN; k++) {
                    x_array[k+j*this->TP_IN+i*this->NR_COLUMN*this->TP_IN] = this->x_buffer_linear[((i<<3)+j)+(k<<5)];
                }
            }
        }
    }
    else if(this->fs == 3) {

        uint8_t *dst, *src;
        for(int32_t i_col=0; i_col<(this->NR_COLUMN); i_col++) { // spatial loop - implemented as a set of muxes
            auto i = i_col / this->FILTER_SIZE;
            auto j = i_col % this->FILTER_SIZE;

            dst = &x_array[i_col*this->COLUMN_SIZE*this->TP_IN];
            src = &x_buffer[i*this->F_BUFFER_SIZE*this->TP_IN+j*this->TP_IN];
            for (auto m=0; m<this->FILTER_SIZE; m++) {
                for (auto q=0; q<this->TP_IN*this->FILTER_SIZE; q++) {
                    *dst++ = *src++;
                }
                src += (this->F_BUFFER_SIZE - this->FILTER_SIZE)*this->TP_IN;
            }
        }
    }
    else {
        // in 1x1 mode, fill only the first qw rows
        for (auto k=0; k<this->TP_IN*this->COLUMN_SIZE*this->NR_COLUMN; k++) {
            this->x_array[k] = 0;
        }

        for(auto i_row=0; i_row<this->qw; i_row++) { // spatial loop - implemented as a set of muxes
            uint8_t *dst = &x_array[i_row*this->TP_IN];
            uint8_t *src = x_buffer;
            auto a = 0;
            for (auto m=0; m<this->FILTER_SIZE; m++) {
                for (auto p=0; p<this->FILTER_SIZE; p++) {
                    for (auto q=0; q<this->TP_IN; q++) {
                        *dst++ = *src++;
                    }
                    dst += (this->COLUMN_SIZE-1)*this->TP_IN;
                }
                src += (this->F_BUFFER_SIZE - this->FILTER_SIZE)*this->TP_IN;
            }
        }
    }
}

// XTENSOR REMOVAL
void Ne16::load_filter_masking() {
    // filter masking
    int32_t W_mask [this->fs*this->fs];
    for (auto j=0; j<this->fs; j++) {
        for (auto i=0; i<this->fs; i++) {
            W_mask[i+j*this->fs] = 1;
        }
    }
    if(this->fs == 3) {
        if(this->filter_mask_top > 0) {
            for (auto j=0; j<this->fs; j++) {
                for (auto i=0; i<this->filter_mask_top; i++) {
                    W_mask[i+j*this->fs] = 0;
                }
            }
        }
        if(this->filter_mask_right > 0) {
            for (auto j=(this->fs)-(this->filter_mask_right); j<this->fs; j++) {
                for (auto i=0; i<this->fs; i++) {
                    W_mask[i+j*this->fs] = 0;
                }
            }
        }
        if(this->filter_mask_bottom > 0) {
            for (auto j=0; j<this->fs; j++) {
                for (auto i=(this->fs)-(this->filter_mask_bottom); i<this->fs; i++) {
                    W_mask[i+j*this->fs] = 0;
                }
            }
        }
        if(this->filter_mask_left > 0) {
            for (auto j=0; j<this->filter_mask_left; j++) {
                for (auto i=0; i<this->fs; i++) {
                    W_mask[i+j*this->fs] = 0;
                }
            }
        }
  }
  auto i_mask = 0;
  auto j_mask = 0;
  for (auto j=0; j<3; j++) {
      for (auto i=0; i<3; i++) {
          this->row_enable[i+j*3] = W_mask[i_mask+j_mask*this->fs];
          i_mask = (i_mask==(this->fs-1)) ? 0 : (i_mask+1);
      }
      j_mask = (j_mask==(this->fs-1)) ? 0 : (j_mask+1);
  }
}

bool Ne16::load_exit_idx() {
    if(this->mode_linear) {
        if(this->load_i_fbuf == (this->load_fbuf_lim-1)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if((this->load_i_fbuf == (this->load_i_fbuf_lim-1)) && (this->load_j_fbuf == (this->load_j_fbuf_lim-1))) {
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
        if(this->load_j_fbuf == (this->load_j_fbuf_lim-1)) {
            this->load_j_fbuf = 0;
            this->load_i_fbuf++;
        }
        else {
            this->load_j_fbuf++;
        }
    }
}
