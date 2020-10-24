/*
 * Copyright (C) 2019 GreenWaves Technologies
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


#ifndef __ARCHI_GAP_UTILS_H__
#define __ARCHI_GAP_UTILS_H__

static inline unsigned int __attribute__ ((always_inline)) ExtInsMaskFast_archi(unsigned int Size, unsigned int Offset) { return ((((Size-1))<<5)|(Offset)); }
static inline unsigned int __attribute__ ((always_inline)) ExtInsMaskSafe_archi(unsigned int Size, unsigned int Offset) { return ((((Size-1)&0x1F)<<5)|(Offset&0x1F)); }

#if defined(__riscv__) && !defined(__LLVM__) && !defined(RV_ISA_RV32)
#define GAP_WRITE_VOL(base, offset, value) __builtin_pulp_write_base_off_v((value), (base), (offset))
#define GAP_WRITE(base, offset, value)     __builtin_pulp_OffsetedWrite((value), (int *)(base), (offset))
#define GAP_READ(base, offset)             __builtin_pulp_OffsetedRead((int *)(base), (offset))
#else
#define GAP_WRITE_VOL(base, offset, value) archi_write32((base) + (offset), (value))
#define GAP_WRITE(base, offset, value)     archi_write32((base) + (offset), (value))
#define GAP_READ(base, offset)             archi_read32((base) + (offset))
#endif

#define GAP_BINSERT(dst,src,size,off)  __builtin_pulp_binsert((dst), ~(((1UL<<(size))-1)<<(off)), (src), (((1UL<<(size))-1)<<(off)), (off))
#define GAP_BINSERT_R(dst,src,size,off)  __builtin_pulp_binsert_r((dst), (src), ExtInsMaskFast_archi((size), (off)))
#define GAP_BEXTRACTU(src,size,off)    __builtin_pulp_bextractu((src), (size), (off))
#define GAP_BEXTRACT(src,size,off)     __builtin_pulp_bextract((src), (size), (off))

#endif