#ifndef __HAL_IMA_V1_H__
#define __HAL_IMA_V1_H__

/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors:  Geethan Karunaratne, IBM <kar@zurich.ibm.com>
 */

#include "pmsis.h"

/* LOW-LEVEL HAL */
#define IMA_ADDR_BASE ARCHI_IMA_ADDR
#define IMA_ADDR_SPACE 0x00000100

// For all the following functions we use __builtin_pulp_OffsetedWrite and __builtin_pulp_OffsetedRead
// instead of classic load/store because otherwise the compiler is not able to correctly factorize
// the HWME base in case several accesses are done, ending up with twice more code

#if defined(__riscv__) && !defined(RV_ISA_RV32)
#define IMA_WRITE(value, offset) __builtin_pulp_OffsetedWrite(value, (int *)IMA_ADDR_BASE, offset)
#define IMA_READ(offset) __builtin_pulp_OffsetedRead((int *)IMA_ADDR_BASE, offset)
#else
#define IMA_WRITE(value, offset) pulp_write32(IMA_ADDR_BASE + (offset), value)
#define IMA_READ(offset) pulp_read32(IMA_ADDR_BASE + (offset))
#endif


static inline void ima_trigger_job() {
  IMA_WRITE(0, IMA_TRIGGER);
}
static inline int ima_acquire() {
  return IMA_READ(IMA_ACQUIRE);
}
static inline int ima_get_finished_jobs() {
  return IMA_READ(IMA_FINISHED_JOBS);
}
static inline int ima_get_status() {
  return IMA_READ(IMA_STATUS);
}
static inline int ima_get_running_task_id() {
  return IMA_READ(IMA_RUNNING_TASK);
}
static inline void ima_soft_clear() {
  IMA_WRITE(0,IMA_SOFT_CLEAR);
}
static inline int ima_get_state() {
  return IMA_READ(IMA_CHECK_STATE);
}

static inline void ima_adc_range(unsigned int adc_low,unsigned int adc_high) {
  IMA_WRITE(adc_low,IMA_ADC_LOW);
  IMA_WRITE(adc_high,IMA_ADC_HIGH);
}

static inline void ima_set_plot(unsigned int start_x,unsigned int start_y,unsigned int width,unsigned int height) {
  IMA_WRITE(start_x,IMA_P_START_X);
  IMA_WRITE(start_y,IMA_P_START_Y);
  IMA_WRITE(width,IMA_P_WIDTH);
  IMA_WRITE(height,IMA_P_HIGHT);
}

static inline void ima_submit_plot(unsigned int plot_val) {
  IMA_WRITE(plot_val,IMA_SUBMIT_PLOT);  
}

static inline void ima_set_job_params(unsigned int start_x,
                                      unsigned int start_y,
                                      unsigned int width,
                                      unsigned int height,
                                      unsigned int fetch_size,
                                      unsigned int store_size,
                                      unsigned int job_line_length,
                                      unsigned int job_feat_stride,
                                      unsigned int job_feat_length,
                                      unsigned int num_of_jobs){

  IMA_WRITE(start_x,IMA_J_START_X);
  IMA_WRITE(start_y,IMA_J_START_Y);
  IMA_WRITE(width,IMA_J_WIDTH);
  IMA_WRITE(height,IMA_J_HIGHT);
  IMA_WRITE(fetch_size,IMA_FETCH_LENGTH);
  IMA_WRITE(store_size,IMA_STORE_LENGTH);
  IMA_WRITE(job_line_length,IMA_JOB_LINE_LENGTH);
  IMA_WRITE(job_feat_stride,IMA_JOB_FEAT_STRIDE);
  IMA_WRITE(job_feat_length,IMA_JOB_FEAT_LENGTH);
  IMA_WRITE(num_of_jobs, IMA_NUM_JOBS);
}

static inline void ima_set_multi_jobs_param(unsigned int alpha_in_length,
                                            unsigned int alpha_in_stride,
                                            unsigned int beta_in_length,
                                            unsigned int beta_in_stride,
                                            unsigned int alpha_out_length,
                                            unsigned int alpha_out_stride,
                                            unsigned int beta_out_length,
                                            unsigned int beta_out_stride){
  IMA_WRITE(alpha_in_length, IMA_ALPHA_IN_LENGTH);
  IMA_WRITE(alpha_in_stride, IMA_ALPHA_IN_STRIDE);
  IMA_WRITE(beta_in_length,  IMA_BETA_IN_LENGTH);
  IMA_WRITE(beta_in_stride,  IMA_BETA_IN_STRIDE);
                                    
  IMA_WRITE(alpha_out_length, IMA_ALPHA_OUT_LENGTH);
  IMA_WRITE(alpha_out_stride, IMA_ALPHA_OUT_STRIDE);
  IMA_WRITE(beta_out_length,  IMA_BETA_OUT_LENGTH);
  IMA_WRITE(beta_out_stride,  IMA_BETA_OUT_STRIDE);
}

static inline void ima_set_streamer_params(unsigned int src_addr,unsigned int dest_addr,unsigned int src_strd,unsigned int dst_strd) {
  IMA_WRITE(src_addr,IMA_J_SRC_ADDR);
  IMA_WRITE(dest_addr,IMA_J_DST_ADDR);
  IMA_WRITE(src_strd,IMA_J_SRC_STRID);
  IMA_WRITE(dst_strd,IMA_J_DST_STRID);
}

static inline unsigned int ima_read_val(unsigned int addr_x,unsigned int addr_y) {
  IMA_WRITE(addr_x,IMA_PR_ADDR_X);
  IMA_WRITE(addr_y,IMA_PR_ADDR_Y);
  return IMA_READ(IMA_PR_VAL);
}


static inline void plp_hwme_enable() {
  *(volatile int*) (ARCHI_SOC_EU_ADDR + (3 << 3)) |=  0xc00;
}

static inline void plp_hwme_disable() {
  *(volatile int*) (ARCHI_SOC_EU_ADDR + (3 << 3)) &= ~0xc00;
}

static inline void plp_hwme_cluster_enable() {
  *(volatile int*) (ARCHI_CLUSTER_CTRL_ADDR + (3 << 3)) |=  0xc00;
}

static inline void plp_hwme_cluster_disable() {
  *(volatile int*) (ARCHI_CLUSTER_CTRL_ADDR + (3 << 3)) &= ~0xc00;
}

#endif