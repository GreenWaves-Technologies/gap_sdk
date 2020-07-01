/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

#endif
