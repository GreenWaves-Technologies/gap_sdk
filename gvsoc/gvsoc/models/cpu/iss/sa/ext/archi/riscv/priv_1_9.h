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

#ifndef _ARCHI_RISCV_PRIV_1_9_H
#define _ARCHI_RISCV_PRIV_1_9_H

#define CSR_MSTATUS  0x300
#define CSR_MEPC     0x341
#define CSR_MCAUSE   0x342
#define CSR_MTVAL    0x343
#define CSR_MESTATUS 0x7C0
#ifdef RISCV_1_7
#define CSR_MCPUID   0xF00
#define CSR_MIMPID   0xF01
#define CSR_MHARTID  0xF10
#else
#define CSR_MISA     0xF10
#define CSR_MIMPID   0xF13
#define CSR_MHARTID  0xF14
#endif

#define CSR_PCCR(N)	(0x780 + (N))
#define CSR_PCER	0x7A0
#define CSR_PCMR	0x7A1

#define CSR_MESTATUS_INTEN_BIT 0
#define CSR_MESTATUS_PRV_BIT   1

#define CSR_MESTATUS_PRV_MACH  3

#endif
