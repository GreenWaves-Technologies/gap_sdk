/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */


#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include "archi/gvsoc/gvsoc.h"
#include "iss.hpp"
#include "gap9_iss_wrapper.hpp"

gap9_iss_wrapper::gap9_iss_wrapper(js::config *config)
: iss_wrapper(config)
{
}


void gap9_iss_wrapper::target_open()
{
    iss_decoder_item_t *isa = iss_isa_get(this, "f");
    int core_id = this->cpu.config.mhartid;

    for (int i=0; i<iss_get_isa_set()->nb_resources; i++)
    {
        iss_resource_t *resource = &iss_get_isa_set()->resources[i];
        int instance_id = core_id % resource->nb_instances;
        this->cpu.resources[i] = resource->instances[instance_id];
    }

    this->declare_pcer(CSR_PCER_CYCLES, "cycles", "Count the number of cycles the core was running");
    this->declare_pcer(CSR_PCER_INSTR, "instr", "Count the number of instructions executed");
    this->declare_pcer(CSR_PCER_LD_STALL, "ld_stall", "Number of load use hazards");
    this->declare_pcer(CSR_PCER_JMP_STALL, "jmp_stall", "Number of jump register hazards");
    this->declare_pcer(CSR_PCER_IMISS, "imiss", "Cycles waiting for instruction fetches. i.e. the number of instructions wasted due to non-ideal caches");
    this->declare_pcer(CSR_PCER_LD, "ld", "Number of memory loads executed. Misaligned accesses are counted twice");
    this->declare_pcer(CSR_PCER_ST, "st", "Number of memory stores executed. Misaligned accesses are counted twice");
    this->declare_pcer(CSR_PCER_JUMP, "jump", "Number of jump instructions seen, i.e. j, jr, jal, jalr");
    this->declare_pcer(CSR_PCER_BRANCH, "branch", "Number of branch instructions seen, i.e. bf, bnf");
    this->declare_pcer(CSR_PCER_TAKEN_BRANCH, "taken_branch", "Number of taken branch instructions seen, i.e. bf, bnf");
    this->declare_pcer(CSR_PCER_RVC, "rvc", "Number of compressed instructions");
    this->declare_pcer(CSR_PCER_ELW, "elw", "Cycles wasted due to ELW instruction");
    this->declare_pcer(CSR_PCER_LD_EXT, "ld_ext", "Number of memory loads to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_ST_EXT, "st_ext", "Number of memory stores to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_LD_EXT_CYC, "ld_ext_cycles", "Cycles used for memory loads to EXT. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_ST_EXT_CYC, "st_ext_cycles", "Cycles used for memory stores to EXT. Every non-TCDM access is considered external");
    this->declare_pcer(CSR_PCER_TCDM_CONT, "tcdm_cont", "Cycles wasted due to TCDM/log-interconnect contention");
    this->declare_pcer(CSR_PCER_MISALIGNED, "misaligned", "Cycles wasted due to misaligned accesses");
    this->declare_pcer(CSR_PCER_INSN_CONT, "insn_cont", "Cycles wasted due to instruction contentions on shared units");

    traces.new_trace_event("pcer_cycles", &pcer_trace_event[0], 1);
    traces.new_trace_event("pcer_instr", &pcer_trace_event[1], 1);
    traces.new_trace_event("pcer_ld_stall", &pcer_trace_event[2], 1);
    traces.new_trace_event("pcer_jmp_stall", &pcer_trace_event[3], 1);
    traces.new_trace_event("pcer_imiss", &pcer_trace_event[4], 1);
    traces.new_trace_event("pcer_ld", &pcer_trace_event[5], 1);
    traces.new_trace_event("pcer_st", &pcer_trace_event[6], 1);
    traces.new_trace_event("pcer_jump", &pcer_trace_event[7], 1);
    traces.new_trace_event("pcer_branch", &pcer_trace_event[8], 1);
    traces.new_trace_event("pcer_taken_branch", &pcer_trace_event[9], 1);
    traces.new_trace_event("pcer_rvc", &pcer_trace_event[10], 1);
    traces.new_trace_event("elw", &pcer_trace_event[11], 1);
    traces.new_trace_event("pcer_ld_ext", &pcer_trace_event[12], 1);
    traces.new_trace_event("pcer_st_ext", &pcer_trace_event[13], 1);
    traces.new_trace_event("pcer_ld_ext_cycles", &pcer_trace_event[14], 1);
    traces.new_trace_event("pcer_st_ext_cycles", &pcer_trace_event[15], 1);
    traces.new_trace_event("pcer_tcdm_cont", &pcer_trace_event[16], 1);

    traces.new_trace_event("pcer_misaligned", &pcer_trace_event[29], 1);
    traces.new_trace_event("pcer_insn_cont", &pcer_trace_event[30], 1);
}


extern "C" vp::component *vp_constructor(js::config *config)
{
  return new gap9_iss_wrapper(config);
}
