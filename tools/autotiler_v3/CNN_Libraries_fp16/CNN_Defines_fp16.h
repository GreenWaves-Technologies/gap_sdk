#ifndef __CNN_DEFINED_FP16_H__
#define __CNN_DEFINED_FP16_H__

#ifdef __pulp__
#define Abs(a)          __builtin_pulp_abs((a))
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))

#define AbsF(a)		__builtin_pulp_f16altabs((a))
#define MaxF(a, b)	__builtin_pulp_f16altmax((a), (b))
#define MinF(a, b)	__builtin_pulp_f16altmin((a), (b))

#define AbsF2(a)	__builtin_pulp_f16altabs2((a))
#define MaxF2(a, b)	__builtin_pulp_f16altmax2((a), (b))
#define MinF2(a, b)	__builtin_pulp_f16altmin2((a), (b))

#define MIN_FLT16S	((float16)(6.10e-5f))
#define MAX_FLT16S	((float16)(65504))

#define MIN_FLT16A	((float16alt)(1.1754943508e-38f))
#define MAX_FLT16A	((float16alt)(3.4028234664e38f))

#define MIN_FLT16	MIN_FLT16A
#define MAX_FLT16	MAX_FLT16A

#define	LEAK_CONSTANT	((F16)0.1)

#else
#define Abs(a)          (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif

#define Minu(a, b)            (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

/* In the following n is the bound and x the value to be clamped */
/* R = Max(0, Min(x, n) */
#define AT_CLIP_POS(x, n)       gap_clipur((x), (n))

/* R = Max(0, Min(x, 2^(n-1)-1 */
#define AT_CLIP_POS_IMM(x, n)   gap_clipu((x), (n))

/* R = Max(n, Min(x, -(n+1)) */
#define AT_CLIP_(x, n)          gap_clipr((x), (n))


#endif
