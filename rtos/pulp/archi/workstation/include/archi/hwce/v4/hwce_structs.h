
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_HWCE_STRUCTS__
#define __ARCHI_HWCE_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int any             :32; // Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_trigger_t;

typedef union {
  struct {
    unsigned int id_err          :8 ; // If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code
    unsigned int err             :24; // An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_acquire_t;

typedef union {
  struct {
    unsigned int jobs            :32; // The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access
  };
  unsigned int raw;
} __attribute__((packed)) hwce_finished_jobs_t;

typedef union {
  struct {
    unsigned int st              :1 ; // Status of the HWCE - 1'b0 Not running a job - 1'b1 Running a job
  };
  unsigned int raw;
} __attribute__((packed)) hwce_status_t;

typedef union {
  struct {
    unsigned int id              :8 ; // ID of the currently running job
  };
  unsigned int raw;
} __attribute__((packed)) hwce_running_job_t;

typedef union {
  struct {
    unsigned int any             :32; // A write of any value to this register will reset the HWCE to its idle state.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_soft_clear_t;

typedef union {
  struct {
    unsigned int qf              :6 ; // Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage.
    unsigned int nf              :1 ; // Operation type: - 1'b0 - Normal convolution - 1'b1 - Does not flip weights (i.e. implements a stencil instead of a mathematical convolution).
    unsigned int ny              :1 ; // No y_in mode: - 1'b0 - Normal operation - 1'b1 - Disable loading of y_in and add a constant set in the CONFIG2 register
    unsigned int uns             :1 ; // Set unsigned multiplication - 1'b0 - Consider multiplication results as signed fixed-point numbers. - 1'b1 - Consider multiplication results as unsigned fixed-point numbers.
    unsigned int vect            :2 ; // Vector mode: - 2'b00 - mode is scalar (1 feat/cycle, 1x16-bit weights). - 2'b01 - mode is vectorial 2 (2 feat/cycle, 2x8-bit weights). - 2'b10 - mode is vectorial 4 (4 feat/cycle, 4x4-bit weights).
    unsigned int conv            :2 ; // Convolution mode: - 2'b00 - mode is 5x5. - 2'b01 - mode is 3x3. - 2'b10 - mode is 4x7.
    unsigned int ncp             :1 ; // No job copy: - 1'b0 - do job copy - 1'b1 - don't do job copy
    unsigned int rnd             :1 ; // If 1, do normalization-and-rounding instead of only normalization.
    unsigned int padding0:1 ;
    unsigned int wstride         :16; // Stride between one FILTER_SIZExFILTER_SIZE filter and the next.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_gen_config0_t;

typedef union {
  struct {
    unsigned int pixshiftl       :5 ; // Shift input pixels to the left by this number of positions when PIXMODE is not 16bit.
    unsigned int padding0:3 ;
    unsigned int pixmode         :2 ; // Input pixel size - 2'b00  16bit - 2'b01 - 8bit - 2'b10 - 4bit - 2'b11  16bit bis
    unsigned int padding1:6 ;
    unsigned int pixshiftr       :5 ; // Shift output pixels to the right by this number of positions when PIXMODE is not 16bit.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_gen_config1_t;

typedef union {
  struct {
    unsigned int size            :32; // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_trans_size_t;

typedef union {
  struct {
    unsigned int length          :16; // Length of a line in number of words
    unsigned int stride          :16; // Distance in bytes between two consecutive lines.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_line_stride_length_t;

typedef union {
  struct {
    unsigned int length          :16; // Length of a line in number of words
    unsigned int stride          :16; // Distance in bytes between two consecutive lines.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_feat_stride_length_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit mode)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_3_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit mode)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_2_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit and 2x8 bit modes)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_1_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit, 2x8 bit and 1x16 bit modes)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_0_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit mode)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_3_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit mode)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_2_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit and 2x8 bit modes)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_1_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory (4x4 bit, 2x8 bit and 1x16 bit modes)
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_0_base_addr_t;

typedef union {
  struct {
    unsigned int size            :32; // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x<sub>in</sub>  stream.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_trans_size_t;

typedef union {
  struct {
    unsigned int length          :16; // Length of a line in number of words
    unsigned int stride          :16; // Distance in bytes between two consecutive lines.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_line_stride_length_t;

typedef union {
  struct {
    unsigned int length          :16; // Length of a line in number of words
    unsigned int stride          :16; // Distance in bytes between two consecutive lines.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_feat_stride_length_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_in_base_addr_t;

typedef union {
  struct {
    unsigned int addr            :32; // Pointer into cluster L1 memory
  };
  unsigned int raw;
} __attribute__((packed)) hwce_w_base_addr_t;

typedef union {
  struct {
    unsigned int lbuflen         :10; // Linebuffer virtual length. Set to the same number as X_LINE_LENGTH. Acceptable LBUFLEN values range between 2 and LINEBUF_LENGTH.
    unsigned int padding0:6 ;
    unsigned int noyconst        :16; // Constant to sum instead of y<sub>in</sub> if the NY flag is active in the CONFIG1 register.
  };
  unsigned int raw;
} __attribute__((packed)) hwce_job_config0_t;

typedef union {
  struct {
    unsigned int vect_disable_mask:4 ; // Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using approximate vector or 3x3 mode. The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, etc.
    unsigned int padding0:4 ;
    unsigned int wof_param       :6 ; // Output feature (OF) counter wrap parameter. If both WIF and WOF are 1'b0, the looping mechanism is disabled
    unsigned int padding1:2 ;
    unsigned int wif_param       :6 ; // Input feature (IF) counter wrap parameter. If both WIF and WOF are 1'b0, the looping mechanism is disabled
    unsigned int padding2:2 ;
    unsigned int lo              :1 ; // Loop order: - 1'b0 - output features (OF) are the outer loop - 1'b1 - input features (IF) are the outer loop
    unsigned int ln              :1 ; // Looping mechanism: - 1'b0 - both counters work as inner loops - 1'b1 - outer loop feature address is updated only when the feature counter reaches the wrap parameter, inner loop feature address is updated when the counter is less than the wrap parameter and reset when it is reached
  };
  unsigned int raw;
} __attribute__((packed)) hwce_job_config1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_trans_size_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_line_stride_length_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_feat_stride_length_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_3_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_2_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_1_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_0_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_3_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_2_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_1_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_0_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_trans_size_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_line_stride_length_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_feat_stride_length_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_in_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_w_base_addr_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_job_config0_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_job_config1_ctx0_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_trans_size_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_line_stride_length_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_feat_stride_length_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_3_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_2_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_1_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_out_0_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_3_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_2_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_1_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_y_in_0_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_trans_size_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_line_stride_length_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_feat_stride_length_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_x_in_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_w_base_addr_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_job_config0_ctx1_t;

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) hwce_job_config1_ctx1_t;

#endif

#endif
