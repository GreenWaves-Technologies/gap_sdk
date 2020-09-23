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

#ifndef __CPU_ISS_RNNEXT_HPP
#define __CPU_ISS_RNNEXT_HPP

static inline void pl_sdotsp_h_0_load_resume(iss_t *iss)
{
  iss_insn_t *insn = iss->cpu.rnnext.sdot_insn;
  iss_csr_msg(iss, "Loaded new sdot_prefetch_0 value (value: 0x%x)\n", iss->cpu.rnnext.sdot_prefetch_0);
}

static inline iss_insn_t *pl_sdotsp_h_0_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t addr = REG_GET(0);
  IN_REG_SET(0, addr + 4);

  REG_SET(0, LIB_CALL3(lib_VEC_SDOTSP_16, REG_GET(2), iss->cpu.rnnext.sdot_prefetch_0, REG_GET(1)));

  if (!iss->data_req(addr, (uint8_t *)&iss->cpu.rnnext.sdot_prefetch_0, 4, false))
  {
    iss_csr_msg(iss, "Loaded new sdot_prefetch_0 value (value: 0x%x)\n", iss->cpu.rnnext.sdot_prefetch_0);
  }
  else
  {
    iss->cpu.state.stall_callback = pl_sdotsp_h_0_load_resume;
    iss->cpu.rnnext.sdot_insn = insn;
    iss_exec_insn_stall(iss);
  }



  return insn->next;
}



static inline void pl_sdotsp_h_1_load_resume(iss_t *iss)
{
  iss_insn_t *insn = iss->cpu.rnnext.sdot_insn;
  iss_csr_msg(iss, "Loaded new sdot_prefetch_1 value (value: 0x%x)\n", iss->cpu.rnnext.sdot_prefetch_1);
}

static inline iss_insn_t *pl_sdotsp_h_1_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_reg_t addr = REG_GET(0);
  IN_REG_SET(0, addr + 4);

  REG_SET(0, LIB_CALL3(lib_VEC_SDOTSP_16, REG_GET(2), iss->cpu.rnnext.sdot_prefetch_1, REG_GET(1)));

  if (!iss->data_req(addr, (uint8_t *)&iss->cpu.rnnext.sdot_prefetch_1, 4, false))
  {
    iss_csr_msg(iss, "Loaded new sdot_prefetch_1 value (value: 0x%x)\n", iss->cpu.rnnext.sdot_prefetch_1);
  }
  else
  {
    iss->cpu.state.stall_callback = pl_sdotsp_h_1_load_resume;
    iss->cpu.rnnext.sdot_insn = insn;
    iss_exec_insn_stall(iss);
  }



  return insn->next;
}



static inline iss_insn_t *pl_tanh_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_TANH, REG_GET(0)));
  return insn->next;
}



static inline iss_insn_t *pl_sig_exec(iss_t *iss, iss_insn_t *insn)
{
  REG_SET(0, LIB_CALL1(lib_SIG, REG_GET(0)));
  return insn->next;
}



#endif
