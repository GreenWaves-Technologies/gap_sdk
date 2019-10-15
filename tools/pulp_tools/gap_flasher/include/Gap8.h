#ifndef __GAP8_H__
#define __GAP8_H__

#ifdef __EMUL__
#include "gap8_emul.h"
#else

#if defined(__MBED__)
#include "gap_autotiler.h"
#else
#include "pulp.h"
#include "rt/rt_api.h"
#endif

/* START_NEW_RUNTIME
#ifndef __HAL_BUILTINS_V2_H__
#define __HAL_BUILTINS_V2_H__
#ifdef ARCHI_CORE_HAS_PULPV2
STOP_NEW_RUNTIME */

/* START_NEW_RUNTIME */
/* ARITHMETIC SECTION */
typedef   signed short v2s __attribute__((vector_size (4)));
typedef unsigned short v2u __attribute__((vector_size (4)));

typedef   signed char  v4s __attribute__((vector_size (4)));
typedef unsigned char  v4u __attribute__((vector_size (4)));

typedef unsigned int rt_pointerT;
/* STOP_NEW_RUNTIME */


/* Operations */

/* Static flags  to track viterbi max */
static int _VitT0_Flag, _VitT1_Flag;

#define DMA_COPY_IN 1
#define DMA_COPY_OUT 0

#if defined(__pulp__) || defined(__MBED__)

#if defined (__MBED__)
#define L2_MEM      GAP_L2_DATA
#define L1_CL_MEM   GAP_L1_GLOBAL_DATA
#define L1_FC_MEM   GAP_FC_GLOBAL_DATA
#else
/* Here we compile for Gap target */
#define L2_MEM      RT_L2_DATA
#define L1_CL_MEM   RT_L1_DATA
#define L1_FC_MEM   RT_FC_GLOBAL_DATA
#endif

static inline unsigned int __attribute__ ((always_inline)) ExtInsMaskFast(unsigned int Size, unsigned int Offset) { return ((((Size-1))<<5)|(Offset)); }
static inline unsigned int __attribute__ ((always_inline)) ExtInsMaskSafe(unsigned int Size, unsigned int Offset) { return ((((Size-1)&0x1F)<<5)|(Offset&0x1F)); }

/* START_DEF_BTIN */
/* Packing of scalars into vectors */
#define gap8_pack2(x, y)		__builtin_pulp_pack2((signed short)   (x), (signed short)   (y)) /*v2h,signed short,signed short*/
#define gap8_packu2(x, y)		__builtin_pulp_pack2((unsigned short) (x), (unsigned short) (y)) /*v2u,unsigned short,unsigned short*/

#define gap8_pack4(x, y, z, t)		__builtin_pulp_pack4((signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t)) /*v4s,signed char,signed char,signed char,signed char*/
#define gap8_packu4(x, y, z, t)		__builtin_pulp_pack4((unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t)) /*v4u,unsigned char,unsigned char,unsigned char,unsigned char*/

/* Max */
#define gap8_max2(x, y) 		__builtin_pulp_max2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_max4(x, y) 		__builtin_pulp_max4((x), (y)) /*v4s,v4s,v4s*/

#define gap8_maxu2(x, y) 		__builtin_pulp_maxu2((x), (y)) /*v2u,v2s,v2s*/
#define gap8_maxu4(x, y) 		__builtin_pulp_maxu4((x), (y)) /*v4u,v4s,v4s*/

/* Min */
#define gap8_min2(x, y) 		__builtin_pulp_min2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_min4(x, y) 		__builtin_pulp_min4((x), (y)) /*v4s,v4s,v4s*/

#define gap8_minu2(x, y) 		__builtin_pulp_minu2((x), (y)) /*v2u,v2s,v2s*/
#define gap8_minu4(x, y) 		__builtin_pulp_minu4((x), (y)) /*v4u,v4s,v4s*/

/* Clip */
#define gap8_clip(x, precision) 	__builtin_pulp_clip((x), -(1<<(precision)), (1<<precision)-1) /*int,int,int*/
#define gap8_clip_r(x, bound) 		__builtin_pulp_clip_r((x), (bound)) /*int,int,int*/

#define gap8_clipu(x, precision) 	__builtin_pulp_clipu((x), 0, (1<<precision)-1) /*unsigned int,int,int*/
#define gap8_clipu_r(x, bound) 		__builtin_pulp_clipu_r((x), (bound)) /*unsigned int,int,int*/

/* Abs */
#define gap8_abs2(x) 			__builtin_pulp_abs2((x)) /*v2u,v2s*/
#define gap8_abs4(x) 			__builtin_pulp_abs4((x)) /*v4u,v4s*/

/* Unary minus */
#define gap8_neg2(x) 			__builtin_pulp_neg2((x)) /*v2s,v2s*/
#define gap8_neg4(x) 			__builtin_pulp_neg4((x)) /*v4s,v4s*/

/* Addition */
#define gap8_add2(x, y) 		__builtin_pulp_add2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_add4(x, y) 		__builtin_pulp_add4((x), (y)) /*v4s,v4s,v4s*/

/* Substraction */
#define gap8_sub2(x, y) 		__builtin_pulp_sub2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_sub4(x, y) 		__builtin_pulp_sub4((x), (y)) /*v4s,v4s,v4s*/

/* Average */
#define gap8_avg2(x, y) 		__builtin_pulp_avg2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_avg4(x, y) 		__builtin_pulp_avg4((x), (y)) /*v4s,v4s,v4s*/

/* Average unsigned */
#define gap8_avgu2(x, y) 		__builtin_pulp_avgu2((x), (y)) /*v2u,v2s,v2s*/
#define gap8_avgu4(x, y) 		__builtin_pulp_avgu4((x), (y)) /*v4u,v4s,v4s*/

/* Bitwise and */
#define gap8_and2(x, y) 		__builtin_pulp_and2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_and4(x, y) 		__builtin_pulp_and4((x), (y)) /*v4s,v4s,v4s*/

/* Bitwise or */
#define gap8_or2(x, y) 			__builtin_pulp_or2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_or4(x, y) 			__builtin_pulp_or4((x), (y)) /*v4s,v4s,v4s*/

/* Bitwise exor */
#define gap8_exor2(x, y) 		__builtin_pulp_exor2(x, y) /*v2s,v2s,v2s*/
#define gap8_exor4(x, y) 		__builtin_pulp_exor4(x, y) /*v4s,v4s,v4s*/

/* Logical shift right */
#define gap8_srl2(x, y) 		__builtin_pulp_srl2(x, y) /*v2u,v2s,v2s*/
#define gap8_srl4(x, y) 		__builtin_pulp_srl4(x, y) /*v4u,v4s,v4s*/

/* Arithmetic shift right */
#define gap8_sra2(x, y) 		__builtin_pulp_sra2(x, y) /*v2s,v2s,v2s*/
#define gap8_sra4(x, y) 		__builtin_pulp_sra4(x, y) /*v4s,v4s,v4s*/

/* Logical shift left */
#define gap8_sll2(x, y) 		__builtin_pulp_sll2(x, y) /*v2s,v2s,v2s*/
#define gap8_sll4(x, y) 		__builtin_pulp_sll4(x, y) /*v4s,v4s,v4s*/

/* Mac */
#define	gap8_mac(Acc, x, y)		__builtin_pulp_mac((x), (y), (Acc)) /*int,int,int,int*/
#define	gap8_msu(Acc, x, y)		__builtin_pulp_msu((x), (y), (Acc)) /*int,int,int,int*/

#define	gap8_macs(Acc, x, y)		__builtin_pulp_macs((x), (y), (Acc)) /*int,int,int,int*/
#define	gap8_machhs(Acc, x, y)		__builtin_pulp_machhs((x), (y), (Acc)) /*int,int,int,int*/
#define	gap8_macu(Acc, x, y)		__builtin_pulp_macu((x), (y), (Acc)) /*unsigned int,unsigned int,int,int*/
#define	gap8_machhu(Acc, x, y)		__builtin_pulp_machhu((x), (y), (Acc)) /*unsigned int,unsigned int,int,int*/

#define gap8_macsN(Acc, x, y, n)	__builtin_pulp_macsN((x), (y), (Acc), (n)) /*int,int,int,int,int*/
#define gap8_macuN(Acc, x, y, n)	__builtin_pulp_macuN((x), (y), (Acc), (n)) /*unsigned int,unsigned int,int,int,int*/
#define gap8_macsRN(Acc, x, y, n)	__builtin_pulp_macsRN((x), (y), (Acc), (n), (1<<((n)-1))) /*int,int,int,int,int*/
#define gap8_macuRN(Acc, x, y, n)	__builtin_pulp_macuRN((x), (y), (Acc), (n), (1<<((n)-1))) /*unsigned int,unsigned int,int,int,int*/

#define gap8_machhsN(Acc, x, y, n)	__builtin_pulp_machhsN((x), (y), (Acc), (n)) /*int,int,int,int,int*/
#define gap8_machhuN(Acc, x, y, n)	__builtin_pulp_machhuN((x), (y), (Acc), (n)) /*unsigned int,unsigned int,int,int,int*/
#define gap8_machhsRN(Acc, x, y, n)	__builtin_pulp_machhsN((x), (y), (Acc), (n), (1<<((n)-1))) /*int,int,int,int,int*/
#define gap8_machhuRN(Acc, x, y, n)	__builtin_pulp_machhuN((x), (y), (Acc), (n), (1<<((n)-1))) /*unsigned int,unsigned int,int,int,int*/

/* Multiplications */
#define gap8_muls(x, y)			__builtin_pulp_muls((x), (y)) /*int,int,int*/
#define gap8_mulu(x, y)			__builtin_pulp_mulu((x), (y)) /*unsigned int,int,int*/
#define gap8_mulhhs(x, y)		__builtin_pulp_mulhhs((x), (y)) /*int,int,int*/
#define gap8_mulhhu(x, y)		__builtin_pulp_mulhhu((x), (y)) /*unsigned int,int,int*/

#define gap8_mulsN(x, y, n)		__builtin_pulp_mulsN((x), (y), (n)) /*int,int,int,int*/
#define gap8_mulsRN(x, y, n)		__builtin_pulp_mulsRN((x), (y), (n), (1<<((n)-1))) /*int,int,int,int*/
#define gap8_muluN(x, y, n)		__builtin_pulp_muluN((x), (y), (n)) /*unsigned int,int,int,int*/
#define gap8_muluRN(x, y, n)		__builtin_pulp_muluRN((x), (y), (n), (1<<((n)-1))) /*unsigned int,int,int,int*/

#define gap8_mulhhsN(x, y, n)		__builtin_pulp_mulhhsN((x), (y), (n)) /*int,int,int,int*/
#define gap8_mulhhsRN(x, y, n)		__builtin_pulp_mulhhsRN((x), (y), (n), (1<<((n)-1))) /*int,int,int,int*/
#define gap8_mulhhuN(x, y, n)		__builtin_pulp_mulhhuN((x), (y), (n)) /*unsigned int,int,int,int*/
#define gap8_mulhhuRN(x, y, n)		__builtin_pulp_mulhhuRN((x), (y), (n), (1<<((n)-1))) /*unsigned int,int,int,int*/

/* Vectorial product and sum of products */
#define gap8_dotp2(x, y)     		__builtin_pulp_dotsp2((x), (y)) /*int,v2s,v2s*/
#define gap8_dotpu2(x, y)     		__builtin_pulp_dotup2((x), (y)) /*unsigned int,v2s,v2s*/
#define gap8_dotpus2(x, y)    		__builtin_pulp_dotusp2((x), (y)) /*int,v2s,v2s*/

#define gap8_dotpsc2(x, y)     		__builtin_pulp_dotspsc2((x), (y)) /*int,v2s,int*/
#define gap8_dotpusc2(x, y)     	__builtin_pulp_dotupsc2((x), (y)) /*unsigned int,v2s,int*/
#define gap8_dotpussc2(x, y)    	__builtin_pulp_dotuspsc2((x), (y)) /*int,v2s,int*/

#define gap8_sumdotp2(x, y, z)		__builtin_pulp_sdotsp2((x), (y), (z)) /*int,v2s,v2s,int*/
#define gap8_sumdotpu2(x, y, z)		__builtin_pulp_sdotup2((x), (y), (z)) /*unsigned int,v2s,v2s,unsigned int*/
#define gap8_sumdotpus2(x, y, z)	__builtin_pulp_sdotusp2((x), (y), (z)) /*int,v2s,v2s,int*/

#define gap8_sumdotpsc2(x, y, z)	__builtin_pulp_sdotspsc2((x), (y), (z)) /*int,v2s,int,int*/
#define gap8_sumdotpusc2(x, y, z)	__builtin_pulp_sdotupsc2((x), (y), (z)) /*unsigned int,v2s,int,unsigned int*/
#define gap8_sumdotpussc2(x, y, z)	__builtin_pulp_sdotuspsc2((x), (y), (z)) /*int,v2s,int,int*/

#define gap8_dotp4(x, y)     		__builtin_pulp_dotsp4((x), (y)) /*int,v4s,v4s*/
#define gap8_dotpu4(x, y)     		__builtin_pulp_dotup4((x), (y)) /*unsigned int,v4s,v4s*/
#define gap8_dotpus4(x, y)    		__builtin_pulp_dotusp4((x), (y)) /*int,v4s,v4s*/

#define gap8_dotpsc4(x, y)     		__builtin_pulp_dotspsc4((x), (y)) /*int,v4s,int*/
#define gap8_dotpusc4(x, y)     	__builtin_pulp_dotupsc4((x), (y)) /*unsigned int,v4s,int*/
#define gap8_dotpussc4(x, y)    	__builtin_pulp_dotuspsc4((x), (y)) /*int,v4s,int*/

#define gap8_sumdotp4(x, y, z)     	__builtin_pulp_sdotsp4((x), (y), (z)) /*int,v4s,v4s,int*/
#define gap8_sumdotpu4(x, y, z)     	__builtin_pulp_sdotup4((x), (y), (z)) /*unsigned int,v4s,v4s,unsigned int*/
#define gap8_sumdotpus4(x, y, z)    	__builtin_pulp_sdotusp4((x), (y), (z)) /*int,v4s,v4s,int*/

#define gap8_sumdotpsc4(x, y, z)     	__builtin_pulp_sdotspsc4((x), (y), (z)) /*int,v4s,int,int*/
#define gap8_sumdotpusc4(x, y, z)     	__builtin_pulp_sdotupsc4((x), (y), (z)) /*unsigned int,v4s,int,unsigned int*/
#define gap8_sumdotpussc4(x, y, z)    	__builtin_pulp_sdotuspsc4((x), (y), (z)) /*int,v4s,int,int*/


/* Complex Multiplication, Q15x15 into Q15, with optional post scaling by 1 or 2 */
#define gap8_cplxmuls(x, y)		__builtin_pulp_cplxmuls((x), (y)) /*v2s,v2s,v2s*/
#define gap8_cplxmulsdiv2(x, y)		__builtin_pulp_cplxmulsdiv2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_cplxmulsdiv4(x, y)		__builtin_pulp_cplxmulsdiv4((x), (y)) /*v2s,v2s,v2s*/

/* Complex conjugate */
#define gap8_cplxconj(x)		__builtin_pulp_cplx_conj((x)) /*v2s,v2s*/

/* Complex substration, result rotated by -pi/2 */
#define gap8_sub2rotmj(x, y)		__builtin_pulp_sub2rotmj((x), (y)) /*v2s,v2s,v2s*/

/* Complex addition with post scaling by 1 or 2 */
#define gap8_add2div2(x, y)		__builtin_pulp_add2div2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_add2div4(x, y)		__builtin_pulp_add2div4((x), (y)) /*v2s,v2s,v2s*/

#define gap8_add4div2(x, y)		__builtin_pulp_add4div2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_add4div4(x, y)		__builtin_pulp_add4div4((x), (y)) /*v2s,v2s,v2s*/

/* Complex subtraction with post scaling by 1 or 2 */
#define gap8_sub2div2(x, y)		__builtin_pulp_sub2div2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_sub2div4(x, y)		__builtin_pulp_sub2div4((x), (y)) /*v2s,v2s,v2s*/

#define gap8_sub4div2(x, y)		__builtin_pulp_sub4div2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_sub4div4(x, y)		__builtin_pulp_sub4div4((x), (y)) /*v2s,v2s,v2s*/

/* Viterbi Max and Viterbi Select, pair of Q15 */
#define gap8_vitmax(x, y) 		__builtin_pulp_vitmax2((x), (y)) /*v2s,v2s,v2s*/
#define gap8_vitsel(x, y) 		__builtin_pulp_vitsel2((x), (y)) /*v2s,v2s,v2s*/

/* Position of the most significant bit of x */
#define gap8_fl1(x)			__builtin_pulp_fl1((x)) /*int,int*/

/* Number of sign bits */
#define gap8_clb(x)			__builtin_pulp_clb((x)) /*int,int*/

/* Bit set */
#define gap8_bitset(x, size, off)               __builtin_pulp_bset((x), (((1<<(size))-1)<<(off))) /*int,int,int,int*/
#define gap8_bitset_r(x, size, off)             __builtin_pulp_bset_r((x), ExtInsMaskFast((size), (off))) /*int,int,int,int*/
#define gap8_bitset_r_safe(x, size, off)        __builtin_pulp_bset_r((x), ExtInsMaskSafe((size), (off))) /*int,int,int,int*/

/* Bit clr */
#define gap8_bitclr(x, size, off)               __builtin_pulp_bclr((x), ~(((1<<(size))-1)<<(off))) /*int,int,int,int*/
#define gap8_bitclr_r(x, size, off)             __builtin_pulp_bclr_r((x), ExtInsMaskFast((size), (off))) /*int,int,int,int*/
#define gap8_bitclr_r_safe(x, size, off)        __builtin_pulp_bclr_r((x), ExtInsMaskSafe((size), (off))) /*int,int,int,int*/

/* Bit Extraction */
#define gap8_bitextract(x, size, off)		__builtin_pulp_bextract((x), (size), (off)) /*int,int,int,int*/
#define gap8_bitextractu(x, size, off)		__builtin_pulp_bextractu((x), (size), (off)) /*unsigned int,int,int,int*/

#define gap8_bitextract_r(x, size, off)		__builtin_pulp_bextract_r((x), ExtInsMaskFast((size), (off))) /*int,int,int,int*/
#define gap8_bitextractu_r(x, size, off)	__builtin_pulp_bextractu_r((x), ExtInsMaskFast((size), (off))) /*unsigned int,int,int,int*/

#define gap8_bitextract_r_safe(x, size, off)	__builtin_pulp_bextract_r((x), ExtInsMaskSafe((size), (off))) /*int,int,int,int*/
#define gap8_bitextractu_r_safe(x, size, off)	__builtin_pulp_bextractu_r((x), ExtInsMaskSafe((size), (off))) /*unsigned int,int,int,int*/

/* Bit insertion */
#define gap8_bitinsert(dst, src, size, off) 	__builtin_pulp_binsert((dst), ~(((1<<(size))-1)<<(off)), (src), (((1<<(size))-1)<<(off)), (off)) /*int,int,int,int,int*/
#define gap8_bitinsert_r(dst, src, size, off) 	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskFast((size), (off))) /*int,int,int,int,int*/
#define gap8_bitinsert_r_safe(dst, src, size, off) 	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskSafe((size), (off))) /*int,int,int,int,int*/

/* 1 bit rotation to the right, 32 bits input */
#define gap8_rotr(x)			__builtin_pulp_rotr((x)) /*int,int*/

/* Add with normalization */
#define gap8_addnormu(x, y, scale)		__builtin_pulp_adduN((x), (y), (scale)) /*unsigned int,int,int,int*/
#define gap8_addnormu_reg(x, y, scale)		__builtin_pulp_adduN_r((x), (y), (scale)) /*unsigned int,int,int,int*/
#define gap8_addnorm(x, y, scale)		__builtin_pulp_addN((x), (y), (scale)) /*int,int,int,int*/
#define gap8_addnorm_reg(x, y, scale)		__builtin_pulp_addN_r((x), (y), (scale)) /*int,int,int,int*/

/* Add with normalization and rounding */
#define gap8_addroundnormu(x, y, scale)		__builtin_pulp_adduRN((x), (y), (scale), (1<<((scale)-1))) /*unsigned int,int,int,int*/
#define gap8_addroundnormu_reg(x, y, scale)	__builtin_pulp_adduRN_r((x), (y), (scale)) /*unsigned int,int,int,int*/
#define gap8_addroundnorm(x, y, scale)		__builtin_pulp_addRN((x), (y), (scale), (1<<((scale)-1))) /*int,int,int,int*/
#define gap8_addroundnorm_reg(x, y, scale)	__builtin_pulp_addRN_r((x), (y), (scale)) /*int,int,int,int*/

/* Sub with normalization */
#define gap8_subnormu(x, y, scale)		__builtin_pulp_subuN((x), (y), (scale)) /*unsigned int,int,int,int*/
#define gap8_subnormu_reg(x, y, scale)		__builtin_pulp_subuN_r((x), (y), (scale)) /*unsigned int,int,int,int*/
#define gap8_subnorm(x, y, scale)		__builtin_pulp_subN((x), (y), (scale)) /*int,int,int,int*/
#define gap8_subnorm_reg(x, y, scale)		__builtin_pulp_subN_r((x), (y), (scale)) /*int,int,int,int*/

/* Sub with normalization and rounding */
#define gap8_subroundnormu(x, y, scale)		__builtin_pulp_subuRN((x), (y), (scale), (1<<((scale)-1))) /*unsigned int,int,int,int*/
#define gap8_subroundnormu_reg(x, y, scale)	__builtin_pulp_subuRN_r((x), (y), (scale)) /*unsigned int,int,int,int*/
#define gap8_subroundnorm(x, y, scale)		__builtin_pulp_subRN((x), (y), (scale), (1<<((scale)-1))) /*int,int,int,int*/
#define gap8_subroundnorm_reg(x, y, scale)	__builtin_pulp_subRN_r((x), (y), (scale)) /*int,int,int,int*/

/* Normalization and rounding */
#define gap8_roundnormu(x, scale)		__builtin_pulp_adduRN((x), 0, (scale), (1<<((scale)-1))) /*unsigned int,int,int*/
#define gap8_roundnormu_reg(x, scale)		__builtin_pulp_adduRN_r((x), 0, (scale)) /*int,int,int*/
#define gap8_roundnorm(x, scale)		__builtin_pulp_addRN((x), 0, (scale), (1<<((scale)-1))) /*unsigned int,int,int*/
#define gap8_roundnorm_reg(x, scale)		__builtin_pulp_addRN_r((x), 0, (scale)) /*int,int,int*/

/* STOP_DEF_BTIN */
#else
/* Here we compile for another target than Gap, PC for example, we emulate the builtins */

#define L2_MEM
#define L1_CL_MEM
#define L1_FC_MEM
/* START_EMUL_BTIN */
/* Packing of scalars into vectors */
#define gap8_pack2(x, y)			((v2s) {(signed short)   (x), (signed short)   (y)})
#define gap8_packu2(x, y)			((v2u) {(unsigned short) (x), (unsigned short) (y)})

#define gap8_pack4(x, y, z, t)			((v4s) {(signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t)})
#define gap8_packu4(x, y, z, t)			((v4u) {(unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t)})

/* Max */
#define gap8_max2(x, y) 			((v2s) {((signed short)(x)[0]>(signed short)(y)[0])?((signed short)(x)[0]):((signed short)(y)[0]), \
							((signed short)(x)[1]>(signed short)(y)[1])?((signed short)(x)[1]):((signed short)(y)[1])})
#define gap8_max4(x, y) 			((v4s) {((signed char)(x)[0]>(signed char)(y)[0])?(signed char)(x)[0]:(signed char)(y)[0], \
							((signed char)(x)[1]>(signed char)(y)[1])?(signed char)(x)[1]:(signed char)(y)[1], \
							((signed char)(x)[2]>(signed char)(y)[2])?(signed char)(x)[2]:(signed char)(y)[2], \
							((signed char)(x)[3]>(signed char)(y)[3])?(signed char)(x)[3]:(signed char)(y)[3]})

#define gap8_maxu2(x, y) 			((v2u) {((unsigned short)(x)[0]>(unsigned short)(y)[0])?(unsigned short)(x)[0]:(unsigned short)(y)[0], \
							((unsigned short)(x)[1]>(unsigned short)(y)[1])?(unsigned short)(x)[1]:(unsigned short)(y)[1]})
#define gap8_maxu4(x, y) 			((v4u) {((unsigned char)(x)[0]>(unsigned char)(y)[0])?(unsigned char)(x)[0]:(unsigned char)(y)[0], \
							((unsigned char)(x)[1]>(unsigned char)(y)[1])?(unsigned char)(x)[1]:(unsigned char)(y)[1], \
							((unsigned char)(x)[2]>(unsigned char)(y)[2])?(unsigned char)(x)[2]:(unsigned char)(y)[2], \
							((unsigned char)(x)[3]>(unsigned char)(y)[3])?(unsigned char)(x)[3]:(unsigned char)(y)[3]})

/* Min */
#define gap8_min2(x, y) 			((v2s) {((signed short)(x)[0]<(signed short)(y)[0])?((signed short)(x)[0]):((signed short)(y)[0]), \
							((signed short)(x)[1]<(signed short)(y)[1])?((signed short)(x)[1]):((signed short)(y)[1])})
#define gap8_min4(x, y) 			((v4s) {((signed char)(x)[0]<(signed char)(y)[0])?(signed char)(x)[0]:(signed char)(y)[0], \
							((signed char)(x)[1]<(signed char)(y)[1])?(signed char)(x)[1]:(signed char)(y)[1], \
							((signed char)(x)[2]<(signed char)(y)[2])?(signed char)(x)[2]:(signed char)(y)[2], \
							((signed char)(x)[3]<(signed char)(y)[3])?(signed char)(x)[3]:(signed char)(y)[3]})

#define gap8_minu2(x, y) 			((v2u) {((unsigned short)(x)[0]<(unsigned short)(y)[0])?(unsigned short)(x)[0]:(unsigned short)(y)[0], \
							((unsigned short)(x)[1]<(unsigned short)(y)[1])?(unsigned short)(x)[1]:(unsigned short)(y)[1]})
#define gap8_minu4(x, y) 			((v4u) {((unsigned char)(x)[0]<(unsigned char)(y)[0])?(unsigned char)(x)[0]:(unsigned char)(y)[0], \
							((unsigned char)(x)[1]<(unsigned char)(y)[1])?(unsigned char)(x)[1]:(unsigned char)(y)[1], \
							((unsigned char)(x)[2]<(unsigned char)(y)[2])?(unsigned char)(x)[2]:(unsigned char)(y)[2], \
							((unsigned char)(x)[3]<(unsigned char)(y)[3])?(unsigned char)(x)[3]:(unsigned char)(y)[3]})

/* Clip */
#define gap8_clip(x, precision)			((x)<(-(1<<(precision)))?(-(1<<(precision))):(((x)>((1<<(precision))-1))?((1<<(precision))-1):(x)))
#define gap8_clip_r(x, bound)			(((x)<=-((bound)+1))?(-((bound)+1)):(((x)>=(bound))?(bound):(x)))

#define gap8_clipu(x, precision)		((x)<0)?0:(((x)>((1<<(precision))-1))?((1<<(precision))-1):(x))
#define gap8_clipu_r(x, bound)			(((x)<=0)?(0):(((x)>=(bound))?(bound):(x)))

/* Abs */
#define gap8_abs2(x) 				((v2u) {((x)[0]<0)?-(x)[0]:(x)[0], ((x)[1]<0)?-(x)[1]:(x)[1]})
#define gap8_abs4(x) 				((v4u) {((x)[0]<0)?-(x)[0]:(x)[0], ((x)[1]<0)?-(x)[1]:(x)[1], \
 						        ((x)[2]<0)?-(x)[2]:(x)[2], ((x)[3]<0)?-(x)[3]:(x)[3]})

/* Unary minus */
#define gap8_neg2(x) 				((v2s) {-(x)[0], -(x)[1]}
#define gap8_neg4(x) 				((v4s) {-(x)[0], -(x)[1], -(x)[2], -(x)[3]}

/* Addition */
#define gap8_add2(x, y) 			((v2s) {(x)[0]+(y)[0], (x)[1]+(y)[1]}
#define gap8_add4(x, y) 			((v4s) {(x)[0]+(y)[0], (x)[1]+(y)[1], (x)[2]+(y)[2], (x)[3]+(y)[3]}

/* Substraction */
#define gap8_sub2(x, y) 			((v2s) {(x)[0]-(y)[0], (x)[1]-(y)[1]}
#define gap8_sub4(x, y) 			((v4s) {(x)[0]-(y)[0], (x)[1]-(y)[1], (x)[2]-(y)[2], (x)[3]-(y)[3]}

/* Average */
#define gap8_avg2(x, y) 			((v2s) {((x)[0]+(y)[0])>>1, ((x)[1]+(y)[1])>>1}
#define gap8_avg4(x, y) 			((v4s) {((x)[0]+(y)[0])>>1, ((x)[1]+(y)[1])>>1, ((x)[2]+(y)[2])>>1, ((x)[3]+(y)[3])>>1}

/* Average unsigned */
#define gap8_avgu2(x, y) 			((v2u) {((unsigned short)(x)[0]+(unsigned short)(y)[0])>>1, ((unsigned short)(x)[1]+(unsigned short)(y)[1])>>1}
#define gap8_avgu4(x, y) 			((v4u) {((unsigned char)(x)[0]+(unsigned char)(y)[0])>>1, ((unsigned char)(x)[1]+(unsigned char)(y)[1])>>1, \
							((unsigned char)(x)[2]+(unsigned char)(y)[2])>>1, ((unsigned char)(x)[3]+(unsigned char)(y)[3])>>1}

/* Bitwise and */
#define gap8_and2(x, y) 			((v2s) {(x)[0]&(y)[0], (x)[1]&(y)[1]}
#define gap8_and4(x, y) 			((v4s) {(x)[0]&(y)[0], (x)[1]&(y)[1], (x)[2]&(y)[2], (x)[3]&(y)[3]}

/* Bitwise or */
#define gap8_or2(x, y) 				((v2s) {(x)[0]|(y)[0], (x)[1]|(y)[1]}
#define gap8_or4(x, y) 				((v4s) {(x)[0]|(y)[0], (x)[1]|(y)[1], (x)[2]|(y)[2], (x)[3]|(y)[3]}

/* Bitwise exor */
#define gap8_exor2(x, y) 			((v2s) {(x)[0]^(y)[0], (x)[1]^(y)[1]}
#define gap8_exor4(x, y) 			((v4s) {(x)[0]^(y)[0], (x)[1]^(y)[1], (x)[2]^(y)[2], (x)[3]^(y)[3]}

/* Logical shift right */
#define gap8_srl2(x, y) 			((v2u) {((unsigned short)(x)[0]>>(unsigned short)(y)[0]), ((unsigned short)(x)[1]>>(unsigned short)(y)[1])}
#define gap8_srl4(x, y) 			((v4u) {((unsigned char)(x)[0]>>(unsigned char)(y)[0]), ((unsigned char)(x)[1]>>(unsigned char)(y)[1]), \
                               		                ((unsigned char)(x)[2]>>(unsigned char)(y)[2]), ((unsigned char)(x)[3]>>(unsigned char)(y)[3])}

/* Arithmetic shift right */
#define gap8_sra2(x, y) 			((v2s) {((signed short)(x)[0]>>(signed short)(y)[0]), ((signed short)(x)[1]>>(signed short)(y)[1])}
#define gap8_sra4(x, y) 			((v4s) {((signed char)(x)[0]>>(signed char)(y)[0]), ((signed char)(x)[1]>>(signed char)(y)[1]), \
                               		                ((signed char)(x)[2]>>(signed char)(y)[2]), ((signed char)(x)[3]>>(signed char)(y)[3])}

/* Logical shift left */
#define gap8_sll2(x, y) 			((v2s) {(x)[0]<<(y)[0], (x)[1]<<(y)[1]}
#define gap8_sll4(x, y) 			((v4s) {(x)[0]<<(y)[0], (x)[1]<<(y)[1], (x)[2]<<(y)[2], (x)[3]<<(y)[3]}

/* Mac */
#define	gap8_mac(Acc, x, y)			((Acc) + ((x) * (y)))
#define	gap8_msu(Acc, x, y)			((Acc) - ((x) * (y)))

#define	gap8_macs(Acc, x, y)			((Acc) + ((short int) (x) * (short int) (y)))
#define	gap8_machhs(Acc, x, y)			((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16)))
#define	gap8_macu(Acc, x, y)			((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))
#define	gap8_machhu(Acc, x, y)			((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))

#define	gap8_macsN(Acc, x, y, n)		(((Acc) + ((short int) (x) * (short int) (y)))>>(n))
#define	gap8_macuN(Acc, x, y, n)		(((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))>>(n))
#define	gap8_macsRN(Acc, x, y, n)		((((Acc) + ((short int) (x) * (short int) (y))) + (1<<((n)-1))) >> (n))
#define	gap8_macuRN(Acc, x, y, n)		((((Acc) + ((unsigned short int) (x) * (unsigned short int) (y))) + (1<<((n)-1))) >> (n))

#define	gap8_machhsN(Acc, x, y, n)		(((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16))) >> (n))
#define	gap8_machhuN(Acc, x, y, n)		(((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16))) >> (n))
#define	gap8_machhsRN(Acc, x, y, n)		((((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16))) + (1<<((n)-1))) >> (n))
#define	gap8_machhuRN(Acc, x, y, n)		((((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16))) + (n)))

/* Multiplications */
#define gap8_muls(x, y)				(((short int) (x) * (short int) (y)))
#define gap8_mulu(x, y)				(((unsigned short int) (x) * (unsigned short int) (y)))
#define gap8_mulhhs(x, y)			(((short int) ((x)>>16) * (short int) ((y)>>16)))
#define gap8_mulhhu(x, y)			(((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))

#define gap8_mulsN(x, y, n)			(((short int) (x) * (short int) (y))>>(n))
#define gap8_mulsRN(x, y, n)			((((short int) (x) * (short int) (y)) + (1<<((n)-1)))>>(n))
#define gap8_muluN(x, y, n)			(((unsigned short int) (x) * (unsigned short int) (y))>>(n))
#define gap8_muluRN(x, y, n)			((((unsigned short int) (x) * (unsigned short int) (y)) + (1<<((n)-1)))>>(n))

#define gap8_mulhhsN(x, y, n)			((((short int) ((x)>>16) * (short int) ((y)>>16)))>>(n))
#define gap8_mulhhsRN(x, y, n)			(((((short int) ((x)>>16) * (short int) ((y)>>16)))+(1<<((n)-1)))>>(n))
#define gap8_mulhhuN(x, y, n)			((((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))>>(n))
#define gap8_mulhhuRN(x, y, n)			(((((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))+(1<<((n)-1)))>>(n))

/* Vectorial product and sum of products */
#define gap8_dotp2(x, y)			(    (short)(x)[0]*(short)(y)[0] + (short)(x)[1]*(short)(y)[1])
#define gap8_dotpu2(x, y)			(    (unsigned short)(x)[0]*(unsigned short)(y)[0] + (unsigned short)(x)[1]*(unsigned short)(y)[1])
#define gap8_dotpus2(x, y)			(    (unsigned short)(x)[0]*(short)(y)[0] + (unsigned short)(x)[1]*(short)(y)[1])

#define gap8_dotpsc2(x, y)			(    (short)(x)[0]*(short)(y) + (short)(x)[1]*(short)(y))
#define gap8_dotpusc2(x, y)			(    (unsigned short)(x)[0]*(unsigned short)(y) + (unsigned short)(x)[1]*(unsigned short)(y))
#define gap8_dotpussc2(x, y)			(    (unsigned short)(x)[0]*(short)(y) + (unsigned short)(x)[1]*(short)(y))

#define gap8_sumdotp2(x, y, z)			((z)+(short)(x)[0]*(short)(y)[0] + (short)(x)[1]*(short)(y)[1])
#define gap8_sumdotpu2(x, y, z)			((z)+(unsigned short)(x)[0]*(unsigned short)(y)[0] + (unsigned short)(x)[1]*(unsigned short)(y)[1])
#define gap8_sumdotpus2(x, y, z)		((z)+(unsigned short)(x)[0]*(short)(y)[0] + (unsigned short)(x)[1]*(short)(y)[1])

#define gap8_sumdotpsc2(x, y, z)		((z)+(short)(x)[0]*(short)(y) + (short)(x)[1]*(short)(y))
#define gap8_sumdotpusc2(x, y, z)		((z)+(unsigned short)(x)[0]*(unsigned short)(y) + (unsigned short)(x)[1]*(unsigned short)(y))
#define gap8_sumdotpussc2(x, y, z)		((z)+(unsigned short)(x)[0]*(short)(y) + (unsigned short)(x)[1]*(short)(y))

#define gap8_dotp4(x, y)			(    (char)(x)[0]*(char)(y)[0] + (char)(x)[1]*(char)(y)[1] + (char)(x)[2]*(char)(y)[2] + (char)(x)[3]*(char)(y)[3])
#define gap8_dotpu4(x, y)			(    (unsigned char)(x)[0]*(unsigned char)(y)[0] + (unsigned char)(x)[1]*(unsigned char)(y)[1] + \
                                       		(unsigned char)(x)[2]*(unsigned char)(y)[2] + (unsigned char)(x)[3]*(unsigned char)(y)[3])
#define gap8_dotpus4(x, y)			(    (unsigned char)(x)[0]*(char)(y)[0] + (unsigned char)(x)[1]*(char)(y)[1] + \
                              			     (unsigned char)(x)[2]*(char)(y)[2] + (unsigned char)(x)[3]*(char)(y)[3])

#define gap8_dotpsc4(x, y)			(    (char)(x)[0]*(char)(y) + (char)(x)[1]*(char)(y) + (char)(x)[2]*(char)(y) + (char)(x)[3]*(char)(y))
#define gap8_dotpusc4(x, y)			(    (unsigned char)(x)[0]*(unsigned char)(y) + (unsigned char)(x)[1]*(unsigned char)(y) + \
                               	              	     (unsigned char)(x)[2]*(unsigned char)(y) + (unsigned char)(x)[3]*(unsigned char)(y))
#define gap8_dotpussc4(x, y)			(    (unsigned char)(x)[0]*(char)(y) + (unsigned char)(x)[1]*(char)(y) + \
                                             	     (unsigned char)(x)[2]*(char)(y) + (unsigned char)(x)[3]*(char)(y))

#define gap8_sumdotp4(x, y, z)			((z)+(char)(x)[0]*(char)(y)[0] + (char)(x)[1]*(char)(y)[1] + (char)(x)[2]*(char)(y)[2] + (char)(x)[3]*(char)(y)[3])
#define gap8_sumdotpu4(x, y, z)			((z)+(unsigned char)(x)[0]*(unsigned char)(y)[0] + (unsigned char)(x)[1]*(unsigned char)(y)[1] + \
                                  		     (unsigned char)(x)[2]*(unsigned char)(y)[2] + (unsigned char)(x)[3]*(unsigned char)(y)[3])
#define gap8_sumdotpus4(x, y, z)		((z)+(unsigned char)(x)[0]*(char)(y)[0] + (unsigned char)(x)[1]*(char)(y)[1] + \
                                             	     (unsigned char)(x)[2]*(char)(y)[2] + (unsigned char)(x)[3]*(char)(y)[3])

#define gap8_sumdotpsc4(x, y, z)		((z)+(char)(x)[0]*(char)(y) + (char)(x)[1]*(char)(y) + (char)(x)[2]*(char)(y) + (char)(x)[3]*(char)(y))
#define gap8_sumdotpusc4(x, y, z)		((z)+(unsigned char)(x)[0]*(unsigned char)(y) + (unsigned char)(x)[1]*(unsigned char)(y) + \
                                       		     (unsigned char)(x)[2]*(unsigned char)(y) + (unsigned char)(x)[3]*(unsigned char)(y))
#define gap8_sumdotpussc4(x, y, z)		((z)+(unsigned char)(x)[0]*(char)(y) + (unsigned char)(x)[1]*(char)(y) + \
                                       		     (unsigned char)(x)[2]*(char)(y) + (unsigned char)(x)[3]*(char)(y))

/* Complex Multiplication, Q15x15 into Q15, with optional post scaling by 1 or 2 */
#define gap8_cplxmuls(x, y)			((v2s) {(signed short) ((((long long) (x)[0]*(long long) (y)[0]) - ((long long) (x)[1]*(long long) (y)[1]))>>15),  \
							(signed short) ((((long long) (x)[0]*(long long) (y)[1]) + ((long long) (x)[1]*(long long) (y)[0]))>>15)})
#define gap8_cplxmulsdiv2(x, y)			(gap8_cplxmuls(x, y)>>(v2s){1,1})
#define gap8_cplxmulsdiv4(x, y)			(gap8_cplxmuls(x, y)>>(v2s){2,2})

/* Complex conjugate */
#define gap8_cplxconj(x)			((v2s) {(x)[0], -(x)[1]})

/* Complex substration, result rotated by -pi/2 */
#define gap8_sub2rotmj(x, y)			((v2s) {(x)[1]-(y)[1], (y)[0]-(x)[0]})

/* Complex addition with post scaling by 1 or 2 */
#define gap8_add2div2(x, y)			(((x)+(y))>>(v2s) {1, 1})
#define gap8_add2div4(x, y)			(((x)+(y))>>(v2s) {2, 2})

#define gap8_add4div2(x, y)			(((x)+(y))>>(v4s) {1, 1, 1, 1})
#define gap8_add4div4(x, y)			(((x)+(y))>>(v4s) {2, 2, 2, 2})

/* Complex substraction with post scaling by 1 or 2 */
#define gap8_sub2div2(x, y)			(((x)-(y))>>(v2s) {1, 1})
#define gap8_sub2div4(x, y)			(((x)-(y))>>(v2s) {2, 2})

#define gap8_sub4div2(x, y)			(((x)-(y))>>(v4s) {1, 1, 1, 1})
#define gap8_sub4div4(x, y)			(((x)-(y))>>(v4s) {2, 2, 2, 2})

/* Complex subtraction with post scaling by 1 or 2 */
#define gap8_sub2div2(x, y)			(((x)-(y))>>(v2s) {1, 1})
#define gap8_sub2div4(x, y)			(((x)-(y))>>(v2s) {2, 2})

/* Viterbi Max and Viterbi Select, pair of Q15 */
#define gap8_vitmax(x, y) 			(_VitT1_Flag=((x)[1]<=(y)[1])?1:0, _VitT0_Flag=((x)[0]<=(y)[0])?1:0,\
					 	(v2s) {((x)[0]>(y)[0])?(x)[0]:(y)[0], ((x)[1]>(y)[1])?(x)[1]:(y)[1]})
#define gap8_vitsel(x, y) 			(v2s) {(_VitT0_Flag?(y)[0]:(x)[0])<<1|_VitT0_Flag, (_VitT1_Flag?(y)[1]:(x)[1])<<1|_VitT1_Flag}

/* Position of the most significant bit of x */
#define gap8_fl1(x)				(31 - __builtin_clz((x)))

/* Number of sign bits */
#define gap8_clb(x)				(__builtin_clrsb((x)))

/* Bit set */
#define gap8_bitset(x, size, off)		((x) | (((1<<(size))-1)<<(off)))
#define gap8_bitset_r(x, size, off)		((x) | (((1<<(size))-1)<<(off)))
#define gap8_bitset_r_safe(x, size, off)	((x) | (((1<<((size)&0x1F))-1)<<((off)&0x1F)))

/* Bit clr */
#define gap8_bitclr(x, size, off)		((x) & ~(((1<<(size))-1)<<(off)))
#define gap8_bitclr_r(x, size, off)		((x) & ~(((1<<(size))-1)<<(off)))
#define gap8_bitclr_r_safe(x, size, off)	((x) & ~(((1<<((size)&0x1F))-1)<<((off)&0x1F)))

/* Bit Extraction */
#define gap8_bitextract(x, size, off) 		(((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap8_bitextractu(x, size, off)		(((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap8_bitextract_r(x, size, off) 	(((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap8_bitextractu_r(x, size, off)	(((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap8_bitextract_r_safe(x, size, off) 	(((((x)>>((off)&0x1F))&((unsigned int)(1<<((((size)>32)?32:(size))))-1))<<(32-((((size)>32)?32:(size)))))>>(32-((((size)>32)?32:(size)))))
#define gap8_bitextractu_r_safe(x, size, off)	(((x)>>((off)&0x1F))&((unsigned int)(1<<((((size)>32)?32:(size))))-1))

/* Bit insertion */
#define gap8_bitinsert(dst, src, size, off) 	(((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define gap8_bitinsert_r(dst, src, size, off) 	(((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define gap8_bitinsert_r_safe(dst, src, size, off) 	(((dst) & ~(((1<<(((size)>32)?32:(size)))-1)<<((off)&0x1F))) | (((src) & ((1<<(((size)>32)?32:(size)))-1))<<((off)&0x1F)))

/* 1 bit rotation to the right, 32 bits input */
#define gap8_rotr(x)				((((x)>>1)&0x7FFFFFFF) | ((x)<<31))

/* Add with normalization */
#define gap8_addnormu(x, y, scale)		((unsigned int)((x) + (y))>>(scale))
#define gap8_addnormu_reg(x, y, scale)		((unsigned int)((x) + (y))>>(scale))
#define gap8_addnorm(x, y, scale)		((int)((x) + (y))>>(scale))
#define gap8_addnorm_reg(x, y, scale)		((int)((x) + (y))>>(scale))

/* Add with normalization and rounding */
#define gap8_addroundnormu(x, y, scale)		((unsigned int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap8_addroundnormu_reg(x, y, scale)	((unsigned int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap8_addroundnorm(x, y, scale)		((int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap8_addroundnorm_reg(x, y, scale)	((int)((x) + (y) + (1<<((scale)-1)))>>(scale))

/* Sub with normalization */
#define gap8_subnormu(x, y, scale)		((unsigned int)((x) - (y))>>(scale))
#define gap8_subnormu_reg(x, y, scale)		((unsigned int)((x) - (y))>>(scale))
#define gap8_subnorm(x, y, scale)		((int)((x) - (y))>>(scale))
#define gap8_subnorm_reg(x, y, scale)		((int)((x) - (y))>>(scale))

/* Sub with normalization and rounding */
#define gap8_subroundnormu(x, y, scale)		((unsigned int)((x) - (y) + (1<<((scale)-1)))>>(scale))
#define gap8_subroundnormu_reg(x, y, scale)	((unsigned int)((x) - (y) + (1<<((scale)-1)))>>(scale))
#define gap8_subroundnorm(x, y, scale)		((int)((x) - (y) + (1<<((scale)-1)))>>(scale))
#define gap8_subroundnorm_reg(x, y, scale)	((int)((x) - (y) + (1<<((scale)-1)))>>(scale))

/* Normalization and rounding */
#define gap8_roundnormu(x, scale)		((unsigned int)((x) + (1<<((scale)-1)))>>(scale))
#define gap8_roundnormu_reg(x, scale)		((unsigned int)((x) + (1<<((scale)-1)))>>(scale))
#define gap8_roundnorm(x, scale)		((int)((x) + (1<<((scale)-1)))>>(scale))
#define gap8_roundnorm_reg(x, scale)		((int)((x) + (1<<((scale)-1)))>>(scale))
/* STOP_EMUL_BTIN */
#endif
/* END OF ARITHMETIC SECTION */

/* ACCESSING SPECIAL REGISTERS, BASE + 12 Bits OFFSETS READ/WRITE FOR MEMORY MAPPED PERIPHERALS */
#if defined (__pulp__)

/* START_DEF_BTIN */
#define gap8_coreid()					__builtin_pulp_CoreId() /*int*/
#define gap8_clusterid()				__builtin_pulp_ClusterId() /*int*/
#define gap8_ncore()					__builtin_pulp_CoreCount() /*int*/
#define gap8_isfc()					__builtin_pulp_IsFc() /*int*/

#define gap8_sprwrite(x, y)				__builtin_pulp_spr_write(x, y) /*void,int,int*/
#define gap8_sprread(x)					__builtin_pulp_spr_read(x) /*int,int*/
#define gap8_sprread_vol(x)				__builtin_pulp_spr_read_vol(x) /*int,int*/

#define gap8_sprbitset(spr, off)			__builtin_pulp_spr_bit_set((spr), (off)) /*void,int,int*/
#define gap8_sprbitclr(spr, off)			__builtin_pulp_spr_bit_clr((spr), (off)) /*void,int,int*/

#define gap8_sprreadthenwrite(spr, x)			__builtin_pulp_read_then_spr_write((spr), (x)) /*int,int,int*/
#define gap8_sprreadthenbitset(spr, off)		__builtin_pulp_read_then_spr_bit_set((spr), (off)) /*int,int,int*/
#define gap8_sprreadthenbitclr(spr, off)		__builtin_pulp_read_then_spr_bit_clr((spr), (off)) /*int,int,int*/



#define gap8_read_base_off(base, off)           	__builtin_pulp_read_base_off((base), (off)) /*int,int *,int*/
#define gap8_write_base_off(base, off, val)     	__builtin_pulp_write_base_off((base), (off), (val)) /*void,int *,int,int*/

#define gap8_read_base_off_vol(base, off)       	__builtin_pulp_OffsetedRead((base), (off)) /*int,int *,int*/
#define gap8_read_base_off_half_vol(base, off)  	__builtin_pulp_OffsetedReadHalf((base), (off)) /*short int,short int *,int*/
#define gap8_read_base_off_byte_vol(base, off)  	__builtin_pulp_OffsetedReadByte((base), (off)) /*signed char,signed char *,int*/

#define gap8_write_base_off_vol(x, base, off)       	__builtin_pulp_OffsetedWrite((x), (base), (off)) /*void,int,int *,int*/
#define gap8_write_base_off_half_vol(x, base, off)  	__builtin_pulp_OffsetedWriteHalf((x), (base), (off)) /*void,int,short int *,int*/
#define gap8_write_base_off_byte_vol(x, base, off)  	__builtin_pulp_OffsetedWriteByte((x), (base), (off)) /*void,int,signed char *,int*/
/* STOP_DEF_BTIN */
#endif

/* SYSTEM SECTION */
#ifdef __pulp__
#define START_TIMER() __asm__("scallimm 1")
#define STOP_TIMER() __asm__("scallimm 3")
#endif

/* SYSTEM SECTION FOR GAP8 TARGET */
#ifdef __MBED__

#define gap8_dma_memcpy(ext, loc, size, ext2loc, merge, copy)			AUTOTILE_DMAMCHAN_Memcpy_1D((ext), (loc), (size), (ext2loc), (merge), (copy))
#define gap8_dma_memcpy_2d(ext, loc, size, stride, length, ext2loc, merge, copy)	AUTOTILE_DMAMCHAN_Memcpy_2D((ext), (loc), (size), (stride), (length), (ext2loc), (merge), (copy))
#define gap8_dma_wait(copy)						DMAMCHAN_WaitRequestEnd(&(copy))
#define gap8_dma_type                               dma_req_t
/* HW barriers */
#define gap8_waitbarrier(BarN)						rt_team_barrier()

#else

#ifdef __pulp__

/* Task dispatch */
#define gap8_task_dispatch(coreMask, handler, arg)			plp_setSlaveEntry((coreMask), (handler), (arg))

/* Interrupts Enable/Disable */
#define gap8_enable_its()						plp_irq_enable()
#define gap8_disable_its()						plp_irq_disable()

/* HW timer */
#define ARCHI_FC_TIMER_ADDR                     ( ARCHI_FC_PERIPHERALS_ADDR + ARCHI_FC_TIMER_OFFSET  )
#define PLP_TIMER_VALUE_LO                      0x08
#define PLP_TIMER_CFG_REG_LO                    0x00
#define PLP_TIMER_ENABLE_BIT            0
#define PLP_TIMER_RESET_BIT             1
#define PLP_TIMER_IRQ_ENABLE_BIT        2
#define PLP_TIMER_IEM_BIT              	3
#define PLP_TIMER_CMP_CLR_BIT           4
#define PLP_TIMER_ONE_SHOT_BIT          5
#define PLP_TIMER_PRESCALER_ENABLE_BIT  6
#define PLP_TIMER_CLOCK_SOURCE_BIT      7
#define PLP_TIMER_PRESCALER_VALUE_BIT   8
#define PLP_TIMER_PRESCALER_VALUE_BITS  8
#define PLP_TIMER_64_BIT                31

#define plp_timer_conf_get(a,b,c,d,e,f,g,h,i)      ((a << PLP_TIMER_ENABLE_BIT) \
        | (b << PLP_TIMER_RESET_BIT) \
        | (c << PLP_TIMER_IRQ_ENABLE_BIT) \
        | (d << PLP_TIMER_IEM_BIT) \
        | (e << PLP_TIMER_CMP_CLR_BIT) \
        | (f << PLP_TIMER_ONE_SHOT_BIT) \
        | (g << PLP_TIMER_PRESCALER_ENABLE_BIT) \
        | (h << PLP_TIMER_PRESCALER_VALUE_BIT) \
        | (i << PLP_TIMER_64_BIT) \
        )
#define gap8_resethwtimer()						pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_LO, plp_timer_conf_get(1,1,0,0,0,0,0,0,0))
#define gap8_readhwtimer()						pulp_read32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_VALUE_LO)

/* Core Counters */
#define gap8_setmodehwcounters(Val)					gap8_sprwrite(HW_COUNT_MODE_COUNTERS, (Val))
#define gap8_setallhwcounters(Val)					gap8_sprwrite((HW_COUNT_BASE_OFFSET+HW_COUNT_WRITE_ALL), (Val))
#define gap8_enablehwcounters(Val)					gap8_sprwrite(HW_COUNT_EN_COUNTERS, (Val))
#define gap8_readhwcounter(Which)					gap8_sprread_vol(HW_COUNT_BASE_OFFSET+(Which))

/* HW barriers */
#define gap8_setupbarrier(BarN, CoreM)					eu_bar_setup(eu_bar_addr(BarN), CoreM)
#define gap8_waitbarrier(BarN)						eu_bar_trig_wait_clr(eu_bar_addr(BarN))

/* Fetch enable */
#define gap8_fetchenable(CoreMask)					eoc_fetch_enable(CoreMask)

/* Wait cluster end of exec */
#define gap8_cluster_wait(x)						plp_cluster_wait(x)

/* DMA copies */
#define gap8_dma_memcpy(ext, loc, size, ext2loc, merge, copy)			rt_dma_memcpy(((unsigned int) ext), ((unsigned int) loc), (size), (ext2loc), (merge), (copy))
#define gap8_dma_memcpy_2d(ext, loc, size, stride, length, ext2loc, merge, copy)	rt_dma_memcpy_2d((ext), (loc), (size), (stride), (length), (ext2loc), (merge), (copy))
#define gap8_dma_wait(copy)						rt_dma_wait(&(copy))
#define gap8_dma_type                               rt_dma_copy_t
#endif

#endif


/* START_NEW_RUNTIME */
/* Utilities, Target independant */
#define FIX2FP(Val, Precision)						((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)						((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)						((int)((Val)*((1 << (Precision))-1)))
/* STOP_NEW_RUNTIME */

/* START_NEW_RUNTIME
#endif
#endif
STOP_NEW_RUNTIME */
#endif
#endif
