
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

#ifndef __ARCHI_CL_DMA_GVSOC__
#define __ARCHI_CL_DMA_GVSOC__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

class vp_cl_dma_cmd : public vp::reg_32
{
public:
    inline void cmd_set(uint32_t value) { this->set_field(value, CL_DMA_CMD_CMD_BIT, CL_DMA_CMD_CMD_WIDTH); }
    inline uint32_t cmd_get() { return this->get_field(CL_DMA_CMD_CMD_BIT, CL_DMA_CMD_CMD_WIDTH); }
    vp_cl_dma_cmd()
    {
        this->hw_name = "CMD";
        this->offset = 0x0;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("CMD", 0, 32));
    }
};

class vp_cl_dma_status : public vp::reg_32
{
public:
    inline void status_set(uint32_t value) { this->set_field(value, CL_DMA_STATUS_STATUS_BIT, CL_DMA_STATUS_STATUS_WIDTH); }
    inline uint32_t status_get() { return this->get_field(CL_DMA_STATUS_STATUS_BIT, CL_DMA_STATUS_STATUS_WIDTH); }
    vp_cl_dma_status()
    {
        this->hw_name = "STATUS";
        this->offset = 0x4;
        this->width = 32;
        this->do_reset = 1;
        this->reset_val = 0x0;
        this->regfields.push_back(new vp::regfield("STATUS", 0, 32));
    }
};


class vp_regmap_cl_dma : public vp::regmap
{
public:
    vp_cl_dma_cmd cmd;
    vp_cl_dma_status status;
    vp_regmap_cl_dma()
    {
        this->registers.push_back(&this->cmd);
        this->registers.push_back(&this->status);
    }
};

#endif

#endif
