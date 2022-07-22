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

void Ne16::clear_all() {
    this->k_out_major = 0;
    this->i_major = 0;
    this->j_major = 0;
    this->k_in_major_iter = 0;
    this->k_in_major = 0;
    this->h_size_in  = 0;
    this->w_size_in  = 0;
    this->h_size_out = 0;
    this->w_size_out = 0;
    this->h_size_in_hw = 0;
    this->w_size_in_hw = 0;
    this->h_size_in_X_w_size_in = 0;
    this->h_size_out_X_w_size_out = 0;
    this->k_out_lim_dw = 0;
    this->dw_lim = 0;
    this->dw_iter = 0;

    // STREAMIN state
    this->streamin_ij_out = 0;

    // LOAD state
    this->load_fbuf_lim = 0;
    this->load_i_fbuf_lim = 0;
    this->load_j_fbuf_lim = 0;
    this->load_k_in_lim = 0;
    this->load_padding = {0, 0};
    this->load_i_fbuf = 0;
    this->load_j_fbuf = 0;

    // MATRIXVEC state
    this->mv_k_out_iter = 0;
    this->mv_qw_iter = 0; // was simply qw

    // STREAMOUT state
    this->streamout_i_out_iter = 0;
    this->streamout_i_out_lim = 0;
    this->streamout_j_out_iter = 0;
    this->streamout_j_out_lim = 0;
    this->streamout_k_out_iter = 0;
    this->streamout_k_out_lim = 0;

    this->clear_accum();
}

void Ne16::clear_accum() {
    for (auto j=0; j<this->COLUMN_SIZE; j++) {
        for (auto i=0; i<this->NR_COLUMN; i++) {
            this->psum_block[i+j*this->NR_COLUMN] = 0;
        }
    }
    for (auto i=0; i<this->NR_COLUMN; i++) {
        this->psum_column[i] = 0;
    }
    for (auto i=0; i<this->TP_OUT; i++) {
        for (auto j=0; j<this->NR_COLUMN; j++) {
            this->accum[i*this->NR_COLUMN+j] = 0;
        }
    }
}

void Ne16::clear_x_buffer() {
    for (auto k=0; k<this->TP_IN; k++) {
        for (auto j=0; j<this->F_BUFFER_SIZE; j++) {
            for (auto i=0; i<this->F_BUFFER_SIZE; i++) {
                this->x_buffer[i+(j*this->F_BUFFER_SIZE)+(k*this->F_BUFFER_SIZE*this->F_BUFFER_SIZE)] = 0;
            }
        }
    }
    for (auto k=0; k<this->TP_IN; k++) {
        for (auto j=0; j<this->COLUMN_SIZE; j++) {
            for (auto i=0; i<this->NR_COLUMN; i++) {
                this->x_array[i+(j*this->NR_COLUMN)+(k*this->NR_COLUMN*this->COLUMN_SIZE)] = 0;
            }
        }
    }
}
