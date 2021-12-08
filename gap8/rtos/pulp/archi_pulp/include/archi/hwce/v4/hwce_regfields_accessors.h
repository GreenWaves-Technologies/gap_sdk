
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

#ifndef __ARCHI_HWCE_REGFIELDS_ACCESSORS__
#define __ARCHI_HWCE_REGFIELDS_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>
#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline void hwce_trigger_any_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_TRIGGER_OFFSET, value);
}

static inline uint32_t hwce_trigger_any_get(uint32_t base)
{
    return GAP_READ(base, HWCE_TRIGGER_OFFSET);
}

static inline int32_t hwce_trigger_any_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_TRIGGER_OFFSET);
}


static inline void hwce_acquire_id_err_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_ACQUIRE_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_ACQUIRE_OFFSET), value, 8, 0));
}

static inline uint32_t hwce_acquire_id_err_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_ACQUIRE_OFFSET), 8, 0);
}

static inline int32_t hwce_acquire_id_err_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_ACQUIRE_OFFSET), 8, 0);
}


static inline void hwce_acquire_err_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_ACQUIRE_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_ACQUIRE_OFFSET), value, 24, 8));
}

static inline uint32_t hwce_acquire_err_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_ACQUIRE_OFFSET), 24, 8);
}

static inline int32_t hwce_acquire_err_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_ACQUIRE_OFFSET), 24, 8);
}


static inline void hwce_finished_jobs_jobs_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_FINISHED_JOBS_OFFSET, value);
}

static inline uint32_t hwce_finished_jobs_jobs_get(uint32_t base)
{
    return GAP_READ(base, HWCE_FINISHED_JOBS_OFFSET);
}

static inline int32_t hwce_finished_jobs_jobs_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_FINISHED_JOBS_OFFSET);
}


static inline void hwce_status_st_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_STATUS_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_STATUS_OFFSET), value, 1, 0));
}

static inline uint32_t hwce_status_st_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_STATUS_OFFSET), 1, 0);
}

static inline int32_t hwce_status_st_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_STATUS_OFFSET), 1, 0);
}


static inline void hwce_running_job_id_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_RUNNING_JOB_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_RUNNING_JOB_OFFSET), value, 8, 0));
}

static inline uint32_t hwce_running_job_id_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_RUNNING_JOB_OFFSET), 8, 0);
}

static inline int32_t hwce_running_job_id_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_RUNNING_JOB_OFFSET), 8, 0);
}


static inline void hwce_soft_clear_any_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_SOFT_CLEAR_OFFSET, value);
}

static inline uint32_t hwce_soft_clear_any_get(uint32_t base)
{
    return GAP_READ(base, HWCE_SOFT_CLEAR_OFFSET);
}

static inline int32_t hwce_soft_clear_any_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_SOFT_CLEAR_OFFSET);
}


static inline void hwce_gen_config0_qf_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 6, 0));
}

static inline uint32_t hwce_gen_config0_qf_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 6, 0);
}

static inline int32_t hwce_gen_config0_qf_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 6, 0);
}


static inline void hwce_gen_config0_nf_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 1, 6));
}

static inline uint32_t hwce_gen_config0_nf_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 6);
}

static inline int32_t hwce_gen_config0_nf_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 6);
}


static inline void hwce_gen_config0_ny_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 1, 7));
}

static inline uint32_t hwce_gen_config0_ny_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 7);
}

static inline int32_t hwce_gen_config0_ny_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 7);
}


static inline void hwce_gen_config0_uns_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 1, 8));
}

static inline uint32_t hwce_gen_config0_uns_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 8);
}

static inline int32_t hwce_gen_config0_uns_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 8);
}


static inline void hwce_gen_config0_vect_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 2, 9));
}

static inline uint32_t hwce_gen_config0_vect_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 2, 9);
}

static inline int32_t hwce_gen_config0_vect_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 2, 9);
}


static inline void hwce_gen_config0_conv_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 2, 11));
}

static inline uint32_t hwce_gen_config0_conv_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 2, 11);
}

static inline int32_t hwce_gen_config0_conv_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 2, 11);
}


static inline void hwce_gen_config0_ncp_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 1, 13));
}

static inline uint32_t hwce_gen_config0_ncp_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 13);
}

static inline int32_t hwce_gen_config0_ncp_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 13);
}


static inline void hwce_gen_config0_rnd_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 1, 14));
}

static inline uint32_t hwce_gen_config0_rnd_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 14);
}

static inline int32_t hwce_gen_config0_rnd_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 1, 14);
}


static inline void hwce_gen_config0_wstride_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), value, 16, 16));
}

static inline uint32_t hwce_gen_config0_wstride_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 16, 16);
}

static inline int32_t hwce_gen_config0_wstride_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG0_OFFSET), 16, 16);
}


static inline void hwce_gen_config1_pixshiftl_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), value, 5, 0));
}

static inline uint32_t hwce_gen_config1_pixshiftl_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), 5, 0);
}

static inline int32_t hwce_gen_config1_pixshiftl_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), 5, 0);
}


static inline void hwce_gen_config1_pixmode_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), value, 2, 8));
}

static inline uint32_t hwce_gen_config1_pixmode_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), 2, 8);
}

static inline int32_t hwce_gen_config1_pixmode_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), 2, 8);
}


static inline void hwce_gen_config1_pixshiftr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_GEN_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), value, 5, 16));
}

static inline uint32_t hwce_gen_config1_pixshiftr_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), 5, 16);
}

static inline int32_t hwce_gen_config1_pixshiftr_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_GEN_CONFIG1_OFFSET), 5, 16);
}


static inline void hwce_y_trans_size_size_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_TRANS_SIZE_OFFSET, value);
}

static inline uint32_t hwce_y_trans_size_size_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_TRANS_SIZE_OFFSET);
}

static inline int32_t hwce_y_trans_size_size_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_TRANS_SIZE_OFFSET);
}


static inline void hwce_y_line_stride_length_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET), value, 16, 0));
}

static inline uint32_t hwce_y_line_stride_length_length_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET), 16, 0);
}

static inline int32_t hwce_y_line_stride_length_length_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET), 16, 0);
}


static inline void hwce_y_line_stride_length_stride_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET), value, 16, 16));
}

static inline uint32_t hwce_y_line_stride_length_stride_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET), 16, 16);
}

static inline int32_t hwce_y_line_stride_length_stride_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_Y_LINE_STRIDE_LENGTH_OFFSET), 16, 16);
}


static inline void hwce_y_feat_stride_length_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET), value, 16, 0));
}

static inline uint32_t hwce_y_feat_stride_length_length_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET), 16, 0);
}

static inline int32_t hwce_y_feat_stride_length_length_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET), 16, 0);
}


static inline void hwce_y_feat_stride_length_stride_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET), value, 16, 16));
}

static inline uint32_t hwce_y_feat_stride_length_stride_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET), 16, 16);
}

static inline int32_t hwce_y_feat_stride_length_stride_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_Y_FEAT_STRIDE_LENGTH_OFFSET), 16, 16);
}


static inline void hwce_y_out_3_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_3_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_out_3_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_3_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_out_3_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_3_BASE_ADDR_OFFSET);
}


static inline void hwce_y_out_2_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_2_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_out_2_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_2_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_out_2_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_2_BASE_ADDR_OFFSET);
}


static inline void hwce_y_out_1_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_1_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_out_1_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_1_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_out_1_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_1_BASE_ADDR_OFFSET);
}


static inline void hwce_y_out_0_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_OUT_0_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_out_0_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_0_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_out_0_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_OUT_0_BASE_ADDR_OFFSET);
}


static inline void hwce_y_in_3_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_3_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_in_3_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_3_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_in_3_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_3_BASE_ADDR_OFFSET);
}


static inline void hwce_y_in_2_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_2_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_in_2_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_2_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_in_2_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_2_BASE_ADDR_OFFSET);
}


static inline void hwce_y_in_1_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_1_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_in_1_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_1_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_in_1_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_1_BASE_ADDR_OFFSET);
}


static inline void hwce_y_in_0_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_Y_IN_0_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_y_in_0_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_0_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_y_in_0_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_Y_IN_0_BASE_ADDR_OFFSET);
}


static inline void hwce_x_trans_size_size_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_TRANS_SIZE_OFFSET, value);
}

static inline uint32_t hwce_x_trans_size_size_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_TRANS_SIZE_OFFSET);
}

static inline int32_t hwce_x_trans_size_size_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_X_TRANS_SIZE_OFFSET);
}


static inline void hwce_x_line_stride_length_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET), value, 16, 0));
}

static inline uint32_t hwce_x_line_stride_length_length_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET), 16, 0);
}

static inline int32_t hwce_x_line_stride_length_length_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET), 16, 0);
}


static inline void hwce_x_line_stride_length_stride_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET), value, 16, 16));
}

static inline uint32_t hwce_x_line_stride_length_stride_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET), 16, 16);
}

static inline int32_t hwce_x_line_stride_length_stride_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_X_LINE_STRIDE_LENGTH_OFFSET), 16, 16);
}


static inline void hwce_x_feat_stride_length_length_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET), value, 16, 0));
}

static inline uint32_t hwce_x_feat_stride_length_length_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET), 16, 0);
}

static inline int32_t hwce_x_feat_stride_length_length_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET), 16, 0);
}


static inline void hwce_x_feat_stride_length_stride_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET), value, 16, 16));
}

static inline uint32_t hwce_x_feat_stride_length_stride_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET), 16, 16);
}

static inline int32_t hwce_x_feat_stride_length_stride_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_X_FEAT_STRIDE_LENGTH_OFFSET), 16, 16);
}


static inline void hwce_x_in_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_X_IN_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_x_in_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_X_IN_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_x_in_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_X_IN_BASE_ADDR_OFFSET);
}


static inline void hwce_w_base_addr_addr_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_W_BASE_ADDR_OFFSET, value);
}

static inline uint32_t hwce_w_base_addr_addr_get(uint32_t base)
{
    return GAP_READ(base, HWCE_W_BASE_ADDR_OFFSET);
}

static inline int32_t hwce_w_base_addr_addr_gets(uint32_t base)
{
    return GAP_READ(base, HWCE_W_BASE_ADDR_OFFSET);
}


static inline void hwce_job_config0_lbuflen_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET), value, 10, 0));
}

static inline uint32_t hwce_job_config0_lbuflen_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET), 10, 0);
}

static inline int32_t hwce_job_config0_lbuflen_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET), 10, 0);
}


static inline void hwce_job_config0_noyconst_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG0_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET), value, 16, 16));
}

static inline uint32_t hwce_job_config0_noyconst_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET), 16, 16);
}

static inline int32_t hwce_job_config0_noyconst_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG0_OFFSET), 16, 16);
}


static inline void hwce_job_config1_vect_disable_mask_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), value, 4, 0));
}

static inline uint32_t hwce_job_config1_vect_disable_mask_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 4, 0);
}

static inline int32_t hwce_job_config1_vect_disable_mask_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 4, 0);
}


static inline void hwce_job_config1_wof_param_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), value, 6, 8));
}

static inline uint32_t hwce_job_config1_wof_param_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 6, 8);
}

static inline int32_t hwce_job_config1_wof_param_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 6, 8);
}


static inline void hwce_job_config1_wif_param_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), value, 6, 16));
}

static inline uint32_t hwce_job_config1_wif_param_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 6, 16);
}

static inline int32_t hwce_job_config1_wif_param_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 6, 16);
}


static inline void hwce_job_config1_lo_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), value, 1, 24));
}

static inline uint32_t hwce_job_config1_lo_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 1, 24);
}

static inline int32_t hwce_job_config1_lo_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 1, 24);
}


static inline void hwce_job_config1_ln_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, HWCE_JOB_CONFIG1_OFFSET, GAP_BINSERT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), value, 1, 25));
}

static inline uint32_t hwce_job_config1_ln_get(uint32_t base)
{
    return GAP_BEXTRACTU(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 1, 25);
}

static inline int32_t hwce_job_config1_ln_gets(uint32_t base)
{
    return GAP_BEXTRACT(GAP_READ(base, HWCE_JOB_CONFIG1_OFFSET), 1, 25);
}


#endif

#endif
