#ifndef __GAP_BUILTINS_H__
#define __GAP_BUILTINS_H__

/* Operations */
#if !defined(__EMUL__) || defined(__pulp__)

static inline unsigned int ExtInsMaskFast(unsigned int Size, unsigned int Offset) { return ((((Size-1))<<5)|(Offset)); }
static inline unsigned int ExtInsMaskSafe(unsigned int Size, unsigned int Offset) { return ((((Size-1)&0x1F)<<5)|(Offset&0x1F)); }

/* Packing of scalars into vectors */
#define gap_pack2(x, y)			__builtin_pulp_pack2((signed short)   (x), (signed short)   (y))
#define gap_packu2(x, y)		__builtin_pulp_pack2((unsigned short) (x), (unsigned short) (y))

#define gap_pack4(x, y, z, t)		__builtin_pulp_pack4((signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t))
#define gap_packu4(x, y, z, t)		__builtin_pulp_pack4((unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t))

/* Max */
#define gap_max(x, y)			__builtin_pulp_maxsi((x), (y))
#define gap_max2(x, y) 			__builtin_pulp_max2((x), (y))
#define gap_max4(x, y) 			__builtin_pulp_max4((x), (y))

#define gap_maxu2(x, y) 		__builtin_pulp_maxu2((x), (y))
#define gap_maxu4(x, y) 		__builtin_pulp_maxu4((x), (y))

/* Min */
#define gap_min(x, y)			__builtin_pulp_minsi((x), (y))
#define gap_min2(x, y) 			__builtin_pulp_min2((x), (y))
#define gap_min4(x, y) 			__builtin_pulp_min4((x), (y))

#define gap_minu2(x, y) 		__builtin_pulp_minu2((x), (y))
#define gap_minu4(x, y) 		__builtin_pulp_minu4((x), (y))

/* Add */
#define gap_add2(x, y) 			__builtin_pulp_add2((x), (y))
#define gap_add4(x, y) 			__builtin_pulp_add4((x), (y))

/* Sub */
#define gap_sub2(x, y) 			__builtin_pulp_sub2((x), (y))
#define gap_sub4(x, y) 			__builtin_pulp_sub4((x), (y))

/* Clip */
#define gap_clip(x, precision) 		__builtin_pulp_clip((x), -(1<<(precision)), (1<<precision)-1)
#define gap_clipu(x, precision) 	__builtin_pulp_clipu((x), 0, (1<<precision)-1)

/* Abs */
#define gap_abs(x) 			__builtin_pulp_abs((x))
#define gap_abs2(x) 			__builtin_pulp_abs2((x))
#define gap_abs4(x) 			__builtin_pulp_abs4((x))


/* Truncation */
#define	gap_trunh(x)			__builtin_pulp_trunch((x))
#define	gap_trunb(x)			__builtin_pulp_truncb((x))

/* Mac */
#define	gap_macs(Acc, x, y)		__builtin_pulp_macs((x), (y), (Acc))
#define	gap_macfs(Acc, x, y)		__builtin_pulp_macfs((x), (y), (Acc))
#define	gap_machhs(Acc, x, y)		__builtin_pulp_machhs((x), (y), (Acc))
#define	gap_macu(Acc, x, y)		__builtin_pulp_macu((x), (y), (Acc))
#define	gap_macfu(Acc, x, y)		__builtin_pulp_macfu((x), (y), (Acc))
#define	gap_machhu(Acc, x, y)		__builtin_pulp_machhu((x), (y), (Acc))

#define gap_macsN(Acc, x, y, n)		__builtin_pulp_macsN((x), (y), (Acc), (n))
#define gap_macfsN(Acc, x, y, n)	__builtin_pulp_macfsN((x), (y), (Acc), (n))
#define gap_macuN(Acc, x, y, n)		__builtin_pulp_macuN((x), (y), (Acc), (n))
#define gap_macfuN(Acc, x, y, n)	__builtin_pulp_macfuN((x), (y), (Acc), (n))
#define gap_macsRN(Acc, x, y, n)	__builtin_pulp_macsRN((x), (y), (Acc), (n), (1<<((n)-1)))
#define gap_macfsRN(Acc, x, y, n)	__builtin_pulp_macfsRN((x), (y), (Acc), (n), (1<<((n)-1)))
#define gap_macuRN(Acc, x, y, n)	__builtin_pulp_macuRN((x), (y), (Acc), (n), (1<<((n)-1)))
#define gap_macfuRN(Acc, x, y, n)	__builtin_pulp_macfuRN((x), (y), (Acc), (n), (1<<((n)-1)))

#define gap_machhsN(Acc, x, y, n)	__builtin_pulp_machhsN((x), (y), (Acc), (n))
#define gap_machhuN(Acc, x, y, n)	__builtin_pulp_machhuN((x), (y), (Acc), (n))
#define gap_machhsRN(Acc, x, y, n)	__builtin_pulp_machhsN((x), (y), (Acc), (n), (1<<((n)-1)))
#define gap_machhuRN(Acc, x, y, n)	__builtin_pulp_machhuN((x), (y), (Acc), (n), (1<<((n)-1)))

/* Multiplications */
#define gap_muls(x, y)			__builtin_pulp_muls((x), (y))
#define gap_mulfs(x, y)			__builtin_pulp_mulfs((x), (y))
#define gap_mulu(x, y)			__builtin_pulp_mulu((x), (y))
#define gap_mulfu(x, y)			__builtin_pulp_mulfu((x), (y))
#define gap_mulhhs(x, y)		__builtin_pulp_mulhhs((x), (y))
#define gap_mulhhu(x, y)		__builtin_pulp_mulhhu((x), (y))

#define gap_mulsN(x, y, n)		__builtin_pulp_mulsN((x), (y), (n))
#define gap_mulfsN(x, y, n)		__builtin_pulp_mulfsN((x), (y), (n))
#define gap_mulsRN(x, y, n)		__builtin_pulp_mulsRN((x), (y), (n), (1<<((n)-1)))
#define gap_mulfsRN(x, y, n)		__builtin_pulp_mulfsRN((x), (y), (n), (1<<((n)-1)))
#define gap_muluN(x, y, n)		__builtin_pulp_muluN((x), (y), (n))
#define gap_mulfuN(x, y, n)		__builtin_pulp_mulfuN((x), (y), (n))
#define gap_muluRN(x, y, n)		__builtin_pulp_muluRN((x), (y), (n), (1<<((n)-1)))

#define gap_mulhhsN(x, y, n)		__builtin_pulp_mulhhsN((x), (y), (n))
#define gap_mulhhsRN(x, y, n)		__builtin_pulp_mulhhsRN((x), (y), (n), (1<<((n)-1)))
#define gap_mulhhuN(x, y, n)		__builtin_pulp_mulhhuN((x), (y), (n))
#define gap_mulhhuRN(x, y, n)		__builtin_pulp_mulhhuRN((x), (y), (n), (1<<((n)-1)))

/* Vectorial product and sum of products */
#define gap_dotp2(x, y)     		__builtin_pulp_dotsp2((x), (y))
#define gap_dotpu2(x, y)     		__builtin_pulp_dotup2((x), (y))
#define gap_dotpus2(x, y)    		__builtin_pulp_dotusp2((x), (y))

#define gap_sumdotp2(x, y, z)		__builtin_pulp_sdotsp2((x), (y), (z))
#define gap_sumdotpu2(x, y, z)		__builtin_pulp_sdotup2((x), (y), (z))
#define gap_sumdotpus2(x, y, z)	__builtin_pulp_sdotusp2((x), (y), (z))

#define gap_dotp4(x, y)     		__builtin_pulp_dotsp4((x), (y))
#define gap_dotpu4(x, y)     		__builtin_pulp_dotup4((x), (y))
#define gap_dotpus4(x, y)    		__builtin_pulp_dotusp4((x), (y))

#define gap_sumdotp4(x, y, z)     	__builtin_pulp_sdotsp4((x), (y), (z))
#define gap_sumdotpu4(x, y, z)     	__builtin_pulp_sdotup4((x), (y), (z))
#define gap_sumdotpus4(x, y, z)    	__builtin_pulp_sdotusp4((x), (y), (z))


/* Complex Multiplication, Q15x15 into Q15, with optional post scaling by 1 or 2 */
#if defined (__gap8__)
#define gap_cplxmuls(x, y)		__builtin_pulp_cplxmuls((x), (y))
#define gap_cplxmulsdiv2(x, y)		__builtin_pulp_cplxmulsdiv2((x), (y))
#define gap_cplxmulsdiv4(x, y)		__builtin_pulp_cplxmulsdiv4((x), (y))
#elif defined (__gap9__)
#define gap_cplxmuls(x, y)		__builtin_pulp_cplxmuls2((x), (y))
#define gap_cplxmulsdiv2(x, y)		__builtin_pulp_cplxmuls2div2((x), (y))
#define gap_cplxmulsdiv4(x, y)		__builtin_pulp_cplxmuls2div4((x), (y))
#define gap_cplxmulsdiv8(x, y)		__builtin_pulp_cplxmuls2div8((x), (y))
#endif

/* Complex conjugate */
#define gap_cplxconj(x)			__builtin_pulp_cplx_conj((x))

/* Complex rotation by -pi/2 */
#define gap_cplxmj(x)			__builtin_pulp_cplxmjrot2((x))

/* Complex substration, result rotated by -pi/2 */
#define gap_sub2rotmj(x, y)		__builtin_pulp_sub2rotmj((x), (y))

/* Complex addition with post scaling by 1 or 2 */
#define gap_add2div2(x, y)		__builtin_pulp_add2div2((x), (y))
#define gap_add2div4(x, y)		__builtin_pulp_add2div4((x), (y))

/* Complex subtraction with post scaling by 1 or 2 */
#define gap_sub2div2(x, y)		__builtin_pulp_sub2div2((x), (y))
#define gap_sub2div4(x, y)		__builtin_pulp_sub2div4((x), (y))

/* Viterbi Max and Viterbi Select, pair of Q15 */
#if defined (__gap8__)
#define gap_vitmax(x, y) 		__builtin_pulp_vitmax2((x), (y))
#define gap_vitsel(x, y) 		__builtin_pulp_vitsel2((x), (y))
#endif

/* Position of the most significant bit of x */
#define gap_fl1(x)			__builtin_pulp_fl1((x))

/* Number of sign bits */
#define gap_clb(x)			__builtin_pulp_clb((x))

/* Bit Extraction */
#define gap_bitextract(x, size, off)		__builtin_pulp_bextract((x), (size), (off))
#define gap_bitextractu(x, size, off)		__builtin_pulp_bextractu((x), (size), (off))

#define gap_bitextract_r(x, size, off)		__builtin_pulp_bextract_r((x), ExtInsMaskFast((size), (off)))
#define gap_bitextractu_r(x, size, off)		__builtin_pulp_bextractu_r((x), ExtInsMaskFast((size), (off)))

#define gap_bitextract_r_safe(x, size, off)	__builtin_pulp_bextract_r((x), ExtInsMaskSafe((size), (off)))
#define gap_bitextractu_r_safe(x, size, off)	__builtin_pulp_bextractu_r((x), ExtInsMaskSafe((size), (off)))

/* Bit insertion */
#define gap_bitinsert(dst, src, size, off) 		__builtin_pulp_binsert((dst), ~(((1<<(size))-1)<<(off)), (src), (((1<<(size))-1)<<(off)), (off))
#define gap_bitinsert_r(dst, src, size, off) 		__builtin_pulp_binsert_r((dst), (src), ExtInsMaskFast((size), (off)))
#define gap_bitinsert_r_safe(dst, src, size, off) 	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskSafe((size), (off)))

/* 1 bit rotation to the right, 32 bits input */
#define gap_rotr(x)				__builtin_pulp_rotr((x))

/* Add with normalization and rounding */
#define gap_addroundnormu(x, y, scale)		__builtin_pulp_adduRN((x), (y), (scale), (1<<((scale)-1)))
#define gap_addroundnormu_reg(x, y, scale)	__builtin_pulp_adduRN_r((x), (y), (scale))
#define gap_addroundnorm(x, y, scale)		__builtin_pulp_addRN((x), (y), (scale), (1<<((scale)-1)))
#define gap_addroundnorm_reg(x, y, scale)	__builtin_pulp_addRN_r((x), (y), (scale))

/* Normalization and rounding */
#define gap_roundnormu(x, scale)		__builtin_pulp_adduRN((x), 0, (scale), (1<<((scale)-1)))
#define gap_roundnormu_reg(x, scale)		__builtin_pulp_adduRN_r((x), 0, (scale))
#define gap_roundnorm(x, scale)			__builtin_pulp_addRN((x), 0, (scale), (1<<((scale)-1)))
#define gap_roundnorm_reg(x, scale)		__builtin_pulp_addRN_r((x), 0, (scale))

#else
/* Emulation */

/* Static flags  to track viterbi max */
static int _VitT0_Flag, _VitT1_Flag;

/* Packing of scalars into vectors */
#define gap_pack2(x, y)			((v2s) {(signed short)   (x), (signed short)   (y)})
#define gap_packu2(x, y)		((v2u) {(unsigned short) (x), (unsigned short) (y)})

#define gap_pack4(x, y, z, t)		((v4s) {(signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t)})
#define gap_packu4(x, y, z, t)		((v4u) {(unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t)})

/* Max */
#define gap_max(x, y)			(((x)>(y))?(x):(y))
#define gap_max2(x, y) 			((v2s) {((signed short)(x)[0]>(signed short)(y)[0])?((signed short)(x)[0]):((signed short)(y)[0]), \
						((signed short)(x)[1]>(signed short)(y)[1])?((signed short)(x)[1]):((signed short)(y)[1])})
#define gap_max4(x, y) 			((v4s) {((signed char)(x)[0]>(signed char)(y)[0])?(signed char)(x)[0]:(signed char)(y)[0], \
						((signed char)(x)[1]>(signed char)(y)[1])?(signed char)(x)[1]:(signed char)(y)[1], \
						((signed char)(x)[2]>(signed char)(y)[2])?(signed char)(x)[2]:(signed char)(y)[2], \
						((signed char)(x)[3]>(signed char)(y)[3])?(signed char)(x)[3]:(signed char)(y)[3]})

#define gap_maxu2(x, y) 		((v2u) {((unsigned short)(x)[0]>(unsigned short)(y)[0])?(unsigned short)(x)[0]:(unsigned short)(y)[0], \
						((unsigned short)(x)[1]>(unsigned short)(y)[1])?(unsigned short)(x)[1]:(unsigned short)(y)[1]})
#define gap_maxu4(x, y) 		((v4u) {((unsigned char)(x)[0]>(unsigned char)(y)[0])?(unsigned char)(x)[0]:(unsigned char)(y)[0], \
						((unsigned char)(x)[1]>(unsigned char)(y)[1])?(unsigned char)(x)[1]:(unsigned char)(y)[1], \
						((unsigned char)(x)[2]>(unsigned char)(y)[2])?(unsigned char)(x)[2]:(unsigned char)(y)[2], \
						((unsigned char)(x)[3]>(unsigned char)(y)[3])?(unsigned char)(x)[3]:(unsigned char)(y)[3]})

/* Min */
#define gap_min(x, y)			(((x)<(y))?(x):(y))
#define gap_min2(x, y) 			((v2s) {((signed short)(x)[0]<(signed short)(y)[0])?((signed short)(x)[0]):((signed short)(y)[0]), \
						((signed short)(x)[1]<(signed short)(y)[1])?((signed short)(x)[1]):((signed short)(y)[1])})
#define gap_min4(x, y) 			((v4s) {((signed char)(x)[0]<(signed char)(y)[0])?(signed char)(x)[0]:(signed char)(y)[0], \
						((signed char)(x)[1]<(signed char)(y)[1])?(signed char)(x)[1]:(signed char)(y)[1], \
						((signed char)(x)[2]<(signed char)(y)[2])?(signed char)(x)[2]:(signed char)(y)[2], \
						((signed char)(x)[3]<(signed char)(y)[3])?(signed char)(x)[3]:(signed char)(y)[3]})

#define gap_minu2(x, y) 		((v2u) {((unsigned short)(x)[0]<(unsigned short)(y)[0])?(unsigned short)(x)[0]:(unsigned short)(y)[0], \
						((unsigned short)(x)[1]<(unsigned short)(y)[1])?(unsigned short)(x)[1]:(unsigned short)(y)[1]})
#define gap_minu4(x, y) 		((v4u) {((unsigned char)(x)[0]<(unsigned char)(y)[0])?(unsigned char)(x)[0]:(unsigned char)(y)[0], \
						((unsigned char)(x)[1]<(unsigned char)(y)[1])?(unsigned char)(x)[1]:(unsigned char)(y)[1], \
						((unsigned char)(x)[2]<(unsigned char)(y)[2])?(unsigned char)(x)[2]:(unsigned char)(y)[2], \
						((unsigned char)(x)[3]<(unsigned char)(y)[3])?(unsigned char)(x)[3]:(unsigned char)(y)[3]})

/* Add */
#define gap_add2(x, y) 			((v2s) {(short int)(x)[0]+(short int)(y)[0], (short int)(x)[1]+(short int)(y)[1]})
#define gap_add4(x, y) 			((v4s) {(signed char)(x)[0]+(signed char)(y)[0], (signed char)(x)[1]+(signed char)(y)[1], \
		 			        (signed char)(x)[2]+(signed char)(y)[2], (signed char)(x)[3]+(signed char)(y)[3]})

/* Sub */
#define gap_sub2(x, y) 			((v2s) {(short int)(x)[0]-(short int)(y)[0], (short int)(x)[1]-(short int)(y)[1]})
#define gap_sub4(x, y) 			((v4s) {(signed char)(x)[0]-(signed char)(y)[0], (signed char)(x)[1]-(signed char)(y)[1], \
		 			        (signed char)(x)[2]-(signed char)(y)[2], (signed char)(x)[3]-(signed char)(y)[3]})

/* Clip */
#define gap_clip(x, precision)		((x)<(-(1<<(precision)))?(-(1<<(precision))):(((x)>((1<<(precision))-1))?((1<<(precision))-1):(x)))
#define gap_clipu(x, precision)		((x)<0)?0:(((x)>((1<<(precision))-1))?((1<<(precision))-1):(x))

/* Abs */
#define gap_abs(x) 			(((x)[0]<0)?-(x):(x))
#define gap_abs2(x) 			((v2s) {((x)[0]<0)?-(x)[0]:(x)[0], ((x)[1]<0)?-(x)[1]:(x)[1]})
#define gap_abs4(x) 			((v4s) {((x)[0]<0)?-(x)[0]:(x)[0], ((x)[1]<0)?-(x)[1]:(x)[1], \
						((x)[2]<0)?-(x)[2]:(x)[2], ((x)[3]<0)?-(x)[3]:(x)[3]})

/* Truncation */
#define	gap_trunh(x)			((short int) (x))
#define	gap_trunb(x)			((signed char) (x))

/* Mac */
#define	gap_macs(Acc, x, y)		((Acc) + ((short int) (x) * (short int) (y)))
#define	gap_macfs(Acc, x, y)		((Acc) + ((short int) (x) * (short int) (y)))
#define	gap_machhs(Acc, x, y)		((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16)))
#define	gap_macu(Acc, x, y)		((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))
#define	gap_macfu(Acc, x, y)		((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))
#define	gap_machhu(Acc, x, y)		((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))

#define	gap_macsN(Acc, x, y, n)		(((Acc) + ((short int) (x) * (short int) (y)))>>(n))
#define	gap_macfsN(Acc, x, y, n)	(((Acc) + ((short int) (x) * (short int) (y)))>>(n))
#define	gap_macuN(Acc, x, y, n)		(((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))>>(n))
#define	gap_macfuN(Acc, x, y, n)	(((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))>>(n))
#define	gap_macsRN(Acc, x, y, n)	((((Acc) + ((short int) (x) * (short int) (y))) + (1<<((n)-1))) >> (n))
#define	gap_macfsRN(Acc, x, y, n)	((((Acc) + ((short int) (x) * (short int) (y))) + (1<<((n)-1))) >> (n))
#define	gap_macuRN(Acc, x, y, n)	((((Acc) + ((unsigned short int) (x) * (unsigned short int) (y))) + (1<<((n)-1))) >> (n))
#define	gap_macfuRN(Acc, x, y, n)	((((Acc) + ((unsigned short int) (x) * (unsigned short int) (y))) + (1<<((n)-1))) >> (n))

#define	gap_machhsN(Acc, x, y, n)	(((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16))) >> (n))
#define	gap_machhuN(Acc, x, y, n)	(((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16))) >> (n))
#define	gap_machhsRN(Acc, x, y, n)	((((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16))) + (1<<((n)-1))) >> (n))
#define	gap_machhuRN(Acc, x, y, n)	((((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16))) + (n)))

/* Multiplications */
#define gap_muls(x, y)			(((short int) (x) * (short int) (y)))
#define gap_mulfs(x, y)			(((short int) (x) * (short int) (y)))
#define gap_mulu(x, y)			(((unsigned short int) (x) * (unsigned short int) (y)))
#define gap_mulfu(x, y)			(((unsigned short int) (x) * (unsigned short int) (y)))
#define gap_mulhhs(x, y)		(((short int) ((x)>>16) * (short int) ((y)>>16)))
#define gap_mulhhu(x, y)		(((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))
#define gap_mulsN(x, y, n)		(((short int) (x) * (short int) (y))>>(n))
#define gap_mulfsN(x, y, n)		(((short int) (x) * (short int) (y))>>(n))
#define gap_mulsRN(x, y, n)		((((short int) (x) * (short int) (y)) + (1<<((n)-1)))>>(n))
#define gap_mulfsRN(x, y, n)		((((short int) (x) * (short int) (y)) + (1<<((n)-1)))>>(n))
#define gap_muluN(x, y, n)		(((unsigned short int) (x) * (unsigned short int) (y))>>(n))
#define gap_mulfuN(x, y, n)		(((unsigned short int) (x) * (unsigned short int) (y))>>(n))
#define gap_muluRN(x, y, n)		((((unsigned short int) (x) * (unsigned short int) (y)) + (1<<((n)-1)))>>(n))
#define gap_mulfuRN(x, y, n)		((((unsigned short int) (x) * (unsigned short int) (y)) + (1<<((n)-1)))>>(n))

/* Vectorial product and sum of products */
#define gap_dotp2(x, y)			(    (x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap_dotpu2(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap_dotpus2(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1])

#define gap_sumdotp2(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap_sumdotpu2(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap_sumdotpus2(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1])

#define gap_dotp4(x, y)			(    (x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap_dotpu4(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap_dotpus4(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])

#define gap_sumdotp4(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap_sumdotpu4(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap_sumdotpus4(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])


/* Complex Multiplication, Q15x15 into Q15, with optional post scaling by 1 or 2 */
#define gap_cplxmuls(x, y)		((v2s) {(signed short) ((((long long) (x)[0]*(long long) (y)[0]) - ((long long) (x)[1]*(long long) (y)[1]))>>15),  \
						(signed short) ((((long long) (x)[0]*(long long) (y)[1]) + ((long long) (x)[1]*(long long) (y)[0]))>>15)})
#define gap_cplxmulsdiv2(x, y)		(gap_cplxmuls(x, y)>>(v2s){1,1})
#define gap_cplxmulsdiv4(x, y)		(gap_cplxmuls(x, y)>>(v2s){2,2})

/* Complex conjugate */
#define gap_cplxconj(x)			((v2s) {(x)[0], -(x)[1]})

/* Complex rotation by -pi/2 */
#define gap_cplxmj(x)			((v2s) {(x)[1], -(x)[0]})

/* Complex substration, result rotated by -pi/2 */
#define gap_sub2rotmj(x, y)		((v2s) {(x)[1]-(y)[1], (y)[0]-(x)[0]})

/* Complex addition with post scaling by 1 or 2 */
#define gap_add2div2(x, y)		(((x)+(y))>>(v2s) {1, 1})
#define gap_add2div4(x, y)		(((x)+(y))>>(v2s) {2, 2})

/* Complex subtraction with post scaling by 1 or 2 */
#define gap_sub2div2(x, y)		(((x)-(y))>>(v2s) {1, 1})
#define gap_sub2div4(x, y)		(((x)-(y))>>(v2s) {2, 2})

/* Viterbi Max and Viterbi Select, pair of Q15 */
#define gap_vitmax(x, y) 		(_VitT1_Flag=((x)[1]<=(y)[1])?1:0, _VitT0_Flag=((x)[0]<=(y)[0])?1:0,\
				 	(v2s) {((x)[0]>(y)[0])?(x)[0]:(y)[0], ((x)[1]>(y)[1])?(x)[1]:(y)[1]})
#define gap_vitsel(x, y) 		(v2s) {(_VitT0_Flag?(y)[0]:(x)[0])<<1|_VitT0_Flag, (_VitT1_Flag?(y)[1]:(x)[1])<<1|_VitT1_Flag}

/* Position of the most significant bit of x */
#define gap_fl1(x)			(31 - __builtin_clz((x)))

/* Number of sign bits */
#define gap_clb(x)			(__builtin_clrsb((x)))

/* Bit Extraction */
#define gap_bitextract(x, size, off) 	(((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap_bitextractu(x, size, off)	(((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap_bitextract_r(x, size, off) 	(((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap_bitextractu_r(x, size, off)	(((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap_bitextract_r_safe(x, size, off) 	(((((x)>>((off)&0x1F))&((unsigned int)(1<<((((size)>32)?32:(size))))-1))<<(32-((((size)>32)?32:(size)))))>>(32-((((size)>32)?32:(size)))))
#define gap_bitextractu_r_safe(x, size, off)	(((x)>>((off)&0x1F))&((unsigned int)(1<<((((size)>32)?32:(size))))-1))

/* Bit insertion */
#define gap_bitinsert(dst, src, size, off) 	(((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define gap_bitinsert_r(dst, src, size, off) 	(((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define gap_bitinsert_r_safe(dst, src, size, off) 	(((dst) & ~(((1<<(((size)>32)?32:(size)))-1)<<((off)&0x1F))) | (((src) & ((1<<(((size)>32)?32:(size)))-1))<<((off)&0x1F)))

/* 1 bit rotation to the right, 32 bits input */
#define gap_rotr(x)				((((x)>>1)&0x7FFFFFFF) | ((x)<<31))

/* Add with normalization and rounding */
#define gap_addroundnormu(x, y, scale)		((unsigned int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap_addroundnormu_reg(x, y, scale)	((unsigned int)((x) + (y) + (scale?(1<<((scale)-1)):0))>>(scale))
#define gap_addroundnorm(x, y, scale)		((int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap_addroundnorm_reg(x, y, scale)	((int)((x) + (y) + (scale?(1<<((scale)-1)):0))>>(scale))

/* Normalization and rounding */
#define gap_roundnormu(x, scale)		((unsigned int)((x) + (1<<((scale)-1)))>>(scale))
#define gap_roundnormu_reg(x, scale)		((unsigned int)((x) + (scale?(1<<((scale)-1)):0))>>(scale))
#define gap_roundnorm(x, scale)			((int)((x) + (1<<((scale)-1)))>>(scale))
#define gap_roundnorm_reg(x, scale)		((int)((x) + (scale?(1<<((scale)-1)):0))>>(scale))

#endif

#define FIX2FP(Val, Precision)		((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)		((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)		((int)((Val)*((1 << (Precision))-1)))

#endif
