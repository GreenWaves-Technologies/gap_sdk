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
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __NE16_HPP__
#define __NE16_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <assert.h>
#include <string>
#include <bitset>
#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xindex_view.hpp"
#include "xtensor/xrandom.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xvectorize.hpp"
#include "xtensor/xpad.hpp"

#define NE16_REG_WEIGHTS_PTR       0
#define NE16_REG_INFEAT_PTR        1
#define NE16_REG_OUTFEAT_PTR       2
#define NE16_REG_SCALE_PTR         3
#define NE16_REG_SCALE_SHIFT_PTR   4
#define NE16_REG_SCALE_BIAS_PTR    5
#define NE16_REG_INFEAT_D0_STRIDE  6
#define NE16_REG_INFEAT_D1_STRIDE  7
#define NE16_REG_INFEAT_D2_STRIDE  8
#define NE16_REG_OUTFEAT_D0_STRIDE 9
#define NE16_REG_OUTFEAT_D1_STRIDE 10
#define NE16_REG_OUTFEAT_D2_STRIDE 11
#define NE16_REG_WEIGHTS_D0_STRIDE 12
#define NE16_REG_WEIGHTS_D1_STRIDE 13
#define NE16_REG_WEIGHTS_D2_STRIDE 14
#define NE16_REG_SUBTILE_REM0      15
#define NE16_REG_SUBTILE_REM1      16
#define NE16_REG_SUBTILE_REM2      17
#define NE16_REG_SUBTILE_NB0       18
#define NE16_REG_SUBTILE_NB1       19
#define NE16_REG_PADDING           20
#define NE16_REG_WEIGHT_OFFSET     21
#define NE16_REG_FILTER_MASK       22
#define NE16_REG_CONFIG0           23

#define NE16_NB_REG 24

#define NE16_SPECIAL_TRACE_REG NE16_NB_REG
#define NE16_SPECIAL_FORMAT_TRACE_REG NE16_NB_REG+1
#define DEFAULT_TRACE_LEVEL L0_CONFIG

enum Ne16State {
    IDLE,
    START,
    START_STREAMIN,
    STREAMIN_LOAD,
    LOAD_MATRIXVEC,
    STREAMIN,
    LOAD,
    MATRIXVEC,
    NORMQUANT_SHIFT,
    NORMQUANT_MULT,
    NORMQUANT_BIAS,
    STREAMOUT,
    END
};

enum Ne16TraceLevel {
    L0_CONFIG,
    L1_ACTIV_INOUT,
    L2_DEBUG,
    L3_ALL
};

// forward definitions
class Ne16;

#define STREAM_MAX_WIDTH_BYTES 40

class Ne16StreamAccess {
  public:
    Ne16StreamAccess(
      Ne16 *ne16,
      int base_addr,
      int d0_length,
      int d0_stride,
      int d1_length,
      int d1_stride,
      int d2_length,
      int d2_stride,
      bool debug = false
    );
    void reset_iteration();
    int iterate();
    void print_config();
    int get_base_addr();
    int get_d0_length();
    int get_d0_stride();
    int get_d1_length();
    int get_d1_stride();
    int get_d2_length();
    int get_d2_stride();

  protected:
    Ne16 *ne16;
    int base_addr;
    int d0_length;
    int d0_stride;
    int d1_length;
    int d1_stride;
    int d2_length;
    int d2_stride;
    bool debug;
    // internal
    int current_addr;
    int ba;
    int la;
    int wa;
    int bc;
    int wc;
    int lc;
    int oc;
};

template <class T>
class Ne16VectorLoad : public Ne16StreamAccess {
  public:
    Ne16VectorLoad(
      Ne16 *_this,
      int base_addr,
      int d0_length,
      int d0_stride,
      int d1_length,
      int d1_stride,
      int d2_length,
      int d2_stride,
      bool debug
    );
    Ne16VectorLoad();
    xt::xarray<T> ex(int width, int64_t& cycles);
    void foo();
};

template <class T>
class Ne16VectorStore : public Ne16StreamAccess {
  public:
    Ne16VectorStore(
      Ne16 *_this,
      int base_addr,
      int d0_length,
      int d0_stride,
      int d1_length,
      int d1_stride,
      int d2_length,
      int d2_stride,
      bool debug
    );
    Ne16VectorStore();
    xt::xarray<T> ex(xt::xarray<T> data, int width, int64_t& cycles, int32_t enable);
};

class Ne16 : public vp::component
{
    friend class Ne16_base;

public:
    Ne16(js::config *config);

    int build();
    void reset(bool active);

    // were private before, but did not work with stream.hpp
    vp::io_req io_req;
    vp::trace trace;
    vp::io_master out;
    vp::reg_32 state;
    vp::reg_8 activity;
    Ne16TraceLevel trace_level;
    int trace_format;

private:

    // HARDWARE parameters
    int TP_IN;
    int TP_IN_LINEAR;
    int TP_OUT;
    int QA_IN;
    int QA_OUT;
    int NR_COLUMN;
    int COLUMN_SIZE;
    int BLOCK_SIZE;
    int F_BUFFER_SIZE;
    int FILTER_SIZE;
    int SHIFT_CYCLES;
    int NORMQUANT_NMULT;
    int OVERHEAD_LD_1X1;
    int OVERHEAD_LD_3X3;
    int OVERHEAD_MV;
    int QUANT_PER_CYCLE;

    static vp::io_req_status_e hwpe_slave(void *__this, vp::io_req *req);

    // DEBUG settings
    bool fsm_traces;
    bool accum_traces;
    bool accum_traces_poststreamin;
    bool accum_traces_postmatrixvec;
    bool accum_traces_normquant;
    bool accum_traces_streamout;
    bool psum_block_traces;
    bool x_buffer_traces;
    bool x_buffer_traces_postload;
    bool binconv_traces;
    void debug_x_buffer();
    void debug_x_array();
    void debug_accum();
    void debug_psum_block();

    // EVENT handlers
    static void fsm_start_handler(void *__this, vp::clock_event *event);
    static void fsm_handler(void *__this, vp::clock_event *event);
    static void fsm_end_handler(void *__this, vp::clock_event *event);

    // MAIN FSM and LOOP
    int  fsm();
    void fsm_loop();
    //Ne16State state;

    // REGISTER FILE member functions
    int  regfile_rd(int);
    void regfile_wr(int, int);
    void regfile_cxt();
    void commit();
    int  acquire();
    bool status();
    void printout();

    // REGISTER FILE and HWPE CTRL
    int  cxt0[NE16_NB_REG];
    int  cxt1[NE16_NB_REG];
    int  cxt_cfg_ptr;
    int  cxt_use_ptr;
    int  job_pending;
    int  job_state;
    unsigned char job_id;
    int cxt_job_id[2];
    char running_job_id;
    int  job_running;

    // REGISTER FILE configuration parameters
    int weights_ptr;
    int infeat_ptr;
    int outfeat_ptr;
    int scale_ptr;
    int scale_shift_ptr;
    int scale_bias_ptr;
    int infeat_d0_stride;
    int infeat_d1_stride;
    int infeat_d2_stride;
    int weights_d0_stride;
    int weights_d1_stride;
    int weights_d2_stride;
    int outfeat_d0_stride;
    int outfeat_d1_stride;
    int outfeat_d2_stride;
    int subtile_nb_ko;
    int subtile_rem_ko;
    int subtile_nb_ki;
    int subtile_rem_ki;
    int subtile_nb_ho;
    int subtile_rem_ho;
    int subtile_nb_wo;
    int subtile_rem_wo;
    int subtile_rem_hi;
    int subtile_rem_wi;
    int padding_top;
    int padding_right;
    int padding_bottom;
    int padding_left;
    int padding_value;
    int Wmin;
    bool norm_option_shift;
    bool norm_option_bias;
    int fs;
    int output_quant;
    int normalization_bits;
    int quantization_bits;
    int quantization_right_shift;
    bool use_relu;
    bool streamin;
    int filter_mask_top;
    int filter_mask_right;
    int filter_mask_bottom;
    int filter_mask_left;
    bool mode16;
    bool mode_linear;
    bool strided2x2;
    int qw;
    bool depthwise;

    // CONVENIENCE configuration
    int h_out;
    int w_out;
    int k_out;
    int k_in;
    int h_out_int;
    int w_out_int;
    int h_in_int;
    int w_in_int;
    int h_in;
    int w_in;

    // STATEFUL BUFFERS
    xt::xarray<int64_t> psum_block;  // partial sums at the output of a BinConv Block  (no actual storage in NE16)
    xt::xarray<int64_t> psum_column; // partial sums at the output of a BinConv Column (no actual storage in NE16)
    xt::xarray<int64_t> accum;       // accumulators (*actual storage* in NE16)
    xt::xarray<int64_t> accum_save;
    xt::xarray<uint8_t> x_buffer;    // feature buffer (*actual storage* in NE16)
    xt::xarray<uint8_t> x_buffer_linear; // feature buffer (*actual storage* in NE16 -- representation for linear case)
    xt::xarray<uint8_t> x_array;     // reordered feature array (no actual storage in NE16)
    xt::xarray<uint8_t> weight;      // input weight stream

    // CLEAR
    void clear_all();
    void clear_accum();
    void clear_x_buffer();

    // STREAMIN
    void constant_setup();
    void streamin_setup();
    int  streamin_cycle();
    bool streamin_exit_idx();
    void streamin_update_idx();

    // LOAD
    void load_setup();
    int  load_cycle();
    int  load_cycle_linear();
    void load_do_padding();
    void load_do_extract();
    void load_filter_masking();
    bool load_exit_idx();
    void load_update_idx();

    // MATRIXVEC
    void depthwise_setup();
    void depthwise_update_idx();
    void weightoffs();
    void matrixvec_setup();
    int  matrixvec_cycle();
    bool matrixvec_exit_idx();
    void matrixvec_update_idx();
    bool matrixvec_to_load_idx();
    bool matrixvec_to_matrixvec_idx();
    // internal functions
    void __BinConvArray(xt::xarray<uint8_t>&, int, int, xt::xarray<int32_t>, xt::xarray<int32_t>, xt::xarray<int32_t>, bool=false, bool=false, bool=false, bool=false, bool=false);
    void __weightoffs(int, xt::xarray<int32_t>, xt::xarray<int32_t>);
    
    // NORMQUANT
    void normquant_shift_setup();
    int  normquant_shift_cycle();
    void normquant_mult_setup();
    int  normquant_mult_cycle();
    bool normquant_mult_exit_idx();
    void normquant_mult_update_idx();
    void normquant_bias_setup();
    int  normquant_bias_cycle();
    bool normquant_bias_exit_idx();
    void normquant_bias_update_idx();
    
    // STREAMOUT
    void streamout_setup();
    int  streamout_cycle();
    bool streamout_exit_idx();
    void streamout_update_idx();
    bool streamout_to_end_idx();

    // INDEX
    void k_in_major_update_idx();
    void high_update_idx();

    // INDEX state
    int k_out_major;
    int i_major;
    int j_major;
    int k_in_major_iter;
    int k_in_major;
    int h_size_in;
    int w_size_in;
    int h_size_out;
    int w_size_out;
    int h_size_in_hw;
    int w_size_in_hw;
    int h_size_in_X_w_size_in;
    int h_size_out_X_w_size_out;
    int k_out_lim_dw;
    int dw_lim;
    int dw_iter;

    // STREAMIN state
    int streamin_ij_out;
    Ne16VectorLoad<uint8_t> vld_streamin;
    int streamin_i_out_iter;
    int streamin_i_out_lim;
    int streamin_j_out_iter;
    int streamin_j_out_lim;
    int streamin_k_out_iter;
    int streamin_k_out_lim;

    // LOAD state
    int load_fbuf_lim;
    int load_i_fbuf_lim;
    int load_j_fbuf_lim;
    int load_k_in_lim;
    std::vector<uint32_t> load_padding;
    int load_i_fbuf;
    int load_j_fbuf;
    Ne16VectorLoad<uint8_t> vld_x;
    xt::xarray<int32_t> row_enable;

    // MATRIXVEC state
    int base_addr_W_dw;
    int base_addr_W_3x3;
    int base_addr_W_1x1;
    int base_addr_W_linear;
    Ne16VectorLoad<uint8_t> vld_W_dw;
    Ne16VectorLoad<uint8_t> vld_W_3x3;
    Ne16VectorLoad<uint8_t> vld_W_1x1;
    Ne16VectorLoad<uint8_t> vld_W_linear;
    int mv_k_out_iter;
    int mv_k_out_lim;
    int mv_qw_iter; // was simply qw
    int mv_qw_lim; // was simply qw
    xt::xarray<int32_t> mac_enable;

    // NORMQUANT state
    Ne16VectorLoad<uint8_t> vld_nqs;
    Ne16VectorLoad<uint8_t> vld_nq;
    Ne16VectorLoad<uint8_t> vld_nqb;
    xt::xarray<uint8_t> nqs;
    int nq_iter;
    int nq_lim;
    int nqb_iter;
    int nqb_lim;

    // STREAMOUT state
    int streamout_i_out_iter;
    int streamout_i_out_lim;
    int streamout_j_out_iter;
    int streamout_j_out_lim;
    int streamout_k_out_iter;
    int streamout_k_out_lim;
    Ne16VectorStore<uint8_t> vst_y;
    xt::xarray<int32_t> col_enable;

    vp::io_slave in;
    vp::wire_master<bool> irq;

    vp::clock_event *fsm_start_event;
    vp::clock_event *fsm_event;
    vp::clock_event *fsm_end_event;
};

#endif /* __NE16_HPP__ */


