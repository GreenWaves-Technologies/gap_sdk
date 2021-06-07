#ifndef __CNN_DEFINED_H__
#define __CNN_DEFINED_H__

#ifdef __pulp__
#define Abs(a)          __builtin_pulp_abs((a))
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)          (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif

#define Minu(a, b)            (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

/* Select Normalization with floor or Normalization with rounding */
#define NORM_ROUND
#ifdef NORM_ROUND
#define AT_NORM(x, n)   gap_roundnorm_reg((x), (n))
#else
#define AT_NORM(x, n)   gap_norm_reg((x), (n))
#endif

#define AT_SCALE(X, Scale, ScaleN)      AT_NORM((X)*(Scale), (ScaleN))

#define AT_LSHIFT(x, n) ((x)<<(n))
#define AT_RSHIFT(x, n) ((x)>>(n))

/* In the following n is the bound and x the value to be clamped */
/* R = Max(0, Min(x, n) */
#define AT_CLIP_POS(x, n)       gap_clipur((x), (n))

/* R = Max(0, Min(x, 2^(n-1)-1 */
#define AT_CLIP_POS_IMM(x, n)   gap_clipu((x), (n))

/* R = Max(n, Min(x, -(n+1)) */
#define AT_CLIP_(x, n)          gap_clipr((x), (n))

#define AT_CLIP(x, n)		gap_clip((x), (n))
#endif
