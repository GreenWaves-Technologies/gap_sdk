
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

#ifndef __ARCHI_HWCE_GVSOC__
#define __ARCHI_HWCE_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_hwce_trigger : public vp::reg_32
{
public:
    inline void any_set(uint32_t value) { this->set_field(value, HWCE_TRIGGER_ANY_BIT, HWCE_TRIGGER_ANY_WIDTH); }
    inline uint32_t any_get() { return this->get_field(HWCE_TRIGGER_ANY_BIT, HWCE_TRIGGER_ANY_WIDTH); }
    vp_hwce_trigger()
    {
        this->hw_name = "TRIGGER";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ANY", 0, 32));
    }
};

class vp_hwce_acquire : public vp::reg_32
{
public:
    inline void id_err_set(uint32_t value) { this->set_field(value, HWCE_ACQUIRE_ID_ERR_BIT, HWCE_ACQUIRE_ID_ERR_WIDTH); }
    inline uint32_t id_err_get() { return this->get_field(HWCE_ACQUIRE_ID_ERR_BIT, HWCE_ACQUIRE_ID_ERR_WIDTH); }
    inline void err_set(uint32_t value) { this->set_field(value, HWCE_ACQUIRE_ERR_BIT, HWCE_ACQUIRE_ERR_WIDTH); }
    inline uint32_t err_get() { return this->get_field(HWCE_ACQUIRE_ERR_BIT, HWCE_ACQUIRE_ERR_WIDTH); }
    vp_hwce_acquire()
    {
        this->hw_name = "ACQUIRE";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ID_ERR", 0, 8));
        this->regfields.push_back(new vp::regfield("ERR", 8, 24));
    }
};

class vp_hwce_finished_jobs : public vp::reg_32
{
public:
    inline void jobs_set(uint32_t value) { this->set_field(value, HWCE_FINISHED_JOBS_JOBS_BIT, HWCE_FINISHED_JOBS_JOBS_WIDTH); }
    inline uint32_t jobs_get() { return this->get_field(HWCE_FINISHED_JOBS_JOBS_BIT, HWCE_FINISHED_JOBS_JOBS_WIDTH); }
    vp_hwce_finished_jobs()
    {
        this->hw_name = "FINISHED_JOBS";
        this->offset = 0x8;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("JOBS", 0, 32));
    }
};

class vp_hwce_status : public vp::reg_32
{
public:
    inline void st_set(uint32_t value) { this->set_field(value, HWCE_STATUS_ST_BIT, HWCE_STATUS_ST_WIDTH); }
    inline uint32_t st_get() { return this->get_field(HWCE_STATUS_ST_BIT, HWCE_STATUS_ST_WIDTH); }
    vp_hwce_status()
    {
        this->hw_name = "STATUS";
        this->offset = 0xc;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ST", 0, 1));
    }
};

class vp_hwce_running_job : public vp::reg_32
{
public:
    inline void id_set(uint32_t value) { this->set_field(value, HWCE_RUNNING_JOB_ID_BIT, HWCE_RUNNING_JOB_ID_WIDTH); }
    inline uint32_t id_get() { return this->get_field(HWCE_RUNNING_JOB_ID_BIT, HWCE_RUNNING_JOB_ID_WIDTH); }
    vp_hwce_running_job()
    {
        this->hw_name = "RUNNING_JOB";
        this->offset = 0x10;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ID", 0, 8));
    }
};

class vp_hwce_soft_clear : public vp::reg_32
{
public:
    inline void any_set(uint32_t value) { this->set_field(value, HWCE_SOFT_CLEAR_ANY_BIT, HWCE_SOFT_CLEAR_ANY_WIDTH); }
    inline uint32_t any_get() { return this->get_field(HWCE_SOFT_CLEAR_ANY_BIT, HWCE_SOFT_CLEAR_ANY_WIDTH); }
    vp_hwce_soft_clear()
    {
        this->hw_name = "SOFT_CLEAR";
        this->offset = 0x14;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ANY", 0, 32));
    }
};

class vp_hwce_gen_config0 : public vp::reg_32
{
public:
    inline void qf_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_QF_BIT, HWCE_GEN_CONFIG0_QF_WIDTH); }
    inline uint32_t qf_get() { return this->get_field(HWCE_GEN_CONFIG0_QF_BIT, HWCE_GEN_CONFIG0_QF_WIDTH); }
    inline void nf_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_NF_BIT, HWCE_GEN_CONFIG0_NF_WIDTH); }
    inline uint32_t nf_get() { return this->get_field(HWCE_GEN_CONFIG0_NF_BIT, HWCE_GEN_CONFIG0_NF_WIDTH); }
    inline void ny_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_NY_BIT, HWCE_GEN_CONFIG0_NY_WIDTH); }
    inline uint32_t ny_get() { return this->get_field(HWCE_GEN_CONFIG0_NY_BIT, HWCE_GEN_CONFIG0_NY_WIDTH); }
    inline void uns_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_UNS_BIT, HWCE_GEN_CONFIG0_UNS_WIDTH); }
    inline uint32_t uns_get() { return this->get_field(HWCE_GEN_CONFIG0_UNS_BIT, HWCE_GEN_CONFIG0_UNS_WIDTH); }
    inline void vect_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_VECT_BIT, HWCE_GEN_CONFIG0_VECT_WIDTH); }
    inline uint32_t vect_get() { return this->get_field(HWCE_GEN_CONFIG0_VECT_BIT, HWCE_GEN_CONFIG0_VECT_WIDTH); }
    inline void conv_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_CONV_BIT, HWCE_GEN_CONFIG0_CONV_WIDTH); }
    inline uint32_t conv_get() { return this->get_field(HWCE_GEN_CONFIG0_CONV_BIT, HWCE_GEN_CONFIG0_CONV_WIDTH); }
    inline void ncp_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_NCP_BIT, HWCE_GEN_CONFIG0_NCP_WIDTH); }
    inline uint32_t ncp_get() { return this->get_field(HWCE_GEN_CONFIG0_NCP_BIT, HWCE_GEN_CONFIG0_NCP_WIDTH); }
    inline void rnd_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_RND_BIT, HWCE_GEN_CONFIG0_RND_WIDTH); }
    inline uint32_t rnd_get() { return this->get_field(HWCE_GEN_CONFIG0_RND_BIT, HWCE_GEN_CONFIG0_RND_WIDTH); }
    inline void wstride_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG0_WSTRIDE_BIT, HWCE_GEN_CONFIG0_WSTRIDE_WIDTH); }
    inline uint32_t wstride_get() { return this->get_field(HWCE_GEN_CONFIG0_WSTRIDE_BIT, HWCE_GEN_CONFIG0_WSTRIDE_WIDTH); }
    vp_hwce_gen_config0()
    {
        this->hw_name = "GEN_CONFIG0";
        this->offset = 0x20;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("QF", 0, 6));
        this->regfields.push_back(new vp::regfield("NF", 6, 1));
        this->regfields.push_back(new vp::regfield("NY", 7, 1));
        this->regfields.push_back(new vp::regfield("UNS", 8, 1));
        this->regfields.push_back(new vp::regfield("VECT", 9, 2));
        this->regfields.push_back(new vp::regfield("CONV", 11, 2));
        this->regfields.push_back(new vp::regfield("NCP", 13, 1));
        this->regfields.push_back(new vp::regfield("RND", 14, 1));
        this->regfields.push_back(new vp::regfield("WSTRIDE", 16, 16));
    }
};

class vp_hwce_gen_config1 : public vp::reg_32
{
public:
    inline void pixshiftl_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG1_PIXSHIFTL_BIT, HWCE_GEN_CONFIG1_PIXSHIFTL_WIDTH); }
    inline uint32_t pixshiftl_get() { return this->get_field(HWCE_GEN_CONFIG1_PIXSHIFTL_BIT, HWCE_GEN_CONFIG1_PIXSHIFTL_WIDTH); }
    inline void pixmode_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG1_PIXMODE_BIT, HWCE_GEN_CONFIG1_PIXMODE_WIDTH); }
    inline uint32_t pixmode_get() { return this->get_field(HWCE_GEN_CONFIG1_PIXMODE_BIT, HWCE_GEN_CONFIG1_PIXMODE_WIDTH); }
    inline void pixshiftr_set(uint32_t value) { this->set_field(value, HWCE_GEN_CONFIG1_PIXSHIFTR_BIT, HWCE_GEN_CONFIG1_PIXSHIFTR_WIDTH); }
    inline uint32_t pixshiftr_get() { return this->get_field(HWCE_GEN_CONFIG1_PIXSHIFTR_BIT, HWCE_GEN_CONFIG1_PIXSHIFTR_WIDTH); }
    vp_hwce_gen_config1()
    {
        this->hw_name = "GEN_CONFIG1";
        this->offset = 0x24;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("PIXSHIFTL", 0, 5));
        this->regfields.push_back(new vp::regfield("PIXMODE", 8, 2));
        this->regfields.push_back(new vp::regfield("PIXSHIFTR", 16, 5));
    }
};

class vp_hwce_y_trans_size : public vp::reg_32
{
public:
    inline void size_set(uint32_t value) { this->set_field(value, HWCE_Y_TRANS_SIZE_SIZE_BIT, HWCE_Y_TRANS_SIZE_SIZE_WIDTH); }
    inline uint32_t size_get() { return this->get_field(HWCE_Y_TRANS_SIZE_SIZE_BIT, HWCE_Y_TRANS_SIZE_SIZE_WIDTH); }
    vp_hwce_y_trans_size()
    {
        this->hw_name = "Y_TRANS_SIZE";
        this->offset = 0x40;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("SIZE", 0, 32));
    }
};

class vp_hwce_y_line_stride_length : public vp::reg_32
{
public:
    inline void length_set(uint32_t value) { this->set_field(value, HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_BIT, HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline uint32_t length_get() { return this->get_field(HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_BIT, HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline void stride_set(uint32_t value) { this->set_field(value, HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_BIT, HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_WIDTH); }
    inline uint32_t stride_get() { return this->get_field(HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_BIT, HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_WIDTH); }
    vp_hwce_y_line_stride_length()
    {
        this->hw_name = "Y_LINE_STRIDE_LENGTH";
        this->offset = 0x44;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("LENGTH", 0, 16));
        this->regfields.push_back(new vp::regfield("STRIDE", 16, 16));
    }
};

class vp_hwce_y_feat_stride_length : public vp::reg_32
{
public:
    inline void length_set(uint32_t value) { this->set_field(value, HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_BIT, HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline uint32_t length_get() { return this->get_field(HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_BIT, HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline void stride_set(uint32_t value) { this->set_field(value, HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_BIT, HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_WIDTH); }
    inline uint32_t stride_get() { return this->get_field(HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_BIT, HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_WIDTH); }
    vp_hwce_y_feat_stride_length()
    {
        this->hw_name = "Y_FEAT_STRIDE_LENGTH";
        this->offset = 0x48;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("LENGTH", 0, 16));
        this->regfields.push_back(new vp::regfield("STRIDE", 16, 16));
    }
};

class vp_hwce_y_out_3_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_OUT_3_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_3_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_OUT_3_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_3_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_out_3_base_addr()
    {
        this->hw_name = "Y_OUT_3_BASE_ADDR";
        this->offset = 0x4c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_out_2_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_OUT_2_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_2_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_OUT_2_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_2_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_out_2_base_addr()
    {
        this->hw_name = "Y_OUT_2_BASE_ADDR";
        this->offset = 0x50;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_out_1_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_OUT_1_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_1_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_OUT_1_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_1_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_out_1_base_addr()
    {
        this->hw_name = "Y_OUT_1_BASE_ADDR";
        this->offset = 0x54;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_out_0_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_OUT_0_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_0_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_OUT_0_BASE_ADDR_ADDR_BIT, HWCE_Y_OUT_0_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_out_0_base_addr()
    {
        this->hw_name = "Y_OUT_0_BASE_ADDR";
        this->offset = 0x58;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_in_3_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_IN_3_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_3_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_IN_3_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_3_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_in_3_base_addr()
    {
        this->hw_name = "Y_IN_3_BASE_ADDR";
        this->offset = 0x5c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_in_2_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_IN_2_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_2_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_IN_2_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_2_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_in_2_base_addr()
    {
        this->hw_name = "Y_IN_2_BASE_ADDR";
        this->offset = 0x60;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_in_1_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_IN_1_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_1_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_IN_1_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_1_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_in_1_base_addr()
    {
        this->hw_name = "Y_IN_1_BASE_ADDR";
        this->offset = 0x64;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_y_in_0_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_Y_IN_0_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_0_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_Y_IN_0_BASE_ADDR_ADDR_BIT, HWCE_Y_IN_0_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_y_in_0_base_addr()
    {
        this->hw_name = "Y_IN_0_BASE_ADDR";
        this->offset = 0x68;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_x_trans_size : public vp::reg_32
{
public:
    inline void size_set(uint32_t value) { this->set_field(value, HWCE_X_TRANS_SIZE_SIZE_BIT, HWCE_X_TRANS_SIZE_SIZE_WIDTH); }
    inline uint32_t size_get() { return this->get_field(HWCE_X_TRANS_SIZE_SIZE_BIT, HWCE_X_TRANS_SIZE_SIZE_WIDTH); }
    vp_hwce_x_trans_size()
    {
        this->hw_name = "X_TRANS_SIZE";
        this->offset = 0x6c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("SIZE", 0, 32));
    }
};

class vp_hwce_x_line_stride_length : public vp::reg_32
{
public:
    inline void length_set(uint32_t value) { this->set_field(value, HWCE_X_LINE_STRIDE_LENGTH_LENGTH_BIT, HWCE_X_LINE_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline uint32_t length_get() { return this->get_field(HWCE_X_LINE_STRIDE_LENGTH_LENGTH_BIT, HWCE_X_LINE_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline void stride_set(uint32_t value) { this->set_field(value, HWCE_X_LINE_STRIDE_LENGTH_STRIDE_BIT, HWCE_X_LINE_STRIDE_LENGTH_STRIDE_WIDTH); }
    inline uint32_t stride_get() { return this->get_field(HWCE_X_LINE_STRIDE_LENGTH_STRIDE_BIT, HWCE_X_LINE_STRIDE_LENGTH_STRIDE_WIDTH); }
    vp_hwce_x_line_stride_length()
    {
        this->hw_name = "X_LINE_STRIDE_LENGTH";
        this->offset = 0x70;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("LENGTH", 0, 16));
        this->regfields.push_back(new vp::regfield("STRIDE", 16, 16));
    }
};

class vp_hwce_x_feat_stride_length : public vp::reg_32
{
public:
    inline void length_set(uint32_t value) { this->set_field(value, HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_BIT, HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline uint32_t length_get() { return this->get_field(HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_BIT, HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_WIDTH); }
    inline void stride_set(uint32_t value) { this->set_field(value, HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_BIT, HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_WIDTH); }
    inline uint32_t stride_get() { return this->get_field(HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_BIT, HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_WIDTH); }
    vp_hwce_x_feat_stride_length()
    {
        this->hw_name = "X_FEAT_STRIDE_LENGTH";
        this->offset = 0x74;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("LENGTH", 0, 16));
        this->regfields.push_back(new vp::regfield("STRIDE", 16, 16));
    }
};

class vp_hwce_x_in_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_X_IN_BASE_ADDR_ADDR_BIT, HWCE_X_IN_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_X_IN_BASE_ADDR_ADDR_BIT, HWCE_X_IN_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_x_in_base_addr()
    {
        this->hw_name = "X_IN_BASE_ADDR";
        this->offset = 0x78;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_w_base_addr : public vp::reg_32
{
public:
    inline void addr_set(uint32_t value) { this->set_field(value, HWCE_W_BASE_ADDR_ADDR_BIT, HWCE_W_BASE_ADDR_ADDR_WIDTH); }
    inline uint32_t addr_get() { return this->get_field(HWCE_W_BASE_ADDR_ADDR_BIT, HWCE_W_BASE_ADDR_ADDR_WIDTH); }
    vp_hwce_w_base_addr()
    {
        this->hw_name = "W_BASE_ADDR";
        this->offset = 0x7c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("ADDR", 0, 32));
    }
};

class vp_hwce_job_config0 : public vp::reg_32
{
public:
    inline void lbuflen_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG0_LBUFLEN_BIT, HWCE_JOB_CONFIG0_LBUFLEN_WIDTH); }
    inline uint32_t lbuflen_get() { return this->get_field(HWCE_JOB_CONFIG0_LBUFLEN_BIT, HWCE_JOB_CONFIG0_LBUFLEN_WIDTH); }
    inline void noyconst_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG0_NOYCONST_BIT, HWCE_JOB_CONFIG0_NOYCONST_WIDTH); }
    inline uint32_t noyconst_get() { return this->get_field(HWCE_JOB_CONFIG0_NOYCONST_BIT, HWCE_JOB_CONFIG0_NOYCONST_WIDTH); }
    vp_hwce_job_config0()
    {
        this->hw_name = "JOB_CONFIG0";
        this->offset = 0x80;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("LBUFLEN", 0, 10));
        this->regfields.push_back(new vp::regfield("NOYCONST", 16, 16));
    }
};

class vp_hwce_job_config1 : public vp::reg_32
{
public:
    inline void vect_disable_mask_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_BIT, HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_WIDTH); }
    inline uint32_t vect_disable_mask_get() { return this->get_field(HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_BIT, HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_WIDTH); }
    inline void wof_param_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG1_WOF_PARAM_BIT, HWCE_JOB_CONFIG1_WOF_PARAM_WIDTH); }
    inline uint32_t wof_param_get() { return this->get_field(HWCE_JOB_CONFIG1_WOF_PARAM_BIT, HWCE_JOB_CONFIG1_WOF_PARAM_WIDTH); }
    inline void wif_param_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG1_WIF_PARAM_BIT, HWCE_JOB_CONFIG1_WIF_PARAM_WIDTH); }
    inline uint32_t wif_param_get() { return this->get_field(HWCE_JOB_CONFIG1_WIF_PARAM_BIT, HWCE_JOB_CONFIG1_WIF_PARAM_WIDTH); }
    inline void lo_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG1_LO_BIT, HWCE_JOB_CONFIG1_LO_WIDTH); }
    inline uint32_t lo_get() { return this->get_field(HWCE_JOB_CONFIG1_LO_BIT, HWCE_JOB_CONFIG1_LO_WIDTH); }
    inline void ln_set(uint32_t value) { this->set_field(value, HWCE_JOB_CONFIG1_LN_BIT, HWCE_JOB_CONFIG1_LN_WIDTH); }
    inline uint32_t ln_get() { return this->get_field(HWCE_JOB_CONFIG1_LN_BIT, HWCE_JOB_CONFIG1_LN_WIDTH); }
    vp_hwce_job_config1()
    {
        this->hw_name = "JOB_CONFIG1";
        this->offset = 0x84;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("VECT_DISABLE_MASK", 0, 4));
        this->regfields.push_back(new vp::regfield("WOF_PARAM", 8, 6));
        this->regfields.push_back(new vp::regfield("WIF_PARAM", 16, 6));
        this->regfields.push_back(new vp::regfield("LO", 24, 1));
        this->regfields.push_back(new vp::regfield("LN", 25, 1));
    }
};

class vp_hwce_y_trans_size_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_trans_size_ctx0()
    {
        this->hw_name = "Y_TRANS_SIZE_CTX0";
        this->offset = 0x140;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_line_stride_length_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_line_stride_length_ctx0()
    {
        this->hw_name = "Y_LINE_STRIDE_LENGTH_CTX0";
        this->offset = 0x144;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_feat_stride_length_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_feat_stride_length_ctx0()
    {
        this->hw_name = "Y_FEAT_STRIDE_LENGTH_CTX0";
        this->offset = 0x148;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_3_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_out_3_base_addr_ctx0()
    {
        this->hw_name = "Y_OUT_3_BASE_ADDR_CTX0";
        this->offset = 0x14c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_2_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_out_2_base_addr_ctx0()
    {
        this->hw_name = "Y_OUT_2_BASE_ADDR_CTX0";
        this->offset = 0x150;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_1_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_out_1_base_addr_ctx0()
    {
        this->hw_name = "Y_OUT_1_BASE_ADDR_CTX0";
        this->offset = 0x154;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_0_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_out_0_base_addr_ctx0()
    {
        this->hw_name = "Y_OUT_0_BASE_ADDR_CTX0";
        this->offset = 0x158;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_3_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_in_3_base_addr_ctx0()
    {
        this->hw_name = "Y_IN_3_BASE_ADDR_CTX0";
        this->offset = 0x15c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_2_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_in_2_base_addr_ctx0()
    {
        this->hw_name = "Y_IN_2_BASE_ADDR_CTX0";
        this->offset = 0x160;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_1_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_in_1_base_addr_ctx0()
    {
        this->hw_name = "Y_IN_1_BASE_ADDR_CTX0";
        this->offset = 0x164;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_0_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_y_in_0_base_addr_ctx0()
    {
        this->hw_name = "Y_IN_0_BASE_ADDR_CTX0";
        this->offset = 0x168;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_trans_size_ctx0 : public vp::reg_32
{
public:
    vp_hwce_x_trans_size_ctx0()
    {
        this->hw_name = "X_TRANS_SIZE_CTX0";
        this->offset = 0x16c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_line_stride_length_ctx0 : public vp::reg_32
{
public:
    vp_hwce_x_line_stride_length_ctx0()
    {
        this->hw_name = "X_LINE_STRIDE_LENGTH_CTX0";
        this->offset = 0x170;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_feat_stride_length_ctx0 : public vp::reg_32
{
public:
    vp_hwce_x_feat_stride_length_ctx0()
    {
        this->hw_name = "X_FEAT_STRIDE_LENGTH_CTX0";
        this->offset = 0x174;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_in_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_x_in_base_addr_ctx0()
    {
        this->hw_name = "X_IN_BASE_ADDR_CTX0";
        this->offset = 0x178;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_w_base_addr_ctx0 : public vp::reg_32
{
public:
    vp_hwce_w_base_addr_ctx0()
    {
        this->hw_name = "W_BASE_ADDR_CTX0";
        this->offset = 0x17c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_job_config0_ctx0 : public vp::reg_32
{
public:
    vp_hwce_job_config0_ctx0()
    {
        this->hw_name = "JOB_CONFIG0_CTX0";
        this->offset = 0x180;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_job_config1_ctx0 : public vp::reg_32
{
public:
    vp_hwce_job_config1_ctx0()
    {
        this->hw_name = "JOB_CONFIG1_CTX0";
        this->offset = 0x184;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_trans_size_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_trans_size_ctx1()
    {
        this->hw_name = "Y_TRANS_SIZE_CTX1";
        this->offset = 0x240;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_line_stride_length_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_line_stride_length_ctx1()
    {
        this->hw_name = "Y_LINE_STRIDE_LENGTH_CTX1";
        this->offset = 0x244;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_feat_stride_length_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_feat_stride_length_ctx1()
    {
        this->hw_name = "Y_FEAT_STRIDE_LENGTH_CTX1";
        this->offset = 0x248;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_3_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_out_3_base_addr_ctx1()
    {
        this->hw_name = "Y_OUT_3_BASE_ADDR_CTX1";
        this->offset = 0x24c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_2_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_out_2_base_addr_ctx1()
    {
        this->hw_name = "Y_OUT_2_BASE_ADDR_CTX1";
        this->offset = 0x250;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_1_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_out_1_base_addr_ctx1()
    {
        this->hw_name = "Y_OUT_1_BASE_ADDR_CTX1";
        this->offset = 0x254;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_out_0_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_out_0_base_addr_ctx1()
    {
        this->hw_name = "Y_OUT_0_BASE_ADDR_CTX1";
        this->offset = 0x258;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_3_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_in_3_base_addr_ctx1()
    {
        this->hw_name = "Y_IN_3_BASE_ADDR_CTX1";
        this->offset = 0x25c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_2_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_in_2_base_addr_ctx1()
    {
        this->hw_name = "Y_IN_2_BASE_ADDR_CTX1";
        this->offset = 0x260;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_1_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_in_1_base_addr_ctx1()
    {
        this->hw_name = "Y_IN_1_BASE_ADDR_CTX1";
        this->offset = 0x264;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_y_in_0_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_y_in_0_base_addr_ctx1()
    {
        this->hw_name = "Y_IN_0_BASE_ADDR_CTX1";
        this->offset = 0x268;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_trans_size_ctx1 : public vp::reg_32
{
public:
    vp_hwce_x_trans_size_ctx1()
    {
        this->hw_name = "X_TRANS_SIZE_CTX1";
        this->offset = 0x26c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_line_stride_length_ctx1 : public vp::reg_32
{
public:
    vp_hwce_x_line_stride_length_ctx1()
    {
        this->hw_name = "X_LINE_STRIDE_LENGTH_CTX1";
        this->offset = 0x270;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_feat_stride_length_ctx1 : public vp::reg_32
{
public:
    vp_hwce_x_feat_stride_length_ctx1()
    {
        this->hw_name = "X_FEAT_STRIDE_LENGTH_CTX1";
        this->offset = 0x274;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_x_in_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_x_in_base_addr_ctx1()
    {
        this->hw_name = "X_IN_BASE_ADDR_CTX1";
        this->offset = 0x278;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_w_base_addr_ctx1 : public vp::reg_32
{
public:
    vp_hwce_w_base_addr_ctx1()
    {
        this->hw_name = "W_BASE_ADDR_CTX1";
        this->offset = 0x27c;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_job_config0_ctx1 : public vp::reg_32
{
public:
    vp_hwce_job_config0_ctx1()
    {
        this->hw_name = "JOB_CONFIG0_CTX1";
        this->offset = 0x280;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};

class vp_hwce_job_config1_ctx1 : public vp::reg_32
{
public:
    vp_hwce_job_config1_ctx1()
    {
        this->hw_name = "JOB_CONFIG1_CTX1";
        this->offset = 0x284;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
    }
};


class vp_regmap_hwce : public vp::regmap
{
public:
    vp_hwce_trigger trigger;
    vp_hwce_acquire acquire;
    vp_hwce_finished_jobs finished_jobs;
    vp_hwce_status status;
    vp_hwce_running_job running_job;
    vp_hwce_soft_clear soft_clear;
    vp_hwce_gen_config0 gen_config0;
    vp_hwce_gen_config1 gen_config1;
    vp_hwce_y_trans_size y_trans_size;
    vp_hwce_y_line_stride_length y_line_stride_length;
    vp_hwce_y_feat_stride_length y_feat_stride_length;
    vp_hwce_y_out_3_base_addr y_out_3_base_addr;
    vp_hwce_y_out_2_base_addr y_out_2_base_addr;
    vp_hwce_y_out_1_base_addr y_out_1_base_addr;
    vp_hwce_y_out_0_base_addr y_out_0_base_addr;
    vp_hwce_y_in_3_base_addr y_in_3_base_addr;
    vp_hwce_y_in_2_base_addr y_in_2_base_addr;
    vp_hwce_y_in_1_base_addr y_in_1_base_addr;
    vp_hwce_y_in_0_base_addr y_in_0_base_addr;
    vp_hwce_x_trans_size x_trans_size;
    vp_hwce_x_line_stride_length x_line_stride_length;
    vp_hwce_x_feat_stride_length x_feat_stride_length;
    vp_hwce_x_in_base_addr x_in_base_addr;
    vp_hwce_w_base_addr w_base_addr;
    vp_hwce_job_config0 job_config0;
    vp_hwce_job_config1 job_config1;
    vp_hwce_y_trans_size y_trans_size_ctx0;
    vp_hwce_y_line_stride_length y_line_stride_length_ctx0;
    vp_hwce_y_feat_stride_length y_feat_stride_length_ctx0;
    vp_hwce_y_out_3_base_addr y_out_3_base_addr_ctx0;
    vp_hwce_y_out_2_base_addr y_out_2_base_addr_ctx0;
    vp_hwce_y_out_1_base_addr y_out_1_base_addr_ctx0;
    vp_hwce_y_out_0_base_addr y_out_0_base_addr_ctx0;
    vp_hwce_y_in_3_base_addr y_in_3_base_addr_ctx0;
    vp_hwce_y_in_2_base_addr y_in_2_base_addr_ctx0;
    vp_hwce_y_in_1_base_addr y_in_1_base_addr_ctx0;
    vp_hwce_y_in_0_base_addr y_in_0_base_addr_ctx0;
    vp_hwce_x_trans_size x_trans_size_ctx0;
    vp_hwce_x_line_stride_length x_line_stride_length_ctx0;
    vp_hwce_x_feat_stride_length x_feat_stride_length_ctx0;
    vp_hwce_x_in_base_addr x_in_base_addr_ctx0;
    vp_hwce_w_base_addr w_base_addr_ctx0;
    vp_hwce_job_config0 job_config0_ctx0;
    vp_hwce_job_config1 job_config1_ctx0;
    vp_hwce_y_trans_size y_trans_size_ctx1;
    vp_hwce_y_line_stride_length y_line_stride_length_ctx1;
    vp_hwce_y_feat_stride_length y_feat_stride_length_ctx1;
    vp_hwce_y_out_3_base_addr y_out_3_base_addr_ctx1;
    vp_hwce_y_out_2_base_addr y_out_2_base_addr_ctx1;
    vp_hwce_y_out_1_base_addr y_out_1_base_addr_ctx1;
    vp_hwce_y_out_0_base_addr y_out_0_base_addr_ctx1;
    vp_hwce_y_in_3_base_addr y_in_3_base_addr_ctx1;
    vp_hwce_y_in_2_base_addr y_in_2_base_addr_ctx1;
    vp_hwce_y_in_1_base_addr y_in_1_base_addr_ctx1;
    vp_hwce_y_in_0_base_addr y_in_0_base_addr_ctx1;
    vp_hwce_x_trans_size x_trans_size_ctx1;
    vp_hwce_x_line_stride_length x_line_stride_length_ctx1;
    vp_hwce_x_feat_stride_length x_feat_stride_length_ctx1;
    vp_hwce_x_in_base_addr x_in_base_addr_ctx1;
    vp_hwce_w_base_addr w_base_addr_ctx1;
    vp_hwce_job_config0 job_config0_ctx1;
    vp_hwce_job_config1 job_config1_ctx1;
    vp_regmap_hwce()
    {
        this->registers.push_back(&this->trigger);
        this->registers.push_back(&this->acquire);
        this->registers.push_back(&this->finished_jobs);
        this->registers.push_back(&this->status);
        this->registers.push_back(&this->running_job);
        this->registers.push_back(&this->soft_clear);
        this->registers.push_back(&this->gen_config0);
        this->registers.push_back(&this->gen_config1);
        this->registers.push_back(&this->y_trans_size);
        this->registers.push_back(&this->y_line_stride_length);
        this->registers.push_back(&this->y_feat_stride_length);
        this->registers.push_back(&this->y_out_3_base_addr);
        this->registers.push_back(&this->y_out_2_base_addr);
        this->registers.push_back(&this->y_out_1_base_addr);
        this->registers.push_back(&this->y_out_0_base_addr);
        this->registers.push_back(&this->y_in_3_base_addr);
        this->registers.push_back(&this->y_in_2_base_addr);
        this->registers.push_back(&this->y_in_1_base_addr);
        this->registers.push_back(&this->y_in_0_base_addr);
        this->registers.push_back(&this->x_trans_size);
        this->registers.push_back(&this->x_line_stride_length);
        this->registers.push_back(&this->x_feat_stride_length);
        this->registers.push_back(&this->x_in_base_addr);
        this->registers.push_back(&this->w_base_addr);
        this->registers.push_back(&this->job_config0);
        this->registers.push_back(&this->job_config1);
        this->registers.push_back(&this->y_trans_size_ctx0);
        this->registers.push_back(&this->y_line_stride_length_ctx0);
        this->registers.push_back(&this->y_feat_stride_length_ctx0);
        this->registers.push_back(&this->y_out_3_base_addr_ctx0);
        this->registers.push_back(&this->y_out_2_base_addr_ctx0);
        this->registers.push_back(&this->y_out_1_base_addr_ctx0);
        this->registers.push_back(&this->y_out_0_base_addr_ctx0);
        this->registers.push_back(&this->y_in_3_base_addr_ctx0);
        this->registers.push_back(&this->y_in_2_base_addr_ctx0);
        this->registers.push_back(&this->y_in_1_base_addr_ctx0);
        this->registers.push_back(&this->y_in_0_base_addr_ctx0);
        this->registers.push_back(&this->x_trans_size_ctx0);
        this->registers.push_back(&this->x_line_stride_length_ctx0);
        this->registers.push_back(&this->x_feat_stride_length_ctx0);
        this->registers.push_back(&this->x_in_base_addr_ctx0);
        this->registers.push_back(&this->w_base_addr_ctx0);
        this->registers.push_back(&this->job_config0_ctx0);
        this->registers.push_back(&this->job_config1_ctx0);
        this->registers.push_back(&this->y_trans_size_ctx1);
        this->registers.push_back(&this->y_line_stride_length_ctx1);
        this->registers.push_back(&this->y_feat_stride_length_ctx1);
        this->registers.push_back(&this->y_out_3_base_addr_ctx1);
        this->registers.push_back(&this->y_out_2_base_addr_ctx1);
        this->registers.push_back(&this->y_out_1_base_addr_ctx1);
        this->registers.push_back(&this->y_out_0_base_addr_ctx1);
        this->registers.push_back(&this->y_in_3_base_addr_ctx1);
        this->registers.push_back(&this->y_in_2_base_addr_ctx1);
        this->registers.push_back(&this->y_in_1_base_addr_ctx1);
        this->registers.push_back(&this->y_in_0_base_addr_ctx1);
        this->registers.push_back(&this->x_trans_size_ctx1);
        this->registers.push_back(&this->x_line_stride_length_ctx1);
        this->registers.push_back(&this->x_feat_stride_length_ctx1);
        this->registers.push_back(&this->x_in_base_addr_ctx1);
        this->registers.push_back(&this->w_base_addr_ctx1);
        this->registers.push_back(&this->job_config0_ctx1);
        this->registers.push_back(&this->job_config1_ctx1);
    }
};

#endif

#endif
