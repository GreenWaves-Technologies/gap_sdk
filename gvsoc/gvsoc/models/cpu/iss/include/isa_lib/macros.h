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

#ifndef __ISA_LIB_MACROS_HPP
#define __ISA_LIB_MACROS_HPP

#define LIB_CALL1(name, s0) name(&iss->cpu.state, s0)
#define LIB_CALL2(name, s0, s1) name(&iss->cpu.state, s0, s1)
#define LIB_CALL3(name, s0, s1, s2) name(&iss->cpu.state, s0, s1, s2)
#define LIB_CALL4(name, s0, s1, s2, s3) name(&iss->cpu.state, s0, s1, s2, s3)
#define LIB_CALL5(name, s0, s1, s2, s3, s4) name(&iss->cpu.state, s0, s1, s2, s3, s4)
#define LIB_CALL6(name, s0, s1, s2, s3, s4, s5) name(&iss->cpu.state, s0, s1, s2, s3, s4, s5)

#define LIB_FF_CALL1(name, s0, s1, s2) LIB_CALL3(name, s0, s1, s2)
#define LIB_FF_CALL2(name, s0, s1, s2, s3) LIB_CALL4(name, s0, s1, s2, s3)
#define LIB_FF_CALL3(name, s0, s1, s2, s3, s4) LIB_CALL5(name, s0, s1, s2, s3, s4)
#define LIB_FF_CALL4(name, s0, s1, s2, s3, s4, s5) LIB_CALL6(name, s0, s1, s2, s3, s4, s5)

#define REG_IN(reg) (insn->in_regs[reg])
#define REG_OUT(reg) (insn->out_regs[reg])

#define REG_GET(reg) iss_get_reg(iss, insn->in_regs[reg])
#define REG_SET(reg,val) iss_set_reg(iss, insn->out_regs[reg], val)
#define IN_REG_SET(reg,val) iss_set_reg(iss, insn->in_regs[reg], val)

#define REG64_GET(reg) iss_get_reg64(iss, insn->in_regs[reg])
#define REG64_SET(reg,val) iss_set_reg64(iss, insn->out_regs[reg], val)

#define SIM_GET(index) insn->sim[index]
#define UIM_GET(index) insn->uim[index]

#define SPR_SET(reg,val) iss_set_spec_purp_reg(iss, reg, val)
#define SPR_GET(reg) iss_get_spec_purp_reg(iss, reg)

#endif
