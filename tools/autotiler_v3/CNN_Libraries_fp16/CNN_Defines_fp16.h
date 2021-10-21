/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __CNN_DEFINED_FP16_H__
#define __CNN_DEFINED_FP16_H__

#include "../DSP_Libraries/FloatDefines.h"

#ifdef __pulp__
	#define Abs(a)          __builtin_pulp_abs((a))
	#define Min(a, b)       __builtin_pulp_minsi((a), (b))
	#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#else
	#define Abs(a)          (((int)(a)<0)?(-(a)):(a))
	#define Min(a, b)       (((a)<(b))?(a):(b))
	#define Max(a, b)       (((a)>(b))?(a):(b))
#endif

#ifdef __gap9__
	#ifdef STD_FLOAT
		#define AbsF(a)		Absf16((a))
		#define MaxF(a, b)	Maxf16((a), (b))
		#define MinF(a, b)	Minf16((a), (b))

		#define AbsF2(a)	Absv2h((a))
		#define MaxF2(a, b)	Maxv2h((a), (b))
		#define MinF2(a, b)	Minv2h((a), (b))

		#define MIN_FLT16	MIN_f16
		#define MAX_FLT16	MAX_f16
		#define	LEAK_CONSTANT	((float16)0.1)
	#else
		#define AbsF(a)		Absf16a((a))
		#define MaxF(a, b)	Maxf16a((a), (b))
		#define MinF(a, b)	Minf16a((a), (b))

		#define AbsF2(a)	((F16V) Absv2ah((a)))
		#define MaxF2(a, b)	((F16V) Maxv2ah((a), (b)))
		#define MinF2(a, b)	((F16V) Minv2ah((a), (b)))

		#define MIN_FLT16	MIN_f16a
		#define MAX_FLT16	MAX_f16a
		#define	LEAK_CONSTANT	((float16alt)0.1)
	#endif // STD_FLOAT
#else
	#define	LEAK_CONSTANT	((float)0.1)
#endif

/* Float Activation Functions */
#ifdef FLOAT_LUT_ACTIVATIONS
	#include "../DSP_Libraries/FloatLutFuncs.h"
	#ifdef STD_FLOAT
		#define FastSigmoidF16(__x) 	sigmoid_lut_f16(__x)
		#define FastSigmoidF16V(__x) 	((v2h) {sigmoid_lut_f16((__x)[0]), sigmoid_lut_f16((__x)[1])})
		#define FastTanhF16(__x) 		tanh_lut_f16(__x)
		#define FastTanhF16V(__x) 		((v2h) {tanh_lut_f16((__x)[0]), tanh_lut_f16((__x)[1])})
		#define FastSwishF16(__x) 		swish_lut_f16(__x, (f16)1.0f)
		#define FastSwishF16V(__x)	 	((v2h) {swish_lut_f16((__x)[0], (f16)1.0f), swish_lut_f16((__x)[1], (f16)1.0f)})
		#define FastExpF16(__x) 		fastexp_f16(__x)
		#define FastExpF16V(__x) 		fastexp_v2h(__x)
	#else
		#define FastSigmoidF16(__x) 	sigmoid_lut_f16a(__x)
		#define FastSigmoidF16V(__x) 	((v2h) {sigmoid_lut_f16a((__x)[0]), sigmoid_lut_f16a((__x)[1])})
		#define FastTanhF16(__x) 		tanh_lut_f16a(__x)
		#define FastTanhF16V(__x) 		((v2h) {tanh_lut_f16a((__x)[0]), tanh_lut_f16a((__x)[1])})
		#define FastSwishF16(__x) 		swish_lut_f16a(__x, (f16)1.0f)
		#define FastSwishF16V(__x) 		((v2h) {swish_lut_f16a((__x)[0], (f16)1.0f), swish_lut_f16a((__x)[1], (f16)1.0f)})
		#define FastExpF16(__x) 		fastexp_f16a(__x)
		#define FastExpF16V(__x) 		fastexp_v2ah(__x)
	#endif // STD_FLOAT
	#define FastSigmoidF32(__x)			sigmoid_lut_f32(__x)
	#define FastTanhF32(__x)			tanh_lut_f32(__x)
	#define FastSwishF32(__x)			swish_lut_f32(__x)
	#define FastExpF32(__x)				fastexp(__x)
#else
	#include "../DSP_Libraries/FastFloatApprox16.h"
	#ifdef STD_FLOAT
		#define FastSigmoidF16(__x) 	fastsigmoid_f16(__x)
		#define FastSigmoidF16V(__x) 	fastsigmoid_v2h(__x)
		#define FastTanhF16(__x) 		fasttanh_f16(__x)
		#define FastTanhF16V(__x) 		fasttanh_v2h(__x)
		#define FastSwishF16(__x) 		fastswish_f16(__x, (f16)1.0f)
		#define FastSwishF16V(__x) 		fastswish_v2h(__x, (f16)1.0f)
		#define FastExpF16(__x) 		fastexp_f16(__x)
		#define FastExpF16V(__x) 		fastexp_v2h(__x)
	#else
		#define FastSigmoidF16(__x) 	fastsigmoid_f16a(__x)
		#define FastSigmoidF16V(__x) 	fastsigmoid_v2ah(__x)
		#define FastTanhF16(__x) 		fasttanh_f16a(__x)
		#define FastTanhF16V(__x) 		fasttanh_v2ah(__x)
		#define FastSwishF16(__x) 		fastswish_f16a(__x, (f16a)1.0f)
		#define FastSwishF16V(__x) 		fastswish_v2ah(__x, (f16a)1.0f)
		#define FastExpF16(__x) 		fastexp_f16a(__x)
		#define FastExpF16V(__x) 		fastexp_v2ah(__x)
	#endif // STD_FLOAT
	#define FastSigmoidF32(__x)			fastsigmoid(__x)
	#define FastTanhF32(__x)			fasttanh(__x)
	#define FastSwishF32(__x)			fastswish(__x)
	#define FastExpF32(__x)				fastexp(__x)
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
