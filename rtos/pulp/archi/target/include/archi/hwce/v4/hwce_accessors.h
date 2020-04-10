
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

#ifndef __ARCHI_HWCE_ACCESSORS__
#define __ARCHI_HWCE_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t hwce_trigger_get(uint32_t base)
{
    return GAP_READ(base, HWCE_TRIGGER_OFFSET);
}

static inline void hwce_trigger_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_TRIGGER_OFFSET, value);
}


static inline uint32_t hwce_acquire_get(uint32_t base)
{
    return GAP_READ(base, HWCE_ACQUIRE_OFFSET);
}

static inline void hwce_acquire_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_ACQUIRE_OFFSET, value);
}


static inline uint32_t hwce_finished_jobs_get(uint32_t base)
{
    return GAP_READ(base, HWCE_FINISHED_JOBS_OFFSET);
}

static inline void hwce_finished_jobs_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_FINISHED_JOBS_OFFSET, value);
}


static inline uint32_t hwce_status_get(uint32_t base)
{
    return GAP_READ(base, HWCE_STATUS_OFFSET);
}

static inline void hwce_status_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_STATUS_OFFSET, value);
}


static inline uint32_t hwce_running_job_get(uint32_t base)
{
    return GAP_READ(base, HWCE_RUNNING_JOB_OFFSET);
}

static inline void hwce_running_job_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_RUNNING_JOB_OFFSET, value);
}


static inline uint32_t hwce_soft_clear_get(uint32_t base)
{
    return GAP_READ(base, HWCE_SOFT_CLEAR_OFFSET);
}

static inline void hwce_soft_clear_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_SOFT_CLEAR_OFFSET, value);
}


static inline uint32_t hwce_gen_config0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET);
}

static inline void hwce_gen_config0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, value);
}


static inline uint32_t hwce_gen_config1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET);
}

static inline void hwce_gen_config1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG1_OFFSET, value);
}


static inline uint32_t hwce_y_trans_size_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_TRANS_SIZE_OFFSET);
}

static inline void hwce_y_trans_size_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_TRANS_SIZE_OFFSET, value);
}


static inline uint32_t hwce_y_line_stride_length_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET);
}

static inline void hwce_y_line_stride_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET, value);
}


static inline uint32_t hwce_y_feat_stride_length_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET);
}

static inline void hwce_y_feat_stride_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET, value);
}


static inline uint32_t hwce_y_out_3_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_3_BASE_ADDR_OFFSET);
}

static inline void hwce_y_out_3_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_3_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_out_2_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_2_BASE_ADDR_OFFSET);
}

static inline void hwce_y_out_2_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_2_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_out_1_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_1_BASE_ADDR_OFFSET);
}

static inline void hwce_y_out_1_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_1_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_out_0_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_0_BASE_ADDR_OFFSET);
}

static inline void hwce_y_out_0_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_0_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_in_3_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_3_BASE_ADDR_OFFSET);
}

static inline void hwce_y_in_3_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_3_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_in_2_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_2_BASE_ADDR_OFFSET);
}

static inline void hwce_y_in_2_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_2_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_in_1_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_1_BASE_ADDR_OFFSET);
}

static inline void hwce_y_in_1_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_1_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_y_in_0_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_0_BASE_ADDR_OFFSET);
}

static inline void hwce_y_in_0_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_0_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_x_trans_size_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_TRANS_SIZE_OFFSET);
}

static inline void hwce_x_trans_size_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_TRANS_SIZE_OFFSET, value);
}


static inline uint32_t hwce_x_line_stride_length_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET);
}

static inline void hwce_x_line_stride_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET, value);
}


static inline uint32_t hwce_x_feat_stride_length_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET);
}

static inline void hwce_x_feat_stride_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET, value);
}


static inline uint32_t hwce_x_in_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_IN_BASE_ADDR_OFFSET);
}

static inline void hwce_x_in_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_IN_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_w_base_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_W_BASE_ADDR_OFFSET);
}

static inline void hwce_w_base_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_W_BASE_ADDR_OFFSET, value);
}


static inline uint32_t hwce_job_config0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET);
}

static inline void hwce_job_config0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG0_OFFSET, value);
}


static inline uint32_t hwce_job_config1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET);
}

static inline void hwce_job_config1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_OFFSET, value);
}


static inline uint32_t hwce_y_trans_size_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_TRANS_SIZE_CTX0_OFFSET);
}

static inline void hwce_y_trans_size_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_TRANS_SIZE_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_line_stride_length_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_CTX0_OFFSET);
}

static inline void hwce_y_line_stride_length_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_LINE_STRIDE_LENGTH_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_feat_stride_length_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_CTX0_OFFSET);
}

static inline void hwce_y_feat_stride_length_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_FEAT_STRIDE_LENGTH_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_out_3_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_3_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_out_3_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_3_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_out_2_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_2_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_out_2_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_2_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_out_1_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_1_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_out_1_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_1_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_out_0_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_0_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_out_0_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_0_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_in_3_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_3_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_in_3_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_3_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_in_2_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_2_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_in_2_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_2_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_in_1_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_1_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_in_1_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_1_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_in_0_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_0_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_y_in_0_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_0_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_x_trans_size_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_TRANS_SIZE_CTX0_OFFSET);
}

static inline void hwce_x_trans_size_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_TRANS_SIZE_CTX0_OFFSET, value);
}


static inline uint32_t hwce_x_line_stride_length_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_CTX0_OFFSET);
}

static inline void hwce_x_line_stride_length_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_LINE_STRIDE_LENGTH_CTX0_OFFSET, value);
}


static inline uint32_t hwce_x_feat_stride_length_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_CTX0_OFFSET);
}

static inline void hwce_x_feat_stride_length_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_FEAT_STRIDE_LENGTH_CTX0_OFFSET, value);
}


static inline uint32_t hwce_x_in_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_IN_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_x_in_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_IN_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_w_base_addr_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_W_BASE_ADDR_CTX0_OFFSET);
}

static inline void hwce_w_base_addr_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_W_BASE_ADDR_CTX0_OFFSET, value);
}


static inline uint32_t hwce_job_config0_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_JOB_CONFIG0_CTX0_OFFSET);
}

static inline void hwce_job_config0_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG0_CTX0_OFFSET, value);
}


static inline uint32_t hwce_job_config1_ctx0_get(uint32_t base)
{
    return GAP_READ(base, HWCE_JOB_CONFIG1_CTX0_OFFSET);
}

static inline void hwce_job_config1_ctx0_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_CTX0_OFFSET, value);
}


static inline uint32_t hwce_y_trans_size_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_TRANS_SIZE_CTX1_OFFSET);
}

static inline void hwce_y_trans_size_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_TRANS_SIZE_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_line_stride_length_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_CTX1_OFFSET);
}

static inline void hwce_y_line_stride_length_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_LINE_STRIDE_LENGTH_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_feat_stride_length_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_CTX1_OFFSET);
}

static inline void hwce_y_feat_stride_length_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_FEAT_STRIDE_LENGTH_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_out_3_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_3_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_out_3_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_3_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_out_2_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_2_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_out_2_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_2_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_out_1_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_1_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_out_1_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_1_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_out_0_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_0_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_out_0_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_0_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_in_3_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_3_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_in_3_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_3_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_in_2_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_2_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_in_2_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_2_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_in_1_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_1_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_in_1_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_1_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_y_in_0_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_0_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_y_in_0_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_0_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_x_trans_size_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_TRANS_SIZE_CTX1_OFFSET);
}

static inline void hwce_x_trans_size_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_TRANS_SIZE_CTX1_OFFSET, value);
}


static inline uint32_t hwce_x_line_stride_length_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_CTX1_OFFSET);
}

static inline void hwce_x_line_stride_length_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_LINE_STRIDE_LENGTH_CTX1_OFFSET, value);
}


static inline uint32_t hwce_x_feat_stride_length_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_CTX1_OFFSET);
}

static inline void hwce_x_feat_stride_length_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_FEAT_STRIDE_LENGTH_CTX1_OFFSET, value);
}


static inline uint32_t hwce_x_in_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_IN_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_x_in_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_IN_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_w_base_addr_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_W_BASE_ADDR_CTX1_OFFSET);
}

static inline void hwce_w_base_addr_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_W_BASE_ADDR_CTX1_OFFSET, value);
}


static inline uint32_t hwce_job_config0_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_JOB_CONFIG0_CTX1_OFFSET);
}

static inline void hwce_job_config0_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG0_CTX1_OFFSET, value);
}


static inline uint32_t hwce_job_config1_ctx1_get(uint32_t base)
{
    return GAP_READ(base, HWCE_JOB_CONFIG1_CTX1_OFFSET);
}

static inline void hwce_job_config1_ctx1_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_CTX1_OFFSET, value);
}


#endif

#endif
